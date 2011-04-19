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
	Little Smalltalk string definitions
*/
/*
	for strings s_size = STRINGSIZE

	Unlike other special objects (integers, floats, etc), strings
	must keep their own super_obj pointer, since the class
	ArrayedCollection (a super class of String) contains instance
	variables, and thus each instance of String must have a unique
	super_obj.
*/

struct string_struct {
	int	s_ref_count;
	int	s_size;
	object 	*s_super_obj;
	char	*s_value;
	} ;

typedef struct string_struct string;

extern object *new_str();		/* make a new string object */
extern string *new_istr();		/* internal form of new string */
extern char   *walloc();		/* allocate a copy a word */

# define string_value(x) (((string *) x)->s_value)
