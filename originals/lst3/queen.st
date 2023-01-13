Class NullQueen Object
Class Queen Object row column neighbor
Methods NullQueen 'all'
	first
		^ true
|
	next
		^ true
|
	checkRow: row column: column
		" we can't attack anything "

		^ false
|
	result
		^ List new
]
Methods Queen 'all'
	setColumn: aNumber neighbor: aQueen
		column <- aNumber.
		neighbor <- aQueen

|
	checkRow: testRow column: testColumn | columnDifference |
		columnDifference <- testColumn - column.
		(((row = testRow) or: 
			[ row + columnDifference = testRow]) or:
			[ row - columnDifference = testRow])
				ifTrue: [ ^ true ].
		^ neighbor checkRow: testRow column: testColumn
|
	first
		neighbor first.
		row <- 1.
		^ self testPosition
|
	next
		^ (self advance) and: [ self testPosition ]
|
	advance
		(row = 8)
			ifTrue: [ (neighbor next) ifFalse: [ ^ false ].
				  row <- 0 ].
		row <- row + 1.
		^ true
|
	testPosition
		[neighbor checkRow: row column: column]
			whileTrue: [ (self advance) ifFalse: [ ^ false ]].
		^ true
|
	result
		^ neighbor result addLast: row
]
Methods Test 'queen'
	queen		| lastQueen |
		lastQueen <- NullQueen new.
		(1 to: 8) do: [:i | lastQueen <- Queen new;
					setColumn: i neighbor: lastQueen ].
		lastQueen first.
		(lastQueen result asArray = #(1 5 8 6 3 7 2 4) )
			ifTrue: ['8 queens test passed' print]
			ifFalse: [smalltalk error: '8queen test failed']
]
