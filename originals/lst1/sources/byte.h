/*
	Little Smalltalk
		Bytearray definitions
*/

struct byte_struct {
	int 	a_ref_count;
	int 	a_size;
	int	a_bsize;
	uchar	*a_bytes;
	} ;

typedef struct byte_struct bytearray;

# define byte_value(x) (((bytearray *)(x))->a_bytes)

/*
	bytearrays of size less than MAXBSAVE are kept on a free list
*/
# define MAXBSAVE 50

/*
	in order to avoid a large number of small mallocs, especially
	while reading the standard prelude, a fixed area of MAXBTABSIZE is
	allocated and used for bytecodes until it is full.  Thereafter
	bytecodes are allocated using malloc.  This area should be large
	enough to hold at least all the bytecodes for the standard prelude.
*/
# define MAXBTABSIZE 5500

/* 
	for the same reason, a number of bytearrays structs are statically
	allocated and placed on a free list
*/
# define MAXBYINIT 400

extern object *new_bytearray();
