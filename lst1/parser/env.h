/*
	Little Smalltalk

	execution environment definitions.

The Little Smalltalk system is tailored to various machines by
changing defined constants.  These constants, and their meanings,
are as follows:

CURSES	defined if the curses(3) library is available and the primitive
	graphics it provides is desired

GAMMA	defined if gamma() is part of the math library

ENVSAVE	defined if it is required to save environ during fast load

FACTMAX	maximum integer value for which a factorial can be computed by
	repeated multiplication without overflow.

FASTDEFAULT	defined if the default behavior should be to do a fast load

FLUSHREQ	if defined a fflush is given after every call to printf
		or fprintf

INLINE	generate inline code for increments or decrements -
	produces larger, but faster, code.

MDWINDOWS	defined if the maryland windows package is used

NOSYSTEM	defined if the system() call is NOT provided
		(seriously limits functionality)

OPEN3ARG	defined if 3 argument style opens are used

PLOT3	defined if you have a device for which the plot(3) routines work
	directly on the terminal (without a filter)
	provides many of these routines as primitive operations
	(see class PEN in /prelude)

SMALLDATA	if defined various means are used to reduce the size of the
		data segment, at the expense of some functionality.

SIGS		define in the signal system call is available
		for trapping user interrupt signals

SETJUMP		defined if the setjump facility is available 

	In addition to defining constants, the identifier type ``unsigned
character'' needs to be defined.  Bytecodes are stored using this datatype.
On machines which do not support this datatype directly, macros need to be
defined that convert normal chars into unsigned chars.  unsigned chars are
defined by a typedef for ``uchar'' and a pair of macros that convert an int
into a uchar and vice-versa.

	In order to simplify installation on systems to which the
Little Smalltalk system has already been ported, various ``meta-defines''
are recognized.  By defining one of these symbols, the correct definitions
for other symbols will automatically be generated.  The currently
recognized meta-defines are as follows:
	
BERK42	Vax Berkeley 4.2
DECPRO	Dec Professional 350 running Venix
HP9000	Hewlett Packard 9000
PDP1170	PdP 11/70 (also other PDP 11 machines)
PERKELM	Perken Elmer 8/32
RIDGE	Ridge ROS 3.1

	Finally, a few path names have to be compiled into the code.
These path names are the following:

TEMPFILE - a temporary file name in mktemp format
PARSER - the location of the parser
PRELUDE - the location of the standard prelude in ascii format
FAST - the location of the standard prelude in saved format
LIBLOC - the location of the directory for additional libraries

*/

# define TEMPFILE "/tmp/stXXXXXX"
# define PARSER   "./parse"
# define PRELUDE  "./standard"
# define FAST     "./stdsave"
# define LIBLOC	  "./prelude"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>meta-define <<<<<<<<<<<<<<<*/

# define BERK42

/*------------------------------  VAX Berkeley 4.2 definition */
# ifdef BERK42

# define GAMMA		/* gamma value is known */
# define FACTMAX 12
# define FLUSHREQ	/* flush after every printf */
# define SIGS
# define SETJUMP
typedef unsigned char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) ((int) x)
/* # define MDWINDOWS */
/* FASTLOADING DOES work, and should eventually be defined to be standard*/
/*# define FASTDEFAULT*/	/* default to fast-loading */

# endif		/* BERK42 definition */

/*------------------------------  HP 9000 / HP - UX definition */
# ifdef HP9000

# define GAMMA		/* gamma value is known */
# define FACTMAX 12
# define FLUSHREQ	/* flush after every printf */
# define SIGS
# define SETJUMP
typedef unsigned char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) ((int) x)

# endif		/* HP 9000 definition */

/* ---------------------------------------RIDGE ROS 3.1 definition */
# ifdef RIDGE

# define GAMMA		/* gamma value is known */
# define FACTMAX 12
typedef unsigned char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) ((int) x)

# endif		/* RIDGE definition */

/* --------------------------------------------DEC PRO definitions */
# ifdef DECPRO

/* GAMMA, OPEN3ARG not defined */
# define ENVSAVE
# define FACTMAX 8
# define SMALLDATA
/* unsigned characters not supported, but can be simulated */
typedef char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) (unsigned) (x >= 0 ? x : x + 256)

# endif		/* DECPRO definition */

/* --------------------------------------------PDP11/70 definitions */
# ifdef PDP1170

/* GAMMA, OPEN3ARG not defined */
# define ENVSAVE
# define FACTMAX 8
# define FLUSHREQ
# define SIGS
# define SETJUMP
/* unsigned characters not supported, but can be simulated */
typedef char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) (unsigned) (x >= 0 ? x : x + 256)

# endif		/* PDP1170 definition */

/*------------------------------  Perkin Elmer 8/32 definitions */
# ifdef PERKELM

# define ENVSAVE
# define FACTMAX 12
# define FLUSHREQ	/* flush after every printf */
typedef unsigned char uchar;
# define itouc(x) ((uchar) x)
# define uctoi(x) ((int) x)

# endif		/* PERKELM definition */

/******************************************************************/
/*
	the following are pretty much independent of any system
*/

# define INLINE		/* produce in line code for incs and decs */
/* # define CURSES	*/
/* # define PLOT3 	*/
