/*
 * lst_ffi_primitives.c
 *
 * The foreign function interface primitives of the LittleSmalltalk system
 *
 * ---------------------------------------------------------------
 * Little Smalltalk, Version 5
 * 
 * Copyright (C) 1987-2005 by Timothy A. Budd
 * Copyright (C) 2007 by Charles R. Childers
 * Copyright (C) 2005-2007 by Danny Reinhold
 * 
 * ============================================================================
 * This license applies to the virtual machine and to the initial image of 
 * the Little Smalltalk system and to all files in the Little Smalltalk 
 * packages.
 * ============================================================================
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */


#include "memory.h"

#if !defined(LST_USE_FFI) || LST_USE_FFI == 0

/* The FFI primitives are disabled.
 * Provide some dummy stuff instead...
 */

void lstFfiInit() { }
void lstFfiRelease() { }
struct object *lstFfiPrimitive(int primitiveNumber, struct object *args)
{
  return nilObject;
}


#else


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lst_ffi_primitives.h"
#include "rss_dynload.h"


#if defined(LST_USE_GUI) && LST_USE_GUI == 1
#include "iup.h"
#include "iupcontrols.h"
#include "iupim.h"
#include "rss_queue.h"
static RSS_Queue lstEventQueue;
static int lstIupCallback(Ihandle *iHandle, ...);
static int lstIupListCallback(Ihandle *iHandle, char *text, int index, int selectionFlag);
#endif


#define LS_MAX_OPEN_LIBS 20
#define LS_MAX_FUNCTIONS 200

#define isValidSO(soIndex)               soIndex >= 0 && soIndex < LS_MAX_OPEN_LIBS && libs[soIndex].handle
#define isValidFunc(soIndex, funcIndex)  isValidSO(soIndex) && funcIndex >= 0 && funcIndex < LS_MAX_FUNCTIONS && libs[soIndex].functions[funcIndex]

static int getNextLibIndex();
static int getNextFunctionIndex(int handle);


struct LST_DL_Struct
{
  RSS_DL_Handle   handle;
  RSS_DL_Function functions[LS_MAX_FUNCTIONS];
};
typedef struct LST_DL_Struct LST_DL_Handle;


static LST_DL_Handle libs[LS_MAX_OPEN_LIBS];

#define LstF0 int(*)(void)
#define LstF1 int(*)(int)
#define LstF2 int(*)(int,int)
#define LstF3 int(*)(int,int,int)
#define LstF4 int(*)(int,int,int,int)
#define LstF5 int(*)(int,int,int,int,int)
#define LstF6 int(*)(int,int,int,int,int,int)
#define LstF7 int(*)(int,int,int,int,int,int,int)


static int lstMakeArg(struct object *obj, int i)
{
  static char buf[10][1024];

  if(IS_SMALLINT(obj))
      return integerValue(obj);

  if(obj->class == StringClass)
  {
      lstGetString(buf[i], 1024, obj);
      return (int)buf[i];
  }

  return integerValue(obj);
}


void lstFfiInit()
{
  int i;
  for(i = 0; i < LS_MAX_OPEN_LIBS; i++)
  {
    int j;
    libs[i].handle = 0;
    for(j = 0; j < LS_MAX_FUNCTIONS; j++)
      libs[i].functions[j] = 0;
  }

#if defined(LST_USE_GUI) && LST_USE_GUI == 1
  lstEventQueue = rss_queueCreate(31);
#endif
}


void lstFfiRelease()
{
  int i;

#if defined(LST_USE_GUI) && LST_USE_GUI == 1
  rss_queueRelease(lstEventQueue);
  lstEventQueue = 0;
#endif

  for(i = 0; i < LS_MAX_OPEN_LIBS; i++)
  {
    if(libs[i].handle)
      rss_dlClose(libs[i].handle);
  }
}


struct object *lstFfiPrimitive(int primitiveNumber, struct object *args)
{
  struct object   *result;
  struct object   *obj;
  const char      *str;
#define LST_BUF_SIZE 1024
  char             buf[LST_BUF_SIZE];
  int              i;
  int              j;
  int              soIndex;
  int              funcIndex;
  RSS_DL_Function  func;

  result = nilObject;

  switch(primitiveNumber)
  {
    case 230:
      /* open or create a shared object (dll) */
      /* Return a handle (in fact an index to an array containing RSS_DL_Handle objects) for the lib */
      /* args->data[0]: (path)name of the library file */
      i = getNextLibIndex();
      if(i >= 0)
      {
        lstGetString(buf, LST_BUF_SIZE, args->data[0]);
        libs[i].handle = rss_dlOpen(buf, RSS_DL_LAZY);
        if(libs[i].handle)
          result = newInteger(i);
      }
      break;

    case 231:
      /* Close the specified database */
      /* args->data[0]: The handle of the database */
      soIndex = integerValue(args->data[0]);
      if(isValidSO(soIndex))
      {
        rss_dlClose(libs[soIndex].handle);
        libs[soIndex].handle = 0;
        for(funcIndex = 0; funcIndex < LS_MAX_FUNCTIONS; funcIndex++)
          libs[soIndex].functions[funcIndex] = 0;
      }
      break;

    case 232:
      /* Return a meaningful error message if something went wrong... */
      /* Returns the message of the last error that occured. */
      str = rss_dlError();
      if(str)
        result = lstNewString(str);
      break;

    case 233:
      /* Tries to resolve a function symbol.*/
      /* args->data[0]: The handle of the shared object */
      /* args->data[1]: The name of the function */
      /* returns a function handle that can be used to call the function. */
      soIndex = integerValue(args->data[0]);
      if(isValidSO(soIndex))
      {
        lstGetString(buf, LST_BUF_SIZE, args->data[1]);
        func = rss_dlGetFunctionSymbol(libs[soIndex].handle, buf);
        if(func)
        {
          funcIndex = getNextFunctionIndex(soIndex);
          libs[soIndex].functions[funcIndex] = func;
          result = newInteger(funcIndex);
        }
      }
      break;

    case 234:
      /* Execute the specified void returning function...                                   */
      /* args->data[0]: The handle to the shared object that contains the function          */
      /* args->data[1]: The handle of the function within the shared object                 */
      /* args->data[2]: The argument array                                                  */
      soIndex   = integerValue(args->data[0]);
      funcIndex = integerValue(args->data[1]);

      if(isValidFunc(soIndex, funcIndex))
      {
        RSS_DL_Function func = libs[soIndex].functions[funcIndex];
        obj = args->data[2];
        i = SIZE(obj);
        switch(i)
        {
        case 0:
            ((void(*)())func)();
            break;

        case 1:
            ((void(*)(int))func)((int)lstMakeArg(obj->data[0], 0));
            break;

        case 2:
            ((void(*)(int,int))func)((int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1));
            break;

        case 3:
            ((void(*)(int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2));
            break;

        case 4:
            ((void(*)(int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3));
            break;

        case 5:
            ((void(*)(int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4));
            break;

        case 6:
            ((void(*)(int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5));
            break;

        case 7:
            ((void(*)(int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6));
            break;

        case 8:
            ((void(*)(int,int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6), (int)lstMakeArg(obj->data[7], 7));
            break;
        }

        result = nilObject;
      }
      break;
    
    case 235:
      /* Execute the specified int returning function...                                    */
      /* args->data[0]: The handle to the shared object that contains the function          */
      /* args->data[1]: The handle of the function within the shared object                 */
      /* args->data[2]: The argument array                                                  */
      soIndex   = integerValue(args->data[0]);
      funcIndex = integerValue(args->data[1]);

      if(isValidFunc(soIndex, funcIndex))
      {
        RSS_DL_Function func = libs[soIndex].functions[funcIndex];
        obj = args->data[2];
        i = SIZE(obj);
        switch(i)
        {
        case 0:
            j = ((int(*)())func)();
            break;

        case 1:
            j = ((int(*)(int))func)((int)lstMakeArg(obj->data[0], 0));
            break;

        case 2:
            j = ((int(*)(int,int))func)((int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1));
            break;

        case 3:
            j = ((int(*)(int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2));
            break;

        case 4:
            j = ((int(*)(int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3));
            break;

        case 5:
            j = ((int(*)(int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4));
            break;

        case 6:
            j = ((int(*)(int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5));
            break;

        case 7:
            j = ((int(*)(int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6));
            break;

        case 8:
            j = ((int(*)(int,int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6), (int)lstMakeArg(obj->data[7], 7));
            break;
        }

        result = newInteger(j);
      }
      break;

    case 236:
      /* Execute the specified char * returning function...                                 */
      /* args->data[0]: The handle to the shared object that contains the function          */
      /* args->data[1]: The handle of the function within the shared object                 */
      /* args->data[2]: The argument array                                                  */
      soIndex   = integerValue(args->data[0]);
      funcIndex = integerValue(args->data[1]);

      if(isValidFunc(soIndex, funcIndex))
      {
        RSS_DL_Function func = libs[soIndex].functions[funcIndex];
        obj = args->data[2];
        i = SIZE(obj);
        switch(i)
        {
        case 0:
            str = ((char*(*)())func)();
            break;

        case 1:
            str = ((char*(*)(int))func)((int)lstMakeArg(obj->data[0], 0));
            break;

        case 2:
            str = ((char*(*)(int,int))func)((int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1));
            break;

        case 3:
            str = ((char*(*)(int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2));
            break;

        case 4:
            str = ((char*(*)(int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3));
            break;

        case 5:
            str = ((char*(*)(int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4));
            break;

        case 6:
            str = ((char*(*)(int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5));
            break;

        case 7:
            str = ((char*(*)(int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6));
            break;

        case 8:
            str = ((char*(*)(int,int,int,int,int,int,int,int))func)(
                (int)lstMakeArg(obj->data[0], 0), (int)lstMakeArg(obj->data[1], 1), (int)lstMakeArg(obj->data[2], 2),
                (int)lstMakeArg(obj->data[3], 3), (int)lstMakeArg(obj->data[4], 4), (int)lstMakeArg(obj->data[5], 5),
                (int)lstMakeArg(obj->data[6], 6), (int)lstMakeArg(obj->data[7], 7));
            break;
        }

        result = lstNewString(str);
      }
      break;

#if defined(LST_USE_GUI) && LST_USE_GUI == 1
    case 237:
      /* register an event */
      /* arguments->data[0]: the action to register */
      lstGetString(buf, LST_BUF_SIZE, args->data[0]);
      IupSetFunction(buf, lstIupCallback);
      break;

    case 238:
      /* register a list event */
      /* arguments->data[0]: the action to register */
      lstGetString(buf, LST_BUF_SIZE, args->data[0]);
      IupSetFunction(buf, (Icallback)lstIupListCallback);
      break;

    case 239:
      /* get next event (non blocking event polling) */
      i = rss_queueSize(lstEventQueue);
      if(i <= 0)
        IupLoopStep();
      if(rss_queueSize(lstEventQueue) > 0)
      {
        char *str = (char *)rss_queueGet(lstEventQueue);
        if(str)
        {
          result = (struct object *)lstNewString(str);
          free(str);
        }
      }
      break;
#endif

    case 240:
      /* getInt                                                                             */
      /* args->data[0]: The handle to the shared object that contains the function          */
      /* args->data[1]: The handle of the symbol within the shared object                   */
      soIndex   = integerValue(args->data[0]);
      funcIndex = integerValue(args->data[1]);

      if (isValidFunc(soIndex, funcIndex)) {
        int *intvar = (int *) libs[soIndex].functions[funcIndex];
        result = newInteger(*intvar);
      }
      break;
    
    case 241:
      /* setInt: i                                                                          */
      /* args->data[0]: The handle to the shared object that contains the function          */
      /* args->data[1]: The handle of the symbol within the shared object                   */
      /* args->data[2]: i                                                                   */
      soIndex   = integerValue(args->data[0]);
      funcIndex = integerValue(args->data[1]);

      if (isValidFunc(soIndex, funcIndex)) {
        int *intvar = (int *) libs[soIndex].functions[funcIndex];
        *intvar = integerValue(args->data[2]);
        result = nilObject;
      }
      break;
    
    default:
      sysError("Primitive not done! - Unknown FFI primitive number: ", primitiveNumber);
  }

  return result;
}


static int getNextLibIndex()
{
  int i;
  for(i = 0; i < LS_MAX_OPEN_LIBS; i++)
  {
    if(!libs[i].handle)
      return i;
  }

  return -1;
}


static int getNextFunctionIndex(int handle)
{
  int i;
  if(!libs[handle].handle)
    return -1;

  for(i = 0; i < LS_MAX_FUNCTIONS; i++)
  {
    if(libs[handle].functions[i] == 0)
      return i;
  }

  return -1;
}


#if defined(LST_USE_GUI) && LST_USE_GUI == 1


static int lstIupCallback(Ihandle *iHandle, ...)
{
  rss_queuePut(lstEventQueue, (void *)(strdup(IupGetActionName())));
  return IUP_DEFAULT;
}


static int lstIupListCallback(Ihandle *iHandle, char *text, int index, int selectionFlag)
{
  static char buf[128];
  rss_queuePut(lstEventQueue, (void *)strdup("__list_event"));
  rss_queuePut(lstEventQueue, (void *)strdup(IupGetActionName()));
  rss_queuePut(lstEventQueue, (void *)strdup(text));
  sprintf(buf, "%d", index);
  rss_queuePut(lstEventQueue, (void *)strdup(buf));
  sprintf(buf, "%d", selectionFlag);
  rss_queuePut(lstEventQueue, (void *)strdup(buf));
  return IUP_DEFAULT;
}


#endif



#endif

