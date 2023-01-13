Class ByteArray :ArrayedCollection
[
	new: size
		^ <NewByteArray size>
|
	at: index
		^ <ByteArrayAt self index>
|
	at: index put: value
		<ByteArrayAtPut self index value>
|
	printString	| str |
		str <- '#[ '.
		(1 to: self size)
			do: [:i | str <- str , (self at: i) printString , ' '].
		^ str , ']'
|
	size
		^ <ByteArraySize self>
]

