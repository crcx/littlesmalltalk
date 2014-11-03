Package [
  GUI
]


GuiWidget subclass: GuiToggle [
  ^new: aText action: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton toggle: aText action: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]

  ^new: aText [
    ^self new: aText action: nil
  ]

  checked [
    ^self attribute: 'VALUE' = 'YES'
  ]

  checked: aFlag [
    ^self attribute: 'VALUE' value: (aFlag ifTrue: ['YES'] ifFalse: ['NO'])
  ]
]
