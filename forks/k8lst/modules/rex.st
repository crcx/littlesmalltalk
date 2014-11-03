"
 coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 Understanding is not required. Only obedience.

 This program is free software. It comes without any warranty, to
 the extent permitted by applicable law. You can redistribute it
 and/or modify it under the terms of the Do What The Fuck You Want
 To Public License, Version 2, as published by Sam Hocevar. See
 http://sam.zoy.org/wtfpl/COPYING for more details.
"
" simple regular expression class
  syntax:
    [...]   range
    [^...]  not-range
    []...]  range with ']'
    .       any char
    ^       at the start of re: match should start at the start of the line
    $       at the end of re: match should end at the end of the line
    (...)   capture
    any atom can be followed by '?', '+' or '*'; add '?' to non-greedy mode
"
Package [ RegExp ]

class: SimpleRegExp [
| rexStr
  atoms string position hasSOL hasEOL
  matchStart matchEnd captures captureCount |

^new [
  self error: 'use "SimpleRegExp new:" to create SimpleRegExp instance'
]

^new: aStr [
  | obj |
  obj := self basicNew.
  obj parseString: aStr.
  ^obj
]

^match: aRex for: aStr [
  | obj |
  obj := self new: aRex.
  ^obj matchFor: aStr
]


"-------- getters --------"
matchStart [
  ^matchStart
]

matchEnd [
  ^matchEnd
]


captures [
  ^captures
]

captureAt: idx [
  ^captures at: idx
]

captureStartAt: idx [
  ^(captures at: idx) at: 1
]

captureEndAt: idx [
  ^(captures at: idx) at: 2
]

asString [
  ^rexStr
]

"-------- atom adder --------"
addAtom: aSymbol [
  atoms := atoms with: aSymbol
]

addAtom: aSymbol arg: aArg [
  atoms := atoms with: (Array with: aSymbol with: aArg)
]


"-------- char eater --------"
peekChar [
  position > string size ifTrue: [ ^nil ]
  ifFalse: [^ string at: position ].
]

nextChar [
  | c |
  position > string size ifTrue: [ ^nil ]
  ifFalse: [
    c := string at: position.
    position := position + 1.
    ^c
  ]
]

"-------- parser --------"
parseString: aStr [
  rexStr := string := aStr.
  position := 1.
  atoms := Array new: 0.
  ^self parse
]

isNonGreedy [
  self peekChar == $? ifTrue: [
    self nextChar.
    ^true
  ].
  ^false
]

parserAddWild: aName arg: aArg [
  self isNonGreedy ifTrue: [ aName := aName + 'NonGreedy' ].
  aName := aName + ':from:'.
  self addAtom: aName asSymbol arg: aArg
]

parse [
  | c curAtom cpt cptClosed |
  captureCount := 0.
  hasSOL := false. hasEOL := false.
  self peekChar == $^ ifTrue: [
    self nextChar.
    hasSOL := true.
  ].
  [ c := self nextChar ] whileNotNil: [
    (cpt := c == $() ifTrue: [
      (c := self nextChar) ifNil: [ self error: 'unexpected end of RegExp' ].
      captureCount := captureCount + 1.
      self addAtom: #captureStart: arg: captureCount.
    ].
    Case test: c;
      case: $[ do: [ curAtom := self parseRange ];
      case: $. do: [ curAtom := Array with: #any: with: nil ];
      case: $$ do: [
        position > string size ifTrue: [ hasEOL := true. ^self ].
        curAtom := Array with: #char: with: $$.
      ];
      else: [:c | curAtom := Array with: #char: with: c ].
    cptClosed := false.
    self peekChar == $) ifTrue: [
      position > (string size + 1) ifFalse: [
        self nextChar.
        ('?*+' includes: self peekChar) ifTrue: [ cptClosed := true. ] ifFalse: [ position := position - 1 ]
      ].
    ].
    Case test: self peekChar;
      case: $? do: [ self nextChar. self parserAddWild: 'zeroOrOne' arg: curAtom ];
      case: $* do: [ self nextChar. self parserAddWild: 'zeroOrMore' arg: curAtom ];
      case: $+ do: [ self nextChar.
        self addAtom: #one:from: arg: curAtom.
        self parserAddWild: 'zeroOrMore' arg: curAtom.
      ];
      else: [:c | self addAtom: #one:from: arg: curAtom. ].
    cpt ifTrue: [
      "capture end"
      cptClosed ifFalse: [ self nextChar == $) ifFalse: [ self error: 'missing ")"' ]].
      self addAtom: #captureEnd: arg: captureCount.
    ].
  ].
]

parseRange [
  | c ce isNot range |
  (c := self peekChar) ifNil: [ self error: 'unexpected end of RegExp' ].
  (isNot := c == $^) ifTrue: [ self nextChar ].
  (c := self peekChar) ifNil: [ self error: 'unexpected end of RegExp' ].
  range := ''.
  (c == $]) ifTrue: [ self nextChar. range := range + ']' ].
  [ c := self nextChar ] whileNotNil: [
    c == $] ifTrue: [
      ^Array with: (isNot ifTrue: [ #rangeNot: ] ifFalse: [ #range: ]) with: range.
    ].
    self peekChar == $- ifTrue: [
      self nextChar.
      (ce := self nextChar) ifNil: [ self error: 'unexpected end of RegExp' ].
      ce == $] ifTrue: [
        range := range + '-'.
        ^Array with: (isNot ifTrue: [ #rangeNot: ] ifFalse: [ #range: ]) with: range.
      ].
      "interval"
      c value to: ce value do: [:c | range := range + (Char new: c) ].
    ] ifFalse: [ range := range + c ].
  ].
  self error: 'unexpected end of RegExp'
]


"-------- captures --------"
captureStart: aNo [
  | c |
  (c := captures at: aNo) ifNil: [ captures at: aNo put: (c := Array new: 2) ].
  c at: 1 put: position.
]

captureEnd: aNo [
  | c |
  (c := captures at: aNo) ifNil: [ captures at: aNo put: (c := Array new: 2) ].
  c at: 2 put: position - 1.
]


"-------- simple matchers --------"
any: aChar [
  ^self nextChar notNil
]

char: aChar [
  ^self nextChar = aChar
]

range: aString [
  | c |
  (c := self nextChar) ifNil: [ ^false ].
  ^aString includes: c
]

rangeNot: aString [
  | c |
  (c := self nextChar) ifNil: [ ^false ].
  ^(aString includes: c) not
]


"-------- repeat matchers --------"
one: aAtom from: aPosNext [
  (self perform: (aAtom at: 1) with: (aAtom at: 2)) ifFalse: [ ^nil ].
  ^false
]

zeroOrOne: aAtom from: aPosNext [
  (self perform: (aAtom at: 1) with: (aAtom at: 2))
    ifTrue: [
      "try to match the rest"
      (self matchFrom: aPosNext) ifTrue: [ ^true ].
    ].
  "i need a backup!"
  position := position - 1.
  ^false
]

zeroOrMore: aAtom from: aPosNext [
  | stpos |
  stpos := position.
  self findLastMatch: aAtom.
  [ position > stpos ] whileTrue: [
    (self matchFrom: aPosNext) ifTrue: [ ^true ].
    position := position - 1.
  ].
  ^false
]

findLastMatch: aAtom [
  "find the last match for aAtom"
  | sym arg |
  sym := aAtom at: 1.
  arg := aAtom at: 2.
  [ position > string size ] whileFalse: [
    (self perform: sym with: arg) ifFalse: [
      position := position - 1.
      ^position
    ].
  ].
  ^position
]

zeroOrOneNonGreedy: aAtom from: aPosNext [
  "try to match the rest"
  (self matchFrom: aPosNext) ifTrue: [ ^true ].
  (self perform: (aAtom at: 1) with: (aAtom at: 2)) ifFalse: [ ^nil ].  "failure"
  "i need a backup!"
  position := position - 1.
  ^false
]

zeroOrMoreNonGreedy: aAtom from: aPosNext [
  | sym arg |
  sym := aAtom at: 1.
  arg := aAtom at: 2.
  [ (self matchFrom: aPosNext) ifTrue: [ ^true ].
    position > string size ]
   whileFalse: [
    (self perform: sym with: arg) ifFalse: [ ^nil ].  "failure"
   ].
  ^nil
]


"-------- sequence matcher --------"
matchFrom: aPos [
  | opos res atom sym arg |
  opos := position.
  [ aPos > atoms size ] whileFalse: [
    sym := (atom := atoms at: aPos) at: 1.
    arg := atom at: 2.
    aPos := aPos + 1.
    sym == #captureStart: ifTrue: [ self captureStart: arg ]
    ifFalse: [
      sym == #captureEnd: ifTrue: [ self captureEnd: arg ]
      ifFalse: [
        (res := (self perform: sym with: arg with: aPos)) ifNil: [ position := opos. ^false ].
        res ifTrue: [ ^true ].
      ].
    ].
  ].
  hasEOL ifTrue: [ res := position > string size ] ifFalse: [ res := true ].
  res ifTrue: [ matchEnd := position - 1 ].
  position := opos.
  ^res
]


"-------- matcher entry point --------"
matchFor: aStr [
  | stpos |
  string := aStr.
  position := matchStart := 1.
  captures := Array new: captureCount.
  (self matchFrom: 1) ifTrue: [ ^true ].
  hasSOL ifTrue: [ ^false ].
  stpos := 2.
  [ stpos > string size ] whileFalse: [
    matchStart := position := stpos.
    (self matchFrom: 1) ifTrue: [ ^true ].
    stpos := stpos + 1.
  ].
  ^false
]
]
