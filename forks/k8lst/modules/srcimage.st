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
Package [
  Image
]

Object subclass: Image [
| imagePathName sourcePathName visited |

^new [
  | obj |
  obj <- super new.
  obj imagePathName: 'workimage.image'.
  obj sourcePathName: 'workimage'.
  ^obj
]

^writeSource [
  ^self new writeSource
]

imagePathName [
  ^imagePathName
]

imagePathName: path [
  imagePathName := path
]

sourcePathName [
  ^sourcePathName
]

sourcePathName: path [
  sourcePathName := path
]

writeSource [
  | path |
  sourcePathName
    ifNil: [ path := 'workimage' ]
    ifNotNil: [ path := sourcePathName asString ].
  ^self writeSource: path
]

writeSource: path [
  | fname file |
  path
    ifNil: [ fname := 'workimage' ]
    ifNotNil: [ fname := path asString ].
  file <- File open: fname asString mode: 'wb'.
  self writeClassesTo: file.
  file newline.
  self writeMethodsTo: file.
  file newline.
  file write: 'BEGIN nil INITIALIZEEVERYTHING REPL'.
  file newline.
  file write: 'END'.
  file newline.
  file close.
  'image source writing seems to be experimental!' printNl.
]

writeClassesTo: file [
  visited <- Dictionary new.
  globals do: [:cls |
    (cls isKindOf: Class)
      ifTrue: [
        self writeClass: cls class to: file.
        self writeClass: cls to: file
      ]
  ]
]

writeMethodsTo: file [
  visited <- Dictionary new.
  globals do: [:cls |
    (cls isKindOf: Class)
      ifTrue: [
        self writeMethodsOf: cls class to: file.
        self writeMethodsOf: cls to: file
      ]
  ]
]

writeClass: cls to: file [
  | stmt vars parent |
  (visited includes: cls asString asSymbol) ifTrue: [ ^self ].

  parent <- cls parent.
  parent ifNotNil: [ (visited includes: parent asString asSymbol) ifFalse: [ self writeClass: parent to: file ] ].
  visited at: cls asString asSymbol put: 1.

  cls = Object ifTrue: [ ^self ].
  stmt <- 'RAWCLASS ' + cls asString + ' ' + cls class asString + ' ' + parent asString.
  vars <- cls variables.
  vars ifNotNil: [ vars do: [:v | stmt <- stmt + ' ' + v asString ] ].
  file write: stmt.
  file newline
]

writeMethodsOf: cls to: file [
  | methods parent |

  (visited includes: cls asString asSymbol) ifTrue: [ ^self ].

  parent <- cls parent.
  parent ifNotNil: [ (visited includes: parent asString asSymbol) ifFalse: [ self writeMethodsOf: parent to: file ] ].
  visited at: cls asString asSymbol put: 1.

  file newline.
  file write: ( 'COMMENT ----------' + cls asString + '---------' ).
  file newline.
  file newline.
  methods <- cls methods.
  methods ifNil: [ ^nil ].
  methods do: [:m | self writeMethod: m of: cls to: file ]
]

writeMethod: meth of: cls to: file [
  | stmt txt c |

  txt <- meth text asString.

  stmt <- 'METHOD ' + cls asString.
  file write: stmt.
  file newline.
  file write: txt.

  "Verify if txt ends with a newline"
  c := txt lastChar.
  (c isNil or: [ c isEOL not ]) ifTrue: [ file newline. ].

  file write: '!'.
  file newline.
  file newline.
]
]
