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
 * class instance creation and deletion
 * timothy a. budd  10/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

extern class *Array, *ArrayedCollection;

extern object *o_object, *o_empty, *o_number, *o_magnitude;
extern object *o_smalltalk, *o_acollection;

static mstruct *fr_class = 0;
int ca_class = 0;	/* count class allocations */

# define CLASSINITMAX 30

static class cl_table[CLASSINITMAX];

class_init()
{	class *p;
	mstruct *new;
	int i;

	for (p = cl_table, i = 0; i < CLASSINITMAX; i++, p++) {
		new = (mstruct *) p;
		new->mlink = fr_class;
		fr_class = new;
		}
}

class *new_class()
{	class *new;

	if (fr_class) {
		new = (class *) fr_class;
		fr_class = fr_class->mlink;
		}
	else {
		new = structalloc(class);
		ca_class++;
		}

	new->c_ref_count = 0;
	new->c_size = CLASSSIZE;
	sassign(new->file_name, o_nil);
	sassign(new->class_name, o_nil);
	new->super_class = (object *) 0;
	sassign(new->c_inst_vars, o_nil);
	new->context_size = 0;
	sassign(new->message_names, o_nil);
	sassign(new->methods, o_nil);
	return(new);
}

class *mk_class(classname, args)
char *classname;
object **args;
{	class *new;
	object *new_iarray();

	new = new_class();
	assign(new->class_name, args[0]);
	if (! streq(classname, "Object"))
		sassign(new->super_class, args[1]);
	assign(new->file_name, args[2]);
	assign(new->c_inst_vars, args[3]);
	assign(new->message_names, args[4]);
	assign(new->methods, args[5]);
	new->context_size = int_value(args[6]);
	new->stack_max = int_value(args[7]);

	if (streq(classname, "Array")) {
		assign(Array, new);
		assign(o_empty, new_iarray(0));
		}
	else if (streq(classname, "ArrayedCollection")) {
		assign(ArrayedCollection, new);
		assign(o_acollection, new_inst(new));
		assign(o_empty, new_iarray(0));
		}
	else if (streq(classname, "False"))
		assign(o_false, new_inst(new))
	else if (streq(classname, "Magnitude"))
		assign(o_magnitude, new_inst(new))
	else if (streq(classname, "Number"))
		assign(o_number, new_inst(new))
	else if (streq(classname, "Object")) 
		assign(o_object, new_inst(new))
	else if (streq(classname, "Smalltalk"))
		assign(o_smalltalk, new_inst(new))
	else if (streq(classname, "True")) 
		assign(o_true, new_inst(new))
	else if (streq(classname, "UndefinedObject"))
		assign(o_nil, new_inst(new))
	return(new);
}

/* new_sinst - new instance with explicit super object */
object *new_sinst(aclass, super)
class *aclass;
object *super;
{	object *new;
	char *classname, buffer[80];

	if (! is_class(aclass))
		cant_happen(4);
	classname = symbol_value(aclass->class_name);
	if (	streq(classname, "Block") ||
		streq(classname, "Char") ||
		streq(classname, "Class") ||
		streq(classname, "Float") ||
		streq(classname, "Integer") ||
		streq(classname, "Process") ||
		streq(classname, "Symbol") ) {
		sprintf(buffer,"%s: does not respond to new", classname);
		sassign(new, new_str(buffer));
		primitive(ERRPRINT, 1, &new);
		obj_dec(new);
		if (super) /* get rid of unwanted object */ 
			{obj_inc((object *) super); 
			 obj_dec((object *) super);}
		new = o_nil;
		}
	else if (streq(classname, "File")) {
		new = new_file();
		if (super) /* get rid of unwanted object */ 
			{obj_inc((object *) super); 
			 obj_dec((object *) super);}
		}
	else if (streq(classname, "String")) {
		new = new_str("");
		if (super)
			assign(((string *) new)->s_super_obj, super);
		}
	else {
		new = new_obj(aclass, (aclass->c_inst_vars)->size, 1);
		if (super)
			sassign(new->super_obj, super);
		}
	return(new);
}

object *new_inst(aclass)
class *aclass;
{	object *super, *sp_class_name, *lookup_class();
	class *super_class;

	if (! is_class(aclass))
		cant_happen(4);
	if (aclass == o_object->class)
		return(o_object);
	super = (object *) 0;
	sp_class_name = aclass->super_class;
	if (sp_class_name && is_symbol(sp_class_name)) {
		super_class = (class *) 
			lookup_class(symbol_value(sp_class_name));
		if (super_class && is_class(super_class)) 
			super = new_inst(super_class);
		}
	return(new_sinst(aclass, super));
}

free_class(c)
class *c;
{
	if (! is_class(c))
		cant_happen(8);
	obj_dec(c->class_name);
	if (c->super_class)
		obj_dec((object *) c->super_class);
	obj_dec(c->file_name);
	obj_dec(c->c_inst_vars);
	obj_dec(c->message_names);
	obj_dec(c->methods);
	((mstruct *) c )->mlink = fr_class;
	fr_class = (mstruct *) c;
}
