Package [
  GUI
]


GuiWidget subclass: GuiLabel [
  ^new: label [
    | obj |
    obj := self newWithId: (GuiSingleton label: label).
    ^obj
  ]
]
