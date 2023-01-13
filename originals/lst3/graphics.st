*
*	graphics support routines
*	used by the standard windows version
*	requires basic routines
*	written by tim budd, January 1989
*
Class Point Magnitude x y
Class Rectangle Object top left bottom right
Class Circle Object center radius
*
Methods Number 'point'
	@ v
		^ Point new; x: self; y: v
]
Methods Object 'testing'
	isPoint
		^ false
]
Methods Point 'all'
	<= aPoint
		^ (x <= aPoint x) and: [y <= aPoint y]
|
	= aPoint
		aPoint isPoint
			ifTrue: [ ^ (x = aPoint x) and: [y = aPoint y] ]
			ifFalse: [ ^ false ]
|
	+ v
		v isPoint
			ifTrue: [ ^ Point new; x: x + v x; y: y + v y ]
			ifFalse: [ ^ Point new; x: x + v; y: y + v]
|
	- v
		v isPoint
			ifTrue: [ ^ Point new; x: x - v x; y: y - v y ]
			ifFalse: [ ^ Point new; x: x - v; y: y - v]
|
	* v
		^ Point new; x: x * v; y: y * v
|
	printString
		^ x printString , '@', y printString
|
	isPoint
		^ true
|
	size: aPoint
		^ self to: self + aPoint
|
	to: aPoint
		" return a rectangle with the given dimensions "
		^ Rectangle new; upperLeft: self; bottomRight: aPoint
|
	radius: n
		^ Circle new; center: self; radius: n
|
	x: v
		x <- v
|
	y: v
		y <- v
|
	x
		^ x
|
	y
		^ y
]
Methods Rectangle 'all'
	+ v
		^ Rectangle new; bottomRight: right@bottom + v;
			upperLeft: left@top + v
|
	- v
		^ Rectangle new; bottomRight: right@bottom - v;
			upperLeft: left@top - v
|
	bottomRight: aPoint
		right <- aPoint x.
		bottom <- aPoint y.
|
	contains: aPoint
		^ aPoint between: left@top and: right@bottom
|
	upperLeft: aPoint
		left <- aPoint x.
		top <- aPoint y.
|
	inset: aPoint
		self upperLeft: left@top + aPoint.
		self bottomRight: right@bottom - aPoint
|
	printString
		^ ((left@top) printString) , ':', ((right@bottom) printString)
]
Methods Circle 'all'
	center: c
		center <- c
|
	radius: r
		radius <- r
]
