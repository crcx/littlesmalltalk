COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS

COMMENT head tail size
METHODS FOR StringBuffer
^new [
  | obj |
  obj := super new.
  self in: obj at: 3 put: 0.
  ^obj
]

^newWith: anObj [
  | obj |
  (obj := self new) addFirst: anObj.
  ^obj
]

clear [
  head := tail := nil.
  size := 0.
]

removeAll [
  ^self clear
]

addFirst: anObj [
  | s cl |
  (((cl := (s := anObj) class) == Char) or: [ cl == String ]) ifFalse: [ s := anObj asString ].
  size := size + (cl == Char ifTrue: [1] ifFalse: [s size]).
  head := Link value: s next: head.
  tail ifNil: [ tail := head ].
  ^s
]

addLast: anObj [
  | s l cl |
  (((cl := (s := anObj) class) == Char) or: [ cl == String ]) ifFalse: [ s := anObj asString ].
  size := size + (cl == Char ifTrue: [1] ifFalse: [s size]).
  l := Link value: s.
  tail
    ifNil: [ head := l ]
    ifNotNil: [ tail next: l ].
  tail := l.
  ^s
]

first [
  head ifNil: [ ^nil ].
  ^head value
]

last [
  tail ifNil: [ ^nil ].
  ^tail value
]

<< anObj [
  self addLast: anObj.
]

size [
  ^size
]

asString [
  "ugly, but fast code; do not refactor, please!"
  | idx res l v len |
  res := String new: size.
  idx := 1.
  l := head.
  [ l notNil ] whileTrue: [
    "(l value) do: [:char | res at: idx put: char. idx := idx + 1 ]."
    (v := l value) class == String ifTrue: [
      "string"
      res replaceFrom: idx to: idx + (len := v size) - 1 with: v.
      idx := idx + len.
    ] ifFalse: [
      "char"
      res basicAt: idx put: v value.
      idx := idx + 1.
    ].
    l := l next
  ].
  ^res
]

COMMENT  inherited from Object
COMMENT  printString [
COMMENT    ^self asString
COMMENT  ]
!
