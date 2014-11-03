METHODS FOR UndefinedObject
initialize [
  Char initialize.
  Package initialize.
  System initialize.
]

loadFile: fname [
  ^File fileIn: fname
]

runREPL [
  ^REPL new REPL
]

doString: str [
  ^str doItSeq
]

newSymbol: aStr [
  ^aStr asSymbol
]

setGlobal: arStrVal [
  globals at: (arStrVal at: 1) asSymbol put: (arStrVal at: 2).
  ^true
]

COMMENT xDump [
COMMENT   'xDump; ' print. UndefinedObject parent print. '|' print. UndefinedObject parent class printNl.
COMMENT ]
!
