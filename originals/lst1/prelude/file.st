Class File :SequenceableCollection
[
	modeCharacter
		<FileSetMode self 0>
|
	modeInteger
		<FileSetMode self 2>
|
	modeString
		<FileSetMode self 1>
|
	at: aPosition
		<FileSetPosition self aPosition>.
		^ self read
|
	at: aPosition put: anObject
		<FileSetPosition self aPosition>.
		^ self write: anObject
|
	currentKey
		^ <FileFindPosition self>
|
	first
		^ self at: 0
|
	next
		^ self read
|
	open: aName
		<FileOpen self aName 'r' >
|
	open: aName for: opType
		<FileOpen self aName opType >
|
	read
		^ <FileRead self>
|
	size
		^ <FileSize self>
|
	write: anObject
		^ <FileWrite self anObject>
]
