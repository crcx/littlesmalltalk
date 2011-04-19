"
	Turing machine simulator contributed by Jan Gray,
		the University of Waterloo
"
Class Main
[
	main			| tm |
		tm <- TuringMachine new initialize.
		tm delta state: 0 input: $# nextState: 1 output: $L.
		tm delta state: 1 input: $I nextState: 1 output: $i.
		tm delta state: 1 input: $i nextState: 1 output: $L.
		tm delta state: 1 input: $# nextState: 2 output: $R.
		tm delta state: 2 input: $i nextState: 2 output: $R.
		tm delta state: 2 input: $# nextState: 'halt' output: $#.
		tm tape: 'IIIIII'.
		tm delta print.
		tm run
]
Class TuringMachine
|       tape            "Infinite tape"
        state           "Current state, TM continues if state is a number"
        delta           "A TransitionTable, which for each (state, input)
                         gives (next state, output)"
        tapeMoves       "A Dictionary which maps L and R into [tape left]
                         and [tape right]"
|
[
        initialize
                tapeMoves <- Dictionary new.
                tapeMoves at: $L put: [tape left].
                tapeMoves at: $R put: [tape right].
                delta <- TransitionTable new.
                self tape: ''.
                self state: 0
|
        tape: aString
                tape <- Tape new with: aString
|
        state: aState
                state <- aState
|
        delta
                ^ delta
|
        step
                | next |
                next <- delta atState: state input: tape read.
                state <- next state.
                (state isKindOf: Number)
                        ifTrue: [(tapeMoves includesKey: next symbol)
                                        ifTrue:  [(tapeMoves at: next symbol) value]
                                        ifFalse: [tape write: next symbol]]
|
        run
                state <- 0.
                self print.
                [state isKindOf: Number] whileTrue: [self step print]
|
        printString
                ^ 'State ', state printString, ', Tape ', tape printString
]
Class Pair	:Magnitude
| state symbol |
[
        state: aState symbol: aSymbol
                state <- aState.
                symbol <- aSymbol
|
        state
                ^ state
|
        symbol
                ^ symbol
|
	< aPair
		^ state < aPair state or:
			[state = aPair state and: [symbol < aPair symbol]]
|
        printString
                ^ state printString, '	', symbol printString
]
Class TransitionTable :Dictionary
[
        state: aState input: in nextState: nextState output: out
                self at: (Pair new state: aState symbol: in)
                     put: (Pair new state: nextState symbol: out).
		^ nil
|
        atState: aState input: in
                ^ self at: (Pair new state: aState symbol: in)
                       ifAbsent: [^ Pair new state: 'hung' symbol: nil].
|
        print
                'State	Read	Next	Write' print.
		self binaryDo: [:x :y |
			(x printString , '	', y printString) print]
]
Class Tape :Object
| tape position |
[
        with: aString
                tape <- '#', aString, '#'.
                position <- tape size
|
        read
                ^ tape at: position
|
        write: aChar
                tape at: position put: aChar.
|
        left
                (position > 1)
                        ifTrue: [position <- position - 1]
|
        right
                (position = tape size)
                        ifTrue: [tape <- tape, '#'].
                position <- position + 1
|
        printString
                ^ (tape copyFrom: 1 to: position - 1), '{',
                  ((tape at: position) asString), '}',
                  (tape copyFrom: position + 1 to: tape size)
]
