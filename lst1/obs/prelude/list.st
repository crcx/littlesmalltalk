"
	Lists are implemented using Points in order to
	reduce the number of classes in the standard prelude
"
Class List :SequenceableCollection
| first current |
[
	add: anItem
		first <- (Point new x: anItem ) y: first .
		^ anItem
|
	addFirst: anItem
		first <- (Point new x: anItem ) y: first .
		^ anItem
|
	addLast: anItem
		(first isNil) 
			ifTrue: [^ self addFirst: anItem].
		(self findLast) y: ((Point new x: anItem) y: nil).
		^ anItem
|
	addAllFirst: aCollection
		aCollection do: [:x | self addFirst: x]
|	
	addAllLast: aCollection
		aCollection do: [:x | self addLast: x]
|
	coerce: aCollection		| newList |
		newList <- List new.
		aCollection do: [:x | newList addLast: x].
		^ newList
|
	findLast		| item |
		((item <- first) isNil)
			ifTrue: [^ nil].
		[(item y) notNil]
			whileTrue: [item <- item y].
		^ item
|
	remove: anItem
		^ self remove: anItem 
			ifAbsent: [self error: 'cant find item']
|
	remove: anItem ifAbsent: exceptionBlock
		(first isNil) 
			ifTrue: [^ exceptionBlock value].
		self inject: nil into: [:prev :current |
			(current x == anItem)
				ifTrue: [(prev isNil)
						ifTrue: [first <- current y]
						ifFalse: [prev y: (current y)].
					 ^ anItem].
			current ] .
		^ exceptionBlock value
|
	removeError
		^ self error: 'cannot remove from an empty list'
|
	removeFirst	| item |
		(first isNil)
			ifTrue: [^ self removeError].
		item <- first.
		first <- first y.
		^ item x
|
	removeLast
		(first isNil)
			ifTrue: [^ self removeError].
		^ self remove: self last 
			ifAbsent: [self removeError]
|
	first
		^ ((current <- first) notNil) 
			ifTrue: [ current x ]
|
	next
		^ ((current <- current y) notNil)
			ifTrue: [ current x ]
|
	current
		^ current x
|
	last
		(first isNil) 
			ifTrue: [^ nil].
		^ self findLast x
|
	isEmpty
		^ first == nil
]
