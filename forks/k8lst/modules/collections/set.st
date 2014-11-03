"
 Little Smalltalk, Version 5

 Copyright (C) 1987-2005 by Timothy A. Budd
 Copyright (C) 2007 by Charles R. Childers
 Copyright (C) 2005-2007 by Danny Reinhold
 Copyright (C) 2010 by Ketmar // Vampire Avalon

 ============================================================================
 This license applies to the virtual machine and to the initial image of
 the Little Smalltalk system and to all files in the Little Smalltalk
 packages except the files explicitly licensed with another license(s).
 ============================================================================
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the 'Software'), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
"
Package [
  System
]


Collection subclass: Set [
  | members growth |

  ^new: aSize [
    | ret |
    ret := super new.
    self in: ret at: 1 put: (Array new: aSize).
    self in: ret at: 2 put: (aSize < 1 ifTrue: [16] ifFalse: [aSize]).
    ^ret
  ]

  ^new [
    ^self new: 19
  ]

  ^with: objA [
    | ret |
    ret := self new.
    ret add: objA.
    ^ret
  ]

  ^with: objA with: objB [
    | ret |
    ret := self new.
    ret add: objA. ret add: objB.
    ^ret
  ]

  ^with: objA with: objB with: objC [
    | ret |
    ret := self new.
    ret add: objA. ret add: objB. ret add: objC.
    ^ret
  ]

  removeAll [
    members := Array new: growth.
  ]

  size [
    | tally |
    tally := 0.
    members do: [:elem | elem ifNotNil: [ tally := tally + 1 ]].
    ^tally
  ]

  grow [
    "recreate ourselves in place with a new, bigger storage"
    | old |
    old := members.
    members := Array new: (old size + growth).
    "reinsert each existing Set member"
    old do: [:elem | self add: elem]
  ]

  compare: t and: e [
    ^t = e
  ]

  location: elem [
    | pos start t |
    start := pos := (elem hash % members size) + 1.
    [ true ] whileTrue: [
      "return this position if we match, or have reached a nil slot"
      t := members at: pos.
      ((t isNil) or: [self compare: t and: elem]) ifTrue: [
        ^pos
      ].
      "advance to next slot, circularly"
      pos := pos + 1.
      (pos > members size) ifTrue: [
        pos := 1
      ].
      "return nil if we have scanned the whole Set"
      (pos = start) ifTrue: [ ^nil ]
    ]
  ]

  add: elem [
    | pos |
    "find the appropriate slot... if none, need to grow the Set"
    pos := self location: elem.
    pos ifNil: [
      self grow.
      ^self add: elem
    ].
    "if the slot is nil, this is a new entry which we put in place now.
     if it wasn't nil, we still restore it so that if it's an association, the value portion will be updated"
    members at: pos put: elem.
    ^elem
  ]

  << elem [
    ^self add: elem
  ]

  rehash: start [
    | pos elem |
    pos := start.
    [ true ] whileTrue: [
      "advance to next slot, ceasing when we reach our start"
      pos := pos + 1.
      (pos > members size) ifTrue: [ pos := 1 ].
      (pos = start) ifTrue: [ ^self ].
      "if we reach a nil slot, there are no further rehash worries"
      elem := members at: pos.
      elem ifNil: [ ^self ].
      "nil out the slot, and then reinsert the element"
      members at: pos put: nil.
      self add: elem
    ]
  ]

  remove: elem ifAbsent: aBlock [
    | pos |
    "if not found, return error"
    (members at: ((pos := self location: elem) ifNil: [ ^aBlock value ])) ifNil: [ ^aBlock value ].
    "remove our element from the Set"
    members at: pos put: nil.
    "rehash all that follow"
    self rehash: pos.
    ^elem
  ]

  remove: elem [
    ^self remove: elem ifAbsent: [ self noElement ]
  ]

  do: aBlock [
    members do: [:elem | elem ifNotNil: [ aBlock value: elem ]]
  ]

  indexOf: elem [
    ^self indexOf: elem ifAbsent: [ nil ]
  ]

  indexOf: elem ifAbsent: aBlock [
    | pos |
    (members at: ((pos := self location: elem) ifNil: [ ^aBlock value ])) ifNil: [ ^aBlock value ].
    ^pos
  ]

  at: elem ifAbsent: aBlock [
    ^(members at: ((self location: elem) ifNil: [ ^aBlock value ])) ifNil: [ ^aBlock value ].
  ]

]


Set subclass: IdentitySet [
  compare: t and: e [
    ^t == e
  ]
]
