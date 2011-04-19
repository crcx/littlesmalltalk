Class Main
[
	main	| x gen |
		gen <- Primes new.
		(smalltalk time: [ x <- gen first.
		[x < 300]
			whileTrue: [ x print. x <- gen next] ] ) print.
]
Class Primes
| lastPrime |
[
	first
		^ lastPrime <- 2
|
	next
		[lastPrime <- lastPrime + 1.
		 self testNumber: lastPrime]
			whileFalse.
		^ lastPrime
|
	testNumber: n
		(Primes new) do: [:x | 
			(x squared > n) ifTrue: [ ^ true ].
			(n \\ x = 0) ifTrue: [ ^ false ] ]
]
