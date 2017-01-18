/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/

# define streq(a,b) (strcmp(a,b) == 0)

/*
	The first major decision to be made in the memory manager is what
an entity of type object really is.  Two obvious choices are a pointer (to 
the actual object memory) or an index into an object table.  We decided to
use the latter, although either would work.
	Similarly, one can either define the token object using a typedef,
or using a define statement.  Either one will work (check this?)
*/

typedef short object;

/*
	The memory module itself is defined by over a dozen routines.
All of these could be defined by procedures, and indeed this was originally
done.  However, for efficiency reasons, many of these procedures can be
replaced by macros generating in-line code.  For the latter approach
to work, the structure of the object table must be known.  For this reason,
it is given here.  Note, however, that ONLY the macros described in this
file make use of this structure: therefore modifications or even complete
replacement is possible as long as the interface remains consistent

*/

struct objectStruct {
	object class;
	short referenceCount;
	byte size;
	byte type;
	object *memory;
	};

extern struct objectStruct objectTable[];
extern void sysDecr();

/* types of object memory */
# define objectMemory 0
# define byteMemory 1
# define charMemory 2
# define floatMemory 3

# define isString(x) ((objectTable[x>>1].type == charMemory) || (objectTable[x>>1].type == byteMemory))
# define isFloat(x)  (objectTable[x>>1].type == floatMemory)

/*
	The most basic routines to the memory manager are incr and decr,
which increment and decrement reference counts in objects.  By separating
decrement from memory freeing, we could replace these as procedure calls
by using the following macros:*/
extern object incrobj;
# define incr(x) if ((incrobj=(x))&&!isInteger(incrobj)) \
objectTable[incrobj>>1].referenceCount++
#  define decr(x) if (((incrobj=x)&&!isInteger(incrobj))&&\
(--objectTable[incrobj>>1].referenceCount<=0)) sysDecr(incrobj);
/*
notice that the argument x is first assigned to a global variable; this is
in case evaluation of x results in side effects (such as assignment) which
should not be repeated. */

# ifndef incr
extern void incr();
# endif
# ifndef decr
extern void decr();
# endif

/*
	The next most basic routines in the memory module are those that
allocate blocks of storage.  There are three routines:
	allocObject(size) - allocate an array of objects
	allocByte(size) - allocate an array of bytes
	allocChar(size) - allocate an array of character values
	allocSymbol(value) - allocate a string value
	allocInt(value) - allocate an integer value
	allocFloat(value) - allocate a floating point object
again, these may be macros, or they may be actual procedure calls
*/

extern object alcObject();	/* the actual routine */
# define allocObject(size) alcObject(size, objectMemory)
# define allocByte(size) alcObject(size, byteMemory)
# define allocChar(size) alcObject(size, charMemory)
extern object allocSymbol();
# define allocInt(value) ((value<0)?value:(value<<1)+1)
extern object allocFloat();

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

# define isInteger(x) ((x) & 0x8001)

# define newInteger(x) ( (intobj = x)<0 ? intobj : (intobj<<1)+1 )
# define intValue(x) ( (intobj = x)<0 ? intobj : (intobj>>1) )

/*
	in addition to alloc floating point routine given above,
another routine must be provided to go the other way.  Note that
the routine newFloat, which fills in the class field as well, must
wait until the global name table is known, in names.h
*/
extern double floatValue();

/*
	there are four routines used to access fields within an object.
Again, some of these could be replaced by macros, for efficiency
	basicAt(x, i) - ith field (start at 1) of object x
	basicAtPut(x, i, v) - put value v in object x
	byteAt(x, i) - ith field (start at 0) of object x
	byteAtPut(x, i, v) - put value v in object x
*/

# define basicAt(x,i) (sysMemPtr(x)[i-1])

# ifndef basicAt
extern object basicAt();
# endif
# ifndef basicAtPut
extern void basicAtPut();
# endif
# ifndef byteAt
extern int byteAt();
# endif
# ifndef byteAtPut
extern void byteAtPut();
# endif

/*
	Finally, a few routines (or macros) are used to access or set
class fields and size fields of objects
*/

# define classField(x) objectTable[x>>1].class
# define setClass(x,y) incr(classField(x)=y)

# define objectSize(x) byteToInt(objectTable[x>>1].size)

# define sysMemPtr(x) objectTable[x>>1].memory
extern object sysobj;
# define memoryPtr(x) (isInteger(sysobj = x)?(object *) 0:sysMemPtr(sysobj))
# define bytePtr(x) ((byte *) memoryPtr(x))
# define charPtr(x) ((char *) memoryPtr(x))

# define nilobj (object) 0

/*
	these two objects are the source of all objects in the system
*/
extern object symbols;
extern object globalNames;
