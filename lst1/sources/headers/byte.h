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
