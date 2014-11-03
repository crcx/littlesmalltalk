Package [
  X11
]


X11ResourceObject subclass: X11Font | fbag fset | [
  | font fname |

  ^register: aName [
    | fnt |
    ((fbag ifNil: [ fbag := Bag new ]) includes: aName)
      ifTrue: [
        fbag add: aName.
        ^(fset at: aName) value
      ].
    fset ifNil: [ fset := Set new ].
    "('new font ' + aName) printNl."
    fset add: (Association key: aName value: (fnt := X11Singleton XftFontOpen: aName)).
    fbag add: aName.
    ^fnt
  ]

  ^unregister: aName [
    "('unregistering font ' + aName) printNl."
    (fbag remove: aName) = 0 ifTrue: [
      "('closing font ' + aName) printNl."
      X11Singleton XftFontClose: (fset at: aName) value.
      fset remove: aName.
    ].
  ]

  ^new [
    ^self new: X11Singleton defaultFont.
  ]

  ^new: aName [
    | obj |
    obj := super new.
    self;
      in: obj at: 1 put: (self register: aName);
      in: obj at: 2 put: aName.
    ^obj
  ]

  free [
    font ifNotNil: [ X11Font unregister: fname. font := nil ].
    fname := nil.
  ]

  font [
    ^font
  ]

  name [
    ^fname
  ]

  textExtents: aText [
    ^X11Singleton XftTextExtents: font text: aText.
  ]

  textWidth: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 1.
  ]

  textHeight: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 2.
  ]

  textX: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 3.
  ]

  textY: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 4.
  ]

  textXOfs: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 5.
  ]

  textYOfs: aText [
    ^X11Singleton XftTextExtents: font text: aText item: 6.
  ]
]


X11ResourceObject subclass: X11FontDraw [
  | draw |

  ^new [
    ^self error: 'X11FontDraw can''t be created with #new'.
  ]

  ^new: wid [
    | obj |
    obj := super new.
    self in: obj at: 1 put: (X11Singleton XftDrawCreate: wid).
    ^obj
  ]

  free [
    draw ifNotNil: [ X11Singleton XftDrawDestroy: draw. draw := nil ].
  ]

  draw [
    ^draw
  ]
]


X11ResourceObject subclass: X11FontColor [
  | color |

  ^new [
    ^self error: 'X11FontColor can''t be created with #new'.
  ]

  ^new: r g: g b: b [
    | obj |
    obj := super new.
    self in: obj at: 1 put: (X11Singleton XftNewColor: r g: g b: b).
    ^obj
  ]

  free [
    color ifNotNil: [ X11Singleton XftFreeColor: color. color := nil ].
  ]

  color [
    ^color
  ]
]
