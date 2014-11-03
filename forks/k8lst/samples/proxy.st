class: TestClass [
  | id |

  ^new: aId [
    | obj |
    obj := self new.
    self in: obj var: #id put: aId.
    ^obj
  ]

  run [
    id printNl.
    'TestClass>>run' printNl.
    self class print.
    '>>' print.
    thisContext method name printNl.
  ]
]

proxy: TestProxy [
  | obj cnt |

  ^new [
    | obj |
    '10' printNl.
    (obj := self basicNew) initialize.
    '11' printNl.
    ^obj
  ]

  initialize [
    cnt := 42.
  ]

  doesNotUnderstand: aSel args: aArgs [
    '!!!!' printNl.
    obj printNl.
    aSel printNl.
    "aArgs printNl."
    '00' printNl.
    obj ifNil: [ obj := TestClass new: cnt. cnt := cnt + 1 ].
    '01' printNl.
    obj printNl.
    ^obj perform: aSel withArguments: (aArgs from: 2).
  ]
]


{[:px |
   px := TestProxy new.
   px run.
   px run.
 ] value
}
