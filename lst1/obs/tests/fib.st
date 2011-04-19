Class Fib :Generator
| lastNumber  nextToLastNumber |
[
	first
    		nextToLastNumber <- 0.
    		^ lastNumber <- 1
|
  	next			| sum |
    		sum <- nextToLastNumber + lastNumber.
    		nextToLastNumber <- lastNumber.
    		^ lastNumber <- sum
]
