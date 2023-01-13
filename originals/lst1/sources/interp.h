/*
        Little Smalltalk interpeter definitions
*/
/*
	for interpreters
		t_size = INTERPSIZE
	 	
		creator is a pointer to the interpreter which created
		the current interpreter.  it is zero except in the case 
		of blocks, in which case it points to the creating
		interpreter for a block.  it is NOT a reference, ie,
		the ref_count field of the creator is not incremented when
		this field is set - this avoids memory reference loops.

		stacktop is a pointer to a pointer to an object, however it
		is not considered a reference.   ie, changing stacktop does
		not alter reference counts.
*/

struct interp_struct {
        int	t_ref_count;
	int	t_size;	/* should always be INTERPSIZE */
	struct interp_struct *creator;
	struct interp_struct *sender;
	object 	*bytecodes;
	object	*receiver;
	object  *literals;
	object	*context;
	object  *stack;
	object	**stacktop;
	uchar   *currentbyte;
        };

typedef struct interp_struct interpreter;

extern interpreter *cr_interpreter();

extern object *o_drive;

# define is_driver(x) (o_drive == (object *) x)
