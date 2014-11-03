Package [
  PkgWriter
]


class: PackageWriter [
  | file pkg visited |

  ^write: pkgName to: fileName [
    | obj fl pkg |
    (pkg := Package find: pkgName) ifNil: [ self error: 'no package: ' + (pkgName asString) ].
    fl := File openWrite: fileName.
    obj := super new.
    self in: obj at: 1 put: fl.
    self in: obj at: 2 put: pkg.
    self in: obj at: 3 put: nil.
    obj writeSource.
    ^true
  ]

  writeSource [
    visited := Dictionary new.
    self writeClasses.
  ]

  inPackage: cls [
    ^pkg classes includes: cls asString asSymbol
  ]

  writeClasses [
    file write: 'Package [\n'.
    file write: '  '.
    file write: pkg name asString.
    file newline.
    file write: ']\n\n\n'.
    pkg classes do: [:cls | self writeClass: cls ].
  ]

  writeClass: cls [
    ((cls isKindOf: Class) and: [ cls isMeta not ]) ifTrue: [
      (visited includes: cls asString asSymbol) ifFalse: [
        visited at: cls asString asSymbol put: true.
        (self inPackage: cls super) ifTrue: [
          self writeClass: cls super.
        ].
        'writing ' print. cls asString printNl.
        self writeOneClass: cls.
      ].
    ].
  ]

  writeOneClass: cls [
    | vars parent |
    parent := cls super.
    file write: parent asString + ' subclass: ' + cls asString + ' '.
    vars := cls class variables.
    ((vars notNil) and: [ vars size > 0 ]) ifTrue: [
      file write: '|'.
      vars do: [:v | file write: ' ' + v asString ].
      file write: ' | '.
    ].
    file write: '[\n'.
    vars := cls variables.
    ((vars notNil) and: [ vars size > 0 ]) ifTrue: [
      file write: '|'.
      vars do: [:v | file write: ' ' + v asString ].
      file write: ' |\n'.
    ].
    self writeMethodsOf: cls class asMeta: true.
    self writeMethodsOf: cls asMeta: false.
    file write: ']\n\n'.
  ]

  writeMethodsOf: cls asMeta: isMeta [
    | methods |
    methods := cls methods.
    methods ifNil: [ ^nil ].
    methods do: [:m | self writeMethod: m of: cls asMeta: isMeta ]
  ]

  "FIXME: skip method name"
  writeMethod: meth of: cls asMeta: isMeta [
    | txt c |
    file newline.
    isMeta ifTrue: [ file write: '^' ].
    txt := meth text asString.
    c := txt indexOf: String newline.
    file write: (txt from: 1 to: c-1) removeTrailingBlanks.
    file write: ' [\n'.
    file write: (txt from: c+1)  removeTrailingBlanks.
    "Verify if txt ends with a newline"
    file write: '\n]\n'.
  ]
]
