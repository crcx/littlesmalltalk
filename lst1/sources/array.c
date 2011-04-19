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
 * array creation
 * builds a new instance of class array.
 * called mostly by the driver to construct array constants
 * timothy a. budd, 10/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

class *Array = (class *) 0;
class *ArrayedCollection = (class *) 0;

extern object *o_nil, *o_empty, *o_acollection;
extern int started;		/* gets set after reading std prelude */

/* new_iarray - internal form of new array */
object *new_iarray(size)
int size;
{	object *new;

	if (size < 0) cant_happen(2);
	new = new_obj(Array, size, 0);
	if (! started) {
		sassign(new->super_obj, o_acollection);
		}
	else if (ArrayedCollection)
		sassign(new->super_obj, new_inst(ArrayedCollection));
	return(new);
}

/* new_array - create a new array */
object *new_array(size, initial)
int size, initial;
{	int i;
	object *new;

	if (size == 0) return(o_empty);
	new = new_iarray(size);
	if (initial) {
		for (i = 0; i < size; i++)
			sassign(new->inst_var[ i ], o_nil);
		}
	return(new);
}
