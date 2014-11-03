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


CLASS LstCompiler  Object \
  text index tokenType token argNames tempNames instNames mtclass \
  maxTemps errBlock warnBlock lineNum lexLineNum exitBlock allowInstanceConflicts \
  blockTempsStart \
  usedTemps \
  doneOnce \
  category


COMMENT --------------LstCompiler-----------------
METHODS FOR LstCompiler
^new [
  | obj |
  obj := self basicNew.
  self in: obj var: #doneOnce put: true.
  ^obj
]

text: aString instanceVars: anArray [
  text := aString fixMacWindozeText.
  index := 1.
  lineNum := 1.
  argNames := Array new: 1.
  argNames at: 1 put: #self.
  instNames := anArray.
  tempNames := Array new: 0.
  maxTemps := 0.
  allowInstanceConflicts := false.
  blockTempsStart := nil.
  usedTemps := Dictionary new.
  doneOnce := false.
  mtclass := nil.
  category := ''.
]

lineNum [
  ^lineNum
]

category [
  ^category
]

category: aCatStr [
  category := aCatStr
]

errorBlock [
  ^errBlock
]

errorBlock: aBlock [
  errBlock := aBlock
]

warningBlockAt [
  ^warnBlock
]

warningBlock: aBlock [
  warnBlock := aBlock
]

clearMemory [
  "free some memory"
  text := nil.
  argNames := nil.
  instNames := nil.
  tempNames := nil.
  usedTemps := nil.
]

compileWithClass: cls with: encoderClass [
  "call with cls==#returnNodes to return parsed tree"
  | encoder meth name rn varsLineNum |
  doneOnce ifTrue: [ super error: 'compiler can''t be called without (re)init' ].
  doneOnce := true.
  exitBlock := [ ^nil ].
  "note -- must call text:instanceVars: first"
  self nextLex.
  "tokenType == $^ ifTrue: [ self nextLex ]."
  encoder := encoderClass new.
  name := self readMethodName. varsLineNum := lineNum.
  encoder name: name.
  self readMethodVariables.
  mtclass := cls.
  "'CLASS: ' print. mtclass printNl."
  tokenType == $ ifTrue: [ rn := (AstBodyNode at: lineNum) statements: (List new) ] ifFalse: [ rn := self readBody ].
  mtclass := nil.
  "warn about unused locals"
  tempNames do: [:i :v |
    v := usedTemps at: i ifAbsent: [ nil ].
    v ifNil: [ self warning: 'unused method variable: "' + i asString + '"' line: varsLineNum]
    ifNotNil: [
      v ifFalse: [ self warning: 'method variable: "' + i asString + '" initialized, but never used' line: varsLineNum ].
    ].
  ].
  cls == #returnNodes ifTrue: [ self clearMemory. ^rn ].
  rn compile: encoder block: false.
  meth := encoder method: maxTemps class: cls text: text.
  meth args: argNames inst: instNames temp: tempNames.
  meth argCount: argNames size.
  meth category: category.
  self clearMemory.
  ^meth
]

compileWithClass: cls [
  ^self compileWithClass: cls with: ByteCodeEncoder
]


error: aMsg line: lno [
  errBlock
    ifNotNil: [ errBlock value: aMsg value: lno ]
    ifNil: [ ('Compile error near line ' + (lno asString) + ': ' + aMsg) printNl ].
  exitBlock value
]

error: aMsg [
  ^self error: aMsg line: lineNum
]

warning: aMsg line: lno  [
  warnBlock
    ifNotNil: [ warnBlock value: aMsg value: lno ]
    ifNil: [ ('Compile warning near line ' + (lno asString) + ': ' + aMsg) printNl ].
]

warning: aMsg [
  ^self warning: aMsg line: lineNum
]


indexOfPrimitive: name [
  ^System indexOfPrimitive: name
]

peekChar [
  ^text at: index ifAbsent: [ Char eof ]
]

aheadChar [
  ^text at: index + 1 ifAbsent: [ Char eof ]
]

nextChar [
  self peekChar isEOL ifTrue: [ lineNum := lineNum + 1 ].
  index := index + 1.
  ^self peekChar
]

skipComment [
  | c lstart |
  lstart := lineNum.
  [ (c := self nextChar) isEOF ifTrue: [ self error: 'unterminated comment' line: lstart ].
    c == $" ] whileFalse: [ nil ].
  self nextChar.
  ^self skipBlanks
]

skipBlanks [
  | c |
  c := self peekChar.
  [ c isBlank ] whileTrue: [ c := self nextChar ].
  ( c == $" ) ifTrue: [ ^self skipComment ]
]

nextLex [
  self skipBlanks.
  lexLineNum := lineNum.
  (tokenType := self peekChar) isEOF ifTrue: [ tokenType := $  . token := nil. ^nil ].
  tokenType isDigit ifTrue: [ ^self lexNumber ].
  tokenType isAlphabetic ifTrue: [ ^self lexAlnum ].
  ^self lexBinary
]

lexNumber [
  "collect number"
  | start base |
  start := index.
  [ self nextChar isDigit ] whileTrue: [ nil ].
  self peekChar == $r ifTrue: [
    "non-decimal number"
    base := (text from: start to: index - 1) asNumber.
    base < 1 ifTrue: [ self error: 'invalid number base' ].
    self nextChar.
    [ self nextChar isDigitInBase: base ] whileTrue: [ nil ].
  ] ifFalse: [
    (((self peekChar == $,) or: [ self peekChar == $. ])
     and: [ self aheadChar isDigit ]) ifTrue: [
      "float number"
      self nextChar.
      [ self nextChar isDigit ] whileTrue: [ nil ].
    ].
  ].
  token := text from: start to: index - 1
]

lexAlnum [
  "keyword (with possible colons)"
  | c start |
  start := index.
  "add any trailing colons too"
  [ ((c := self nextChar) isAlphanumeric) or: [ c == $: ]] whileTrue: [ nil ].
  token := text from: start to: index - 1
]

isNotBinaryChar: ch [
  ch ifNil: [ ^true ].
  ch isEOF ifTrue: [ ^true ].
  ^(((ch isBlank) or: [ ch isAlphanumeric ]) or: [ self charIsSyntax: ch ])
]

lexBinary [
  "alphanums already eaten"
  | c start |
  token := (c := self peekChar) asString.
  (self charIsSyntax: c) ifTrue: [ self nextChar. ^token ].
  start := index.
  [ c := self nextChar. self isNotBinaryChar: c ] whileFalse: [ nil ].
  token := text from: start to: index - 1.
]

charIsSyntax: c [
  ^('.()[]#^$;{}' includesChar: c) or: [ c == $' ]
]

readMethodName [
  | name ltok |
  self tokenIsName ifTrue: [ name := token. self nextLex. ^name ].  "unary"
  self tokenIsBinary  "binary"
    ifTrue: [
      name := token. ltok := lexLineNum.
      self nextLex.
      self tokenIsName ifFalse: [ ^self error: 'missing argument in method header' line: ltok ].
      self addArgName: token asSymbol.
      self nextLex.
      ^name ].
  self tokenIsKeyword ifFalse: [ ^self error: 'invalid method header' line: lexLineNum ].  "keyword"
  name := ''.
  [ self tokenIsKeyword ]
    whileTrue: [
      name := name + token. ltok := lexLineNum.
      self nextLex.
      self tokenIsName ifFalse: [ ^self error: 'missing argument in method header' line: ltok ].
      self addArgName: token asSymbol.
      self nextLex ].
  ^name
]

addArgName: name [
  (argNames includes: name) ifTrue: [ self error: 'doubly defined argument: "' + name asString + '"' line: lexLineNum ].
  (instNames includes: name) ifTrue: [ self warning: 'argument "' + name asString + '" shadows instance var' line: lexLineNum ].
  argNames := argNames with: name
]

tokenIsName [
  tokenType isAlphabetic ifFalse: [ ^false ].
  ^token lastChar isAlphanumeric
]

tokenIsKeyword [
  tokenType isAlphabetic ifFalse: [ ^false ].
  ^token lastChar == $:
]

tokenIsBinary [
  ^((((token isNil
       or: [ self tokenIsName ])
       or: [ self tokenIsKeyword ])
       or: [ self charIsSyntax: tokenType ])
       or: [ tokenType isDigit ]) not
]

readMethodVariables [
  (tokenType == $| and: [ token = '|' ]) ifFalse: [ ^nil ].
  self nextLex.
  [ self tokenIsName ] whileTrue: [ self addTempName: token asSymbol. self nextLex ].
  (tokenType == $| and: [ token = '|' ]) ifFalse: [ ^self error: 'illegal method variable declaration' line: lexLineNum ].
  self nextLex.
]

addTempName: name [
  (argNames includes: name) ifTrue: [ self error: 'method variable "' + name asString + '" shadows argument' line: lexLineNum ].
  allowInstanceConflicts ifFalse: [
    (instNames includes: name) ifTrue: [ self warning: 'method variable "' + name asString + '" shadows instance variable' line: lexLineNum ].
  ].
  (tempNames includes: name) ifTrue: [ self error: 'method variable "' + name asString + '" is doubly defined' line: lexLineNum ].
  tempNames := tempNames with: name.
  maxTemps := maxTemps max: tempNames size.
]

readBody [
  | lnum |
  lnum := lineNum.
  ^(AstBodyNode at: lnum) statements: self readStatementList
]

readStatementList [
  | list doAdd stat wasReturn statline |
  doAdd := true.
  list := List new.
  [ [ wasReturn := (tokenType == $^).
      statline := lineNum.
      stat := self readStatement.
      doAdd ifTrue: [
        "check and warn about potentially missing dots"
        tokenType ifNotNil: [ (' .])' includes: tokenType) ifFalse: [ self warning: 'missing dot after statement' line: statline ]].
        list << stat.
        wasReturn ifTrue: [ doAdd := false ].  "do not generate code after return"
      ].
      tokenType == $^ ]
     whileTrue: [ nil ].  "this fixes forgotten dots before ^"
    tokenType == $. ]
   whileTrue: [ self nextLex. (tokenType == $  or: [ tokenType == $] ]) ifTrue: [ ^list ]].
  ^list
]

readStatement [
  | lnum |
  tokenType == $^
    ifTrue: [
      lnum := lineNum.
      self nextLex.
      (tokenType == $< and: [ token = '<' ]) ifTrue: [ ^self readPrimitive ].
      ^(AstReturnNode at: lnum) expression: self readExpression
    ].
  (tokenType == $< and: [ token = '<' ]) ifTrue: [ ^self readPrimitive ].
  "no binary messages allowed here"
  self tokenIsBinary ifTrue: [ self error: 'unexpected binary message' line: lexLineNum ].
  ^self readExpression
]

readExpression [
  | node lnum name newVar nameLine |
  self tokenIsName ifFalse: [ ^self readCascade: self readTerm ].
  (name := token asSymbol). nameLine := lexLineNum.
  self nextLex.
  newVar := false.
  self tokenIsAssign ifTrue: [
    (usedTemps at: name ifAbsent: [ nil ]) ifNil: [
      newVar := true.
      usedTemps at: name put: false.  "assigned, but never used"
    ].
  ].
  node := self nameNode: name.
  newVar ifTrue: [ usedTemps at: name put: false ].  "restore 'assigned, but never used' state"
  self tokenIsAssign ifTrue: [
    node assignable ifFalse: [ self error: 'illegal assignment' line: nameLine ].
    lnum := lineNum.
    self nextLex.
    ^(AstAssignNode at: lnum) target: node expression: self readExpression
  ].
  ^self readCascade: node
]

tokenIsAssign [
  (token isKindOf: String) ifFalse: [ ^false ].
  ^token = ':=' or: [ token = '<-' ]
]

readTerm [
  | node lnum parLine |
  token ifNil: [ self error: 'unexpected end of input' line: lineNum + 1 ].
  tokenType == $(
    ifTrue: [
      parLine := lexLineNum.
      self nextLex.
      node := self readExpression.
      tokenType == $) ifFalse: [ self error: 'unbalanced parenthesis' line: parLine ].
      self nextLex.
      ^node ].
  tokenType == $[ ifTrue: [ ^self readBlock ].
  self tokenIsName ifTrue: [ node := self nameNode: token asSymbol. self nextLex. ^node ].
  lnum := lineNum.
  ^(AstLiteralNode at: lnum) value: self readLiteral
]

nameNode: name [
  "make a new name node"
  | idx |
  name == #super ifTrue: [ ^(AstArgumentNode at: lineNum) position: 0 ].
  name == #thisContext ifTrue: [ ^(AstArgumentNode at: lineNum) position: -1 ].
  "tempNames should be looked from the last item, so block locals will be found first"
  idx := tempNames size.
  [ idx > 0 ] whileTrue: [
    name == (tempNames at: idx) ifTrue: [
      (usedTemps at: name ifAbsent: [
        ((blockTempsStart isNil) or: [ idx < blockTempsStart ]) ifTrue: [
          self warning: 'using uninitialized method var "' + name asString + '"' line: lexLineNum
        ].
        false
      ]) ifFalse: [ usedTemps at: name put: true ].
      ^(AstTemporaryNode at: lineNum) position: idx
    ].
    idx := idx - 1.
  ].
  (1 to: argNames size) do: [:i | (name == (argNames at: i)) ifTrue: [ ^(AstArgumentNode at: lineNum) position: i ]].
  (1 to: instNames size) do: [:i | (name == (instNames at: i)) ifTrue: [ ^(AstInstNode at: lineNum) position: i ]].
  ^(AstLiteralNode at: lineNum);
    value: (globals at: name ifAbsent: [ ^self error: 'unrecognized name: ' + name line: lexLineNum ])
]

isLiteralStart: c [
  c == $$ ifTrue: [ ^true ].
  c == $' ifTrue: [ ^true ].
  c == $# ifTrue: [ ^true ].
  c isDigit ifTrue: [ ^true ].
  ^false
]

readLiteral [
  | node |
  tokenType isDigit ifTrue: [ ^self readNumber ].
  tokenType == $$ ifTrue: [ node := self peekChar. self nextChar. self nextLex. ^node ].
  tokenType == $' ifTrue: [ ^self readString ].
  tokenType == $# ifTrue: [ ^self readSymbol ].
  token = '-' ifTrue: [ self nextLex. ^self readNumber negated ].
  self error: 'invalid literal: <' + token + '>' line: lexLineNum
]

parseNumber [
  "parse number"
  | value bn base |
  value := token asNumber.
  value ifNil: [
    "based number or float"
    (value := token asFloat) ifNil: [
      bn := token break: 'r'.
      bn size = 2 ifFalse: [ self error: 'invalid integer' line: lexLineNum ].
      base := (bn at: 1) asNumber.
      (base isNil or: [ base < 1 ]) ifTrue: [ self error: 'invalid integer base' line: lexLineNum ].
      value := (bn at: 2) asNumberInBase: ((bn at: 1) asNumber).
      value ifNil: [ self error: 'integer expected' line: lexLineNum ].
    ].
  ].
  ^value
]

readNumber [
  | value |
  value := self parseNumber.
  self nextLex.
  ^value
]

readCharCodeInBase: base maxLen: len line: lineNo doNC: aNCFlag [
  | res c |
  res := 0.
  1 to: len do: [:i |
    aNCFlag ifTrue: [ self nextChar ] ifFalse: [ aNCFlag := true ].
    (c := self peekChar) isEOF ifTrue: [ self error: 'unterminated string constant' line: lineNo ].
    (c := c asDigitInBase: base) < base ifFalse: [
      i = 1 ifTrue: [ self error: 'invalid character code in string escape' line: lineNo ].
      ^res.
    ].
    res := res * base + c.
  ].
  ^res
]

readString [
  | c str strline |
  strline := lineNum.
  index := index - 1.
  str := StringBuffer new.
  [ (c := self nextChar) isEOF ifTrue: [ self error: 'unterminated string constant' line: strline ].
    c ~= $' ]
   whileTrue: [
    c == $\ ifTrue: [
      "escape sequence"
      (c := self nextChar) isEOF ifTrue: [ self error: 'unterminated string constant' line: strline ].
      Case test: c lowerCase;
        case: $a do: [ c := Char new: 7 ];
        case: $b do: [ c := Char new: 8 ];
        case: $d do: [ c := Char new: 127 ]; "delete"
        case: $e do: [ c := Char new: 27 ];  "escape"
        case: $f do: [ c := Char new: 12 ];  "form feed"
        case: $n do: [ c := Char new: 10 ];
        case: $r do: [ c := Char new: 13 ];
        case: $t do: [ c := Char new: 9 ];
        case: $v do: [ c := Char new: 11 ];  "vertical tab"
        case: $z do: [ c := Char new: 0 ];
        case: $\ do: [ c := $\ ];
        case: $/ do: [ c := $/ ];
        case: $" do: [ c := $" ];
        case: $' do: [ c := $' ];
        case: $x do: [ c := Char new: (self readCharCodeInBase: 16 maxLen: 2 line: strline doNC: true) ];
        when: [:c | c value between: 48 and: 57] do: [ c := Char new: (self readCharCodeInBase: 8 maxLen: 3 doNC: false) ];
        else: [ self error: 'invalid escape sequence: ' + c asString  line: strline ].
     ].
    str << c.
   ].
  self nextChar.
  str := str asString.
  self peekChar == $' ifTrue: [ self nextChar. ^str + '''' + self readString ].
  self nextLex.
  ^str
]

readSymbol [
  | c |
  c := self peekChar.
  (c isEOF or: [ c isBlank ]) ifTrue: [ self error: 'invalid symbol' ].
  c == $( ifTrue: [ ^self readArray ].
  c == ${ ifTrue: [ ^self readSpecialArray ].
  c == $[ ifTrue: [ ^self readByteArray ].
  (self charIsSyntax: c) ifTrue: [ self error: 'invalid symbol' ].
  self nextLex.
  c := Symbol new: token. self nextLex.
  ^c
]

readArray [
  | value |
  value := Array new: 0.
  self nextChar. self nextLex.
  [ tokenType ~= $) ] whileTrue: [ value := value with: self arrayLiteral ].
  self nextLex.
  ^value
]

arrayLiteral [
  | node |
  tokenType isAlphabetic ifTrue: [ node := Symbol new: token. self nextLex. ^node ].
  ^self readLiteral
]

readSpecialArray [
  | value c word |
  value := Array new: 0.
  self nextChar.  "skip {"
  [ self skipBlanks.
    (c := self peekChar) == $} ]
   whileFalse: [
     "special syntax for things like #{< <= + - * / % > >= ~= = & |}"
     word := c asString.  "don't need any StringBuffers"
     [ c := self nextChar. c isBlank or: [ c == $} ]] whileFalse: [ word := word + c asString ].
     value := value with: word asSymbol.
  ].
  self nextChar.
  self nextLex.
  ^value
]

readByteArray [
  | value c word code list |
  self nextChar.  "skip ["
  (code := (self peekChar) == $[) ifTrue: [ self nextChar ].
  list := List new.
  [ self nextLex.
    tokenType == $] ]
   whileFalse: [
     tokenType == $  ifTrue: [ ^self error: 'unexpected EOF in byte array literal' ].
     tokenType isDigit ifFalse: [ ^self error: 'invalid value in byte array literal' ].
     (c := self parseNumber) isFloat ifTrue: [ ^self error: 'float value in byte array literal' ].
     (c between: -128 and: 255) ifFalse: [ ^self error: 'out-of-range value in byte array literal' ].
     c < 0 ifTrue: [ c := 256 + c ].
     list << c.
  ].
  self nextLex.
  tokenType == $] ifTrue: [ self nextLex ].
  value := code ifTrue: [ ByteCode new: list size ] ifFalse: [ ByteArray new: list size ].
  c := 1. list do: [:i | value at: c put: i. c := c + 1].
  ^value
]

readPrimitive [
  | name start end c args lnum |
  lnum := lineNum.
  "collect name"
  (self peekChar) == $# ifFalse: [ self error: 'type annotaions are not supported yet' ].
  self nextChar. "skip '#'"
  start := index.
  end := start.
  [ c := self nextChar. ((c isBlank) or: [ c isEOF ]) or: [ c == $> ] ] whileFalse: [ end := end + 1. ].
  name := text from: start to: end.
  "get primitive number"
  c := self indexOfPrimitive: name.
  c ifNil: [ self error: 'unknown primitive: ' + name ].
  "collect args"
  self skipBlanks.
  self nextLex.
  args := List new.
  [ tokenType ~= $> ] whileTrue: [ args << self readTerm ].
  token = '>' ifFalse: [ self error: 'unexpected token "' + token + '"' line: lexLineNum ].
  self nextLex.
  ^(AstPrimitiveNode at: lnum) number: c arguments: args
]

readBlock [
  | stmts saveTemps saveUsed lnum acnt obts sst temploc tsym |
  saveTemps := tempNames.
  saveUsed := usedTemps.
  obts := blockTempsStart.
  blockTempsStart ifNil: [ blockTempsStart := saveTemps size ].
  lnum := lineNum.
  self nextLex.
  temploc := maxTemps.
  acnt := 0.
  tokenType == $: ifTrue: [
    tempNames := Array new: 0.  "so block can have the same temps as it's owner"
    allowInstanceConflicts := true.  "don't report about instance var shadowing (less noise)"
    self readBlockTemporaries.
    allowInstanceConflicts := false.
    acnt := tempNames size.
    acnt + temploc > 255 ifTrue: [ self error: 'too many locals in block' line: lnum ].
    "this silly code allows us to assign blocks to variables and then
     call one block from another block when both defined in the same method"
    sst := tempNames.
    tempNames := Array new: temploc + acnt.
    tempNames replaceFrom: 1 to: saveTemps size with: saveTemps.
    saveTemps size < temploc ifTrue: [
      tsym := '.' asSymbol.
      (saveTemps size + 1) to: temploc do: [:i | tempNames at: i put: tsym ].
    ].
    acnt > 0 ifTrue: [ tempNames replaceFrom: temploc + 1 to: temploc + acnt with: sst ].
    sst := nil.  "free some memory"
    "'tl: ' print. temploc print. '; x: ' print. tempNames printNl."
    maxTemps := maxTemps max: tempNames size.
  ].
  tokenType == $]
    ifTrue: [
      "empty block; just return nil"
      (stmts := List new) << (AstLiteralNode newWithValue: nil line: lineNum).
    ] ifFalse: [
      stmts := self readStatementList.
    ].
  tempNames := saveTemps.
  usedTemps := saveUsed.
  blockTempsStart := obts.
  tokenType == $]
    ifTrue: [ self nextLex. ^(AstBlockNode at: lnum) statements: stmts temporaryLocation: temploc argCount: acnt ]
    ifFalse: [ self error: 'unterminated block' line: lnum ].
]

readBlockTemporaries [
  [ tokenType == $: ]
    whileTrue: [ self peekChar isAlphabetic
      ifFalse: [ self error: 'ill formed block argument'].
      self nextLex.
      self tokenIsName
        ifTrue: [ self addTempName: token asSymbol ]
        ifFalse: [ self error: 'invalid block argument list'].
      self nextLex ].
  (tokenType == $| and: [ token = '|' ])
    ifTrue: [ self nextLex ]
    ifFalse: [ self error: 'invalid block argument list ']
]

readCascade: base [
  | node list |
  node := self keywordContinuation: base.
  tokenType == $; ifFalse: [ ^node ].
  node := (AstCascadeNode at: lineNum) head: node.
  list := List new.
  [ tokenType == $; ] whileTrue: [
    self nextLex.
    tokenType == $;
      ifTrue: [ self warning: 'extra ";"' line: lexLineNum ]
      ifFalse: [
        "this is a hack, but i like it"
        ('.]) ' includesChar: tokenType) ifTrue: [
          list isEmpty ifTrue: [ ^node head ].
          node list: list.
          ^node
        ].
        "check for the good token"
        (((self tokenIsName) or: [ self tokenIsKeyword ]) or: [ self tokenIsBinary ])
          ifFalse: [ self error: 'invalid cascade (unexpected token)' line: lexLineNum ].
        "go on"
        list << (self keywordContinuation: nil)
      ].
  ].
  node list: list.
  ^node
]

isLoopName: name [
  ^(((name = 'whileTrue:'
      or: [ name = 'whileFalse:' ])
      or: [ name = 'whileNil:' ])
      or: [ name = 'whileNotNil:' ])
]

keywordContinuation: base [
  | receiver name args lnum ntok kcnt ptline |
  receiver := self binaryContinuation: base.
  self tokenIsKeyword ifFalse: [ ^receiver ].
  name := ''.
  args := List new.
  lnum := lineNum.
  kcnt := 0.
  ptline := lexLineNum.
  [ self tokenIsKeyword ] whileTrue: [
    (kcnt := kcnt + 1) = 2
      ifTrue: [
        (self isLoopName: name) ifTrue: [ ^self error: 'loop is not properly terminated' line: ptline ].
        Case test: name;
          case: 'ifTrue:' do: [ ntok := 'ifFalse:' ];
          case: 'ifFalse:' do: [ ntok := 'ifTrue:' ];
          case: 'ifNil:' do: [ ntok := 'ifNotNil:' ];
          case: 'ifNotNil:' do: [ ntok := 'ifNil:' ];
          else: [ ntok := nil ].
        ntok ifNotNil: [
          token = ntok ifFalse: [ ^self error: 'condition is not properly terminated' line: ptline ].
        ].
    ].
    name := name + token.
    self nextLex.
    ptline := lexLineNum.
    args << (self binaryContinuation: self readTerm)
  ].
  ^(AstMessageNode at: lnum) receiver: receiver name: name asSymbol arguments: args.
]

binaryContinuation: base [
  | receiver name lnum |
  receiver := self unaryContinuation: base.
  [ self tokenIsBinary ]
    whileTrue: [
      self tokenIsAssign ifTrue: [ ^self error: 'assign as binary -- previous statement not terminated' line: lexLineNum ].
      lnum := lineNum.
      name := token asSymbol.
      self nextLex.
      receiver := (AstMessageNode at: lnum)
        receiver: receiver name: name arguments: (List with: (self unaryContinuation: self readTerm)).
      receiver := receiver optimizeMath.
    ].
  ^receiver
]

unaryContinuation: base [
  | receiver lnum |
  receiver := base.
  [ self tokenIsName ]
   whileTrue: [
    lnum := lineNum.
    receiver := (AstMessageNode at: lnum) receiver: receiver name: token asSymbol arguments: (List new).
    self nextLex
  ].
  ^receiver
]
!
