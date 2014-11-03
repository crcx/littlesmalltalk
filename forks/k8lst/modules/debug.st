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
"debug.st - a simple interactive debugger for Tiny Smalltalk
 by Andy Valencia, May 2001
 heavily modified by Ketmar // Vampire Avalon

 To use the debugger, fileIn: this file.  Then do:

 Debug run: 'command...'

 The given command line will be compiled (much as it would if you had
 typed it interactively).  Then the debugger will be started on this
 command itself.  Generally, you'll do a 's' (or 'step') to step down
 from the command line into the first method call.
"


Package [
  Debug
]

Requires [ disasm ]


"======== String ========"
String extend [
asLines [
  | c ret slot idx base top nl s |
  "Convert a string with embedded newlines into an Array with one slot per line.
   The newlines are not a part of these lines."
  "Size the array of lines which will result"
  nl := Char newline.
  ret := Array new: (self occurencesOf: nl) + 1.
  "Walk the elements, assembling lines"
  slot := base := idx := 1.
  top := self size.
  [idx < top] whileTrue: [
    c := self at: idx.
    c = nl ifTrue: [
      (s := self from: base to: idx) ifNil: [ s := '' ].
      ret at: slot put: s removeTrailingBlanks.
      slot := slot + 1.
      base := idx + 1
    ].
    idx := idx + 1
  ].
  (idx > top) ifTrue: [ idx := top ].
  (idx > base) ifTrue: [
    (s := self from: base to: idx) ifNil: [ s := '' ].
    ret at: slot put: s removeTrailingBlanks.
  ].
  ^ret
]
]


"======== Class ========"
Class extend [
setDebug: aMethodName [
  | meth cl dict |
  aMethodName := aMethodName asSymbol.
  "Chase up the superclass chain, trying to find our Method"
  cl := self. meth := nil.
  [ meth ] whileNil: [
    dict := cl methods.
    meth := dict at: aMethodName ifAbsent: [ cl := cl superclass. nil ].
    cl ifNil: [
      ('Undefined method ' + aMethodName + ' for class ' + self printString) printNl.
      ^nil
    ]
  ].
  "FIXME: certain methods for classes like Char can't be recompiled with debugger now"
  "cl == Char ifTrue: [ ^meth ]."
  "cl isMeta ifTrue: [ ^meth ]."  "debugger fails for MetaChar>>new: and maybe for others"
  ^meth
]
]


"======== DebugMethodIFace ========"
class: DebugMethodIFace [
| mth textlines bpoints active |
^newFor: aMethod [
  | obj |
  obj := self new.
  self in: obj var: #mth put: aMethod.
  self in: obj var: #active put: false.
  ^obj
]

active [
  ^active
]

active: a [
  active := a
]

textlines [
  textlines ifNil: [ textlines := mth text asLines ].
  ^textlines
]

hasBreakAt: bp [
  bpoints ifNil: [ ^false ].
  ^bpoints includes: bp.
]

putBreakAt: bp value: old [
  bpoints ifNil: [ bpoints := Dictionary new ].
  bpoints at: bp put: old.
]

delBreakAt: bp [
  | res |
  bpoints ifNil: [ ^nil ].
  (bpoints includes: bp) ifFalse: [ ^nil ].
  res := bpoints at: bp.
  bpoints removeKey: bp.
  bpoints isEmpty ifTrue: [ bpoints := nil ].
  ^res
]

breakValueAt: bp [
  bpoints ifNil: [ ^nil ].
  (bpoints includes: bp) ifFalse: [ ^nil ].
  ^bpoints at: bp.
]

breakpoints [
  ^bpoints
]
]


"======== Method ========"
Method extend [
setupDebugData [
  dbgData ifNil: [ dbgData := DebugMethodIFace newFor: self ]
]

textlines [
  self setupDebugData.
  ^dbgData textlines.
]

srcLine: bp [
  "map the VM instruction byte pointer onto a source line #"
  | line |
  (line := self findLineForPC: bp) ifNotNil: [ ^line ].
  'No match for ' print. bp printString printNl.
  ^nil
]

whatis: var in: ctx [
  "describe a variable in this method"
  | idx obj |
  obj := nil.

  "special name"
  var = 'self' ifTrue: [
    var print. ' is a self reference' print.
    obj := ctx arguments at: 1.
    idx := 1.
  ].

  "name of an argument"
  obj ifNil: [
    argNames ifNotNil: [
      (idx := argNames indexOfVal: var) ifNotNil: [
        idx := idx + 1.
        var print. ' is an argument' print.
        obj := ctx arguments at: idx.
      ]
    ]
  ].

  "temporary"
  obj ifNil: [
    tempNames ifNotNil: [
      (idx := tempNames indexOfVal: var) ifNotNil: [
        var print. ' is a method variable' print.
        obj := ctx temporaries at: idx
      ]
    ]
  ].

  "instance variable"
  obj ifNil: [
    (idx := (self forClass instanceVariables) indexOfVal: var) ifNotNil: [
      var print. ' is an instance variable' print.
      obj := ctx arguments at: 1.
      obj := (Object class in: obj at: idx)
    ]
  ].

  "if we found it, display the generic information"
  obj ifNotNil: [
    ' (index ' print. idx print. ')' printNl.
    ' Value: ' print. obj printNl.
    ' Class: ' print. obj class printString print.
    ' basicSize: ' print. obj basicSize printNl.
  ] ifFalse: [
    "couldn't find it..."
    'Unknown variable: ' print. var printNl
  ]
]

getVar: var in: ctx ifAbsent: aBlock [
  "get a variable in this method, return its value"
  | idx |
  "special name"
  var = 'self' ifTrue: [ ^ctx arguments at: 1 ].
  "name of an argument"
  argNames ifNotNil: [ (idx := argNames indexOfVal: var) ifNotNil: [ ctx arguments at: idx + 1 ]].
  "temporary"
  tempNames ifNotNil: [ (idx := tempNames indexOfVal: var) ifNotNil: [ ctx temporaries at: idx ]].
  "instance variable"
  (idx := (self forClass instanceVariables) indexOfVal: var) ifNotNil: [
    var := ctx arguments at: 1.
    ^(Object class in: var at: idx)
  ].
  "couldn't find it..."
  ^aBlock value
]

getVar: var in: ctx [
  ^self getVar: var in: ctx ifAbsent: [ ^nil ]
]

print: var in: ctx [
  "print a variable in this method"
  | obj |
  obj := self getVar: var in: ctx ifAbsent: [ ('Unknown variable: ' + var) printNl. ^self ].
  obj printNl
]

setBreak: bp [
  "set a breakpoint in this method"
  | old |
  self setupDebugData.
  "if already set, ignore"
  (dbgData hasBreakAt: bp) ifTrue: [ ^self ].
  "record current opcode at code location and remember breakpoint"
  old := byteCodes at: bp + 1.
  dbgData putBreakAt: bp value: old.
  "update the code location if it's already active"
  dbgData active ifTrue: [ self patchBreak: bp active: true ].
]

clearBreak: bp [
  "remove a breakpoint in this method"
  self setupDebugData.
  "if not set, ignore"
  (dbgData hasBreakAt: bp) ifFalse: [ ^self ].
  "restore code contents"
  self patchBreak: bp active: false.
  "remove record of this breakpoint"
  dbgData delBreakAt: bp.
]

patchBreak: bp active: flag [
  "set or clear the breakpoint instruction in the code"
  flag ifTrue: [
    "patch in a DoSpecial operation 12 (breakpoint)"
    byteCodes at: (bp + 1) put: ((15*16) + 12).
  ] ifFalse: [
    "restore old code at this location"
    byteCodes at: (bp + 1) put: (dbgData breakValueAt: bp).
  ]
]

breakActive: flag [
  "activate or deactivate breakpoints for this method"
  | bpl |
  self setupDebugData.
  (bpl := dbgData breakpoints) ifNil: [ ^self ].
  "skip all this if we aren't changing settings"
  (dbgData active = flag) ifTrue: [ ^self ].
  "for each location with a breakpoint, update it"
  bpl keysDo: [:bp | self patchBreak: bp active: flag].
  dbgData active: flag.
]

codeLoc: line [
  "map source line # to a code location"
  ^self findPCForLine: line
]

browse: args [
  "get the DebugMethod, which has symbolic information for variables"
  '================' printNl.
  self forClass printString print. '>>' print. name printNl.
  '----------------' printNl.
  text printNl.
  '----------------' printNl.
]

internalPrintValue: obj [
  (obj isKindOf: Context)
    ifTrue: [
      '{' print. obj class print. '}' printNl.
    ] ifFalse: [
      obj printNl.
    ].
]

printAllVarsIn: ctx [
  | iv cls |
  '=========' printNl.
  argNames ifNotNil: [
    'arguments:' printNl.
    1 to: argNames size do: [:idx |
      ' ' print. (argNames at: idx) print. ' = ' print.
      self internalPrintValue: (ctx arguments at: idx + 1).
    ]
  ].
  tempNames ifNotNil: [
    'method vars:' printNl.
    1 to: tempNames size do: [:idx |
      ' ' print. (tempNames at: idx) print. ' = ' print.
      self internalPrintValue: (ctx temporaries at: idx).
    ]
  ].
  iv := self forClass instanceVariables.
  iv isEmpty ifFalse: [
    cls := ctx arguments at: 1.
    'instance vars:' printNl.
    1 to: iv size do: [:idx |
      ' ' print. (iv at: idx) print. ' = ' print.
      self internalPrintValue: (self in: cls at: idx).
    ]
  ].
  '---------' printNl.
]

debugOn [
]
]


"======== Debug ========"
Object subclass: Debug [
| proc bpoints prevList selctx  lastDbgWarnMethod |

runIt: count [
  | ret |
  "blow away any selected context when we run"
  selctx := nil.
  "execute for one instruction
   return whether or not the return was 'normal' (i.e., VM stopped due to debugger control, not something else)
   spit out a message for a breakpoint"
  ret := proc doExecute: count + 1.
  (ret = 5) ifTrue: [ ^true ].
  (ret = 6) ifTrue: [ self onBreak. ^true ].
  (ret = 2) ifTrue: [ 'Error trap' printNl ].
  (ret = 3) ifTrue: [ 'Message not understood' printNl ].
  (ret = 4) ifTrue: [ 'Method returned' printNl ].
  (ret = 7) ifTrue: [ self error: 'DEBUGGER ERROR: no "yiled" processing yet' ].
  ^false
]

srcLine: ctx [
  "get source line corresponding to current byte pointer"
  ^(ctx method) srcLine: ctx bytePointer.
]

showLine: ctx [
  "show source line corresponding to current VM instruction of a given context"
  | line meth |
  ctx ifNil: [ ^nil ].
  meth := ctx method.
  line := self srcLine: ctx.
  line
    ifNil: [
      'Method ' print.  meth name print.
      ' for class ' print.  meth forClass print.
      ': no source displayed.' printNl
    ]
    ifNotNil: [
      ' ' print. line print. ': ' print.
      (meth textlines at: line ifAbsent: ['']) printNl
    ].
]

showLine [
  "display current line of active procedure"
  ^self showLine: self curContext
]

^run: line [
  "run a command line under the debugger"
  | meth ret ctx proc |
  meth := (LstCompiler new text: ('debugCmd ^' + line) instanceVars: #()) compileWithClass: Undefined.
  meth ifNotNil: [
    meth debugOn.
    ret := super new.
    ctx := Context new.
    ctx setup: meth withArguments: (Array new: 1).
    proc := Process new.
    proc context: ctx.
    ret proc: proc.
    ret run.
  ]
]

proc: p [
  "initialize our new debug session"
  proc := p.
  bpoints := Array new: 0.
]

atCall [
  "tell if the VM instruction pointer is at a method invocation"
  "TODO: process unary and binary messages"
  | ret meth ctx pc low high |
  "get the next instruction"
  ctx := proc context.
  meth := ctx method.
  pc := ctx bytePointer.
  high := meth byteCodes at: (pc + 1) ifAbsent: [ ^nil ].
  pc := pc + 1.
  low := high % 16.
  high := high / 16.
  (high = 0) ifTrue: [
    high := low.
    low := meth byteCodes at: (pc + 1) ifAbsent: [ ^nil ].
    pc := pc + 1
  ].
  "return nil if it isn't a SendMessage"
  (high = 9) ifFalse: [ ^nil ].
  "otherwise return the selector and return address"
  ret := Array new: 2.
  ret at: 1 put: (meth literals at: (low + 1)).
  ret at: 2 put: pc.
  ^ret
]

stepCall: sel [
  "set up to step into a new method"
  | ctx stack sp args target meth |
  ctx := proc context.
  stack := ctx stack.
  sp := ctx stackTop.
  args := stack at: sp.
  target := args at: 1.
  meth := target class setDebug: sel.
  meth ifNil: [ ^true ].
  (self runIt: 1) ifFalse: [
    'Execution done in ' print.
    meth name print.
    ' of class ' print.
    target class printNl.
    ^true
  ].
  ^false
]

onBreak [
  "tell if we're at a breakpoint; as a side effect, display this fact to the user"
  | ctx meth rec |
  ctx := proc context.
  ctx ifNil: [ ^false ].
  meth := ctx method.
  1 to: bpoints size do: [:idx|
    rec := bpoints at: idx.
    (((rec at: 1) = meth) and:
        [(rec at: 2) = (self srcLine: ctx)])
    ifTrue: [
      'Breakpoint ' print. idx print. ' hit in ' print.
      meth name printString print. '/' print.
      (rec at: 2) printNl.
      ^true
    ]
  ].
  ^false
]

overCall: pc [
  "set a breakpoint at the instruction beyond the SendMessage"
  | ctx res meth |
  ctx := proc context.
  "if we're within a non-debug method, just limp forward"
"
  (self isDebugMethod: (meth := ctx method)) ifFalse: [
    'doing single stepping' printNl.
    self runIt: 1.
    ^false
  ].
"
  meth := ctx method.
  "otherwise break beyond the call"
  meth setBreak: pc.
  "now let it run until it hits the breakpoint, and clear the breakpoint"
  self breakActive: true. meth breakActive: true.
  res := self runIt: -1.
  self breakActive: false.  meth clearBreak: pc.
  res ifTrue: [
    "should be stopped at the expected location"
    ((proc context = ctx) and: [ ctx bytePointer = pc ]) ifTrue: [ ^false ].
    "or hit some other breakpoint"
    (self onBreak) ifTrue: [ ^false ].
    "otherwise, what's going on?"
    'Unexpected run completion' printNl.
    ^true
  ].
  "some other error killed us"
  'Execution aborted' printNl.
  ^true
]

doStep: intoCalls [
  "implement a single step, stepping either over or into calls (method invocations) depending on the intoCalls argument"
  | srcl ret ctx |
  ctx := proc context.
  srcl := self srcLine: ctx.
  [(proc context == ctx) and: [srcl == (self srcLine: ctx)]] whileTrue: [
    "if dropping into a new method, deal with it"
    ret := self atCall.
    ret ifNotNil: [
      "stepping into the call"
      intoCalls ifTrue: [ ^self stepCall: (ret at: 1) ].
      "stepping over call"
      (self overCall: (ret at: 2)) ifTrue: [ ^true ].
    ] ifNil: [
      "otherwise run a single VM operation"
      (self runIt: 1) ifFalse: [
        'Execution done at line ' print. srcl printString printNl.
        ^true
      ]
    ]
  ].
  ^false
]

printReg: reg in: ctx [
  "print a VM register"
  (reg = '$pc') ifTrue: [ ctx bytePointer print. ^self ].
  (reg = '$sp') ifTrue: [ ctx stackTop print. ^self ].
  'Unknown register: ' print. reg print
]

curContext [
  selctx ifNil: [ ^proc context ].
  ^selctx
]

whatis: args [
  "display arguments, temporaries, instance variables, and VM registers"
  | ctx meth |
  "get the DebugMethod, which has symbolic information for variables"
  ctx := self curContext.
  "(self isDebugMethod: (meth := ctx method)) ifFalse: [ ^nil ]."
  meth := ctx method.
  "walk each variable, printing its value"
  args do: [:var |
    var print. ': ' print.
    ((var at: 1) == $$)
    ifTrue: [
      var print. ' is a register variable' printNl.
    ] ifFalse: [
      meth whatis: var in: ctx
    ]
  ]
]

examine: args [
  "display arguments, temporaries, instance variables, and VM registers"
  | ctx meth |
  "get the DebugMethod, which has symbolic information for variables"
  ctx := self curContext.
  "(self isDebugMethod: (meth := ctx method)) ifFalse: [ ^nil ]."
  meth := ctx method.
  "walk each variable, printing its value"
  args do: [:var |
    var print. ': ' print.
    ((var at: 1) == $$)
    ifTrue: [
      self printReg: var in: ctx
    ] ifFalse: [
      meth print: var in: ctx
    ].
    '\n' print.
  ]
]

setBreak: args [
  "set a breakpoint"
  | s cl clname meth methname i rec lineNum inClass arg loc |
  "map straight line # to current class/method"
  arg := args at: 1.
  ((arg at: 1) isDigit) ifTrue: [
    lineNum := arg asNumber.
    lineNum ifNil: [
      'Bad line #' print. arg printNl.
      ^nil
    ].
    meth := self curContext method.
    arg := (meth forClass printString) + '/' + (meth name printString) + '/' + lineNum printString.
  ].
  "parse <class>:<method>"
  s := arg break: '/'.
  (s size < 2) ifTrue: [
    'Format is <class>/<method>' printNl.
    ^nil
  ].
  "look up in instance methods unless it's Meta<class>, in which case trim the 'Meta' and look up in class methods"
  clname := s at: 1.
  ((clname from: 1 to: 4) = 'Meta') ifTrue: [
    inClass := true.
    clname := clname from: 5 to: clname size
  ] ifFalse: [
    inClass := false
  ].
  clname := clname asSymbol.
  methname := (s at: 2) asSymbol.
  "parse line number"
  (s size > 2) ifTrue: [
    lineNum := (s at: 3) asNumber.
    lineNum ifNil: [
      'Bad line #' print. (s at: 3) printNl.
      ^nil
    ]
  ] ifFalse: [
    lineNum := 1
  ].
  "find class"
  cl := Smalltalk at: clname ifAbsent: [
    ('Unknown class: ' + clname printString) printNl.
    ^nil
  ].
  "convert to metaclass if needed"
  inClass ifTrue: [ cl := cl class ].
  "now get method, in its debuggable format"
  meth := cl setDebug: methname.
  meth ifNil: [
    ('Unknown method: ' + methname printString) printNl.
    ^nil
  ].
  "(self isDebugMethod: meth) ifFalse: [ ^nil ]."
  "if it's already set, don't do it again"
  rec := Array with: meth with: lineNum.
  i := bpoints indexOfVal: rec.
  i ifNotNil: [
    'Already set as breakpoint ' print.
    i printNl.
    ^nil
  ].
  "see if we can turn line # into a code location"
  loc := meth codeLoc: lineNum.
  loc ifNil: [
    'No code for source line ' print. lineNum printNl.
    ^nil
  ].
  "set & record the breakpoint"
  meth setBreak: loc.
  bpoints := bpoints with: rec
]

clearBreak: args [
  "delete an existing breakpoint"
  | arg n rec meth lineNum |
  arg := args at: 1 ifAbsent: ['Missing argument' printNl. ^nil].
  n := arg asNumber.
  n ifNil: [
    ('Invalid argument: ' + arg) printNl
  ] ifNotNil: [
    ((n < 1) or: [n > bpoints size]) ifTrue: [
      ('No such breakpoint: ' + arg) printNl
    ] ifFalse: [
      rec := bpoints at: n.
      meth := rec at: 1.
      lineNum := rec at: 2.
      meth clearBreak: (meth codeLoc: lineNum).
      bpoints := bpoints removeIndex: n.
      n print. ': deleted' printNl
    ]
  ]
]

listBreak [
  "list breakpoints"
  | rec meth lineNum |
  'Breakpoints:' printNl.
  1 to: bpoints size do: [:x|
    x print. ': ' print.
    rec := bpoints at: x.
    meth := rec at: 1.
    lineNum := rec at: 2.
    meth name printString print. '/' print.
    lineNum printNl
  ]
]

breakActive: flag [
  "make all our breakpoints active or inactive, depending on flag's value"
  | meths |
  meths := Set new.
  bpoints do: [:rec|
    meths add: (rec at: 1)
  ].
  meths do: [:meth| meth breakActive: flag]
]

list: args [
  "list source code"
  | meth where src ctx |
  "get the method we're going to display"
  ctx := self curContext.
  "(self isDebugMethod: (meth := ctx method)) ifFalse: [ ^self ]."
  meth := ctx method.
  "either continue listing, or start from the given place"
  (args size < 1) ifTrue: [
    prevList ifNil: [
      "list around where we're currently executing"
      where := (self srcLine: ctx) - 5
    ] ifNotNil: [
      where := prevList + 1
    ]
  ] ifFalse: [
    where := (args at: 1) asNumber.
    where ifNil: [
      'Invalid line number: ' print.
      (args at: 1) printNl.
      ^self
    ]
  ].
  "show 9 lines"
  src := meth textlines.
  where to: (where + 8) do: [:x|
    ((x > 0) and: [x <= src size]) ifTrue: [
      (x printString printWidth: 8) print.
      (src at: x) printNl.
      prevList := x
    ]
  ]
]

nextContext: ctx [
  "return next context deeper in context stack
   because contexts are only forward linked, we have to search from the top inward, then return the next one out"
  | c prev |
  c := proc context.
  [(prev := c previousContext) ~= ctx] whileTrue: [
    prev ifNil: [ ^nil ].
    c := prev
  ].
  ^c
]

upDown: up count: args [
  "move up or down the stack frames"
  | c count |
  "if nothing selected, start from bottom of stack"
  selctx := self curContext.
  "get count, default 1"
  (args size > 0) ifTrue: [
    count := (args at: 1) asNumber
  ] ifFalse: [
    count := 1
  ].
  "walk the context chain"
  1 to: count do: [:ignore |
    "get next/prev context depending on step direction"
    up ifTrue: [
      c := selctx previousContext
    ] ifFalse: [
      c := self nextContext: selctx
    ].
    "just ignore running off the end"
    c ifNotNil: [ selctx := c ]
  ]
]

makeDebug: args [
  "convert Class methods to DebugMethod's"
  | cl meta n |
  ^self
  args do: [:clname |
    "map MetaFOO -> FOO class"
    ((clname from: 1 to: 4) = 'Meta') ifTrue: [
      n := clname from: 5.
      meta := true
    ] ifFalse: [
      n := clname.
      meta := false
    ].
    "look up class"
    cl := Smalltalk at: n asSymbol ifAbsent: [ nil ].
    cl
     ifNil: [ ('Unknown class: ' + clname) printNl ]
     ifNotNil: [
      "map to metaclass if needed"
      meta ifTrue: [ cl := cl class ].
      "convert methods"
      cl methods keysDo: [:k | cl setDebug: k ]
     ]
  ]
]


showHelp [
  'available debugger commands' printNl.
  '---------------------------' printNl.
  's  step\n   single step to the next line' printNl.
  'si  stepi\n   single step one VM instruction' printNl.
  'n  next\n   step over method calls on the current line' printNl.
  'b <class>/<method>[/<line #>]  break <class>/<method>[/<line #>]' printNl.
    '   set a breakpoint at the named method.' printNl.
    '   meta<class> accesses class methods.' printNl.
    '   a plain line number applies to the current class/method.' printNl.
  'c  cont\n   continue running until error, completion, or breakpoint.' printNl.
  'd int  delete int\n   delete breakpoint.' printNl.
  'lb  listbreak\n   list breakpoints.' printNl.
  'p varlist  print varlist\n   print variable(s).' printNl.
    '   You may also use $pc (VM instruction pointer) and $sp (VM stack top pointer).' printNl.
  'q  quit\n   leave the debugger (abandon the executing target code).' printNl.
  'where  bt\n   show stack backtrace.' printNl.
  'l [line]  list [line]\n   list source (can provide a line # as argument).' printNl.
  'whatis\n   describe variable more thoroughly.' printNl.
  'up  down\n   move up and down the call stack for purposes of accessing variables.' printNl.
  'debug class\n   compiles all method''s for that class in their debuggable form.' printNl.
  'br  browse\n   invokes system data structure browser.' printNl.
  'di  disasm\n   disassemble current method.' printNl.
  'allvars\n   show all variables.' printNl.
  '(blank line)\n   re-enter previous command.  Useful for single stepping statements in a row.' printNl.
]

run [
  "main command loop for the debugger session"
  | prev did cmd done line  ctx meth goodpt |
  prev := 's'.
  done := false.
  [ true ] whileTrue: [
    (ctx := self curContext) ifNil: [ ^nil ].
    (meth := ctx method) ifNil: [ ^nil ].
    goodpt := true.
    (ctx := selctx)
      ifNil: [ ctx := self curContext ]
      ifNotNil: [
        (meth = ctx method) ifNil: [ meth := (ctx := self curContext) method. ].
      ].
    goodpt ifTrue: [
      "show disassembled line"
      ' ' print. meth disassemble: 0 at: ctx bytePointer for: 1.
      "show where we are"
      self showLine.
      "show prompt"
      meth forClass printString print. '>>' print. meth name print.
      '(' print. ((ctx bytePointer) printWidth: 4) print. ')' print.
      ' | ' print.
    ] ifFalse: [
      'Debug> ' print.
    ].
    "get command"
    line := String input.
    "re-insert previous command if empty line"
    (line isEmpty) ifTrue: [ line := prev ].
    prev := line.
    "parse into words"
    line := line break: ' \t\n\r'.
    "command is first, arguments follow"
    cmd := line at: 1.
    line := line from: 2 to: line size.
    "set flag to indicate command hasn't matched yet"
    did := false.
    "step a single VM instruction"
    (((cmd = '?') or: [ cmd = 'h' ]) or: [ cmd = 'help' ]) ifTrue: [ self showHelp. did := true ].
    ((cmd = 'si') or: [ cmd = 'stepi' ]) ifTrue: [
      done
       ifTrue: [ 'Not runnable' printNl ]
       ifFalse: [
        prevList := nil.
        (self runIt: 1) ifFalse: [
          done := true
        ]
      ].
      did := true
    ].
    "step a source line"
    ((cmd = 'step') or: [cmd = 's']) ifTrue: [
      done
       ifTrue: [ 'Not runnable' printNl ]
       ifFalse: [
         prevList := nil.
         done := self doStep: true
       ].
      did := true
    ].
    "step a source line, stepping over message sends"
    ((cmd = 'next') or: [cmd = 'n']) ifTrue: [
      done
       ifTrue: [ 'Not runnable' printNl ]
       ifFalse: [
         prevList := nil.
         done := self doStep: false
       ].
      did := true.
    ].
    "examine variables"
    ((cmd = 'p') or: [cmd = 'print']) ifTrue: [
      self examine: line.
      did := true
    ].
    "describe variable"
    (cmd = 'whatis') ifTrue: [
      self whatis: line.
      did := true
    ].
    "set a breakpoint"
    ((cmd = 'b') or: [cmd = 'break']) ifTrue: [
      self setBreak: line.
      did := true
    ].

    "clear breakpoint(s)"
    ((cmd = 'd') or: [cmd = 'delete']) ifTrue: [
      self clearBreak: line.
      did := true
    ].
    "list breakpoints"
    ((cmd = 'lb') or: [cmd = 'listbreak']) ifTrue: [
      self listBreak.
      did := true
    ].
    "just let it run"
    ((cmd = 'cont') or: [cmd = 'c']) ifTrue: [
      "clear previous listing position"
      prevList := nil.
      "step forward once, even over a breakpoint"
      done := (self runIt: 1) not.
      "now run until completion or next break"
      done ifFalse: [
        "activate, run, and deactivate"
        self breakActive: true.
        done := (self runIt: -1) not.
        self breakActive: false.
        "display a message if hit a breakpoint"
        done ifFalse: [ self onBreak ].
      ].
      did := true
    ].
    "source listing"
    ((cmd = 'l') or: [cmd = 'list']) ifTrue: [
      self list: line.
      did := true
    ].
    "abandon the method"
    ((cmd = 'q') or: [cmd = 'quit']) ifTrue: [
      ^nil
    ].
    "stack backtrace"
    ((cmd = 'where') or: [cmd = 'bt']) ifTrue: [
      proc context
      ifNil: [
        'Process has terminated' printNl
      ] ifNotNil: [
        proc context backtrace
      ].
      did := true
    ].
    "go up or down the stack chain"
    ((cmd = 'up') or: [cmd = 'down']) ifTrue: [
      self upDown: (cmd = 'up') count: line.
      did := true
    ].
    "make all procedures of the named class debuggable"
    (cmd = 'debug') ifTrue: [
      self makeDebug: line.
      did := true
    ].
    "hook to data structure browser"
    ((cmd = 'br') or: [cmd = 'browse']) ifTrue: [
      self browse: line.
      did := true
    ].
    "disassemble"
    ((cmd = 'di') or: [cmd = 'disasm']) ifTrue: [
      meth disassemble.
      did := true
    ].
    (cmd = 'allvars') ifTrue: [
      meth printAllVarsIn: ctx.
      did := true
    ].
    "error"
    did ifFalse: [ 'Unknown command.' printNl ].
  ]
]

browse: args [
  | meth ctx |
  "get the DebugMethod, which has symbolic information for variables"
  ctx := self curContext.
  "(self isDebugMethod: (meth := ctx method)) ifFalse: [ ^nil ]."
  meth := ctx method.
  meth forClass printString print. '>>' print. meth name printNl.
  meth browse: args.
]
]
