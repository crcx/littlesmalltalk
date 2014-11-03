Package [
  GUI
]


GuiWidget subclass: GuiRestrictedGroup [
  | widgetlist |

  includes: aWC [
    widgetlist ifNotNil: [ ^widgetlist includes: aWC ].
    ^false
  ]

  findWidgetById: aId [
    widgetlist ifNotNil: [ widgetlist do: [:w | w idAsString = aId ifTrue: [ ^w ]]].
    ^nil
  ]

  passEvent: aId withData: aData [
    "returns true if event handler found"
    | w |
    (super passEvent: aId withData: aData) ifTrue: [ ^true ].
    (w := self findWidgetById: aId) ifNotNil: [ w onAction: aData. ^true ].
    widgetlist ifNotNil: [ widgetlist do: [:w | (w passEvent: aId withData: aData) ifTrue: [ ^true ]]].
    ^false
  ]
]


GuiRestrictedGroup subclass: GuiGroup [
  add: aWC [
    widgetlist ifNil: [ widgetlist := List new ].
    widgetlist << aWC.
  ]

  remove: aWC [
    widgetlist ifNotNil: [ widgetlist remove: aWC ifAbsent: [ nil ]]
  ]

  removeAll [
    widgetlist ifNotNil: [
      [ widgetlist size > 0 ] whileTrue: [ self remove: widgetlist first ].
    ]
  ]
]
