Package [
  X11
]


class: X11Singleton  | widgetlist quitflag defaultFont dumpEvents | [
  ^dumpEvents [
    ^dumpEvents
  ]
  ^dumpEvents: aFlag [
    dumpEvents := aFlag
  ]

  ^subtypeName: aX11 [
    <#X11Do 0 aX11>.
    ^nil
  ]

  ^wid2str: wid [
    <#X11Do 1 wid>.
    self primitiveFailed
  ]

  ^sameWids: w0 and: w1 [
    <#X11Do 2 w0 w1>.
    self primitiveFailed
  ]

  ^flush [
    <#X11Do 8>.
    self primitiveFailed
  ]

  ^initialize [
    defaultFont ifNil: [ defaultFont := 'verdana,sans-10' ].
    dumpEvents ifNil: [ dumpEvents := false ].
    widgetlist := List new.
    self XInitialize: nil.
  ]

  ^deinitialize [
    widgetlist := nil.
    ^self XDeinitialize
  ]

  ^defaultFont [
    ^defaultFont
  ]

  ^defaultFont: aName [
    defaultFont := aName ifNil: [ 'verdana,sans-10' ].
  ]

  ^blackPixel [
    <#X11Do 5 0>.
    self primitiveFailed
  ]

  ^whitePixel [
    <#X11Do 5 1>.
    self primitiveFailed
  ]

  ^allPlanes [
    <#X11Do 5 2>.
    self primitiveFailed
  ]

  ^connectionFd [
    <#X11Do 5 3>.
    self primitiveFailed
  ]

  ^depth [
    <#X11Do 5 4>.
    self primitiveFailed
  ]

  ^planes [
    <#X11Do 5 5>.
    self primitiveFailed
  ]

  ^name [
    <#X11Do 5 6>.
    self primitiveFailed
  ]

  ^vendor [
    <#X11Do 5 7>.
    self primitiveFailed
  ]

  ^release [
    <#X11Do 5 8>.
    self primitiveFailed
  ]

  ^width [
    <#X11Do 5 9>.
    self primitiveFailed
  ]

  ^height [
    <#X11Do 5 10>.
    self primitiveFailed
  ]


  ^XInitialize: aName [
    <#X11Do 10 aName>.
    self primitiveFailed
  ]

  ^XDeinitialize [
    <#X11Do 11>.
    self primitiveFailed
  ]

  ^getEvent [
    <#X11Do 14>.
    ^nil
  ]

  ^XSelectInput: aWindow interestFlag: aFlag [
    <#X11Do 15 aWindow aFlag>.
    self primitiveFailed
  ]

  ^XCreateSimpleWindow: parentWid x: x y: y width: wdt height: hgt bgcolor: clr [
    <#X11Do 20 parentWid x y wdt hgt clr>.
    self primitiveFailed
  ]

  ^XDestroyWindow: wid [
    <#X11Do 21 wid>.
    self primitiveFailed
  ]

  ^XSetTitle: wid title: aTitle [
    <#X11Do 22 wid aTitle>.
    self primitiveFailed
  ]

  ^XMapWindow: wid [
    <#X11Do 23 wid true>.
    self primitiveFailed
  ]

  ^XUnmapWindow: wid [
    <#X11Do 23 wid false>.
    self primitiveFailed
  ]

  ^XGetGeometry: wid item: aItem [
    <#X11Do 24 wid aItem>.
    self primitiveFailed
  ]

  ^XGetGeometry: wid [
    <#X11Do 24 wid 0>.
    self primitiveFailed
  ]

  ^XSetWindowBackground: wid color: clr [
    <#X11Do 25 wid clr>.
    self primitiveFailed
  ]

  ^XClearWindow: wid [
    <#X11Do 26 wid>.
    self primitiveFailed
  ]

  ^XMoveWindow: wid x: x y: y [
    <#X11Do 27 wid 1 x y>.
    self primitiveFailed
  ]

  ^XResizeWindow: wid width: w height: h [
    <#X11Do 27 wid 2 w h>.
    self primitiveFailed
  ]

  ^XMoveResizeWindow: wid x: x y: y width: w height: h [
    <#X11Do 27 wid 3 x y w h>.
    self primitiveFailed
  ]

  ^XSetInputFocus: wid [
    <#X11Do 28 wid>.
    self primitiveFailed
  ]

  ^XGetInputFocus [
    <#X11Do 28 true>.
    self primitiveFailed
  ]

  ^XWithdrawWindow: wid [
    <#X11Do 80 wid true>.
  ]

  ^XIconifyWindow: wid [
    <#X11Do 80 wid false>.
  ]

  ^XUrgency: wid urgent: aFlag [
    <#X11Do 81 wid 1 aFlag>.
  ]

  ^XCreateGC: wid fgColor: aColor [
    <#X11Do 30 wid aColor>.
    self primitiveFailed
  ]

  ^XCreateGC: wid [
    <#X11Do 30 wid>.
    self primitiveFailed
  ]

  ^XFreeGC: aGC [
    <#X11Do 31 aGC>.
    self primitiveFailed
  ]

  ^XNewColor: r g: g b: b [
    <#X11Do 32 r g b>.
    self primitiveFailed
  ]

  ^XFreeColor: aClr [
    <#X11Do 33 aClr>.
    self primitiveFailed
  ]

  ^XSetColor: aGC color: aClr backFlag: aBF [
    <#X11Do 34 aGC aClr aBF>.
    self primitiveFailed
  ]

  ^fgColor: aGC color: aClr [
    ^self XSetColor: aGC color: aClr backFlag: false
  ]

  ^bgColor: aGC color: aClr [
    ^self XSetColor: aGC color: aClr backFlag: true
  ]

  ^XSetLineAttr: aGC width: wdt line: line cap: cap join: join [
    <#X11Do 35 aGC wdt line cap join>.
    self primitiveFailed
  ]

  ^XDrawPoint: wid gc: gc x: x y: y [
    <#X11Do 40 wid gc x y>.
    self primitiveFailed
  ]

  ^XDrawLine: wid gc: gc x0: x0 y0: y0 x1: x1 y1: y1 [
    <#X11Do 41 wid gc x0 y0 x1 y1>.
    self primitiveFailed
  ]

  ^XDrawRect: wid gc: gc x: x y: y width: wdt height: hgt [
    <#X11Do 42 wid gc x y wdt hgt>.
    self primitiveFailed
  ]

  ^XFillRect: wid gc: gc x: x y: y width: wdt height: hgt [
    <#X11Do 43 wid gc x y wdt hgt>.
    self primitiveFailed
  ]

  ^XftFontOpen: aName [
    <#X11Do 50 aName>.
    self primitiveFailed
  ]

  ^XftFontClose: aFont [
    <#X11Do 51 aFont>.
    self primitiveFailed
  ]

  ^XftDrawCreate: wid [
    <#X11Do 52 wid>.
    self primitiveFailed
  ]

  ^XftDrawDestroy: aDraw [
    <#X11Do 53 aDraw>.
    self primitiveFailed
  ]

  ^XftNewColor: r g: g b: b [
    <#X11Do 54 r g b>.
    self primitiveFailed
  ]

  ^XftFreeColor: aClr [
    <#X11Do 55 aClr>.
    self primitiveFailed
  ]

  ^XftFontDraw: aDraw color: aClr font: aFont x: x y: y text: text [
    <#X11Do 56 aDraw aClr aFont x y text>.
    self primitiveFailed
  ]

  ^XftTextExtents: aFont text: text item: aNo [
    <#X11Do 57 aFont text aNo>.
    self primitiveFailed
  ]

  ^XftTextExtents: aFont text: text [
    <#X11Do 57 aFont text>.
    self primitiveFailed
  ]


  ^add: aWC [
    widgetlist << aWC.
  ]

  ^remove: aWC [
    widgetlist remove: aWC ifAbsent: [ nil ]
  ]

  ^removeById: aWid [
    widgetlist do: [:w |
      (X11Singleton sameWids: aWid and: w wid)
        ifTrue: [
          "'dead meat: ' print. (self wid2str: aWid) printNl."
          self remove: w.
          ^self
        ]
    ].
  ]

  ^findWidgetById: aWid [
    widgetlist do: [:w | (X11Singleton sameWids: aWid and: w wid) ifTrue: [ ^w ]].
    ^nil
  ]

  ^setQuitFlag [
    quitflag := true.
  ]

  ^resetQuitFlag [
    quitflag := false.
  ]

  ^passEvent: wid event: evt [
    widgetlist do: [:w | (w passEvent: wid event: evt) ifTrue: [ ^true ]].
    ^false
  ]

  ^eventLoop [
    | evt wid kill |
    quitflag := false.
    [ quitflag ] whileFalse: [
      System eventWaitFor: 1200.
      (evt := self getEvent) ifNotNil: [
        wid := evt at: 2.
        dumpEvents ifTrue: [
          'wid: ' print. (X11Singleton wid2str: wid) print. '; ' print. evt printNl.
        ].
        kill := (evt at: 1) == #DestroyNotify.
        self passEvent: wid event: evt.
        kill ifTrue: [
          self removeById: wid.
          widgetlist isEmpty ifTrue: [ quitflag := true ].
        ].
      ].
    ]
  ]
]


class: X11ResourceObject [
  ^new [
    | obj |
    (obj := self basicNew) addToBeFinalized.
    ^obj
  ]

  free [
  ]

  finalize [
    self free
  ]
]
