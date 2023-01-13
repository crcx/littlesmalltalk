Class Main
[
	main
		(Primes new) do: [:x | x print]
]
Class Primes
| primeGenerator lastFactor |
[
	first
		primeGenerator <- 2 to: 300.
		lastFactor <- primeGenerator first.
		^ lastFactor
|
	next
		primeGenerator <- (Factor new ;
					remove: lastFactor
					from:   primeGenerator ).
		^ lastFactor <- primeGenerator next.
]

Class Factor
| myFactor generator |
[
	remove: factorValue from: generatorValue
		myFactor <- factorValue.
		generator <- generatorValue
|
	next		| possible |
		[(possible <- generator next) notNil]
			whileTrue:
				[(possible \\ myFactor ~= 0)
					ifTrue: [ ^ possible] ].
		^ nil
]


