Class CheckBook
| balance |
[
	new
		balance <- 0
|
	+ amount
		balance <- balance + amount.
		^ balance
|
	- amount
		balance <- balance - amount.
		^ balance
]

