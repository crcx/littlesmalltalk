"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Package [
  FFI
]


class: FfiLibrary [
  | h name |

  ^loadLib: aName [
    aName := aName + System suffixForDLL.
    <#FFILoadLib aName>.
    self error: 'can''t load library '+aName
  ]

  ^closeLib: h [
    <#FFICloseLib h>.
  ]

  ^new [
    self error: 'FfiLibrary should be created with #new:'
  ]

  ^new: aName [
    | obj h |
    h := self loadLib: aName.
    obj := self basicNew.
    self;
      in: obj at: 1 put: h;
      in: obj at: 2 put: aName.
    obj addToBeFinalized.
    ^obj
  ]

  handle [
    ^h
  ]

  name [
    ^name
  ]

  finalize [
    h ifNotNil: [ self closeLib: h ].
    h := name := nil.
  ]
]


class: FfiValue [
  | func lib name |

  ^loadSym: aLib name: aName [
    <#FFIResolveName aLib aName>.
    self error: 'can''t load value '+aName
  ]

  ^new [
    self error: 'FfiValue should be created with #new:name:'
  ]

  ^new: aLib name: aName [
    | obj h |
    h := self loadSym: aLib handle name: aName.
    obj := self basicNew.
    self;
      in: obj at: 1 put: h;
      in: obj at: 2 put: aLib;
      in: obj at: 3 put: aName.
    obj addToBeFinalized.
    ^obj
  ]

  finalize [
    func := lib := name := nil.
  ]

  as: aType [
    <#FFIGetVal func aType>.
    self primitiveFailed.
  ]
]


class: FfiFunction [
  | func lib name |

  ^loadSym: aLib name: aName [
    <#FFIResolveName aLib aName>.
    self error: 'can''t load function '+aName
  ]

  ^new [
    self error: 'FfiFunction should be created with #new:name:'
  ]

  ^new: aLib name: aName [
    | obj h |
    h := self loadSym: aLib handle name: aName.
    obj := self basicNew.
    self;
      in: obj at: 1 put: h;
      in: obj at: 2 put: aLib;
      in: obj at: 3 put: aName.
    obj addToBeFinalized.
    ^obj
  ]

  finalize [
    func := lib := name := nil.
  ]

  call [
    <#FFICall func nil>.
    self primitiveFailed.
  ]

  retType: aType [
    <#FFICall func aType>.
    self primitiveFailed.
  ]
]

Requires [ ffi/fficall ]
