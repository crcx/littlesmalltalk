Class Main
| i j k l |
[
	main
		i <- Test new.
		i set: 17.
		j <- Test new.
		j set: i.
		k <- j deepCopy.
		l <- j shallowCopy.
		i set: 12.
		k print.
		l print.
		i <- Test new.
		i set: 17.
		j <- #(2).
		j at: 1 put: i.
		k <- j deepCopy.
		l <- j shallowCopy.
		i set: 12.
		k print.
		l print.
]
Class Test
| var |
[
	printString
		^ 'test value ', var printString
|
	set: aVal
		var <- aVal
]

