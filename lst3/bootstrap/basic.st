*
* Little Smalltalk, version 3
* basic methods needed for execution, including
*	object creation
*	block creation, execution and return
*
Class Object
Class Block Object context argCount argLoc bytePointer
Class Boolean Object
Class    True Boolean
Class    False Boolean
Class Class Object name instanceSize methods superClass variables
Class Context Object linkLocation method arguments temporaries
Class Integer Object
Class Method Object text message bytecodes literals stackSize temporarySize class watch
Class Smalltalk Object
Class Switch Object const notdone
Class Symbol Object
Class UndefinedObject Object
*
Methods Block 'initialization'
	checkArgumentCount: count
		^ (argCount = count)
			ifTrue: [ true ]
			ifFalse: [ smalltalk error:
				'wrong number of arguments passed to block'.
				false ]
|
	blockContext: ctx
		context <- ctx
|
	value
		^ (self checkArgumentCount: 0)
			ifTrue: [ context returnToBlock: bytePointer ]
|
	value: x
		^ (self checkArgumentCount:  1)
			ifTrue: [ context at: argLoc put: x.
				  context returnToBlock: bytePointer ]
|
	value: x value: y
		^ (self checkArgumentCount: 2)
			ifTrue: [ context at: argLoc put: x.
				  context at: argLoc + 1 put: y.
				  context returnToBlock: bytePointer ]
|
	value: x value: y value: z
		^ (self checkArgumentCount:  3)
			ifTrue: [ context at: argLoc put: x.
				  context at: argLoc + 1 put: y.
				  context at: argLoc + 2 put: z.
				  context returnToBlock: bytePointer ]
|
	whileTrue: aBlock
		( self value ) ifTrue:
			[ aBlock value. 
				self whileTrue: aBlock ]
|
	whileTrue
		self whileTrue: []
|
	whileFalse: aBlock
		[ self value not ] whileTrue: aBlock
]
Methods Boolean 'all'
	ifTrue: trueBlock
		^ self ifTrue: trueBlock ifFalse: []
|
	ifFalse: falseBlock
		^ self ifTrue: [] ifFalse: falseBlock
|
	ifFalse: falseBlock ifTrue: trueBlock
		^ self ifTrue: trueBlock
			ifFalse: falseBlock
|
	and: aBlock
		^ self ifTrue: aBlock ifFalse: [ false ]
|
	or: aBlock
		^ self ifTrue: [ true ] ifFalse: aBlock
]
Methods Class 'creation'
	new		| newObject |
		newObject <- self new: instanceSize.
		^ (self == Class)
			ifTrue: [ newObject initialize ]
			ifFalse: [ newObject new ]
|
	new: size	" hack out block the right size and class "
		"create a new block, set its class"
		^ < 22 < 58 size > self >
|
	addSubClass: aSymbol instanceVariableNames: aString	| newClass |
		newClass <- Class new; name: aSymbol; superClass: self;
				variables: 
				  (aString words: [:x | x isAlphabetic ]).
		aSymbol assign: newClass.
		classes at: aSymbol put: newClass
|
	initialize
		superClass <- Object.
		instanceSize <- 0.
		methods <- Dictionary new
|
	methods
		^ methods
|
	methodNamed: name
		(methods includesKey: name)
			ifTrue: [ ^ methods at: name ].
		(superClass notNil)
			ifTrue: [ ^ superClass methodNamed: name ].
		^ nil
|
	name
		^ name
|
	name: aString
		name <- aString
|
	instanceSize
		^ instanceSize
|
	printString
		^ name asString
|
	respondsTo	| theSet |
		theSet <- Dictionary new.
		self upSuperclassChain: 
			[:x | theSet addAll: x methods ].
		^ theSet
|
	subClasses
		^ classes inject: List new
			into: [:x :y | (y superClass == self)
						ifTrue: [ x add: y]. x ]
|
	superClass
		^ superClass
|
	superClass: aClass
		superClass <- aClass
|
	upSuperclassChain: aBlock
		aBlock value: self.
		(superClass notNil)
			ifTrue: [ superClass upSuperclassChain: aBlock ]
|
	variables
		^ variables
|
	variables: nameArray
		variables <- nameArray.
		instanceSize <- superClass instanceSize + nameArray size
|
	watch: name	| m |
		m <- self methodNamed: name.
		(m notNil) 
			ifTrue: [ ^ m watch: 
				[:a | ('executing ', name) print. a print] ]
			ifFalse: [ ^ 'no such method' ]
]
Methods Context 'all'
	at: key put: value
		temporaries at: key put: value
|
	method: m
		method <- m
|
	arguments: a
		arguments <- a
|
	temporaries: t
		temporaries <- t
|
	returnToBlock: bytePtr
		" change the location we will return to, to execute a block"
		<28 self bytePtr>
|
	copy
		^ super copy temporaries: temporaries copy
|
	blockReturn
		<18 self>
			ifFalse: [ ^ smalltalk error: 
				'incorrect context for block return']
]
Methods False 'all'
	ifTrue: trueBlock ifFalse: falseBlock
		^ falseBlock value
|
	not
		^ true
|
	xor: aBoolean
		^ aBoolean
|
	printString
		^ 'false'
]
Methods Method 'all'
	compileWithClass: aClass
		^ <39 aClass text self>
|
	name
		^ message
|
	message: aSymbol
		message <- aSymbol
|
	printString
		^ message asString
|
	signature
		^ class asString,' ', message asString
|
	text
		^ (text notNil)
			ifTrue: [ text ]
			ifFalse: [ 'text not saved']
|
	text: aString
		text <- aString
|
	display
		('Method ', message) print.
		'text' print.
		text print.
		'literals' print.
		literals print.
		'bytecodes' print.
		bytecodes class print.
		bytecodes do: [:x |
			(x printString, ' ', (x quo: 16), ' ', (x rem: 16))
				print ]
|
	executeWith: arguments
		^ ( Context new ; method: self ; 
			temporaries: ( Array new: temporarySize) ;
			arguments: arguments )
		   returnToBlock: 1
|
	watch: aBlock
		watch <- aBlock
|
	watchWith: arguments
		" note that we are being watched "
		text print.
		watch value: arguments.
		^ self executeWith: arguments
]
Methods Object 'all'
	assign: name value: val
		^ name assign: val
|
	== aValue
		^ <21 self aValue>
|
	~~ aValue
		^ (self == aValue) not
|
	= aValue
		^ self == aValue
|
	asString
		^ self printString
|
	basicAt: index
		^ <25 self index>
|
	basicAt: index put: value
		^ <31 self index value>
|
	basicSize
		^ <12 self>
|
	class
		^ <11 self>
|
	copy
		^ self shallowCopy
|
	deepCopy	| newObj |
		newObj <- self class new.
		(1 to: self basicSize) do: 
			[:i | newObj basicAt: i put: (self basicAt: i) copy].
		^ newObj
|
	display
		('(Class ', self class, ') ' , self printString ) print
|
	hash
		^ <13 self>
|
	isMemberOf: aClass
		^ self class == aClass
|
	isNil
		^ false
|
	isKindOf: aClass
		self class upSuperclassChain:
			[:x | (x == aClass) ifTrue: [ ^ true ] ].
		^ false
|
	new
		" default initialization protocol"
		^ self
|
	notNil
		^ true
|
	print
		self printString print 
|
	printString
		^ self class printString
|
	respondsTo: message
		self class upSuperclassChain: 
			[:c | (c methodNamed: message) notNil
					ifTrue: [ ^ true ]].
		^ false
|
	shallowCopy	| newObj |
		newObj <- self class new.
		(1 to: self basicSize) do: 
			[:i | newObj basicAt: i put: (self basicAt: i) ].
		^ newObj
]
Methods Smalltalk 'all'
	perform: message withArguments: args ifError: aBlock	
			| receiver method |
		receiver <- args at: 1 ifAbsent: [ ^ aBlock value ].
		method <- receiver class methodNamed: message.
		^ method notNil 
			ifTrue: [ method executeWith: args ]
			ifFalse: aBlock
|
	perform: message withArguments: args
		^ self perform: message withArguments: args
			ifError: [ self error: 'cant perform' ]
|
	watch
		^ <5>
]
Methods True 'all'
	ifTrue: trueBlock ifFalse: falseBlock
		^ trueBlock value
|
	not
		^ false
|
	xor: aBoolean
		^ aBoolean not
|
	printString
		^ 'true'
]
Methods Switch 'all'
	key: value
		const <- value.
		notdone <- true.
|
	ifMatch: key do: block
		(notdone and: [ const = key ])
			ifTrue: [ notdone <- false. block value ]
|
	else: block
		notdone ifTrue: [ notdone <- false. block value ]
]
Methods Symbol 'all'
    	apply: args
		^ self apply: args ifError: [ 'does not apply' ]
|
    	apply: args ifError: aBlock
		^ smalltalk perform: self withArguments: args ifError: aBlock
|
	assign: value
		<27 self value>. ^ value
|
	asString
		" catenation makes string and copy automatically "
		^ <24 self ''>
|
	copy
		^ self
|
	printString
		^ '#' , self asString
|
	respondsTo
		^ classes inject: Set new
			into: [:x :y | ((y methodNamed: self) notNil)
						ifTrue: [ x add: y]. x]
|
	value
		^ <87 self>
]
Methods UndefinedObject 'all'
	isNil
		^ true
|
	notNil
		^ false
|
	printString
		^ 'nil'
]
Methods Object 'errors'
	message: m notRecognizedWithArguments: a
		^ smalltalk error: 'not recognized ', (self class printString),
			' ', (m printString)
]
