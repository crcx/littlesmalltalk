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


COMMENT -----------Method--------------
METHODS FOR Method
^name: n byteCodes: b literals: l stackSize: s temporarySize: ts class: c text: t [
  | newMethod |
  newMethod := self new.
  super in: newMethod at: 1 put: n.
  super in: newMethod at: 2 put: b.
  super in: newMethod at: 3 put: l.
  super in: newMethod at: 4 put: s.
  super in: newMethod at: 5 put: ts.
  super in: newMethod at: 6 put: c.
  super in: newMethod at: 7 put: t.
  super in: newMethod var: #invokeCount put: 0.
  ^newMethod
]

byteCodes [
  ^byteCodes
]

literals [
  ^literals
]

text [
  ^text
]


category [
  ^category
]

category: aCatStr [
  category := aCatStr
]

name [
  ^name
]

stackSize [
  ^stackSize
]

temporarySize [
  ^temporarySize
]

forClass [
  ^class
]

argCount [
  ^argCount
]

argCount: ac [
  "internal function"
  argCount := ac
]

argNames [
  ^argNames
]

tempNames [
  ^tempNames
]

debugInfo [
  ^debugInfo
]

debugInfo: aDbgInfo [
  debugInfo := aDbgInfo
]

findLineForPC: aPC [
  "return line number or nil"
  | idx ostart oend line s e |
  debugInfo ifNil: [ ^nil ].
  line := nil. idx := 1. ostart := 0. oend := byteCodes size.
  [ (idx + 4) < debugInfo size ] whileTrue: [
    s := debugInfo wordAt: idx + 2.
    e := debugInfo wordAt: idx + 4.
    (aPC between: s and: e) ifTrue: [
      ((s > ostart) or: [ e < oend ]) ifTrue: [
        line := debugInfo wordAt: idx.
        ostart := s.
        oend := e.
      ].
    ].
    idx := idx + 6.
  ].
  ^line
]

findPCForLine: aLine [
  "find the lowes PC for the given line"
  | idx pc line s lineHit |
  debugInfo ifNil: [ ^nil ].
  pc := -1. idx := 1. lineHit := 0.
  [ (idx + 4) < debugInfo size ] whileTrue: [
    s := debugInfo wordAt: idx + 2.
    (line := debugInfo wordAt: idx) = aLine ifTrue: [
      lineHit := line.
      pc < s ifTrue: [ pc := s ].
    ].
    ((line < aLine) and: [ line > lineHit ]) ifTrue: [
      lineHit := line - 1.  "we can encounter this line again"
      pc < s ifTrue: [ pc := s ].
    ].
    idx := idx + 6.
  ].
  pc < 0 ifTrue: [ ^nil ].
  ^pc
]

args: aArgNames inst: aInstNames temp: aTempNames [
  "Hook for recording symbolic debug"
  "1st arg is always 'self'"
  (argNames := aArgNames from: 2) isEmpty ifTrue: [ argNames := nil ].
  tempNames := aTempNames isEmpty ifTrue: [ nil ] ifFalse: [ aTempNames ].
]
!
