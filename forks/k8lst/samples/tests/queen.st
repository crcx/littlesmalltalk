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
" eight queens exercise "
Object subclass: NullQueen [
first [
  ^true
]

next [
  ^false
]

checkRow: row column: column [
  ^false
]

result [
  ^List new
]
]


Object subclass: Queen [
| row column neighbor |
setColumn: aNumber neighbor: aQueen [
  column <- aNumber.
  neighbor <- aQueen
]

checkRow: testRow column: testColumn [
  | columnDifference |
  columnDifference <- testColumn - column.
  (((row = testRow) or:
    [ row + columnDifference = testRow]) or:
    [ row - columnDifference = testRow])
      ifTrue: [ ^true ].
  ^neighbor checkRow: testRow column: testColumn
]

first [
  neighbor first.
  row <- 1.
  ^self testPosition
]

next [
  ^self advance and: [ self testPosition ]
]

advance [
  (row = 8)
    ifTrue: [ neighbor next ifFalse: [ ^false ].
      row <- 0 ].
  row <- row + 1.
  ^true
]

testPosition [
  [ neighbor checkRow: row column: column ]
    whileTrue: [ self advance ifFalse: [ ^false ]].
  ^true
]

result [
  ^neighbor result; addLast: row
]
]


Object subclass: Main [
run [
  | lastQueen |
  lastQueen <- NullQueen new.
  1 to: 8 do: [:i | lastQueen <- Queen new;
    setColumn: i neighbor: lastQueen ].
  lastQueen first.
  'got a result' print.
  lastQueen result do: [:x | x print. ' ' print ].
  Char newline print.
  [ lastQueen next ] whileTrue:
    [
  lastQueen result do: [:x | x print. ' ' print ].
  Char newline print ]
]
]

Eval [ Main new run. ]
