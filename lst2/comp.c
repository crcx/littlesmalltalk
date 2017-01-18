/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	Unix specific front end for the initial object image maker
*/

# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"

extern 	void buildInitialNameTables();
extern	void readFile();
extern	void imageWrite();
extern	void initMemoryManager();

int main(argc, argv) 
int argc;
char **argv;
{ FILE *fp;
	int i;

	initMemoryManager();

	buildInitialNameTables();

	if (argc == 1)
		readFile(stdin);
	else
		for (i = 1; i < argc; i++) {
			fp = fopen(argv[i], "r");
			if (fp == NULL)
				sysError("can't open file", argv[i]);
			else {
				readFile(fp);
				ignore fclose(fp);
				}
			}

	fp = fopen("imageFile", "w");
	if (fp == NULL) sysError("error during image file open","imageFile");
	imageWrite(fp);
	ignore fclose(fp);
	return 0;
}
