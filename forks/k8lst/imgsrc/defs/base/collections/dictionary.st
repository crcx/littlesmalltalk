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


COMMENT ----------Dictionary------------
METHODS FOR Dictionary
^new [
  | newDict |
  newDict := super new.
  self in: newDict at: 1 put: (OrderedArray new: 0).
  self in: newDict at: 2 put: (Array new: 0).
  ^newDict
]

noKey [
  self error: 'key not found in dictionary lookup'
]

at: key [
  ^self at: key ifAbsent: [ self noKey ]
]

at: key put: value [
  | position |
  position := keys location: key.
  (position <= keys size and: [ key = (keys at: position)])
    ifTrue: [ values at: position put: value ]
    ifFalse: [ keys := keys insert: key at: position.
      values := values insert: value at: position ].
  ^value
]

at: key ifAbsent: exceptionBlock [
  | position |
  position := keys location: key.
  ((position <= keys size) and: [ key = (keys at: position)])
    ifTrue: [ ^values at: position ]
    ifFalse: [ ^exceptionBlock value ]
]

keysAndValuesDo: aBlock [
  "[ :key :value | ... ]"
  1 to: keys size do: [:i | aBlock value: (keys at: i) value: (values at: i) ]
]

keysDo: aBlock [
  1 to: keys size do: [:i | aBlock value: (keys at: i)]
]

keysAsArray [
  | i ret |
  ret := Array new: keys size.
  1 to: keys size do: [:i | ret at: i put: (keys at: i)].
  ^ret
]

isEmpty [
  ^keys isEmpty
]

do: aBlock [
  values do: aBlock
]

removeKey: key ifAbsent: exceptionBlock [
  | position |
  position := keys location: key.
  (position <= keys size and: [ key = (keys at: position) ])
    ifTrue: [ keys := keys removeIndex: position.
      values := values removeIndex: position]
    ifFalse: [ ^exceptionBlock value ]
]

removeKey: key [
  ^self removeKey: key ifAbsent: [ self noKey ]
]

printString [
  | count res |
  res := self class printString + ' ('.
  count := 0.
  self keysAndValuesDo: [:k :elem |
    (count = 0) ifFalse: [ res := res + ', ' ].
    res := res + (k printString + ' -> ' + elem printString).
    count := count + 1.
    (count >= 20) ifTrue: [ ^res + ', ...)' ]
  ].
  ^res + ')'
]

add: anAssoc [
  self at: anAssoc key put: anAssoc value.
]
!
