/*
	Little Smalltalk number definitions

*/
/* 
	integer and character definitions
	for integers
		i_size = INTEGERSIZE

	for characters
		i_size = CHARSIZE

*/

struct int_struct {
        int     i_ref_count;
	int     i_size;
	int	i_value;
	};

typedef struct int_struct integer;

# define int_value(x) (((integer *)x)->i_value)
# define char_value(x) ((char) int_value(x))

extern object *new_cori();	/* new Character OR Integer */

# define new_int(x) new_cori(x, 1)
# define new_char(x) new_cori(x, 0)

# define INTINITMAX 50

/*
	floating point definitions
	size should always be FLOATSIZE
*/

struct float_struct {
	int	f_ref_count;
	int	f_size;
	double	f_value;
	};

typedef struct float_struct sfloat;

# define float_value(x) (((sfloat *)x)->f_value)

extern object *new_float();
