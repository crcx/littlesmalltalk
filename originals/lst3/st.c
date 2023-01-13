/*
	Little Smalltalk, version 3
	Main Driver
	written By Tim Budd, September 1988
	Oregon State University
*/
# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"

int initial = 0;	/* not making initial image */

extern int objectCount();

# ifdef NOARGC
main()
# endif
# ifndef NOARGC
main(argc, argv)
int argc;
char **argv;
# endif
{
FILE *fp;
object firstProcess;
char *p, buffer[120];

initMemoryManager();
 
strcpy(buffer,"systemImage");
p = buffer;

# ifdef STDWIN
/* initialize the standard windows package */
winit();
wmenusetdeflocal(1);

# ifdef NOARGC
if (! waskync("use default initial object image?", 1))
	waskfile("image file name", buffer, 120, 0);
# endif
# endif

# ifndef NOARGC
if (argc != 1) p = argv[1];
# endif

# ifdef BINREADWRITE
fp = fopen(p, "rb");
# endif
# ifndef BINREADWRITE
fp = fopen(p, "r");
# endif

if (fp == NULL) {
	sysError("cannot open image", p);
	exit(1);
	}
imageRead(fp);
initCommonSymbols();

firstProcess = globalSymbol("systemProcess");
if (firstProcess == nilobj) {
	sysError("no initial process","in image");
	exit(1); return 1;
	}

/* execute the main system process loop repeatedly */
/*debugging = true;*/

# ifndef STDWIN
/* not using windowing interface, safe to print out message */
printf("Little Smalltalk, Version 3.04\n");
printf("Written by Tim Budd, Oregon State University\n");
# endif

while (execute(firstProcess, 15000)) ;

# ifdef STDWIN
wdone();
# endif

/* exit and return - belt and suspenders, but it keeps lint happy */
exit(0); return 0;
}
