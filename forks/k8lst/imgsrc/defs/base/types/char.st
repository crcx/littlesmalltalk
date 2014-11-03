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


RAWCLASS MetaChar  Class     MetaMagnitude  chars
RAWCLASS Char      MetaChar  Magnitude      value

COMMENT -----------Char--------------
METHODS FOR Char
^initialize [
  chars ifNil: [
    chars := Array new: 257.
    1 to: 257 do: [:idx | chars at: idx put: (Char basicNew: idx-1) ]
  ]
]

^basicNew: aValue [
  "create and initialize a new char"
  ^self in: self new at: 1 put: aValue
]

^new: aValue [
  "return unique Char for ASCII value (or EOF)"
  (aValue < 257) ifTrue: [ ^chars at: aValue+1 ].
  "otherwise build a custom Char"
  ^self basicNew: aValue
]

^newline [
  "return newline character"
  ^chars at: 11
]

^cr [
  "return carriage return character"
  ^chars at: 14
]

^tab [
  "return tab character"
  ^chars at: 10
]

^eof [
  "return an EOF indication--not a true Char, but polymorphic"
  ^chars at: 257
]

^input [
  "read a single char from input stream"
  | c |
  (c := System readStdInCharAsSmallInt) ifNotNil: [ ^self new: c ].
  ^nil
]

value [
  "return our ascii value as an integer"
  ^value
]

hash [
  ^value
]

isDigit [
  ^self between: $0 and: $9
]

isLowerCase [
  ^self between: $a and: $z
]

isUpperCase [
  ^self between: $A and: $Z
]

isAlphabetic [
  ^self isLowerCase or: [ self isUpperCase ]
]

isEOF [
  ^value = 256
]

asDigitInBase: base [
  (self between: $0 and: $9) ifTrue: [ ^(self value) - 48 ].
  (self between: $A and: $Z) ifTrue: [ ^(self value) - 55 ].
  (self between: $a and: $z) ifTrue: [ ^(self value) - 87 ].
  ^nil
]

isDigitInBase: base [
  | dd |
  (dd := self asDigitInBase: base) ifNil: [ ^false ].
  ^dd < base
]

lowerCase [
  self isUpperCase ifTrue: [ ^Char new: value + 32 ]
]

upperCase [
  self isLowerCase ifTrue: [ ^Char new: value - 32 ]
]

isAlphanumeric [
  "are we a letter or a digit?"
  ^self isAlphabetic or: [ self isDigit ]
]

isBlank [
  "spaces, tabs and newlines are all blanks"
  ^value = 32 or: [ value > 8 and: [ value < 14 ]]
]

isEOL [
  "is newline?"
  ^value = 10
]

isCR [
  "is cr?"
  ^value = 13
]

isLF [
  "is lf?"
  ^value = 10
]

print [
  System printBaseObject: self.
]

printNl [
  System printBaseObjectNl: self.
]

asString [
  "return char as a string value"
  ^String new: 1; at: 1 put: self
]

printString [
  ^String new: 2; at: 1 put: $$ ; at: 2 put: self
]

= aChar [
  ^value = aChar value
]

< aChar [
  ^value < aChar value
]
!
