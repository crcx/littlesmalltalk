Class Symbol
[
	== aSymbol
		^ <SameTypeOfObject self aSymbol >
			ifTrue:  [<SymbolCompare self aSymbol >]
			ifFalse: [false]
|
	printString
		^ <SymbolPrintString self>
|
	asString
		^ <SymbolAsString self>
]

