/*
 Little Smalltalk, version 3
 Written by Tim Budd, Oregon State University, June 1988

 routines used in reading in textual descriptions of classes
 */

#include <stdio.h>
#include <string.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "lex.h"
#include <string.h>
#include "tty.h"
#include "news.h"
#include "parser.h"

#define MethodTableSize 39

/*
 the following are switch settings, with default values
 */
boolean savetext = true;

/*
 we read the input a line at a time, putting lines into the following
 buffer.  In addition, all methods must also fit into this buffer.
 */
# define TextBufferSize 1024
static char textBuffer[TextBufferSize];

/*
 findClass gets a class object,
 either by finding it already or making it
 in addition, it makes sure it has a size, by setting
 the size to zero if it is nil.
 */
static object findClass(char *name)
{
    object newobj;

    newobj = nameTableLookup(symbols,name);
    if (newobj == nilobj)
        newobj = newClass(name);
    if (basicAt(newobj, sizeInClass) == nilobj)
    {
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
    if (nextToken() == nameconst)   /* read superclass name */
    {
        super = findClass(tokenString);
        basicAtPut(classObj, superClassInClass, super);
        size = intValue(basicAt(super, sizeInClass));
        nextToken();
    }
    if (token == nameconst)   /* read instance var names */
    {
        instanceTop = 0;
        while (token == nameconst)
        {
            instanceVariables[instanceTop++] = newSymbol(tokenString);
            size++;
            nextToken();
        }
        vars = newArray(instanceTop);
        for (i = 0; i < instanceTop; i++)
        {
            basicAtPut(vars, i+1, instanceVariables[i]);
        }
        basicAtPut(classObj, variablesInClass, vars);
    }
    basicAtPut(classObj, sizeInClass, newInteger(size));
}

/*
 readClass reads a class method description
 */
static void readMethods(FILE *fd, boolean printit)
{
    object classObj, methTable, theMethod, selector;
# define LINEBUFFERSIZE 512
    char *cp, *eoftest, lineBuffer[LINEBUFFERSIZE];

    if (nextToken() != nameconst)
        sysError("missing name", "following Method keyword");
    classObj = findClass(tokenString);
    setInstanceVariables(classObj);
    if (printit)
        cp = charPtr(basicAt(classObj, nameInClass));

    /* now find or create a method table */
    methTable = basicAt(classObj, methodsInClass);
    if (methTable == nilobj)   /* must make */
    {
        methTable = newDictionary(MethodTableSize);
        basicAtPut(classObj, methodsInClass, methTable);
    }

    /* now go read the methods */
    do
    {
        if (lineBuffer[0] == '|') /* get any left over text */
            strcpy(textBuffer, &lineBuffer[1]);
        else
            textBuffer[0] = '\0';
        while ((eoftest = fgets(lineBuffer, LINEBUFFERSIZE, fd)) != NULL)
        {
            if ((lineBuffer[0] == '|') || (lineBuffer[0] == ']'))
                break;
            strcat(textBuffer, lineBuffer);
        }
        if (eoftest == NULL)
        {
            sysError("unexpected end of file", "while reading method");
            break;
        }

        /* now we have a method */
        theMethod = newMethod();
        if (parse(theMethod, textBuffer, savetext))
        {
            selector = basicAt(theMethod, messageInMethod);
            basicAtPut(theMethod, methodClassInMethod, classObj);
            if (printit)
                dspMethod(cp, charPtr(selector));
            nameTableInsert(methTable, (int) selector, selector, theMethod);
        }
        else
        {
            /* get rid of unwanted method */
            incr(theMethod);
            decr(theMethod);
            givepause();
        }

    }
    while (lineBuffer[0] != ']');
}

/*
 fileIn reads in a module definition
 */
void fileIn(FILE *fd, boolean printit)
{
    while (fgets(textBuffer, TextBufferSize, fd) != NULL)
    {
        lexinit(textBuffer);
        if (token == inputend)
            ; /* do nothing, get next line */
        else if ((token == binary) && (strcmp(tokenString, "\"")==0))
            ; /* do nothing, its a comment */
        /* Syntax for Class and Methods definitions
         
         Number extend [
           radiusToArea [
             ^self squared * Float pi
           ]
           radiusToCircumference 
             ^self * 2 * Float pi
           ]
         ]
        */
        else if ((token == nameconst) && (strcmp(tokenString,"Class")==0))
            readClassDeclaration();
        else if ((token == nameconst) && (strcmp(tokenString,"Methods")==0))
            readMethods(fd, printit);
        else
            sysError("unrecognized line", textBuffer);
    }
}
