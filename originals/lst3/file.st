*
* Little Smalltalk, version 3
* Written by Tim Budd, Oregon State University, January 1989
*
*  methods for dealing with files
*   these are independent of the stdio package, 
*    work in all systems
*
*  a global variable "files" maintains all currently opened files
*  these are reopened on system initialization time
*
Class File Object name number mode
*
Methods File 'all'
	asString	| text line |
		text <- ''.
		[ (line <- self getString) notNil ]
			whileTrue: [ text <- text , line ].
		^ text
|
	name: string
		name <- string
|
	name
		^ name
|
	getNumber
		" get a file number - called only by open"
		(1 to: 15) do: [:i | (files at: i) isNil
			ifTrue: [ files at: i put: self. number <- i. ^ nil]]
|
	open: m
		self mode: m.
		self open
|
	mode: m
		mode <- m
|
	open
		number notNil ifTrue: [ self close ].
		self getNumber.
		<120 number name mode> isNil
			ifTrue: [ smalltalk error: 
				 'open failed: ', name. ^ false].
		^ true
|
	close
		" close file, take entry out of global variable "
		number isNil ifTrue: [ ^ nil ].
		files at: number put: nil.
		<121 number>.
		number <- nil.
|
	fileIn
		" read code into the system "
		(number notNil) ifTrue: [<123 number>]
|
	fileIn: name
		self name: name.
		self open: 'r'.
		self fileIn.
		self close.
|
	getString
		^ (number notNil)
			ifTrue: [<125 number>]
|
	print: aString
		(number notNil)
			ifTrue: [<129 number aString>]
			ifFalse: [smalltalk error: 'file not open']
|
	printNoReturn: aString
		(number notNil)
			ifTrue: [<128 number aString>]
			ifFalse: [smalltalk error: 'file not open']
|
	readUntil: conditionBlock doing: actionBlock	| line |
		[ line <- self getString. line notNil]
			whileTrue: [ (conditionBlock value: line)
					ifTrue: [ ^ line ].
					actionBlock value: line ].
		^ nil
|
	saveImage
		" subtle problem - when we read in image don't want
		  image file to be open for writing, so we remove it's
		  number from files array temporarily "
		(number notNil)
			ifTrue: [ files at: number put: nil. 
				  <127 number>.
				  files at: number put: self]
			ifFalse: [smalltalk error: 'saveImage: file not open']
]
Methods File 'all'
	scratchFile
		name <- 'junk.tmp'
|
	delete
		('rm ', name) unixCommand
]
Methods Smalltalk 'image'
	saveImage
		self saveImage: (self getPrompt: 'type image name: ').
		^ 'done'
|
	saveImage: name	
		scheduler critical: [
			" first get rid of our own process "
			scheduler removeProcess: scheduler currentProcess.
		File new;
			name: name;
			open: 'w';
			saveImage;
			close ]
]
Methods Class 'all'
	fileOut		| f |
		" file out whole class on class.st "
		f <- File new; name: (name asString,'.st'); mode: 'w'; open.
		self fileOutOn: f.
		f close.
|
	fileOutOn: aFile
		" file out class description "
		aFile printNoReturn: 
			'Class ', name , ' ', superClass.
		(variables notNil) ifTrue: [
			variables do: [:v | aFile printNoReturn: ' ', v ]].
		aFile print: ''.
		self fileOutMethodsOn: aFile.
|
	fileOutMethodsOn: aFile
		" file out all methods "
		methods do: [:m |
			aFile print: 'Methods ', name.
			aFile print: m text.
			aFile print: ']'
			]
]
