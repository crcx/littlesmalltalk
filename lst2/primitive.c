/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	Primitive processor

	primitives are how actions are ultimately executed in the Smalltalk 
	system.
	unlike ST-80, Little Smalltalk primitives cannot fail (although
	they can return nil, and methods can take this as an indication
	of failure).  In this respect primitives in Little Smalltalk are
	much more like traditional system calls.

	Primitives are combined into groups of 10 according to 
	argument count and type, and in some cases type checking is performed.
*/

# include <stdio.h>
# include <math.h>
# include <stdlib.h>
# include <string.h>
# include "env.h"
# include "memory.h"
# include "names.h"
# include "process.h"

# define normalresult 1
# define counterror 2
# define typeerror  3
# define quitinterp 4

extern object doInterp();
extern double modf();
extern char *getenv();
extern object getClass(object);
extern void setInstanceVariables();
extern void imageWrite();
extern boolean parse();
extern void flushMessageCache();

char tempfilename[100];		/* temp file for editing */

static int zeroaryPrims(number)
int number;
{	char buffer[100];
	short i;

	returnedObject = nilobj;
	switch(number) {
		case 1:			/* read from user */
			if (gets(buffer) != NULL)
				returnedObject = newStString(buffer);
			break;

		case 2:
			flushMessageCache();
			break;

		case 3:			/* return a random number */
			/* this is hacked because of the representation */
			/* of integers as shorts */
			i = rand() >> 8;	/* strip off lower bits */
			if (i < 0) i = - i;
			returnedObject = newInteger(i>>1);
			break;

		default:		/* unknown primitive */
			sysError("unknown primitive","zeroargPrims");
			break;
	}
	return(normalresult);
}

static int unaryPrims(number, firstarg)
int number;
object firstarg;
{

	returnedObject = firstarg;
	switch(number) {
		case 1:		/* class of object */
			returnedObject = getClass(firstarg);
			break;

		case 2:		/* basic size of object */
			if (isInteger(firstarg))
				returnedObject = newInteger(0);
			else
				returnedObject = newInteger(objectSize(firstarg));
			break;

		case 3:		/* hash value of object */
			if (isInteger(firstarg))
				returnedObject = firstarg;
			else
				returnedObject = newInteger((int) firstarg);
			break;

		case 9:		/* interpreter bytecodes */
			returnedObject = doInterp(firstarg);
			break;

		default:		/* unknown primitive */
			sysError("unknown primitive","unaryPrims");
			break;
	}
	return(normalresult);
}

static int binaryPrims(number, firstarg, secondarg)
int number;
object firstarg, secondarg;
{	char buffer[120];
	char *bp;

	returnedObject = firstarg;
	switch(number) {
		case 1:		/* object identity test */
			if (firstarg == secondarg)
				returnedObject = trueobj;
			else
				returnedObject = falseobj;
			break;

		case 2:		/* set class of object */
			decr(classField(firstarg));
			setClass(firstarg, secondarg);
			returnedObject = firstarg;
			break;

		case 4:		/* string cat */
			ignore strcpy(buffer, charPtr(firstarg));
			ignore strcat(buffer, charPtr(secondarg));
			returnedObject = newStString(buffer);
			break;
		
		case 5:		/* basicAt: */
			returnedObject = basicAt(firstarg, intValue(secondarg));
			break;

		case 6:		/* byteAt: */
			bp = charPtr(firstarg);
			returnedObject = newInteger(bp[intValue(secondarg)-1]);
			break;

		case 8:		/* execute a context */
			messageToSend = firstarg;
			argumentsOnStack = intValue(secondarg);
			finalTask = ContextExecuteTask;
			return(quitinterp);

		default:		/* unknown primitive */
			sysError("unknown primitive","binaryPrims");
			break;

	}
	return(normalresult);
}

static int trinaryPrims(number, firstarg, secondarg, thirdarg)
int number;
object firstarg, secondarg, thirdarg;
{	char *bp;

	returnedObject = firstarg;
	switch(number) {
		case 1:			/* basicAt:Put: */
			basicAtPut(firstarg, intValue(secondarg), thirdarg);
			break;

		case 2:			/* basicAt:Put: for bytes */
			bp = charPtr(firstarg);
			bp[intValue(secondarg)-1] = intValue(thirdarg);
			break;

		case 9:			/* compile method */
			setInstanceVariables(firstarg);
			if (parse(thirdarg, charPtr(secondarg)))
				returnedObject = trueobj;
			else
				returnedObject = falseobj;
			break;
		
		default:		/* unknown primitive */
			sysError("unknown primitive","trinaryPrims");
			break;
		}
	return(normalresult);
}

static int intUnary(number, firstarg)
int number, firstarg;
{	char buffer[20];

	switch(number) {
		case 1:		/* float equiv of integer */
			returnedObject = newFloat((double) firstarg);
			break;

		case 5:		/* set random number */
			srand(firstarg);
			returnedObject = nilobj;
			break;

		case 6:		/* string equiv of number */
			buffer[0] = firstarg;
			buffer[1] = '\0';
			returnedObject = newStString(buffer);
			break;

		case 7:
			ignore sprintf(buffer,"%d",firstarg);
			returnedObject = newStString(buffer);
			break;

		case 8:
			returnedObject = allocObject(firstarg);
			break;

		case 9:
			returnedObject = allocByte(firstarg);
			break;

		default:
			sysError("intUnary primitive","not implemented yet");
		}
	return(normalresult);
}

int intBinary(number, firstarg, secondarg)
register int firstarg, secondarg;
int number;
{	boolean binresult=0;

	switch(number) {
		case 0:
			firstarg += secondarg; break;
		case 1:
			firstarg -= secondarg; break;
		case 2:
			binresult = firstarg < secondarg; break;
		case 3:
			binresult = firstarg > secondarg; break;
		case 4:
			binresult = firstarg <= secondarg; break;
		case 5:
			binresult = firstarg >= secondarg; break;
		case 6:
			binresult = firstarg == secondarg; break;
		case 7:
			binresult = firstarg != secondarg; break;
		case 8:
			firstarg *= secondarg; break;
		case 9:
			firstarg /= secondarg; break;
		case 10:
			firstarg %= secondarg; break;
		case 11:
			firstarg &= secondarg; break;
		case 12:
			firstarg ^= secondarg; break;
		case 19:
			if (secondarg < 0)
				firstarg >>= (- secondarg);
			else
				firstarg <<= secondarg;
			break;
	}
	if ((number >= 2) && (number <= 7))
		if (binresult)
			returnedObject = trueobj;
		else
			returnedObject = falseobj;
	else
		returnedObject = newInteger(firstarg);
	return(normalresult);
}

static int strUnary(number, firstargument)
int number;
char *firstargument;
{	FILE *fp;
	char *p, buffer[1000];

	switch(number) {
		case 1:		/* length of string */
			returnedObject = newInteger(strlen(firstargument));
			break;

		case 2:		/* copy of string */
			returnedObject = newStString(firstargument);
			break;

		case 3:		/* string as symbol */
			returnedObject = newSymbol(firstargument);
			break;

		case 6:		/* print, no newline */
			fputs(firstargument, stdout);
			ignore fflush(stdout);
			returnedObject = nilobj;
			break;

		case 7:		/* make an object image */
			returnedObject = falseobj;
			fp = fopen(firstargument, "w");
			if (fp == NULL) break;
			imageWrite(fp);
			ignore fclose(fp);
			returnedObject = trueobj;
			break;

		case 8:		/* print a string */
			puts(firstargument);
			ignore fflush(stdout);
			returnedObject = nilobj;
			break;

		case 9:		/* edit a string */
			fp = fopen(tempfilename, "w");
			fputs(firstargument, fp);
			ignore fclose(fp);
			p = getenv("EDITOR");
			if (! p) p = "ed";
			sprintf(buffer,"%s %s", p, tempfilename);
			ignore system(buffer);
			fp = fopen(tempfilename, "r");
			for (p = buffer; (*p = getc(fp)) != EOF; p++);
			*p = '\0';
			ignore fclose(fp);
			returnedObject = newStString(buffer);
			sprintf(buffer,"rm %s", tempfilename);
			ignore system(buffer);
			break;

		default:
			sysError("unknown primitive", "strUnary");
			break;
		}

	return(normalresult);
}

static int floatUnary(number, firstarg)
int number;
double firstarg;
{	char buffer[20];
	double temp;

	switch(number) {
		case 1:		/* asString */
			ignore sprintf(buffer,"%g", firstarg);
			returnedObject = newStString(buffer);
			break;

		case 2:		/* log */
			returnedObject = newFloat(log(firstarg));
			break;

		case 3:		/* exp */
			returnedObject = newFloat(exp(firstarg));
			break;

		case 4:		/* sqrt */
			returnedObject = newFloat(sqrt(firstarg));
			break;

		case 5:		/* gamma */
			returnedObject = newFloat(gamma(firstarg));
			break;

		case 6:		/* integer part */
			modf(firstarg, &temp);
			returnedObject = newInteger((int) temp);
			break;

		default:
			sysError("unknown primitive","floatUnary");
			break;
		}

	return(normalresult);
}

int floatBinary(number, first, second)
int number;
double first, second;
{	 boolean binResult=0;

	switch(number) {
		case 0: first += second; break;

		case 1:	first -= second; break;
		case 2: binResult = (first < second); break;
		case 3: binResult = (first > second); break;
		case 4: binResult = (first <= second); break;
		case 5: binResult = (first >= second); break;
		case 6: binResult = (first == second); break;
		case 7: binResult = (first != second); break;
		case 8: first *= second; break;
		case 9: first /= second; break;
		default:	
			sysError("unknown primitive", "floatBinary");
			break;
		}

	if ((number >= 2) && (number <= 7))
		if (binResult)
			returnedObject = trueobj;
		else
			returnedObject = falseobj;
	else
		returnedObject = newFloat(first);
	return(normalresult);
}

boolean primitive(primitiveNumber, arguments, size)
int primitiveNumber, size;
object *arguments;
{	int primitiveGroup;
	boolean done = false;
	int response;

	primitiveGroup = primitiveNumber / 10;
	response = normalresult;
	switch(primitiveGroup) {
		case 0: case 1: case 2: case 3:
			if (size != primitiveGroup)
				response = counterror;
			else {
				switch(primitiveGroup) {
					case 0:
						response = zeroaryPrims(primitiveNumber);
						break;
					case 1:
						response = unaryPrims(primitiveNumber - 10, arguments[0]);
						break;
					case 2:
						response = binaryPrims(primitiveNumber-20, arguments[0], arguments[1]);
						break;
					case 3:
						response = trinaryPrims(primitiveNumber-30, arguments[0], arguments[1], arguments[2]);
						break;
				}
			}
			break;


		case 5:			/* integer unary operations */
			if (size != 1)
				response = counterror;
			else if (! isInteger(arguments[0]))
				response = typeerror;
			else
				response = intUnary(primitiveNumber-50,
						intValue(arguments[0]));
			break;

		case 6: case 7:		/* integer binary operations */
			if (size != 2)
				response = counterror;
			else if ((! isInteger(arguments[0])) || 
				  ! isInteger(arguments[1]))
				response = typeerror;
			else
				response = intBinary(primitiveNumber-60,
					intValue(arguments[0]), 
					intValue(arguments[1]));
			break;

		case 8:			/* string unary */
			if (size != 1)
				response = counterror;
			else if (! isString(arguments[0]))
				response = typeerror;
			else
				response = strUnary(primitiveNumber-80,
					charPtr(arguments[0]));
			break;

		case 10:		/* float unary */
			if (size != 1)
				response = counterror;
			else if (! isFloat(arguments[0]))
				response = typeerror;
			else
				response = floatUnary(primitiveNumber-100,
					floatValue(arguments[0]));
			break;

		case 11:		/* float binary */
			if (size != 2)
				response = counterror;
			else if ((! isFloat(arguments[0])) ||
				 (! isFloat(arguments[1])))
				response = typeerror;
			else
				response = floatBinary(primitiveNumber-110,
					floatValue(arguments[0]),
					floatValue(arguments[1]));
			break;

	}

	/* now check return code */
	switch(response) {
		case normalresult:
			break;
		case quitinterp:
			done = true;
			break;
		case counterror:
			sysError("count error","in primitive");
			break;
		case typeerror:
fprintf(stderr,"primitive number %d\n", primitiveNumber);
			sysError("type error","in primitive");
			returnedObject = nilobj;
			break;

		default:
			sysError("unknown return code","in primitive");
			returnedObject = nilobj;
			break;
	}
	return (done);
}

