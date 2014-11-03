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

#include <stdio.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "tty.h"
#include "news.h"
#include "interp.h"

void nameTableInsert(object dict, int hash, object key, object value)
{
    object table, link, nwLink, nextLink, tablentry;

    /* first get the hash table */
    table = basicAt(dict, 1);

    if (sizeField(table) < 3)
        sysError("attempt to insert into", "too small name table");
    else
    {
        hash = 3 * (hash % (sizeField(table) / 3));
        tablentry = basicAt(table, hash+1);
        if ((tablentry == nilobj) || (tablentry == key))
        {
            basicAtPut(table, hash+1, key);
            basicAtPut(table, hash+2, value);
        }
        else
        {
            nwLink = newLink(key, value);
            incr(nwLink);
            link = basicAt(table, hash+3);
            if (link == nilobj)
            {
                basicAtPut(table, hash+3, nwLink);
            }
            else
                while (1)
                    if (basicAt(link,1) == key)
                    {
                        basicAtPut(link, 2, value);
                        break;
                    }
                    else if ((nextLink = basicAt(link, 3)) == nilobj)
                    {
                        basicAtPut(link, 3, nwLink);
                        break;
                    }
                    else
                        link = nextLink;
            decr(nwLink);
        }
    }
}

object hashEachElement(object dict, register int hash, int(*fun)(object))
{
    object table, key, value, link;
    register object *hp;
    int tablesize;

    table = basicAt(dict, 1);

    /* now see if table is valid */
    if ((tablesize = sizeField(table)) < 3)
        sysError("system error", "lookup on null table");
    else
    {
        hash = 1 + (3 * (hash % (tablesize / 3)));
        hp = sysMemPtr(table) + (hash - 1);
        key = *hp++; /* table at: hash */
        value = *hp++; /* table at: hash + 1 */
        if ((key != nilobj) && (*fun)(key))
            return value;
        for (link = *hp; link != nilobj; link = *hp)
        {
            hp = sysMemPtr(link);
            key = *hp++; /* link at: 1 */
            value = *hp++; /* link at: 2 */
            if ((key != nilobj) && (*fun)(key))
                return value;
        }
    }
    return nilobj;
}

/* compute hash value of string ---- strHash */
int strHash(const char *str)
{
    register int hash;
    register const char *p;

    hash = 0;
    for (p = str; *p; p++)
        hash += *p;
    if (hash < 0)
        hash = -hash;
    /* make sure it can be a smalltalk integer */
    if (hash > 16384)
        hash >>= 2;
    return hash;
}

static object objBuffer;
static const char *charBuffer;

/* test for string equality ---- strTest */
static int strTest(object key)
{
    if (charPtr(key) && (strcmp(charPtr(key), charBuffer)==0))
    {
        objBuffer = key;
        return 1;
    }
    return 0;
}

/* return key associated with global symbol */
object globalKey(const char *str)
{
    objBuffer = nilobj;
    charBuffer = str;
    hashEachElement(symbols, strHash(str), strTest);
    return objBuffer;
}

object nameTableLookup(object dict, const char *str)
{
    charBuffer = str;
    return hashEachElement(dict, strHash(str), strTest);
}

object unSyms[12];
object binSyms[30];

const char *unStrs[] = { "isNil", "notNil", "value", "new", "class", "size",
                         "basicSize", "print", "printString", 0
                       };

const char *binStrs[] = { "+", "-", "<", ">", "<=", ">=", "=", "~=", "*", "quo:",
                          "rem:", "bitAnd:", "bitXor:", "==", ",", "at:", "basicAt:", "do:",
                          "coerce:", "error:", "includesKey:", "isMemberOf:", "new:", "to:",
                          "value:", "whileTrue:", "addFirst:", "addLast:", 0
                        };

/* initialize common symbols used by the parser and interpreter */
void initCommonSymbols()
{
    int i;

    trueobj = nameTableLookup(symbols,"true");
    falseobj = nameTableLookup(symbols,"false");
    for (i = 0; unStrs[i]; i++)
        unSyms[i] = newSymbol(unStrs[i]);
    for (i = 0; binStrs[i]; i++)
        binSyms[i] = newSymbol(binStrs[i]);
}
