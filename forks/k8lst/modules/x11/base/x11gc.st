Package [
  X11
]


X11ResourceObject subclass: X11Color [
  | color |

  ^new [
    ^self error: 'X11Color can''t be created with #new'.
  ]

  ^new: r g: g b: b [
    | obj |
    obj := super new.
    self in: obj at: 1 put: (X11Singleton XNewColor: r g: g b: b).
    ^obj
  ]

  ^black [
    ^self new: 0 g: 0 b: 0.
  ]

  ^white [
    ^self new: 1.0 g: 1.0 b: 1.0.
  ]

  free [
    color ifNotNil: [
      "'free color: ' print. color printNl."
      X11Singleton XFreeColor: color. color := nil
    ]
  ]

  color [
    ^color
  ]
]


X11ResourceObject subclass: X11GC [
  | gc fgcolor bgcolor linestyle capstyle joinstyle width |

  ^new [
    ^self error: 'X11GC can''t be created with #new'.
  ]

  ^new: wid [
    | obj |
    obj := super new.
    self;
      in: obj at: 1 put: (X11Singleton XCreateGC: wid);
      in: obj at: 2 put: X11Color white;
      in: obj at: 3 put: X11Color black;
      in: obj at: 4 put: 0;
      in: obj at: 5 put: 1;
      in: obj at: 6 put: 0;
      in: obj at: 7 put: 0.
    obj realizeLineStyle.
    ^obj
  ]

  ^new: wid fgColor: aClr [
    | obj |
    obj := super new.
    self;
      in: obj at: 1 put: (X11Singleton XCreateGC: wid fgColor: aClr color);
      in: obj at: 2 put: aClr;
      in: obj at: 3 put: X11Color black.
    ^obj
  ]

  free [
    gc ifNotNil: [ X11Singleton XFreeGC: gc. gc := nil ]
  ]

  gc [
    ^gc
  ]

  fgColor: aClr [
    fgcolor := aClr.
    X11Singleton fgColor: gc color: aClr color.
  ]

  bgColor: aClr [
    bgcolor := aClr.
    X11Singleton bgColor: gc color: aClr color.
  ]

  realizeLineStyle [
    X11Singleton XSetLineAttr: gc
      width: width
      line: linestyle
      cap: capstyle
      join: joinstyle
  ]

  width [
    ^width
  ]
  width: aValue [
    width := aValue.
    ^self realizeLineStyle
  ]

  lineStyle [
    ^linestyle
  ]
  lineStyle: aValue [
    linestyle := aValue.
    ^self realizeLineStyle
  ]
  lineSolid [
    ^0
  ]
  lineDoubleDash [
    ^1
  ]
  lineOnOffDash [
    ^2
  ]
]
