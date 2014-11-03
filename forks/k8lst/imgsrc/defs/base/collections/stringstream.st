COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


CLASS StringStream  Stream  text index

METHODS FOR StringStream
^newWith: aText [
  | obj |
  obj := self new.
  self in: obj at: 1 put: aText.
  self in: obj at: 2 put: 1.
  ^obj
]

text: aText [
  text := aText.
  index := 1.
  unreadChar := nil.
  lineNum := 1.
]

doReadChar [
  | c |
  index > text size ifTrue: [ ^nil ].
  c := text at: index.
  index := index + 1.
  ^c
]
!
