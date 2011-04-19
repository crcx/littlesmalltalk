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
MISC = disclaim Makefile *.h sstr.c symbols

st: sstr headers/drive.h headers/cmds.h headers/env.h $(Objects)
	cc $(LFLAGS) -o st $(Objects) $(LIB)

newst: sstr headers/drive.h headers/cmds.h headers/env.h $(Objects)
	cc $(LFLAGS) -o newst $(Objects) $(LIB)

# the following is used by st make script for installation on the DecPro 350
#	ld -o st -X -u __doprnt -u fltused -u fptrap -m \
# -lfpsim /lib/fcrt0.o $(Objects) -lm -lc

install: st
	mv st $(BINDIR)

bundle: $(MISC) $(Objects.c) 
	rm -f headers/drive.h headers/cmds.h headers/env.h
	bundle $(MISC) $(Objects.c) >../sources.bundle

lint.out:$(Objects.c)
	lint $(Objects.c)

syms.c: sstr symbols
	./sstr -t symbols SYMTABMAX '#include "headers/object.h"' '#include "headers/symbol.h"' >syms.c

sstr: sstr.c
	cc $(LFLAGS) -o sstr sstr.c

headers/drive.h: $(PARSEDIR)/drive.h symbols
	cp $(PARSEDIR)/drive.h ./headers

headers/cmds.h: $(PARSEDIR)/cmds.h symbols
	./sstr symbols <$(PARSEDIR)/cmds.h >headers/cmds.h

headers/env.h: $(PARSEDIR)/env.h
	cp $(PARSEDIR)/env.h ./headers

number.o: number.c headers/number.h
interp.o: headers/drive.h headers/cmds.h
primitive.o: headers/*.h
main.o: headers/*.h

clean:
	-rm *.o headers/drive.h headers/cmds.h headers/env.h
