/*************************************************************************
 * Little Smalltalk, Version 1
 *
 * The source code for the Little Smalltalk system may be freely copied
 * provided that the source of all files is acknowledged and that this
 * condition is copied with each file.
 *
 * The Little Smalltalk system is distributed without responsibility for
 * the performance of the program and without any guarantee of maintenance.
 *
 * All questions concerning Little Smalltalk should be addressed to:
 *
 *         Professor Tim Budd
 *         Department of Computer Science
 *         Oregon State University
 *         Corvallis, Oregon
 *         97331
 *         USA
 *
 * This copy of Little Smalltalk has been updated by Charles R. Childers,
 * http://charleschilders.com
 *************************************************************************/
/*
        Little Smalltalk object definitions
*/
/*
	for objects the inst_var array is actually made as large as
	necessary (as large as the size field).  since C does not do
	subscript bounds checking array indexing can be used
*/

struct obj_struct {
        int                   ref_count;
	int                   size;
        struct class_struct   *class;
        struct obj_struct     *super_obj;
        struct obj_struct     *inst_var[1];
        };

/*
	for classes
		c_size = CLASSSIZE

		class_name and super_class should be SYMBOLs
		containing the names of the class and superclass,
		respectively.

		c_inst_vars should be an array of symbols, containing the
		names of the instance variables

		context size is the size of the context that should be
		created each time a message is sent to objects of this
		class.

		message_names should be an array of symbols, corresponding
		to the messages accepted by objects of this class.

		methods should be an array of arrays, each element being a
		two element array of bytecodes and literals.
*/

struct class_struct {
	int			c_ref_count;
	int			c_size;
	struct obj_struct	*class_name;
	struct obj_struct	*super_class;
	struct obj_struct	*file_name;
	struct obj_struct	*c_inst_vars;
	int			context_size;
	struct obj_struct	*message_names;
	struct obj_struct	*methods;
	int			stack_max;
	};

typedef struct class_struct class;
typedef struct obj_struct object;

/*
	objects with non-object value (classes, integers, etc) have a
	negative size field, the particular value being used to indicate
	the type of object (the class field cannot be used for this purpose
	since all classes, even those for built in objects, can be redefined)

	check_bltin is a macro that tests the size field for a particular
	value.  it is used to define other macros, such as is_class, that
	test each particular type of object.

	The following classes are builtin

		Block
		ByteArray
		Char 
		Class
		Float
		Integer
		Interpreter
		String
		Symbol
*/

# define BLOCKSIZE 	-83
# define BYTEARRAYSIZE 	-567
# define CHARSIZE 	-33
# define CLASSSIZE 	-3
# define FILESIZE 	-5
# define FLOATSIZE 	-31415
# define INTEGERSIZE 	-17
# define INTERPSIZE 	-15
# define PROCSIZE  	-100
# define STRINGSIZE 	-258
# define SYMBOLSIZE 	-14

# define is_bltin(x) (x && (((object *) x)->size < 0))
# define check_bltin(obj, type) (obj && (((object *) obj)->size == type))

# define is_block(x)		check_bltin(x, BLOCKSIZE)
# define is_bytearray(x)	check_bltin(x, BYTEARRAYSIZE)
# define is_character(x)	check_bltin(x, CHARSIZE)
# define is_class(x)		check_bltin(x, CLASSSIZE)
# define is_file(x)		check_bltin(x, FILESIZE)
# define is_float(x)		check_bltin(x, FLOATSIZE)
# define is_integer(x)		check_bltin(x, INTEGERSIZE)
# define is_interpreter(x)	check_bltin(x, INTERPSIZE)
# define is_process(p)	 	check_bltin(p, PROCSIZE)
# define is_string(x)		check_bltin(x, STRINGSIZE)
# define is_symbol(x)		check_bltin(x, SYMBOLSIZE)

/*
	mstruct is used (via casts) to store linked lists of structures of
	various types for memory saving and recovering
*/

struct mem_struct {
	struct mem_struct *mlink;
	};

typedef struct mem_struct mstruct;

/*
	sassign assigns val to obj, which should not have a valid
	value in it already.
	assign decrements an existing val field first, then assigns.
	note this will not work for assign(x,x) if x ref count is 1.
	safeassign, although producing less efficient code, will work even
	in this case
*/
# define sassign(obj, val) obj_inc((object *) (obj = val))
# define assign(obj, val)  {obj_dec((object *) obj); sassign(obj, val);}
# define safeassign(obj, val) {obj_inc((object *) val); \
	obj_dec((object *) obj); obj = val; }

/* structalloc calls alloc to allocate a block of memory 
   for a structure and casts the returned
   pointer to the appropriate type */
# define structalloc(type) (type *) o_alloc(sizeof(type))

/*
	if INLINE is defined ( see env.h ) , inline code will be generated 
	for object increments.  inline code is generally faster, but
	larger than using subroutine calls for incs and decs
*/

extern int  n_incs, n_decs;

# ifdef INLINE

# define obj_inc(x) n_incs++, (x)->ref_count++
extern object *_dx;
# define obj_dec(x) {n_decs++; if (--((_dx=x)->ref_count) <= 0) ob_dec(_dx);}

# endif

extern char   *o_alloc();	/* allocate a block of memory */
extern object *new_inst();	/* make a new instance of a class */
extern object *new_sinst();	/* an internal (system) version of new_inst*/
extern object *new_obj();	/* allocate a new object */
extern object *new_array();	/* make a new array */
extern object *primitive();	/* perform a primitive operation */
 
extern object *o_nil;		/* current value of pseudo variable nil */
extern object *o_true;		/* current value of pseudo variable true */
extern object *o_false;		/* current value of pseudo variable false */
extern object *o_smalltalk;	/* current value of pseudo var smalltalk */

extern int debug;		/* debugging toggle */

/* reference count macro, used during debugging */
# define rc(x) ((object *)x)->ref_count
