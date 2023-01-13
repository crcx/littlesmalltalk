Class Main
[
	main
		(2 < 3) ifTrue: ['correct-1' print].
		((2 < 3) ifTrue: ['correct-2']) print.
		[:x | x print] value: 'correct-3' .
		((2 < 3) or: [3 < 4]) ifTrue: ['correct-4' print].
		((2 > 3) or: [3 < 4]) ifTrue: ['correct-5' print].
		((2 < 3) and: [3 < 4]) ifTrue: ['correct-6' print].
		((2 > 3) and: [3 < 4]) ifFalse: ['correct-7' print].
		self test1 print
|
	test1
		self test2: [^ 'correct-8'].
		'should not print' print
|
	test2: aBlock
		self test3: aBlock.
		'should not print' print
|
	test3: bBlock
		bBlock value.
		'should not print' print
]
