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
		misc lexer related routines
		timothy a. budd 12/84
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
#include <ctype.h>
#include "lst.h"

extern char toktext[];

/* dolexcommand - read a ) type directive, and process it */
dolexcommand(p)
char *p;
{       char *q, buffer[100];

	/* replace trailing newline with end of string */
	for (q = p; *q && *q != '\n'; q++);
	if (*q == '\n') *q = '\0';

        switch( *++p) {
           case '!': 
# ifndef NOSYSTEM
		system(++p); 
# endif
		break;

           case 'e': for (++p; isspace(*p); p++);
		     if (! lexedit(p)) lexinclude(p);
                     break;

	   case 'g': for (++p; isspace(*p); p++);
		     sprintf(buffer,"%s/%s", LIBLOC, p);
		     lexread(buffer);
		     break;

           case 'i': for (++p; isspace(*p); p++);
                     lexinclude(p);
                     break;

           case 'r': for (++p; isspace(*p); p++);
                     lexread(p);
                     break;

	   case 's': for(++p; isspace(*p); p++);
		     dosave(p);
		     break;

	   case 'l': for(++p; isspace(*p); p++);
		     doload(p);
		     break;

           default:  lexerr("unknown command %s", toktext);
           }
}

/* doload/dosave routines written by nick buchholz */
/*
	doload and dosave routines make the following assumptions
	1. version is the first global variable declared in main.
	2. main is the first procedure seen by the loader
	3. the loader allocates memory in the order it sees the procedures
	4. memory is laid out as on the vax 780 under 4.2

	on other machines any or all of these might be false and the
	doload/dosave routines will not work
*/
extern int version;

dosave(p) char *p;{
    int fd; 
    char *start, *end, *sbrk(); 
    unsigned int length, len;
    int dlen;

# ifdef OPEN3ARG
    if ((fd = open(p, O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1)
# endif
# ifndef OPEN3ARG
    if ((fd = creat(p, 0666)) == -1)
# endif
   	fprintf(stderr,"can't open: %s\n",p);

    start = (char *) &version;
    end = sbrk(0);
    length = end - start;

    write(fd, &version, sizeof(int));
    write(fd, &start, sizeof(char *));
    write(fd, &length, sizeof(unsigned int));

    for (len = 0; len < length; len += dlen) {
	dlen = ((length - len) > 512) ? 512 : (length - len);
	if (dlen != write(fd, start + len, dlen)) {
		cant_happen(23);
		}
	}

    fprintf(stderr,"%u bytes written\n",len);

    close(fd);
}

# ifdef ENVSAVE
extern char **environ;
# endif

doload(p) char *p;{
    int fd; 
    char *start, *end, *brk(); 
    unsigned int length, len; 
    int dlen;
    int test;
# ifdef ENVSAVE
    char **evsave;
# endif

# ifdef OPEN3ARG
    if ((fd = open(p, O_RDONLY, 0)) == -1)
# endif
# ifndef OPEN3ARG
    if ((fd = open(p, 0 )) == -1)
# endif
	fprintf(stderr,"no such context as: %s\n", p);

    else {
	read(fd, &test, sizeof(int));
	read(fd, &start, sizeof(char *));
	read(fd, &length, sizeof(unsigned int));

	if ((test != version) || (start != (char *) &version))
	    fprintf(stderr,"%s: not a valid context file for version %d\n", 
				p, version);
	else {
	    start = (char *) &version;
	    end = brk(start + length + 1);
# ifdef ENVSAVE
	    evsave = environ;
# endif

    	    for (len = 0; len < length; len += dlen) {
		dlen = ((length - len) > 512) ? 512 : (length - len);
		if (dlen != read(fd, start + len, dlen)) {
			cant_happen(23);
			}
		}
# ifdef ENVSAVE
	   environ = evsave;
# endif
	    fprintf(stderr,"%u bytes read\n",len);
	}
	close(fd);
    }
}

/* lexread - read commands from a file */
lexread(name)
char *name;
{	FILE *fd;

	fd = fopen(name, "r");
	if (fd == NULL) {
		fprintf(stderr,"can't open %s\n", name);
		}
	else {
		set_file(fd);
		}
}

/* lexinclude - parse a class and include the class description */
lexinclude(name)
char *name;
{  char template[60], cmdbuf[120];
   int  i;

# ifndef NOSYSTEM
   gettemp(template);
   sprintf(cmdbuf,"%s %s >%s", PARSER, name, template);
   i = system(cmdbuf);
   if (i == 0)
   	lexread(template);
# endif
# ifdef NOSYSTEM
   fprintf(stderr,")i does not work on this system\n");
# endif
}

/* lexedit - edit a class description */
int lexedit(name)
char *name;
{	char *e, buffer[100], *getenv();

# ifndef NOSYSTEM
	e = getenv("EDITOR");
	if (!e) e = "ed";
	sprintf(buffer,"%s %s", e, name);
	return(system(buffer));
# endif
# ifdef NOSYSTEM
	fprintf(stderr,")e does not work on this system\n");
	return(1);
# endif
}
