/*
 Little Smalltalk, version two
 Written by Tim Budd, Oregon State University, July 1987

 =============== rules for various systems ====================
*/

#ifndef ENV_H
#define	ENV_H

#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

/* this is a bit sloppy - but it works */
#define longCanBeInt(l) ((l >= -16383) && (l <= 16383))

#define STRINGS
#define SIGNAL

/* ======== various defines that should work on all systems ==== */

# define true 1
# define false 0

/* define the datatype boolean */
typedef char boolean;

#endif