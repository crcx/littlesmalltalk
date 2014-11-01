CFLAGS =
LFLAGS =

BINDIR = ../bin

SOURCES = parser.y parser.lex parse1.c parse2.c
MISC = disclaim Makefile *.h y.tab.c lex.yy.c uchar.c

install: parse
	mv parse $(BINDIR)

parse: y.tab.o parse1.o parse2.o
	cc $(LFLAGS) -o parse y.tab.o parse1.o parse2.o -lm

newparse: y.tab.o parse1.o parse2.o
	cc $(LFLAGS) -o newparse y.tab.o parse1.o parse2.o -lm

bundle: y.tab.c lex.yy.c
	bundle $(SOURCES) $(MISC) >../parser.bundle

y.tab.o: y.tab.c lex.yy.c primnum.h

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: parser.lex
	lex parser.lex

parse2.o: parse2.c drive.h

lint.out: y.tab.c
	lint y.tab.c parse1.c parse2.c -lm

clean:
	-rm -f lex.yy.c
	-rm -f y.tab.c
	-rm -f y.tab.h
	-rm -f *.o
