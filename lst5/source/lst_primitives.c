/*
 * lst_primitives.h
 *
 * Primitives of the LittleSmalltalk system
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


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "globs.h"

#ifdef LST_ON_WINDOWS
#include "Windows.h" /* for Sleep() */
#define defaultEditor "Notepad.exe "
#else
#include <unistd.h>
#define Sleep(X) usleep(1000L * (X))
#define defaultEditor "vi "
#endif

#include "lst_primitives.h"
#include "memory.h"


/*
	primitive handler
	(note that many primitives are handled in the interpreter)
*/

#define LST_FILEMAX 20
static FILE    *filePointers[LST_FILEMAX];

void lstPrimitivesInit()
{
#if defined(LST_USE_FFI) && 1 == LST_USE_FFI
    lstFfiInit();
#endif
#if defined(LST_USE_SOCKET) && 1 == LST_USE_SOCKET
    lstSocketInit();
#endif
}

void lstPrimitivesRelease()
{
#if defined(LST_USE_FFI) && 1 == LST_USE_FFI
    lstFfiRelease();
#endif
#if defined(LST_USE_SOCKET) && 1 == LST_USE_SOCKET
    lstSocketRelease();
#endif
}

struct object  *primitive(int primitiveNumber, struct object *args,
                          int *failed)
{
  struct object  *returnedValue = nilObject;
  int             i,
                  j;
  FILE           *fp;
  unsigned char  *p;
  struct byteObject *stringReturn;
  char            nameBuffer[80],
                  modeBuffer[80];

  *failed = 0;
  switch (primitiveNumber)
  {
    case 100:                  /* open a file */
      lstGetString(nameBuffer, 80, args->data[0]);
      lstGetString(modeBuffer, 10, args->data[1]);
      fp = fopen(nameBuffer, modeBuffer);
      if(fp != NULL)
      {
        for(i = 0; i < LST_FILEMAX; ++i)
        {
          if(filePointers[i] == NULL)
          {
            break;
          }
        }
        if(i >= LST_FILEMAX)
        {
          sysError("too many open files", 0);
          fclose(fp);
          *failed = 1;
        }
        else
        {
          returnedValue = newInteger(i);
          filePointers[i] = fp;
        }
      }
      else
      {
        *failed = 1;
      }
      break;

    case 101:                  /* read a single character from a file */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }
      i = fgetc(fp);
      if(i != EOF)
      {
        returnedValue = newInteger(i);
      }
      break;

    case 102:                  /* write a single character to a file */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }
      fputc(integerValue(args->data[1]), fp);
      break;

    case 103:                  /* close file */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }
      fclose(fp);
      filePointers[i] = NULL;
      break;

    case 104:                  /* file out image */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }
      fileOut(fp);
      break;

    case 105:                  /* edit a string */
      {
        struct stat buf;
        /*
        first get the name of a temp file 
        */
        sprintf(nameBuffer, "%s/lsteditXXXXXX", lstTmpDir);
        mkstemp(nameBuffer);
        /*
        copy string to file.
        write the string in text mode - so even notepad can visualize it without problems.
        */
        fp = fopen(nameBuffer, "w");
        if(fp == NULL)
          sysError("cannot open temp edit file", 0);
        j = SIZE(args->data[0]);
        p = ((struct byteObject *) args->data[0])->bytes;
        for(i = 0; i < j; i++)
          fputc(*p++, fp);
        fputc('\n', fp);
        fclose(fp);
        /*
        edit string 
        */
        strcpy(modeBuffer, defaultEditor);
        strcat(modeBuffer, nameBuffer);
        system(modeBuffer);
        /*
        copy back to new string.
        read the file in binary mode - otherwise we would try to read too many bytes on Windows...
        */
        /*
          get length of file 
        */
        stat(nameBuffer, &buf);
        j = buf.st_size;
        stringReturn = (struct byteObject *)gcialloc(j);
        returnedValue = (struct object *) stringReturn;
        returnedValue->class = args->data[0]->class;

        fp = fopen(nameBuffer, "rb");
        if(fp == NULL)
          sysError("cannot open temp edit file", 0);
        /*
        reset to beginning, and read values 
        */
        for(i = 0; i < j; i++)
          stringReturn->bytes[i] = (unsigned char)fgetc(fp);
        /*
        now clean up files 
        */
        fclose(fp);
        unlink(nameBuffer);
      }
      break;

    case 106:                  /* Read into ByteArray */
      /*
         File descriptor 
       */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }

      /*
         Make sure we're populating an array of bytes 
       */
      returnedValue = args->data[1];
      if((returnedValue->size & FLAG_BIN) == 0)
      {
        *failed = 1;
        break;
      }

      /*
         Sanity check on I/O count 
       */
      i = integerValue(args->data[2]);
      if((LstUInt)i > SIZE(returnedValue))
      {
        *failed = 1;
        break;
      }

      /*
         Do the I/O 
       */
      i = fread(bytePtr(returnedValue), sizeof(char), i, fp);
      if(i < 0)
      {
        *failed = 1;
        break;
      }
      returnedValue = newInteger(i);
      break;

    case 107:                  /* Write from ByteArray */
      /*
         File descriptor 
       */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }

      /*
         Make sure we're writing an array of bytes 
       */
      returnedValue = args->data[1];
      if((returnedValue->size & FLAG_BIN) == 0)
      {
        *failed = 1;
        break;
      }

      /*
         Sanity check on I/O count 
       */
      i = integerValue(args->data[2]);
      if((LstUInt)i > SIZE(returnedValue))
      {
        *failed = 1;
        break;
      }

      /*
         Do the I/O 
       */
      i = fwrite(bytePtr(returnedValue), sizeof(char), i, fp);
      if(i < 0)
      {
        *failed = 1;
        break;
      }
      returnedValue = newInteger(i);
      break;

    case 108:                  /* Seek to file position */
      /*
         File descriptor 
       */
      i = integerValue(args->data[0]);
      if((i < 0) || (i >= LST_FILEMAX) || !(fp = filePointers[i]))
      {
        *failed = 1;
        break;
      }

      /*
         File position 
       */
      i = integerValue(args->data[1]);
      if((i < 0) || ((i = fseek(fp, i, SEEK_SET)) < 0))
      {
        *failed = 1;
        break;
      }

      /*
         Return position as our value 
       */
      returnedValue = newInteger(i);
      break;

    case 109:  /* Return the name of the current operating system */
        returnedValue = lstNewString(lstOsName);
      break;

    case 110: /* Sleep */
      Sleep(integerValue(args->data[0]));
      break;

#if defined(LST_USE_FFI)
      /* FFI (Foreign Function Interface) primitives */
    case 230:
    case 231:
    case 232:
    case 233:
    case 234:
    case 235:
    case 236:
    case 237:
    case 238:
    case 239:
    case 240:   /* getInt */
    case 241:   /* setInt */
        returnedValue = lstFfiPrimitive(primitiveNumber, args);
        break;
#endif

#if defined(LST_USE_SOCKET)
    case 250: /* Socket */
        returnedValue = lstSocketPrimitive(primitiveNumber, args, failed);
        break;
    case 251: /* Inet */
        returnedValue = lstInetPrimitive(primitiveNumber, args, failed);
        break;
#endif

    default:
	    sysError("unknown primitive", (void *)(INT_PTR)primitiveNumber);
  }
  return (returnedValue);
}

