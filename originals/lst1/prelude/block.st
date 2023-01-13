"
	Class Block.

	Note how whileTrue: and whileFalse: depend upon the parser
	optimizing the loops into control flow, rather than message
	passing.  If this were not the case, whileTrue: would have to
	be implemented using recursion, as follows:

	whileTrue: aBlock
		(self value) ifFalse: [^nil].
		aBlock value.
		^ self whileTrue: aBlock
"
Class Block
[
	newProcess
		^ <NewProcess  self>
|
	newProcessWith: argumentArray
		^ <NewProcess  self argumentArray>
|
	fork
		self newProcess resume.
		^ nil
|
	forkWith: argumentArray
		(self newProcessWith: argumentArray) resume.
		^ nil
|
	whileTrue
		^ [self value ] whileTrue: []
|
	whileTrue: aBlock
		^ [ self value ] whileTrue: [ aBlock value ]
|
	whileFalse
		^ [ self value ] whileFalse: []
|
	whileFalse: aBlock
		^ [ self value ] whileFalse: [ aBlock value ]
|
 	value
		<BlockExecute  0>
|
	value: a
		<BlockExecute  1>
|
	value: a value: b
		<BlockExecute  2>
|
	value: a value: b value: c
		<BlockExecute  3>
|
	value: a value: b value: c value: d
		<BlockExecute  4>
|
	value: a value: b value: c value: d value: e
		<BlockExecute  5>
]
