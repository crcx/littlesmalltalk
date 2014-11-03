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
"simple browser"
Class extend [
view: methodName [
  "print the text of the given method"
  (methods at: methodName ifAbsent: [ ^self error: 'no such method: ' + methodName printString ]) text print
]

viewMethod: nm  [
  | meth |
  meth := self allMethods at: nm ifAbsent: [ ^self error: 'no such method: ' + nm printString ].
  meth text print.
  ^''
]

subclasses: indent [
  globals do: [:obj |
    ((obj isKindOf: Class) and: [ obj superclass == self])
      ifTrue: [
        1 to: indent do: [:ignore | $  print ].
        obj printNl.
        obj subclasses: indent + 4 ] ]
]

listMethods [
  methods keysDo: [:name | name printNl ]
]

listAllMethods [
  self allMethods keysDo: [:n | n printNl ]
]

"
test: name [
  | methods |
  ^nil
]
"
]
