Class String :ArrayedCollection
[
	, aString
		^ <StringCatenation self 
			(<SameTypeOfObject self aString>
				ifTrue:  [aString]
				ifFalse: [aString printString])>
|
	= aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompare self aString> = 0]
			ifFalse: [self compareError]
|
	< aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompare self aString> < 0]
			ifFalse: [self compareError]
|
	<=  aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompare self aString> <= 0]
			ifFalse: [self compareError]
|
	>=  aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompare self aString> >= 0]
			ifFalse: [self compareError]
|
	>  aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompare self aString> > 0]
			ifFalse: [self compareError]
|
	asInteger
		^ <primitive 164 self>
|
	asFloat
		^ <primitive 165 self>
|
	asSymbol
		^ <StringAsSymbol self>
|
	at: aNumber
		^ <StringAt self aNumber>
|
	at: aNumber put: aChar
		<StringAtPut self aNumber aChar>
|
	compareError
		^ self error: 'strings can only be compared to strings'
|
	copyFrom: start to: stop
		^ <CopyFromLength self start (stop - start + 1) >
|
	copyFrom: start length: len
		^ <CopyFromLength self start len >
|
	deepCopy
		^ <StringCopy self >
|
	new: size
		^ <NewString size>
|
	printAt: aPoint
		<PrintAt self (aPoint x) (aPoint y)>
|
	printString
		^ <StringPrintString self>
|
	print
		<PrintWithReturn self>
|
	printNoReturn
		<PrintNoReturn self>
|
	size
		^ <StringLength self>
|
	sameAs: aString
		^ <SameTypeOfObject self aString>
			ifTrue:  [<StringCompareWithoutCase self aString>]
			ifFalse: [self compareError]
]
