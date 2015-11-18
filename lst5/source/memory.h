
/*
 * memory.h
 *
 * The memory management and garbage collection module
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
    memory management for the Little Smalltalk system
    Uses a variation on the Baker two-space algorithm
  */

/*
	The fundamental data type is the object.
	The first field in an object is a size, the low order two
		bits being used to maintain:
			* binary flag, used if data is binary
			* indirection flag, used if object has been relocated
	The next data field is the class
	The following fields are either objects, or character values

	The initial image is loaded into static memory space --
	space which is never garbage collected
	This improves speed, as these items are not moved during GC
*/

#ifndef LST_MEMORY_H
#define LST_MEMORY_H
#include <sys/types.h>
#include "globs.h"


/* an int that is the same size as a pointer */
#ifndef INT_PTR
#ifdef _M_X64
typedef long long INT_PTR;
#else
typedef long INT_PTR;
#endif
#endif

struct object
{
  LstUInt         size;
  struct object  *class;
  struct object  *data[0];
};

/*
	byte objects are used to represent strings and symbols
		bytes per word must be correct
*/

struct byteObject
{
  LstUInt         size;
  struct object  *class;
  unsigned char   bytes[0];
};

#define BytesPerWord ((int)sizeof(void *))
#define bytePtr(x) (((struct byteObject *) x)->bytes)
#define WORDSUP(ptr, amt) ((struct object *)(((char *)(ptr)) + \
	((amt) * BytesPerWord)))
#define WORDSDOWN(ptr, amt) WORDSUP(ptr, 0 - (amt))

/*
 * SmallInt objects are used to represent short integers.  They are
 * encoded as 31 bits, signed, with the low bit set to 1.  This
 * distinguishes them from all other objects, which are longword
 * aligned and are proper C memory pointers.
 */
#include <limits.h>

#define IS_SMALLINT(x) ((((INT_PTR)(x)) & 0x01) != 0)
#define FITS_SMALLINT(x) ((((INT_PTR)(x)) >= INT_MIN/2) && \
	(((int)(x)) <= INT_MAX/2))
#define CLASS(x) (IS_SMALLINT(x) ? SmallIntClass : ((x)->class))
#define integerValue(x) (((INT_PTR)(x)) >> 1)
#define newInteger(x) ((struct object *)((((INT_PTR)(x)) << 1) | 0x01))

/*
 * The "size" field is the top 30 bits; the bottom two are flags
 */
#define SIZE(op) ((op)->size >> 2)
#define SETSIZE(op, val) ((op)->size = ((val) << 2))
#define FLAG_GCDONE (0x01)
#define FLAG_BIN (0x02)

/*
	memoryBase holds the pointer to the current space,
	memoryPointer is the pointer into this space.
	To allocate, decrement memoryPointer by the correct amount.
	If the result is less than memoryBase, then garbage collection
	must take place

*/

extern struct object *memoryPointer,
               *memoryBase;

/*
	roots for the memory space
	these are traced down during memory management
	rootStack is the dynamic stack
	staticRoots are values in static memory that point to
	dynamic values
*/
#define ROOTSTACKLIMIT 50
extern struct object *rootStack[];
extern LstUInt rootTop;
extern void     addStaticRoot(struct object **);

/*
	The following are roots for the file out 
*/

extern struct object *nilObject,
               *trueObject,
               *falseObject,
               *SmallIntClass,
               *ArrayClass,
               *BlockClass,
               *ContextClass,
               *StringClass,
               *globalsObject,
               *initialMethod,
               *binaryMessages[3],
               *IntegerClass,
               *badMethodSym;

/*
	entry points
*/

extern void     gcinit(int, int);
extern struct object *gcollect(int),
               *staticAllocate(int),
               *staticIAllocate(int),
               *gcialloc(int);
extern void     exchangeObjects(struct object *, struct object *, LstUInt);

extern int      isDynamicMemory(struct object *);

#define gcalloc(sz) (((memoryPointer = WORDSDOWN(memoryPointer, (sz) + 2)) < \
	memoryBase) ? gcollect(sz) : \
	(SETSIZE(memoryPointer, (sz)), memoryPointer))

#ifndef gcalloc
extern struct object *gcalloc(int);
#endif


struct object *lstNewString(const char *str);
void lstGetString(char *to, int size, struct object *from);
struct object *lstNewBinary(void *p, LstUInt l);


#endif

