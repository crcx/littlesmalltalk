Package [
  GUI
]


GuiWidget subclass: GuiTree [
  ^new [
    | obj |
    obj := self newWithId: (GuiSingleton tree).
    ^obj
  ]
]
