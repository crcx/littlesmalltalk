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


CLASS AstParserNode     Object      lineNum
CLASS AstBodyNode       AstParserNode  statements
CLASS AstReturnNode     AstParserNode  expression
CLASS AstAssignNode     AstParserNode  target expression
CLASS AstLiteralNode    AstParserNode  value
CLASS AstArgumentNode   AstParserNode  position
CLASS AstTemporaryNode  AstParserNode  position
CLASS AstInstNode       AstParserNode  position
CLASS AstPrimitiveNode  AstParserNode  number arguments
CLASS AstBlockNode      AstParserNode  statements temporaryLocation argCount
CLASS AstCascadeNode    AstParserNode  head list
CLASS AstMessageNode    AstParserNode  receiver name arguments


COMMENT --------------AstParserNode-----------------
METHODS FOR AstParserNode
^new [
  self error: 'Must use at: for creation'
]

^at: l [
  | ret |
  ret := super new.
  self in: ret at: 1 put: l.
  ^ret
]

isSuper [
  ^false
]

isBlock [
  ^false
]

isMessage [
  ^false
]

isLiteral [
  ^false
]

assignable [
  ^false
]

lineNum [
  ^lineNum
]

compile: encoder [
  encoder lineNum: lineNum
]
!


COMMENT --------------AstBodyNode-----------------
METHODS FOR AstBodyNode
statements [
  ^statements
]

statements: s [
  statements := s
]

compile: encoder block: inBlock [
  | lastop stl lret |
  super compile: encoder.
  statements isEmpty ifFalse: [
    "optimization: do not generate useless SelfReturn"
    (lret := (lastop := statements last) isKindOf: AstReturnNode)
      ifTrue: [ stl := statements copy removeLast ]
      ifFalse: [ stl := statements ].
    stl do: [:stmt | stmt compile: encoder block: inBlock. encoder genPopTop ].
    lret ifTrue: [ lastop compile: encoder block: inBlock. ^self ].
  ].
  encoder genSelfReturn
]
!


COMMENT --------------AstReturnNode-----------------
METHODS FOR AstReturnNode
expression [
  ^expression
]

expression: e [
  expression := e
]

compile: encoder block: inBlock [
  super compile: encoder.
  expression compile: encoder block: inBlock.
  inBlock ifTrue: [ encoder genBlockReturn ] ifFalse: [ encoder genStackReturn ].
]
!


COMMENT --------------AstAssignNode-----------------
METHODS FOR AstAssignNode
expression [
  ^expression
]

target [
  ^target
]

target: t expression: e [
  target := t.
  expression := e
]

compile: encoder block: inBlock [
  super compile: encoder.
  expression compile: encoder block: inBlock.
  target assign: encoder
]
!


COMMENT --------------AstLiteralNode-----------------
METHODS FOR AstLiteralNode
^newWithValue: v line: lno [
  | obj |
  (obj := self at: lno) value: v.
  ^obj
]

value: v [
  value := v
]

value [
  ^value
]

isLiteral [
  ^true
]

compile: encoder block: inBlock [
  super compile: encoder.
  value == nil ifTrue: [ encoder genPushNil. ^self ].
  value == true ifTrue: [ encoder genPushTrue. ^self ].
  value == false ifTrue: [ encoder genPushFalse. ^self ].
  ((value class == SmallInt) and: [ value between: 0 and: 252 ])
    ifTrue: [ encoder genPushConstant: (value + 3). ^self ].
  encoder genPushLiteral: (encoder addLiteral: value)
]
!


COMMENT --------------AstArgumentNode-----------------
METHODS FOR AstArgumentNode
position: p [
  position := p
]

isSuper [
  ^position = 0
]

isSelf [
  ^position = 1
]

assignable [
  ^position > 1
]

assign: encoder [
  encoder genAssignArgument: position - 1
]

compile: encoder block: inBlock [
  super compile: encoder.
  position = 0 ifTrue: [ encoder genPushArgument: 0. ^self ]. "super"
  position < 0 ifTrue: [ encoder genThisContext. ^self ].     "thisContext"
  encoder genPushArgument: position - 1.
]
!


COMMENT --------------AstTemporaryNode-----------------
METHODS FOR AstTemporaryNode
position: p [
  position := p
]

compile: encoder block: inBlock [
  super compile: encoder.
  encoder genPushTemporary: position - 1
]

assignable [
  ^true
]

assign: encoder [
  encoder genAssignTemporary: position - 1
]
!


COMMENT --------------AstInstNode-----------------
METHODS FOR AstInstNode
position: p [
  position := p
]

compile: encoder block: inBlock [
  super compile: encoder.
  encoder genPushInstance: position - 1
]

assign: encoder [
  encoder genAssignInstance: position - 1
]

assignable [
  ^true
]
!


COMMENT --------------AstPrimitiveNode-----------------
METHODS FOR AstPrimitiveNode
number: n arguments: a [
  number := n.
  arguments := a.
]

compile: encoder block: inBlock [
  | argsize |
  argsize := arguments size.
  super compile: encoder.
  encoder pushArgs: argsize.
  arguments do: [:a | a compile: encoder block: inBlock ].
  encoder genDoPrimitive: argsize.
  encoder genCode: number.
  encoder popArgs: argsize.
  argsize = 0 ifTrue: [ encoder pushArgs: 1. encoder popArgs: 1 ].  "reserve one slot for result"
]
!


COMMENT --------------AstBlockNode-----------------
METHODS FOR AstBlockNode [
statements: s temporaryLocation: t argCount: ac [
  statements := s.
  temporaryLocation := t.
  argCount := ac
]

temporaryLocation [
  ^temporaryLocation
]

argCount [
  ^argCount
]

compileInLine: encoder block: inBlock clearTemps: doClr [
  | genPop |
  "clear temps (if any)"
  doClr ifNotNil: [
    ((doClr >= 0) and: [ argCount > doClr ]) ifTrue: [
      super compile: encoder.
      encoder genPushNil.
      doClr to: (argCount-1) do: [:p | encoder genAssignTemporary: (temporaryLocation + p) ].
      encoder genPopTop.
    ].
  ].
  genPop := false.
  statements do: [:stmt |
    genPop ifTrue: [ encoder genPopTop ].
    stmt compile: encoder block: inBlock.
    genPop := true.
  ]
]

compileInLine: encoder block: inBlock [
  ^self compileInLine: encoder block: inBlock clearTemps: 0.
]

isBlock [
  ^true
]

compile: encoder block: inBlock [
  | patchLocation |
  super compile: encoder.
  encoder genPushBlock: temporaryLocation.
  patchLocation := encoder genVal: 0.
  encoder genCode: argCount.
  self compileInLine: encoder block: true clearTemps: nil.
  encoder genStackReturn.
  encoder patch: patchLocation
]
!


COMMENT --------------AstCascadeNode-----------------
METHODS FOR AstCascadeNode
head [
  ^head
]

head: h [
  head := h
]

list: l [
  list := l
]

COMMENT  st arguments size = 2 ifFalse: [ 'SHIIIIIIIIIIIIIIIT' printNl ].
checkCArgs: st b1: ab1 [
  (st arguments first) isBlock == ab1 ifFalse: [ ^false ].
  ^(st arguments at: 2) isBlock
]

compileBlockCall: encoder blk: aBlock block: inBlock [
  "setup argument if necessary"
  aBlock argCount > 0 ifTrue: [ encoder genAssignTemporary: aBlock temporaryLocation. ].
  encoder genPopTop. "and drop the value"
  aBlock compileInLine: encoder block: inBlock clearTemps: 1.
]

optimizeCase: encoder block: inBlock [
  | a1 a2 plist prevJmp nomorecode |
  ((((head isKindOf: AstMessageNode)
      and: [ head receiver isKindOf: AstLiteralNode ])
      and: [ head receiver value == Case ])
      and: [ head name == #test: ])
    ifFalse: [ ^false ].
  list isEmpty ifTrue: [ ^false ].
  "check arguments"
  list do: [:st |
    (st isKindOf: AstMessageNode) ifFalse: [ ^false ].
    Case test: st name;
      case: #case:do: do: [ (self checkCArgs: st b1: false) ifFalse: [ ^false ]];
      case: #same:do: do: [ (self checkCArgs: st b1: false) ifFalse: [ ^false ]];
      case: #when:do: do: [ (self checkCArgs: st b1: true) ifFalse: [ ^false ]];
      case: #else:    do: [ (st arguments first) isBlock ifFalse: [ ^false ]];
      else: [ ^false ].
  ].
  "switch value"
  (head arguments first) compile: encoder block: inBlock.
  "here we have arguments; do cascase"
  encoder pushArgs: 1.
  nomorecode := false.
  plist := 0. prevJmp := nil.
  list do: [:st |
    nomorecode ifFalse: [
      prevJmp ifNotNil: [ encoder patch: prevJmp. prevJmp := nil. ].
      a1 := st arguments first.
      st name == #else:
        ifTrue: [
          "only body block, no conditions"
          self compileBlockCall: encoder blk: a1 block: inBlock.
          nomorecode := true.
        ] ifFalse: [
          "checks"
          a2 := st arguments at: 2.
          st name == #when:do:
            ifTrue: [
              "condition block; no need to duplicate argument"
              encoder pushArgs: 1.
              a1 argCount > 0 ifTrue: [ encoder genAssignTemporary: a1 temporaryLocation. ].
              a1 compileInLine: encoder block: inBlock clearTemps: 1.
              encoder popArgs: 1.
              encoder genBranchIfFalse.
              prevJmp := encoder genVal: 0.
            ] ifFalse: [
              encoder pushArgs: 2.
              encoder genDuplicate.
              a1 compile: encoder block: inBlock.  "argument"
              "condition"
              encoder genSendBinary: ((st name == #case:do:) ifTrue: [10] ifFalse: [13]).  "= or =="
              encoder popArgs: 2.
              encoder genBranchIfFalse.
              prevJmp := encoder genVal: 0.
            ].
          "body"
          self compileBlockCall: encoder blk: a2 block: inBlock.
          encoder genBranch.
          plist := encoder genVal: plist.
        ].
    ].
  ].
  encoder popArgs: 1.
  "now patch branches"
  encoder patchChain: plist.
  "patch last jump here, 'cause patchChain can remove last jump"
  prevJmp ifNotNil: [ encoder patch: prevJmp. prevJmp := nil. ].
  ^true
]

compile: encoder block: inBlock [
  super compile: encoder.
  (self optimizeCase: encoder block: inBlock) ifTrue: [ ^self ].
  head compile: encoder block: inBlock.
  encoder pushArgs: 1.
  list do: [:stmt |
    encoder genDuplicate.
    stmt compile: encoder block: inBlock.
    encoder genPopTop ].
  encoder popArgs: 1.
]
!


COMMENT --------------AstMessageNode-----------------
METHODS FOR AstMessageNode
receiver: r name: n arguments: a [
  receiver := r.
  name := n.
  arguments := a
]

isMessage [
  ^true
]

name [
  ^name
]

receiver [
  ^receiver
]

arguments [
  ^arguments
]

isNilOrNotNil [
  ^(name == #isNil) or: [ name == #notNil ]
]

isCondition [
  ^((((name == #ifTrue:)
    or: [ name == #ifFalse: ])
    or: [ name == #ifTrue:ifFalse: ])
    or: [ name == #ifFalse:ifTrue: ])
]

isLoop [
  ^((((name == #whileTrue:)
    or: [ name == #whileFalse: ])
    or: [ name == #whileNil: ])
    or: [ name == #whileNotNil: ])
]

COMMENTS
optimizeNilChecks: encoder block: inBlock [
  | nn |
  receiver isMessage ifFalse: [ ^false ].
  receiver isNilOrNotNil ifFalse: [ ^false ].
  self isCondition ifFalse: [ ^false ].
  self argumentsAreBlocks ifFalse: [ ^false ].
  receiver name == #isNil
    ifTrue: [
      Case test: name;
        same: #ifTrue:          do: [ nn := #ifNil: ];
        same: #ifFalse:         do: [ nn := #ifNotNil: ];
        same: #ifTrue:ifFalse:  do: [ nn := #ifNil:ifNotNil: ];
        same: #ifFalse:ifTrue:  do: [ nn := #ifNotNil:ifNil: ].
    ]
    ifFalse: [
      Case test: name;
        same: #ifTrue:          do: [ nn := #ifNotNil: ];
        same: #ifFalse:         do: [ nn := #ifNil: ];
        same: #ifTrue:ifFalse:  do: [ nn := #ifNotNil:ifNil: ];
        same: #ifFalse:ifTrue:  do: [ nn := #ifNil:ifNotNil: ].
    ].
  "get the original receiver"
  receiver := receiver receiver.
  name := nn.
  self compile: encoder block: inBlock.
  ^true
]
ENDCOMMENTS

optimizableMathClass: aClass [
  (aClass isKindOf: Number) ifTrue: [ ^true ].
  aClass := aClass class.
  ^((aClass == String) or: [ aClass == Array ]).
]

compatibleMathClasses: clsl and: clsr [
  (clsl isKindOf: Number) ifTrue: [ ^clsr isKindOf: Number ].
  ^clsl class == clsr class.
]

optimizeMath [
  "return new object or nil; should be called from compiler"
  | arg res |
  receiver ifNil: [ ^self ].
  receiver isLiteral ifFalse: [ ^self ].
  (self optimizableMathClass: receiver value) ifFalse: [ ^self ].
  arguments size = 1 ifFalse: [ ^self ].
  arg := arguments first.
  arg ifNil: [ ^self ].
  arg isLiteral ifFalse: [ ^self ].
  (self optimizableMathClass: arg value) ifFalse: [ ^self ].
  (self compatibleMathClasses: receiver value and: arg value) ifFalse: [ ^self ].
  (receiver isKindOf: Number) ifTrue: [
    (#{< <= + - * / % > >= ~= = ==} indexOf: name) ifNil: [ ^self ].
  ] ifFalse: [
    (#{< <= + - * / % > >= ~= = == ,} indexOf: name) ifNil: [ ^self ].
  ].
  res := receiver value perform: name withArgument: arg value.
  ^AstLiteralNode newWithValue: res line: receiver lineNum
]

compile: encoder block: inBlock [
  receiver ifNil: [ ^self cascade: encoder block: inBlock ].
  "(self optimizeNilChecks: encoder block: inBlock) ifTrue: [ ^self ]."
  super compile: encoder.
  (receiver isBlock and: [ self argumentsAreBlocks ]) ifTrue: [
    name == #whileTrue:   ifTrue: [ ^self optimizeWhile: encoder block: inBlock brcode: (encoder branchCodeFalse) ].
    name == #whileFalse:  ifTrue: [ ^self optimizeWhile: encoder block: inBlock brcode: (encoder branchCodeTrue) ].
    name == #whileNil:    ifTrue: [ ^self optimizeWhile: encoder block: inBlock brcode: (encoder branchCodeNotNil) ].
    name == #whileNotNil: ifTrue: [ ^self optimizeWhile: encoder block: inBlock brcode: (encoder branchCodeNil) ].
  ].
  receiver compile: encoder block: inBlock.
  receiver isSuper ifTrue: [ ^self sendToSuper: encoder block: inBlock ].
  name == #isNil ifTrue: [ encoder genSendUnaryIsNil. ^self ].
  name == #notNil ifTrue: [ encoder genSendUnaryNotNil. ^self ].
  ^self compile2: encoder block: inBlock
]

compile2: encoder block: inBlock [
  | bidx |
  self argumentsAreBlocks ifTrue: [
    name = #ifTrue:
      ifTrue: [ ^self optimizeCond: encoder bfalse: (encoder branchCodeFalse) aux: (encoder constCodeFalse) block: inBlock ].
    name = #ifFalse:
      ifTrue: [ ^self optimizeCond: encoder bfalse: (encoder branchCodeTrue) aux: (encoder constCodeTrue) block: inBlock ].
    name = #ifNil:
      ifTrue: [ ^self optimizeCond: encoder bfalse: (encoder branchCodeNotNil) aux: nil block: inBlock ].
    name = #ifNotNil:
      ifTrue: [ ^self optimizeCond: encoder bfalse: (encoder branchCodeNil) aux: nil block: inBlock ].
    name = #ifTrue:ifFalse:
      ifTrue: [ ^self optimizeIfElse: encoder block: inBlock bfalse: (encoder branchCodeFalse) ].
    name = #ifFalse:ifTrue:
      ifTrue: [ ^self optimizeIfElse: encoder block: inBlock bfalse: (encoder branchCodeTrue) ].
    name = #ifNil:ifNotNil:
      ifTrue: [ ^self optimizeIfElse: encoder block: inBlock bfalse: (encoder branchCodeNotNil) ].
    name = #ifNotNil:ifNil:
      ifTrue: [ ^self optimizeIfElse: encoder block: inBlock bfalse: (encoder branchCodeNil) ].
    name = #and: ifTrue:
      [ ^self optimizeCond: encoder bfalse: (encoder branchCodeFalse) aux: (encoder constCodeFalse) block: inBlock ].
    name = #or: ifTrue:
      [ ^self optimizeCond: encoder bfalse: (encoder branchCodeTrue) aux: (encoder constCodeTrue) block: inBlock ].
  ].
  self evaluateArguments: encoder block: inBlock.
  (bidx := #{< <= + - * / % > >= ~= = & | ==} indexOf: name) ifNil: [
    ^self sendMessage: encoder block: inBlock
  ].
  encoder genSendBinary: bidx - 1.
  encoder popArgs: 2.
]

evaluateArguments: encoder block: inBlock [
  encoder pushArgs: 1 + arguments size.  "+1 due to receiver"
  arguments do: [:arg | arg compile: encoder block: inBlock ]
]

markArgs: encoder [
  encoder popArgs: 1 + arguments size.  "MarkArguments will turn 'em into array"
  encoder genMarkArguments: 1 + arguments size.
]

sendToSuper: encoder block: inBlock [
  self evaluateArguments: encoder block: inBlock.
  self markArgs: encoder.
  encoder genSendToSuper.
  encoder genCode: (encoder addLiteral: name).
]

sendMessage: encoder block: inBlock [
  self markArgs: encoder.
  encoder genSendMessage: (encoder addLiteral: name)
]

cascade: encoder block: inBlock [
  super compile: encoder.
  self evaluateArguments: encoder block: inBlock.
  ^self sendMessage: encoder block: inBlock
]

argumentsAreBlocks [
  arguments do: [:arg | arg isBlock ifFalse: [ ^false ]].
  ^true
]

optimizeWhile: encoder block: inBlock brcode: brc [
  | start save |
  encoder pushArgs: 1.  "for receiver"
  start := encoder currentLocation.
  receiver compileInLine: encoder block: inBlock.
  encoder genCondBranch: brc.
  save := encoder genVal: 0.
  arguments first compileInLine: encoder block: inBlock.
  encoder genPopTop.
  encoder genBranch.
  encoder genVal: start.
  encoder patch: save.
  encoder genPushNil.
  encoder popArgs: 1.
]

optimizeCond: encoder bfalse: brf aux: aux block: inBlock [
  | save ssave |
  encoder pushArgs: 1.  "for receiver"
  aux ifNil: [ encoder pushArgs: 1. encoder genDuplicate ].
  encoder genCondBranch: brf.
  save := encoder genVal: 0.
  aux ifNil: [ encoder popArgs: 1. encoder genPopTop ].
  arguments first compileInLine: encoder block: inBlock.
  aux ifNotNil: [
    "push constant"
    encoder genBranch.
    ssave := encoder genVal: 0.
    encoder patch: save.
    encoder genPushConstant: aux.
    save := ssave.
  ].
  encoder patch: save.
  encoder popArgs: 1.
]

optimizeIfElse: encoder block: inBlock bfalse: brf [
  | save ssave |
  encoder pushArgs: 1.  "for receiver"
  "first will be 'then' block"
  encoder genCondBranch: brf.
  save := encoder genVal: 0.
  arguments first compileInLine: encoder block: inBlock.
  encoder genBranch.
  ssave := encoder genVal: 0.
  "and then 'else' block"
  encoder patch: save.
  (arguments at: 2) compileInLine: encoder block: inBlock.
  encoder patch: ssave.
  encoder popArgs: 1.
]
!
