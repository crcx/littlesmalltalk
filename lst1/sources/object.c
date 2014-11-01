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
 * object memory management
 * timothy a. budd, 10/84
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "lst.h"

#define DEBUG 0

extern object *o_acollection;

int n_incs = 0;		/* number of increments counter */
int n_decs = 0;		/* number of decrements counter (should be equal)*/
int n_mallocs = 0;	/* number of mallocs counter */

/* o_alloc - allocate a block of memory, checking for end of memory */
char *o_alloc(n)
unsigned n;
{  char *p;

   p = malloc(n);
   if (p == (char *) 0) cant_happen(1);	/* out of memory */
   n_mallocs++;
   return(p);
}

#ifndef INLINE

/* obj_inc - increment an object (usually expanded in-line) */
obj_inc(x) 
register object *x;
{
	x->ref_count++;
	n_incs++;
}

/* obj_dec - decrement an object (usually half expanded in-line) */
void obj_dec(x) 
object *x;
{
	n_decs++;
	if (--(x->ref_count) > 0) return;
# endif
# ifdef INLINE
void ob_dec(x)
object *x;
{
# endif
	if (x->ref_count < 0) {
		fprintf(stderr,"ref count %d %p\n", x->ref_count, x);
		primitive(REFCOUNTERROR, 1, &x);
		return;
		}
	if (is_bltin(x)) {	/* free a built-in object */
		switch(x->size) {
			case BLOCKSIZE:
				free_block(x); break;
			case BYTEARRAYSIZE:
				free_bytearray((bytearray *) x); break;
			case CLASSSIZE : 
				free_class((class *) x); break;
			case FILESIZE:
				free_file((struct file_struct *) x);
				break;
			case FLOATSIZE:
				free_float((sfloat *) x); break;
			case INTEGERSIZE: case CHARSIZE:
				free_integer((integer *) x); break;
			case INTERPSIZE:
				free_terpreter((interpreter *) x); break;
			case PROCSIZE:
				free_process((process *) x); break;
			case SYMBOLSIZE: 
				cant_happen(16);
			case STRINGSIZE:
				free_string((string *) x); break;
			default: cant_happen(6);
			}
		}
	else {		/* free a normal (non-special) object */
		if (x->super_obj)
			obj_dec(x->super_obj);
		free_obj(x, 1);
		}
}

# define MAXOBJLIST 100
# define sizeobj(x) (sizeof(object) + ((x) - 1) * sizeof(object *) )

/* obj_free_list is a free list for memory blocks */

static object *obj_free_list[MAXOBJLIST]; /* better be initialized to zero! */

int ca_obj = 0;			/* count the number of allocations made */
int ca_cobj[5] = {0,0,0,0,0};	/* count how many allocations for small vals*/

/* make sure the following list is null terminated! */
int size_obj_init[] = {15, 75, 420, 10, 10, 5, 0};

/* init_objs - initialize the memory management module */
init_objs() {
	int i, j, max, size;
	char *p;
	object *new;

	for (i = 0; (max = size_obj_init[i]); i++) {
		size = sizeobj(i);
		p = o_alloc((unsigned int) (max * size));
		for (j = 0; j < max; j++) {
			new = (object *) p;
			new->super_obj = obj_free_list[i];
			obj_free_list[i] = new;
			p += size;
			}
		}
}

/* new_obj - create a new non-special object */
object *new_obj(nclass, nsize, alloc)
class *nclass;
int nsize, alloc;
{	register object *new;
	int i;
	
	if (nsize < 0)
		cant_happen(2);
	if (nsize < MAXOBJLIST && obj_free_list[nsize])
		obj_free_list[nsize] = (new = obj_free_list[nsize])->super_obj;
	else {
		new = (object *) o_alloc(sizeobj(nsize));
		ca_obj++;
		if (nsize < 5)
			ca_cobj[nsize]++;
	}
	new->super_obj = (object *) 0;
	new->class = nclass;
	if (nclass)
		obj_inc((object *) new->class );
	new->ref_count = 0;
	new->size = nsize;
	if (alloc)
		for (i = 0; i < nsize; i++) {
			sassign(new->inst_var[ i ], o_nil);
		 }	
	return(new);
}
	
/* free_obj - free a non-special object */
free_obj(obj, dofree)
register object *obj;
int    dofree;
{	int size, i;

	size = obj->size;
	if (dofree)
		for (i = 0; i < size; i++)
			obj_dec(obj->inst_var[i]);
	if (obj->class)
		obj_dec((object *) obj->class);
	if (size < MAXOBJLIST) {
		obj->super_obj = obj_free_list[size];
		obj_free_list[size] = obj;
		}
	else {
		free(obj);
      		}
}

/* fnd_class - find the class of a special object */
object *fnd_class(object* anObject)
{	object *result, *lookup_class();
	char *name;

	if (is_bltin(anObject)) {
		switch(anObject->size) {
			case BLOCKSIZE:   name = "Block"; break; 
			case CLASSSIZE:   name = "Class"; break;
			case FILESIZE:    name = "File"; break;
			case FLOATSIZE:   name = "Float"; break;
			case INTEGERSIZE: name = "Integer"; break;
			case CHARSIZE:    name = "Char"; break;
			case INTERPSIZE:  name = "Interp"; break;
			case PROCSIZE:    name = "Process"; break;
			case SYMBOLSIZE:  name = "Symbol"; break;
			case STRINGSIZE:  name = "String"; break;
			case BYTEARRAYSIZE: name = "ByteArray"; break;
			default: cant_happen(6);
			}
		result = lookup_class(name);
		}
	else
		result = (object *) anObject->class;
	return(result);
}

extern object *o_object, *o_magnitude, *o_number;

/* fnd_super - produce a super-object for a special object */
object *fnd_super(object *anObject)
{	object *result;

	if (is_bltin(anObject)) {
		switch(anObject->size) {
			case BLOCKSIZE:   result = o_object; break;
			case CLASSSIZE:   result = o_object; break;
			case FILESIZE:    result = o_object; break;
			case FLOATSIZE:   result = o_number; break;
			case INTEGERSIZE: result = o_number; break;
			case CHARSIZE:    result = o_magnitude; break;
			case INTERPSIZE:  result = o_object; break;
			case PROCSIZE:    result = o_object; break;
			case SYMBOLSIZE:  result = o_object; break;
			case STRINGSIZE:   /* strings DO have superobjs*/
				result = ((string *) anObject)->s_super_obj; 
				break;
			case BYTEARRAYSIZE: result = o_acollection; break;
			default: cant_happen(6);
			}
		}
	else
		result = anObject->super_obj;
	return(result);
}
