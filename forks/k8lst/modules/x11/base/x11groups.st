Package [
  X11
]


X11BaseWindow subclass: X11RestrictedGroup [
  | widgetlist kbdselected |

  init [
    super init.
    widgetlist := List new.
    kbdselected := nil.
  ]

  includes: aWC [
    ^widgetlist includes: aWC.
  ]

  findWidgetById: aWid [
    widgetlist do: [:w | (X11Singleton sameWids: aWid and: w wid) ifTrue: [ ^w ]].
    ^nil
  ]

  passEvent: aWid event: evt [
    "returns true if event handler found"
    | w |
    (super passEvent: aWid event: evt) ifTrue: [ ^true ].
    (w := self findWidgetById: aWid) ifNotNil: [
      (evt at: 1) == #KeyPress ifTrue: [
        (w keyPressEvent: evt) ifTrue: [ ^true ].
        ^self keyPressEvent: evt.
      ] ifFalse: [
        w onEvent: evt. ^true
      ]
    ].
    ^false
  ]

  markAsDead [
    super markAsDead.
    widgetlist do: [:w | w markAsDead ].
    widgetlist removeAll.
    kbdselected := nil.
  ]

  childVisibility: aChild vis: aVisFlag [
    self isActive ifFalse: [ ^self ].
    aVisFlag ifFalse: [
      aChild ifNil: [ ^self ].
      (aChild sameWid: kbdselected) ifTrue: [
        kbdselected selected: false.
        kbdselected := nil.
      ].
    ].
  ]

  fixKbdFocus [
    self isActive ifFalse: [ ^self ].
    kbdselected ifNotNil: [
      (kbdselected visible and: [ kbdselected loveKbdFocus ]) ifFalse: [ kbdselected := nil ].
    ].
    kbdselected ifNil: [
      widgetlist do: [:w |
        (w visible and: [ w loveKbdFocus ]) ifTrue: [
          "'focusing' printNl."
          X11Singleton flush.
          w focusMe.
          ^self childFocusing: w focused: true
        ].
      ].
    ].
    kbdselected ifNotNil: [ kbdselected focusMe ].
    ^self childFocusing: kbdselected focused: true
  ]

  focusNextChild [
    self isActive ifFalse: [ ^self ].
    kbdselected ifNotNil: [ kbdselected := widgetlist next: kbdselected ].
    kbdselected ifNil: [ kbdselected := widgetlist first ].
    [ kbdselected ] whileNotNil: [
      (kbdselected visible and: [ kbdselected loveKbdFocus ]) ifTrue: [
        kbdselected focusMe.
        ^self
      ].
      kbdselected := widgetlist next: kbdselected.
    ].
    self changeKbdFocusTo: nil.
  ]

  focusPrevChild [
    self isActive ifFalse: [ ^self ].
    kbdselected ifNotNil: [ kbdselected := widgetlist prev: kbdselected ].
    kbdselected ifNil: [ kbdselected := widgetlist last ].
    [ kbdselected ] whileNotNil: [
      (kbdselected visible and: [ kbdselected loveKbdFocus ]) ifTrue: [
        kbdselected focusMe.
        ^self
      ].
      kbdselected := widgetlist prev: kbdselected.
    ].
    self changeKbdFocusTo: nil.
  ]

  keyPressEvent: evt [
    Case test: (evt at: 8);
      case: 113 do: [ self focusPrevChild. ^true ];
      case: 114 do: [ self focusNextChild. ^true ].
    ^false
  ]

  changeKbdFocusTo: aChild [
    ^self childFocusing: aChild focused: true
  ]

  childFocusing: aChild focused: aFFlag [
    self isActive ifFalse: [ ^self ].
    aChild ifNil: [ aFFlag := false ].
    "'child: ' print. aChild print. '; flag: ' print. aFFlag printNl."
    aFFlag ifTrue: [
      "something was focused"
      (aChild visible and: [ aChild loveKbdFocus ]) ifFalse: [
        kbdselected ifNotNil: [ kbdselected focusMe ].
      ] ifTrue: [
        aChild = kbdselected ifFalse: [ kbdselected ifNotNil: [ kbdselected selected: false ]].
        (kbdselected := aChild) selected: true.
      ].
    ] ifFalse: [
      "something was defocused"
      aChild ifNotNil: [ aChild selected: false ].
      kbdselected ifNotNil: [ kbdselected = aChild ifTrue: [ kbdselected := nil ]].
    ].
  ]
]


X11RestrictedGroup subclass: X11Group [
  isGroup [
    ^true
  ]

  add: aWC [
    widgetlist << aWC.
  ]

  remove: aWC [
    widgetlist remove: aWC ifAbsent: [ nil ]
  ]

  removeAll [
    widgetlist removeAll
  ]
]
