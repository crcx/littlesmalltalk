#!/bin/sh
CC="gcc -pipe"
OPT="-O2 -Wall"
DEFS="-DLST_COMPACT_WRITE_WORD -DLST_USE_SOCKET=1 -DLST_USE_FFI=1"
INCS="-Isrc/lstcore -I./src"

rm lst imgbuilder
rm -Rf ./_build
mkdir -p ./_build/lib

$CC -c -o ./_build/lst_imgio.o $OPT $DEFS $INCS src/lstcore/lst_imgio.c
$CC -c -o ./_build/lst_interp.o $OPT $DEFS $INCS src/lstcore/lst_interp.c
ar ru ./_build/lib/liblstcore.a ./_build/lst_imgio.o ./_build/lst_interp.o
ranlib ./_build/lib/liblstcore.a
rm -f ./_build/lst_imgio.o ./_build/lst_interp.o
$CC -c -o ./_build/lstpl_stdlib.o $OPT $DEFS -Isrc/primlib/stdlib -I./src src/primlib/stdlib/lstpl_stdlib.c
ar ru ./_build/lib/liblstpl_stdlib.a ./_build/lstpl_stdlib.o
ranlib ./_build/lib/liblstpl_stdlib.a
rm -f ./_build/lstpl_stdlib.o
$CC -c -o ./_build/lstpl_ffi.o $OPT $DEFS -Isrc/primlib/ffi -I./src src/primlib/ffi/lstpl_ffi.c
ar ru ./_build/lib/liblstpl_ffi.a ./_build/lstpl_ffi.o
ranlib ./_build/lib/liblstpl_ffi.a
rm -f ./_build/lstpl_ffi.o
$CC -c -o ./_build/lstpl_socket.o $OPT $DEFS -Isrc/primlib/socket -I./src src/primlib/socket/lstpl_socket.c
ar ru ./_build/lib/liblstpl_socket.a ./_build/lstpl_socket.o
ranlib ./_build/lib/liblstpl_socket.a
rm -f ./_build/lstpl_socket.o

$CC -c -o ./_build/hsregexp.o $OPT $DEFS -Isrc/primlib/relib -I./src src/primlib/relib/hsregexp.c
ar ru ./_build/lib/libhsrelib.a ./_build/hsregexp.o
ranlib ./_build/lib/libhsrelib.a
rm -f ./_build/hsregexp.o

$CC -c -o ./_build/trex.o $OPT $DEFS -Isrc/primlib/relib -I./src src/primlib/relib/trex.c
ar ru ./_build/lib/libtrex.a ./_build/trex.o
ranlib ./_build/lib/libtrex.a
rm -f ./_build/trex.o

$CC -c -o ./_build/lstpl_regexp.o $OPT $DEFS -Isrc/primlib/relib -I./src src/primlib/relib/lstpl_regexp.c
ar ru ./_build/lib/liblstpl_relib.a ./_build/lstpl_regexp.o
ranlib ./_build/lib/liblstpl_relib.a
rm -f ./_build/lstpl_regexp.o


$CC -c -o ./_build/main.o $OPT $DEFS -Isrc -I./src src/main.c
$CC -s -o lst ./_build/main.o ./_build/lib/liblstcore.a ./_build/lib/liblstpl_stdlib.a \
  ./_build/lib/liblstpl_ffi.a ./_build/lib/liblstpl_socket.a \
  ./_build/lib/liblstpl_relib.a ./_build/lib/libhsrelib.a ./_build/lib/libtrex.a \
  -ldl -lrt -lm
chmod 755 lst
$CC -c -o ./_build/imgbuild.o $OPT $DEFS -Isrc/builder -I./src src/builder/imgbuild.c
$CC -s -o imgbuilder  ./_build/imgbuild.o
chmod 755 imgbuilder

./imgbuilder
