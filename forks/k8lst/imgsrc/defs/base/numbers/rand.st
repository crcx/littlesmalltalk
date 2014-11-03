Object subclass: Random  | seed
METHODS FOR Random
^new [
  | obj |
  obj := self basicNew.
  obj seed: System tickCountMS.
  ^obj
]

^new: aSeed [
  | obj |
  obj := self basicNew.
  aSeed isFloat ifTrue: [ aSeed := (aSeed trunc > 0 ifTrue: [ aSeed ] ifFalse: [ aSeed * 1073741823 ]) trunc ].
  obj seed: aSeed.
  ^obj
]

seed [
  ^seed
]

seed: aSeed [
  seed := aSeed bitAnd: 1073741823
]

nextSmallInt [
  seed := (1103515245 * seed + 12345) bitAnd: 1073741823.  "1073741823 = 0x3fffffff"
  ^seed
]

next [
  "random float between 0 and 1"
  ^self nextSmallInt asFloat / 1073741824
]

between: low and: high [
  ^((self nextSmallInt asFloat / 1073741823) * (high - low + 1)) trunc + low
]
!
