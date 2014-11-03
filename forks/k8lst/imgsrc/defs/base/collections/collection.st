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


COMMENT ----------Collection------------
METHODS FOR Collection
noElement [
  self error: 'Element not present'
]

size [
  | tally |
  tally := 0.
  self do: [:i | tally := tally + 1].
  ^tally
]

isEmpty [
  "empty if there are no elements"
  ^self size = 0
]

at: value ifAbsent: exceptionBlock [
  self do: [:element | element = value ifTrue: [ ^element ]].
  ^exceptionBlock value
]

at: value [
  ^self at: value ifAbsent: [ self noElement ]
]

includes: value [
  self at: value ifAbsent: [ ^false ].
  ^true
]

asArray [
  | newArray index |
  newArray := Array new: self size.
  index := 1.
  self do: [:element | newArray at: index put: element. index := index + 1 ].
  ^newArray
]

from: argLow to: argHigh [
  | ret idx size base low high |
  low := argLow max: 1.
  high := argHigh min: self size.
  size := (high - low) + 1.
  (size < 1) ifTrue: [ ^Array new: 0 ].
  ret := Array new: size.
  base := idx := 1.
  self do: [:elem |
    ((idx >= low) and: [idx <= high]) ifTrue: [
      ret at: base put: elem.
      base := base + 1.
      (base > size) ifTrue: [ ^ret ]
    ].
    idx := idx + 1.
  ].
  ^ret
]

from: argLow [
  ^self from: argLow to: (self size)
]

asString [
  | newString index |
  newString := String new: self size.
  index := 1.
  self do: [:element | newString at: index put: element. index := index + 1 ].
  ^newString
]

asList [
  ^List new addAll: self
]

collect: transformBlock [
  "returns a new list holding the values answered by block"
  | newList |
  newList := List new.
  self do: [:x | newList << (transformBlock value: x)].
  ^newList
]

detect: aBlock ifNone: aNoneBlock [
  "answers first element causing block to answer true"
  self do: [:x | (aBlock value: x) ifTrue: [ ^x ]].
  ^aNoneBlock value
]

detect: aBlock [
  "answers first element causing block to answer true"
  ^self detect: aBlock ifNone: [ ^self noElement ]
]

inject: aValue into: aBlock [
  self do: [:x | aValue := aBlock value: aValue value: x ].
  ^aValue
]

reject: testBlock [
  "select the things that do not match predicate"
  ^self select: [:x | (testBlock value: x) not ]
]

select: testBlock [
  "answer a new list holding each element for which block answers true"
  | newList |
  newList := List new.
  self do: [:x | (testBlock value: x) ifTrue: [newList << x]].
  ^newList
]

< aCollection [
  self do: [:element | (aCollection includes: element)
    ifFalse: [ ^false ] ].
  ^true
]

= aCollection [
  ^self < aCollection and: [ aCollection < self ]
]

printString [
  | count res |
  res := super printString.
  (self respondsTo: #do:) ifFalse: [ ^res ].
  count := 0.
  res := res + ' ('.
  self basicDo: [:elem |
    (count = 0) ifFalse: [ res := res + ' ' ].
    res := res + elem printString.
    count := count + 1.
    (count >= 20) ifTrue: [ ^res + ' ...)' ]
  ].
  ^res + ')'
]

occurencesOf: obj [
  | count |
  count := 0.
  self do: [:o | (o = obj) ifTrue: [ count := count + 1]].
  ^count
]

anyOne [
  self do: [:it | ^it].
  self emptyCollection
]

emptyCollection [
  self error: (self class printString + ' is empty')
]

do: aBlock [
  self subclassResponsibility
]
!
