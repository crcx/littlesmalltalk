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


CLASS Boolean  Object
CLASS True     Boolean
CLASS False    Boolean

COMMENT -----------Boolean--------------
METHODS FOR Boolean
and: aBlock [
  ^self
    ifTrue: [ aBlock value ]
    ifFalse: [ false ]
]

or: aBlock [
  ^self ifTrue: [ true ] ifFalse: [ aBlock value ]
]

| aBool [
  ^self ifTrue: [ true ] ifFalse: [ aBool ]
]

& aBool [
  ^self ifTrue: [ aBool ] ifFalse: [ false ]
]

not [
  ^self ifTrue: [ false ] ifFalse: [ true ]
]

ifFalse: falseBlock ifTrue: trueBlock [
  ^self ifTrue: [ trueBlock  value ] ifFalse: [ falseBlock value ]
]

ifTrue: trueBlock ifFalse: falseBlock [
  ^self ifTrue: [ trueBlock  value ] ifFalse: [ falseBlock value ]
]

ifTrue: aBlock [
  ^self ifTrue: [ aBlock value ] ifFalse: [ false ]
]

ifFalse: aBlock [
  ^self ifTrue: [ true ] ifFalse: [ aBlock value ]
]
!


COMMENT -----------True--------------
METHODS FOR True
^new [
  "there is only one true value"
  ^true
]

not [
  ^false
]

asString [
  ^'true'
]

printString [
  ^'true'
]

ifTrue: trueBlock ifFalse: falseBlock [
  ^trueBlock value
]

ifFalse: falseBlock ifTrue: trueBlock [
  ^trueBlock value
]

or: aBlock [
  ^true
]

and: aBlock [
  ^aBlock value
]

| aBool [
  ^true
]

& aBool [
  ^aBool
]
!


COMMENT -----------False--------------
METHODS FOR False
^new [
  "there is only one false value"
  ^false
]

not [
  ^true
]

asString [
  ^'false'
]

printString [
  ^'false'
]

ifTrue: trueBlock ifFalse: falseBlock [
  ^falseBlock value
]

ifFalse: falseBlock ifTrue: trueBlock [
  ^falseBlock value
]

or: aBlock [
  ^aBlock value
]

and: aBlock [
  ^false
]

| aBool [
  ^aBool
]

& aBool [
  ^false
]
!
