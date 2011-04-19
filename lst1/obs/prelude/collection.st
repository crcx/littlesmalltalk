Class Collection
[
        addAll: aCollection
                aCollection do: [:x | self add: x ]

|
	asArray
		^ Array new: self size ;
			replaceFrom: 1 to: self size with: self
|
	asBag
                ^ Bag new addAll: self
|
	asSet
                ^ Set new addAll: self
|
	asList
                ^ List new addAllLast: self
|
	asString
		^ String new: self size ; 
			replaceFrom: 1 to: self size with: self
|
	coerce: aCollection	| newobj |
		newobj <- self class new.
		aCollection do: [:x | newobj add: x].
		^ newobj
|
	collect: aBlock
		^ self inject: self class new
		       into: [:x :y | x add: (aBlock value: y). x ]
|
	deepCopy		| newobj |
		newobj <- List new .
		self do: [:x | newobj addLast: x copy ].
		^ self coerce: newobj
|
	detect: aBlock
		^ self detect: aBlock
		ifAbsent: [self error: 'no object found matching detect']

|
        detect: aBlock ifAbsent: exceptionBlock   
                self do: [:x | 
                          (aBlock value: x) ifTrue: [^ x]].
                ^ exceptionBlock value
|
 	first
		^ self error: 'subclass should implement first'
|
        includes: anObject
		self do: [:x | (x == anObject) ifTrue: [^ true]].
		^ false
|
        inject: thisValue into: binaryBlock     | last |
                last <- thisValue.
                self do: [:x | last <- binaryBlock value: last value: x].
                ^ last
|
        isEmpty
                ^ (self size = 0)
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
	reject: aBlock          
		^ self select: [:x | (aBlock value: x) not ]
|
        remove: oldObject
                self remove: oldObject ifAbsent:
                  [^ self error: 
			'attempt to remove object not found in collection' ].
                ^ oldObject
|
	remove: oldObject ifAbsent: exceptionBlock
		^ (self includes: oldObject)
			ifTrue: [self remove: oldObject]
			ifFalse: exceptionBlock
|
	select: aBlock          
		^ self inject: self class new
		       into: [:x :y | (aBlock value: y) 
                                        ifTrue: [x add: y]. x]
|
	shallowCopy		| newobj |
		newobj <- List new .
		self do: [:x | newobj addLast: x].
		^ self coerce: newobj
|
	size		| i |
		i <- 0.
		self do: [:x | i <- i + 1 ].
		^ i
]
