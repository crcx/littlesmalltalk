/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	routines used in the making of the initial object image
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "env.h"
#include "memory.h"
#include "names.h"
#include "lex.h"

extern void setInstanceVariables();
extern boolean parse();
extern void lexinit();
extern void initCommonSymbols();

#define SymbolTableSize 71
#define GlobalNameTableSize 53
#define MethodTableSize 39

#define globalNameSet(sym, value) nameTableInsert(globalNames, sym, value)
/*
	the following classes are used repeately, so we put them in globals.
*/
static object ObjectClass;
static object ClassClass;
static object LinkClass;
static object DictionaryClass;
static object ArrayClass;

/*
	we read the input a line at a time, putting lines into the following
	buffer.  In addition, all methods must also fit into this buffer.
*/
#define TextBufferSize 1024
char textBuffer[TextBufferSize];

/*
	nameTableInsert is used to insert a symbol into a name table.
	see names.c for futher information on name tables
*/
void nameTableInsert(table, symbol, value)
object table, symbol, value;
{	object link, newLink, nextLink, entry;
	int hash;

	if (objectSize(table) < 3)
		sysError("attempt to insert into","too small name table");
	else {
		hash = 3 * ( symbol % (objectSize(table) / 3));
		entry = basicAt(table, hash+1);
		if ((entry == nilobj) || (entry == symbol)) {
			basicAtPut(table, hash+1, symbol);
			basicAtPut(table, hash+2, value);
			}
		else {
			newLink = allocObject(3);
			incr(newLink);
			setClass(newLink, globalSymbol("Link"));
			basicAtPut(newLink, 1, symbol);
			basicAtPut(newLink, 2, value);
			link = basicAt(table, hash+3);
			if (link == nilobj)
				basicAtPut(table, hash+3, newLink);
			else
				while(1)
					if (basicAt(link,1) == symbol) {
						basicAtPut(link, 2, value);
						break;
						}
					else if ((nextLink = basicAt(link, 3)) == nilobj) {
						basicAtPut(link, 3, newLink);
						break;
						}
					else
						link = nextLink;
			decr(newLink);
			}
	}
}

/*
	there is sort of a chicken and egg problem about building the 
	first classes.
	in order to do it, you need symbols, 
	but in order to make symbols, you need the class Symbol.
	the routines makeClass and buildInitialNameTable attempt to get 
	carefully get around this initialization problem
*/

object makeClass(name)
char *name;
{	object theClass, theSymbol;

	/* this can only be called once newSymbol works properly */

	theClass = allocObject(classSize);
	theSymbol = newSymbol(name);
	basicAtPut(theClass, nameInClass, theSymbol);
	globalNameSet(theSymbol, theClass);
	setClass(theClass, ClassClass);

	return(theClass);
}

void buildInitialNameTables()
{	object symbolString, classString;
	int hash;
	char *p;

	/* build the table that contains all symbols */
	symbols = allocObject(2 * SymbolTableSize);
	incr(symbols);
	/* build the table that contains all global names */
	globalNames = allocObject(3 * GlobalNameTableSize);
	incr(globalNames);

	/* next create class Symbol, so we can call newSymbol */
	/* notice newSymbol uses the global variable symbolclass */
	symbolString = allocSymbol("Symbol");
	symbolclass =  allocObject(classSize);
	setClass(symbolString, symbolclass);
	basicAtPut(symbolclass, nameInClass, symbolString);
	/* we recreate the hash computation used by newSymbol */
	hash = 0;
	for (p = "Symbol"; *p; p++)
		hash += *p;
	if (hash < 0) hash = - hash;
	hash %= (objectSize(symbols) / 2);
	basicAtPut(symbols, 2*hash + 1, symbolString);
	globalNameSet(symbolString, symbolclass);
	/* now the routine newSymbol should work properly */

	/* now go on to make class Class so we can call makeClass */
	ClassClass = allocObject(classSize);
	classString = newSymbol("Class");
	basicAtPut(ClassClass, nameInClass, classString);
	globalNameSet(classString, ClassClass);
	setClass(ClassClass, ClassClass);
	setClass(symbolclass, ClassClass);

	/* now create a few other important classes */
	ObjectClass = makeClass("Object");
	LinkClass = makeClass("Link");
	setClass(nilobj, makeClass("UndefinedObject"));
	DictionaryClass = makeClass("Dictionary");
	ArrayClass = makeClass("Array");
	setClass(symbols, DictionaryClass);
	setClass(globalNames, DictionaryClass);
	
	globalNameSet(newSymbol("globalNames"), globalNames);
}

/*
	findClass gets a class object,
	either by finding it already or making it
	in addition, it makes sure it has a size, by setting
	the size to zero if it is nil.
*/
object findClass(name)
char *name;
{	object newobj;

	newobj = globalSymbol(name);
	if (newobj == nilobj)
		newobj = makeClass(name);
	if (basicAt(newobj, sizeInClass) == nilobj)
		basicAtPut(newobj, sizeInClass, newInteger(0));
	return(newobj);
}

/*
	readDeclaration reads a declaration of a class
*/
void readDeclaration()
{	object classObj, super, vars;
	int i, size, instanceTop;
	object instanceVariables[15];

	if (nextToken() != name)
		sysError("bad file format","no name in declaration");
	classObj = findClass(tokenString);
	size = 0;
	if (nextToken() == name) {	/* read superclass name */
		super = findClass(tokenString);
		basicAtPut(classObj, superClassInClass, super);
		size = intValue(basicAt(super, sizeInClass));
		ignore nextToken();
		}
	if (token == name) {		/* read instance var names */
		instanceTop = 0;
		while (token == name) {
			instanceVariables[instanceTop++] = newSymbol(tokenString);
			size++;
			ignore nextToken();
			}
		vars = newArray(instanceTop);
		for (i = 0; i < instanceTop; i++)
			basicAtPut(vars, i+1, instanceVariables[i]);
		basicAtPut(classObj, variablesInClass, vars);
		}
	basicAtPut(classObj, sizeInClass, newInteger(size));
}

/*
	readInstance - read an instance directive 
*/
void readInstance()
{	object classObj, newObj;
	int size;

	if (nextToken() != name)
		sysError("no name","following instance command");
	classObj = globalSymbol(tokenString);
	if (nextToken() != name)
		sysError("no instance name","in instance command");

	/* now make a new instance of the class -
		note that we can't do any initialization */
	size = intValue(basicAt(classObj, sizeInClass));
	newObj = allocObject(size);
	setClass(newObj, classObj);
	globalNameSet(newSymbol(tokenString), newObj);
}

/*
	readClass reads a class method description
*/
void readClass(fd)
FILE *fd;
{	object classObj, methTable, theMethod, selector;
	char *eoftest, lineBuffer[80];

	/* if we haven't done it already, read symbols now */
	if (trueobj == nilobj)
		initCommonSymbols();

	if (nextToken() != name)
		sysError("missing name","following Class keyword");
	classObj = findClass(tokenString);
	setInstanceVariables(classObj);
fprintf(stderr,"class %s\n", charPtr(basicAt(classObj, nameInClass)));

	/* find or create a methods table */
	methTable = basicAt(classObj, methodsInClass);
	if (methTable == nilobj) {
		methTable = allocObject(MethodTableSize);
		setClass(methTable, globalSymbol("Dictionary"));
		basicAtPut(classObj, methodsInClass, methTable);
		}

	/* now go read the methods */
	do {
		textBuffer[0] = '\0';
		while((eoftest = fgets(lineBuffer, 80, fd)) != NULL) {
			if ((lineBuffer[0] == '|') || (lineBuffer[0] == ']'))
				break;
			ignore strcat(textBuffer, lineBuffer);
			}
		if (eoftest == NULL) {
			sysError("unexpected end of file","while reading method");
			break;
			}
		/* now we have a method */
		theMethod = allocObject(methodSize);
		setClass(theMethod, globalSymbol("Method"));
		if (parse(theMethod, textBuffer)) {
			selector = basicAt(theMethod, messageInMethod);
fprintf(stderr,"method %s\n", charPtr(selector));
			nameTableInsert(methTable, selector, theMethod);
			}
		else {
			/* get rid of unwanted method */
			incr(theMethod);
			decr(theMethod);
fprintf(stderr,"push return to continue\n");
fgets(textBuffer, 1024, stdin);
			}
		
	} while (lineBuffer[0] != ']');
}

/*
	readFile reads a class descriptions file
*/
void readFile(fd)
FILE *fd;
{
	while(fgets(textBuffer, TextBufferSize, fd) != NULL) {
		lexinit(textBuffer);
		if (token == inputend)
			; /* do nothing, get next line */
		else if ((token == binary) && streq(tokenString, "*"))
			; /* do nothing, its a comment */
		else if ((token == name) && streq(tokenString, "Declare"))
			readDeclaration();
		else if ((token == name) && streq(tokenString,"Instance"))
			readInstance();
		else if ((token == name) && streq(tokenString,"Class"))
			readClass(fd);
		else 
			printf("unknown line %s\n", textBuffer);
		}
}
