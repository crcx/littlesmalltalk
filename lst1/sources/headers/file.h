/*************************************************************************
 * Little Smalltalk, Version 1
 *
 * The source code for the Little Smalltalk system may be freely copied
 * provided that the source of all files is acknowledged and that this
 * condition is copied with each file.
 *
 * The Little Smalltalk system is distributed without responsibility for
 * the performance of the program and without any guarantee of maintenance.
 *
 * All questions concerning Little Smalltalk should be addressed to:
 *
 *         Professor Tim Budd
 *         Department of Computer Science
 *         Oregon State University
 *         Corvallis, Oregon
 *         97331
 *         USA
 *
 * This copy of Little Smalltalk has been updated by Charles R. Childers,
 * http://charleschilders.com
 *************************************************************************/
/*
	Little Smalltalk

		class File definitions
		timothy a. budd, 11/84
*/
/*
	files use standard i/o package
*/
#include <stdio.h>

struct file_struct {
	int l_ref_count;
	int l_size;
	int file_mode;
	FILE *fp;
	};

typedef struct file_struct file;

extern object *new_file();
extern object *file_read();

/* files can be opened in one of three modes, modes are either
	0 - char mode - each read gets one char
	1 - string mode - each read gets a string
	2 - integer mode - each read gets an integer
*/
# define CHARMODE 0
# define STRMODE  1
# define INTMODE  2
