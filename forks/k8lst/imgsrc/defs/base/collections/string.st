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


EXTPRIM StringToURLEncoding
EXTPRIM StringFromURLEncoding

COMMENT ----------String------------
METHODS FOR String
^new: aSize [
  <#NewByteArray self aSize>.
  self primitiveFailed
]

^input [
  "read a line from input"
  "TODO: use stringbuffer?"
  | value c pos vn |
  System flushStdOut.
  value := String new: 256. pos := 1.
  [ (c := Char input) ifNil: [ pos = 1 ifTrue: [ ^nil ]. ^value from: 1 to: pos ].
    c isEOL ] whileFalse: [
    pos > value size ifTrue: [
      "extend buffer"
      vn := String new: (value size) + 256.
      vn replaceFrom: 1 to: pos with: value.
      value := vn. vn := nil.
    ].
    value at: pos put: c.
    pos := pos + 1.
  ].
  ^value from: 1 to: pos - 1.
]

^newline [
  ^Char newline asString
]

^tab [
  ^Char tab asString
]

, aValue [
  ^self + aValue
]

+ aValue [
  "concatenate two strings/arrays together"
  aValue ifNil: [ ^self basePlus: aValue asString ].
  "((aValue class == String) or: [ aValue class == Symbol ]) ifTrue: [ ^self basePlus: aValue ]."
  ((aValue class == Char) or: [ aValue isKindOf: Number ]) ifTrue: [ ^self basePlus: aValue asString ].
  ^self basePlus: aValue
]

break: separators [
  "break string into words, using separators"
  | words word |
  word := StringBuffer new.
  words := List new.
  self do: [:c |
    (separators includesChar: c)
      ifTrue: [
        (word size > 0)  "found a word"
          ifTrue: [ words << word asString. word := StringBuffer new ]]
      ifFalse: [ word << c ]].
    "maybe a last word"
  (word size > 0) ifTrue: [ words << word asString ].
  ^words
]

position: aString from: idx [
  "find arg as substring and return position or nil"
  <#String#Position:from: self aString idx>.
  self primitiveFailed
]

position: aString [
  "find arg as substring and return position or nil"
  ^self position: aString from: 1
]

lastPosition: aString from: idx [
  "find arg as substring and return position or nil"
  <#String#LastPosition:from: self aString idx>.
  self primitiveFailed
]

lastPosition: aString [
  "find arg as substring and return position or nil"
  ^self lastPosition: aString from: 1
]

includesChar: c [
  (self position: c) ifNotNil: [ ^true ].
  ^false
]

includes: aWhat [
  (((aWhat class == String)
     or: [ aWhat class == Char ])
     or: [ aWhat class == SmallInt]) ifTrue: [
    (self position: aWhat) ifNotNil: [ ^true ].
    ^false
  ].
  ^super includes: aWhat
]

collect: transformationBlock [
  ^(super collect: transformationBlock) asString
]

reverse [
  ^self asList reverse asString
]

select: testBlock [
  ^(super select: testBlock) asString
]

printString [
  ^self
]

asString [
  ^self
]

printWidth: width padChar: c [
  | ret |
  (self size >= width absolute) ifTrue: [ ^self ].
  ret := self.
  (width negative)
    ifTrue: [
      (self size + 1) to: (width negated) do: [:ignore | ret := c + ret].
    ] ifFalse: [
      (self size + 1) to: width do: [:ignore | ret := ret + c ].
    ].
  ^ret
]

printWidth: width [
  ^self printWidth: width padChar: ' '.
]

asSymbol [
  ^Symbol new: self
]

basicAt: index [
  <#String#at: self index>.
  ^nil
]

at: index [
  ^self at: index ifAbsent: [ self badIndex ]
]

at: index ifAbsent: exceptionBlock [
  | c |
  (c := self basicAt: index)
    ifNil: [ ^exceptionBlock value ]
    ifNotNil: [ ^Char new: c ]
]

basicAt: index put: value [
  <#String#at:put: value self index>.
  ^nil
]

at: index put: aValue [
  (self basicAt: index put: aValue value) ifNil: [ self badIndex ]
]

copy [
  "make a clone of ourself"
  <#String#clone: self String>.
  self primitiveFailed
]

asNumber [
  "parse a base-10 ASCII number, return nil on failure"
  ^self asNumberInBase: 10
]

asNumberInBase: base [
  "parse a number, return nil on failure"
  | val |
  val := 0.
  self do: [:c |
    (c isDigitInBase: base) ifFalse: [ ^nil ].
    val := (val * base) + (c asDigitInBase: base)
  ].
  ^val
]

stupidHash [
  | sz |
  sz := self size.
  sz < 2 ifTrue: [ sz = 1 ifTrue: [ ^(self at: 1) value ]. ^0 ].
  ^(self at: 1) value + (self at: sz) value
]

hash [
  <#FNVHash self>.
  ^self stupidHash.
]

lowerCase [
  | ret |
  ret := String new: (self size).
  1 to: self size do: [:x | ret at: x put: (self at: x) lowerCase ].
  ^ret
]

upperCase [
  | ret |
  ret := String new: (self size).
  1 to: self size do: [:x | ret at: x put: (self at: x) upperCase ].
  ^ret
]

indexOf: val [
  "for non-strings, search scalar Array elements"
  (val isKindOf: String) ifFalse: [ ^super indexOf: val ].
  ^self position: val
]

lastIndexOf: val [
  "for non-strings, search scalar Array elements"
  (val isKindOf: String) ifFalse: [ ^super lastIndexOf: val ].
  ^self lastPosition: val
]

COMMENTS
indexOf: val [
  | c s |
  "for non-strings, search scalar Array elements"
  (val isKindOf: String) ifFalse: [ ^super indexOf: val ].
  "otherwise look for substring match"
  c := val at: 1.
  s := val size.
  1 to: (self size - s + 1) do: [:idx |
    "cheap check for initial char equality"
    ((self at: idx) = c) ifTrue: [
      ((self from: idx to: (idx + s - 1)) = val)
        ifTrue: [ ^idx ]
    ]
  ].
  ^nil
]
ENDCOMMENTS

lastChar [
  self size < 1 ifTrue: [ ^nil ].
  ^self at: (self size)
]

firstChar [
  self size < 1 ifTrue: [ ^nil ].
  ^self at: 1
]

removeTrailingBlanks [
  | nlen |
  "can return the same object"
  nlen := self size.
  nlen > 0 ifTrue: [
    [ nlen > 0 ]
     whileTrue: [
      (self at: nlen) isBlank ifFalse: [ ^self from: 1 to: nlen ].
      nlen := nlen - 1.
    ].
    ^''.
  ].
  ^self
]

removeLeadingBlanks [
  | nlen sz |
  "can return the same object"
  nlen := 1. sz := self size.
  [ nlen <= sz ] whileTrue: [
    (self at: nlen) isBlank ifFalse: [
      nlen = 1 ifTrue: [ ^self ].
      ^self from: nlen
    ].
    nlen := nlen + 1.
  ].
  ^''.
]

from: low to: high [
  <#String#CopyFromTo self low high>.
  ^super from: low to: high
]

doIt [
  "execute this string"
  | meth |
  meth := UndefinedObject compileMethod: 'doItCommand\n^' + self.
  meth ifNotNil: [ ^Context new perform: meth withSelf: UndefinedObject ].
  ^nil.
]

doItSeq [
  "execute this sequence"
  | meth |
  meth := UndefinedObject compileMethod: ('doItCommand\n' + self).
  ^meth ifNotNil: [
    ^Context new perform: meth withSelf: UndefinedObject.
  ]
]

toUrl [
  "convert to URL encoding"
  <#StringToURLEncoding self>.
  self primitiveFailed
]

fromUrl [
  "convert from URL encoding"
  <#StringFromURLEncoding self>.
  self primitiveFailed
]

fromUrlWithPlus [
  "convert from URL encoding, process '+'"
  <#StringFromURLEncoding self true>.
  self primitiveFailed
]

firstNonBlankChar [
  self isEmpty ifTrue: [ ^Char eof ].
  1 to: self size do: [:c | (c := self at: c) isBlank ifFalse: [ ^c ]].
  ^Char eof
]

fixMacWindozeText [
  "removes CR before CR/LF, changes single CR to LF, remove leading and trailing empty lines"
  | rpos text |
  text := self removeTrailingBlanks.
  [ rpos := text position: '\r' ] whileNotNil: [
    "check if '\r' follows by '\n'; note that we cannot have '\r' as the last char"
    (text at: rpos + 1) value = 10 ifTrue: [
      "'\r\n': remove '\r'"
      text := (text from: 1 to: rpos - 1) + (text from: rpos + 1)
    ] ifFalse: [
      "single '\r': change to '\n'"
      text at: rpos put: Char newline.
    ].
  ].
  text := text + '\n'.
"
  [ text isEmpty ] whileFalse: [
    text firstChar value = 10 ifFalse: [ ^text ].
    text := text from: 2.
  ].
"
  ^text
]

toPrintable [
  | res |
  res := ''.
  self do: [:c |
    c value < 32 ifTrue: [
      Case test: (c := c value);
        case: 9 do: [ res := res + '\\t' ];
        case: 10 do: [ res := res + '\\n' ];
        case: 13 do: [ res := res + '\\r' ];
        else: [:c | res := res + '\\x' + (c printWidth: 2 base: 16) lowerCase ].
    ] ifFalse: [
      (c == $' or: [ c == $\ ]) ifTrue: [ res := res + '\\' ].
      res := res + c
    ].
  ].
  ^res
]
!
