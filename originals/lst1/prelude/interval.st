Class Interval :SequenceableCollection
| lower upper step current |
[
	from: lowerBound to: upperBound by: stepSize
		current <- lower <- lowerBound.
		upper <- upperBound.
		step  <- stepSize

|	size	
		^ ((step strictlyPositive)
			ifTrue: [upper < lower]
			ifFalse: [lower < upper] )
		   ifTrue: [ 0 ]
		   ifFalse: [upper - lower // step + 1]

|	inRange: value
		^ (step strictlyPositive)
			ifTrue: [(value >= lower) and: [value <= upper]]
			ifFalse: [(value >= upper) and: [value <= lower]]

|       first
                current <- lower.
		^ (self inRange: current) ifTrue: [current]

|       next
                current <- current + step.
		^ (self inRange: current) ifTrue: [current]

|	at: index ifAbsent: exceptionBlock	| val |
		val <- lower + (step * (index - 1)).
		^ (self inRange: val)
		   ifTrue: [ val ]
		   ifFalse: [exceptionBlock value]

|	printString
		^ 'Interval ', lower printString , ' to ',
                     upper printString , ' by ' , step printString 

|	coerce: newcollection
		^ newcollection asArray

|	at: index put: val
		^ self error: 'cannot store into Interval'

|	add: val
		^ self error: 'cannot store into Interval'

|	removeKey: key ifAbsent: exceptionBlock
		self error: 'cannot remove from Interval'.
		^ exceptionBlock value
|
	deepCopy
		^ lower to: upper by: step
|	
	shallowCopy
		^ lower to: upper by: step
]
