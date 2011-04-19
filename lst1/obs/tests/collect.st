Class Main
| i |
[
	main
		self test1.
		self test2.
		self test3
|
	test1		| j |
		(i <- 'example') print.
		i size print.
		i asArray print.
		(i occurrencesOf: $e) print.
		i asBag print.
		(j <- i asSet) print.
		j asString reversed print.
		i asDictionary print.
		(j <- i asList) print.
		j addFirst: 2 / 3.
		j addAllLast: (12.5 to: 15 by: 0.75).
		j print.
		j removeLast print.
		(j , #($a 7) ) print.
		(i reject: [:x | x isVowel] ) print.
		(i copyWithout: $e) print.
		i sort print.
		(i sort: [:x :y | y < x]) print.
		i keys print.
		i values print.
		(i atAll: (1 to: 7 by: 2) put: $x) print
|
	test2			| j |
		i <- (1 to: 6) asBag print.
		i size print.
		(i select: [:x | (x \\ 2) strictlyPositive] ) print.
		(j <- (i collect: [:x | x \\ 3]) asSet ) print.
		j size print
|
	test3
		('bead' at: 1 put: $r) print
]
