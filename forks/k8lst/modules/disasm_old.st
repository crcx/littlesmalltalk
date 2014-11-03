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
  Debug
]


Method extend [
disassemble: indent at: initPC for: initCount [
  | pc low high pcend needCR ac pname bins |
  pc := initPC + 1.
  pcend := pc + initCount.
  bins := #('<' '<=' '+' '-' '*' '/' '%' '>' '>=' '~=' '=' '&' '|' '==').
  [ pc < pcend ] whileTrue: [
    "Default, terminate line with CR"
    needCR := true.
    pc > byteCodes size ifTrue: [ ^self ].
    "Show PC and indent listing of line"
    ((pc - 1) printWidth: 4) print. ':' print.
    1 to: indent do: [:x | ' ' print].
    "Fetch basic opcode fields"
    high := byteCodes at: pc.
    pc := pc + 1.
    low := high % 16.
    high := high / 16.
    (high = 0) ifTrue: [
      high := low.
      low := byteCodes at: pc.
      pc := pc + 1.
    ].
    high = 0 ifTrue: [ 'NO MORE CODE' printNl. ^self ].
    Case test: high;
      case: 1 do: [ 'PushInstance ' print. low print ];
      case: 2 do: [ 'PushArgument ' print. low print ];
      case: 3 do: [ 'PushTemporary ' print. low print ];
      case: 4 do: [ 'PushLiteral ' print. low print ];
      case: 5 do: [
        'PushConstant ' print.
        Case test: low;
          case: 0 do: [ 'nil' print ];
          case: 1 do: [ 'true' print ];
          case: 2 do: [ 'false' print ];
          else: [:t | (t - 3) print ].
      ];
      case: 6 do: [ 'AssignInstance ' print. low print ];
      case: 7 do: [ 'AssignArgument ' print. low print ];
      case: 8 do: [ 'AssignTemporary ' print. low print ];
      case: 9 do: [ 'MarkArguments ' print. low print ];
      case: 10 do: [
        low := byteCodes wordAt: pc.
        pc := pc + 2.
        ac := byteCodes at: pc.
        pc := pc + 1.
        high := low - (pc-1).
        'PushBlock argCount=' print. ac printNl.
        self disassemble: indent + 1 at: pc for: high.
        pc := pc + high.
        needCR := false
      ];
      case: 11 do: [
        'SendUnary ' print.
        Case test: low;
          case: 0 do: [ 'isNil' print ];
          case: 1 do: [ 'notNil' print ];
          else: [:t | 'unknown #' print. t print ].
      ];
      case: 12 do: [
        'SendBinary ' print.
        ((low >= 0) and: [ low < bins size ])
          ifTrue: [ (bins at: low + 1) print ]
          ifFalse: [ 'unknown #' print. low print ].
      ];
      case: 13 do: [ 'SendMessage ' print. (literals at: (low+1)) print ];
      case: 14 do: [
        'DoPrimitive ' print.
        high := byteCodes at: pc.
        pc := pc + 1.
        pname := System nameOfPrimitive: high.
        pname ifNil: [ 'unknown #' print. high print ] ifNotNil: [ pname print ].
        '; argc: ' print. low print.
      ];
      case: 15 do: [
        "'DoSpecial ' print."
        Case test: low;
          case: 0 do: [ 'Breakpoint' print ];
          case: 1 do: [ 'SelfReturn' print ];
          case: 2 do: [ 'StackReturn' print ];
          case: 3 do: [ 'BlockReturn' print ];
          case: 4 do: [ 'Duplicate' print ];
          case: 5 do: [ 'PopTop' print ];
          when: [ :v | (v > 5) & (v < 11) ] do: [:bc|
            Case test: bc;
              case: 6 do: [ 'Branch ' print ];
              case: 7 do: [ 'BranchIfTrue ' print ];
              case: 8 do: [ 'BranchIfFalse ' print ];
              case: 9 do: [ 'BranchIfNil ' print ];
              case: 10 do: [ 'BranchIfNotNil ' print ].
            high := byteCodes wordAt: pc.
            pc := pc + 2.
            high print
          ];
          case: 11 do: [
            'SendToSuper ' print.
            low := byteCodes at: pc.
            pc := pc + 1.
            (literals at: low+1) print
          ];
          case: 12 do: [ 'ThisContext' print ];
          else: [:opc | 'unknown special opcode #' print. opc print ].
      ];
      else: [:opc | 'unknown opcode #' print. opc print ].
    needCR ifTrue: [
      Char newline print
    ]
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
  meth disassemble.
]
]
