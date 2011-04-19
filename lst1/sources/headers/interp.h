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
