#
#	Makefile for Little Smalltalk system
#	written by Tim Budd, Oregon State University, budd@cs.orst.edu
#

CC = gcc
CFLAGS = -g

st: main.o interp.o memory.o
	$(CC) $(CFLAGS) -o st main.o interp.o memory.o

sst: main.o safeinterp.o memory.o
	$(CC) -o sst main.o safeinterp.o memory.o

distr.tar:
	tar cvf distr.tar ReadMe.html Makefile interp.h memory.h \
	main.c interp.c memory.c LittleSmalltalkImage Tests book.ps

main.o:   memory.h interp.h main.c
memory.o: memory.h interp.h memory.c
interp.o: memory.h interp.h interp.c
safeinterp.o: memory.h interp.h safeinterp.c

#
#	Makefile Notes
#
#	main.c contains three defined constants that may need to be changed
#
#	memory.h defines the main memory allocation routine (gcalloc) as
#	a macro -- this can be commented out and a procedure call will
#	be used instead -- slower but safer during debugging
#
#	for some reason, using the -O flag or the -p flag often causes
#	generation of code that won't run
