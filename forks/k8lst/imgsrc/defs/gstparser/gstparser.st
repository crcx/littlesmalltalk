COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS

COMMENT GST-style parser
COMMENT
COMMENT A new class is created using this syntax:
COMMENT
COMMENT   superclass-name subclass: new-class-name | class vars | [
COMMENT     | instance vars |
COMMENT     message-pattern-1 [ statements ]
COMMENT     message-pattern-2 [ statements ]
COMMENT     ...
COMMENT   ]
COMMENT
COMMENT if class has no class variables, the entire '| class variables |' clause
COMMENT can be omited.
COMMENT
COMMENT if message-pattern starts with ^, this is class message, not instance one.
COMMENT
COMMENT
COMMENT A similar syntax is used to define new methods in an existing class.
COMMENT
COMMENT   class-expression extend [
COMMENT     ...
COMMENT   ]
COMMENT
COMMENT
COMMENT inStream must support 3 methods:
COMMENT   readChar: returns char or nil on EOS
COMMENT   unreadChar: unreads char (only one is needed)
COMMENT   lineNum: returns current line number
COMMENT

CLASS GSTParser  Object  inStream errorBlock warningBlock lastMethodName lastMethodLine lastWordLine category

METHODS FOR GSTParser
^newWith: inStream [
  | obj |
  obj := self new.
  self in: obj var: #inStream put: inStream.
  ^obj
]

inStream [
  ^inStream
]

errorBlock [
  ^errorBlock
]

errorBlock: aBlock [
  errorBlock := aBlock
]

warningBlock [
  ^warningBlock
]

warningBlock: aBlock [
  warningBlock := aBlock
]

fileName [
  | fn li |
  (fn := inStream fileName) ifNil: [ ^nil ].
  (li := fn lastIndexOf: '/') ifNil: [ ^fn ].
  ^fn from: li + 1
]

error: aString at: lineNum [
  | fn |
  errorBlock ifNotNil: [ errorBlock value: aString value: lineNum ]
    ifNil: [
      "comment the following to avoid duplicate error messages"
      (aString := 'Compile error near line ' + lineNum + ': ' + aString) printNl.
      (fn := self fileName) ifNotNil: [ aString := fn + ': ' + aString ].
      super error: aString
    ].
]

error: aString [
  ^self error: aString at: inStream lineNum
]

warning: aString at: lineNum [
  | fn |
  warningBlock ifNotNil: [ warningBlock value: aString value: lineNum ]
    ifNil: [
      aString := ('Compile warning near line ' + lineNum + ': ' + aString).
      (fn := self fileName) ifNotNil: [ aString := fn + ': ' + aString ].
      aString printNl.
    ].
]

warning: aString [
  ^self warning: aString at: inStream lineNum
]


dontWantEof: word [
  word ifNil: [ self error: 'unexpected end of file' ].
  ^word
]

expectedWord: want got: word [
  self error: ('"' + want +'" expected, but got "' + word + '"')
]

skipComment [
  | c |
  [ (c := inStream readChar) ifNil: [ ^nil ]. c == $" ] whileFalse: [ nil ].
]

skipBlanksAndComments [
  "skip blanks and comments, return nil or first non-blank char (char is eaten)"
  | c |
  [ [ (c := inStream readChar) ifNil: [ ^nil ]. c isBlank ] whileTrue: [ nil ].
    c == $" ] whileTrue: [ self skipComment. ].
  ^c
]

skipBlanks [
  "skip blanks, return nil or first non-blank char (char is not eaten)"
  | c |
  [ (c := inStream readChar) ifNil: [ ^nil ]. c isBlank ] whileTrue: [ nil ].
  ^inStream unreadChar: c
]

collectUntil: terminator to: buf [
  | pc c |
  c := $ .
  [ pc := c. buf << (self dontWantEof: (c := inStream readChar)). c == terminator ] whileFalse: [ nil ].
  (terminator == $' and: [ pc == $\ ]) ifTrue: [ ^self collectUntil: $' to: buf ].
  ^buf
]

collectBlanksAndCommentsTo: buf [
  | c |
  [ [ (self dontWantEof: (c := inStream readChar)) isBlank ] whileTrue: [ buf << c ].
    c == $" ] whileTrue: [ buf << c. self collectUntil: $" to: buf. ].
  inStream unreadChar: c.
]

lookAhead [
  | c |
  c := inStream readChar.
  ^(inStream unreadChar: c)
  "^(inStream unreadChar: (inStream readChar))"
]

readWord [
  "read one word from input"
  | word c |
  (c := self skipBlanksAndComments) ifNil: [ ^nil ].
  lastWordLine := inStream lineNum.
  c isAlphanumeric ifFalse: [ ^c asString ].
  (word := StringBuffer new) << c.
  [ (c := inStream readChar) ifNil: [ ^word asString ].
    c isAlphanumeric ] whileTrue: [ word << c ].
  inStream unreadChar: c.
  ^word asString
]

expectWord: word [
  | w |
  self dontWantEof: (w := self readWord).
  w ~= word ifTrue: [ self expectedWord: word got: w ].
  ^true
]

charIsSyntax: c [
  ^('.()[]#^$;{}' includesChar: c) or: [ c == $' ]
]

readMethodHeader [
  | c w body mname isKWord isUnary goon |
  self dontWantEof: (mname := self readWord).
  "(self charIsSyntax: mname firstChar) ifTrue: [ self error: 'method name expected' at: lastWordLine ]."
  lastMethodLine := lastWordLine.
  isKWord := false.
  isUnary := false.
  (body := StringBuffer new) << mname.
  mname firstChar isAlphanumeric
    ifTrue: [
      self lookAhead == $: ifTrue: [
        inStream readChar.
        isKWord := true.
        mname := mname + ':'.
        body << $:
       ] ifFalse: [ isUnary := true ].
    ] ifFalse: [
      "read other chars of the method name"
      [ (self charIsSyntax: (self dontWantEof: (c := inStream readChar))) ifTrue: [ self error: 'invalid binary method name' ].
        c isAlphanumeric ] whileFalse: [ body << c ].
      inStream unreadChar: c.  "this is not ours"
    ].
  lastMethodName := mname.
  isUnary ifFalse: [
    "now read args and rest keywords if any"
    [ self collectBlanksAndCommentsTo: body.
      "here we MUST have an argname"
      (self dontWantEof: (w := self readWord)).
      w firstChar isAlphanumeric ifFalse: [ self error: 'argument name expected' ].
      self lookAhead == $: ifTrue: [ self error: 'unexpected keyword' ].
      body << w.  "save argument name"
      isKWord ifTrue: [
         "here must be keyword or sqp"
         self collectBlanksAndCommentsTo: body.
         (self dontWantEof: (c := self lookAhead)) isAlphanumeric ifTrue: [
           "keyword or simple word"
           body << (self dontWantEof: (w := self readWord)).
           (self dontWantEof: inStream readChar) == $: ifFalse: [ self error: 'keyword expected' ].
           body << $:.
           lastMethodName := lastMethodName + w + ':'.
           goon := true.
          ] ifFalse: [
            "not a keyword"
            goon := false.
          ].
       ] ifFalse: [
         goon := false.
       ].
      goon ] whileTrue: [ nil ].
  ].
  "here MUST be sqp"
  self collectBlanksAndCommentsTo: body.
  (self dontWantEof: inStream readChar) == $[ ifFalse: [ self error: '"[" expected' ].
  "now skip until eol or so"
  [ self dontWantEof: (c := inStream readChar) isEOL ] whileFalse: [
    c isBlank ifFalse: [ inStream unreadChar: c. ^(body asString) removeTrailingBlanks ].
  ].
  ^(body asString) removeTrailingBlanks
]

readMethodBody [
  | c body sqcnt |
  body := StringBuffer new.
  sqcnt := 0.
  [ (self dontWantEof: (c := inStream readChar)) == $[ ifTrue: [ sqcnt := sqcnt + 1 ].
    (c == $] and: [ sqcnt = 0 ]) ]
   whileFalse: [
     c == $] ifTrue: [ sqcnt := sqcnt - 1 ].
     body << c.
     (c == $" or: [ c == $' ]) ifTrue: [ self collectUntil: c to: body ]
       ifFalse: [ c == $$ ifTrue: [ body << (self dontWantEof: inStream readChar) ]].
  ].
  "remove trailing blanks"
  body := (body asString) removeTrailingBlanks.
  ^(body + String newline)  "just in case"
]

findLeftMargin: s [
  | minspc lines pos mrg |
  minspc := s size.
  lines := s break: '\n'.
  lines do: [:l |
    l := l removeTrailingBlanks.
    l isEmpty ifFalse: [
      pos := 1. mrg := 0.
      [ (l at: pos) isBlank ] whileTrue: [ mrg := mrg + 1. pos := pos + 1 ].
      "mrg print. '|' print. l printNl."
      mrg < 1 ifTrue: [ ^0 ].
      minspc := minspc min: mrg.
    ]
  ].
  "'margin: ' print. minspc printNl."
  ^minspc
]

removeLeftMargin: s [
  | mrg lines sbuf |
  mrg := (self findLeftMargin: s) + 1.
  lines := s break: '\n'.
  sbuf := StringBuffer new.
  lines do: [:l |
    l := l removeTrailingBlanks.
    l isEmpty ifFalse: [ l := l from: mrg ].
    sbuf << '  ' << l << '\n'.
  ].
  ^sbuf asString.
]

parseCategory [
  "single-quote char already skiped"
  | name c |
  name := StringBuffer new.
  [ (self dontWantEof: (c := inStream readChar)) == $' ] whileFalse: [ name << c ].
  category := name asString.
]

compileMethod: aClass [
  "return true if method was succesfully parsed or false if sqp found"
  | c mtname isMeta hdr body p |
  (self dontWantEof: (c := self skipBlanksAndComments)) == $] ifTrue: [ ^false ].
  (isMeta := (c == $^)) ifFalse: [
    c == $' ifTrue: [ self parseCategory. ^self compileMethod: aClass ].
    (self charIsSyntax: c) ifTrue: [ self error: 'method name or "]" expected' ].
    inStream unreadChar: c.
  ].
  hdr := self readMethodHeader.
  body := self readMethodBody.
  "hdr print. '|' printNl. body print. '||' printNl."
  body := self removeLeftMargin: body.
  "body print. '|||' printNl."
  body := hdr + '\n' + body.
  mtname := (aClass getName asString) + '>>' + (isMeta ifTrue: ['^'] ifFalse: ['']) + lastMethodName.
  p := LstCompiler new.
  p errorBlock: [ :msg :lineNum | self error: 'in method "' + mtname + '": ' + msg at: lastMethodLine + lineNum - 1 ].
  p warningBlock: [ :msg :lineNum | self warning: 'in method "' + mtname + '": ' + msg at: lastMethodLine + lineNum - 1 ].
  c := (isMeta ifTrue: [ aClass class ] ifFalse: [ aClass ]).
  p category: category.
  (c addMethod: body withCompiler: p) ifNil: [ self error: 'in method "' + mtname + '": compilation error' at: lastMethodLine ].
  ^true
]

compileMethods: aClass [
  category := ''.
  [ self compileMethod: aClass ] whileTrue: [ nil ].
]

doExtend: className [
  "parse 'extend' directive"
  | aClass |
  self expectWord: '['.
  aClass := globals at: className asSymbol ifAbsent: [ self error: 'unknown class name: ' + className ].
  self compileMethods: aClass.
]

parseVarDefs [
  "parse variable definitions between '|' (if any)"
  | c vars |
  self skipBlanks.
  self dontWantEof: (c := inStream readChar).
  vars := (Array new: 0).
  c == $| ifTrue: [
    "wow! we have some vars!"
    [ self dontWantEof: (c := self readWord).
      c = '|' ]
     whileFalse: [
      c firstChar isAlphanumeric ifFalse: [ self error: 'invalid variable name: "' + c + '"' ].
      vars := vars with: c asSymbol.
    ].
  ] ifFalse: [
    inStream unreadChar: c.
  ].
  ^vars
]

doSubclass: className asProxy: isProxy [
  | aClass newClassName cvars ivars |
  aClass := globals at: className asSymbol ifAbsent: [ self error: 'unknown class name: ' + className ].
  newClassName := self readWord.
  self dontWantEof: newClassName.
  newClassName lastChar isAlphanumeric ifFalse: [ self error: 'invalid class name: "' + newClassName + '"' ].
  (globals at: newClassName asSymbol ifAbsent: [ nil ]) ifNotNil: [ ('redefinition of class "' + newClassName + '"') printNl ].
  "parse class variables (if any)"
  cvars := self parseVarDefs.
  self expectWord: '['.
  "parse instance variables (if any)"
  ivars := self parseVarDefs.
  "create new class"
  aClass := (aClass subclass: newClassName asSymbol variables: ivars classVariables: cvars asProxy: isProxy).
  self compileMethods: aClass.
  ^nil
]

doEval [
  | body |
  body := (self readMethodBody) removeTrailingBlanks.
  body isEmpty ifFalse: [ body doItSeq. ]
]

doRequires [
  | mname c |
  "FIXME: word parsing must be strict; also process comments"
  [ (self dontWantEof: (mname := self readWord)) = ']' ]
   whileFalse: [
     [ (self dontWantEof: (c := self lookAhead)) == $/ ] whileTrue: [
       inStream readChar.  "skip slash"
       ('\t\n\r ]' includesChar: (self dontWantEof: (c := self lookAhead))) ifTrue: [
         self error: 'invalid module name'
       ].
       mname := mname + '/' + self readWord.
     ].
     mname firstChar isAlphanumeric ifFalse: [ self error: 'module name expected' ].
     System loadModule: mname.
  ].
]

doSetPackage [
  | aName |
  (self dontWantEof: (aName := self readWord)) = ']' ifTrue: [ Package current: 'User'. ^self ].
  aName firstChar isAlphanumeric ifFalse: [ self error: 'package name expected' ].
  Package current: aName.  "returns isNew"
  (self dontWantEof: (aName := self readWord)) = ']' ifTrue: [ ^self ].
  ((aName = 'requires') and: [ inStream readChar == $: ]) ifFalse: [ self error: '"requires:" expected' ].
  self error: 'Package: no support for "requires:" yet!'.
  self expectWord: ']'.
]

readCurly [
  | body c |
  body := StringBuffer new.
  [ (self dontWantEof: (c := inStream readChar)) == $} ]
   whileFalse: [
     body << c.
     (c == $" or: [ c == $' ]) ifTrue: [ self collectUntil: c to: body ].
  ].
  ^(body asString) removeTrailingBlanks
]

doCommand1: cmd [
  inStream readChar.
  Case test: cmd;
    case: 'class' do: [ self doSubclass: 'Object' asProxy: false ];
    case: 'proxy' do: [ self doSubclass: 'Object' asProxy: true ];
    else: [ self error: ('invalid command: ' + cmd) ].
]

doCommand0: cmd [
  Case test: cmd;
    case: 'Eval' do: [ self doEval ];
    case: 'Requires' do: [ self doRequires ];
    case: 'Package' do: [ self doSetPackage ];
    else: [ self error: ('invalid command: ' + cmd) ].
]

doCommand2: cmd withClass: className [
  cmd firstChar isAlphanumeric ifFalse: [ self error: 'command name expected' ].
  Case test: cmd;
    case: 'extend' do: [ self doExtend: className ];
    case: 'subclass' do: [
      inStream readChar == $: ifFalse: [ self error: '":" expected after "subclass"' ].
      self doSubclass: className asProxy: false.
    ];
    else: [ self error: ('invalid command: ' + className) ].
]

parse [
  | className cmd c oldpkg |
  "deal with shebangs"
  (c := inStream readChar) == $# ifTrue: [
    "possible shebang, skip it"
    [ (c := inStream readChar) ifNil: [ ^true ].
      c isEOL ] whileFalse: [ nil ].
  ] ifFalse: [ inStream unreadChar: c ].
  "main part"
  oldpkg := Package current.
  Package current: 'User'.  "egg solution; should restore package on error"
  category := ''.
  [ className := self readWord ] whileNotNil: [
    className firstChar isAlphanumeric ifTrue: [
      (inStream unreadChar: (inStream readChar)) == $: ifTrue: [
        self doCommand1: className
      ] ifFalse: [
        (self dontWantEof: (cmd := self readWord)) = '[' ifTrue: [
          self doCommand0: className
        ] ifFalse: [
          self doCommand2: cmd withClass: className.
        ].
      ].
    ] ifFalse: [
      className = '{' ifFalse: [ self error: ('invalid command: ' + className) ].
      c := self readCurly.
      c isEmpty ifFalse: [ c doItSeq ].
    ].
  ].
  Package setCurrent: oldpkg.
  ^true
]
!
