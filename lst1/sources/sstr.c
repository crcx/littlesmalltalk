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
	sstr - find and replace string occurrences
		with common addresses,
		can be used to share strings accross compiled boundaries
		written by tim budd, 9/84
*/
/*
	The source code for the Little Smalltalk System may be freely
	copied provided that the source of all files is acknowledged
	and that this condition is copied with each file.

	The Little Smalltalk System is distributed without responsibility
	for the performance of the program and without any guarantee of
	maintenance.

	All questions concerning Little Smalltalk should be addressed to:

		Professor Tim Budd
		Department of Computer Science
		Oregon State University
		Corvallis, Oregon
		97331
		USA
*/
# include <stdio.h>
# define WORDTABMAX 1000
# define STRTABMAX 10000

int x_cmax = 0;
int x_tmax = -1;
char x_str[STRTABMAX];
char *x_tab[WORDTABMAX];


main(argc, argv)
int argc;
char **argv;
{	int i;
	FILE *fd;

	if (strcmp(argv[1], "-f") == 0) {
		for (i = 2; i < argc; i++) {
			fd = fopen(argv[i], "r");
			if (fd != NULL) {
				findstrs(fd);
				fclose(fd);
				}
			}
		}
	else if (strcmp(argv[1], "-t") == 0) {
		for (i = 4; i < argc; i++)
		   puts(argv[i]);
		fd = fopen(argv[2], "r");
		if (fd == NULL) {
			fprintf(stderr,"can't open string table\n");
			exit(1);
			}
		maketab(fd, stdout, argv[3]);
		}
	else {
		fd = fopen(argv[1], "r");
		if (fd == NULL) {
			fprintf(stderr,"can't open string table\n");
			exit(1);
			}
		maketab(fd, 0, 0);
		printf("extern char x_str[];\n");
		replacestr(stdin);
		}
	exit(0);
}

/* findstrs - find all strings and output them to stdout */
findstrs(fd)
FILE *fd;
{
	char *p, buffer[500];
	int c;

	for (; (c = getc(fd)) != EOF; )
		if (c == '\"') {
			for (p = buffer; (c = getc(fd)) != '\"'; p++)
				if (c == EOF) {
					fprintf(stderr,"unexpected eof\n");
					exit(1);
					}
				else *p = c;
			*p = '\0';
			puts(buffer);
			}
}

/* replacestr - replace strings with their address in x_str */
replacestr(fd)
FILE *fd;
{
	char *p, buffer[500], *w_search();
	int c;

	for (; (c = getc(fd)) != EOF; )
		if (c != '\"') putchar(c);
		else {
			for (p = buffer; (c = getc(fd)) != '\"'; p++)
				if (c == EOF) {
					fprintf(stderr,"unexpected eof\n");
					exit(1);
					}
				else *p = c;
			*p = '\0';
			p = w_search(buffer, 0);
			if (p) printf("&x_str[%d]", p - &x_str[0]);
			else printf("\"%s\"", buffer);
			}
}

maketab(ifd, ofd, itab)
FILE *ifd, *ofd;
char *itab;
{	char wbuf[100], *p;
	int i;

	x_cmax = 0;
	if (ofd)
		fprintf(ofd, "char x_str[] = {");
	while (fgets(wbuf, 100, ifd) != NULL) {
		x_tab[++x_tmax] = &x_str[x_cmax];
		for (p = wbuf; *p; p++) {
			if (*p == '\n') {*p = '\0'; break;}
			if (ofd)
				fprintf(ofd,"0%o, ", *p);
			x_str[x_cmax++] = *p;
			}
		if (ofd)
			fprintf(ofd, "0,   /* %s */\n", wbuf);
		x_str[x_cmax++] = '\0';
		}
	if (ofd) {
		fprintf(ofd, "0 };\n");
		fprintf(ofd, "int x_cmax = %d;\n", x_cmax);
		}
	if (itab) {
		fprintf(ofd, "static symbol x_sytab[] = {\n");
		for (i = 0; i <= x_tmax; i++) {
			fprintf(ofd, "{1, SYMBOLSIZE, &x_str[%d]}, /* ", 
				x_tab[i]-x_tab[0]);
			for (p = x_tab[i]; *p; p++) 
				putc(*p, ofd);
			fprintf(ofd," */\n");
			}
		fprintf(ofd, "0};\n");
		fprintf(ofd, "symbol *x_tab[%s] = {\n", itab);
		for (i = 0; i <= x_tmax; i++) {
			fprintf(ofd, "&x_sytab[%d], /* ",i); 
			for (p = x_tab[i]; *p; p++) 
				putc(*p, ofd);
			fprintf(ofd," */\n");
			}
		fprintf(ofd, "0};\n");
		fprintf(ofd,"int x_tmax = %d;\n", x_tmax);
		}
}

/* 	
	word search for table routines
*/

char *w_search(word, insert)
char *word;
int  insert;
{	int i,j,k;

	for (i=1; i <= x_tmax; i <<= 1);
	for (i >>= 1, j = i >>1, i--; ; j >>= 1) {
		if (! (k = strcmp(word, x_tab[i])))
			return(x_tab[i]);

		if (!j) break;
		if (k < 0) i -= j;
		else {
			if ((i += j) > x_tmax) i = x_tmax;
			}
		}
	if (insert) {
		for (k = ++x_tmax; k > i; k--) {
			x_tab[k] = x_tab[k-1];
			}
		if (!(x_tab[i] = (char *) malloc(1 + strlen(word))))
			return((char *) 0);
		strcpy(x_tab[i], word);
		return(x_tab[i]);
		}
	else return((char *) 0);
}
