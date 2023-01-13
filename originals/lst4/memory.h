/*
	memory management for the Little Smalltalk system
	Uses a variation on the Baker two-space algorithm

	Written by Tim Budd, Oregon State University, 1994
	Comments to budd@cs.orst.edu
	All rights reserved, no guarantees given whatsoever.
	May be freely redistributed if not for profit.
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

struct object {
	int size;
	struct object * class;
	struct object * data [0];
	};

/*
	byte objects are used to represent strings and symbols
		bytes per word must be correct
*/

struct byteObject {
	int size;
	struct object *class;
	unsigned char bytes[0];
	};

# define BytesPerWord 4
# define bytePtr(x) (((struct byteObject *) x)->bytes)

/*
	integer objects are used to represent short integers
*/

struct integerObject {
	int size;
	struct object * class;
	unsigned int value;
	};

# define integerValue(x) (((struct integerObject *) x)->value)

/*
	memoryBase holds the pointer to the current space,
	memoryPointer is the pointer into this space.
	To allocate, decrement memoryPointer by the correct amount.
	If the result is less than memoryBase, then garbage collection
	must take place

*/

extern struct object * memoryPointer;
extern struct object * memoryBase;

/*
	roots for the memory space
	these are traced down during memory management
	rootStack is the dynamic stack
	staticRoots are values in static memory that point to
	dynamic values
*/
# define ROOTSTACKLIMIT 50
extern struct object * rootStack[];
extern int rootTop;
# define STATICROOTLIMIT 200
extern struct object * * staticRoots[];
extern int staticRootTop;

/*
	The following are roots for the file out 
*/

extern struct object * nilObject;
extern struct object * smallInts[10];
extern struct object * trueObject;
extern struct object * falseObject;
extern struct object * SmallIntClass;
extern struct object * ArrayClass;
extern struct object * BlockClass;
extern struct object * ContextClass;
extern struct object * globalsObject;
extern struct object * initialMethod;
extern struct object * binaryMessages[3];

/*
	entry points
*/

void gcinit(int, int);
struct object * gcollect(int);
struct object * staticAllocate(int);
struct object * staticIAllocate(int);
struct object * gcialloc(int);

int isDynamicMemory(struct object *);

# define gcalloc(sz) (((memoryPointer-=(sz+1))<memoryBase)?\
	gcollect(sz):(memoryPointer->size=sz<<2,memoryPointer))
# ifndef gcalloc
extern struct object * gcalloc(int);
# endif
