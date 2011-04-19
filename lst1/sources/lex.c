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
	Little Smalltalk lexical analyzer for driver 
		timothy a. budd 12/84
*/
/*
	The source code for the Little Smalltalk System may be freely
	copied provided that the source of all files is acknowledged
	and that this condition is copied with each file.

	The Little Smalltalk System is distributed without responsibility
	for the performance of the program and without any guarantee of
	maintenance.

	All questions concerning Little Smalltalk should be addressed to:

		Professor Tim Budd
		Department of Computer Science
		Oregon State University
		Corvallis, Oregon
		97331
		USA
*/
# include <stdio.h>
# include <ctype.h>
# include <math.h>
# define DRIVECODE
# include "drive.h"

# define MAXTOKEN 100
char toktext[MAXTOKEN];
tok_type t;
enum lextokens token;

extern char *lexptr;
extern double atof();

static char ocbuf = 0;
static int pbbuf[20];

# define input() (ocbuf ? pbbuf[--ocbuf] : *lexptr++ )
# define putbak(c) (pbbuf[ocbuf++] = c)

static char *psuvars[] = {"nil", "true", "false", "smalltalk", 0};
static enum pseuvars psuval[] = {nilvar, truevar, falsevar, smallvar};
static char symbols[] = "\n-()[]!|.;>" ;
static enum lextokens symval[] = {NL, MINUS, LP, RP, LB, RB, BAR, BAR, 
	PERIOD, SEMI, PE};

static enum lextokens lexsave(type)
enum lextokens type;
{	char *w_search();

	if (! (t.c = w_search(toktext, 1)))
		lexerr("cannot create symbol %s", toktext);
	/* assign token, and return value */
	return(token = type);
}

enum lextokens nextlex() {
	register char c;
	register char *p;
	char *q;
	int  i, n, base;
	double d, denom;

	do {			/* read whitespace (including comments) */
		c = input();
		if (c == '\"') {
			while ((c = input()) && c != '\"') ;
			if (c == '\"') c = input();
			else lexerr("unterminated comment", "");
			}
		} while (c == ' ' || c == '\t') ;

	if (!c) return(token = nothing);

	p = toktext;
	*p = c;
	toktext[1] = '\0';

						/* identifiers and keywords */
	if (( c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		for (*p++ = c; (c = input()) && isalnum(c) ; *p++ = c) ;
		*p = '\0';
		lexsave(0);
		if (c == ':') {
			return(token = KEYWORD);
			}
		else {
			putbak(c);
			if (islower(toktext[0])) {
				for (i = 0; psuvars[i]; i++)
					if (strcmp(toktext, psuvars[i]) == 0) {
						t.p = psuval[i];
						return(token = PSEUDO);
						}
				return(token = LOWERCASEVAR);
				}
			else {
				return(token = UPPERCASEVAR);
				}
			}
		}
	
# define scandigits(x) for(*p++ = c; (c = input()) && isdigit(c) ; *p++ = c) x

	if (c >= '0' && c <= '9') {		/* numbers */
		i = c - '0';
		scandigits( i = 10 * i + (c - '0') );
		if (c == '.' || c == 'e') {
			if (c == '.')
				scandigits();
			if (c == 'e') {
				*p++ = c;
				c = input();
				if (c == '+' || c == '-') {
					*p++ = c; c = input(); }
				scandigits();
				}
			putbak(c);
			*p = '\0';
			t.f = atof(toktext);
			return(token = LITFNUM);
			}
		else if ((c == 'r') && ((i >= 2) && (i <= 36))) {
			base = i;
			i = 0;
			for (*p++ = c; c = input(); *p++ = c) {
				if (isdigit(c)) n = c - '0';
				else if (isupper(c)) n = (c - 'A') + 10;
				else break;
				if (n >= base) break;
				i = base * i + n;
				}
			if (c == '.' || c == 'e') {
				d = (double) i;
				if (c == '.') {
					denom = 1.0 / (double) base;
					for (*p++ = c; c = input(); *p++ = c) {
						if (isdigit(c))
							n = c - '0';
						else if (isupper(c))
							n = (c - 'A') + 10;
						else break;
						if (n >= base) break;
						d += n * denom;
						denom /= base;
						}
					}
				if (c == 'e') {
					*p++ = c;
					c = input();
					if (c == '+' || c == '-') {
						n = c;
						*p++ = c;
						c = input();
						}
					else n = 0;
					i = c - '0';
					scandigits(i = 10 * i + (c - '0'));
					if (n == '-') i = - i;
					d *= pow((double) base, (double) i);
					}
				putbak(c);
				*p = '\0';
				t.f = d;
				return(token = LITFNUM);
				}
			}
		putbak(c);
		*p = '\0';
		t.i = i;
		return(token = LITNUM);
		}

	if (c == '#') {				/* symbol */
		i = 1;
		while (i)
			switch(c = input()) {
				case '\0': case ' ': case '\t': case '\n':
				case '(': case '[': case ')':
					putbak(c);
					i = 0;
					break;
				default:
					*p++ = c;
				}
		if (p == toktext)
			return(token = PS);
		else {
			*p = '\0';
			if ((p - toktext) >= MAXTOKEN) cant_happen(18);
			return(lexsave(LITSYM));
			}
		}

	if (c == '\'') {			/* quoted string */
		do {
			for ( ; (c = input()) && c != '\'' ; *p++ = c) ;
			c = input();
			if (c == '\'') *p++ = '\'';
			} while (c == '\'');
		putbak(c);
		*p = '\0';
		if ((p - toktext) >= MAXTOKEN) cant_happen(18);
		t.c = toktext;
		return(token = LITSTR);
		}

	if (c == ':') {				/* colon or argument name */
		c = input();
		if (c == '=') 
			return(token = ASSIGN);
		else if (isalnum(c)) {
			for (*p++ = c; isalnum(c = input()); *p++ = c );
			putbak(c);
			*p = '\0';
			return(lexsave(COLONVAR));
			}
		putbak(c);
		return(lexsave(BINARY));
		}

	if (c == '<') {			/* assign, less than or primitive */
		*p++ = c; *p = '\0';
		c = input();
		if (c == '-')
			return(token = ASSIGN);
		for (p = q = "primitive"; *p && *p == c; p++)
			c = input();
		putbak(c);
		if (*p) {
			for (p--; p >= q; p--) putbak(*p);
			return(lexsave(BINARY));
			}
		else 
			return(token = PRIMITIVE);
		}

	if (c == '.') {			/* number or period */
		c = input();
		if (c >= '0' && c <= '9') {
			putbak(c);		/* reparse with digit */
			putbak('.');		/* inserted on front */
			putbak('0');		/* so it looks like */
			return(nextlex());	/* a number */
			}
		putbak(c);
		return(token = PERIOD);
		}

	if (c == '\\') {		/* binary or hidden newline */
		c = input();
		if (c == '\n')
			return(nextlex());
		putbak(c);
		return(lexsave(BINARY));
		}

	if (c == '$') {			/* literal character or binary */
		c = input();
		if (c) {
			t.i = c;
			return(token = LITCHAR);
			}
		return(lexsave(BINARY));
		}

	for (i = 0; symbols[i]; i++)
		if (c == symbols[i])
			return(lexsave(symval[i]));

	return(lexsave(BINARY));
}
