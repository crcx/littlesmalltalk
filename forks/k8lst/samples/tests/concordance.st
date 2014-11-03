"
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
 of this software and associated documentation files (the 'Software'), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
"
"  Concordance program "
Object subclass: Concordance [
| dict |

word: word occursOnLine: line [
  (dict includes: word)
    ifFalse: [ dict at: word put: List new ].
  ((dict at: word) includes: line)
    ifFalse: [ (dict at: word) addLast: line]
]

fileIn [
  | text lineNumber words |
  lineNumber <- 0.
  [ text <- String input.  text notNil ]
    whileTrue: [
      text <- text collect: [ :c | c lowerCase ].
      lineNumber <- lineNumber + 1.
      words <- text break: ' .,'.
      words do: [ :word | self word: word
        occursOnLine: lineNumber ] ]
]

displayDictionary [
  dict keysAndValuesDo: [ :word :lines |
    word print.
    lines do: [ :line | ' ' print. line print ].
    Char newline print ]
]

run [
  dict <- Dictionary new.
  self fileIn.
  self displayDictionary
]
]


Eval [
  'enter test lines, followed by end-of-file' printNl.
  Concordance new run
]
