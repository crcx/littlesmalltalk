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


RAWCLASS MetaSymbol  Class       MetaMagnitude  symbols
RAWCLASS Symbol      MetaSymbol  Magnitude

COMMENT -----------Symbol--------------
METHODS FOR Symbol
^intern: string [
  <#String#clone: string Symbol>.
  self primitiveFailed
]

^new: fromString [
  ^symbols at: fromString ifAbsent: [ symbols add: (self intern: fromString) ]
]

printString [
  <#String#clone: self String>.
  self primitiveFailed
]

asString [
  ^self printString
]

hash [
  <#FNVHash self>.
  ^self printString hash
]

asSymbol [
  ^self
]

= aString [
  "works with either symbol or string arguments"
  ^self printString = aString printString
]

< arg [
  "works with either symbol or string arguments"
  ^self printString < arg printString
]

size [
  <#ObjectSize self>
]

COMMENT little faster then cloning to string
print [
  System printBaseObject: self.
]

printNl [
  System printBaseObjectNl: self.
]
!
