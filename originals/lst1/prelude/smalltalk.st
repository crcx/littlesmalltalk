Class Smalltalk :Dictionary
[
	clearScreen
		<Clear>
|
	date
		^ <CurrentTime >
|
	debug: n
		^ <Debug 2 n>
|
	display
		^ <Debug 1 1>
|
	displayAssign
		^ <Debug 1 2>
|
	doPrimitive: primNumber withArguments: argArray
		^ <DoPrimitive primNumber argArray>
|
	getString
		^ <primitive 163>
|
	noDisplay
		^ <Debug 1 0>
|
	perform: aMessage withArguments: argArray
		^ <Perform argArray aMessage >
|
	sh: command
		^ <System command >
|
	time: aBlock		| start |
		start <- <TimeCounter>.
		aBlock value.
		^ <TimeCounter> - start
]
