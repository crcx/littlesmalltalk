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
 * programs used by class File
 * timothy a. budd, 11/84
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include "lst.h"

static mstruct *fr_file = 0;	/* free file list */

object *new_file()
{	struct file_struct *new;

	if (fr_file) {
		new = (struct file_struct *) fr_file;
		fr_file = fr_file->mlink;
		}
	else {
		new = structalloc(struct file_struct);
		}

	new->l_size = FILESIZE;
	new->l_ref_count = 0;
	new->file_mode = STRMODE;
	new->fp = NULL;
	return((object *) new);
}

free_file(phil)
struct file_struct *phil;
{
	if (! is_file(phil))
		cant_happen(8);
	if (phil->fp != NULL)
		fclose(phil->fp);
	((mstruct *) phil)->mlink = fr_file;
	fr_file = (mstruct *) phil;
}

file_err(message)
char *message;
{	object *errp;
	char buffer[150];

	sprintf(buffer,"File: %s", message);
	sassign(errp, new_str(buffer));
	primitive(ERRPRINT, 1, &errp);
	obj_dec(errp);
}

file_open(phil, name, type)
struct file_struct *phil;
char *name, *type;
{	char buffer[100];

	if (phil->fp != NULL)
		fclose(phil->fp);
	phil->fp = fopen(name, type);
	if (phil->fp == NULL) {
		sprintf(buffer,"can't open: %s\n", name);
		file_err(buffer);
		}
}

# define BUFLENGTH 250

object *file_read(phil)
struct file_struct *phil;
{	object *new;
	int c;
	char buffer[BUFLENGTH], *p;

	if (phil->fp == NULL) {
		file_err("attempt to read from unopened file");
		return(o_nil);
		}
	switch(phil->file_mode) {
		case CHARMODE:
			if (EOF == (c = fgetc(phil->fp)))
				new = o_nil;
			else
				new = new_char(c);
			break;
		case STRMODE:
			if (NULL == fgets(buffer, BUFLENGTH, phil->fp))
				new = o_nil;
			else {
				p = &buffer[strlen(buffer) - 1];
				if (*p == '\n') *p = '\0';
				new = new_str(buffer);
				}
			break;
		case INTMODE:
			if (EOF == (c = getw(phil->fp)))
				new = o_nil;
			else
				new = new_int(c);
			break;
		default:
			file_err("unknown mode");
			new = o_nil;
		}
	return(new);
}

void file_write(phil, obj)
struct file_struct *phil;
object *obj;
{
	if (phil->fp == NULL) {
		file_err("attempt to write to unopened file");
		return;
		}
	switch(phil->file_mode) {
		case CHARMODE:
			if (! is_character(obj)) goto modeerr;
			fputc(int_value(obj), phil->fp);
			break;
		case STRMODE:
			if (! is_string(obj)) goto modeerr;
			fputs(string_value(obj), phil->fp);
			fputc('\n', phil->fp);
			break;
		case INTMODE:
			if (! is_integer(obj)) goto modeerr;
			putw(int_value(obj), phil->fp);
			break;
		}
	return;
modeerr:
	file_err("attempt to write object of wrong type for mode");
}
