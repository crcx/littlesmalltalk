Requires [ simulation ]


class: Customer [
  numberOfScoops [
    | num |
    num := SmallInt atRandom % 3 + 1.
    'customer has ' print. num print. ' scoop' print.
    num > 1 ifTrue: [ 's' print ].
    '.' printNl.
    ^num
  ]
]


Simulation subclass: IceCreamStore [
  | profit |

  ^new [
    | obj |
    obj := super new.
    self in: obj var: #profit put: 0.0.
    obj sheduleArrival.
    ^obj
  ]

  sheduleArrival [
    self addEvent: Customer new at: (self time + (SmallInt atRandom % 5 + 1))
  ]

  reportProfits [
    'profits are ' print. profit print. '.' printNl.
  ]

  processEvent: event [
    'customer received at ' print. self time print. '.' printNl.
    profit := profit + (event numberOfScoops asFloat * 0.17).
    self sheduleArrival
  ]
]


{
  | store |
  store := IceCreamStore new.
  [ store time < 15 ] whileTrue: [ store proceed ].
  store reportProfits.
}
