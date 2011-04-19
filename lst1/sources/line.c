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

		line grabber - does lowest level input for command lines.
*/
/*
	The source code for the Little Smalltalk System may be freely
	copied provided that the source of all files is acknowledged
	and that this condition is copied with each file.

	The Little Smalltalk System is distributed without responsibility
	for the performance of the program and without any guarantee of
	maintenance.

	All questions concerning Little Smalltalk should be addressed to:

		Professor Tim Budd
		Department of Computer Science
		Oregon State University
		Corvallis, Oregon
		97331
		USA
*/
#include <stdio.h>
#include "lst.h"

# define MAXINCLUDE  10
# define MAXBUFFER  1200		/* text buffer */

static FILE *fdstack[MAXINCLUDE];
static int fdtop = -1;

static char buffer[MAXBUFFER];
static char *buftop = buffer;
char *lexptr = buffer;
static enum {empty, half, filled} bufstate = empty;
int inisstd = 0;
extern object *o_tab;

/* set file - set a file on the file descriptor stack */
set_file(fd)
FILE *fd;
{
	if ((++fdtop) > MAXINCLUDE)
		cant_happen(18);
	fdstack[fdtop] = fd;
	if (fd == stdin) inisstd = 1;
	else inisstd = 0;
}

/* line-grabber - read a line of text 
	do blocked i/o if blocked is nonzero,
	otherwise do non-blocking i/o */

int line_grabber(int block)
{
	/* if it was filled last time, it is now empty */
	if (bufstate == filled) {
		bufstate = empty;
		buftop = buffer;
		lexptr = buffer;
		}

	if ( ! block)
		return(0); /* for now, only respond to blocked requests*/
	else while (bufstate != filled) {
		if (fdtop < 0) {
			fprintf(stderr,"no files to read\n");
			return(-1);
			}
		if (inisstd && o_tab)
			primitive(RAWPRINT, 1, &o_tab);
		if (fgets(buftop, MAXBUFFER, fdstack[fdtop]) == NULL) {
			bufstate = empty;
			if (fdstack[fdtop] != stdin)
				fclose(fdstack[fdtop]);
			if (--fdtop < 0) return(-1);
			inisstd = (fdstack[fdtop] == stdin);
			}
		else {
			bufstate = half;
			while (*buftop) buftop++;
			if (*(buftop-1) == '\n') {
				if (*(buftop-2) == '\\') {
					buftop -= 2;
					}
				else {
					if ((buftop - buffer) > MAXBUFFER)
						cant_happen(18);
					*buftop = '\0';
					bufstate = filled;
					}
				}
			}	
		}
	return(bufstate == filled);
}
