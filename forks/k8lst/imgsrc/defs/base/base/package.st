COMMENTS
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
ENDCOMMENTS


Object subclass: Package packages current | classes name version requires

METHODS FOR Package
^initialize [
  packages ifNil: [ packages := Dictionary new ]
]

"classes name version requires"
^new: aName version: aVersion [
  | obj |
  obj := self new.
  self in: obj var: #classes put: (Dictionary new).
  self in: obj var: #name put: (aName asSymbol).
  self in: obj var: #version put: aVersion.
  self in: obj var: #requires put: (Dictionary new).
  ^obj
]

^packages [
  ^packages
]

^find: aName [
  ^packages at: (aName asSymbol) ifAbsent: [ ^nil ]
]

^current [
  ^current
]

^current: aName version: version [
  "change default package; return true if this package is new"
  | isNew |
  version class == SmallInt ifFalse: [ self error: 'invalid package version type' ].
  isNew := false.
  aName ifNil: [
    current := packages at: #User
  ] ifNotNil: [
    ((aName class == String) or: [ aName class == Symbol ]) ifFalse: [ self error: 'invalid package name type' ].
    aName := aName asSymbol.
    (packages includes: aName) ifFalse: [
      version < 0 ifTrue: [ version := 1 ].
      current := Package new: aName version: version.
      packages at: aName put: current.
      isNew := true.
    ] ifTrue: [
      current := packages at: aName.
      version > 0 ifTrue: [
        current version = version ifFalse: [ self error: 'invalid package "' + aName asString + '" version' ].
      ].
    ].
  ].
  ^isNew
]

^current: aName [
  ^self current: aName version: 0
]

^setCurrent: cur [
  current := cur.
  ^cur
]

^addToCurrent: aClass [
  current ifNotNil: [
    aClass package: current.
    current classes at: aClass getName put: aClass.
  ]
]

classes [
  ^classes
]

name [
  ^name asString
]

version [
  ^version
]

requires [
  ^requires
]
!
