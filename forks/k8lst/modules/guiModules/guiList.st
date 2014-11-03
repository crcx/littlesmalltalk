Package [
  GUI
]


GuiWidgedWithBlockOnly subclass: GuiList [
  ^new: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton list: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]

  size [
    ^(self attribute: 'COUNT') asNumber
  ]

  at: n put: aItem [
    self attribute: n asString value: aItem asString
  ]

  at: n [
    ^self attribute: n asString
  ]

  empty [
    self removeAttribute: '1'
  ]

  selected [
    | n |
    (n := self attribute: 'VALUE') ifNil: [ ^nil ].
    (n := n asNumber) ifNil: [ ^nil ].
    n > 0 ifFalse: [ ^nil ].
    ^n
  ]

  addLast: aItem [
    self at: self size + 1 put: aItem
  ]

  << aItem [
    self addLast: aItem
  ]

  onAction: aData [
    ablock ifNotNil: [ ablock value: (aData at: 1) asNumber value: (aData at: 2) = '1' ]
  ]
]
