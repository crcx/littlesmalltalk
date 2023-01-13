Class Main
[
	main			| i |
		((2 < 3) ifTrue: ['correct']) print.
		(2 < 3) ifTrue: ['correct' print ].
		i <- 1.
		[i < 3] whileTrue: [i <- i + 1].
		(i >= 3) ifTrue: ['correct' print]
]

