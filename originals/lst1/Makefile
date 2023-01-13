install:
	echo "Installing Little Smalltalk System"
	cd parser ; make install
	cd sources ; make install
	cd prelude ; make install
	cd tests ; make install

clean:
	cd sources; make clean
	cd parser; make clean

tartape:
	tar cv Makefile unbundle *.bundle projects newsletters

800tartape:
	tar cvf /dev/rmt0 Makefile unbundle *.bundle projects newsletters

#	the following should be treated with some care!
gone:
	-cd bin ; rm *
	-cd parser ; rm *
	-cd prelude ; rm *
	-cd tests ; rm *
	-cd sources ; rm *
	-cd docs ; rm *
	rmdir bin parser prelude tests sources docs
	-rm -r projects
	rm *
