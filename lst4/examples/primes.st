!Number
isPrime
	((self rem: 2) = 0) ifTrue: [ ^ false ].
	(3 to: (self quo: 2) by: 2) do: [:cand|
		((self rem: cand) = 0) ifTrue: [ ^ false ]
	].
	^ true
!
=Number
primes: n | cand count |
	2 print.
	cand <- 3.
	count <- 0.
	[count < n] whileTrue: [
		[cand isPrime not] whileTrue: [
			cand <- cand + 2
		].
		' ' print. cand print.
		count <- count + 1.
		cand <- cand + 2
	].
	Char newline print
!
