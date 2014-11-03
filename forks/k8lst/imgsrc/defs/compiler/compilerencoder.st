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


CLASS ByteCodeEncoder  Object  name byteCodes index literals stackSize maxStack lineNum dbgInfo maxJmp

COMMENT --------------ByteCodeEncoder-----------------
METHODS FOR ByteCodeEncoder
name: n [
  name := n asSymbol.
  byteCodes := ByteCode new: 512.
  index := 0.
  literals := Array new: 0.
  stackSize := 0.
  maxStack := 1.
  dbgInfo := Array new: 0.
  lineNum := 1.
  maxJmp := -1.
]

lineNum: l [
  lineNum := l.
  l < 1 ifTrue: [ lineNum := 1 ].
]

currentLocation [
  ^index
]

pushArgs: n [
  stackSize := stackSize + n.
  maxStack := stackSize max: maxStack
]

popArgs: n [
  stackSize := stackSize - n.
]

addLiteral: aValue [
  "return literal index"
  | idx |
  (idx := literals indexOf: aValue) ifNotNil: [ ^idx - 1 ].
  "we need to do such idiotic things 'cause not all literals can be 'valued'"
  "find the same number (Float or Integer)"
  (aValue isKindOf: Number) ifTrue: [
    idx := 0.
    literals do: [:i |
      ((i isKindOf: Number) and: [ i = aValue ]) ifTrue: [ ^idx ].
      idx := idx + 1.
    ].
  ].
  "find the same string"
  (aValue class == String) ifTrue: [
    idx := 0.
    literals do: [:i |
      ((i class == String) and: [ i = aValue ]) ifTrue: [ ^idx ].
      idx := idx + 1.
    ].
  ].
  "new literal"
  literals := literals with: aValue.
  ^literals size - 1
]


dbgAdd [
  | i |
  "FIXME: slooooooooow"
  [ lineNum > dbgInfo size ] whileTrue: [ dbgInfo := dbgInfo with: nil ].
"'PC=' print. index print. '; line=' print. lineNum print. '; dbgInfo:size=' print. (dbgInfo size) printNl."
  (i := dbgInfo at: lineNum) ifNil: [
    i := Array with: lineNum with: index with: index.
    dbgInfo at: lineNum put: i.
    ^self
  ].
  "extend range"
  index < (i at: 2) ifTrue: [ self error: 'internal error in bytecode generator' ].
  index > (i at: 3) ifTrue: [ i at: 3 put: index. "dbgInfo at: lineNum put: i." ].
]

genCode: byte [
  self dbgAdd.
  index := index + 1.
  (index >= byteCodes size) ifTrue: [ self expandByteCodes ].
  byteCodes at: index put: byte.
  ^index
]

genHigh: high low: low [
  (low >= 16)
    ifTrue: [ self genHigh: 0 low: high. self genCode: low ]
    ifFalse: [ self genCode: high * 16 + low ]
]

genVal: word [
  word > maxJmp ifTrue: [ maxJmp := word ].
  self genCode: (word % 256).
  self genCode: (word / 256).
  ^index - 1
]


genPushInstance: aIdx [
  ^self genHigh: 1 low: aIdx
]

genPushArgument: aIdx [
  ^self genHigh: 2 low: aIdx
]

genPushTemporary: aIdx [
  ^self genHigh: 3 low: aIdx
]

genPushLiteral: aIdx [
  ^self genHigh: 4 low: aIdx
]

genPushConstant: aIdx [
  ^self genHigh: 5 low: aIdx
]

genAssignInstance: aIdx [
  ^self genHigh: 6 low: aIdx
]

genAssignArgument: aIdx [
  ^self genHigh: 7 low: aIdx
]

genAssignTemporary: aIdx [
  ^self genHigh: 8 low: aIdx
]

genMarkArguments: aCount [
  ^self genHigh: 9 low: aCount
]

genPushBlock: aTempLoc [
  ^self genHigh: 10 low: aTempLoc
]

genSendUnary: aIdx [
  ^self genHigh: 11 low: aIdx
]

genSendBinary: aIdx [
  ^self genHigh: 12 low: aIdx
]

genSendMessage: aLitIdx [
  ^self genHigh: 13 low: aLitIdx
]

genDoPrimitive: aIdx [
  ^self genHigh: 14 low: aIdx
]

genExtended: aCode [
  ^self genHigh: 15 low: aCode
]


genSendUnaryIsNil [
  ^self genSendUnary: 0
]

genSendUnaryNotNil [
  ^self genSendUnary: 1
]


constCodeNil [
  ^0
]

constCodeTrue [
  ^1
]

constCodeFalse [
  ^2
]

genPushNil [
  ^self genPushConstant: (self constCodeNil)
]

genPushTrue [
  ^self genPushConstant: (self constCodeTrue)
]

genPushFalse [
  ^self genPushConstant: (self constCodeFalse)
]


genBreakpoint [
  ^self genExtended: 0
]

genSelfReturn [
  ^self genExtended: 1
]

genStackReturn [
  ^self genExtended: 2
]

genBlockReturn [
  ^self genExtended: 3
]

genDuplicate [
  ^self genExtended: 4
]

genPopTop [
  ^self genExtended: 5
]

genBranch [
  ^self genCondBranch: (self branchCodeNormal)
]

genBranchIfTrue [
  ^self genCondBranch: (self branchCodeTrue)
]

genBranchIfFalse [
  ^self genCondBranch: (self branchCodeFalse)
]

genBranchIfNil [
  ^self genCondBranch: (self branchCodeNil)
]

genBranchIfNotNil [
  ^self genCondBranch: (self branchCodeNotNil)
]

genSendToSuper [
  ^self genExtended: 11
]

genThisContext [
  ^self genExtended: 12
]


genCondBranch: aType [
  ^self genExtended: 6 + aType
]

branchCodeNormal [
  ^0
]

branchCodeTrue [
  ^1
]

branchCodeFalse [
  ^2
]

branchCodeNil [
  ^3
]

branchCodeNotNil [
  ^4
]


expandByteCodes [
  | newarray size |
  size := byteCodes size.
  newarray := ByteArray new: size + 64.
  newarray replaceFrom: 1 to: size with: byteCodes.
  byteCodes := newarray
]

shrinkByteCodes [
  | newarray |
  byteCodes size > index ifTrue: [
    newarray := ByteArray new: index.
    newarray replaceFrom: 1 to: index with: byteCodes.
    byteCodes := newarray
  ]
]


patch: loc [
  "patch a goto from a block"
  loc > maxJmp ifTrue: [ maxJmp := loc ].
  byteCodes at: loc putWord: index.
]

patchChain: loc [
  "patch a goto chain"
  | l1 |
  index > maxJmp ifTrue: [ maxJmp := index ].
  [ loc > 0 ] whileTrue: [
    l1 := byteCodes wordAt: loc.
    byteCodes at: loc putWord: index.
    loc := l1 ].
]


buildDebugInfo [
  | inf t |
  "calculate size"
  t := 0.
  dbgInfo do: [:i | i ifNotNil: [ t := t + 6 ]].
  t < 1 ifTrue: [ ^nil ].
  "put ranges"
  inf := ByteArray new: t.
  t := 1.
  dbgInfo do: [:i |
    i ifNotNil: [
      "line number, pc start, pc end"
      inf at: t   putWord: (i at: 1).  "line number"
      inf at: t+2 putWord: (i at: 2).  "pc start"
      inf at: t+4 putWord: (i at: 3).  "pc end"
      t := t + 6.
    ].
  ].
  ^inf.
]


COMMENT 'bc: ' print. name print. '; stack: ' print. maxStack printNl.
method: maxTemps class: c text: text [
  | mth |
  maxJmp > index ifTrue: [ ^self error: 'internal error in compiler' ].
  maxJmp = index ifTrue: [
    'RETFIX!' printNl.
    self genSelfReturn.
  ].
  self shrinkByteCodes.
  mth :=
   Method name: name byteCodes: byteCodes literals: literals
    stackSize: maxStack temporarySize: maxTemps class: c
    text: text.
  mth debugInfo: (self buildDebugInfo).
  ^mth
]
!
