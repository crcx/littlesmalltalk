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


CLASS Context  Object  method arguments temporaries stack bytePointer stackTop previousContext procOwner

COMMENT -----------Context--------------
METHODS FOR Context
setup: aMethod withArguments: a [
  method := aMethod.
  ((a isNil) or: [ a size < 1 ])
    ifTrue: [ arguments := Array with: UndefinedObject ]
    ifFalse: [ arguments := a ].
  bytePointer := 0.
  stack := Array new: method stackSize.
  stackTop := 0.
  temporaries := Array new: method temporarySize.
  previousContext := nil.
]

setProcessOwner: aProc [
  procOwner := aProc.
]

processOwner [
  ^procOwner
]

perform: aMethod withArguments: a [
  | proc |
  self setup: aMethod withArguments: a.
  (proc := Process new) context: self.
  ^proc execute
]

perform: aMethod withSelf: aSelf [
  ^self perform: aMethod withArguments: (Array with: aSelf).
]

previousContext [
  ^previousContext
]

method [
  ^method
]

bytePointer [
  ^bytePointer
]

stack [
  ^stack
]

stackTop [
  ^stackTop
]

temporaries [
  ^temporaries
]

arguments [
  ^arguments
]

backtraceShowHeader [
  method name print.
]

backtraceShowArgs [
  | narg |
  narg := 0.
  ' (' print.
  arguments do: [:a |
    (narg > 0) ifTrue: [ ', ' print ].
    a class print.
    narg := narg + 1.
  ].
  ')' print.
]

backtrace [
  "backtrace context calls"
  | line |
  ' ' print.
  (bytePointer isKindOf: Number) ifTrue: [
    (bytePointer printWidth: 4) print.
    (line := method findLineForPC: bytePointer)
      ifNotNil: [ ':(' print. line print. ') ' print ]
      ifNil: [ ': ' print ].
  ].
  self backtraceShowHeader.
  self backtraceShowArgs.
  '\n' print.
  previousContext ifNotNil: [ previousContext backtrace ]
]
!


METHODS FOR Class
perform: aName withSelf: aSelf withArguments: a [
  | meth args |
  (aName isKindOf: Method) ifTrue: [ meth := aName ] ifFalse: [ meth := self findMethodInAll: aName ].
  ((a isNil) or: [ a size < 1])
    ifTrue: [ (args := Array new: 1) at: 1 put: aSelf ]
    ifFalse: [ args := a insert: aSelf at: 1 ].
  ^Context new perform: meth withArguments: args
]
!
