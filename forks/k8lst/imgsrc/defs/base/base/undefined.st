COMMENTS
 Little Smalltalk, Version 5

 Copyright (C) 1987-2005 by Timothy A. Budd
 Copyright (C) 2007 by Charles R. Childers
 Copyright (C) 2005-2007 by Danny Reinhold
 Copyright (C) 2010 by Ketmar // Vampire Avalon

 ============================================================================
 This license applies to the virtual machine and to the initial image of
 the Little Smalltalk system and to all files in the Little Smalltalk
 packages except the files explicitly licensed with another license(s).
 ============================================================================
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
ENDCOMMENTS


CLASS UndefinedObject  Object

COMMENT -----------UndefinedObject--------------
METHODS FOR UndefinedObject
^new [
  "there is only one nil object"
  ^nil
]

^basicNew [
  "there is only one nil object"
  ^nil
]

printString [
  ^'nil'
]

asString [
  ^'nil'
]

isNil [
  "yes, we are nil"
  ^true
]

notNil [
  "no, we are not not-nil"
  ^false
]

ifNil: aBlock [
  ^aBlock value
]

ifNotNil: aBlock [
  ^nil
]

ifNil: aNilBlock ifNotNil: aNotBlock [
  ^aNilBlock value
]

ifNotNil: aNotBlock ifNil: aNilBlock [
  ^aNilBlock value
]

| aVal [
  ^aVal
]

& aVal [
  ^false
]
!
