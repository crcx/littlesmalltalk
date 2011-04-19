# Makefile for Little Smalltalk, Version 5
#

#
# Basic feature configuration
#
USE_GUI=no
USE_FFI=yes
USE_SOCKET=yes


#
# Basic compiler configuration (should be ok)
#
CC=gcc
CPPFLAGS=
CFLAGS=-g -Wall $(CPPFLAGS)
LDFLAGS_EXTRA=
LDFLAGS=$(LDFLAGS_EXTRA)


#
# Common dependencies and source list
#
DEPS = source/interp.h source/memory.h source/lst_primitives.h
SOURCES = source/interp.c source/main.c source/memory.c source/lst_primitives.c


#
# Add additional compiler flags and dependencies for FFI (if enabled)
#
ifeq (yes, $(USE_FFI))
DEPS+= source/lst_ffi_primitives.h source/rss_dynload.h source/rss_queue.h
SOURCES += source/lst_ffi_primitives.c source/rss_dynload.c source/rss_queue.c
CFLAGS+=-DLST_USE_FFI=1
endif


#
# Add additional compiler flags and dependencies for sockets (if enabled)
#
ifeq (yes, $(USE_SOCKET))
SOURCES += source/lst_socket_primitives.c
CFLAGS+=-DLST_USE_SOCKET=1
endif


#
# Add additional compiler flags and dependencies for gui (if enabled)
#
ifeq (yes, $(USE_GUI))
CFLAGS+= -I thirdparty/include -DLST_USE_GUI=1
LDFLAGS+= -lXm -L thirdparty -liup
endif


#
# This section adds specific flags needed based
# on OS and architecture.
#
UNAME_O=$(shell uname -s)
UNAME_M=$(shell uname -m)
ifeq (x86_64,$(UNAME_M))
CFLAGS+=-m32
LDFLAGS+=-m32
endif
ifneq ($(UNAME_O), $(filter MinGW Msys, $(UNAME_O)))
LDFLAGS+=-ldl
endif


#
# Rules for actually building Little Smalltalk
#

.PHONY: all image clean

all: bin/st image

bin/st: $(SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/imageBuilder: ImageBuilder/imageBuilder.o
	$(CC) -o $@ $^ $(LDFLAGS)

bin/LittleSmalltalk.image: bin/imageSource bin/imageBuilder
	$ cd bin && ./imageBuilder >imageBuilder.log

image: bin/LittleSmalltalk.image

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -f `find . | grep \~`
	@rm -f source/*.o ImageBuilder/*.o bin/*.o
	@rm -f bin/imageBuilder bin/LittleSmalltalk.image bin/st bin/st_noffi bin/*.log

query-%:
	@echo $($(*))
