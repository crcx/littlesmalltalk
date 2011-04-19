Class ArrayedCollection :SequenceableCollection
| current |
[
       = anArray                       | i |
                (self size ~= anArray size) ifTrue: [^ false].
                i <- 0.
                self do: [:x | (x ~= (anArray at: (i <- i + 1)))
                                ifTrue: [^ false]].
		^ true
|
        at: key ifAbsent: exceptionBlock
		((key <= 0) or: [key > self size])
			ifTrue: [^ exceptionBlock value].
                ^ self at: key
|
	coerce: aCollection		| temp |
		temp <- self class new: aCollection size.
		temp replaceFrom: 1 to: aCollection size with: aCollection.
		^ temp
|
       copyFrom: start to: stop                | size temp |
		size <- stop - start + 1.
		temp <- self class new: size.
		temp replaceFrom: 1 to: size with: self startingAt: start.
		^ temp
|
        currentKey
                ^ current
| 
	deepCopy		| newobj |
		newobj <- self class new: self size.
		(1 to: self size) do:
			[:i | newobj at: i
				put: (self at: i) copy ].
		^ newobj
|
       do: aBlock
                (1 to: self size) 
		    do: [:i | current <- i. 
				aBlock value: (self at: i)]
|
       first
                current <- 1.
                ^ (current <= self size) 
			ifTrue: [ self at: current]
|
	firstKey
		^ 1
|
	lastKey
		^ self size
|
	next
                current <- current + 1.
                ^ (current <= self size) 
			ifTrue: [ self at: current]
|
	padTo: length
		^ (self size < length)
			ifTrue: [ self , 
				(self class new: (length - self size) ) ]
			ifFalse: [ self ]
|
	shallowCopy		| newobj |
		newobj <- self class new: self size.
		(1 to: self size) do:
			[:i | newobj at: i 
				put: (self at: i) ].
		^ newobj
]
