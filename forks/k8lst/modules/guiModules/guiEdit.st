Package [
  GUI
]


GuiWidgedWithBlockOnly subclass: GuiEdit [
  ^new: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton edit: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]

  value [
    ^self attribute: 'VALUE'
  ]

  value: aText [
    ^self attribute: 'VALUE' value: aText asString
  ]

  empty [
    self value: ''
  ]

  onAction [
    ablock ifNotNil: [ ablock value: self value ]
  ]
]


GuiEdit subclass: GuiMultiEdit [
  ^new: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton multiEdit: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]
]
