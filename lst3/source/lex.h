/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	values returned by the lexical analyzer
*/

typedef enum tokensyms { nothing, nameconst, namecolon,
    intconst, floatconst, charconst, symconst,
    arraybegin, strconst, binary, closing, inputend
} tokentype;

extern tokentype nextToken(NOARGS);

extern tokentype token;		/* token variety */
extern char tokenString[];	/* text of current token */
extern int tokenInteger;	/* integer (or character) value of token */
extern double tokenFloat;	/* floating point value of token */
extern noreturn lexinit();	/* initialization routine */
