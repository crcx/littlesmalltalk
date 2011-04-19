Class KeyedCollection :Collection
[
	add: anElement
		^ self error: 'Must add with explicit key'
|
	addAll: aCollection
                aCollection binaryDo: [:x :y | self at: x put: y].
                ^ aCollection
|
	asDictionary			| newCollection |
		newCollection <- Dictionary new.
		self binaryDo: 
			[:key :val | newCollection at: key put: val].
		^ newCollection
|
	at: key
                ^ self at: key ifAbsent:
                   [self error:
                         (key printString , ': association not found').
                    ^ key]
|
	atAll: aCollection put: anObject
		aCollection do: [:x | self at: x put: anObject]
|
	binaryDo: aBlock                | item |
		self do: [:x | aBlock value: self currentKey
					value: x ].
                ^ nil
|
	coerce: aCollection	| newobj |
		newobj <- self class new.
		aCollection binaryDo: [:x :y | newobj at: x put: y].
		^ newobj
|
	collect: aBlock 
		^ self coerce:
		     (self inject: Dictionary new
                           into: [:x :y | x at: self currentKey
					    put: (aBlock value: y) . x ] )
|
	includesKey: key
                self at: key ifAbsent: [^ false].
                ^ true
|
	indexOf: anElement
		^ self indexOf: anElement
		ifAbsent: [self error: 'indexOf element not found']
|
	indexOf: anElement ifAbsent: exceptionBlock
                self do: [:x | (x = anElement) 
  				  ifTrue: [ ^ self currentKey ]].
                 ^ exceptionBlock value
|
	keys                             | newset |
                newset <- Set new.
                self keysDo: [:x | newset add: x].
                ^ newset
|
	keysDo: aBlock
                ^ self do: [ :x | aBlock value: self currentKey ]
|
	keysSelect: aBlock          
		^ self coerce:
		     (self inject: Dictionary new
                           into: [:x :y | (aBlock value: y currentKey)
                                           ifTrue: [x at: self currentKey
                                                      put: y]. x ] )
|
	remove: anElement
		^ self error: 'object must be removed with explicit key'
|
	removeKey: key
                ^ self removeKey: key ifAbsent:
                   [self error: 'no element associated with key'. ^ key]
|
	removeKey: key ifAbsent: exceptionBlock
		^ self error: 'subclass should implement RemoveKey:ifAbsent:'
|
	select: aBlock          
		^ self coerce:
		     (self inject: Dictionary new
                           into: [:x :y | (aBlock value: y)
                                           ifTrue: [x at: self currentKey
                                                      put: y]. x ] )
|
	values                           | newbag |
                newbag <- Bag new.
                self do: [:x | newbag add: x].
                ^ newbag
]
