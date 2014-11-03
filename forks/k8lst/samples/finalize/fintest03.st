class: FinTestItem [
  | id |

  ^new: anId [
    | obj |
    obj := self new.
    self in: obj at: 1 put: anId.
    obj addToBeFinalized.
    ^obj
  ]

  finalize [
    'finalizing FinTestItem ' print. id printNl.
  ]
]

class: FinTest | cid first total | [
  | id fti |

  ^new [
    | obj |
    total ifNil: [ total := 0 ].
    obj := self basicNew.
    cid ifNil: [ cid := 0. first := obj. ].
    self in: obj at: 1 put: (cid := cid + 1).
    self in: obj at: 2 put: (FinTestItem new: cid).
    'adding ' print. cid print. ' to st-finalize list' printNl.
    obj addToBeFinalized.
    total := total + 1.
    ^obj
  ]

  ^oneIsDead [
    (total := total - 1) = 1 ifTrue: [
      'only one object left; killing it' printNl.
      first := nil.
    ].
    System gc.
  ]

  finalize [
    'finalizing ' print. id printNl.
    self class oneIsDead.
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
"
  System newProcessGroupWith: (Process newWithMethod: #runDummy class: FinTest class).
  'running REPL' printNl.
  ^REPL new REPL.
"
  "[ true ] whileTrue: [ nil ]."
  System eventWaitFor: 1001.
}
