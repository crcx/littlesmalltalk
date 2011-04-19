Class Array :ArrayedCollection
[
	new: aValue
		^ <NewArray aValue>
|
	at: aNumber
		( (aNumber < 1) or: [aNumber > <Size self> ] )
			ifTrue: [ self error: 'index error'. ^nil ].
		^ <At self aNumber >
|
	at: aNumber put: aValue
		( (aNumber < 1) or: [aNumber > <Size self> ] )
			ifTrue: [ self error: 'index error'. ^nil ].
		<AtPut self aNumber aValue >.
		^ aValue
|
	grow: newObject
		^ <Grow self newObject>
|
	printString		| value i |
		value <- ')'.
		i <- <Size self>.
		[i > 0] whileTrue: 
			[ value <- <At self i>  printString ,
					' ', value.
				    i <- i - 1].
		^ '#( ' , value
|
	size
		^ <Size self>
]
