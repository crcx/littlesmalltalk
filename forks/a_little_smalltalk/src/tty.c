/*
 Little Smalltalk, version 3
 Written by Tim Budd, January 1989

 tty interface routines
 this is used by those systems that have a bare tty interface
 systems using another interface, such as the stdwin interface
 will replace this file with another.
 */

#include <stdio.h>
#include <stdlib.h>
#include "env.h"
#include "memory.h"
#include "tty.h"

extern boolean parseok;

/* report a fatal system error */
void sysError(const char *s1, const char *s2)
{
    fprintf(stderr, "%s\n%s\n", s1, s2);
    abort();
}

/* report a nonfatal system error */
void sysWarn(const char *s1, const char *s2)
{
    fprintf(stderr, "%s\n%s\n", s1, s2);
}

void compilWarn(const char *selector, const char *str1, const char *str2)
{
    fprintf(stderr, "compiler warning: Method %s : %s %s\n", selector,
            str1, str2);
}

void compilError(const char *selector, const char *str1, const char *str2)
{
    fprintf(stderr, "compiler error: Method %s : %s %s\n", selector,
            str1, str2);
    parseok = false;
}

void dspMethod(const char *cp, const char *mp)
{
    /* fprintf(stderr,"%s %s\n", cp, mp);*/
}

void givepause()
{
    char buffer[80];

    fprintf(stderr, "push return to continue\n");
    fgets(buffer, 79, stdin);
}

object sysPrimitive(int number, object *arguments)
{
    object returnedObject;

    /* someday there will be more here */
    switch (number - 150)
    {
    case 0: /* do a system() call */
        returnedObject = newInteger(system(
                                        charPtr(arguments[0])));
        break;

    default:
        sysError("unknown primitive", "sysPrimitive");
    }
    return (returnedObject);
}
