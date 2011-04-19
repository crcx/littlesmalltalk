*
* Little Smalltalk, version 3
* Written by Tim Budd, Oregon State University, July 1988
*
*  methods for the unix front end - single process version
*
*
Methods Class 'all'
	addMethod	| m |
		m <- Method new; text: ''.
		(self doEdit: m)
			ifTrue: [ methods at: m name put: m ]
|
	doEdit: method
		" edit a method until it compiles correctly "
		[ method text: method text edit.
		  (method compileWithClass: self)
			ifTrue: [ ^ true ]
			ifFalse: [ smalltalk inquire: 'edit again (yn) ? ' ]
				] whileTrue.
		^ false
|
	display
		('Class name: ', name asString)  print.
		(superClass notNil)
			ifTrue: [ ('Superclass: ', superClass ) print ].
		'Instance Variables:' print.
		variables isNil
			ifTrue: [ 'no instance variables ' print ]
			ifFalse: [ variables display ].
		'Subclasses: ' print.
		self subClasses display
|
	editMethod: name	| m |
		m <- self methodNamed: name.
		(m notNil)
			ifTrue: [ self doEdit: m ]
			ifFalse: [ superClass notNil
				    ifTrue: [ superClass editMethod: name ]
				    ifFalse: [ 'no such method' print ] ]
|
	readInstanceVariables
		self variables:
			((smalltalk getPrompt: 'Instance Variables? ')
			words: [:x | x isAlphabetic ])
|
	readMethods
		[ smalltalk inquire: 'Add a method (yn) ? ' ]
			whileTrue: [ self addMethod ]
|
	viewMethod: methodName	| m |
		m <- self methodNamed: methodName.
		(m notNil) 
			ifTrue: [ m signature print.  m text print ]
			ifFalse: [ 'no such method' print ]
]
Methods Smalltalk 'all'
	getPrompt: aString
		stdout printNoReturn: aString.
		^ stdin getString
|
	inquire: aString	| response |
		response <- self getPrompt: aString.
		response isNil
			ifTrue: [ ^ false ].
		^ 'Yy' includes: (response at: 1 ifAbsent: [])
|
	echo
		" enable - disable echo input "
		echoInput <- echoInput not
]
Methods String 'all'
	edit	| file text |
		file <- File new; 
			scratchFile;
			open: 'w';
			print: self;
			close.
		(editor, ' ', file name) unixCommand.
		file open: 'r'.
		text <- file asString.
		file close; delete.
		^ text
|
	print
		stdout print: self
]
*
* initialization code
* this is executed once, by the initial image maker
*
*
Methods Smalltalk 'doit'
	error: aString
		" print a message, and remove current process "
		stderr print: aString.
		scheduler currentProcess; trace; terminate.
]
Methods Scheduler 'get commands'
	initialize	| string |
		<2>.
		string <- smalltalk getPrompt: '>	'.
		string isNil
			ifTrue: [ notdone <- false ]
			ifFalse: [ (string size > 0)
				ifTrue: [ 
					echoInput ifTrue:
						[ string print ].
					[ string value print ] fork ] ]
]
Methods UndefinedObject 'initial image'
	createGlobals	| aBlock |
		" create global variables in initial image "
		true <- True new.
		false <- False new.
		smalltalk <- Smalltalk new.
		files <- Array new: 15.
		stdin <- File new; name: 'stdin'; mode: 'r'; open.
		stdout <- File new; name: 'stdout'; mode: 'w'; open.
		stderr <- File new; name: 'stderr'; mode: 'w'; open.
		editor <- 'vi'.
		" create a dictionary of classes "
		classes <- Dictionary new.
		symbols binaryDo: [:x :y | 
			(y class == Class)
				ifTrue: [ classes at: x put: y ] ].
		scheduler <- Scheduler new.
		" create the initial system process "
		" note the delayed recursive call "
		aBlock <- [ files do: [:f | f notNil ifTrue: [ f open ]].
				   systemProcess <- aBlock newProcess.
				   echoInput <- false.
				   scheduler run ].
		systemProcess <- aBlock newProcess.
|
	initialize
		" initialize the initial object image "
		self createGlobals.
		File new;
			name: 'systemImage';
			open: 'w';
			saveImage;
			close.
]
