"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [
  Debug
]


"note that this disasm engine can be used to instrument bytecodes;
 if i'll add bytecode generator in each DisasmedInstruction, the
 engine can serve as a basis for writing peephole optimizers, for
 example, or inliners or such"


class: DisasmedInstruction [
  | pc        "in the original bytecode"
    length    "of instruction bytecode"
    mnemonics "mnemonics, without operands"
    "operands area; unused vars are set to nil"
    jmpDest   "branch destination (for branches and block creation)"
    litNum    "literal number"
    unNum     "for SendUnary"
    binNum    "for SendBinary"
    msgSym    "for SendMessage"
    const     "for PushConstant"
    varNum    "for Push/Assign temps, args and insts"
    varSym    "temp, arg or inst varname; can be string for block vars"
    argCount  "for blocks, primitives and MarkArguments"
    primNum   "primitive number"
    primName  "string: primitive name"
    tempLoc   "for PushBlock"
  |

  pc [
    ^pc
  ]
  pc: aVal [
    pc := aVal
  ]

  length [
    ^length
  ]
  length: aVal [
    length := aVal
  ]
  mnemonics [
    ^mnemonics
  ]
  mnemonics: aVal [
    mnemonics := aVal
  ]

  jmpDest [
    ^jmpDest
  ]
  jmpDest: aVal [
    jmpDest := aVal
  ]

  litNum [
    ^litNum
  ]
  litNum: aVal [
    litNum := aVal
  ]

  unNum [
    ^unNum
  ]
  unNum: aVal [
    unNum := aVal
  ]

  binNum [
    ^binNum
  ]
  binNum: aVal [
    binNum := aVal
  ]

  msgSym [
    ^msgSym
  ]
  msgSym: aVal [
    msgSym := aVal
  ]

  const [
    ^const
  ]
  const: aVal [
    const := aVal
  ]

  varNum [
    ^varNum
  ]
  varNum: aVal [
    varNum := aVal
  ]

  varSym [
    ^varSym
  ]
  varSym: aVal [
    varSym := aVal
  ]

  argCount [
    ^argCount
  ]
  argCount: aVal [
    argCount := aVal
  ]

  primNum [
    ^primNum
  ]
  primNum: aVal [
    primNum := aVal
  ]

  primName [
    ^primName
  ]
  primName: aVal [
    primName := aVal
  ]

  tempLoc [
    ^tempLoc
  ]
  tempLoc: aVal [
    tempLoc := aVal
  ]

  printString [
    ^mnemonics
  ]
]


DisasmedInstruction subclass: DisasmPushVar [
  ^new: aMnemo varNum: aVarNum [
    ^(self new);
      mnemonics: aMnemo;
      varNum: aVarNum.
  ]

  printString [
    ^(mnemonics printWidth: 16) + '#' + varSym asString
  ]
]

DisasmPushVar subclass: DisasmPushInstance [
  ^new: aVarNum [
    ^self new: 'PushInstance' varNum: aVarNum
  ]
]

DisasmPushVar subclass: DisasmPushArgument [
  ^new: aVarNum [
    ^self new: 'PushArgument' varNum: aVarNum
  ]
]

DisasmPushVar subclass: DisasmPushTemporary [
  ^new: aVarNum [
    ^self new: 'PushTemporary' varNum: aVarNum
  ]
]


DisasmedInstruction subclass: DisasmAssignVar [
  ^new: aMnemo varNum: aVarNum [
    ^(self new);
      mnemonics: aMnemo;
      varNum: aVarNum.
  ]

  printString [
    ^(mnemonics printWidth: 16) + '#' + varSym asString
  ]
]

DisasmAssignVar subclass: DisasmAssignInstance [
  ^new: aVarNum [
    ^self new: 'AssignInstance' varNum: aVarNum
  ]
]

DisasmAssignVar subclass: DisasmAssignArgument [
  ^new: aVarNum [
    ^self new: 'AssignArgument' varNum: aVarNum
  ]
]

DisasmAssignVar subclass: DisasmAssignTemporary [
  ^new: aVarNum [
    ^self new: 'AssignTemporary' varNum: aVarNum
  ]
]


DisasmedInstruction subclass: DisasmPushLiteral [
  ^new: aLitNum [
    ^(self new);
      mnemonics: 'PushLiteral';
      litNum: aLitNum.
  ]

  printString [
    const class == Symbol ifTrue: [ ^(mnemonics printWidth: 16) + '#' + const asString ].
    const class == String ifTrue: [ ^(mnemonics printWidth: 16) + '\'' + const toPrintable + '\'' ].
    (const isKindOf: Collection) ifTrue: [ ^(mnemonics printWidth: 16) + '<' + const class asString + '>' ].
    ^(mnemonics printWidth: 16) + const printString
  ]
]


DisasmedInstruction subclass: DisasmPushConstant [
  ^new: aConstNum [
    | obj |
    (obj := self new) mnemonics: 'PushConstant'.
    Case test: aConstNum;
      case: 0 do: [ obj const: nil ];
      case: 1 do: [ obj const: true ];
      case: 2 do: [ obj const: false ];
      else: [:t | obj const: t - 3 ].
    ^obj
  ]

  printString [
    ^(mnemonics printWidth: 16) + const asString
  ]
]


DisasmedInstruction subclass: DisasmMarkArguments [
  ^new: aArgNo [
    ^(self new);
      mnemonics: 'MarkArguments';
      argCount: aArgNo.
  ]

  printString [
    ^(mnemonics printWidth: 16) + argCount asString
  ]
]


DisasmedInstruction subclass: DisasmPushBlock [
  ^new: aArgNo jmp: aJmpDest tempLoc: aTempLoc [
    ^(self new);
      mnemonics: 'PushBlock';
      argCount: aArgNo;
      jmpDest: aJmpDest;
      tempLoc: aTempLoc.
  ]

  printString [
    ^(mnemonics printWidth: 16) + 'argc: ' + argCount asString + ' jmpDest: ' + jmpDest asString + ' locStart: ' + tempLoc asString
  ]
]


DisasmedInstruction subclass: DisasmSendMsg [
  ^new: aMnemo litNum: aLitNum name: aNameSym [
    | obj |
    (obj := self new); mnemonics: 'Send'+aMnemo; litNum: aLitNum.
    aNameSym class == Symbol ifTrue: [ obj msgSym: aNameSym ].
    ^obj
  ]

  printString [
    ^(mnemonics printWidth: 16) + '#' + msgSym asString
  ]
]


DisasmSendMsg subclass: DisasmSendUnary [
  ^new: aUnNum [
    | obj |
    (obj := self new) mnemonics: 'SendUnary'.
    aUnNum = 0 ifTrue: [ obj msgSym: #isNil ].
    aUnNum = 1 ifTrue: [ obj msgSym: #notNil ].
    ^obj
  ]
]


DisasmSendMsg subclass: DisasmSendBinary [
  ^new: aBinNum [
    | obj bins |
    (obj := self new); mnemonics: 'SendBinary'; binNum: aBinNum.
    bins := #{< <= + - * / % > >= ~= = & | ==}.
    ((aBinNum >= 0) and: [ aBinNum < bins size ]) ifTrue: [ obj msgSym: (bins at: aBinNum + 1) ].
    ^obj
  ]
]


DisasmSendMsg subclass: DisasmSendMessage [
  ^new: aLitNum name: aNameSym [
    ^self new: 'Message' litNum: aLitNum name: aNameSym
  ]
]

DisasmSendMsg subclass: DisasmSendToSuper [
  ^new: aLitNum name: aNameSym [
    ^self new: 'ToSuper' litNum: aLitNum name: aNameSym
  ]
]


DisasmedInstruction subclass: DisasmDoPrimitive [
  ^new: aPrimNum argCount: aArgNo name: aName [
    ^(self new); mnemonics: 'DoPrimitive'; primNum: aPrimNum; argCount: aArgNo; primName: aName
  ]

  printString [
    ^(mnemonics printWidth: 16) + '#' + primName asString + ' argc: ' + argCount asString
  ]
]


DisasmedInstruction subclass: DisasmBranch [
  ^new: aMnemo jmpDest: aJmpDest [
    ^(self new); mnemonics: 'Branch' + aMnemo; jmpDest: aJmpDest
  ]

  printString [
    ^(mnemonics printWidth: 16) + jmpDest asString
  ]
]


DisasmedInstruction subclass: DisasmReturn [
  ^new: aMnemo [
    ^(self new); mnemonics: aMnemo + 'Return'
  ]
]


DisasmedInstruction subclass: DisasmBreakpoint [
  ^new [
    ^(self basicNew); mnemonics: 'Breakpoint'
  ]
]


DisasmedInstruction subclass: DisasmDuplicate [
  ^new [
    ^(self basicNew); mnemonics: 'Duplicate'
  ]
]


DisasmedInstruction subclass: DisasmPopTop [
  ^new [
    ^(self basicNew); mnemonics: 'PopTop'
  ]
]


DisasmedInstruction subclass: DisasmThisContext [
  ^new [
    ^(self basicNew); mnemonics: 'ThisContext'
  ]
]


Method extend [
  disasmParseInstr: pc [
    "returns DisasmedInstruction or nil; pc is 0-based"
    | opcode oparg oplen res ac jmpd pname |
    "fetch opcode"
    oparg := (opcode := byteCodes at: pc + 1 ifAbsent: [ ^nil ]) bitAnd: 15.
    (opcode := opcode bitShift: -4) = 0 ifTrue: [
      "special opcode form"
      opcode := oparg.
      oparg := byteCodes at: pc + 2 ifAbsent: [ ^nil ].
      oplen := 2.
    ] ifFalse: [ oplen := 1 ].
    Case test: opcode;
      case: 1 do: [
        pname := self forClass instanceVariables.
        oparg >= pname size ifTrue: [ ^nil ].
        (res := DisasmPushInstance new: oparg); varSym: (pname at: oparg + 1)
      ];
      case: 2 do: [
        "FIXME: argNames can be nil"
        oparg = 0
          ifTrue: [ pname := #self ]
          ifFalse: [
            oparg > argNames size ifTrue: [ ^nil ].
            pname := argNames at: oparg ].
        (res := DisasmPushArgument new: oparg); varSym: pname
      ];
      case: 3 do: [
        tempNames ifNotNil: [
          oparg < tempNames size ifTrue: [ oparg := tempNames at: oparg + 1] ifFalse: [ oparg := '#' + oparg asString ].
        ] ifNil: [ oparg := '#' + oparg asString ].
        (res := DisasmPushTemporary new: oparg); varSym: oparg
      ];
      case: 4 do: [
        "FIXME: check oparg bounds"
        (res := DisasmPushLiteral new: oparg) const: (literals at: oparg + 1)
      ];
      case: 5 do: [ res := DisasmPushConstant new: oparg ];
      case: 6 do: [
        pname := self forClass instanceVariables.
        oparg >= pname size ifTrue: [ ^nil ].
        (res := DisasmAssignInstance new: oparg); varSym: (pname at: oparg + 1)
      ];
      case: 7 do: [
        "FIXME: argNames can be nil"
        oparg = 0
          ifTrue: [ pname := #self ]
          ifFalse: [
            oparg > argNames size ifTrue: [ ^nil ].
            pname := argNames at: oparg ].
        (res := DisasmAssignArgument new: oparg); varSym: (argNames at: oparg + 1)
      ];
      case: 8 do: [
        tempNames ifNotNil: [
          oparg < tempNames size ifTrue: [ oparg := tempNames at: oparg + 1] ifFalse: [ oparg := '#' + oparg asString ].
        ] ifNil: [ oparg := '#' + oparg asString ].
        (res := DisasmAssignTemporary new: oparg); varSym: oparg
      ];
      case: 9 do: [ res := DisasmMarkArguments new: oparg ];
      case: 10 do: [  "PushBlock"
        byteCodes size < (pc + oplen + 3) ifTrue: [ ^nil ].
        jmpd := byteCodes wordAt: pc + oplen + 1.
        ac := byteCodes at: pc + oplen + 3.
        oplen := oplen + 3.
        res := DisasmPushBlock new: ac jmp: jmpd tempLoc: oparg.
      ];
      case: 11 do: [ (res := DisasmSendUnary new: oparg) msgSym ifNil: [ ^nil ]];
      case: 12 do: [ (res := DisasmSendBinary new: oparg) msgSym ifNil: [ ^nil ]];
      case: 13 do: [  "SendMessage"
        "FIXME: check oparg bounds"
        (res := DisasmSendMessage new: oparg name: (literals at: oparg + 1)) msgSym ifNil: [ ^nil ]
      ];
      case: 14 do: [  "DoPrimitive"
        opcode := byteCodes at: pc + oplen + 1 ifAbsent: [ ^nil ].
        oplen := oplen + 1.
        (pname := System nameOfPrimitive: opcode) ifNil: [ pname := '#' + opcode asString ].
        res := DisasmDoPrimitive new: opcode argCount: oparg name: pname.
      ];
      case: 15 do: [  "DoSpecial"
        Case test: oparg;
          case: 0 do: [ res := DisasmBreakpoint new ];
          case: 1 do: [ res := DisasmReturn new: 'Self' ];
          case: 2 do: [ res := DisasmReturn new: 'Stack' ];
          case: 3 do: [ res := DisasmReturn new: 'Block' ];
          case: 4 do: [ res := DisasmDuplicate new ];
          case: 5 do: [ res := DisasmPopTop new ];
          when: [ :v | (v > 5) & (v < 11) ] do: [:bc |
            byteCodes size < (pc + oplen + 2) ifTrue: [ ^nil ].
            jmpd := byteCodes wordAt: pc + oplen + 1.
            oplen := oplen + 2.
            Case test: bc;
              case: 6 do: [ res := DisasmBranch new: '' jmpDest: jmpd ];
              case: 7 do: [ res := DisasmBranch new: 'IfTrue' jmpDest: jmpd ];
              case: 8 do: [ res := DisasmBranch new: 'IfFalse' jmpDest: jmpd ];
              case: 9 do: [ res := DisasmBranch new: 'IfNil' jmpDest: jmpd ];
              case: 10 do: [ res := DisasmBranch new: 'IfNotNil' jmpDest: jmpd ].
          ];
          case: 11 do: [  "SendToSuper"
            oparg := byteCodes at: pc + oplen + 1 ifAbsent: [ ^nil ].
            oplen := oplen + 1.
            "FIXME: check oparg bounds"
            (res := DisasmSendToSuper new: oparg name: (literals at: oparg + 1)) msgSym ifNil: [ ^nil ]
          ];
          case: 12 do: [ res := DisasmThisContext new ];
          else: [ ^nil ].
      ];
      else: [ ^nil ].
    res; pc: pc; length: oplen.
    ^res
  ]

  disassemble: indent at: pc for: initCount [
    | pcend instr blockEnds |
    blockEnds := nil.
    pcend := pc + initCount.
    [ pc < pcend ] whileTrue: [
      (instr := self disasmParseInstr: pc) ifNil: [ ^self ].
      "show PC and indent listing of line"
      (pc printWidth: 4) print. ':' print. 1 to: indent do: [:x | ' ' print].
      instr printString printNl.
      pc := pc + instr length.
      instr class == DisasmPushBlock ifTrue: [
        indent := indent + 1.
        blockEnds ifNil: [ blockEnds := List new ].
        blockEnds add: instr jmpDest.
      ].
      blockEnds ifNotNil: [
        pc = blockEnds first ifTrue: [
          (blockEnds removeFirst) isEmpty ifTrue: [ blockEnds := nil ].
          indent := indent - 1.
        ].
      ].
    ]
  ]

  disassemble [
    self disassemble: 1 at: 0 for: (byteCodes size)
  ]
]


Class extend [
  disasmMethod: nm [
    | meth |
    meth := self allMethods at: nm ifAbsent: [ ^self error: 'no such method' ].
    'max stack size: ' print. meth stackSize printNl.
    meth disassemble.
  ]
]
