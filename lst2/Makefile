#
# Makefile for Little Smalltalk, version 2
#
CFLAGS = -O2 -Wall -g -Wno-sequence-point -Wno-nonnull -Wno-unused-result -Wno-deprecated-declarations

COMMONc = memory.c names.c lex.c parser.c
COMMONo = memory.o names.o lex.o parser.o
PARSEc  = comp.c $(COMMONc) image.c
PARSEo  = comp.o $(COMMONo) image.o
STc     = main.c $(COMMONc) process.c primitive.c interp.c
STo     = main.o $(COMMONo) process.o primitive.o interp.o
classes = basicclasses unixclasses multclasses unix2classes testclasses
B1F     = READ_ME Bugs Makefile at top *.h comp.c image.c main.c process.c
B2F     = $(COMMONc) primitive.c interp.c
B3F	= $(classes) stest.out

default: parse st sunix
	echo "created single process version, see docs for more info"

#
# parse - the object image parser.  
# used to build the initial object image
#
parse: $(PARSEo)
	cc -o parse $(CFLAGS) $(PARSEo)

parseprint:
	pr *.h $(PARSEc) | lpr

parselint:
	lint $(PARSEc)

#
# st - the actual bytecode interpreter
# runs bytecodes from the initial image, or another image
#
st: $(STo)
	cc $(CFLAGS) -o st $(STo) -lm

stlint: 
	lint $(STc)

stprint:
	pr *.h $(STc) | lpr

report: memory.o report.o
	cc -o report memory.o report.o

#
# image - build the initial object image
#
classlpr:
	pr $(classes) | lpr

sunix: parse 
	cd imageSource && ../parse basicclasses unixclasses

munix: parse
	cd imageSource && ../parse basicclasses multclasses unix2classes

stest: parse
	cd imageSource && ../parse basicclasses unixclasses testclasses

mtest: parse
	cd imageSource && ../parse basicclasses multclasses unix2classes testclasses

#
# distribution bundles
#

bundles:
	bundle $(B1F) >bundle.1
	bundle $(B2F) >bundle.2
	bundle $(B3F) >bundle.3

tar:
	tar cvf ../smalltalk.v2.tar .
	compress -c ../smalltalk.v2.tar >../smalltalk.v2.tar.Z

clean:
	rm -f *.o *~ parse st imageFile
