"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
Requires [ httpsvx ]

Package [
  HttpClassBrowser
]


HttpHandler subclass: HttpHandlerAhah [
  emitPackages [
    Package packages keysDo: [:obj | self emit: obj asString; emit: '\n' ].
  ]

  emitClasses [
    | pkg |
    (pkg := req var: #package) ifNotNil: [ pkg := Package find: pkg asSymbol ].
    pkg ifNotNil: [ pkg := pkg classes ] ifNil: [ pkg := globals ].
    pkg do: [:obj | (obj isKindOf: Class) ifTrue: [
      obj isMeta ifFalse: [ self emit: obj asString; emit: '\n'. ]
    ]].
  ]

  emitMethods [
    | cls |
    (cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]].
    cls ifNotNil: [
      cls class methods do: [:mth | self emit: '^'; emit: mth name asString; emit: '\n' ].
      cls methods do: [:mth | self emit: mth name asString; emit: '\n' ].
    ].
  ]

  emitSource [
    | cls mth isMeta |
    (mth := req var: #srctext ifAbsent: ['']) = '' ifTrue: [
      ((cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]]) ifNotNil: [
        (mth := req var: #method) ifNotNil: [
          mth firstChar == $^ ifTrue: [ mth := mth from: 2. cls := cls class. isMeta := '^' ] ifFalse: [ isMeta := '' ].
          (mth := cls findMethodInAll: mth asSymbol ifAbsent: [ nil ]) ifNotNil: [
            self emit: isMeta; emit: mth text.
          ]
        ]
      ]
    ].
  ]

  compileMethod [
    | mth cls p txt isMeta |
    (txt := (req var: #srctext ifAbsent: ['']) removeTrailingBlanks) = '' ifTrue: [
      ^self emit: 'ERROR: nothing to compile!'.
    ].
    ((cls := req var: #class) ifNotNil: [ cls := globals at: (cls asSymbol) ifAbsent: [ nil ]]) ifNil: [
      ^self emit: 'ERROR: no class selected!'.
    ].
    txt := txt reject: [ :c | c isCR ].
    "compile and add method"
    p := LstCompiler new.
    p errorBlock: [ :msg :lineNum |
      ^self emit: 'ERROR near line ' + lineNum asString + ': ' + msg htmlEscape + '\n'.
    ].
    p warningBlock: [ :msg :lineNum |
      self emit: 'WARNING near line ' + lineNum asString + ': ' + msg htmlEscape + '\n'.
    ].
    (mth := (cls addMethod: txt withCompiler: p)) ifNotNil: [
      isMeta := txt firstNonBlankChar == $^.
      p := cls asString + '>>'+ (isMeta ifTrue: ['^'] ifFalse: ['']) + mth name asString.
      self emit: 'SUCCESS: method ' + p + ' succcesfully compiled.\n'
    ].
  ]

  emitBody [
    ctype := 'text/plain'.
    self set2xx.
    req file = 'package' ifTrue: [ ^self emitPackages ].
    req file = 'class' ifTrue: [ ^self emitClasses ].
    req file = 'method' ifTrue: [ ^self emitMethods ].
    req file = 'srctext' ifTrue: [ ^self emitSource ].
    req file = 'compile' ifTrue: [ ^self compileMethod ].
    ^super emitBody
  ]
]
