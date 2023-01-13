Class Class
[
	edit
		<ClassEdit self>
|
	list
		<ClassList self>
|
 	new		| superclass newinstance |
		superclass <- <SuperClass self>.
		<RespondsToNew superclass >
			ifTrue: [newinstance <- superclass new ]. 
		newinstance <- <ClassNew self newinstance >.
		<RespondsTo self #new > 
			ifTrue: [newinstance <- newinstance new].
		^ newinstance
|
 	new: aValue 		| superclass newinstance |
		superclass <- <SuperClass self>.
		<RespondsToNew superclass >
			ifTrue: [newinstance <- superclass new ]. 
		newinstance <- <ClassNew self newinstance >.
		<RespondsTo self #new: > 
			ifTrue: [newinstance <- newinstance new: aValue ].
		^ newinstance
|
 	printString
		^ <ClassName self >
|
	respondsTo
		<PrintMessages self>
|
	respondsTo: aSymbol		| aClass |
		aClass <- self.
		[aClass notNil] whileTrue:
			[ <RespondsTo aClass aSymbol> ifTrue: [ ^ true ].
			 aClass <- aClass superClass ].
		^ false
|
	superClass
		^ <SuperClass self>

|
	variables
		^ <Variables self>
|
	view
		<ClassView self>
]
