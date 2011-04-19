Class DiscreteProbability
	| randnum |
[
	initialize
		randnum <- Random new

|	next
		^ self inverseDistribution: randnum next

|	computeSample: m outOf: n	
		m > n ifTrue: [^ 0.0]
		^ n factorial / (n - m) factorial
]

Class Geometric	:DiscreteProbability
	| prob |	

[
	mean: m
		prob <- m

|	mean
		^ 1.0 / prob

|	variance
		^ (1.0 - prob) / prob * prob

|	density: x
		x > 0 ifTrue: [^prob * ((1.0-prob) raisedTo: x-1)]
		      ifFalse: [^1.0]

|	inverseDistribution: x
		^ (x ln / (1.0 - prob) ln) ceiling
]

Class Binomial	:DiscreteProbability
	| number prob |
[
	events: num mean: p
		(p between: 0.0 and: 1.0)
		   ifFalse: [self error: 'mean must be > 0'].
		number <- num.
		prob <- p

|	mean
		^ prob

|	variance
		^ prob * (1 - prob)

|	density: x
		(x between: 0.0 and number)
		   ifTrue: [^((self computeSample: x outOf: number)
			/ (self computeSample: x outOf: x))
			* (prob raisedTo: x) * ((1 - prob) raisedTo: number - x)]
		   ifFalse: [^0.0]

|	inverseDistribution: x
		x <= prob
			ifTrue: [^ 1]
			ifFalse: [^ 0]

|	next
	| t |
		t <- 0.
		number timesRepeat: [t <- t + super next].
		^ t
]
