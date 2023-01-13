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
