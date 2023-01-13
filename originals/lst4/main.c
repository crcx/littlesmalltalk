/*
	Little Smalltalk main program, unix version
	Written by Tim Budd, budd@cs.orst.edu
	All rights reserved, no guarantees given whatsoever.
	May be freely redistributed if not for profit.

	starting point, primitive handler for unix
	version of the little smalltalk system
*/

/*
	the following defaults must be set

*/
# define DefaultImageFile "/u1/budd/Little/Src/LittleSmalltalkImage"
# define DefaultStaticSize 40000
# define DefaultDynamicSize 40000

/*
--------------------
*/

# include "memory.h"
# include "interp.h"
# include <stdio.h>

/* # define COUNTTEMPS */

int debugging = 0;
int cacheHit = 0;
int cacheMiss = 0;
int gccount = 0;

void sysError(char * a, int b)
{
	fprintf(stderr,"unrecoverable system error: %s %d\n", a, b);
	exit(1);
}

struct object * nwInteger(unsigned int v)
{
	register struct integerObject * n;

	if (v < 10)
		return smallInts[v];
	n = (struct integerObject *) gcialloc(BytesPerWord);
	n->class = SmallIntClass;
	n->value = v;
	return (struct object *) n;
}

struct object * staticInteger(int v)
{
	struct integerObject * n;

	n = (struct integerObject *) staticIAllocate(BytesPerWord);
	n->class = SmallIntClass;
	n->value = v;
	return (struct object *) n;
}

void backTrace(struct object * aContext)
{
	printf("back trace\n");
	while (aContext && (aContext != nilObject)) {
		struct object * arguments; int i;
		printf("message %s ", 
			bytePtr(aContext->data[methodInContext]
				->data[nameInMethod]));
		arguments = aContext->data[argumentsInContext];
		if (arguments && (arguments != nilObject)) {
			printf("(");
			for (i = 0; i < (arguments->size >> 2); i++)
				printf("%s,", 
				bytePtr(arguments->data[i]->class->
					data[nameInClass]));
			printf(")");
			}
		printf("\n");
		aContext = aContext->data[previousContextInContext];
		}
}

# ifdef COUNTTEMPS
FILE * tempFile;
# endif

main(int argc, char ** argv) {

	struct object * aProcess;
	struct object * aContext;
	int size, i;
	int staticSize, dynamicSize;
	FILE * fp;
	char imageFileName[120];

	strcpy(imageFileName, DefaultImageFile);
	staticSize = DefaultStaticSize;
	dynamicSize = DefaultDynamicSize;

	/* first parse arguments */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			printf("Little Smalltalk, version 4.01\n");
			}
		else if (strcmp(argv[i], "-s") == 0) {
			staticSize = atoi(argv[++i]);
			}
		else if (strcmp(argv[i], "-d") == 0) {
			dynamicSize = atoi(argv[++i]);
			}
		else {
			strcpy(imageFileName, argv[i]);
			}
		}

# ifdef COUNTTEMPS
	tempFile = fopen("/usr/tmp/counts", "w");
# endif

	gcinit(staticSize, dynamicSize);

	/* read in the method from the image file */
	fp = fopen(imageFileName, "r");
	if (! fp) {
		fprintf(stderr,"cannot open image file: %s\n", imageFileName);
		exit(1);
		}

	printf("%d objects in image\n", fileIn(fp));
	fclose(fp);

	/* build a context around it */

	aProcess = staticAllocate(3);
		/* context should be dynamic */
	aContext = gcalloc(contextSize);


	aProcess->data[contextInProcess] = aContext;
	size = integerValue(initialMethod->data[stackSizeInMethod]);
	aContext->data[argumentsInContext] = staticAllocate(2);
	aContext->data[stackInContext] = staticAllocate(size);
	aContext->data[argumentsInContext] = staticAllocate(2);

	aContext->data[temporariesInContext] = staticAllocate(19);
	aContext->data[bytePointerInContext] = staticInteger(0);
	aContext->data[stackTopInContext] = staticInteger(0);
	aContext->data[previousContextInContext] = nilObject;
	aContext->data[methodInContext] = initialMethod;

	/* now go do it */
	rootStack[rootTop++] = aProcess;

	switch(execute(aProcess)) {
		case 2: printf("User defined return\n"); break;

		case 3: printf("can't find method in call\n"); 
			aProcess = rootStack[--rootTop];
			aContext = aProcess->data[contextInProcess];
			backTrace(aContext);
			break;

		case 4: printf("\nnormal return\n"); break;

		case 5: printf("time out\n"); break;

		default: printf("unknown return code\n"); break;
		}
	printf("cache hit %d miss %d ratio %f\%\n", cacheHit, cacheMiss,
		100.0 * ((double) cacheHit) / ((double) cacheHit + cacheMiss));
	printf("%d garbage collections\n", gccount);
}

/*
	primitive handler
	(note that many primitives are handled in the interpreter)
*/

# define FILEMAX 10
FILE * filePointers[FILEMAX];
int fileTop = 0;

void getUnixString(char * to, int size, struct object * from)
{
	int i;
	int fsize = from->size >> 2;
	struct byteObject * bobj = (struct byteObject *) from;

	if (fsize > size) sysError("error converting text into unix string",
		fsize);
	for (i = 0; i < fsize; i++)
		to[i] = bobj->bytes[i];
	to[i] = '\0';	/* put null terminator at end */
}

struct object * primitive(int primitiveNumber, struct object * args)
{	struct object * returnedValue = nilObject;
	int i, j;
	FILE * fp;
	char * p;
	struct byteObject * stringReturn;
	char nameBuffer[80];
	char modeBuffer[80];

	switch(primitiveNumber) {
		case 30: 	/* open a file */
			getUnixString(nameBuffer, 80, args->data[0]);
			getUnixString(modeBuffer, 10, args->data[1]);
			fp = fopen(nameBuffer, modeBuffer);
			if (fp != NULL) {
				if (fileTop + 1 >= FILEMAX)
					sysError("too many open files", 0);
				returnedValue = nwInteger(fileTop);
				filePointers[fileTop++] = fp;
				}
			break;

		case 31:	/* read a single character from a file */
			i = integerValue(args->data[0]);
			i = fgetc(filePointers[i]);
			if (i != EOF)
				returnedValue = nwInteger(i);
			break;

		case 32:	/* write a single character to a file */
			fputc(integerValue(args->data[1]),
				filePointers[integerValue(args->data[0])]);
			break;

		case 33:	/* close file */
			i = integerValue(args->data[0]);
			fclose(filePointers[i]);
			if (i+1 == fileTop)
				fileTop--;
			break;

		case 34:	/* file out image */
			i = integerValue(args->data[0]);
			fileOut(filePointers[i]);
			break;

		case 35:	/* edit a string */
				/* first get the name of a temp file */
			strcpy(nameBuffer, "/usr/tmp/lsteditXXXXXX");
			mktemp(nameBuffer);
				/* copy string to file */
			fp = fopen(nameBuffer, "w");
			if (fp == NULL) 
				sysError("cannot open temp edit file", 0);
			j = args->data[0]->size >> 2;
			p = ((struct byteObject *) args->data[0])->bytes;
			for (i = 0; i < j; i++)
				fputc(*p++, fp);
			fputc('\n', fp);
			fclose(fp);
				/* edit string */
			strcpy(modeBuffer,"vi ");
			strcat(modeBuffer,nameBuffer);
			system(modeBuffer);
				/* copy back to new string */
			fp = fopen(nameBuffer, "r");
			if (fp == NULL) 
				sysError("cannot open temp edit file", 0);
				/* get length of file */
			fseek(fp, 0, 2);
			j = (int) ftell(fp);
			returnedValue = 
				(struct object *) stringReturn = gcialloc(j);
			returnedValue->class = args->data[0]->class;
				/* reset to beginning, and read values */
			fseek(fp, 0, 0);
			for (i = 0; i < j; i++)
				stringReturn->bytes[i] = fgetc(fp);
				/* now clean up files */
			fclose(fp);
			unlink(nameBuffer);
			break;

		default:
			sysError("unknown primitive", primitiveNumber);
		}
	return returnedValue;
}
