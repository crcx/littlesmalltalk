"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [ System ]


File extend [
readDWord [
  | res shift |
  res := shift := 0.
  1 to: 4 do: [:c |
    (c := self readCharValue) ifNil: [ self error: 'unexpeced end of file' ].
    c := c bitShift: shift.
    res := res bitOr: c.
    shift := shift + 8.
  ].
  ^res
]

writeDWord: dw [
  | shift |
  shift := 0.
  1 to: 4 do: [:c |
    c := (dw bitShift: 0 - shift) bitAnd: 255.
    self writeCharValue: c.
    shift := shift + 8.
  ].
]

copyFrom: fsrc size: count [
  | buf rd |
  buf := ByteArray new: 65536.
  [ count > 0 ] whileTrue: [
    rd := count min: buf size.
    rd := (fsrc read: buf size: rd).
    rd <= 0 ifTrue: [ self error: 'file reading error' ].
    (self write: buf size: rd) = rd ifFalse: [ self error: 'file writing error' ].
    count := count - rd. ].
]
]
