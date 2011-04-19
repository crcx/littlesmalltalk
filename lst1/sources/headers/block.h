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
	Little Smalltalk
		
		block definitions
		timothy a. budd, 10/84
*/
/*
	for blocks

		b_size = BLOCKSIZE

		b_interpreter is an instance of interpreter that will
		actually execute the bytecodes for the block.

		b_numargs and b_arglocation are the number of arguments and
		the starting argument location in the context array.

*/

struct block_struct {
	int	b_ref_count;
	int	b_size;
	interpreter	*b_interpreter;
	int	b_numargs;
	int	b_arglocation;
	} ;

typedef struct block_struct block;

extern object *new_block();
extern interpreter *block_execute();
