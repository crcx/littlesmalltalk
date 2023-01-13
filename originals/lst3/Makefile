CFLAGS = -O

# define groups of files, to make later commands easier
INTERPc = memory.c names.c news.c interp.c
INTERPo = memory.o names.o news.o interp.o
PRIMITIVEc = primitive.c filein.c lex.c parser.c unixio.c
PRIMITIVEo = primitive.o filein.o lex.o parser.o unixio.o
basicST = basic.st mag.st collect.st 
unixST = file.st mult.st 
testST = test.st queen.st
CFILES = *.h $(INTERPc) $(PRIMITIVEc) st.c initial.c tty.c winprim.c
OFILES = $(INTERPo) $(PRIMITIVEo) 
STFILES = $(basicST) $(unixST) $(testST) tty.st graphics.st stdwin.st
FILES = README Makefile vms.com $(CFILES) $(STFILES) *.ms
# the following are used only by turboc on the ibm pc
TURBOc=memory names news interp primitive filein lex parser unixio tty
TINCLUDE=c:\turboc\include
TLIB=c:\turboc\lib

# the following is for those poor souls who say ``make'' without looking
help:
	@echo "select one of the following to make"
	@echo "bsdtty: Berkeley 4.2/4.3 with teletype interface "
	@echo "sysvtty: System V with teletype interface "
	@echo "bsdx11: Berkeley 4.2/4.3 with stdwin interface on top of X11"
	@echo "ibmturboc: IBM PC with Turbo C compiler (see install.ms)"

winfiles: $(OFILES) winprim.o initial.o st.o

# initial - the initial object maker
initial: $(INTERPo) $(PRIMITIVEo) initial.o $(INTERFACE)
	cc -o initial $(CFLAGS) $(INTERPo) $(PRIMITIVEo) initial.o $(INTERFACE) -lm $(LIBS)

# st - the actual bytecode interpreter
st: $(INTERPo) $(PRIMITIVEo) st.o $(INTERFACE)
	cc -o st $(CFLAGS) $(INTERPo) $(PRIMITIVEo) st.o $(INTERFACE) -lm $(LIBS)

# bsdtty - berkeley 4.2/4.3 with tty style interface
bsdtty:
	make "CFLAGS=$(CFLAGS) -DB42" "LIBS=" "INTERFACE= tty.o" initial st
	initial $(basicST) $(unixST) tty.st

bsdtty.lint:
	lint -DB42 $(INTERPc) $(PRIMITIVEc) tty.c initial.c -lm
	lint -DB42 $(INTERPc) $(PRIMITIVEc) tty.c st.c -lm

# sysvtty - system V with tty style interface
sysvtty:
	make "CFLAGS=$(CFLAGS) -DSYSV" "LIBS=" "INTERFACE= tty.o" initial st
	initial $(basicST) $(unixST) tty.st

sysvtty.lint:
	lint -DSYSV $(INTERPc) $(PRIMITIVEc) tty.c initial.c -lm
	lint -DSYSV $(INTERPc) $(PRIMITIVEc) tty.c st.c -lm

# bsdterm - berkekey 4.2/4.3 with stdwin interface on top of termcap
# requires Guido van Rossum's standard window package
# (currently doesn't work)
bsdterm: stdw.o
	make "CFLAGS=$(CFLAGS) -DB42 -DSTDWIN" "LIBS= -ltermcap" "INTERFACE= winprim.o stdw.o" initial st
	initial $(basicST) $(unixST) graphics.st stdwin.st

# bsdx11 - berkekey 4.2/4.3 with stdwin interface on top of x11
# requires Guido van Rossum's standard window package
bsdx11: stdw.o
	make "CFLAGS=$(CFLAGS) -DB42 -DSTDWIN" "LIBS= -lX" "INTERFACE= winprim.o stdw.o" initial st
	initial $(basicST) $(unixST) graphics.st stdwin.st

bsdx11.lint:
	lint -DB42 -DSTDWIN $(INTERPc) $(PRIMITIVEc) winprim.c initial.c -lm -lX
	lint -DB42 -DSTDWIN $(INTERPc) $(PRIMITIVEc) winprim.c st.c -lm -lX

# ibmturboc - IBM PC with Turbo C compiler
# see installation notes for editing that must be performed first
ibmturboc:
	tcc -I$(TINCLUDE) -L$(TLIB) -mc -C -a -w- -est $(TURBOc) st
	tcc -I$(TINCLUDE) -L$(TLIB) -mc -a -w- initial $(TURBOc)
	initial basic.st mag.st collect.st file.st mult.st tty.st

# stdw.o - guidos van rossum's standard window package
stdw.o:
	@echo to create stdw.o see installation instructions

# all the dependencies on .h files
filein.o : filein.c env.h memory.h names.h lex.h 
initial.o : initial.c env.h memory.h names.h 
interp.o : interp.c env.h memory.h names.h interp.h 
lex.o : lex.c env.h memory.h lex.h 
memory.o : memory.c env.h memory.h 
names.o : names.c env.h memory.h names.h 
news.o : news.c env.h memory.h names.h 
parser.o : parser.c env.h memory.h names.h interp.h lex.h 
primitive.o : primitive.c env.h memory.h names.h 
st.o : st.c env.h memory.h names.h 
tty.o : tty.c env.h memory.h 
unixio.o : unixio.c env.h memory.h names.h 
winprim.o : winprim.c stdwin.h stdevent.h stdtext.h env.h memory.h names.h 

# - pack - pack up the files for mailing
pack: $(FILES)
	packmail $(FILES)

# - tar - make a compressed tar file
tar: $(FILES)
	tar cvf small.v3.tar $(FILES)
	compress small.v3.tar
