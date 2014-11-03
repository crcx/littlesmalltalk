Package [
  GUI
]


GuiWidget subclass: GuiButton [
  ^new: aText action: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton button: aText action: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]

  ^new: aText [
    ^self new: aText action: nil
  ]
]
