Package [
  X11
]


class: X11BaseWindow [
  | wid gc font fcolor fdraw ablock visible
    bgcolor title parent dead
    owdt ohgt
  |

  ^new [
    ^self error: 'X11BaseWindow instances must be created with special methods'.
  ]

  ^newWithId: wid [
    | obj |
    obj := self basicNew.
    self in: obj at: 1 put: wid.
    self in: obj var: #dead put: false.
    obj internalInit.
    ^obj
  ]

  ^new: aParent at: xy size: sz bgColor: clr [
    | wid obj |
    wid := X11Singleton XCreateSimpleWindow: (aParent ifNotNil: [ aParent wid ])
      x: xy x y: xy y
      width: sz x height: sz y
      bgcolor: clr color.
    obj := self newWithId: wid.
    obj; bgColor: clr; backgroundColor: clr.
    self in: obj var: #bgcolor put: clr.
    self in: obj var: #parent put: aParent.
    aParent ifNotNil: [ aParent isGroup ifTrue: [ aParent add: obj ]].
    ^obj
  ]

  isGroup [
    ^false
  ]

  loveKbdFocus [
    ^false
  ]

  parent [
    ^parent
  ]

  internalInit [
    gc := X11GC new: wid.
    font := X11Font new.
    fcolor := X11FontColor new: 1.0 g: 1.0 b: 1.0.
    fdraw := X11FontDraw new: wid.
    X11Singleton XSelectInput: wid interestFlag: 1.
    visible := false.
    title := ''.
    self addToBeFinalized.
    self init.
  ]

  init [
  ]

  deinit [
    dead ifFalse: [
      self hide.
      dead := true.
      fdraw ifNotNil: [ fdraw free ].
      fcolor ifNotNil: [ fcolor free ].
      font ifNotNil: [ font free ].
      bgcolor ifNotNil: [ bgcolor free ].
      gc ifNotNil: [ gc free ].
      title := parent := nil.
      self markAsDead.
      X11Singleton XDestroyWindow: wid.
    ]
  ]

  finalize [
    self close.
    fdraw := fcolor := font := gc := wid := bgcolor := title := parent := nil.
  ]

  dead [
    ^dead
  ]

  close [
    dead ifFalse: [ self deinit ].
  ]

  wid [
    ^wid
  ]

  gc [
    ^gc
  ]

  title [
    ^title
  ]

  title: aTitle [
    dead ifTrue: [ ^self ].
    title := aTitle ifNil: [ '' ].
    X11Singleton XSetTitle: wid title: title.
  ]

  'show/hide'
  visible [
    ^visible
  ]

  visible: aVis [
    aVis ifTrue: [ ^self show ].
    ^self hide
  ]

  show [
    visible ifFalse: [
      dead ifFalse: [ X11Singleton XMapWindow: wid ].
    ]
  ]

  hide [
    visible ifTrue: [
      dead ifFalse: [ X11Singleton XUnmapWindow: wid ].
    ]
  ]

  isActive [
    ^dead not and: [ visible ].
  ]

  'colors'
  backgroundColor: clr [
    "window background; not GC"
    dead ifTrue: [ ^self ].
    bgcolor := clr.
    X11Singleton XSetWindowBackground: wid color: clr color
  ]

  fgColor: aClr [
    dead ifTrue: [ ^self ].
    gc fgColor: aClr.
  ]

  bgColor: aClr [
    dead ifTrue: [ ^self ].
    gc bgColor: aClr.
  ]

  fgColor: r g: g b: b [
    self fgColor: (X11Color new: r g: g b: b)
  ]

  bgColor: r g: g b: b [
    self bgColor: (X11Color new: r g: g b: b)
  ]

  'event handling'
  sameWid: aWid [
    aWid ifNil: [ ^false ].
    (aWid isKindOf: X11BaseWindow) ifTrue: [ aWid := aWid wid ].
    ^(X11Singleton sameWids: wid and: aWid)
  ]

  = aWid [
    ^self sameWid: aWid
  ]

  passEvent: aWid event: evt [
    "return true if event handler found"
    (X11Singleton sameWids: wid and: aWid) ifTrue: [
      (evt at: 1) == #KeyPress ifTrue: [ ^self keyPressEvent: evt ].
      self onEvent: evt.
      ^true
    ].
    ^false
  ]

  onEvent: evt [
    | mt |
    (mt := self class findMethodInAll: ('onX11' + (evt at: 1) asString + ':') asSymbol ifAbsent: [nil]) ifNotNil: [
      "'perform: ' print. mt name printNl."
      ^self perform: mt withArgument: evt
    ].
  ]

  focusMe [
    self isActive ifTrue: [
      self focused ifFalse: [ X11Singleton XSetInputFocus: wid ].
    ].
  ]

  focused [
    dead ifTrue: [ ^false ].
    visible ifFalse: [ ^false ].
    ^self sameWid: (X11Singleton XGetInputFocus)
  ]

  keyPressEvent: evt [
    ^true  "eat all keys"
  ]

  action: aBlock [
    ablock := aBlock
  ]

  action [
    ^ablock
  ]

  doAction [
    ablock ifNotNil: [ ablock value: self ]
  ]

  'geometry'
  move: aPoint [
    dead ifTrue: [ ^self ].
    X11Singleton XMoveWindow: wid x: aPoint x y: aPoint y
  ]

  resize: aPoint [
    dead ifTrue: [ ^self ].
    X11Singleton XResizeWindow: wid width: aPoint x height: aPoint y
  ]

  move: aPoint resize: aSize [
    dead ifTrue: [ ^self ].
    X11Singleton XMoveResizeWindow: wid x: aPoint x y: aPoint y width: aSize x height: aSize y
  ]

  position [
    | x y |
    dead ifTrue: [ ^0 @ 0 ].
    x := X11Singleton XGetGeometry: wid item: 1.
    y := X11Singleton XGetGeometry: wid item: 2.
    ^x @ y
  ]

  dimensions [
    | w h |
    dead ifTrue: [ ^0 @ 0 ].
    w := X11Singleton XGetGeometry: wid item: 3.
    h := X11Singleton XGetGeometry: wid item: 4.
    ^w @ h
  ]

  maxXY [
    | w h |
    dead ifTrue: [ ^0 @ 0 ].
    w := X11Singleton XGetGeometry: wid item: 3.
    h := X11Singleton XGetGeometry: wid item: 4.
    ^(w-1) @ (h-1)
  ]

  x [
    dead ifTrue: [ ^0 ].
    ^X11Singleton XGetGeometry: wid item: 1.
  ]

  y [
    dead ifTrue: [ ^0 ].
    ^X11Singleton XGetGeometry: wid item: 2.
  ]

  width [
    dead ifTrue: [ ^0 ].
    ^X11Singleton XGetGeometry: wid item: 3.
  ]

  height [
    dead ifTrue: [ ^0 ].
    ^X11Singleton XGetGeometry: wid item: 4.
  ]

  clientSizeRect [
    ^Rectangle newFrom: 0 @ 0 size: self dimensions
  ]

  clientRect [
    ^Rectangle newFrom: 0 @ 0 size: self maxXY
  ]

  'drawing'
  clear [
    dead ifTrue: [ ^self ].
    X11Singleton XClearWindow: wid
  ]

  drawPoint: p0 [
    dead ifTrue: [ ^self ].
    X11Singleton XDrawPoint: wid gc: gc gc x: p0 x y: p0 y.
  ]

  drawLine: p0 to: p1 [
    dead ifTrue: [ ^self ].
    X11Singleton XDrawLine: wid gc: gc gc x0: p0 x y0: p0 y x1: p1 x y1: p1 y.
  ]

  drawRect: aRect [
    dead ifTrue: [ ^self ].
    X11Singleton XDrawRect: wid gc: gc gc x: aRect x y: aRect y width: aRect width height: aRect height.
  ]

  fillRect: aRect [
    dead ifTrue: [ ^self ].
    X11Singleton XFillRect: wid gc: gc gc x: aRect x y: aRect y width: aRect width height: aRect height.
  ]

  'text'
  font [
    ^font
  ]

  font: aName [
    dead ifTrue: [ ^self ].
    font := X11Font new: aName
  ]

  fontColor: aClr [
    dead ifTrue: [ ^self ].
    fcolor := aClr
  ]

  fontColor: r g: g b: b [
    dead ifTrue: [ ^self ].
    fcolor := X11FontColor new: r g: g b: b.
  ]

  putTextAt: p0 text: text [
    dead ifTrue: [ ^self ].
    X11Singleton XftFontDraw: fdraw draw
      color: fcolor color
      font: font font
      x: p0 x y: p0 y
      text: text.
  ]

  drawTextAt: p0 text: text [
    | gy |
    dead ifTrue: [ ^self ].
    gy := font textY: 'ylgqft'.
    X11Singleton XftFontDraw: fdraw draw
      color: fcolor color
      font: font font
      x: p0 x y: p0 y + gy
      text: text.
  ]

  markAsDead [
    "'-' print. self print. ' ' print. dead printNl.
    dead ifFalse: [ 'died: ' print. (X11Singleton wid2str: wid) printNl ]."
    dead ifFalse: [ self deinit. dead := true ]
  ]

  childVisibility: aChild vis: aVisFlag [
  ]

  childFocusing: aChild focused: aFFlag [
  ]

  selected [
    ^false
  ]
  selected: aFlag [
  ]

  resized [
  ]

  'drawing'
  draw: aFull [
    aFull ifTrue: [ self redraw ].
  ]

  redraw [
  ]

  'keyboard focus'
  fixKbdFocus [
  ]

  'X11 event handling'
  onX11MotionNotify: evt [
  ]

  onX11ButtonPress: evt [
  ]

  onX11ButtonRelease: evt [
  ]

  onX11EnterNotify: evt [
  ]

  onX11LeaveNotify: evt [
  ]

  onX11Expose: evt [
    self fixKbdFocus.
    self draw: (evt at: 7) = 0
  ]

  onX11FocusIn: evt [
    parent ifNil: [ ^self selected: true ].
    parent childFocusing: self focused: true.
  ]

  onX11FocusOut: evt [
    parent ifNil: [ ^self selected: false ].
    parent childFocusing: self focused: false.
  ]

  onX11KeymapNotify: evt [
  ]

  onX11KeyPress: evt [
  ]

  onX11KeyRelease: evt [
  ]

  onX11MapNotify: evt [
    visible := true.
    parent ifNotNil: [
      parent childVisibility: self vis: true.
      parent fixKbdFocus.
    ].
  ]

  onX11UnmapNotify: evt [
    visible := false.
    parent ifNotNil: [
      parent childVisibility: self vis: true.
      parent fixKbdFocus.
    ].
  ]

  onX11DestroyNotify: evt [
    "'dying: ' print. (X11Singleton wid2str: wid) printNl."
    self markAsDead.
  ]

  onX11ConfigureNotify: evt [
    owdt ifNil: [ owdt := 0 ].
    ohgt ifNil: [ ohgt := 0 ].
    ((evt at: 5) = owdt and: [ (evt at: 6) = ohgt ]) ifFalse: [ self resized ].
    owdt := evt at: 5.
    ohgt := evt at: 6.
  ]

  onX11WMCloseRequest: evt [
    self close
  ]
]
