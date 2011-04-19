Class Main
[
	main		| f g |
		f <- File new ; open: 'file.st'.
		g <- File new ; open: 'foo' for: 'w'.
		f do: [:x | g write: x reversed].
		g <- File new ; open: 'foo' for: 'r'.
		g do: [:x | x print].
		f modeCharacter.
		f first print.
		10 timesRepeat: [ f next print ].
		(f at: 2) print.
		f currentKey print.
		f size print.
]

