/*
 Little Smalltalk, version 3
 Main Driver
 written By Tim Budd, September 1988
 Oregon State University
 */

#include <stdio.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "tty.h"
#include "interp.h"
#include "st.h"
#include "news.h"
#include "parser.h"

static void makeInitialImage();

int initial = 0; /* not making initial image */

int main(int argc, char **argv)
{
    FILE *fp;
    object firstProcess;
    char *p, buffer[120];

    initMemoryManager();

    strcpy(buffer, "systemImage");
    p = buffer;

    if (argc <= 2)
    {
        if (argc != 1) p = argv[1];

        fp = fopen(p, "rb");

        if (fp == NULL)
        {
            sysError("cannot open image", p);
            exit(1);
        }
        imageRead(fp);
        initCommonSymbols();

        firstProcess = nameTableLookup(symbols,"systemProcess");
        if (firstProcess == nilobj)
        {
            sysError("no initial process", "in image");
            exit(1);
            return 1;
        }

        /* execute the main system process loop repeatedly */
        /*debugging = true;*/

        /* not using windowing interface, safe to print out message */
        printf("Little Smalltalk Improved, Version 0.01\n");

        while (execute(firstProcess, 15000))
            ;
    }
    else     // We have been called for image creation
    {
        char methbuf[100];
        int i;

        initMemoryManager();

        makeInitialImage();

        initCommonSymbols();

        for (i = 1; i < argc; i++)
        {
            fprintf(stderr, "%s:\n", argv[i]);
            sprintf(methbuf, "x <120 1 '%s' 'r'>. <123 1>. <121 1>", argv[i]);
            goDoIt(methbuf);
        }

        /* when we are all done looking at the arguments, do initialization */
        fprintf(stderr, "initialization\n");
        /*debugging = true;*/
        goDoIt("x nil initialize\n");
        fprintf(stderr, "finished\n");

        /* exit and return - belt and suspenders, but it keeps lint happy */
    }
    return 0;
}

static void goDoIt(const char *text)
{
    object process, stack, method;

    method = newMethod();
    incr(method);
    setInstanceVariables(nilobj);
    parse(method, text, false);

    process = allocObject(processSize);
    incr(process);
    stack = allocObject(50);
    incr(stack);

    /* make a process */
    basicAtPut(process, stackInProcess, stack);
    basicAtPut(process, stackTopInProcess, newInteger(10));
    basicAtPut(process, linkPtrInProcess, newInteger(2));

    /* put argument on stack */
    basicAtPut(stack, 1, nilobj); /* argument */
    /* now make a linkage area in stack */
    basicAtPut(stack, 2, nilobj); /* previous link */
    basicAtPut(stack, 3, nilobj); /* context object (nil = stack) */
    basicAtPut(stack, 4, newInteger(1)); /* return point */
    basicAtPut(stack, 5, method); /* method */
    basicAtPut(stack, 6, newInteger(1)); /* byte offset */

    /* now go execute it */
    while (execute(process, 15000))
        fprintf(stderr, "..");
}

/*
 there is a sort of chicken and egg problem with regards to making
 the initial image
 */
static void makeInitialImage()
{
    object hashTable;
    object symbolObj, symbolClass, classClass;

    /* first create the table, without class links */
    symbols = allocObject(1);
    incr(symbols);
    hashTable = allocObject(3 * 53);
    basicAtPut(symbols, 1, hashTable);

    /* next create #Symbol, Symbol and Class */
    symbolObj = newSymbol("Symbol");
    symbolClass = newClass("Symbol");
    setClass(symbolObj, symbolClass);
    classClass = newClass("Class");
    setClass(symbolClass, classClass);
    setClass(classClass, classClass);

    /* now fix up classes for symbol table */
    /* and make a couple common classes, just to hold their places */
    newClass("Link");
    newClass("ByteArray");
    setClass(hashTable, newClass("Array"));
    setClass(symbols, newClass("Dictionary"));
    setClass(nilobj, newClass("UndefinedObject"));
    newClass("String");
    nameTableInsert(symbols, strHash("symbols"), newSymbol("symbols"), symbols);

    /* finally at least make true and false to be distinct */
    trueobj = newSymbol("true");
    nameTableInsert(symbols, strHash("true"), trueobj, trueobj);
    falseobj = newSymbol("false");
    nameTableInsert(symbols, strHash("false"), falseobj, falseobj);
}
