/*
	Little Smalltalk, version two
	Written by Tim Budd, Oregon State University, July 1987

	environmental factors

	This include file gathers together environmental factors that
	are likely to change from one C compiler to another, or from
	one system to another.  These include:

	1. The type boolean.  A typedef is used to define this;
		on some older systems typedefs may not work, and a
		# define statement should be used instead.
		The only other typedef appears in memory.h

	2. The statement ignore - this appears on functions being used
		as procedures.  It has no effect except as a lint
		silencer, and is also the only place where the type
		``void'' appears.  If your system doesn't support void,
		define ignore to be nothing.

	3. The datatype byte - an 8-bit unsigned quantity.
		On some systems the datatype ``unsigned char'' does not
		work - for these some experimentation may be necessary.
		The macro byteToInt() converts a byte value into an integer.
		Again a typedef is used, which can be replaced by a
		define.

	4. If your system does not have enumerated constants, the define
		NOENUM should be given, in which case enumerated constants
		are replaced by defines.

	5. The define constant INITIALIMAGE should give the name (as a path)
		of the default standard object image file.
*/

# define true 1
# define false 0

typedef int boolean;

# define ignore (void)

typedef unsigned char byte;

# define byteToInt(b) (b)

# define INITIALIMAGE "imageFile"
 
# define TEMPFILENAME "/usr/tmp/lstXXXXXX"
