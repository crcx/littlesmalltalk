
/*
 * main.c
 *
 * Main module, some primitives and global functions
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

/* Notes on the indentation:
 * Use GNU indent to format the C source code of the LittleSmalltalk virtual machine.
 * Use these options for the formatter:
 * -bad -bap -bbb -sob -cdb -bli0 -ncdw -cbi0 -cli2 -ss -npcs -nbs
 * -nsaf -nsai -nsaw -nprs -di16 -bc -bls -npsl -lp -ts2 -ppi3 -i2 -nut -sbi0
 */

/*
  Little Smalltalk main program

  starting point, primitive handler for unix
  version of the little smalltalk system
*/
#include <string.h>
#include <stdlib.h>
#include <limits.h>


#include "globs.h"
#include "lst_primitives.h"

/*
	the following defaults must be set

*/

#ifndef DefaultImageFile
#define DefaultImageFile "LittleSmalltalk.image"
#endif

#define DefaultStaticSize 40000
#define DefaultDynamicSize 40000

#ifdef LST_ON_WINDOWS
#define DefaultTmpdir "C:\\Temp"
#else
#define DefaultTmpdir "/tmp"
#endif


/*
--------------------
*/

#include "memory.h"
#include "interp.h"
#include <stdio.h>

/* #define COUNTTEMPS */

unsigned int    debugging = 0,
  cacheHit = 0,
  cacheMiss = 0,
  gccount = 0;
char *lstTmpDir = DefaultTmpdir;

void sysError(char *a, unsigned int b)
{
  fprintf(stderr, "unrecoverable system error: %s 0x%x\n", a, b);
  exit(1);
}

static void backTrace(struct object *aContext)
{
  printf("back trace\n");
  while(aContext && (aContext != nilObject))
  {
    struct object  *arguments;
    LstUInt         i;

    printf("message %s ",
           bytePtr(aContext->data[methodInContext]->data[nameInMethod]));
    arguments = aContext->data[argumentsInContext];
    if(arguments && (arguments != nilObject))
    {
      printf("(");
      for(i = 0; i < SIZE(arguments); i++)
        printf("%s%s", (i == 0) ? "" : ", ", bytePtr(arguments->data[i]->class->data[nameInClass]));
      printf(")");
    }
    printf("\n");
    aContext = aContext->data[previousContextInContext];
  }
}

int main(int argc, char **argv)
{
  struct object  *aProcess,
                 *aContext,
                 *o;
  int             size,
                  i,
                  staticSize,
                  dynamicSize;
  FILE           *fp;
  char            imageFileName[120],
                 *p;

  strcpy(imageFileName, DefaultImageFile);
  staticSize = DefaultStaticSize;
  dynamicSize = DefaultDynamicSize;

  /*
   * See if our environment tells us what TMPDIR to use
   */
  p = getenv("TMPDIR");
  if(p)
    lstTmpDir = strdup(p);

  /*
     first parse arguments 
   */
  for(i = 1; i < argc; i++)
  {
    if(strcmp(argv[i], "-v") == 0)
    {
      printf("Little Smalltalk, version 5.0 (Alpha)\n");
    }
    else if(strcmp(argv[i], "-s") == 0)
    {
      staticSize = atoi(argv[++i]);
    }
    else if(strcmp(argv[i], "-d") == 0)
    {
      dynamicSize = atoi(argv[++i]);
    }
    else if(strcmp(argv[i], "-g") == 0)
    {
      debugging = 1;
    }
    else
    {
      strcpy(imageFileName, argv[i]);
    }
  }

  gcinit(staticSize, dynamicSize);

  /*
     read in the method from the image file 
   */
  fp = fopen(imageFileName, "rb");
  if(!fp)
  {
    fprintf(stderr, "cannot open image file: %s\n", imageFileName);
    exit(1);
  }

  printf("%d objects in image\n", fileIn(fp));
  fclose(fp);

  lstPrimitivesInit();

  /*
     build a context around it 
   */

  aProcess = staticAllocate(3);
  /*
     context should be dynamic 
   */
  aContext = gcalloc(contextSize);
  aContext->class = ContextClass;

  aProcess->data[contextInProcess] = aContext;
  size = integerValue(initialMethod->data[stackSizeInMethod]);
  aContext->data[stackInContext] = staticAllocate(size);
  aContext->data[argumentsInContext] = nilObject;

  aContext->data[temporariesInContext] = staticAllocate(19);
  aContext->data[bytePointerInContext] = newInteger(0);
  aContext->data[stackTopInContext] = newInteger(0);
  aContext->data[previousContextInContext] = nilObject;
  aContext->data[methodInContext] = initialMethod;

  /*
     now go do it 
   */
  rootStack[rootTop++] = aProcess;

  switch (execute(aProcess, 0))
  {
    case 2:
      printf("User defined return\n");
      break;

    case 3:
      printf("can't find method in call\n");
      aProcess = rootStack[--rootTop];
      o = aProcess->data[resultInProcess];
      printf("Unknown method: %s\n", bytePtr(o));
      aContext = aProcess->data[contextInProcess];
      backTrace(aContext);
      break;

    case 4:
      printf("\nnormal return\n");
      break;

    case 5:
      printf("time out\n");
      break;

    default:
      printf("unknown return code\n");
      break;
  }
  printf("cache hit %u miss %u", cacheHit, cacheMiss);
#define SCALE (1000)
  while((cacheHit > INT_MAX / SCALE) || (cacheMiss > INT_MAX / SCALE))
  {
    cacheHit /= 10;
    cacheMiss /= 10;
  }
  i = (SCALE * cacheHit) / (cacheHit + cacheMiss);
  printf(" ratio %u.%u%%\n", i / 10, i % 10);
  printf("%u garbage collections\n", gccount);

  lstPrimitivesRelease();

  return (0);
}


#ifdef LST_ON_WINDOWS

void memoryClear(void *address, LstUInt sizeInBytes)
{
  memset(address, 0, sizeInBytes);
}

void memoryCopy(void *sourceAddress, void *targetAddress, LstUInt sizeInBytes)
{
  memcpy(targetAddress, sourceAddress, sizeInBytes);
}

#endif
