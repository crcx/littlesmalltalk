COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


CLASS Stream  Object  lineNum unreadChar

"doReadChar should return char or nil"

METHODS FOR Stream
^new [
  | obj |
  obj := super new.
  self in: obj at: 1 put: 1.
  ^obj
]

lineNum [
  ^lineNum
]

lineNum: n [
  lineNum := n
]

fileName [
  ^nil
]

readChar [
  "return char or nil on EOS"
  | c |
  unreadChar ifNotNil: [ c := unreadChar. unreadChar := nil. ^c ].
  (c := self doReadChar) ifNotNil: [
    c isEOL ifTrue: [ lineNum := lineNum + 1 ].
  ].
  ^c
]

unreadChar: c [
  "always return char"
  unreadChar ifNotNil: [ self error: 'no room for unreadChar:' ].
  ^(unreadChar := c)
]

readLine [
  | sb c |
  sb := StringBuffer new.
  [ c := self readChar ] whileNotNil: [
    c isEOL ifTrue: [
      "ah, let's care about windoze"
      (sb size > 0 and: [ sb last value == 13 ]) ifTrue: [ ^(sb asString) from: 1 to: sb size - 1 ].
      ^sb asString
    ].
    sb << c.
  ].
  sb size = 0 ifTrue: [ ^nil ].
  ^sb asString.
]
!
