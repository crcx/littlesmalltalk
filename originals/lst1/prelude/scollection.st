Class SequenceableCollection :KeyedCollection
[
	, aCollection
		^ self coerce: (List new ; 
					addAllLast: self ;
					addAllLast: aCollection )
|
        collect: aBlock 
		^ self coerce:
		     (self inject: List new
                           into: [:x :y | x addLast: (aBlock value: y) . x ] )
|
	copyFrom: start to: stop                | newcol |
                newcol <- List new.
		(start to: stop) do: [:i | newcol addLast: (self at: i)].
                ^ self coerce: newcol
|
	copyWith: newElement
		^ self coerce: (List new ; 
					addAllLast: self ;
					addLast: newElement )
|
	copyWithout: oldElement                 | newcol |
                newcol <- List new.
                self do: [ :x | (x == oldElement) 
                                ifFalse: [ newcol addLast: x ]].
                ^ self coerce: newcol
|
	equals: aSubCollection startingAt: anIndex      | i |
                i <- 0.
                self do: [:x |
                        (x = (aSubCollection at: (anIndex + i)
                                            ifAbsent: [^ false]))
                                ifFalse: [^ false].
                        i <- i + 1].
                ^ true
|
	findFirst: aBlock
		^ self findFirst: aBlock 
			ifAbsent: [self error: 'first element not found']
|
	findFirst: aBlock ifAbsent: exceptionBlock
                self do: [:x | (aBlock value: x) 
				ifTrue: [ ^ self currentKey]].
                ^ exceptionBlock value
|
	findLast: aBlock
		self findLast: aBlock
			ifAbsent: [self error: 'last element not found']
|
	findLast: aBlock ifAbsent: exceptionBlock
                self reverseDo: [:x | (aBlock value: x) 
                                        ifTrue: [ ^ self currentKey]].
                ^ exceptionBlock value
|
	indexOfSubCollection: aSubCollection 
	startingAt: anIndex 
	ifAbsent: exceptionBlock  			| n m |

                n <- anIndex.
                m <- self size - aSubCollection size.
                [n <= m] whileTrue:
                        [(aSubCollection equals: self startingAt: n)
                                ifTrue: [^ n].
                         n <- n + 1].
                ^ exceptionBlock value
|
	indexOfSubCollection: aSubCollection startingAt: anIndex
                ^ self indexOfSubCollection: aSubCollection 
		       startingAt: anIndex
                       ifAbsent: [ self error: 'element not found'. nil]
|
	last
                ^ (0 = self size) ifFalse: [ self at: self lastKey ]
|
	replaceFrom: start to: stop with: repcol
		repcol inject: start
		       into: [:x :y | self at: x put: y. x + 1]
|
	replaceFrom: first to: stop with: repcol startingAt: repStart | i |
                i <- 0 .
                [(first + i) <= stop] whileTrue:
                        [self at: (first + i)
                              put: (repcol at: i + repStart).
			 i <- i + 1 ]
|
        reverseDo: aBlock                       | n m |
                n <- self lastKey.  m <- self firstKey.
                [n >= m] whileTrue:
                        [(self includesKey: n) ifTrue:
                                [aBlock value: (self at: n)].
                         n <- n - 1].
                ^ nil
|
	reversed                | newar i |
                newar <- Array new: (i <- self size).
                self do: [:x | newar at: i put: x. i <- i - 1].
                ^ self coerce: newar
|
	select: aBlock          
		^ self coerce:
		     (self inject: List new
                           into: [:x :y | (aBlock value: y)
                                           ifTrue: [x addLast: y]. x ] )
|
	sort
		^ self sort: [:x :y | x <= y]
|
	sort: sortBlock		| index temp newArray |
		newArray <- self asArray.
		(2 to: newArray size) do:
		  [ :highIndex | index <- highIndex - 1.
			[(index >= 1) and:
			   [(sortBlock value: (newArray at: index)
				      value: (newArray at: (index + 1))) not]]
			   whileTrue: [temp <- newArray at: index.
				       newArray at: index 
						put: (newArray at: index + 1).
				       newArray at: index + 1 put: temp.
				       index <- index - 1 ]].
		^ self coerce: newArray

|
	with: aSequenceableCollection do: aBlock        | arg1 arg2 |
                arg1 <- self first. arg2 <- aSequenceableCollection first.
                [ arg1 notNil] whileTrue:
                        [ aBlock value: arg1 value: arg2.
                          arg1 <- self next.
                          arg2 <- aSequenceableCollection next].
                ^ nil

]
