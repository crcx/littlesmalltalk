/*
 Little Smalltalk, version 2
 Written by Tim Budd, Oregon State University, July 1987
 */

/*
 The first major decision to be made in the memory manager is what
 an entity of type object really is.  Two obvious choices are a pointer (to
 the actual object memory) or an index into an object table.  We decided to
 use the latter, although either would work.
 Similarly, one can either define the token object using a typedef,
 or using a define statement.  Either one will work (check this?)
 */

#ifndef _MEMORY_H
#define _MEMORY_H

typedef short object;

/*
 The memory module itself is defined by over a dozen routines.
 All of these could be defined by procedures, and indeed this was originally
 done.  However, for efficiency reasons, many of these procedures can be
 replaced by macros generating in-line code.  For the latter approach
 to work, the structure of the object table must be known.  For this reason,
 it is given here.  Note, however, that outside of the files memory.c and
 unixio.c (or macio.c on the macintosh) ONLY the macros described in this
 file make use of this structure: therefore modifications or even complete
 replacement is possible as long as the interface remains consistent
 */

struct objectStruct
{
    object STclass;
    short referenceCount;
    short size;
    object *memory;
};
# define ObjectTableMax 6500

# ifdef obtalloc
extern struct objectStruct *objectTable;
# endif
# ifndef obtalloc
extern struct objectStruct objectTable[];
# endif

void incr(object z);
void decr(object z);

/*
 The next most basic routines in the memory module are those that
 allocate blocks of storage.  There are three routines:
 allocObject(size) - allocate an array of objects
 allocByte(size) - allocate an array of bytes
 allocStr(str) - allocate a string and fill it in
 again, these may be macros, or they may be actual procedure calls
 */

object allocObject(int memorySize);
object allocByte(int size);
object allocStr(register const char *str);

/*
 integer objects are (but need not be) treated specially.
 In this memory manager, negative integers are just left as is, but
 positive integers are changed to x*2+1.  Either a negative or an odd
 number is therefore an integer, while a nonzero even number is an
 object pointer (multiplied by two).  Zero is reserved for the object ``nil''
 Since newInteger does not fill in the class field, it can be given here.
 If it was required to use the class field, it would have to be deferred
 until names.h
 */

extern object intobj;
boolean isInteger(object intobj);
object newInteger(int intobj);
int intValue(object intobj);

/*
 there are four routines used to access fields within an object.
 Again, some of these could be replaced by macros, for efficiency
 basicAt(x, i) - ith field (start at 1) of object x
 basicAtPut(x, i, v) - put value v in object x
 byteAt(x, i) - ith field (start at 0) of object x
 byteAtPut(x, i, v) - put value v in object x*/

# define basicAt(x,i) (sysMemPtr(x)[i-1])
# define byteAt(x, i) ((int) ((bytePtr(x)[i-1])))

# ifndef basicAt
extern object basicAt(object, int);
# endif

# define simpleAtPut(x,i,y) (sysMemPtr(x)[i-1] = y)
# ifndef simpleAtPut
extern void simpleAtPut(object, int, object);
# endif

# define basicAtPut(x,i,y) incr(simpleAtPut(x, i, y))
# ifndef basicAtPut
extern void basicAtPut(object, int, object);
# endif
# define fieldAtPut(x,i,y) f_i=i; decr(basicAt(x,f_i)); basicAtPut(x,f_i,y)
# ifdef fieldAtPut
extern int f_i;
#endif
# ifndef fieldAtPut
extern void fieldAtPut(object, int, object);
# endif

# ifndef byteAt
extern int byteAt(object, int);
# endif
# ifndef byteAtPut
extern void byteAtPut(object z, int i, int x);
# endif

/*
 Finally, a few routines (or macros) are used to access or set
 class fields and size fields of objects
 */

# define classField(x) objectTable[x>>1].STclass
# define setClass(x,y) incr(classField(x)=y)
# define sizeField(x) objectTable[x>>1].size

# define sysMemPtr(x) objectTable[x>>1].memory
extern object sysobj;

//I'm not sure it was a good idea to replace (object*)0 to sysMemPtr(sysobj)
# define memoryPtr(x) (isInteger(sysobj = x)?sysMemPtr(sysobj):sysMemPtr(sysobj))
# define bytePtr(x) ((byte *) memoryPtr(x))
# define charPtr(x) ((char *) memoryPtr(x))

# define nilobj (object) 0

/*
 There is a large amount of differences in the qualities of malloc
 procedures in the Unix world.  Some perform very badly when asked
 to allocate thousands of very small memory blocks, while others
 take this without any difficulty.  The routine mBlockAlloc is used
 to allocate a small bit of memory; the version given below
 allocates a large block and then chops it up as needed; if desired,
 for versions of malloc that can handle small blocks with ease
 this can be replaced using the following macro:

 # define mBlockAlloc(size) (object *) calloc((unsigned) size, sizeof(object))

 This can, and should, be replaced by a better memory management
 algorithm.
 */
# ifndef mBlockAlloc
object *mBlockAlloc(int);
# endif

/*
 the dictionary symbols is the source of all symbols in the system
 */
extern object symbols;

/*
 finally some external declarations with prototypes
 */

void initMemoryManager( void );
void imageWrite( FILE*);
void imageRead( FILE*);
extern boolean debugging;

void setFreeLists();
void visit(register object x) ;

void sysDecr(object z);
#endif