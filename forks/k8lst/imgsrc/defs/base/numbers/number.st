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


COMMENT CLASS Number        Magnitude

COMMENT ----------Number------------
METHODS FOR Number
^new [
  "can't create this way, return zero"
  ^0
]

negative [
  ^self < 0
]

absolute [
  (self negative) ifTrue: [ ^self negated ]
]

negated [
  ^0 - self
]

asDigit [
  (self < 10) ifTrue: [ ^(Char new: (self asSmallInt + 48)) asString ].
  ^(Char new: (self asSmallInt + 55)) asString
]

printWidth: width base: base [
  | res n wasNeg wide |
  res := ''.
  (self negative) ifTrue: [
    wasNeg := true.
    wide := width-1.
    n := self negated
  ] ifFalse: [
    wasNeg := false.
    wide := width.
    n := self
  ].
  [true] whileTrue: [
    res := ((n % base) asDigit) + res.
    n := n / base.
    (n = 0) ifTrue: [
      ((res size)+1) to: wide do: [:ignore | res := '0' + res ].
      wasNeg ifTrue: [ res := '-' + res ].
      ^res
    ]
  ]
]

printWidth: width [
  ^self printWidth: width base: 10
]

printString [
  ^self printWidth: 1 base: 10
]

to: limit [
  ^Interval from: self to: limit step: 1
]

to: limit by: step [
  ^Interval from: self to: limit step: step
]

to: limit do: aBlock [
  | i |
  i := self.
  [ i > limit ] whileFalse: [ aBlock value: i. i := i + 1 ]
]

to: limit by: step do: aBlock [
  | i |
  i := self.
  step < 0
    ifTrue: [[ limit > i ] whileFalse: [ aBlock value: i. i := i + step ]]
    ifFalse: [[ i > limit ] whileFalse: [ aBlock value: i. i := i + step ]].
]

timesRepeat: aBlock [
  | cnt |
  cnt := self.
  [ cnt > 0 ] whileTrue: [ aBlock value. cnt := cnt - 1 ].
]

overflow [
  self error: 'Numeric overflow'
]

bitOr: arg [
  <#bit2op: self arg 0>.
  self primitiveFailed.
]

bitAnd: arg [
  <#bit2op: self arg 1>.
  self primitiveFailed.
]

bitXor: arg [
  <#bit2op: self arg 2>.
  self primitiveFailed.
]

bitNot [
  <#bitNot self>.
  self primitiveFailed.
]

bitShift: arg [
  <#bitShift: self arg>.
  self primitiveFailed.
]

atRandom [
  "Return random number from 1 to self"
  (self < 2) ifTrue: [ ^self ].
  ^((1 to: self) atRandom)
]

asChar [
  ^Char new: (self asSmallInt)
]

log2 [
  ^self asFloat log2
]
!
