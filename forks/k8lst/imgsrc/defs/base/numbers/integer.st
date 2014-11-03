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


COMMENT CLASS Integer  Number

COMMENT ----------Integer------------
METHODS FOR Integer
^new: low [
  <#SmallIntToInteger low>.
  low := low asSmallInt.
  <#SmallIntToInteger low>.
  self primitiveFailed
]

isFloat [
  ^false
]

asByteArray [
  <#IntegerBAIO 0 self>.
  self primitiveFailed
]

^fromByteArray: ba [
  <#IntegerBAIO 1 ba>.
  self primitiveFailed
]

asSmallInt [
  <#IntegerToSmallInt self>.
  self overflow
]

asInteger [
  ^self
]

asFloat [
  ^Float new: self
]

truncSmallInt [
  <#IntegerToSmallIntTrunc self>.
  self primitiveFailed
]

hash [
  <#IntegerToSmallInt self>.
  ^(self % 65536) asSmallInt
]

/ arg [
  <#IntegerDiv self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self / arg asInteger].
  (0 = arg) ifTrue: [^self error: 'division by zero'].
  self primitiveFailed
]

% arg [
  <#IntegerMod self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self % arg asInteger].
  (0 = arg) ifTrue: [^self error: 'division by zero'].
  self primitiveFailed
]

+ arg [
  <#IntegerAdd self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self + arg asInteger].
  self primitiveFailed
]

* arg [
  <#IntegerMul self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self * arg asInteger].
  self primitiveFailed
]

- arg [
  <#IntegerSub self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self - arg asInteger].
  self primitiveFailed
]

< arg [
  <#IntegerLess self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self < arg asInteger].
  self primitiveFailed
]

<= arg [
  <#IntegerLessEqu self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self <= arg asInteger].
  self primitiveFailed
]

= arg [
  <#IntegerEqu self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self = arg asInteger].
  self primitiveFailed
]

> arg [
  <#IntegerGreat self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self > arg asInteger].
  self primitiveFailed
]

>= arg [
  <#IntegerGreatEqu self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self >= arg asInteger].
  self primitiveFailed
]

~= arg [
  <#IntegerNotEqu self arg>.
  (arg isMemberOf: Integer) ifFalse: [^self ~= arg asInteger].
  self primitiveFailed
]
!
