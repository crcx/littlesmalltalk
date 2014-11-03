Package [
  GUI
]


GuiGroup subclass: GuiBox [
  add: aWid [
    aWid; detach; attachTo: self.
    ^super add: aWid
  ]

  remove: aWid [
    (self includes: aWid) ifTrue: [
      aWid detach.
      super remove: aWid.
    ]
  ]
]

GuiBox subclass: GuiHBox [
  ^new [
    ^self newWithId: (GuiSingleton hbox).
  ]
]

GuiBox subclass: GuiVBox [
  ^new [
    ^self newWithId: (GuiSingleton vbox).
  ]
]

GuiBox subclass: GuiZBox [
  ^new [
    ^self newWithId: (GuiSingleton vbox).
  ]
]

GuiBox subclass: GuiCBox [
  ^new [
    ^self newWithId: (GuiSingleton cbox).
  ]
]

GuiBox subclass: GuiSBox [
  ^new [
    ^self newWithId: (GuiSingleton sbox).
  ]
]

GuiBox subclass: GuiNormBox [
  ^new [
    ^self newWithId: (GuiSingleton normalizer).
  ]
]


GuiBox subclass: GuiSplit [
  ^new [
    ^self newWithId: (GuiSingleton split).
  ]

  ^new: aWid0 and: aWid1 [
    | obj |
    aWid0 attribute: 'EXPAND' value: 'YES'.
    aWid1 attribute: 'EXPAND' value: 'YES'.
    obj := self newWithId: (GuiSingleton split: aWid0 id and: aWid1 id).
    self in: obj var: #widgetlist put: (List with: aWid0 with: aWid1).
    ^obj
  ]

  ^new: aWid0 [
    ^(self newWithId: GuiSingleton split) add: aWid0.
  ]

  add: aWid [
    aWid attribute: 'EXPAND' value: 'YES'.
    ^super add: aWid
  ]
]


GuiSplit subclass: GuiVSplit [
]

GuiSplit subclass: GuiHSplit [
  ^new [
    ^(super new); attribute: 'ORIENTATION' value: 'HORIZONTAL'
  ]

  ^new: aWid0 and: aWid1 [
    ^(super new: aWid0 and: aWid1); attribute: 'ORIENTATION' value: 'HORIZONTAL'
  ]
]


GuiRestrictedGroup subclass: GuiRadioBox [
  ^new: aWid [
    | obj |
    obj := self newWithId: (GuiSingleton radiobox: aWid id).
    self in: obj var: #widgetlist put: (List with: aWid).
    ^obj
  ]

  mainWidget [
    ^widgetlist first
  ]
]


GuiGroup subclass: GuiFrame [
  ^new: aWid [
    | obj |
    obj := self newWithId: (GuiSingleton frame: aWid id).
    self in: obj var: #widgetlist put: (List with: aWid).
    ^obj
  ]

  mainWidget [
    ^widgetlist first
  ]
]
