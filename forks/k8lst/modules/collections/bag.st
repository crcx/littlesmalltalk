Requires [ collections/set collections/association ]

Package [
  System
]


Collection subclass: Bag [
  | contents size |

  ^new [
    | obj |
    obj := self basicNew.
    self;
      in: obj at: 1 put: Set new;
      in: obj at: 2 put: 0.
    ^obj
  ]

  size [
    ^size
  ]

  removeAll [
    contents := Set new.
    size := 0.
  ]

  add: anElement [
    | a |
    a := contents at: anElement ifAbsent: [ a := Association key: anElement value: 0 ].
    a value: (a value + 1).
    contents add: a.
    size := size + 1.
  ]

  << anElement [
    ^self add: anElement
  ]

  add: anElement withOccurrences: aCount [
    | a |
    aCount > 0 ifTrue: [
      a := contents at: anElement ifAbsent: [ a := Association key: anElement value: 0 ].
      a value: (a value + aCount).
      contents add: a.
      size := size + aCount.
    ].
    ^anElement
  ]

  remove: anElement ifAbsent: aBlock [
    "return count"
    | a res |
    a := contents at: anElement ifAbsent: [ ^aBlock value ].
    (a value = 1)
      ifTrue: [ contents remove: a. res := 0 ]
      ifFalse: [ a value: (res := a value - 1). contents add: a ].
    size := size - 1.
    ^res
  ]

  remove: anElement [
    ^self remove: anElement ifAbsent: [ self error: 'out of item in bag' ]
  ]

  includes: anElement [
    ^contents includes: anElement
  ]

  occurrencesOf: anElement [
    ^(contents at: anElement ifAbsent: [ ^0 ]) value
  ]

  do: aBlock [
    "thru associations"
    contents do: aBlock
  ]

  itemsDo: aBlock [
    "thru items"
    contents do: [:a | aBlock value: a key ]
  ]
]
