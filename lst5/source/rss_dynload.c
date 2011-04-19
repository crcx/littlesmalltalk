/*
 * rss_dynload.c
 *
 * Encapsulated access to shared objects (DLLs)
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


#include "rss_dynload.h"


#ifdef WIN32

/* Windows implementation */

#include <Windows.h>

struct RSS_DL_Handle_Struct
{
  HMODULE handle;
};


RSS_DL_Handle rss_dlOpen(const char *pathname, RSS_DL_Mode mode)
{
  RSS_DL_Handle h = (RSS_DL_Handle)malloc(sizeof(struct RSS_DL_Handle_Struct));
  if(!h)
    return 0;

  mode = mode; /* Avoid a compiler warning because of an unused formal parameter */

  h->handle = LoadLibrary(pathname);
  if(!h->handle)
  {
    free(h);
    h = 0;
  }

  return h;
}


int rss_dlClose(RSS_DL_Handle handle)
{
  BOOL success = 0;

  if(handle)
  {
    if(handle->handle)
      success = FreeLibrary(handle->handle);

    free(handle);
  }

  return success ? 0 : -1;
}


void *rss_dlGetDataSymbol(RSS_DL_Handle handle, const char *symbol)
{
  handle = handle; /* Avoid a compiler warning because of an unused formal parameter */
  symbol = symbol; /* Avoid a compiler warning because of an unused formal parameter */
  return 0; // Not implemented on Windows

/*
  if(!handle || !handle->handle)
    return 0;

  return (void *)GetProcAddress(handle->handle, symbol);
*/
}


RSS_DL_Function rss_dlGetFunctionSymbol(RSS_DL_Handle handle, const char *symbol)
{
  if(!handle || !handle->handle)
    return 0;

  return (RSS_DL_Function)GetProcAddress(handle->handle, symbol);
}


const char *rss_dlError()
{
  return "error in dynload function...";
}


#else

#ifdef HP

/* special implementation for HP-UX (based on shl_...()) */

#include <dl.h>
#include <errno.h>
#include <string.h>

static char errorMsg[4096] = "0: no error";

struct RSS_DL_Handle_Struct
{
  shl_t handle;
};


RSS_DL_Handle rss_dlOpen(const char *pathname, RSS_DL_Mode mode)
{
  int flags;
  RSS_DL_Handle h = (RSS_DL_Handle)malloc(sizeof(struct RSS_DL_Handle_Struct));

  if(!h)
    return 0;

  flags = mode == RSS_DL_NOW ? BIND_IMMEDIATE : BIND_DEFERRED;
  flags |= BIND_NONFATAL;
  h->handle = shl_load(pathname, flags, 0L);
  if(!h->handle)
  {
    sprintf(errorMsg, "%d: %s", errno, strerror(errno));
    free(h);
    return 0;
  }

  sprintf(errorMsg, "0: no error");
  return h;
}


int rss_dlClose(RSS_DL_Handle handle)
{
  int result = -1;

  if(!handle || !handle->handle)
    return -1;

  result = shl_unload(handle->handle);

  free(handle);

  sprintf(errorMsg, "0: no error");
  return result;
}


void *rss_dlGetDataSymbol(RSS_DL_Handle handle, const char *symbol)
{
  int success;
  void *addr;
  shl_t h;

  if(!handle || !handle->handle)
    return 0;

  h = handle->handle;
  success = shl_findsym(&h, symbol, TYPE_DATA, &addr);

  if(success != 0)
  {
    sprintf(errorMsg, "%d: %s", errno, strerror(errno));
    return 0;
  }

  sprintf(errorMsg, "0: no error");
  return addr;
}


RSS_DL_Function rss_dlGetFunctionSymbol(RSS_DL_Handle handle, const char *symbol)
{
  int success;
  void *addr;
  shl_t h;

  if(!handle || !handle->handle)
    return 0;

  h = handle->handle;
  success = shl_findsym(&h, symbol, TYPE_PROCEDURE, &addr);

  if(success != 0)
  {
    sprintf(errorMsg, "%d: %s", errno, strerror(errno));
    return 0;
  }

  sprintf(errorMsg, "0: no error");
  return (RSS_DL_Function)addr;
}


const char *rss_dlError()
{
  return errorMsg;
}


#else


/* general UNIX implementation (based on dlopen(), dlclose(), dlget() and dlerror()) */

#include <stdlib.h>
#include <dlfcn.h>

struct RSS_DL_Handle_Struct
{
  void *handle;
};



RSS_DL_Handle rss_dlOpen(const char *pathname, RSS_DL_Mode mode)
{
  int dlMode;
  RSS_DL_Handle h = (RSS_DL_Handle)malloc(sizeof(struct RSS_DL_Handle_Struct));
  if(!h)
    return 0;

  dlMode = mode == RSS_DL_LAZY ? RTLD_LAZY : RTLD_NOW;
  /*h->handle = dlopen(pathname, dlMode | RTLD_LOCAL);*/
  h->handle = dlopen(pathname, dlMode | RTLD_GLOBAL);

  if(!h->handle)
  {
    free(h);
    h = 0;
  }

  return h;
}


int rss_dlClose(RSS_DL_Handle handle)
{
  int result = -1;

  if(!handle)
    return -1;

  if(handle->handle)
    result = dlclose(handle->handle);

  free(handle);
  return result;
}


void *rss_dlGetDataSymbol(RSS_DL_Handle handle, const char *symbol)
{
  if(!handle || !handle->handle)
    return 0;

  return dlsym(handle->handle, symbol);
}


RSS_DL_Function rss_dlGetFunctionSymbol(RSS_DL_Handle handle, const char *symbol)
{
  if(!handle || !handle->handle)
    return 0;

  /* This may cause a warning on some platforms because
   * a conversion of void * to a function pointer is
   * not allowed. This is due to a design problem in
   * the dl...() functions and not avoidable.
   * Maybe there will be a distinction between dlsym()
   * for data and for functions as in this abstraction
   * library in the future...
   */
  return (RSS_DL_Function)dlsym(handle->handle, symbol);
}


const char *rss_dlError(void)
{
  return dlerror();
}


#endif
#endif

