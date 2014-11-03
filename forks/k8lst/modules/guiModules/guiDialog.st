Package [
  GUI
]


GuiRestrictedGroup subclass: GuiDialog [
  ^new: aTitle widget: aWidget onClose: aBlock [
    | obj |
    obj := self newWithId: (GuiSingleton dialog: aWidget id).
    self;
      in: obj var: #widgetlist put: (List with: aWidget);
      in: obj var: #ablock put: aBlock.
    obj attribute: 'TITLE' value: aTitle asString.
    obj attribute: 'CLOSE_CB' value: obj idAsString.
    ^obj
  ]

  ^new: aText widget: aWidget [
    ^self new: aText widget: aWidget onClose: nil
  ]

  mainWidget [
    ^widgetlist first
  ]
]
