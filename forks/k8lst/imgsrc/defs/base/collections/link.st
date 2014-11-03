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


CLASS Link  Object  value next

METHODS FOR Link
^value: v [
  "return a new link with given value field and empty link field"
  ^self in: self new at: 1 put: v
]

^value: v next: n [
  "return a new link with the given fields"
  | obj |
  obj := self new.
  self in: obj at: 1 put: v.
  self in: obj at: 2 put: n.
  ^obj
]

value [
  ^value
]

value: val [
  value := val
]

next [
  ^next
]

next: aNext [
  next := aNext
]

do: aBlock [
  aBlock value: value.
  next ifNotNil: [ ^next do: aBlock ]
]

remove: anElement ifAbsent: exceptionBlock [
  "simple heap eater; not used anyway"
  value = anElement
    ifTrue: [ ^next ]
    ifFalse: [
      next
        ifNotNil: [ next := next remove: anElement ifAbsent: exceptionBlock. ^self ]
        ifNil: [ ^exceptionBlock value ]
    ]
]

reverseDo: aBlock [
  "eats heap like crazy"
  next ifNotNil: [ next reverseDo: aBlock ].
  aBlock value: value
]

addLast: anElement [
  "heap eater"
  next
    ifNotNil: [ ^next addLast: anElement ]
    ifNil: [ next := Link value: anElement ]
]
!
