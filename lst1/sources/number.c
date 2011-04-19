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
 * number definitions
 * timothy a. budd, 10/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

#define MAXLOW 100	/* maximum low numbers kept */

static integer *low_nums[MAXLOW];  /* better be initialized to zero ! */

static mstruct *fr_integer = 0;
static mstruct *fr_float = 0;

static integer init_itable[INTINITMAX];

int_init() {
	integer *p;
	mstruct *new;
	int i;

	for (p = init_itable, i = 0; i < INTINITMAX; i++, p++) {
		new = (mstruct *) p;
		new->mlink = fr_integer;
		/*fprintf(stderr,"init int %d %d\n", new, new->mlink);*/
		fr_integer = new;
		}
}

int ca_int = 0;	/* count the number of integer allocations */

extern object *o_magnitude;
extern object *o_number;

/* new_cori - new character or integer */
object *new_cori(val, type)
int val, type;
{	register integer *new;

	if ((type == 1) && (val >= 0 && val < MAXLOW) && low_nums[val])
		return( (struct obj_struct *) low_nums[val]);

	if (fr_integer) {
		new = (integer *) fr_integer;
		/*fprintf(stderr,"int off list %d %d\n", fr_integer,
		fr_integer->mlink);*/
		fr_integer = fr_integer->mlink;
		}
	else {
		new = structalloc(integer);
		/*fprintf(stderr,"allocating new int %d\n", new);*/
		ca_int++;
		}

	new->i_ref_count = 0;
	new->i_value = val;
	switch(type) {
		case 0: /* chars */
			new->i_size = CHARSIZE;
		  	break;

		case 1: /* integers */
			new->i_size = INTEGERSIZE;
			if (val >= 0 && val < MAXLOW)
				sassign(low_nums[val], new);
		  	break;

		default: cant_happen(5);
		}
	return ((object *) new);
}

free_integer(i)
integer *i;
{
	if ((! is_integer(i)) && (! is_character(i)))
		cant_happen(8);
	((mstruct *) i)->mlink = fr_integer;
	fr_integer = (mstruct *) i;
	/*fprintf(stderr,"freeing integer %d %d\n", fr_integer,
	fr_integer->mlink);*/
}

free_low_nums()
{	int i;

	for (i = 0; i < MAXLOW; i++)
		if (low_nums[i]) {
			obj_dec((object *) low_nums[i]);
			low_nums[i] = (integer *) 0;
			}
}

int ca_float = 0;

/* new_float - produce a new floating point number */
object *new_float(val)
double val;
{	register sfloat *new;

	if (fr_float) {
		new = (sfloat *) fr_float;
		fr_float = fr_float->mlink;
		}
	else {
		new = structalloc(sfloat);
		ca_float++;
		}

	new->f_ref_count = 0;
	new->f_size = FLOATSIZE;
	new->f_value = val;
	return( (object *) new);
}

free_float(f)
sfloat *f;
{
	if (! is_float(f))
		cant_happen(8);
	((mstruct *) f)->mlink = fr_float;
	fr_float = (mstruct *) f;
}
