class: Test [
  | count |

  nextAlarm: secs [
    'wait ' print. secs print. ' seconds.' printNl.
    <#NextAlarm secs>.
    self primitiveFailed
  ]

  alarm [
    [ count > 0 ] whileTrue: [
      'ALARM' printNl.
      self nextAlarm: (count * 2).
      System eventWaitFor: 1.
      count := count - 1.
    ].
    'ALARM complete' printNl.
    System userBreakSignal.
  ]

  run [
    count := 2.
    self alarm.
  ]
]


{ [ :shed |
    System isWindows "true" ifTrue: [
      'windoze eats shit!' printNl.
    ] ifFalse: [
      System newProcessGroupWith: (Process newWithMethod: #run class: Test new).
      ^REPL new REPL.
    ].
  ] value
}
