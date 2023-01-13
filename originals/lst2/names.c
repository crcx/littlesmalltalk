/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	Name Table module

	A name table is the term used for a Dictionary indexed by symbols.
	There are two name tables used internally by the bytecode interpreter.
	The first is the table, contained in the variable globalNames,
	that contains the names and values of all globally accessible 
	identifiers.  The second is the table of methods associated with
	every class.  Notice that in neither of these cases does the
	system ever put anything INTO the tables, thus there are only
	routines here for reading FROM tables.

	(putting things INTO the table is all done in Smalltalk code,
	using the methods from class Dictionary)

	One complication of instances of class Symbol is that all
	symbols must be unique, not only so that == will work as expected,
	but so that memory does not get overly clogged up with symbols.
	Thus all symbols are kept in a hash table, and when new symbols
	are created (via newSymbol(), below) they are inserted into this
	table, if not already there.

	This module also manages the definition of various symbols that are
	given fixed values for efficiency sake.  These include the objects
	nil, true, false, and various classes.
*/

# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"

/* global variables used to avoid repeated examinations of the global symbol table */
object trueobj = nilobj;	/* the pseudo variable true */
object falseobj = nilobj;	/* the pseudo variable false */
object smallobj = nilobj;	/* the pseudo variable smalltalk */
object arrayclass = nilobj;	/* the class ``Array'' */
object blockclass = nilobj;	/* the class ``Block'' */
object contextclass = nilobj;	/* the class ``Context'' */
object intclass = nilobj;	/* the class ``Integer'' */
object intrclass = nilobj;	/* the class ``Interpreter'' */
object symbolclass = nilobj;	/* the class ``Symbol'' */
object stringclass = nilobj;	/* the class ``String'' */

/*
	some messages are encoded in concise bytecode format -
to reduce the size of the compiled methods
(also, in some cases, to more efficiently detect special cases
handled in the interpreter, rather than by methods)
*/

char *binStrs[] = {"+", "-", "<", ">", "<=", ">=", "=", "~=", "*", 
"quo:", "rem:", "bitAnd:", "bitXor:", 
"==", ",", "at:", "basicAt:", "do:", "coerce:", "error:", "includesKey:",
"isMemberOf:", "new:", "to:", "value:", "whileTrue:", "addFirst:", "addLast:",
0};

object binSyms[28];

char *unStrs[] = {"isNil", "notNil", "new", "value", "class", "size",
"basicSize", "print", "printString", 0};

object unSyms[9];

char *keyStrs[] = {"at:ifAbsent:", "at:put:", "basicAt:put:", "between:and:",
0};

object keySyms[4];

object nameTableLookup(table, symbol)
object table, symbol;
{	int hash, tablesize;
	object link;

	if ((tablesize = objectSize(table)) == 0)
		sysError("system error","lookup on null table");
	else {
		hash = 3 * ( symbol % (tablesize / 3));
		if (basicAt(table, hash+1) == symbol)
			return(basicAt(table, hash+2));

		/* otherwise look along the chain of links */
		for (link=basicAt(table, hash+3); link != nilobj; 
					link=basicAt(link, 3))
			if (basicAt(link, 1) == symbol)
				return(basicAt(link, 2));

	}
	return (nilobj);
}

getClass(obj)
object obj;
{
	if (isInteger(obj))
		return(intclass);
	return (classField(obj));
}

static object globalGet(name)
char *name;
{	object newobj;

	newobj = globalSymbol(name);
	if (newobj == nilobj)
		sysError("symbol not found in image", name);
	return(newobj);
}

initCommonSymbols()
{	int i;

	trueobj = globalGet("true");
	falseobj = globalGet("false");
	smallobj  = globalGet("smalltalk");
	arrayclass = globalGet("Array");
	blockclass = globalGet("Block");
	contextclass = globalGet("Context");
	intclass = globalGet("Integer");
	symbolclass = globalGet("Symbol");
	stringclass = globalGet("String");
	/* interpreter may or may not be there */
	intrclass = globalSymbol("Interpreter");

	for (i = 0; i < 28; i++)
		binSyms[i] = newSymbol(binStrs[i]);

	for (i = 0; i < 9; i++)
		unSyms[i] = newSymbol(unStrs[i]);

	for (i = 0; i < 4; i++)
		keySyms[i] = newSymbol(keyStrs[i]);
}

object newArray(size)
int size;
{	object newobj;

	newobj = allocObject(size);
	setClass(newobj, arrayclass);
	return(newobj);
}

object newSymbol(str)
char *str;
{	int hash;
	object newSym, link;
	char *p;

	/* first compute hash value of string text */
	/* this is duplicated in image.c - make sure any changes match there */
	hash = 0;
	for (p = str; *p; p++)
		hash += *p;
	if (hash < 0) hash = - hash;
	hash = 2 * ( hash % (objectSize(symbols) / 2));

	/* next look to see if it is in symbols - note that this
	   text duplicates that found in nameTableLookup, only using
	   string comparison instead of symbol comparison */
	newSym = basicAt(symbols, hash+1);
	if (streq(str, charPtr(newSym)))
		return(newSym);

	/* not in table, look along links */
	for (link=basicAt(symbols, hash+2); link != nilobj; link=basicAt(link,2)) {
		newSym = basicAt(link, 1);
		if (streq(str, charPtr(newSym)))
			return(newSym);
		}

	/* not found, make a new symbol */
	newSym = allocSymbol(str);
	setClass(newSym, symbolclass);

	/* now insert new symbol in table, so next time we will find it */
	if (basicAt(symbols, hash+1) == nilobj)
		basicAtPut(symbols, hash+1, newSym);
	else {		/* insert along links */
		link = allocObject(2);
		basicAtPut(link, 1, newSym);
		basicAtPut(link, 2, basicAt(symbols, hash+2));
		basicAtPut(symbols, hash+2, link);
		}

	return(newSym);
}

object newStString(value)
char *value;
{	object newobj;

	newobj = allocSymbol(value);
	setClass(newobj, stringclass);
	return(newobj);
}

object newFloat(d)
double d;
{	object newobj;

	newobj = allocFloat(d);
	setClass(newobj, globalSymbol("Float"));
	return(newobj);
}
