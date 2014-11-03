Package [
  GUI
]


class: GuiSingleton  | widgetlist quitflag | [
  ^idAsString: elid [
    <#IUPDispatcher 250 elid>.
    ^nil
  ]

  ^message: msg title: title [
    <#IUPDispatcher 1 title msg>
  ]

  ^message: msg  [
    ^self message: msg title: 'LittleSmalltalk Message'
  ]

  ^canvas: action [
    <#IUPDispatcher 2 0 action>.
    self primitiveFailed
  ]

  ^button: label action: action [
    <#IUPDispatcher 2 1 action label>.
    self primitiveFailed
  ]

  ^toggle: label action: action [
    <#IUPDispatcher 2 2 action label>.
    self primitiveFailed
  ]

  ^edit: action [
    <#IUPDispatcher 2 3 action>.
    self primitiveFailed
  ]

  ^multiEdit: action [
    <#IUPDispatcher 2 4 action>.
    self primitiveFailed
  ]

  ^list: action [
    <#IUPDispatcher 2 5 action>.
    self primitiveFailed
  ]

  ^label: label [
    <#IUPDispatcher 2 6 label>.
    self primitiveFailed
  ]

  ^progressBar [
    <#IUPDispatcher 2 7>.
    self primitiveFailed
  ]

  ^spin [
    <#IUPDispatcher 2 8>.
    self primitiveFailed
  ]

  ^tree [
    <#IUPDispatcher 2 9>.
    self primitiveFailed
  ]

  ^frame: elid [
    <#IUPDispatcher 2 10 elid>.
    self primitiveFailed
  ]

  ^dialog: elid [
    <#IUPDispatcher 2 11 elid>.
    self primitiveFailed
  ]

  ^trackbar: aVertFlag [
    <#IUPDispatcher 2 12 aVertFlag>.
    self primitiveFailed
  ]

  ^fill [
    <#IUPDispatcher 3 0>.
    self primitiveFailed
  ]

  ^hbox [
    <#IUPDispatcher 3 1>.
    self primitiveFailed
  ]

  ^vbox [
    <#IUPDispatcher 3 2>.
    self primitiveFailed
  ]

  ^zbox [
    <#IUPDispatcher 3 3>.
    self primitiveFailed
  ]

  ^radiobox: elid [
    <#IUPDispatcher 3 4 elid>.
    self primitiveFailed
  ]

  ^cbox [
    <#IUPDispatcher 3 5>.
    self primitiveFailed
  ]

  ^sbox [
    <#IUPDispatcher 3 6>.
    self primitiveFailed
  ]

  ^normalizer [
    <#IUPDispatcher 3 7>.
    self primitiveFailed
  ]

  ^split: elid0 and: elid1 [
    <#IUPDispatcher 3 8 elid0 elid1>.
    self primitiveFailed
  ]

  ^split [
    <#IUPDispatcher 3 8>.
    self primitiveFailed
  ]

  ^attachTo: boxid element: elid [
    <#IUPDispatcher 4 boxid elid>.
    self primitiveFailed
  ]

  ^detach: elid [
    <#IUPDispatcher 5 elid>.
    self primitiveFailed
  ]

  ^destroy: elid [
    <#IUPDispatcher 6 elid>.
    self primitiveFailed
  ]

  ^show: elid [
    <#IUPDispatcher 7 elid>.
    self primitiveFailed
  ]

  ^registerEvent: aName [
    <#IUPDispatcher 8 aName>.
    self primitiveFailed
  ]

  ^unregisterEvent: aName [
    <#IUPDispatcher 8 aName false>.
    self primitiveFailed
  ]

  ^registerListEvent: aName [
    <#IUPDispatcher 9 aName>.
    self primitiveFailed
  ]

  ^unregisterListEvent: aName [
    <#IUPDispatcher 9 aName false>.
    self primitiveFailed
  ]

  ^getEvent [
    <#IUPDispatcher 10>.
    self primitiveFailed
  ]

  ^attribute: elid name: aName value: value [
    <#IUPDispatcher 11 elid aName value>.
    self primitiveFailed
  ]

  ^attribute: elid name: aName [
    <#IUPDispatcher 12 elid aName>.
    self primitiveFailed
  ]

  ^removeAttribute: elid name: aName [
    <#IUPDispatcher 13 elid aName>.
    self primitiveFailed
  ]

  ^popup: elid [
    <#IUPDispatcher 14 elid>.
    self primitiveFailed
  ]

  ^show: elid [
    <#IUPDispatcher 15 elid>.
    self primitiveFailed
  ]

  ^hide: elid [
    <#IUPDispatcher 15 elid true>.
    self primitiveFailed
  ]

  ^add: aWC [
    widgetlist ifNil: [ widgetlist := List new ].
    widgetlist << aWC.
  ]

  ^remove: aWC [
    widgetlist ifNotNil: [ widgetlist remove: aWC ifAbsent: [ nil ]]
  ]

  ^setQuitFlag [
    quitflag := true.
  ]

  ^passEvent: aId withData: aData [
    widgetlist ifNotNil: [ widgetlist do: [:w | (w passEvent: aId withData: aData) ifTrue: [ ^true ]]].
    ^false
  ]

  ^passEvent: aId [
    widgetlist ifNotNil: [ widgetlist do: [:w | (w passEvent: aId withData: nil) ifTrue: [ ^true ]]].
    ^false
  ]

  ^mainLoop [
    | evt ii |
    quitflag := false.
    [ quitflag ] whileFalse: [
      System eventWaitFor: 1100.
      evt := self getEvent.
      (evt position: ' ') ifNil: [ self passEvent: evt ]
      ifNotNil: [
        ii := evt break: ' '.
        evt := ii first.
        ii removeFirst.
        self passEvent: evt withData: ii
      ]
    ].
  ]
]
