/*
	Little Smalltalk string and symbol definitions
*/
/*
	for symbols y_size = SYMBOLSIZE

	only one text copy of each symbol is kept.
	A global symbol table is searched each time a new symbol is
	created, and symbols with the same character representation are
	given the same entry.

*/

struct symbol_struct {
	int	y_ref_count;
	int	y_size;
	char	*y_value;
	} ;

typedef struct symbol_struct symbol;

extern symbol *sy_search();	/* binary search for a symbol */
extern char   *w_search();	/* binary search for a word */

# define symbol_value(x) (((symbol *) x)->y_value)
# define new_sym(val) ((object *) sy_search(val, 1))


# define SYMTABMAX 500

/* SYMINITSIZE symbol entries are allocated at the start of execution,
which prevents malloc from being called too many times */

# define SYMINITSIZE 60
