Requires [ simulation ]


class: DiscrepeProbability [
  | weights max |

  defineWeights: anArray [
    weights := anArray.
    max := anArray inject: 0 into: [:i :e | i + e ].
  ]

  next [
    | index value |
    value := SmallInt atRandom % max + 1.
    index := 1.
    [ value > (weights at: index) ]
      whileTrue: [
        value := value - (weights at: index).
        index := index + 1.
      ].
    ^index
  ]
]


class: Customer [
  | groupSize |

  ^new [
    | obj |
    obj := self basicNew.
    self in: obj var: #groupSize put: (SmallInt atRandom % 8) + 1.
    ^obj
  ]

  groupSize [
    ^groupSize
  ]
]


Simulation subclass: IceCreamStore [
  | profit scoopDistribution |

  ^new [
    | obj |
    obj := super new.
    self;
      in: obj var: #profit put: 0.0;
      in: obj var: #scoopDistribution put:
        ((DiscrepeProbability new); defineWeights: #(65 25 10)).
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
    profit := profit + ((self scoopsFor: event groupSize) asFloat * 0.17).
    self sheduleArrival
  ]

  scoopsFor: group [
    | num |
    num := 0.
    group timesRepeat: [ num := num + scoopDistribution next ].
    'group of ' print. group print. ' have ' print. num print. ' scoop' print.
    num > 1 ifTrue: [ 's' print ].
    '.' printNl.
    ^num
  ]
]


{
  | store |
  store := IceCreamStore new.
  [ store time < 15 ] whileTrue: [ store proceed ].
  store reportProfits.
}
