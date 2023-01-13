.SUFFIXES : .st .test

BINDIR = ../bin

FILES = Makefile in *.st *.out

.st.test:
	$(BINDIR)/st -m $*.st <in | diff - $*.out

install:
	echo Performing Self Checking Tests
	-make basic.test
	-make blocks.test
	-make fork.test
	-make new.test
	-make super.test
	-make copy.test
	-make num.test
	-make file.test
	-make primes.test
	-make collect.test
	-make 4queen.test
	echo The following produce cycles, thus have nonzero differences
	-make phil.test
	echo Differences in random numbers may change results in following
	-make sim1.test
	-make sim2.test
	echo Finished Self Checking Tests
	
bundle:
	bundle $(FILES) >../tests.bundle

# if the CURSES routines are available, and the form library has been
# built in the /prelude subdirectory (see Makefile there), the following
# executes the plane example
plane:
	$(BINDIR)/st -m -g form plane.st <in

# if the PLOT(3) routines are available, and the pen library has been
# built in the /prelude subdirectory (see Makefile there), the following
# executes the pens exame
pen:
	$(BINDIR)/st -m -g pen penshow.st <in
