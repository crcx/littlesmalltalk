CFLAGS =
LFLAGS =
LIB = -lm

BINDIR = ../bin
PARSEDIR = ../parser

Objects = main.o object.o line.o \
class.o number.o symbol.o string.o byte.o array.o file.o \
primitive.o syms.o cldict.o process.o interp.o block.o courier.o \
lex.o drive.o lexcmd.o
Objects.c = main.c object.c line.c \
class.c number.c symbol.c string.c byte.c array.c file.c \
primitive.c syms.c cldict.c process.c interp.c block.c courier.c \
lex.c drive.c lexcmd.c
MISC = disclaim Makefile *.h sstr.c symbols newmal.c

st: sstr drive.h cmds.h env.h $(Objects)
	cc $(LFLAGS) -o st $(Objects) $(LIB)

newst: sstr drive.h cmds.h env.h $(Objects)
	cc $(LFLAGS) -o newst $(Objects) $(LIB)

# the following is used by st make script for installation on the DecPro 350
#	ld -o st -X -u __doprnt -u fltused -u fptrap -m \
# -lfpsim /lib/fcrt0.o $(Objects) -lm -lc

install: st
	mv st $(BINDIR)

bundle: $(MISC) $(Objects.c) 
	rm -f drive.h cmds.h env.h
	bundle $(MISC) $(Objects.c) >../sources.bundle

lint.out:$(Objects.c)
	lint $(Objects.c)

syms.c: sstr symbols
	sstr -t symbols SYMTABMAX '# include "object.h"' '# include "symbol.h"' >syms.c

sstr: sstr.c
	cc $(LFLAGS) -o sstr sstr.c

drive.h: $(PARSEDIR)/drive.h symbols
	cp $(PARSEDIR)/drive.h .

cmds.h: $(PARSEDIR)/cmds.h symbols
	sstr symbols <$(PARSEDIR)/cmds.h >cmds.h

env.h: $(PARSEDIR)/env.h
	cp $(PARSEDIR)/env.h .

number.o: number.c number.h
interp.o: drive.h cmds.h
primitive.o: *.h
main.o: *.h

clean:
	-rm *.o drive.h cmds.h env.h
