class: FinTest | cid | [
  | id |

  ^new [
    | obj |
    cid ifNil: [ cid := 0 ].
    obj := self basicNew.
    self in: obj at: 1 put: (cid := cid + 1).
    'adding ' print. cid print. ' to st-finalize list' printNl.
    obj addToBeFinalized.
    ^obj
  ]

  finalize [
    'finalizing ' print. id printNl.
    (globals at: #weak) dump.
  ]

  ^runDummy [
    1 to: 50000 do: [ nil ].
  ]
]


class: WeakObj [
  | link num |

  ^new: aObj [
    | obj |
    (obj := self new) obj: aObj.
    obj makeWeak.
    ^obj
  ]

  obj: aObj [
    num := 666.1.
    link := aObj
  ]

  mourn [
    'mourn' printNl.
    self dump.
  ]

  dump [
    num print. ' ' print.
    (link ifNil: [ 'nullified' ] ifNotNil: [ 'weak' ]) printNl.
  ]
]


"Requires [ disasm ]"

{
  | wo |
"  WeakObj disasmMethod: #dump.
  WeakObj disasmMethod: #mourn."
  wo := WeakObj new: nil.
  globals at: #weak put: wo.
  wo obj: FinTest new.
  wo := nil.
  FinTest new.
  FinTest new.
  System gc.
  System newProcessGroupWith: (Process newWithMethod: #runDummy class: FinTest class).
  'running REPL' printNl.
  ^REPL new REPL.
}
