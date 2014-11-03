Package [
  X11
]


class: Point [
  | x y |

  ^newFrom: aPoint [
    ^self newX: aPoint x y: aPoint y.
  ]

  ^newX: aX y: aY [
    | obj |
    obj := self new.
    self; in: obj at: 1 put: aX; in: obj at: 2 put: aY.
    ^obj
  ]

  clone [
    ^Point newFrom: self
  ]

  x [
    ^x
  ]

  y [
    ^y
  ]

  moveX: aXD [
    ^Point newX: x+aXD y: y
  ]

  moveY: aYD [
    ^Point newX: x y: y+aYD
  ]

  moveX: aXD y: aYD [
    ^Point newX: x+aXD y: y+aYD
  ]

  + aPoint [
    ^self moveX: aPoint x y: aPoint y
  ]

  printString [
    ^x printString + '@' + y printString
  ]
]


SmallInt extend [
  @ aY [
    ^Point newX: self y: aY
  ]
]
