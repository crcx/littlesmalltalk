.SUFFIXES : .st .p
PREPATH = .
BINDIR = ../../bin

PARSED = class.p object.p \
string.p larray.p nil.p array.p\
boolean.p true.p false.p block.p symbol.p \
magnitude.p number.p integer.p char.p float.p radian.p point.p random.p \
collection.p bag.p set.p kcollection.p dictionary.p scollection.p interval.p \
list.p acollection.p file.p bytearray.p \
semaphore.p process.p smalltalk.p

UNPARSED = class.st object.st \
string.st larray.st nil.st array.st\
boolean.st true.st false.st block.st symbol.st \
magnitude.st number.st integer.st char.st float.st radian.st point.st random.st \
collection.st bag.st set.st kcollection.st dictionary.st scollection.st interval.st \
list.st acollection.st file.st bytearray.st \
semaphore.st process.st smalltalk.st

.st.p:
	$(BINDIR)/parse $(PREPATH)/$*.st >$*.p

install: standard
	-make fastsave

bundle: *.st Makefile savescript
	bundle Makefile savescript init *.st >../prelude.bundle

standard: $(PARSED)
	cat $(PARSED) init >standard

newstd: $(PARSED)
	cat $(PARSED) >newstd

fop:
	cat $(UNPARSED) >crc

fastsave: standard
	$(BINDIR)/st -m <savescript

clean:
	-rm *.p

# the following are libraries that can be included using the -g switch
#	or using the )g directive

#  form - simple ascii graphics using the curses routines
form: form.p
	mv form.p form

#  pen - line drawing with the plot(3) routines
pen: pen.p
	mv pen.p pen
