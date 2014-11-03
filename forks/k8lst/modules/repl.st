"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Requires [ quit ]

Package [
  Repl
]

class: ReplCommand [
| aBlock descr |

^newWith: blk descr: dsc [
  ^(super new) initWith: blk descr: dsc
]

initWith: blk descr: dsc [
  aBlock := blk.
  descr := dsc.
  ^self
]

doWith: args [
  ^aBlock value: args
]

descr [
  ^descr
]
]


class: ReplCommands [
| cmdList |

^new [
  ^(super new) init
]

init [
  cmdList := Dictionary new.
  ^self
]

cmdList [
  ^cmdList
]

add: cmd descr: descr handler: hand [
  | co |
  co := ReplCommand newWith: hand descr: descr.
  cmdList at: cmd put: co.
]

do: cmdline [
  | args cmd |
  args := cmdline break: (' \t').
  args size < 1 ifTrue: [ ^nil ].
  cmd := cmdList at: (args at: 1)
    ifAbsent: [
      (args at: 1) = ':h'
        ifTrue: [
          'avaliable commands:' printNl.
          cmdList keysAndValuesDo: [ :key :value |
            '  ' print. key print. String tab print.
            value descr printNl.
          ].
         ]
        ifFalse: [
          'unknown command: ' print.
          (args at: 1) printNl.
        ].
      ^nil ].
  ^cmd doWith: args
]
]


class: ExtREPL [
| commands |

^new [
  | obj clicmd |
  obj := super new.
  "initialize REPL commands"
  clicmd := ReplCommands new.
  clicmd
    add: ':q'
     descr: 'quit'
     handler: [ :args |
       System quit: 0.
       ^true
     ];
    add: ':x'
     descr: 'save current image and quit'
     handler: [ :args |
       System loadModule: 'binimage'.
      'writing "workimage.image"...' printNl.
      args := File image: 'workimage.image'.
      args print. ' objects written to image.' printNl.
      ^true
    ];
    add: ':w'
     descr: 'save current image'
     handler: [ :args |
       System loadModule: 'binimage'.
      'writing "workimage.image"...' printNl.
      args := File image: 'workimage.image'.
      args print. ' objects written to image.' printNl.
    ];
    add: ':Z'
     descr: 'save image w/o sources'
     handler: [ :args |
       System loadModule: 'binimage'.
      'writing "releaseimage.image"...' printNl.
      args := File imageNoSources: 'releaseimage.image'.
      args print. ' objects written to image.' printNl.
    ];
    add: ':l'
     descr: 'load file'
     handler: [ :args |
       args size = 2 ifTrue: [
         'loading file: ' print. (args at: 2) printNl.
         File fileIn: (args at: 2).
        ]
        ifFalse: [ 'ERROR: invalid number of arguments for ":l"' printNl. ].
    ];
    add: ':m'
     descr: 'load module'
     handler: [ :args |
       args size = 2 ifTrue: [ System loadModule: (args at: 2). ]
        ifFalse: [ 'ERROR: invalid number of arguments for ":m"' printNl. ].
    ];
    add: ':r'
     descr: 'reload module'
     handler: [ :args |
       args size = 2 ifTrue: [ System reloadModule: (args at: 2). ]
        ifFalse: [ 'ERROR: invalid number of arguments for ":r"' printNl. ].
    ];
    add: ':p'
     descr: 'show packages or package classes'
     handler: [ :args :pkg |
       args size = 1 ifTrue: [
         Package packages keysDo: [:obj | obj asString printNl. ].
       ] ifFalse: [
         args size = 2 ifTrue: [
          pkg := Package find: (args at: 2) asSymbol.
          pkg ifNil: [ 'ERROR: invalid package name' printNl ]
          ifNotNil: [
            pkg name asString printNl.
            pkg classes do: [:obj |
              (obj isKindOf: Class) ifTrue: [
                obj isMeta ifFalse: [ ' ' print. obj asString printNl. ]
             ]].
           ]
         ] ifFalse: [
           'ERROR: invalid number of arguments for ":p"' printNl.
         ]
       ].
    ];
    add: ':cm'
      descr: 'show class methods'
      handler: [ :args :cls |
        args size = 2 ifTrue: [
          cls := globals at: ((args at: 2) asSymbol) ifAbsent: [ nil ].
          ((cls notNil) and: [ cls isKindOf: Class ]) ifTrue: [
            cls asString printNl.
            cls class methods do: [:mth | ' ^' print. mth name asString printNl. ].
            cls methods do: [:mth | ' ' print. mth name asString printNl. ].
          ].
        ] ifFalse: [
          'ERROR: invalid number of arguments for ":cm"' printNl.
        ].
    ];
    add: ':PW'
      descr: 'write package to Smalltalk source file'
      handler: [ :args :fname :pkg |
        args size > 1 ifTrue: [
          System loadModule: 'pkgwrite'.
          pkg := Package find: (args at: 2) asSymbol.
          pkg ifNil: [ 'ERROR: invalid package name' printNl ]
          ifNotNil: [
            args size > 2 ifTrue: [ fname := args at: 3 ]
            ifFalse: [ fname := (pkg name asString) + '.st'. ].
            'writing package ' print. pkg name asString print. ' to ' print. fname printNl.
            (globals at: #PackageWriter) write: pkg name to: fname.
          ].
        ] ifFalse: [
          'ERROR: invalid number of arguments for ":PW"' printNl.
        ].
    ];
    add: ':dis'
      descr: 'disasm method; args: class method'
      handler: [ :args :cls |
        args size = 3 ifTrue: [
          System loadModule: 'disasm'.
          (args at: 2) print. '>>' print. (args at: 3) printNl.
          cls := globals at: ((args at: 2) asSymbol) ifAbsent: [ nil ].
          ((cls notNil) and: [ cls isKindOf: Class ]) ifTrue: [
            (args at: 3) firstChar == $^ ifTrue: [
              args at: 3 put: ((args at: 3) from: 2).
              cls := cls class.
            ].
            "cls disasmMethod: (args at: 3) asSymbol."
            (cls := cls allMethods at: (args at: 3) asSymbol ifAbsent: [ nil ]) ifNil: [ 'no such method' printNl ]
              ifNotNil: [ cls disassemble ].
          ].
        ] ifFalse: [
          'ERROR: invalid number of arguments for ":dis"' printNl.
        ].
    ];
    add: ':gc'
      descr: 'run GC'
      handler: [ :args :cls | System gc. ];
  .
  obj commandsSet: clicmd.
  ^obj
]

commands [
  ^commands
]

commandsSet: clist [
  commands := clist
]

checkForAssign: cmd [
  | res s |
  cmd := cmd break: (' \t').
  ((cmd size < 3) or: [ (cmd at: 2) ~= 'is' ] ) ifTrue: [ ^false ].
  (res := cmd at: 1) isEmpty ifTrue: [ self error: 'empty var name in "is"' ].
  cmd := cmd removeFirst removeFirst.
  s := ''. cmd do: [ :elem | s := s + elem + ' ' ].
  s := s doIt.
  globals at: res asSymbol put: s.
  res print. ' set to ' print. s printNl.
  ^true
]

REPL [
  "main execution loop"
  | cmd res s |
  [
    System isStdInTTY ifTrue: [ '-> ' print ].
    cmd := String input.
    cmd
   ] whileNotNil: [
    cmd isEmpty
      ifTrue: [ 'type ":h" to get some help or ":q" to quit' printNl ]
      ifFalse: [
        Case test: (cmd firstChar);
          case: $: do: [ commands do: cmd ];
          case: $! do: [
            "create new global var"
            cmd := (cmd from: 2) break: (' \t').
            ((cmd size < 3) or: [ (cmd at: 2) ~= 'is' ] ) ifTrue: [ self error: 'invalid "=" command' ].
            (res := cmd at: 1) isEmpty ifTrue: [ self error: 'empty var name in "="' ].
            cmd := cmd removeFirst removeFirst.
            s := ''. cmd do: [ :elem | s := s + elem + ' ' ].
            s := s doIt.
            globals at: res asSymbol put: s.
            ];
          else: [ :fc |
            (self checkForAssign: cmd) ifFalse: [
              res := cmd doIt.
              cmd removeTrailingBlanks lastChar == $. ifFalse: [ res ifNotNil: [ res printNl ]].
            ]
          ].
      ].
  ].
]
]

"now hook '^new' method so REPL EP will get our new object"
REPL extend [
^new [
  ^ExtREPL new
]
]
