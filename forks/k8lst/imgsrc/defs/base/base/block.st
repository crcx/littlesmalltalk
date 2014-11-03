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


CLASS Block  Context  argumentLocation creatingContext oldBytePointer argumentCount

COMMENT ---------- Block ------------
METHODS FOR Block
argCount [
  self error: 'Incorrect argument passing to Block'
]

value [
  "start block execution"
  <#BlockInvocation self>.
  self argCount.
]

runLocked [
  "execute block with process lock"
  | res |
  Process lock.
  res := self value.
  Process unlock.
  ^res
]

value: a [
  "start block execution"
  <#BlockInvocation a self>.
  self argCount.
]

value: a value: b [
  "start block execution"
  <#BlockInvocation a b self>.
  self argCount.
]

value: a value: b value: c [
  "start block execution"
  <#BlockInvocation a b c self>.
  self argCount.
]

value: a value: b value: c value: d [
  "start block execution"
  <#BlockInvocation a b c d self>.
  self argCount.
]

whileTrue: aBlock [
  self value ifTrue: [ aBlock value. ^self whileTrue: aBlock ].
  ^nil
]

whileFalse: aBlock [
  self value ifFalse: [ aBlock value. ^self whileFalse: aBlock ].
  ^nil
]

whileNil: aBlock [
  self value ifNil: [ aBlock value. ^self whileNil: aBlock ].
  ^nil
]

whileNotNil: aBlock [
  self value ifNotNil: [ aBlock value. ^self whileNotNil: aBlock ].
  ^nil
]

argumentCount [
  ^argumentCount
]

backtraceShowHeader [
  'block from ' print. method name print.
]
!
