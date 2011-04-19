Class Acl
| vara |
[
	new
		vara <- 'correct'
|
	printa
		vara print
]

Class Bcl :Acl
| varb |
[
	new
		varb <- 'correct'
|
	printb
		varb print
]

Class Main
[
	main		| i |
		i <- Bcl new .
		i printb .
		i printa
]
