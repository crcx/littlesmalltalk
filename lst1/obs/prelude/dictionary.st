"
	Dictionarys are implemented using Points in order to reduce
	the number of classes in the standard prelude

	this also has the advantage of making the output appear in
		key @ value
	form
"
Class Dictionary :KeyedCollection
| hashTable currentBucket currentList |
[
	new
		hashTable <- Array new: 17
|
	hashNumber: aKey
		^ ( <HashNumber aKey> \\ hashTable size) + 1
|
	getList: aKey			| list bucketNumber |
		bucketNumber <- self hashNumber: aKey.
		list <- hashTable at: bucketNumber.
		(list isNil)
			ifTrue: [list <- List new.
				 hashTable at: bucketNumber put: list].
		^ list

|
	at: aKey put: anObject			| list anAssoc |

		list <- self getList: aKey.
		anAssoc <- self findAssociation: aKey inList: list.
		(anAssoc isNil)
			ifTrue:  [anAssoc <- (Point new x: aKey) y: anObject.
				  list add: anAssoc]
			ifFalse: [anAssoc y: anObject].
		^ anObject
|
	at: aKey ifAbsent: exceptionBlock	| list anAssoc | 

		list <- self getList: aKey.
		anAssoc <- self findAssociation: aKey inList: list.
		(anAssoc isNil)
			ifTrue: [^ exceptionBlock value].
		^ anAssoc y
|
	removeKey: aKey ifAbsent: exceptionBlock 	| list anAssoc|
		
		list <- self getList: aKey.
		anAssoc <- self findAssociation: aKey inList: list.
		(anAssoc isNil)
			ifTrue: [^ exceptionBlock value].
		^ ( list remove: anAssoc 
			 ifAbsent: [ ^ exceptionBlock value ] ) y
|
	findAssociation: aKey inList: linkedList

		linkedList do: 
			[:item | (item x = aKey) ifTrue: [^ item]].
		^ nil
|
	first				| item |

		(1 to: 17) do: 
			[:i | ((item <- self checkBucket: i) notNil)
						ifTrue: [ ^ item y] ] .
		^ nil
|
	next				| item |

		((item <- currentList next) notNil)
			ifTrue: [ ^ item y ].
		[currentBucket < 17] whileTrue:
			[currentBucket <- currentBucket + 1.
			 ((item <- self checkBucket: currentBucket) notNil)
				ifTrue: [ ^ item y ] ].
		^ nil
|
	printString
		^ (self inject: (self class printString) , ' ( '
			into: [ :aString :aValue |
				aString , self currentKey printString ,
					' @ ' , aValue printString , ' ' ]
			) , ')'
|
	currentKey	| clist|
		^ (currentList notNil) 
			ifTrue: [clist <- currentList current.
				 (clist notNil) ifTrue: [clist x]
			        ]
|
	checkBucket: bucketNumber

		((currentList <- hashTable at: 
				(currentBucket <- bucketNumber)) isNil)
			ifTrue: [ ^ nil ].
		^ currentList first
]
