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
 * string creation and deletion
 * timothy a. budd, 10/84
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include "lst.h"

int ca_str = 0;
int ca_wal = 0;

/* walloc allocates a string containing the same chars as the arg */

# define WALLOCINITSIZE 1000

static char wtable[WALLOCINITSIZE];
int wtop = 0;

char *walloc(val)
char *val;
{	char *p;
	int  size;

	size = 1 + strlen(val);
	if ((size < 40) && ((wtop + size) < WALLOCINITSIZE)) {
		p = &wtable[wtop];
		wtop += size;
		}
	else {
		p = o_alloc((unsigned) size);
		ca_wal++;
		}
	strcpy(p, val);
	return(p);
}

/*---------------------------------------*/
extern class *ArrayedCollection;
extern object *o_acollection;

static mstruct *fr_string = 0;

# define STRINITSIZE 50

static string st_init_table[STRINITSIZE];

str_init() {
	string *p;
	mstruct *new;
	int i;

	for (p = st_init_table, i = 0; i < STRINITSIZE; i++, p++) {
		new = (mstruct *) p;
		new->mlink = fr_string;
		fr_string = new;
		}
}

extern int started;
static new_rstr(new)
string *new;
{
	new->s_ref_count = 0;
	new->s_size = STRINGSIZE;
	if (! started)
		sassign(new->s_super_obj, o_acollection);
	else if (ArrayedCollection)
		sassign(new->s_super_obj, new_inst(ArrayedCollection));
	else
		new->s_super_obj = (object *) 0;
}

string *new_istr(text)
char *text;
{	register string *new;

	if (fr_string) {
		new = (string *) fr_string;
		fr_string = fr_string->mlink;
		}
	else {
		ca_str++;
		new = structalloc(string);
		}

	new->s_value = text;
	new_rstr(new);
	return(new);
}

# define STRLISTMAX 100

mstruct *frl_str[STRLISTMAX];

object *new_str(text)
char *text;
{	int size;
	string *new;

	size = 1 + strlen(text);
	if ((size < STRLISTMAX) && frl_str[size]) {
		new = (string *) frl_str[size];
		frl_str[size] = frl_str[size]->mlink;
		strcpy(new->s_value, text);
		new_rstr(new);
		}
	else {
		new = new_istr(walloc(text));
		}
	return((object *) new);
}

free_string(s)
string *s;
{	int size;

	if (s->s_super_obj)
		obj_dec(s->s_super_obj);
	size = 1 + strlen(s->s_value);
	if (size < STRLISTMAX) {
		((mstruct *)s)->mlink = frl_str[size];
		frl_str[size] = (mstruct *) s;
		}
	else {
		((mstruct *)s)->mlink = fr_string;
		fr_string = (mstruct *) s;
		}
}
