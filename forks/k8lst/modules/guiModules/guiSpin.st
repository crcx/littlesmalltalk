Package [
  GUI
]


GuiWidget subclass: GuiSpin [
  ^new [
    | obj |
    obj := self newWithId: (GuiSingleton spin).
    ^obj
  ]
]
