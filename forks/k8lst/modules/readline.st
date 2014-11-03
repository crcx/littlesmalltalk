Requires [ termkey ]

System extend [
  ^readStdInCharAsSmallInt [
    System eventWaitFor: 1001.
    <#ReadCharacter>.
    self primitiveFailed
  ]
]


String extend [
  ^input [
    "read a line from input"
    "TODO: use stringbuffer?"
    | value c pos vn |
    System flushStdOut.
    System; termRaw: true.
    value := String new: 256. pos := 1.
    [ (c := Char input)
      ifNil: [
        pos = 1 ifTrue: [ value := nil ] ifFalse: [ value := value from: 1 to: pos ].
        System; termRaw: false.
        ^value
      ].
      c isEOL ] whileFalse: [
      c value = 127 ifTrue: [
        "backspace"
        pos > 1 ifTrue: [
          '\b \b' print.
          pos := pos - 1.
        ].
      ] ifFalse: [
        "c value printNl."
        c value < 32 ifTrue: [
          c value = 3 ifTrue: [
            '^C' printNl.
            System userBreakSignal.
          ].
        ] ifFalse: [
          c print.
          pos > value size ifTrue: [
            "extend buffer"
            vn := String new: (value size) + 256.
            1 to: value size do: [:i | vn at: i put: (value at: i)].
            value := vn. vn := nil.
          ].
          value at: pos put: c.
          pos := pos + 1.
        ].
      ].
      System flushStdOut.
    ].
    '\n' print.
    System flushStdOut.
    System; termRaw: false.
    ^value from: 1 to: pos - 1.
  ]
]
