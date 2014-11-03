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


CLASS Node  Object  value left right

METHODS FOR Node
^new: value [
  "creation, left left and right empty"
  ^self in: self new at: 1 put: value
]

do: aBlock [
  left ifNotNil: [ left do: aBlock ].
  aBlock value: value.
  ^right ifNotNil: [ right do: aBlock ]
]

reverseDo: aBlock [
  right ifNotNil: [ right do: aBlock ].
  aBlock value: value.
  left ifNotNil: [ left do: aBlock ]
]

first [
  ^left ifNotNil: [ left first ] ifNil: [ value ]
]

removeFirst [
  left
    ifNotNil: [ left := left removeFirst. ^self ]
    ifNil: [ ^right ]
]

add: anElement [
  value < anElement
    ifTrue: [
      right
        ifNotNil: [ right add: anElement ]
        ifNil: [ right := Node new: anElement ] ]
    ifFalse: [
      left
        ifNotNil: [ left add: anElement ]
        ifNil: [ left := Node new: anElement ] ]
]

remove: key ifAbsent: exceptionBlock [
  value = key
    ifTrue: [
      right
        ifNotNil: [ value := right first. right := right removeFirst. ^self ]
        ifNil: [ ^left ]].
  value < key
    ifTrue: [
      right
        ifNotNil: [ right := right remove: key ifAbsent: exceptionBlock ]
        ifNil: [ ^exceptionBlock value ] ]
    ifFalse: [
      left
        ifNotNil: [ left := left remove: key ifAbsent: exceptionBlock ]
        ifNil: [ ^exceptionBlock value ] ]
]

value [
  ^value
]

at: key ifAbsent: exceptionBlock [
  value = key ifTrue: [ ^value ].
  value < key
    ifTrue: [
      right
        ifNotNil: [ ^right at: key ifAbsent: exceptionBlock ]
        ifNil: [ ^exceptionBlock value ] ]
    ifFalse: [
      left
        ifNotNil: [ ^left at: key ifAbsent: exceptionBlock ]
        ifNil: [ ^exceptionBlock value ] ]
]
!
