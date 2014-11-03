{
  System isWindows ifFalse: [ System loadModule: 'readline' ].
  System loadModule: 'quit'.
  System loadModule: 'repl'.
  "System loadModule: 'binimage'."

  "System loadModule: 'image'."
  "System loadModule: 'ffi'."
  "System loadModule: 'date'."

  System verbose ifTrue: [ 'init.st loaded' printNl ].

  (cliArgs at: 1) ifNil: [
    "try to load 'main.st'"
    (File exists: 'main.st') ifTrue: [
      System loadModule: 'quit'.
      File fileIn: 'main.st'.
      System quit: 0.
    ].
  ]
}
