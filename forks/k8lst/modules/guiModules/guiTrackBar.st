Package [
  GUI
]


GuiWidgedWithBlockOnly subclass: GuiTrackBar [
  ^new: aBlock vertical: aVertFlag [
    | obj |
    obj := self newWithId: (GuiSingleton trackbar: aVertFlag).
    self in: obj var: #ablock put: aBlock.
    obj attribute: 'VALUECHANGED_CB' value: obj idAsString.
    ^obj
  ]

  asNum: s [
    | n |
    (n := s asNumber) ifNil: [ n := s asFloat ].
    ^n
  ]

  min [
    ^self asNum: (self attribute: 'MIN')
  ]

  max [
    ^self asNum: (self attribute: 'MAX')
  ]

  step [
    ^self asNum: (self attribute: 'STEP')
  ]

  pageStep [
    ^self asNum: (self attribute: 'PAGESTEP')
  ]

  value [
    ^self asNum: (self attribute: 'VALUE')
  ]

  min: aVal [
    self attribute: 'MIN' value: aVal asString
  ]

  max: aVal [
    self attribute: 'MAX' value: aVal asString
  ]

  step: aVal [
    self attribute: 'STEP' value: aVal asString
  ]

  pageStep: aVal [
    self attribute: 'PAGESTEP' value: aVal asString
  ]

  value: aVal [
    self attribute: 'VALUE' value: aVal asString
  ]

  onAction [
    ablock ifNotNil: [ ablock value: self value ]
  ]
]

GuiTrackBar subclass: GuiHTrackBar [
  ^new: aBlock [
    ^self new: aBlock vertical: false
  ]
]

GuiTrackBar subclass: GuiVTrackBar [
  ^new: aBlock [
    ^self new: aBlock vertical: true
  ]
]
