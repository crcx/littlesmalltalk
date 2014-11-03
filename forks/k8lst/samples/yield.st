class: Calculator [
| val |

^newWith: aVal [
  | obj |
  obj := self new.
  self in: obj var: #val put: aVal.
  ^obj
]

run [
  [ val > 0 ] whileTrue: [
    System yield: val.
    val := val - 1.
  ].
]
]


class: Generator [
| proc |

^newWith: aClass [
  | obj ctx proc |
  ctx := Context new.
  ctx setup: (aClass findMethod: #run) withArguments: (Array with: aClass).
  proc := Process new.
  proc context: ctx.
  obj := self new.
  self in: obj var: #proc put: proc.
  ^obj
]

next: aDoneBlock [
  | res |
  res := proc doExecute: 0.
  res = 4 ifTrue: [ ^aDoneBlock value ].
  res = 7 ifTrue: [ ^proc result ].
  proc errorReport: res.
  ^aDoneBlock value.
]

printAll [
  | res |
  [ true ] whileTrue: [
    res := self next: [ 'done.' printNl. ^true ].
    res printNl.
  ].
]
]


{ (Generator newWith: (Calculator newWith: 4)) printAll printNl. }
