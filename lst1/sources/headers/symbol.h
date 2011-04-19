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
