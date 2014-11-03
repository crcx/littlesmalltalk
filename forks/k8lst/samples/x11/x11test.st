Requires [ x11 ]


X11Window subclass: MyX11Window [
  | ox oy lineList urg |

  centerText: y text: aText [
    | x e |
    e := font textExtents: aText.
    y := y + (e at: 4).
    x := (self width - (e at: 1) + (e at: 3)) / 2.
    self drawTextAt: x @ y text: aText.
  ]

  resized [
    | x y allwdt |
    allwdt := -4.
    x := y := nil.
    widgetlist do: [:w |
      (w isKindOf: X11Button) ifTrue: [
        y ifNil: [ y := self height - w height - 4 ].
        allwdt := allwdt + 4 + w width.
      ].
    ].
    allwdt < 1 ifTrue: [ ^self ].
    x := (self width - allwdt) / 2.
    widgetlist do: [:w |
      (w isKindOf: X11Button) ifTrue: [
        w move: x @ y.
        x := x + w width + 4.
      ].
    ].
  ]

  addButton: aText action: aBlock [
    (X11Button new: self at: 0 @ 0 text: aText) onClick: aBlock.
  ]

  init [
    | btn |
    super init.

    self;
      addButton: 'quit' action: [ 'quit clicked!' printNl. self close ];
      addButton: 'button 1' action: [ 'button clicked!' printNl ];
      addButton: 'iconify' action: [ self iconify ];
      addButton: 'urgency' action: [:btn |
        urg ifNil: [ btn text: 'normal'. urg := true ] ifNotNil: [ btn text: 'urgency'. urg := nil ].
        self urgency: urg.
      ].

    self fontColor: 1.0 g: 1.0 b: 1.0.
    self onExpose: [:w :full  :rc |
      full ifTrue: [
        rc := (w clientSizeRect) growX: 1 y: 1.
        w;
         fgColor: 0.0 g: 0.1 b: 0;
         fillRect: rc;
         fgColor: 1.0 g: 0 b: 0;
         drawLine: 1 @ 1 to: 80 @ 60;
         fgColor: 0 g: 1.0 b: 0;
         drawLine: 1 @ 10 to: 80 @ 60;
         fgColor: 0 g: 0 b: 1.0;
         drawLine: 1 @ 20 to: 80 @ 60;
         fgColor: 0.5 g: 0 b: 0;
         drawLine: 1 @ 50 to: 80 @ 60;
         fgColor: 1.0 g: 1.0 b: 0.
        lineList ifNotNil: [ lineList do: [:i | w drawLine: (i at: 1) to: (i at: 2) ]].
        w centerText: 5 text: 'Xft works!'.
        w drawTextAt: 2 @ 2 text: 'drawText'.
      ].
    ].
    self onButtonDown: [:w :bt :evt  :x :y |
      x := evt at: 3.
      y := evt at: 4.
      bt = 3 ifTrue: [ ox := x. oy := y. lineList := List new. ].
      bt = 1 ifTrue: [
        ox ifNotNil: [
          lineList << (Array with: ox @ oy with: x @ y).
          w;
           fgColor: 1.0 g: 1.0 b: 0;
           drawLine: ox @ oy to: x @ y.
          ox := x. oy := y.
        ] ifNil: [
          ox := x. oy := y. lineList := List new.
        ]
      ].
    ].
  ]
]


{
  | w |
  "X11Singleton dumpEvents: true."
  X11Singleton initialize.
  w := MyX11Window new: nil at: 0 @ 0 size: 400 @ 110 title: 'LST X11 test'.
  w show.
  X11Singleton eventLoop.
  'quit' printNl.
  X11Singleton deinitialize.
  'done' printNl.
}
