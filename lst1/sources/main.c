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
 * main driver
 * timothy a. budd
 *
 * 1.   initializes various smalltalk constants and classes with
 *      legitimate values.  these values, however, will for the most part
 *      be overridden when the standard prelude is read in.
 *
 * 2.   reads in the standard prelude, plus any additional files listed
 *      on the command line.
 *
 * 3.   places the driver reading stdin on the process queue and starts
 *      the process driver running.
 *************************************************************************/
int version = 2; /* a Kludge to get us the start of the data segment.
			used to save and restore contexts */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lst.h"

object *null_object;	/* a totally classless object */
char filebase[80];	/* base for forming temp file names */

extern int n_incs, n_decs, n_mallocs;	/* counters */
extern int opcount[], ohcount, spcount[];

extern int ca_block, ca_barray, ca_class, ca_terp, ca_int, ca_float;
extern int ca_obj, ca_str, ca_sym, ca_wal, ca_cdict;
extern int ca_cobj[];
extern int btabletop, wtop;	/* more counters */

# ifdef INLINE
object *_dx;		/* object pointer used for decrementing */
# endif

int silence = 0;    	/* 1 if silence is desired on output */
int noload = 0;     	/* 1 if no loading of standard prelude is desired */
int debug = 0;		/* debug flag, set by a primitive call */
int fastload = 0;	/* 1 if doing a fast load of saved image */
int lexprnt = 0;	/* 1 if printing during lex is desired (for debug) */
int prallocs = 0;	/* 1 if printing final allocation figures is wanted */
int started = 0;	/* 1 if we have started reading user commands */
int prntcmd = 1;	/* 1 or 2 and commands will be printed as evaled */

/* pseudo-variables */
object *o_acollection;		/* arrayed collection (used internally) */
object *o_drive;		/* driver interpreter */
object *o_empty;		/* the empty array (used during initial) */
object *o_false;		/* value for pseudo variable false */
object *o_magnitude;		/* instance of class Magnitude */
object *o_nil;			/* value for pseudo variable nil */
object *o_number;		/* instance of class Number */
object *o_object;		/* instance of class Object */
object *o_tab;			/* string with tab only */
object *o_true;			/* value of pseudo variable true */
object *o_smalltalk;		/* value of pseudo variable smalltalk */

/* classes to be initialized */
extern class *Array;
extern class *ArrayedCollection;

/* input stack */
extern FILE *fdstack[];
extern int fdtop;

/* main - main driver */
main(argc, argv)
int argc;
char **argv;
{	int i;
	class *null_class();
	object *tempobj;
	FILE *sfd;

# ifdef FASTDEFAULT
	fastload = 1;
# endif
# ifndef FASTDEFAULT
	fastload = 0;
# endif

	/* first check for flags */
	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-')
			switch(argv[i][1]) {
				case 'f': fastload = 1; break;
				case 'l': 		/* fall through */
				case 'n': noload = 1; /* fall through */
				case 'm': fastload = 0; break;
				case 'z': lexprnt = 1; break;
			}

	if (fastload) {
		dofast();
		}
	else {			/* gotta do it the hard way */
		strcpy(filebase, TEMPFILE);
		mkstemp(filebase);

		byte_init();
		class_init();
		cdic_init();
		int_init();
		str_init();
		sym_init();
		init_objs();

		null_object = new_obj((class *) 0, 0, 0);

		sassign(o_object, null_object);
		/* true is given a different object from others , so comparisons
					work correctly */
		sassign(o_true, new_obj((class *) 0, 0, 0));
		sassign(o_false, null_object);
		sassign(o_nil, null_object);
		sassign(o_number, null_object);
		sassign(o_magnitude, null_object);
		sassign(o_empty, null_object);
		sassign(o_smalltalk, null_object);
		sassign(o_acollection, null_object);

		sassign(Array, null_class("Array"));
		sassign(ArrayedCollection, null_class("ArrayedCollection"));

		drv_init();	/* initialize the driver */
		sassign(o_drive, (object *) cr_interpreter((interpreter *) 0,
			null_object, null_object, null_object, null_object));
		init_process((interpreter *) o_drive);

		/* now read in standard prelude */
		if (! noload) {
			sfd = fopen(PRELUDE, "r");
			if (sfd == NULL) cant_happen(20);
			set_file(sfd);
			start_execution();
			fclose(sfd);
			}

		/* then set lexer up to read stdin */
		set_file(stdin);
		sassign(o_tab, new_str("\t"));

# ifdef CURSES
		/* finally initialize the curses window package */
		initscr();
# endif
# ifdef PLOT3
		/* initialize the plotting device */
		openpl();
# endif
		}

	/* announce that we're ready for action */
	sassign(tempobj, new_sym("Little Smalltalk"));
	primitive(SYMPRINT, 1, &tempobj);
	obj_dec(tempobj);
	started = 1;

	/* now read in the command line files */
	user_read(argc, argv);

	start_execution();

	/* print out one last newline - to move everything out of output
	queue */
	sassign(tempobj, new_sym("\n"));
	primitive(SYMPRINT, 1, &tempobj);
	obj_dec(tempobj);

	/* now free things up, hopefully keeping ref counts straight */

	drv_free();

	flush_processes();

	free_low_nums();

	obj_dec((object *) Array);
	obj_dec((object *) ArrayedCollection);

	free_all_classes();
	
	obj_dec(o_tab);
	obj_dec(o_drive);
	obj_dec(o_magnitude);
	obj_dec(o_number);
	obj_dec(o_nil);
	obj_dec(o_false);
	obj_dec(o_true);
	obj_dec(o_object);
	obj_dec(o_empty);
	obj_dec(o_smalltalk);
	obj_dec(o_acollection);

	if (! silence)
		fprintf(stderr,"incs %u decs %u difference %d allocs %d\n", 
			n_incs, n_decs, n_incs - n_decs, n_mallocs);
	ohcount = 0;
	for (i = 0; i < 16; i++)
		ohcount += opcount[i];
	fprintf(stderr,"opcount %d\n", ohcount);
		/*fprintf(stderr,"opcode [%d] counts %d\n", i, opcount[i]);*/
	/*fprintf(stderr,"ohcount %d\n", ohcount);
	for (i = 0; i < 16; i++)
		fprintf(stderr,"sp count %d %d\n", i , spcount[i]);*/
	if (prallocs) {
		fprintf(stderr,"blocks allocated %d\n", ca_block);
		fprintf(stderr,"bytearrays allocated %d\n", ca_barray);
		fprintf(stderr,"classes allocated %d\n", ca_class);
		fprintf(stderr,"interpreters allocated %d\n", ca_terp);
		fprintf(stderr,"ints allocated %d\n", ca_int);
		fprintf(stderr,"floats allocated %d\n", ca_float);
		fprintf(stderr,"strings allocated %d\n", ca_str);
		fprintf(stderr,"symbols allocated %d\n", ca_sym);
		fprintf(stderr,"class entryies %d\n", ca_cdict);
		fprintf(stderr,"wallocs %d\n", ca_wal);
		fprintf(stderr,"wtop %d\n", wtop);
		fprintf(stderr,"byte table top %d\n", btabletop);
		fprintf(stderr,"smalltalk objects allocated %d\n", ca_obj);
		for (i = 0; i < 5; i++)
			fprintf(stderr,"size %d objects %d\n", i, ca_cobj[i]);
	}
	clean_files();

# ifdef PLOT3
	closepl();
# endif
# ifdef CURSES
	endwin();
# endif

	exit(0);	/* say good by gracie */
}

/* dofast - do a fast load of the standard prelude */
 dofast() {
	char buffer[100];

	sprintf(buffer,")l %s\n", FAST);
	dolexcommand(buffer);
}

/* null_class - create a null class for bootstrapping purposes */
 class *null_class(name)
char *name;
{	class *new, *new_class();

	new = new_class();
	assign(new->class_name, new_sym(name));
	enter_class(name, (object *) new);
	return(new);
}

/* user_read - read the user command line arguments */
 user_read(argc, argv)
int argc;
char **argv;
{	int i, count;
	char c, buffer[100];
	char name[100];
	FILE *fd = 0;

	gettemp(name);
	count = 0;
	fd = fopen(name, "w");
	if (fd == NULL)
		cant_happen(22);
	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'a':
					prallocs = 1; break;
				case 'g': case 'l': case 'r':
					c = argv[i][1];
					sprintf(buffer,")%c %s\n", 
						c, argv[++i]);
					count++;
					fputs(buffer, fd);
					break;
				case 'd':
					prntcmd = argv[i][1] - '0';
					break;
				case 's':
					silence = 1;
					break;
				}
			}
		else {
			sprintf(buffer,")i %s\n", argv[i]);
			count++;
			fputs(buffer, fd);
			}
	fclose(fd);
	if (count) {
		fd = fopen(name, "r");
		if (fd == NULL)
			cant_happen(22);
		set_file(fd);
		}
}

/* gettemp makes a temp file name that can be deleted when finished */
 char c = 'a';
gettemp(buffer)
char *buffer;
{
	sprintf(buffer,"%s%c", filebase, c++);
	if (c > 'z') c = 'a';	/* wrap around forever */
}

/* clean_files - delete all temp files created */
 clean_files()
{
	char buffer[100];

# ifndef NOSYSTEM
	sprintf(buffer,"rm -f %s*", filebase);
	system(buffer);
# endif
}
