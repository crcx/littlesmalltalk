COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


COMMENT CLASS Float    Number

COMMENT ----------Float------------
METHODS FOR Float
^new: num [
  <#NumberToFloat num>.
  self primitiveFailed
]

isFloat [
  ^true
]

asByteArray [
  <#FloatBAIO 0 self>.
  self primitiveFailed
]

^fromByteArray: ba [
  <#FloatBAIO 1 ba>.
  self primitiveFailed
]

asFloat [
  ^self
]

asSmallInt [
  ^(self asInteger) asSmallInt
]

asInteger [
  <#FloatToInteger self>.
  self overflow
]

negated [
  <#FloatNegate self>.
]

hash [
  ^self asSmallInt
]

trunc [
  <#FloatToInteger 1 self>.
  self overflow
]

round [
  <#FloatToInteger 2 self>.
  self overflow
]

ceil [
  <#FloatToInteger 3 self>.
  self overflow
]

floor [
  <#FloatToInteger 4 self>.
  self overflow
]

log2 [
  <#FloatFunc self 0>.
  self overflow
]

/ arg [
  <#FloatDiv self arg>.
  (0 = arg) ifTrue: [ ^self error: 'division by zero' ].
  self primitiveFailed
]

% arg [
  self error: 'no % for floats yet'.
]

+ arg [
  <#FloatAdd self arg>.
  self primitiveFailed
]

* arg [
  <#FloatMul self arg>.
  self primitiveFailed
]

- arg [
  <#FloatSub self arg>.
  self primitiveFailed
]

< arg [
  <#FloatLess self arg>.
  (arg isMemberOf: Float) ifFalse: [^self < arg asFloat].
  self primitiveFailed
]

<= arg [
  <#FloatLessEqu self arg>.
  (arg isMemberOf: Float) ifFalse: [^self <= arg asFloat].
  self primitiveFailed
]

= arg [
  <#FloatEqu self arg>.
  (arg isMemberOf: Float) ifFalse: [^self = arg asFloat].
  self primitiveFailed
]

> arg [
  <#FloatGreat self arg>.
  (arg isMemberOf: Float) ifFalse: [^self > arg asFloat].
  self primitiveFailed
]

>= arg [
  <#FloatGreatEqu self arg>.
  (arg isMemberOf: Float) ifFalse: [^self >= arg asFloat].
  self primitiveFailed
]

~= arg [
  <#FloatNotEqu self arg>.
  (arg isMemberOf: Float) ifFalse: [^self ~= arg asFloat].
  self primitiveFailed
]

asString [
  <#FloatToString self>.
  self primitiveFailed
]

printString [
  ^self asString
]
!


METHOD FOR String
asFloat [
  | val infrac frac |
  val := Float new: 0.
  infrac := false.
  self do: [:c |
    (c isDigit) ifFalse: [
      infrac ifTrue: [ ^nil ].
      ((c == $,) or: [ c == $. ]) ifFalse: [ ^nil ].
      frac := Float new: 1.
      infrac := true.
    ] ifTrue: [
      infrac ifTrue: [
        frac := frac/10.
        val := val+(frac*(c asDigitInBase: 10)).
      ] ifFalse: [ val := val*10+(c asDigitInBase: 10) ].
    ].
  ].
  ^val
]
