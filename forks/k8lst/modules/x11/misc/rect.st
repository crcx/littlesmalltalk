Package [
  X11
]


class: Rectangle [
  | x y width height |

  ^newFrom: aRect [
    ^self newX: aRect x y: aRect y width: aRect width height: aRect height.
  ]

  ^newFrom: aXY size: aSize [
    ^self newX: aXY x y: aXY y width: aSize x height: aSize y.
  ]

  ^newFrom: aXY0 to: aXY1 [
    | x y w h |
    (w := (aXY1 x) - (aXY0 x) - 1) < 0
      ifTrue: [ x := aXY1 x. w := w negated ]
      ifFalse: [ x := aXY0 ].
    (h := (aXY1 y) - (aXY0 y) - 1) < 0
      ifTrue: [ y := aXY1. h := h negated ]
      ifFalse: [ y := aXY0 y ].
    ^self newX: x y: y width: w height: h.
  ]

  ^newX: aX y: aY width: aWdt height: aHgt [
    | obj |
    obj := self new.
    self;
      in: obj at: 1 put: aX;
      in: obj at: 2 put: aY;
      in: obj at: 3 put: aWdt;
      in: obj at: 4 put: aHgt.
    ^obj
  ]

  ^newX0: x0 y0: y0 x1: x1 y1: y1 [
    ^self newFrom: x0 @ y0 to: x1 @ y1
  ]

  x [
    ^x
  ]
  y [
    ^y
  ]
  width [
    ^width
  ]
  height [
    ^height
  ]
  x0 [
    ^x
  ]
  y0 [
    ^y
  ]
  x1 [
    ^x+width-1
  ]
  y1 [
    ^y+height-1
  ]

  xy [
    ^Point newX: x y: y
  ]
  size [
    ^Point newX: width y: height
  ]
  x0y0 [
    ^Point newX: x y: y
  ]
  x1y1 [
    ^Point newX: x+width-1 y: y+height-1
  ]

  moveX: aXD [
    ^Rectangle newX: x+aXD y: y width: width height: height
  ]

  moveY: aYD [
    ^Rectangle newX: x y: y+aYD width: width height: height
  ]

  moveX: aXD y: aYD [
    ^Rectangle newX: x+aXD y: y+aYD width: width height: height
  ]

  growX: aXG [
    ^Rectangle newX: x-aXG y: y width: width+(aXG*2) height: height
  ]

  growY: aYG [
    ^Rectangle newX: x y: y-aYG width: width height: height+(aYG*2)
  ]

  growX: aXG y: aYG [
    ^Rectangle newX: x-aXG y: y-aYG width: width+(aXG*2) height: height+(aYG*2)
  ]

  + aPoR [
    | axy1 xy1 |
    (aPoR isKindOf: Point) ifTrue: [ ^self moveX: aPoR x y: aPoR y ].
    "rect union"
    axy1 := aPoR x1y1.
    xy1 := self x1y1.
    ^Rectangle
      newFrom: (x min: aPoR x) @ (y min: aPoR y)
      to: (xy1 x max: axy1 x) @ (xy1 y max: axy1 y)
  ]

  printString [
    ^'(' + x printString + '@' + y printString + '; ' +
     width printString + 'x' + height printString + ')'.
  ]
]


Point extend [
  @ aXY1 [
    ^Rectangle newFrom: self to: aXY1
  ]
]
