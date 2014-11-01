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
 * Internal class dictionary
 * timothy a. budd, 10/84
 *
 * In order to facilitate lookup, classes are kept in an internal data
 * dictionary.  Classes are inserted into this dictionary using a
 * primtitive, and are removed by either being overridden, or being
 * flushed at the end of execution.
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

struct class_entry {		/* structure for internal dictionary */
	char *cl_name;
	object *cl_description;
	struct class_entry *cl_link;
	};

static struct class_entry *class_dictionary = 0;
int ca_cdict = 0;
static mstruct *fr_cdict = 0;		/* class dictionary free list */

# define CDICTINIT 30
static struct class_entry cdsinit[CDICTINIT];

/* cdic_init - initialize the internal class dictionary */
cdic_init() {
	struct class_entry *p;
	mstruct *new;
	int i;

	for (p = cdsinit, i = 0; i < CDICTINIT; i++, p++) {
		new = (mstruct *) p;
		new->mlink = fr_cdict;
		fr_cdict = new;
		}
}

/* enter_class - enter a class into the internal class dictionary */
int enter_class(name, description)
char *name;
object *description;
{	struct class_entry *p;

	for (p = class_dictionary; p; p = p->cl_link)
		if (strcmp(name, p->cl_name) == 0) {
			assign(p->cl_description, description);
			return 0;
			}
	/* not found, make a new entry */
	if (fr_cdict) {
		p = (struct class_entry *) fr_cdict;
		fr_cdict = fr_cdict->mlink;
		}
	else {
		p = structalloc(struct class_entry);
		ca_cdict++;
		}
	p->cl_name = name;
	sassign(p->cl_description, description);
	p->cl_link = class_dictionary;
	class_dictionary = p;
}

/* lookup - take a name and find the associated class object */
object *lookup_class(name)
char *name;
{	struct class_entry *p;

	for (p = class_dictionary; p; p = p->cl_link)
		if (strcmp(name, p->cl_name) == 0)
			return(p->cl_description);
	return((object *) 0);
}

/* free_all_classes - flush all references for the class dictionary */
free_all_classes()
{	struct class_entry *p;

	for (p = class_dictionary; p; p = p->cl_link) {
		obj_dec(p->cl_description);
		}
}

/* class_list - list all the subclasses of a class (recursively),
	indenting by a specified number of tab stops */
int class_list(c, n)
class *c;
int n;
{	struct class_entry *p;
	object *prs[2];
	class *q;
	char *name;

	/* first print out this class name */
	if (! is_symbol(c->class_name))
		return 0;
	sassign(prs[0], c->class_name);
	name = symbol_value(c->class_name);
	sassign(prs[1], new_int(n));
	primitive(SYMPRINT, 2, prs);
	obj_dec(prs[0]);
	obj_dec(prs[1]);

	/* now find all subclasses and print them out */
	for (p = class_dictionary; p; p = p->cl_link) {
		q = (class *) p->cl_description;
		if ((is_symbol(q->super_class)) && 
		   (strcmp(name, symbol_value(q->super_class)) == 0) )
			class_list(q, n+1);
		}
}
