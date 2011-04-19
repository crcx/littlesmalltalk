Class Main
[
	main	| x gen |
		gen <- Primes new.
		(smalltalk time: [
		x <- gen first.
		[x < 300]
			whileTrue: [ x print. x <- gen next] ]) print
]
Class Primes
| prevPrimes lastPrime |
[
	first
		prevPrimes <- LinkedList new.
		prevPrimes add: (lastPrime <- 2).
		^ lastPrime
|
	next
		[lastPrime <- lastPrime + 1.
		 self testNumber: lastPrime]
			whileFalse.
		prevPrimes addLast: lastPrime.
		^ lastPrime
|
	testNumber: n
		prevPrimes do: [:x | 
			(x squared > n) ifTrue: [ ^ true ].
			(n \\ x = 0) ifTrue: [ ^ false ] ]
]
