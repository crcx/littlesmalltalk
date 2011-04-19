CC=gcc
CFLAGS=-Wall -g -I../source

image: imageBuilder imageSource
	./imageBuilder <imageSource

imageBuilder: imageBuilder.o
	rm -f imageBuilder
	$(CC) -o imageBuilder imageBuilder.o

jconv.class: jconv.java
	javac jconv.java

binaryImage:
	java jconv <image

# always clean up after yourself
clean:
	rm -f imageBuilder.o imageBuilder image

mv:
	mv binaryImage ~/Java
