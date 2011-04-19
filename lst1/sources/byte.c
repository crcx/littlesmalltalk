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
 *************************************************************************
 * bytearray manipulation
 * bytearrays are used almost entirely for storing bytecode
 * timothy a. budd, 11/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

/* 
	bytearrays of less than MAXBSAVE are kept on a free list
*/

static mstruct *fr_bytearray[MAXBSAVE]; 	/* better be initialized to zero ! */

/*
	in order to avoid a large number of small mallocs, a table is used
	for the first new bytearrays.  After the table becomes full,
	malloc is used to get more space.
	table should be large enough for the standard prelude, at least 
*/

static uchar btable[MAXBTABSIZE];
int btabletop = 0;

/*
	for the same reason, a number of bytearray bases are statically
	allocated and kept on a free list 
*/

int ca_barray = 0;
static mstruct *fr_bybase = 0;

static bytearray by_init[MAXBYINIT];

byte_init()
{	int i;
	bytearray *p;
	mstruct *new;

	p = by_init;
	for (i = 0; i < MAXBYINIT; i++) {
		new = (mstruct *) p;
		new->mlink = fr_bybase;
		fr_bybase = new;
		p++;
		}
}

object *new_bytearray(values, size)
uchar *values;
int size;
{	bytearray *new;
	uchar *p, *q;

	if (size < MAXBSAVE && fr_bytearray[size]) {
		new = (bytearray *) fr_bytearray[size];
		fr_bytearray[size] = fr_bytearray[size]->mlink;
		}
	else {
		if (fr_bybase) {
			new = (bytearray *) fr_bybase;
			fr_bybase = fr_bybase->mlink;
			}
		else {
			new = structalloc(bytearray);
			ca_barray++;
			}
		if ((btabletop + size) < MAXBTABSIZE) {
			new->a_bytes = &btable[btabletop];
			btabletop += size;
			}
		else {
			new->a_bytes = (uchar *) o_alloc((unsigned) size);
			}
		}
	new->a_ref_count = 0;
	new->a_size = BYTEARRAYSIZE;
	new->a_bsize = size;
	for (p = new->a_bytes, q = values; size; size--) {
		*p++ = *q++;
		}
	return((object *) new);
}

free_bytearray(obj)
bytearray *obj;
{	int size;

	if (! is_bytearray(obj))
		cant_happen(8);
	size = obj->a_bsize;
	if (size < MAXBSAVE) {
		((mstruct *) obj)->mlink = fr_bytearray[size];
		fr_bytearray[size] = ((mstruct *) obj);
		}
}
