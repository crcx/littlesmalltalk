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


COMMENT ---------- Array ------------
METHODS FOR Array
^new [
  ^self new: 0
]

^new: sz [
  <#NewObject self sz>.
  self primitiveFailed
]

^with: elemA [
  | ret |
  (ret := self new: 1) at: 1 put: elemA.
  ^ret
]

^with: elemA with: elemB [
  | ret |
  ret := self new: 2;
    at: 1 put: elemA;
    at: 2 put: elemB.
  ^ret
]

^with: elemA with: elemB with: elemC [
  | ret |
  ret := self new: 3;
    at: 1 put: elemA;
    at: 2 put: elemB;
    at: 3 put: elemC.
  ^ret
]

badIndex [
  self error: 'array indexing error'
]

at: index [
  <#Array#at: self index>.
  (self includesKey: index) ifFalse: [ self badIndex ].
  self primitiveFailed
]

at: index ifAbsent: exceptionBlock [
  <#Array#at: self index>.
  ^exceptionBlock value
]

includes: aValue [
  self do: [:element | element = aValue ifTrue: [ ^true ]].
  ^false
]

indexOf: aValue ifAbsent: aBlock [
  1 to: self size do: [:idx | ((self at: idx) == aValue) ifTrue: [ ^idx ]].
  ^aBlock value
]

indexOf: aValue [
  1 to: self size do: [:idx | ((self at: idx) == aValue) ifTrue: [ ^idx ]].
  ^nil
]

indexOfVal: aValue ifAbsent: aBlock [
  1 to: self size do: [:idx | ((self at: idx) = aValue) ifTrue: [ ^idx ]].
  ^aBlock value
]

indexOfVal: aValue [
  1 to: self size do: [:idx | ((self at: idx) = aValue) ifTrue: [ ^idx ]].
  ^nil
]

highestIndexOf: aValue [
  | i |
  i := nil.
  1 to: self size do: [:idx | ((self at: idx) = aValue) ifTrue: [ i := idx ]].
  ^i
]

atAllPut: aValue [
  1 to: self size do: [:i | self at: i put: aValue ]
]

copy [
  ^self asArray
]

with: newItem [
  | newArray size |
  size := self size.
  newArray := self class new: size + 1.
  newArray replaceFrom: 1 to: size with: self.
  newArray at: size + 1 put: newItem.
  ^newArray
]

withFirst: newItem [
  | newArray size |
  size := self size.
  newArray := self class new: size + 1.
  newArray replaceFrom: 2 to: size + 1 with: self.
  newArray at: 1 put: newItem.
  ^newArray
]

basePlus: aValue [
  "concatenate two strings/arrays together"
  | size1 size2 newValue |
  size1 := self size.
  size2 := aValue size.
  newValue := self class new: (size1 + size2).
  newValue replaceFrom: 1 to: size1 with: self.
  newValue replaceFrom: size1+1 to: size1+size2 with: aValue.
  ^newValue
]

+ aValue [
  ^self basePlus: aValue
]

size [
  "compute number of elements"
  <#ObjectSize self>
]

at: index put: value [
  <#Array#at:put: value self index>.
  (self includesKey: index) ifFalse: [ self badIndex ].
  self primitiveFailed
]

do: aBlock [
  1 to: self size do: [:i | aBlock value: (self at: i)]
]

transform: aBlock [
  1 to: self size do: [:i | self at: i put: (aBlock value: (self at: i))]
]

< arg [
  | selfsize argsize |
  selfsize := self size. argsize := arg size.
  1 to: (selfsize min: argsize)
    do: [:i | (self at: i) ~= (arg at: i) ifTrue: [ ^(self at: i) < (arg at: i) ]].
  ^selfsize < argsize
]

= anArray [
  self size = anArray size ifFalse: [ ^false ].
  1 to: self size do: [:i | (self at: i) = (anArray at: i) ifFalse: [ ^false ]].
  ^true
]

includesKey: index [
  ^index between: 1 and: self size
]

insert: value at: position [
  | newArray newSize |
  newSize := self size + 1.
  newArray := self class new: newSize.
  newArray replaceFrom: 1 to: position-1 with: self.
  newArray at: position put: value.
  newArray replaceFrom: position+1 to: newSize with: self startingAt: position.
  ^newArray
]

removeIndex: position [
  | newArray newSize |
  newSize := self size - 1.
  newArray := self class new: newSize.
  newArray replaceFrom: 1 to: position-1 with: self.
  newArray replaceFrom: position to: newSize with: self startingAt: position+1.
  ^newArray
]

first [
  ^self at: 1
]

from: low to: high [
  | start stop size obj |
  start := low max: 0.
  stop := high min: self size.
  size := (stop + 1 - start) max: 0.
  obj := (self species) new: size.
  1 to: size do: [:i |
    obj at: i put: (self at: start).
    start := start + 1 ].
  ^obj
]

from: low [
  ^self from: low to: self size
]

hash [
  | sz |
  (sz := self size) < 2 ifTrue: [ sz = 1 ifTrue: [ ^((self at: 1) hash + sz) bitAnd: 1073741823 ]. ^0 ].
  ^((self at: 1) hash + (self at: sz) hash) bitAnd: 1073741823
]

elementsExchangeIdentityWith: otherArray [
  <#BulkObjectExchange self otherArray>.
  self primitiveFailed
]

replaceFrom: start to: stop with: replacement [
  ^self replaceFrom: start to: stop with: replacement startingAt: 1
]

replaceFrom: start to: stop with: replacement startingAt: repStart [
  <#replaceFrom:to:with:startingAt: start stop replacement repStart self>.
  0 to: (stop - start) do: [:idx | self at: (idx + start) put: (replacement at: (idx + repStart)) ]
]
!
