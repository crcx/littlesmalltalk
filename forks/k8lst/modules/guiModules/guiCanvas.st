Package [
  GUI
]


GuiWidget subclass: GuiCanvas [
  ^new: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton canvas: true).
    self in: obj var: #ablock put: aBlock.
    ^obj
  ]
  "onAction: redraw"
]
