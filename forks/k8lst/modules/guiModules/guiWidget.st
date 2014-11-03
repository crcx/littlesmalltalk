Package [
  GUI
]


class: GuiWidget [
  | id ablock |

  ^new [
    self error: 'GuiWidget instances must be created with special methods'.
  ]

  ^newWithId: aId [
    | obj |
    obj := self basicNew.
    self in: obj at: 1 put: aId.
    ^obj
  ]

  idAsString [
    ^GuiSingleton idAsString: id
  ]

  id [
    ^id
  ]

  attribute: aName value: value [
    GuiSingleton attribute: id name: aName value: value
  ]

  attribute: aName [
    ^GuiSingleton attribute: id name: aName
  ]

  removeAttribute: aName [
    GuiSingleton removeAttribute: id name: aName
  ]

  popup [
    GuiSingleton popup: id
  ]

  show [
    GuiSingleton show: id
  ]

  passEvent: aId withData: aData [
    "returns true if event handler found"
    self idAsString = aId ifTrue: [ self onAction: aData. ^true ].
    ^false
  ]

  detach [
    GuiSingleton detach: id
  ]

  attachTo: aWid [
    GuiSingleton attachTo: aWid id element: id
  ]

  attach: aWid [
    GuiSingleton attachTo: id element: aWid id
  ]

  boolAttrubute: aName [
    ^self attribute: aName = 'YES'
  ]

  boolAttrubute: aName value: aFlag [
    self attribute: aName value: (aFlag ifNil: ['NO'] ifNotNil: [ aFlag ifTrue: ['YES'] ifFalse: ['NO']])
  ]

  enabled [
    ^self boolAttrubute: 'ACTIVE'
  ]

  enabled: aFlag [
    self boolAttrubute: 'ACTIVE' value: aFlag
  ]

  notExpandable [
    self attribute: 'EXPAND' value: 'NO'
  ]

  expandable [
    self attribute: 'EXPAND' value: 'YES'
  ]

  expandableHorizontal [
    self attribute: 'EXPAND' value: 'HORIZONTAL'
  ]

  expandableVertical [
    self attribute: 'EXPAND' value: 'VERTICAL'
  ]

  onAction: aData [
    ^self onAction
  ]

  onAction [
    ablock ifNotNil: [ ablock value ]
  ]

  action: aBlock [
    ablock := aBlock
  ]

  action [
    ^ablock
  ]
]


GuiWidget subclass: GuiWidgedWithBlockOnly [
  ^new [
    ^self new: nil
  ]
]
