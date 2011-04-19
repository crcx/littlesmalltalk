/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	lexical analysis routines for method parser
	should be called only by parser 
*/

#include <stdio.h>
#include <ctype.h>
#include "env.h"
#include "memory.h"
#include "lex.h"

extern double atof();

/* global variables returned by lexical analyser */

tokentype token;		/* token variety */
char tokenString[80];		/* text of current token */
int tokenInteger;		/* integer (or character) value of token */
double tokenFloat;		/* floating point value of token */

/* local variables used only by lexical analyser */

static char *cp;		/* character pointer */
static char pushBuffer[10];	/* pushed back buffer */
static int pushindex;		/* index of last pushed back char */
static char cc;			/* current character */
static long longresult;		/* value used when building int tokens */

/* lexinit - initialize the lexical analysis routines */
noreturn lexinit(str)
char *str;
{
    pushindex = 0;
    cp = str;
    /* get first token */
    ignore nextToken();
}

/* pushBack - push one character back into the input */
static void pushBack(c)
char c;
{
    pushBuffer[pushindex++] = c;
}

/* nextChar - retrieve the next char, from buffer or input */
static char nextChar()
{
    if (pushindex > 0)
	cc = pushBuffer[--pushindex];
    else if (*cp)
	cc = *cp++;
    else
	cc = '\0';
    return (cc);
}

/* peek - take a peek at the next character */
char peek()
{
    pushBack(nextChar());
    return (cc);
}

/* isClosing - characters which can close an expression */
static boolean isClosing(c)
char c;
{
    switch (c) {
    case '.':
    case ']':
    case ')':
    case ';':
    case '\"':
    case '\'':
	return (true);
    }
    return (false);
}

/* isSymbolChar - characters which can be part of symbols */
static boolean isSymbolChar(c)
char c;
{
    if (isdigit(c) || isalpha(c))
	return (true);
    if (isspace(c) || isClosing(c))
	return (false);
    return (true);
}

/* singleBinary - binary characters that cannot be continued */
static boolean singleBinary(c)
char c;
{
    switch (c) {
    case '[':
    case '(':
    case ')':
    case ']':
	return (true);
    }
    return (false);
}

/* binarySecond - return true if char can be second char in binary symbol */
static boolean binarySecond(c)
char c;
{
    if (isalpha(c) || isdigit(c) || isspace(c) || isClosing(c) ||
	singleBinary(c))
	return (false);
    return (true);
}

tokentype nextToken()
{
    char *tp;
    boolean sign;

    /* skip over blanks and comments */
    while (nextChar() && (isspace(cc) || (cc == '"')))
	if (cc == '"') {
	    /* read comment */
	    while (nextChar() && (cc != '"'));
	    if (!cc)
		break;		/* break if we run into eof */
	}

    tp = tokenString;
    *tp++ = cc;

    if (!cc)			/* end of input */
	token = inputend;

    else if (isalpha(cc)) {	/* identifier */
	while (nextChar() && isalnum(cc))
	    *tp++ = cc;
	if (cc == ':') {
	    *tp++ = cc;
	    token = namecolon;
	} else {
	    pushBack(cc);
	    token = nameconst;
	}
    }

    else if (isdigit(cc)) {	/* number */
	longresult = cc - '0';
	while (nextChar() && isdigit(cc)) {
	    *tp++ = cc;
	    longresult = (longresult * 10) + (cc - '0');
	}
	if (longCanBeInt(longresult)) {
	    tokenInteger = longresult;
	    token = intconst;
	} else {
	    token = floatconst;
	    tokenFloat = (double) longresult;
	}
	if (cc == '.') {	/* possible float */
	    if (nextChar() && isdigit(cc)) {
		*tp++ = '.';
		do
		    *tp++ = cc;
		while (nextChar() && isdigit(cc));
		if (cc)
		    pushBack(cc);
		token = floatconst;
		*tp = '\0';
		tokenFloat = atof(tokenString);
	    } else {
		/* nope, just an ordinary period */
		if (cc)
		    pushBack(cc);
		pushBack('.');
	    }
	} else
	    pushBack(cc);

	if (nextChar() && cc == 'e') {	/* possible float */
	    if (nextChar() && cc == '-') {
		sign = true;
		ignore nextChar();
	    } else
		sign = false;
	    if (cc && isdigit(cc)) {	/* yep, its a float */
		*tp++ = 'e';
		if (sign)
		    *tp++ = '-';
		while (cc && isdigit(cc)) {
		    *tp++ = cc;
		    ignore nextChar();
		}
		if (cc)
		    pushBack(cc);
		*tp = '\0';
		token = floatconst;
		tokenFloat = atof(tokenString);
	    } else {		/* nope, wrong again */
		if (cc)
		    pushBack(cc);
		if (sign)
		    pushBack('-');
		pushBack('e');
	    }
	} else if (cc)
	    pushBack(cc);
    }

    else if (cc == '$') {	/* character constant */
	tokenInteger = (int) nextChar();
	token = charconst;
    }

    else if (cc == '#') {	/* symbol */
	tp--;			/* erase pound sign */
	if (nextChar() == '(')
	    token = arraybegin;
	else {
	    pushBack(cc);
	    while (nextChar() && isSymbolChar(cc))
		*tp++ = cc;
	    pushBack(cc);
	    token = symconst;
	}
    }

    else if (cc == '\'') {	/* string constant */
	tp--;			/* erase pound sign */
      strloop:
	while (nextChar() && (cc != '\''))
	    *tp++ = cc;
	/* check for nested quote marks */
	if (cc && nextChar() && (cc == '\'')) {
	    *tp++ = cc;
	    goto strloop;
	}
	pushBack(cc);
	token = strconst;
    }

    else if (isClosing(cc))	/* closing expressions */
	token = closing;

    else if (singleBinary(cc)) {	/* single binary expressions */
	token = binary;
    }

    else {			/* anything else is binary */
	if (nextChar() && binarySecond(cc))
	    *tp++ = cc;
	else
	    pushBack(cc);
	token = binary;
    }

    *tp = '\0';
    return (token);
}
