/*
	Little Smalltalk, version 3
	Written by Tim Budd, January 1989

	tty interface routines
	this is used by those systems that have a bare tty interface
	systems using another interface, such as the stdwin interface
	will replace this file with another.
*/

# include <stdio.h>
# include "env.h"
# include "memory.h"

extern boolean parseok;

/* report a fatal system error */
noreturn sysError(s1, s2)
char *s1, *s2;
{
	ignore fprintf(stderr,"%s\n%s\n", s1, s2);
	ignore abort();
}

/* report a nonfatal system error */
noreturn sysWarn(s1, s2)
char *s1, *s2;
{
	ignore fprintf(stderr,"%s\n%s\n", s1, s2);
}

compilWarn(selector, str1, str2)
char *selector, *str1, *str2;
{
	ignore fprintf(stderr,"compiler warning: Method %s : %s %s\n", 
		selector, str1, str2);
}

compilError(selector, str1, str2)
char *selector, *str1, *str2;
{
	ignore fprintf(stderr,"compiler error: Method %s : %s %s\n", 
		selector, str1, str2);
	parseok = false;
}

noreturn dspMethod(cp, mp)
char *cp, *mp;
{
	/*ignore fprintf(stderr,"%s %s\n", cp, mp);*/
}

givepause()
{	char buffer[80];

	ignore fprintf(stderr,"push return to continue\n");
	ignore gets(buffer);
}

object sysPrimitive(number, arguments)
int number;
object *arguments;
{	object returnedObject;

		/* someday there will be more here */
	switch(number - 150) {
		case 0:		/* do a system() call */
			returnedObject = newInteger(system(
				charPtr(arguments[0])));
			break;

		default:
			sysError("unknown primitive","sysPrimitive");
		}
	return(returnedObject);
}
