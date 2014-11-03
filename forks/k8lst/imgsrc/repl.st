CLASS REPL  Object

METHODS FOR REPL
REPL [
  "main execution loop"
  | cmd res |
  [
    System isStdInTTY ifTrue: [ '-> ' print ].
    cmd := String input.
    cmd notNil
   ] whileTrue: [
     res := cmd doIt.
     cmd removeTrailingBlanks lastChar == $. ifFalse: [ res printNl ].
  ].
]
!
