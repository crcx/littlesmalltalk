/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	values returned by the lexical analyzer
*/

# ifndef NOENUMS

typedef enum tokensyms { nothing, name, namecolon, 
	intconst, floatconst, charconst, symconst,
	arraybegin, strconst, binary, closing, inputend} tokentype;
# endif

# ifdef NOENUMS
# define tokentype int
# define nothing 0
# define name 1
# define namecolon 2
# define intconst 3
# define floatconst 4
# define charconst 5
# define symconst 6
# define arraybegin 7
# define strconst 8
# define binary 9
# define closing 10
# define inputend 11

# endif

extern tokentype nextToken();

extern tokentype token;		/* token variety */
extern char tokenString[];	/* text of current token */
extern int tokenInteger;	/* integer (or character) value of token */
extern double tokenFloat;	/* floating point value of token */
