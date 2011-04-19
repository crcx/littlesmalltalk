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
 *************************************************************************
 * symbol creation - symbols are never deleted once created
 * timothy a. budd, 10/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

/*
	only one copy of symbol values are kept.
	multiple copies of the same symbol point to the same
	location.
	sy_search will find, and if necessary insert, a string into
	this common table 
*/

extern char x_str[];		/* initialized common string table */
extern symbol *x_tab[];		/* initialized common symbols table */
extern int x_tmax;		/* top of symbols table */
extern char *walloc();		/* routine to allocate a new word */
int ca_sym = 0;			/* symbol allocation counter */

/* sy_search performs a binary search of a symbol, is the main interface to
the symbols routines */
symbol *sy_search(word, insert)
char *word;
int  insert;
{	register int i;
	register int j;
	register int k;
	char *p;
	symbol *new_y();

	for (i=1; i <= x_tmax; i <<= 1);
	for (i >>= 1, j = i >>1, i--; ; j >>= 1) {
		p = symbol_value(x_tab[i]);
		if (word == p) return(x_tab[i]);
		k = *word - *p;
		if (!k) k = *(word+1) - *(p+1);
		if (!k) k = strcmp(word, p);
		if (!k)
			return(x_tab[i]);
		if (!j) break;
		if (k < 0) i -= j;
		else {
			if ((i += j) > x_tmax) i = x_tmax;
			}
		}
	if (insert) {
		if (k > 0) i++;
		if ((k = ++x_tmax) >= SYMTABMAX)
			cant_happen(12);
		for (; k > i; k--) {
			x_tab[k] = x_tab[k-1];
			}
		/*fprintf(stderr,"adding %s\n", word);*/
		x_tab[i] = new_y(walloc(word));
		x_tab[i]->y_ref_count++; /* make sure not freed */
		return(x_tab[i]);
		}
	else return((symbol *) 0);
}

/* w_search performs a search for a word, not a symbol */
char *w_search(word, insert)
char *word;
int insert;
{	symbol *sym;

	sym = sy_search(word, insert);
	if (sym)
		return(symbol_value(sym));
	else
		return((char *) 0);
}

/*---------------------------------------*/

static mstruct *fr_symbol = 0;		/* symbols free list */
static symbol strspace[SYMINITSIZE];	/* initial symbols free list */

extern object *o_object;		/* common instance of Object */
extern class *ArrayedCollection;

/* sym_init - initialize the symbols routine */
sym_init() {
	int  i;
	symbol *p;
	mstruct   *new;

	p = strspace;
	for (i = 0; i < SYMINITSIZE; i++) {
		new = (mstruct *) p;
		new->mlink = fr_symbol;
		fr_symbol = new;
		p++;
		}
}

/* new_y is the internal routine for making new symbols */
symbol *new_y(text)
char *text;
{	symbol *new;

	if (fr_symbol) {
		new = (symbol *) fr_symbol;
		fr_symbol = fr_symbol->mlink;
		}
	else {
		ca_sym++;
		new = structalloc(symbol);
		}

	new->y_ref_count = 0;
	new->y_size = SYMBOLSIZE;
	new->y_value = text;
	return(new);
}
