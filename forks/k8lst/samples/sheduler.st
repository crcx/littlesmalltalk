"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
class: ShedulerProcList [
| procs curProc |

  current [
    curProc ifNil: [ ^nil ].
    ^curProc value
  ]

  add: aProc [
    aProc ifNotNil: [
      procs := Link value: aProc next: procs.
      curProc ifNil: [ curProc := procs ].
    ].
  ]

  includes: aProc [
    | link |
    link := procs.
    [ link isNil ] whileFalse: [
      link value == aProc ifTrue: [ ^true ].
      link := link next
    ].
    ^false
  ]

  remove: aProc [
    | link prev |
    link := procs.
    prev := nil.
    [ link isNil ] whileFalse: [
      link value == aProc ifTrue: [
        prev ifNil: [ procs := procs next ] ifNotNil: [ prev next: link next ].
        curProc value == aProc ifTrue: [
          (curProc := link next) ifNil: [ curProc := procs ].
        ].
        ^true
      ].
      prev := link.
      link := link next
    ].
    ^false
  ]

  sheduleNext [
    curProc ifNotNil: [ curProc := curProc next ].
    curProc ifNil: [ curProc := procs ].
    ^curProc
  ]
]


class: Sheduler [
| procs abort slice |

  ^new: aSlice [
    | obj |
    obj := self basicNew.
    self in: obj at: 1 put: (ShedulerProcList new).
    self in: obj at: 2 put: false.
    obj slice: aSlice.
    ^obj
  ]

  ^new [
    ^self new: 100
  ]

  slice [
    ^slice
  ]

  slice: aTicks [
    aTicks < 0 ifTrue: [ slice := 1 ] ifFalse: [ slice := aTicks + 1 ]
  ]

  newProcess: aRunner [
    | proc ctx args |
    (args := Array new: 1) at: 1 put: aRunner.
    ctx := Context new.
    ctx setup: (aRunner findMethod: #run) withArguments: args.
    proc := Process new.
    proc context: ctx.
    [ procs add: proc ] runLocked.  "so processes are alowed to do this as atomic action"
    ^true
  ]

  removeProcess: aProc [
    [ procs remove: aProc ] runLocked  "so processes are alowed to do this as atomic action"
  ]

  abortAll [
    abort := true
  ]

  run [
    | ret cp |
    [ abort or: [ (cp := procs current) isNil ] ] whileFalse:
    [ ret := cp doExecute: slice.
      "ret print. ' is res' printNl."
      ret = 5 ifFalse: [
        "process complete or aborted"
        ret = 7 ifFalse: [
          "not yielded"
          ret = 4 ifFalse: [ cp errorReport: ret ].
          procs remove: cp.
        ].
      ] ifTrue: [
        "time quantum expired"
        procs sheduleNext.
      ].
    ].
    'sheduler complete' printNl.
  ]
]


class: Runner [
  | limit num |

  ^new: aLimit num: aNum [
    | obj |
    obj := self new.
    self in: obj var: #limit put: aLimit.
    self in: obj var: #num put: aNum.
    ^obj
  ]

  run [
    ('starting process ' + num asString + '\n') print.
    1 to: limit do: [ :f | ('p' + num asString + ': ' + f asString + '\n') print ].
    ('process ' + num asString + ' complete\n') print.
  ]
]


{ [ :shed |
    shed := Sheduler new.
    shed newProcess: (Runner new: 160 num: 0).
    shed newProcess: (Runner new: 300 num: 1).
    System isWindows "true" ifTrue: [
      'windoze eats shit!' printNl.
      shed run.
    ] ifFalse: [
      System newProcessGroupWith: (Process newWithMethod: #run class: shed).
      ^REPL new REPL.
    ].
  ] value
}
