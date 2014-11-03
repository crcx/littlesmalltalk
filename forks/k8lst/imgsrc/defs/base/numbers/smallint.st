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


COMMENT CLASS SmallInt  Number

COMMENT RAWCLASS MetaSmallInt  Class         MetaNumber           seed
COMMENT RAWCLASS SmallInt      MetaSmallInt  Number

COMMENT ----------SmallInt------------
METHODS FOR SmallInt [
^atRandom [
  seed ifNil: [ self seed: SmallInt getInitialSeed ].
  seed := (1103515245 * seed + 12345) bitAnd: 1073741823.  "268435455 = 0x3fffffff"
  ^seed
]

^seed [
  ^seed
]

^seed: aSeed [
  seed := aSeed bitAnd: 1073741823
]

isFloat [
  ^false
]

asSmallInt [
  ^self
]

asInteger [
  ^Integer new: self
]

asFloat [
  ^Float new: self
]

truncSmallInt [
  ^self
]

/ arg [
  <#SmallIntDiv self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger / arg].
  (0 = arg) ifTrue: [^self error: 'division by zero'].
  self primitiveFailed
]

% arg [
  <#SmallIntMod self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger % arg].
  (0 = arg) ifTrue: [^self error: 'division by zero'].
  self primitiveFailed
]

+ arg [
  <#SmallIntAdd self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger + arg].
  self primitiveFailed
]

* arg [
  <#SmallIntMul self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger * arg].
  self primitiveFailed
]

- arg [
  <#SmallIntSub self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger - arg].
  self primitiveFailed
]

< arg [
  <#SmallIntLess self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger < arg].
  self primitiveFailed
]

<= arg [
  <#SmallIntLessEqu self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger <= arg].
  self primitiveFailed
]

= arg [
  <#SmallIntEqu self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger = arg].
  self primitiveFailed
]

> arg [
  <#SmallIntGreat self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger > arg].
  self primitiveFailed
]

>= arg [
  <#SmallIntGreatEqu self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger >= arg].
  self primitiveFailed
]

~= arg [
  <#SmallIntNotEqu self arg>.
  (arg isMemberOf: SmallInt) ifFalse: [^self asInteger ~= arg].
  self primitiveFailed
]

hash [
  ^self
]

printString [
  "optimized for SmallInts"
  | n res pos sz |
  self = 0 ifTrue: [ ^'0' ].
  self < 0 ifTrue: [ ^'-' + self negated printString ].
  res := String new: (pos := 10).
  n := self.
  [ n > 0 ] whileTrue: [
    res basicAt: pos put: 48 + (n % 10).
    n := n / 10.
    pos := pos - 1.
  ].
  pos < 1 ifTrue: [ ^res ].
  ^(String new: (sz := 10 - pos)) replaceFrom: 1 to: sz with: res startingAt: pos + 1
]
!
