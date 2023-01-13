*
* Little Smalltalk, version 3
* Written by Tim Budd, Oregon State University, July 1988
*
* methods for Collection classes
*
Class Link Object key value nextLink
Class Collection Magnitude
Class    IndexedCollection Collection
Class       Array IndexedCollection
Class          ByteArray Array
Class             String ByteArray
Class       Dictionary IndexedCollection hashTable
Class    Interval Collection lower upper step
Class    List Collection links
Class       Set List
*
Methods Array 'all'
	< coll
		(coll isKindOf: Array)
			ifTrue: [ self with: coll 
				   do: [:x :y | (x = y) ifFalse: 
						  [ ^ x < y ]].
				  ^ self size < coll size ]
			ifFalse: [ ^ super < coll ]
|
	= coll
		(coll isKindOf: Array)
			ifTrue: [ (self size = coll size)
					ifFalse: [ ^ false ].
				  self with: coll
					do: [:x :y | (x = y) 
						ifFalse: [ ^ false ] ]. 
				 ^ true ]
			ifFalse: [ ^ super = coll ]
|
	at: index put: value
		(self includesKey: index)
			ifTrue: [ self basicAt: index put: value ]
			ifFalse: [ smalltalk error: 
				'illegal index to at:put: for array' ]
|
	binaryDo: aBlock
		(1 to: self size) do:
			[:i | aBlock value: i value: (self at: i) ]
|
	collect: aBlock		| s newArray |
		s <- self size.
		newArray <- Array new: s.
		(1 to: s) do: [:i | newArray at: i put: 
			(aBlock value: (self at: i))].
		^ newArray
|
	copyFrom: low to: high	| newArray newlow newhigh |
		newlow <- low max: 1.
		newhigh <- high min: self size.
		newArray <- self class new: (0 max: newhigh - newlow + 1).
		(newlow to: newhigh)
			do: [:i |  newArray at: ((i - newlow) + 1)
					put: (self at: i) ].
		^ newArray
|
	deepCopy
		^ self deepCopyFrom: 1 to: self size
|
	deepCopyFrom: low to: high	| newArray newlow newhigh |
		newlow <- low max: 1.
		newhigh <- high min: self size.
		newArray <- self class new: (0 max: newhigh - newlow + 1).
		(newlow to: newhigh)
			do: [:i |  newArray at: ((i - newlow) + 1)
					put: (self at: i) copy ].
		^ newArray
|
	do: aBlock
		(1 to: self size) do:
			[:i | aBlock value: (self at: i) ]
|
	exchange: a and: b	| temp |
		temp <- self at: a.
		self at: a put: (self at: b).
		self at: b put: temp
|
	grow: aValue	| s newArray |
		s <- self size.
		newArray <- Array new: s + 1.
		(1 to: s) do: [:i | newArray at: i put: (self at: i)].
		newArray at: s+1 put: aValue.
		^ newArray
|
	includesKey: index
		^ index between: 1 and: self size
|
	new
		^ smalltalk error: 'arrays and strings cannot be created using new'
|
	reverseDo: aBlock
		(self size to: 1 by: -1) do:
			[:i | aBlock value: (self at: i) ]
|
	select: aCond	| newList |
		newList <- List new.
		self do: [:i | (aCond value: i) ifTrue: [newList addLast: i]].
		^ newList asArray
|
	shallowCopy
		^ self copyFrom: 1 to: self size
|
	size
		^ self basicSize
|
	with: newElement	| s newArray |
		s <- self size.
		newArray <- Array new: (s + 1).
		(1 to: s) do: [:i | newArray at: i put: (self at: i) ].
		newArray at: s+1 put: newElement.
		^ newArray
|
	with: coll do: aBlock
		(1 to: (self size min: coll size))
			do: [:i | aBlock value: (self at: i) 
					value: (coll at: i) ]
|
	with: coll ifAbsent: z do: aBlock	| xsize ysize |
		xsize <- self size.
		ysize <- coll size.
		(1 to: (xsize max: ysize))
			do: [:i | aBlock value:
			  (i <= xsize ifTrue: [ self at: i ] ifFalse: [ z ])
			  value:
			  (i <= ysize ifTrue: [ coll at: i ] ifFalse: [ z ])]
]
Methods ByteArray 'all'
	asString
		<22 self String>
|
	basicAt: index put: value
		^ ((value isMemberOf: Integer) and: [value between: 0 and: 255])
			ifTrue: [ <32 self index value > ]
			ifFalse: [ value print. smalltalk error: 
				'assign illegal value to ByteArray']
|
	basicAt: index
		^ <26 self index>
|
	size: value
		^ <22 <59 value> ByteArray>
]
Methods Collection 'all'
	< coll
		(coll respondsTo: #includes:)
			ifFalse: [ ^ smalltalk error:
				  'collection compared to non collection'].
		self do: [:x | ((self occurrencesOf: x) < 
			(coll occurrencesOf: x))ifFalse: [ ^ false ]].
		coll do: [:x | (self includes: x) ifFalse: [ ^ true ]].
		^ false
|
	= coll
		self do: [:x | (self occurrencesOf: x) = 
				(coll occurrencesOf: x) ifFalse: [ ^ false ] ].
		^ true
|
	asArray		| newArray i |
		newArray <- Array new: self size.
		i <- 0.
		self do: [:x | i <- i + 1. newArray at: i put: x].
		^ newArray
|
	asByteArray	| newArray i |
		newArray <- ByteArray new size: self size.
		i <- 0.
		self do: [:x | i <- i + 1. newArray at: i put: x].
		^ newArray
|
	asSet
		^ Set new addAll: self
|
	asString
		^ self asByteArray asString
|
	display
		self do: [:x | x print ]
|
	includes: value
		self do: [:x | (x = value) ifTrue: [ ^ true ] ].
		^ false
|
	inject: thisValue into: binaryBlock     | last |
		last <- thisValue.
		self do: [:x | last <- binaryBlock value: last value: x].
		^ last
|
	isEmpty 
		^ self size == 0
|
	occurrencesOf: anObject
		^ self inject: 0
		       into: [:x :y | (y = anObject) 
					 ifTrue: [x + 1]
					 ifFalse: [x] ]
|
	printString
		^ ( self inject: self class printString , ' ('
			 into: [:x :y | x , ' ' , y printString]), ' )'
|
	size
		^ self inject: 0 into: [:x :y | x + 1]
|
	sort: aBlock
		^ self inject: List new
			into: [:x :y | x add: y ordered: aBlock. x]
|
	sort
		^ self sort: [:x :y | x < y ]
]
Methods Dictionary 'all'
	new
		hashTable <- Array new: 39
|
	hash: aKey
		^ 3 * ((aKey hash) rem: ((hashTable size) quo: 3))
|
	at: aKey ifAbsent: exceptionBlock	| hashPosition  link |

		hashPosition <- self hash: aKey.
		((hashTable at: hashPosition + 1) = aKey)
			ifTrue: [ ^ hashTable at: hashPosition + 2].
		link <- hashTable at: hashPosition + 3.
		^ (link notNil)
			ifTrue: [ link at: aKey ifAbsent: exceptionBlock ]
			ifFalse: exceptionBlock
|
	at: aKey put: aValue			| hashPosition link |

		hashPosition <- self hash: aKey.
		((hashTable at: hashPosition + 1) isNil)
		   ifTrue: [ hashTable at: hashPosition + 1 put: aKey ].
		((hashTable at: hashPosition + 1) = aKey)
		   ifTrue: [ hashTable at: hashPosition + 2 put: aValue ]
		   ifFalse: [ link <- hashTable at: hashPosition + 3.
			(link notNil)
				ifTrue: [ link at: aKey put: aValue ]
				ifFalse: [ hashTable at: hashPosition + 3
					put: (Link new; key: aKey; value: aValue)]]
|
	binaryDo: aBlock
		(1 to: hashTable size by: 3) do:
			[:i | (hashTable at: i) notNil
				ifTrue: [ aBlock value: (hashTable at: i)
						value: (hashTable at: i+1) ].
			      (hashTable at: i+2) notNil
				ifTrue: [ (hashTable at: i+2) 
						binaryDo: aBlock ] ]
|
	display
		self binaryDo: [:x :y | (x printString , ' -> ', 
					y printString ) print ]
|
	includesKey: aKey
		" look up, but throw away result "
		self at: aKey ifAbsent: [ ^ false ].
		^ true
|
	removeKey: aKey
		^ self removeKey: aKey
			ifAbsent: [ smalltalk error: 'remove key not found']
|
	removeKey: aKey ifAbsent: exceptionBlock
		^ (self includesKey: aKey)
			ifTrue: [ self basicRemoveKey: aKey ]
			ifFalse: exceptionBlock
|
	basicRemoveKey: aKey		| hashPosition link |
		hashPosition <- self hash: aKey.
		((hashTable at: hashPosition + 1) = aKey)
			ifTrue: [ hashTable at: hashPosition + 1 put: nil.
				  hashTable at: hashPosition + 2 put: nil]
			ifFalse: [ link <- hashTable at: hashPosition + 3.
				(link notNil)
					ifTrue: [ hashTable at: hashPosition + 3
							put: (link removeKey: aKey) ]]
]
Methods IndexedCollection 'all'
	addAll: aCollection
		aCollection binaryDo: [:i :x | self at: i put: x ]
|
	asArray	
		^ Array new: self size ; addAll: self
|
	asDictionary
		^ Dictionary new ; addAll: self
|
	at: aKey
		^ self at: aKey 
			ifAbsent: [ smalltalk error: 'index to at: illegal' ]
|
	at: index ifAbsent: exceptionBlock
		 ^ (self includesKey: index)
			ifTrue: [ self basicAt: index ]
			ifFalse: exceptionBlock
|
	binaryInject: thisValue into: aBlock     | last |
		last <- thisValue.
		self binaryDo: [:i :x | last <- aBlock value: last 
						value: i value: x].
		^ last
|
	collect: aBlock
		^ self binaryInject: Dictionary new
			into: [:s :i :x | s at: i put: (aBlock value: x).  s]
|
	do: aBlock
		self binaryDo: [:i :x | aBlock value: x ]
|
	keys
		^ self binaryInject: Set new 
			into: [:s :i :x | s add: i ]
|
	indexOf: aBlock
		^ self indexOf: aBlock
			ifAbsent: [ smalltalk error: 'index not found']
|
	indexOf: aBlock ifAbsent: exceptionBlock
		self binaryDo: [:i :x | (aBlock value: x)
				ifTrue: [ ^ i ] ].
		^ exceptionBlock value
|
	select: aBlock
		^ self binaryInject: Dictionary new
			into: [:s :i :x | (aBlock value: x)
					ifTrue: [ s at: i put: x ]. s ]
|
	values
		^ self binaryInject: List new
			into: [:s :i :x | s add: x ]
]
Methods Interval 'all'
	do: aBlock		| current |
		current <- lower.
		(step > 0) 
			ifTrue: [ [ current <= upper ] whileTrue:
					[ aBlock value: current.
			  		current <- current + step ] ]
			ifFalse: [ [ current >= upper ] whileTrue:
					[ aBlock value: current.
					current <- current + step ] ]
|
	lower: aValue
		lower <- aValue
|
	upper: aValue
		upper <- aValue
|
	step: aValue
		step <- aValue
]
Methods Link 'all'
	add: newValue whenFalse: aBlock
		(aBlock value: value value: newValue)
			ifTrue: [ (nextLink notNil)
				ifTrue: [ nextLink <- nextLink add: newValue 
					whenFalse: aBlock ]
			ifFalse: [ nextLink <- Link new; value: newValue] ]
			ifFalse: [ ^ Link new; value: newValue; link: self ]
|
	at: aKey ifAbsent: exceptionBlock
		(aKey = key)
			ifTrue: [ ^value ]
			ifFalse: [ ^ (nextLink notNil)
					ifTrue: [ nextLink at: aKey
						    ifAbsent: exceptionBlock ]
					ifFalse: exceptionBlock ]
|
	at: aKey put: aValue
		(aKey = key)
			ifTrue: [ value <- aValue ]
			ifFalse: [ (nextLink notNil)
				ifTrue: [ nextLink at: aKey put: aValue]
				ifFalse: [ nextLink <- Link new;
						key: aKey; value: aValue] ]
|
	binaryDo: aBlock
		aBlock value: key value: value.
		(nextLink notNil)
			ifTrue: [ nextLink binaryDo: aBlock ]
|
	key: aKey
		key <- aKey
|
	includesKey: aKey
		(key = aKey)
			ifTrue: [ ^ true ].
		(nextLink notNil)
			ifTrue: [ ^ nextLink includesKey: aKey ]
			ifFalse: [ ^ false ]
|
	link: aLink
		nextLink <- aLink
|
	next
		^ nextLink
|
	removeKey: aKey
		(aKey = key)
			ifTrue: [ ^ nextLink ]
			ifFalse: [ (nextLink notNil)
				ifTrue: [ nextLink <- nextLink removeKey: aKey]]
|
	removeValue: aValue
		(aValue = value)
			ifTrue: [ ^ nextLink ]
			ifFalse: [ (nextLink notNil)
				ifTrue: [ nextLink <- nextLink removeValue: aValue]]
|
	reverseDo: aBlock
		(nextLink notNil)
			ifTrue: [ nextLink reverseDo: aBlock ].
		aBlock value: value
|
	size
		(nextLink notNil)
			ifTrue: [ ^ 1 + nextLink size]
			ifFalse: [ ^ 1 ]
|
	value: aValue
		value <- aValue
|
	value
		^ value
]
Methods List 'all'
	add: aValue
		^ self addLast: aValue
|
	add: aValue ordered: aBlock
		(links isNil)
			ifTrue: [ self addFirst: aValue]
			ifFalse: [ links <- links add: aValue 
					whenFalse: aBlock ]
|
	addAll: aValue
		aValue do: [:x | self add: x ]
|
	addFirst: aValue
		links <- Link new; value: aValue; link: links
|
	addLast: aValue
		(links isNil)
			ifTrue: [ self addFirst: aValue ]
			ifFalse: [ links add: aValue whenFalse: [ :x :y | true ] ]
|
	collect: aBlock
		^ self inject: self class new
		       into: [:x :y | x add: (aBlock value: y). x ]
|
	links
		^ links  "used to walk two lists in parallel "
|
	reject: aBlock          
		^ self select: [:x | (aBlock value: x) not ]
|
	reverseDo: aBlock
		(links notNil)
			ifTrue: [ links reverseDo: aBlock ]
|
	select: aBlock          
		^ self inject: self class new
		       into: [:x :y | (aBlock value: y) 
					ifTrue: [x add: y]. x]
|
	do: aBlock
		(links notNil)
			ifTrue: [ links binaryDo: [:x :y | aBlock value: y]]
|
	first
		^ (links notNil)
			ifTrue: links
			ifFalse: [ smalltalk error: 'first on empty list']
|
	removeFirst
		self remove: self first
|
	remove: value
		(links notNil)
			ifTrue: [ links <- links removeValue: value ]
|
	size
		(links isNil)
			ifTrue: [ ^ 0 ]
			ifFalse: [ ^ links size ]
]
Methods Set 'all'
	add: value
		(self includes: value)
			ifFalse: [ self addFirst: value ]
]
Methods String 'all'
	, value
		(value isMemberOf: String)
			ifTrue: [ (self size + value size) > 2000
				    ifTrue: [ 'string too large' print. ^ self ]
				    ifFalse: [ ^ <24 self value> ] ]
			ifFalse: [ ^ self , value asString ]
|
	= value
		(value isKindOf: String)
			ifTrue: [ ^ super = value ]
			ifFalse: [ ^ false ]
|
	< value
		(value isKindOf: String)
			ifTrue: [ ^ super < value ]
			ifFalse: [ ^ false ]
|
	asByteArray	| newArray i |
		newArray <- ByteArray new size: self size.
		i <- 0.
		self do: [:x | i <- i + 1. newArray at: i put: x asInteger].
		^ newArray
|
	asInteger
		^ self inject: 0 into: [:x :y | x * 10 + y digitValue ]
|
	basicAt: index
		^  (super basicAt: index) asCharacter
|
	basicAt: index put: aValue
		(aValue isMemberOf: Char)
			ifTrue: [ super basicAt: index put: aValue asInteger ]
			ifFalse: [ smalltalk error:
				'cannot put non Char into string' ]
|
	asSymbol
		^ <83 self>
|
	copy
		" catenation makes copy automatically "
		^ '',self
|
	copyFrom: position1 to: position2
		^ <33 self position1 position2>
|
	hash
		^ <82 self>
|
	printString
		^ '''' , self, ''''
|
	size
		^ <81 self>
|
	words: aBlock	| text index list |
		list <- List new.
		text <- self.
		[ text <- text copyFrom: 
			(text indexOf: aBlock ifAbsent: [ text size + 1])
				to: text size.
		  text size > 0 ] whileTrue:
			[ index <- text 
				indexOf: [:x | (aBlock value: x) not ]
				ifAbsent: [ text size + 1].
			  list addLast: (text copyFrom: 1 to: index - 1).
			  text <- text copyFrom: index to: text size ].
		^ list asArray
|
	value
		" evaluate self as an expression "
		^ ( '^ [ ', self, ' ] value' ) execute
|
	execute	| meth |
		" execute self as body of a method "
		meth <- Method new; text: 'compile ', self.
		(meth compileWithClass: Object)
			ifTrue: [ ^ meth executeWith: #(0) ].
		^ nil
|
	unixCommand
		^ <88 self>
]
