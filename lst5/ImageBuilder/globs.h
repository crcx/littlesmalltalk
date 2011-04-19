
/*
 * globs.h
 *
 * Global definitions - used by the image builder
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
 * globs.h
 *	Global defs for VM modules
 */
#ifndef GLOBS_H
#   define GLOBS_H
#   include <sys/types.h>
#   include <stdio.h>

#   if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MSVCRT__)

#       ifdef NETWORK_BYTE_ORDER

#define HTONS(A)  ((((A) & 0xff00) >> 8) | \
                   (((A) & 0x00ff) << 8))
#define HTONL(A)  ((((A) & 0xff000000) >> 24) | \
                   (((A) & 0x00ff0000) >> 8)  | \
                   (((A) & 0x0000ff00) << 8)  | \
                   (((A) & 0x000000ff) << 24))
#define NTOHS     HTONS
#define NTOHL     HTONL

#       else

#define NOP(A)  (A)
#define HTONS   NOP
#define HTONL   NOP
#define NTOHS   NOP
#define NTOHL   NOP

#       endif

#   else

#       ifdef NETWORK_BYTE_ORDER

#include <arpa/inet.h>
#define HTONS   htons
#define HTONL   htonl
#define NTOHS   ntohs
#define NTOHL   ntohl

#       else

#define NOP(A)  (A)
#define HTONS   NOP
#define HTONL   NOP
#define NTOHS   NOP
#define NTOHL   NOP

#       endif

#   endif

typedef unsigned int LstUInt;
typedef int     LstInt;

extern int      fileIn(FILE * fp),
                fileOut(FILE * fp);
extern void     sysError(char *, char *),
                flushCache(void);
extern struct object *primitive(int, struct object *, int *);

#endif /* GLOBS_H */
