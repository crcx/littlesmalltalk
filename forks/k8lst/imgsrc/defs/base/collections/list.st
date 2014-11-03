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


COMMENT ---------List-----------
METHODS FOR List
^new [
  | obj |
  obj := self basicNew.
  self in: obj at: 3 put: 0.
  ^obj
]

^with: elemA [
  | newList |
  (newList := self new) add: elemA.
  ^newList
]

^with: elemA with: elemB [
  | newList |
  (newList := self new); add: elemB; add: elemA.
  ^newList
]

^with: elemA with: elemB with: elemC [
  | newList |
  (newList := self new); add: elemC; add: elemB; add: elemA.
  ^newList
]

size [
  ^count
]

removeAll [
  "remove all elements"
  head := tail := nil.
  count := 0.
]

add: anElement [
  head := Link value: anElement next: head.
  tail ifNil: [ tail := head. count := 1 ] ifNotNil: [ count := count + 1 ].
  ^anElement
]

"little faster than recursive version"
addLast: anElement [
  | t |
  t := Link value: anElement.
  tail ifNotNil: [
    tail next: t.
    tail := t.
    count := count + 1.
  ] ifNil: [
    head := tail := t.
    count := 1.
  ].
  ^anElement
]

addAll: aCollection [
  aCollection do: [:element | self addLast: element]
]

+ aItem [
  "makes a copy"
  | obj |
  obj := self copy.
  obj << aItem.
  ^obj
]

<< aItem [
  "modifies in-place"
  (aItem isKindOf: List)
    ifTrue: [ self addAll: aItem ]
    ifFalse: [ self addLast: aItem ]
]

copy [
  ^self asList
]

badIndex [
  self error: 'Invalid List index'
]

findLink: index ifAbsent: aBlock [
  | link |
  link := head.
  [ link ] whileNotNil: [
    (index := index - 1) = 0 ifTrue: [ ^link ].
    link := link next.
  ].
  ^aBlock value
]

at: index [
  ^(self findLink: index ifAbsent: [ ^self badIndex ]) value
]

at: index ifAbsent: aBlock [
  ^(self findLink: index ifAbsent: [ ^aBlock value ]) value
]

at: index put: value [
  (self findLink: index ifAbsent: [ ^self badIndex ]) value: value
]

first [
  head ifNil: [ ^nil ].
  ^head value
]

last [
  tail ifNil: [ ^nil ].
  ^tail value
]

next: aItem [
  | link |
  link := head.
  [ link ] whileNotNil: [
    link value = aItem ifTrue: [
      ^(link := link next) ifNotNil: [ link value ]
    ].
    link := link next.
  ].
  ^nil
]

prev: aItem [
  | link prev |
  prev := nil. link := head.
  [ link ] whileNotNil: [
    link value = aItem ifTrue: [
      ^prev ifNotNil: [ prev value ]
    ].
    prev := link.
    link := link next.
  ].
  ^nil
]

isEmpty [
  ^head isNil
]

removeFirst [
  head ifNil: [ self emptyCollection ].
  (head := head next) ifNil: [ tail := nil ].
  count := count - 1.
]

removeLast [
  tail ifNil: [ self emptyCollection ].
  self remove: tail value
]

remove: anElement ifAbsent: exceptionBlock [
  | link prev |
  link := head.
  [ link ] whileNotNil: [
    link value = anElement ifTrue: [
      count := count - 1.
      prev ifNotNil: [ prev next: link next ] ifNil: [ head := link next ].
      link == tail ifTrue: [ tail := prev ].
      ^self
    ].
    prev := link.
    link := link next.
  ].
  ^exceptionBlock value
]

remove: anElement [
  self remove: anElement ifAbsent: [ self emptyCollection ]
]

reverse [
  | newList |
  newList := List new.
  self do: [:element | newList add: element ].
  ^newList
]

do: aBlock [
  ^head ifNotNil: [ head do: aBlock ]
]

reverseDo: aBlock [
  ^head ifNotNil: [ head reverseDo: aBlock ]
]
!
