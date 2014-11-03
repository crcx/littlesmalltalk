Package [
  X11
]


X11BaseWindow subclass: X11Button [
  | text hovered wasPress enabled normback hoverback selected |

  ^new: parent at: xy size: sz text: aText [
    | obj |
    obj := self new: parent at: xy size: sz bgColor: (X11Color new: 0 g: 0 b: 0.7).
    self;
      in: obj var: #text put: aText;
      in: obj var: #selected put: false.
    obj; fontColor: 1.0 g: 1.0 b: 1.0; show.
    ^obj
  ]

  ^new: parent at: xy text: aText [
    | obj gw gh font |
    obj := self new: parent at: xy size: 1 @ 1 text: aText.
    font := obj font.
    gw := font textWidth: aText.
    gh := (font textHeight: aText) max: (font textHeight: 'qgldy').
    obj resize: (gw + 12) @ (gh + 8).
    ^obj
  ]

  text [
    ^text
  ]

  text: aText [
    text := aText.
    self isActive ifTrue: [ self redraw ].
  ]

  loveKbdFocus [
    ^true
  ]

  init [
    super init.
    normback := X11Color new: 0.0 g: 0.0 b: 0.7.
    hoverback := X11Color new: 0.0 g: 0.0 b: 0.9.
  ]

  onClick [
    ^self action
  ]
  onClick: aBlock [
    ^self action: aBlock
  ]
  clicked [
    ^self doAction
  ]

  selected [
    ^selected
  ]
  selected: aFlag [
    "'button selected: ' print. aFlag printNl."
    aFlag = selected ifFalse: [
      selected := aFlag.
      self redraw.
    ]
  ]

  redraw [
    | tpt rc tw |
    self fgColor: (hovered ifNotNil: [ hoverback ] ifNil: [ normback ]).
    rc := (self clientSizeRect) growX: 1 y: 1.
    self fillRect: rc.
    tw := font textWidth: text.
    tpt := ((self width - tw) / 2) @ 4.
    wasPress ifNotNil: [ tpt := tpt moveX: 1 y: 1 ].
    self drawTextAt: tpt text: text.
    selected ifTrue: [
      gc lineStyle: gc lineOnOffDash.
      rc := (self clientRect) growX: -2 y: -2.
      self fgColor: (X11Color new: 0.4 g: 0.4 b: 0.4).
      self drawRect: rc.
      gc lineStyle: gc lineSolid.
    ].
  ]

  keyPressEvent: evt [
    "'keycode: ' print. (evt at: 8) print. '; keyname: [' print. (evt at: 9) print. ']' printNl."
    "'key: ' print. evt printNl."
    (evt at: 9) == $  ifTrue: [ self clicked. ^true ].
    ^false
  ]

  onX11EnterNotify: evt [
    super onX11EnterNotify: evt.
    hovered := true.
    ^self redraw.
  ]

  onX11LeaveNotify: evt [
    super onX11LeaveNotify: evt.
    hovered := wasPress := nil.
    ^self redraw.
  ]

  onX11ButtonPress: evt [
    super onX11ButtonPress: evt.
    (evt at: 8) = 1 ifTrue: [
      wasPress := true.
      self focusMe.
      self redraw.
    ].
  ]

  onX11ButtonRelease: evt [
    super onX11ButtonRelease: evt.
    ((wasPress notNil) and: [ (evt at: 8) = 1 ]) ifTrue: [
      wasPress := nil.
      self redraw.
      self clicked.
    ].
  ]
]
