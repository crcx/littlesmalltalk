#
#	Makefile for Little Smalltalk system
#	written by Tim Budd, Oregon State University, budd@cs.orst.edu
#

CC=gcc
CFLAGS=-g -Wall
LIBS=

default:
	make st
	make image

st:
	$(CC) $(CFLAGS) source/*.c -o ./bin/st

image:
	cd ImageBuilder && make
	cp ImageBuilder/image bin/systemImage

clean:
	rm -f `find . | grep \~`
	rm -f bin/*
	cd ImageBuilder && make clean
