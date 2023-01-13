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
