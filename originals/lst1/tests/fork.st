Class Main
[
	loop1
		10 timesRepeat: [17 print]
|
	loop2
		10 timesRepeat: [23 print]
|
	main
		[self loop1] fork.
		self loop2
]

