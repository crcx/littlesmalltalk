"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"

"TODO: cookies"
Package [
  HttpServer
]


String extend [
  htmlEscape [
    "encode '<>&'"
    | res |
    (self includes: '<>&') ifFalse: [ ^self ].
    res := StringBuffer new.
    self do: [:c |
      c == $< ifTrue: [ res << '&lt;' ]
      ifFalse: [
        c == $> ifTrue: [ res << '&gt;' ]
        ifFalse: [
          c == $& ifTrue: [ res << '&amp;' ]
          ifFalse: [ res << c ]]]].
    ^res asString
  ]
]


class: HTTPRequest [
| sock
  reqStr
  abortBlock
  "parsed request"
  reqMethod  "string: GET, POST, etc"
  reqHost    "string: host name"
  reqPath    "string: path"
  reqFile    "string: file name"
  reqHash    "string: text after '#'"
  reqVars    "dictionary: (nameSymbol stringValue)"
  reqFields  "header fields"
|

  ^new: s abortBlock: aBlock [
    | obj |
    obj := self new.
    obj abortBlock: aBlock.
    obj parseRequest: s.
    ^obj
  ]

  ^new: s [
    ^self new: s abortBlock: [:err | self error: err ]
  ]

  debugDump [
    'method: ' print. reqMethod print. '|' printNl.
    'host: ' print. reqHost print. '|' printNl.
    'path: ' print. reqPath print. '|' printNl.
    'file: ' print. reqFile print. '|' printNl.
    'hash: ' print. reqHash print. '|' printNl.
    'vars: ' print. reqVars print. '|' printNl.
    'fields: ' print. reqFields print. '|' printNl.
  ]

  abortBlock: aBlock [
    abortBlock := aBlock
  ]

  path [
    ^reqPath
  ]

  file [
    ^reqFile
  ]

  hash [
    ^reqHash
  ]

  host [
    ^reqHost
  ]

  method [
    ^reqMethod
  ]

  var: aName [
    ^reqVars at: aName asSymbol ifAbsent: [ nil ].
  ]

  var: aName ifAbsent: aBlock [
    ^reqVars at: aName asSymbol ifAbsent: [ aBlock value ].
  ]

  var: aName put: aValue [
    reqVars at: aName asSymbol put: aValue.
  ]

  reqStr: str [
    reqStr := str
  ]

  field: aName [
    ^reqFields at: aName ifAbsent: [ nil ]
  ]

  parseRequest [
    | lines req |
    lines := (reqStr removeTrailingBlanks break: '\n') asArray.
    lines size < 1 ifTrue: [ abortBlock value: 'empty header' ].
    lines transform: [:str | str removeTrailingBlanks ].
    (req := (lines at: 1) break: ' ') size < 3 ifTrue: [ abortBlock value: 'invalid request line' ].
    reqMethod := req at: 1.
    self parsePath: (req at: 2).
    self parseFields: lines.
    reqHost := reqFields at: 'host' ifAbsent: [ reqHost ].
  ]

  parseFields: lines [
    | s p n v |
    reqFields := Dictionary new.
    "TODO: lines starting with space is 'continuation'"
    2 to: lines size do: [:idx |
      (p := (s := lines at: idx) position: $:) ifNotNil: [
        n := (s from: 1 to: p - 1) transform: [:c | c lowerCase ].
        p := p + 1.
        [(p < s size) and: [ (s at: p) isBlank ]] whileTrue: [ p := p + 1 ].
        v := s from: p.
        reqFields at: n put: v.
        "n print. '|' print. v print. '|' printNl."
      ].
    ].
  ]

  parsePath: aPath [
    | p |
    reqHash := ''. reqHost := ''.
    reqVars := Dictionary new.
    (aPath from: 1 to: 7) = 'http://' ifTrue: [
      "we have a host here"
      (p := (aPath := aPath from: 8) indexOf: '/') ifNil: [
        reqHost := aPath
        aPath := '/'.
       ] ifNotNil: [
        reqHost := aPath from: 1 to: p - 1.
        aPath := aPath from: p.
       ].
    ].
    (p := aPath indexOf: '?') ifNil: [
        (p := aPath indexOf: '#') ifNil: [
          reqPath := aPath.
         ] ifNotNil: [
           reqPath := aPath from: 1 to: p - 1.
           reqHash := aPath from: p + 1.
         ].
     ] ifNotNil: [
       reqPath := aPath from: 1 to: p - 1.
       aPath := aPath from: p + 1.
       (p := aPath indexOf: '#') ifNotNil: [
         reqHash := aPath from: p + 1.
         aPath := aPath from: 1 to: p - 1.
        ].
        self parseVars: aPath.
     ].
     "
     [ 'rp: ' print. reqPath printNl.
       self normPath.
       'rp: ' print. reqPath print. '|' print. reqFile print. '|' printNl.
     ] runLocked.
     "
     self normPath.
  ]

  parseVars: aVars [
    (aVars break: '&') do: [:vv :p :n :v |
      (p := vv indexOf: '=') ifNil: [
        n := vv.
        v := true.
       ] ifNotNil: [
        n := (vv from: 1 to: p - 1) fromUrl.
        v := (vv from: p + 1) fromUrl.
       ].
      self var: n put: v.
    ].
  ]

  normPath [
    | p res |
    reqPath isEmpty ifTrue: [ reqPath := '/'. reqFile := ''. ^self ].
    reqPath lastChar == $/
      ifTrue: [
        reqFile := ''.
      ] ifFalse: [
        (p := reqPath lastPosition: $/) ifNil: [ reqFile := reqPath. reqPath := '/'. ^self ].
        reqFile := reqPath from: p+1.
        reqPath := reqPath from: 1 to: p-1.
      ].
    p := reqPath break: '/'.
    res := List new.
    p do: [:dir |
      dir = '.' ifFalse: [
        dir = '..' ifTrue: [
          res isEmpty ifFalse: [ res removeFirst ]
        ] ifFalse: [
          res add: dir
        ].
      ]
    ].
    p := '/'.
    res do: [:dir | p := '/' + dir + p ].
    reqPath := p.
  ]

  parseRequest: s [
    reqStr := s.
    self parseRequest.
    reqStr := nil.
  ]
]
