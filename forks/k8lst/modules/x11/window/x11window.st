Package [
  X11
]


X11Group subclass: X11Window [
  | title bOnExpose bOnShow bOnHide bOnButtonDown bOnButtonUp bOnClose |

  'instance creation'
  ^new: parent at: xy size: sz title: aTitle [
    | obj |
    obj := self new: parent at: xy size: sz bgColor: (X11Color black).
    obj title: aTitle.
    ^obj
  ]

  'init/deinit'
  init [
    super init.
    X11Singleton add: self.
  ]

  deinit [
    dead ifFalse: [
      super deinit.
      bOnClose ifNotNil: [ bOnClose value: self ].
    ]
  ]

  'WM'
  withdraw [
    dead ifTrue: [ ^self ].
    X11Singleton XWithdrawWindow: wid
  ]

  iconify [
    dead ifTrue: [ ^self ].
    X11Singleton XIconifyWindow: wid
  ]

  urgency: aFlag [
    dead ifTrue: [ ^self ].
    X11Singleton XUrgency: wid urgent: aFlag
  ]

  'handlers'
  onClose [
    ^bOnClose
  ]
  onClose: aBlock [
    bOnClose := aBlock
  ]

  onExpose [
    ^bOnExpose
  ]
  onExpose: aBlock [
    bOnExpose := aBlock
  ]

  onShow [
    ^bOnShow
  ]
  onShow: aBlock [
    bOnShow := aBlock
  ]

  onHide [
    ^bOnShow
  ]
  onHide: aBlock [
    bOnShow := aBlock
  ]

  onButtonDown [
    ^bOnButtonDown
  ]
  onButtonDown: aBlock [
    bOnButtonDown := aBlock
  ]

  onButtonUp [
    ^bOnButtonUp
  ]
  onButtonUp: aBlock [
    bOnButtonUp := aBlock
  ]

  'X11 event handlers'
  onX11Expose: evt [
    super onX11Expose: evt.
    bOnExpose ifNotNil: [ bOnExpose value: self value: (evt at: 7) = 0 value: evt ].
  ]

  onX11MapNotify: evt [
    super onX11MapNotify: evt.
    bOnShow ifNotNil: [ bOnShow value: self value: evt ].
  ]

  onX11UnmapNotify: evt [
    super onX11UnmapNotify: evt.
    bOnHide ifNotNil: [ bOnHide value: self value: evt ].
  ]

  onX11ButtonPress: evt [
    super onX11ButtonPress: evt.
    bOnButtonDown ifNotNil: [ bOnButtonDown value: self value: (evt at: 8) value: evt ].
  ]

  onX11ButtonRelease: evt [
    super onX11ButtonRelease: evt.
    bOnButtonUp ifNotNil: [ bOnButtonUp value: self value: (evt at: 8) value: evt ].
  ]

  resized [
    "'resized from: ' print. owdt print. 'x' print. ohgt print.
    ' to: ' print. self width print. 'x' print. self height printNl."
  ]
]
