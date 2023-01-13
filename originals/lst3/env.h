/*
	Little Smalltalk, version two
	Written by Tim Budd, Oregon State University, July 1987

	environmental factors

	This include file gathers together environmental factors that
	are likely to change from one C compiler to another, or from
	one system to another.  Please refer to the installation 
	notes for more information.

	for systems using the Make utility, the system name is set
	by the make script.
	other systems (such as the Mac) should put a define statement
	at the beginning of the file, as shown below
*/

/*
systems that don't use the Make utility should do something like this:
# define LIGHTC
*/

/*=============== rules for various systems ====================*/

# ifdef B42
	/* Berkeley 4.2, 4.3 and compatible running tty interface */
		/*   which include: */
		/* sequent balance */
		/* Harris HCX-7 */
		/* sun workstations */

typedef unsigned char byte;

# define byteToInt(b) (b)

/* this is a bit sloppy - but it works */
# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRINGS
# define SIGNAL

# endif

# ifdef SYSV
	/* system V systems including: */
	/*	HP-UX for the HP-9000 series */
	/* 	TEK 4404 with some modifications (see install.ms) */

typedef unsigned char byte;

# define byteToInt(b) (b)

# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRING

# endif

# ifdef TURBOC
	/* IBM PC and compatiables using the TURBO C compiler */
	
	/* there are also changes that have to be made to the 
		smalltalk source; see installation notes for
		details */

typedef unsigned char byte;

# define byteToInt(b) (b)

# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRING
# define ALLOC
# define BINREADWRITE
# define CTRLBRK
# define PROTO
# define obtalloc(x,y) (struct objectStruct huge *) farcalloc((unsigned long) x, (unsigned long) y)

#endif

# ifdef ATARI
	/* Atari st 1040 - still exprimental */
	
typedef unsigned char byte;

# define byteToInt(b) (b)

# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRING
# define ALLOC
# define BINREADWRITE
# define obtalloc(x,y) (struct objectStruct *) calloc((unsigned) x, (unsigned) y)

#endif

# ifdef LIGHTC
		/* Macintosh using Lightspeed C compiler */
		/* see install.ms for other changes */

typedef unsigned char byte;

# define byteToInt(b) (b)

# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRINGS
# define BINREADWRITE
# define STDWIN
# define NOARGC
# define PROTO
# define NOSYSTEM
# define obtalloc(x,y) (struct objectStruct *) calloc((unsigned) x, (unsigned) y)

# endif

# ifdef VMS 
	/* VAX VMS */

typedef unsigned char byte;

# define byteToInt(b) (b)

# define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

# define STRING
# define NOARGC

# endif

/* ======== various defines that should work on all systems ==== */

# define streq(a,b) (strcmp(a,b) == 0)

# define true 1
# define false 0

	/* define the datatype boolean */
# ifdef NOTYPEDEF
# define boolean int
# endif
# ifndef NOTYPEDEF
typedef int boolean;
# endif

	/* define a bit of lint silencing */
	/*  ignore means ``i know this function returns something,
		but I really, really do mean to ignore it */
# ifdef NOVOID
# define ignore
# define noreturn
# define void int
# endif
# ifndef NOVOID
# define ignore (void)
# define noreturn void
# endif

/* prototypes are another problem.  If they are available, they should be
used; but if they are not available their use will cause compiler errors.
To get around this we define a lot of symbols which become nothing if
prototypes aren't available */
# ifdef PROTO

# define X ,
# define OBJ object
# define OBJP object *
# define INT int
# define BOOL boolean
# define STR char *
# define FLOAT double
# define NOARGS void
# define FILEP FILE *
# define FUNC ()

# endif

# ifndef PROTO

# define X
# define OBJ
# define OBJP
# define INT
# define BOOL
# define STR
# define FLOAT
# define NOARGS
# define FILEP
# define FUNC

# endif
