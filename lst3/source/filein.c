/*
	Little Smalltalk, version 3
	Written by Tim Budd, Oregon State University, June 1988

	routines used in reading in textual descriptions of classes
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "lex.h"

void setInstanceVariables(object aClass);
boolean parse(object method, char *text, boolean savetext);
void sysDecr(object z);
void givepause();


#define MethodTableSize 39

/*
	the following are switch settings, with default values
*/
boolean savetext = true;

/*
	we read the input a line at a time, putting lines into the following
	buffer.  In addition, all methods must also fit into this buffer.
*/
#define TextBufferSize 1024
static char textBuffer[TextBufferSize];

/*
	findClass gets a class object,
	either by finding it already or making it
	in addition, it makes sure it has a size, by setting
	the size to zero if it is nil.
*/
static object findClass(name)
char *name;
{
    object newobj;

    newobj = globalSymbol(name);
    if (newobj == nilobj)
	newobj = newClass(name);
    if (basicAt(newobj, sizeInClass) == nilobj) {
	basicAtPut(newobj, sizeInClass, newInteger(0));
    }
    return newobj;
}

/*
	readDeclaration reads a declaration of a class
*/
static void readClassDeclaration()
{
    object classObj, super, vars;
    int i, size, instanceTop;
    object instanceVariables[15];

    if (nextToken() != nameconst)
	sysError("bad file format", "no name in declaration");
    classObj = findClass(tokenString);
    size = 0;
    if (nextToken() == nameconst) {	/* read superclass name */
	super = findClass(tokenString);
	basicAtPut(classObj, superClassInClass, super);
	size = intValue(basicAt(super, sizeInClass));
	ignore nextToken();
    }
    if (token == nameconst) {	/* read instance var names */
	instanceTop = 0;
	while (token == nameconst) {
	    instanceVariables[instanceTop++] = newSymbol(tokenString);
	    size++;
	    ignore nextToken();
	}
	vars = newArray(instanceTop);
	for (i = 0; i < instanceTop; i++) {
	    basicAtPut(vars, i + 1, instanceVariables[i]);
	}
	basicAtPut(classObj, variablesInClass, vars);
    }
    basicAtPut(classObj, sizeInClass, newInteger(size));
}

/*
	readClass reads a class method description
*/
static void readMethods(fd, printit)
FILE *fd;
boolean printit;
{
    object classObj, methTable, theMethod, selector;
#define LINEBUFFERSIZE 512
    char *cp, *eoftest, lineBuffer[LINEBUFFERSIZE];

    if (nextToken() != nameconst)
	sysError("missing name", "following Method keyword");
    classObj = findClass(tokenString);
    setInstanceVariables(classObj);
    if (printit)
	cp = charPtr(basicAt(classObj, nameInClass));

    /* now find or create a method table */
    methTable = basicAt(classObj, methodsInClass);
    if (methTable == nilobj) {	/* must make */
	methTable = newDictionary(MethodTableSize);
	basicAtPut(classObj, methodsInClass, methTable);
    }

    /* now go read the methods */
    do {
	if (lineBuffer[0] == '|')	/* get any left over text */
	    strcpy(textBuffer, &lineBuffer[1]);
	else
	    textBuffer[0] = '\0';
	while ((eoftest = fgets(lineBuffer, LINEBUFFERSIZE, fd)) != NULL) {
	    if ((lineBuffer[0] == '|') || (lineBuffer[0] == ']'))
		break;
	    ignore strcat(textBuffer, lineBuffer);
	}
	if (eoftest == NULL) {
	    sysError("unexpected end of file", "while reading method");
	    break;
	}

	/* now we have a method */
	theMethod = newMethod();
	if (parse(theMethod, textBuffer, savetext)) {
	    selector = basicAt(theMethod, messageInMethod);
	    basicAtPut(theMethod, methodClassInMethod, classObj);
	    if (printit)
		dspMethod(cp, charPtr(selector));
	    nameTableInsert(methTable, (int) selector,
			    selector, theMethod);
	} else {
	    /* get rid of unwanted method */
	    incr(theMethod);
	    decr(theMethod);
	    givepause();
	}

    } while (lineBuffer[0] != ']');
}

/*
	fileIn reads in a module definition
*/
void fileIn(FILE * fd, boolean printit)
{
    while (fgets(textBuffer, TextBufferSize, fd) != NULL) {
	lexinit(textBuffer);
	if (token == inputend);	/* do nothing, get next line */
	else if ((token == binary) && streq(tokenString, "*"));	/* do nothing, its a comment */
	else if ((token == nameconst) && streq(tokenString, "Class"))
	    readClassDeclaration();
	else if ((token == nameconst) && streq(tokenString, "Methods"))
	    readMethods(fd, printit);
	else
	    sysError("unrecognized line", textBuffer);
    }
}
