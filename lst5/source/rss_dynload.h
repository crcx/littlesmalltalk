/*
 * rss_dynload.h
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

/*
 * Platform independent runtime access to shared
 * objects (DLLs)...
 */


#ifndef RSS_DYNLOAD_H
#define RSS_DYNLOAD_H

#ifdef __cplusplus
extern "C"
{
#endif


struct RSS_DL_Handle_Struct;
typedef struct RSS_DL_Handle_Struct *RSS_DL_Handle;

typedef int (*RSS_DL_Function)(int);


typedef
  enum
  {
    RSS_DL_LAZY,
    RSS_DL_NOW
  }
RSS_DL_Mode;


RSS_DL_Handle rss_dlOpen(const char *pathname, RSS_DL_Mode mode);
int rss_dlClose(RSS_DL_Handle handle);


void *rss_dlGetDataSymbol(RSS_DL_Handle handle, const char *symbol);
RSS_DL_Function rss_dlGetFunctionSymbol(RSS_DL_Handle handle, const char *symbol);

const char *rss_dlError(void);


#ifdef __cplusplus
}
#endif

#endif

