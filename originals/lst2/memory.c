/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	memory management module

	This is a rather simple, straightforward, reference counting scheme.
	There are no provisions for detecting cycles, nor any attempt made
	at compaction.  Free lists of various sizes are maintained.
	At present only objects up to 255 bytes can be allocated, 
	which mostly only limits the size of method (in text) you can create.

	About the only tricky feature to this code is the fact that
	reference counts are not stored as part of an object image, but
	are instead recreated when the object is read back in.
	(This will, in fact, eliminate cycles, as well as other unreachable
	objects).

	This can, and should, be replaced by a better memory management
	algorithm.
*/
# include <stdio.h>
# include "env.h"
# include "memory.h"

# define ObjectTableMax 5000
# define MemoryBlockSize 1000

boolean debugging = false;
object sysobj;	/* temporary used to avoid rereference in macros */
object intobj;

object symbols;		/* table of all symbols created */
object globalNames;	/* table of all accessible global names */

/*
	in theory the objectTable should only be accessible to the memory
	manager.  Indeed, given the right macro definitions, this can be
	made so.  Never the less, for efficiency sake some of the macros
	can also be defined to access the object table directly
*/

struct objectStruct objectTable[ObjectTableMax];

/*
	The following global variables are strictly local to the memory
	manager module
*/

static object objectFreeList[256];	/* free list of objects */
static short objectTop;			/* last object allocated */
static object *memoryBlock;		/* malloc'ed chunck of memory */
static int    currentMemoryPosition;	/* last used position in above */


/* initialize the memory management module */
initMemoryManager() {
	int i;

	/* set all the free list pointers to zero */
	for (i = 0; i < 256; i++)
		objectFreeList[i] = nilobj;

	/* set all the reference counts to zero */
	for (i = 0; i < ObjectTableMax; i++)
		objectTable[i].referenceCount = 0;

	objectTop = 0;

	/* force an allocation on first object assignment */
	currentMemoryPosition = MemoryBlockSize + 1;

	/* object at location 0 is the nil object, so give it nonzero ref */
	objectTable[0].referenceCount = 1;
	objectTable[0].size = 0;
	objectTable[0].type = objectMemory;
}

/* report a (generally fatal) memory manager error */
sysError(s1, s2)
char *s1, *s2;
{	int i;
	fprintf(stderr,"%s\n%s\n", s1, s2);
	i = 0;
	i = 32 / i;
}

/*
  mBlockAlloc - rip out a block (array) of object of the given size from
	the current malloc block 
*/
static object *mBlockAlloc(memorySize)
int memorySize;
{	object *objptr;

	if (currentMemoryPosition + memorySize >= MemoryBlockSize) {
		memoryBlock = (object *) calloc(MemoryBlockSize, sizeof(object));
		if (! memoryBlock)
			sysError("out of memory","malloc failed");
		currentMemoryPosition = 0;
		}
	objptr = (object *) &memoryBlock[currentMemoryPosition];
	currentMemoryPosition += memorySize;
	return(objptr);
}

/* allocate a new memory object */
object alcObject(memorySize, memoryType)
int memorySize;
int memoryType;
{	int position, trip;

	if (memorySize >= 256) {
		sysError("allocation bigger than 256","");
		}

	if (objectFreeList[memorySize] != 0) {
		objectFreeList[memorySize] = 
			objectTable[ position = objectFreeList[memorySize]].class;
		}
	else {		/* not found, must allocate a new object */
		position = trip = 0;
		do { 
			objectTop = objectTop + 1;
			if (objectTop >= ObjectTableMax)
				if (trip) {
					sysError("out of objects ","  ");
					position = 1;
					}
				else {
					trip = objectTop =1;
					}
			else if (objectTable[objectTop].referenceCount <= 0)
				position = objectTop;
		} while (position == 0);

		/* allocate memory pointer */
		objectTable[position].size = memorySize;
		objectTable[position].memory = mBlockAlloc(memorySize);

		}

	/* set class and type */
	objectTable[position].referenceCount = 0;
	objectTable[position].class = nilobj;
	objectTable[position].type = memoryType;
	return(position << 1);
}

object allocSymbol(str)
char *str;
{	object newSym;

	newSym = alcObject((2 + strlen(str))/2, charMemory);
	ignore strcpy(charPtr(newSym), str);
	return(newSym);
}

# ifdef incr
object incrobj;		/* buffer for increment macro */
# endif
# ifndef incr
void incr(z)
object z;
{
	if (z && ! isInteger(z)) {
		objectTable[z>>1].referenceCount++;
		globalinccount++;
		}
}
# endif

# ifndef decr
void decr(z)
object z;
{
	if (z && ! isInteger(z)) {
		if (--objectTable[z>>1].referenceCount <= 0) {
			sysDecr(z);
			}
		globaldeccount++;
		}
}
# endif

/* do the real work in the decr procedure */
sysDecr(z)
object z;
{	register struct objectStruct *p;
	register int i;

	p = &objectTable[z>>1];
	if (p->referenceCount < 0) {
		sysError("negative reference count","");
		}
	decr(p->class);
	p->class = objectFreeList[p->size];
	objectFreeList[p->size] = z>>1;
	if (((int) p->size) > 0) {
		if (p->type == objectMemory)
			for (i = p->size; i > 0 ; )
				decr(p->memory[--i]);
		for (i = p->size; i > 0; )
			p->memory[--i] = nilobj;
		}

}

# ifndef basicAt
object basicAt(z, i)
object z;
register int i;
{
	if (isInteger(z))
		sysError("attempt to index","into integer");
	else if ((i <= 0) || (i > objectSize(z))) {
		fprintf(stderr,"index %d size %d\n", i, (int) objectSize(z));
		sysError("index out of range","in basicAt");
		}
	else
		return(sysMemPtr(z)[i-1]);
	return(0);
}
# endif

void basicAtPut(z, i, v)
object z, v;
register int i;
{
	if (isInteger(z))
		sysError("assigning index to","integer value");
	else if ((i <= 0) || (i > objectSize(z))) {
		fprintf(stderr,"index %d size %d\n", i, (int) objectSize(z));
		sysError("index out of range","in basicAtPut");
		}
	else {
		incr(v);
		decr(sysMemPtr(z)[i-1]);
		sysMemPtr(z)[i-1] = v;
		}
}

/*
	imageWrite - write out an object image
*/
static iwerr() { sysError("imageWrite count error",""); }

imageWrite(fp)
FILE *fp;
{	short i;

	if (fwrite(&symbols, sizeof(object), 1, fp) != 1) iwerr();
	if (fwrite(&globalNames, sizeof(object), 1, fp) != 1) iwerr();

	for (i = 0; i < ObjectTableMax; i++) {
		if (objectTable[i].referenceCount > 0) {
			if (fwrite(&i, sizeof(short), 1, fp) != 1) iwerr();
			if (fwrite(&objectTable[i].class, sizeof(object), 1, fp)
				!= 1) iwerr();
			if (fwrite(&objectTable[i].size, sizeof(byte), 1, fp)
				!= 1) iwerr();
			if (fwrite(&objectTable[i].type, sizeof(byte), 1, fp)
				!= 1) iwerr();
			if (objectTable[i].size != 0)
				if (fwrite(objectTable[i].memory, sizeof(object),
					objectTable[i].size, fp) != objectTable[i].size)
						iwerr();
			}
		}
}

/*
	imageRead - read in an object image
*/
static irerr() { sysError("imageWrite count error",""); }

/*
	the following two routines, addmittedly a bit complicated,
assure that objects read in are really referenced, eliminating junk
that may be in the object file but not referenced */

static membump(i, j)
int i, j;
{	int k;
	object *p;

	k = objectTable[j].class;
	if (k) memincr(i, k>>1);
	if (objectTable[j].type == objectMemory) {
		p = objectTable[j].memory;
		for (k = byteToInt(objectTable[j].size) - 1; k >= 0; k--)
			if (p[k] && ! isInteger(p[k]))
				memincr(i, p[k]>>1);
		}
}

static memincr(i, j)
int i, j;
{
	objectTable[j].referenceCount++;
	if ((j <= i) && (objectTable[j].referenceCount == 1))
		membump(i, j);
}

imageRead(fp)
FILE *fp;
{	short i;
	object *p;

	if (fread( &symbols, sizeof(object), 1, fp) != 1) irerr();
	if (fread( &globalNames, sizeof(object), 1, fp) != 1) irerr();
	objectTable[symbols>>1].referenceCount++;
	objectTable[globalNames>>1].referenceCount++;

	while(fread( &i, sizeof(short), 1, fp) == 1) {
		if (fread( &objectTable[i].class, sizeof(object), 1, fp)
				!= 1) irerr();

		if (fread( &objectTable[i].size, sizeof(byte), 1, fp)
				!= 1) irerr();
		if (fread( &objectTable[i].type, sizeof(byte), 1, fp)
				!= 1) irerr();
		if (objectTable[i].size != 0) {
			p = objectTable[i].memory = mBlockAlloc((int) objectTable[i].size);
			if (fread( p, sizeof(object),
				 byteToInt(objectTable[i].size), fp) != byteToInt(objectTable[i].size))
						irerr();
			if (objectTable[i].referenceCount > 0)
				membump(i, i);
			}
		else
			objectTable[i].memory = (object *) 0;
		}
}

static ncopy(p, q, n)
char *p, *q;
int n;
{

	while (n>0) {
		*p++ = *q++; 
		n--;
		}
}

object allocFloat(d)
double d;
{	object newObj;

	newObj = alcObject((int) sizeof (double), floatMemory);
	ncopy(charPtr(newObj), (char *) &d, (int) sizeof (double));
	return(newObj);
}

double floatValue(obj)
object obj;
{	double d;

	ncopy((char *) &d, charPtr(obj), (int) sizeof (double));
	return(d);
}

int objcount() 
{	int i, count;

	
	for (count = i = 0; i < ObjectTableMax; i++)
		if (objectTable[i].referenceCount > 0)
			count++;
	return(count);
}
