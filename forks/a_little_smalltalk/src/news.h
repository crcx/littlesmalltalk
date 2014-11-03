/*
 * File:   news.h
 * Author: oni
 *
 * Created on 31. August 2012, 17:54
 */

#ifndef NEWS_H
#define	NEWS_H

/*
 little smalltalk, version 3.1
 written by tim budd, July 1988

 new object creation routines
 built on top of memory allocation, these routines
 handle the creation of various kinds of objects
 */

#include <stdio.h>
#include "env.h"
#include "memory.h"
#include "names.h"

/* ncopy - copy exactly n bytes from place to place */
void ncopy(register char *p, register char *q, register int n);

/* getClass - get the class of an object */
object getClass(register object obj);

object newArray(int size);

object newBlock();

object newByteArray(int size);

object newChar(int value);

object newClass(const char *name);

object copyFrom(object obj, int start, int size);

object newContext(int link, object method, object args, object temp);

object newDictionary(int size);

object newFloat(double d);
double floatValue(object o);

object newLink(object key, object value);

object newMethod();

object newStString(const char *value);

object newSymbol(const char *str);


#endif	/* NEWS_H */

