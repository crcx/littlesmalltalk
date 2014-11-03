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
  ]

  ^runDummy [
    1 to: 50000 do: [ nil ].
  ]
]


{
  FinTest new.
  FinTest new.
  FinTest new.
  FinTest new.
  System gc.
  System newProcessGroupWith: (Process newWithMethod: #runDummy class: FinTest class).
  'running REPL' printNl.
  ^REPL new REPL.
}
