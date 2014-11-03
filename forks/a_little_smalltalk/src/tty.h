/*
 Little Smalltalk, version 3
 Written by Tim Budd, January 1989

 tty interface routines
 this is used by those systems that have a bare tty interface
 systems using another interface, such as the stdwin interface
 will replace this file with another.
 */

#ifndef TTY_H
#define	TTY_H

#include <stdio.h>
#include <stdlib.h>
#include "env.h"
#include "memory.h"
#include <string.h>

extern boolean parseok;

/* report a fatal system error */
void sysError(const char *s1, const char *s2);

/* report a nonfatal system error */
void sysWarn(const char *s1, const char *s2);
void compilWarn(const char *selector, const char *str1, const char *str2);
void compilError(const char *selector, const char *str1, const char *str2);
void dspMethod(const char *cp, const char *mp);
void givepause();
object sysPrimitive(int number, object *arguments);

#endif	/* TTY_H */

