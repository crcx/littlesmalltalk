Package [
  GUI
]


GuiWidget subclass: GuiProgressBar [
  ^new [
    | obj |
    obj := self newWithId: (GuiSingleton progressBar).
    ^obj
  ]
]
