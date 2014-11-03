"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [ RegExp ]

class: RegExpr [
| rh mstr |

^newREHandle: aStr type: aType [
  <#RegExpDo 0 aType aStr>.
  self primitiveFailed
]

^new [
  self error: 'use newHS: or newTX: to create RegExpr'
]

^newWithRH: rh [
  | obj |
  (rh class == String) ifTrue: [ ^rh ].
  obj := self basicNew.
  self in: obj var: #rh put: rh.
  ^obj
]

^newHS: aStr [
  ^self newWithRH: (self newREHandle: aStr type: 0)
]

^newTX: aStr [
  ^self newWithRH: (self newREHandle: aStr type: 1)
]

matchStr [
  ^mstr
]

match: aStr [
  mstr := aStr.
  <#RegExpDo 1 rh aStr>.
  self primitiveFailed
]

subCount [
  <#RegExpDo 2 rh>.
  self primitiveFailed
]

subStart: aIdx [
  <#RegExpDo 3 rh aIdx>.
  self primitiveFailed
]

subEnd: aIdx [
  <#RegExpDo 4 rh aIdx>.
  self primitiveFailed
]

captureAt: aIdx [
  | s e |
  (s := self subStart: aIdx) < 0 ifTrue: [ ^nil ].
  (e := self subEnd: aIdx) < 0 ifTrue: [ ^nil ].
  's=' print. s print. '; e=' print. e printNl.
  ^mstr from: s+1 to: e.
]
]
