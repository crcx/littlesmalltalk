Class Random
| seed |
[
        new
                seed <- 1
|
	randomize
		seed <- <TimeCounter>
|
        first
                ^ <RandomFloat (seed <- <Random seed > ) >
|
        next
                ^ <RandomFloat (seed <- <Random seed > ) >
|
	between: low and: high
		^ (self next * (high - low)) + low
|
	randInteger: limit
		^ (self next * limit) truncated + 1
|
	next: n			| newa |
		newa <- Array new: n.
		(1 to: n) do: [:x | newa at: x put: self next].
		^ newa
]
