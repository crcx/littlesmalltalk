COMMENT The Smalltalk source code of the initial image.
COMMENT Must be compiled with the image builder tools to
COMMENT create a binary image for the Little Smalltalk
COMMENT virtual machine.
COMMENT
COMMENT ---------------------------------------------------------------
COMMENT Little Smalltalk, Version 5
COMMENT
COMMENT Copyright (C) 1987-2005 by Timothy A. Budd
COMMENT Copyright (C) 2007 by Charles R. Childers
COMMENT Copyright (C) 2005-2007 by Danny Reinhold
COMMENT
COMMENT ============================================================================
COMMENT This license applies to the virtual machine and to the initial image of
COMMENT the Little Smalltalk system and to all files in the Little Smalltalk
COMMENT packages except the files explicitly licensed with another license(s).
COMMENT ============================================================================
COMMENT Permission is hereby granted, free of charge, to any person obtaining a copy
COMMENT of this software and associated documentation files (the "Software"), to deal
COMMENT in the Software without restriction, including without limitation the rights
COMMENT to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
COMMENT copies of the Software, and to permit persons to whom the Software is
COMMENT furnished to do so, subject to the following conditions:
COMMENT
COMMENT The above copyright notice and this permission notice shall be included in
COMMENT all copies or substantial portions of the Software.
COMMENT
COMMENT THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
COMMENT IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
COMMENT FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COMMENT AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
COMMENT LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
COMMENT FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
COMMENT DEALINGS IN THE SOFTWARE.
COMMENT
COMMENT
COMMENT -----------------------------------------------------------
COMMENT RAWCLASS name   instanceOf subclassOf  (instance variable names)
COMMENT  or... CLASS name subclassOf (instance vars)
COMMENT RAWCLASS doesn't automatically build Meta class; CLASS does
COMMENT -----------------------------------------------------------
COMMENT
COMMENT -----------------------------------------------------------
COMMENT Object subclass: System  cvar0 ...  [ | ivar0 ... ]
COMMENT -----------------------------------------------------------
COMMENT

PACKAGE System 1
MODULE defs/base

PACKAGE LstCompiler 1 REQUIRES System 1
MODULE defs/compiler

PACKAGE Files 1 REQUIRES System 1 LstCompiler 1
MODULE defs/file

PACKAGE GstParser 1 REQUIRES System 1 LstCompiler 1
MODULE defs/gstparser

PACKAGE HostOS 1 REQUIRES System 1
MODULE defs/system

COMMENT PACKAGE HostOS 1 REQUIRES System 1
COMMENT MODULE defs/ffi

COMMENT PACKAGE Image 1 REQUIRES HostOS 1
COMMENT MODULE defs/image


PACKAGE Repl 1 REQUIRES System 1
INCLUDE repl
COMMENT INCLUDE transcript


COMMENTS
CLASS ZTest  Object

METHODS FOR ZTest
test [
  [ [ 5 ifNil: [
          ^nil ].
    ] whileTrue: [ nil ].
    6 ] whileTrue: [
     7 ].
  ^self
]
!
ENDCOMMENTS


INCLUDE entries
END
