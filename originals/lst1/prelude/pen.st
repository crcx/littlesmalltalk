"
	the following use the primitives interfacing to the plot(3)
	routines
"

" pen - a simple drawing instrument "
Class Pen
| x y up direction |
[
	new
		self up.
		self direction: 0.0.
		self goTo: 100 @ 100.
|
	circleRadius: rad
		<primitive 174 x y rad>
|
	direction
		^ direction
|
	direction: radians
		direction <- radians
|
	down
		up <- false.
|
	erase	
		<primitive 170>
|
	extent: lowerLeft to: upperRight
		<primitive 176 (lowerLeft x) (lowerLeft y)
			(upperRight x) (upperRight y)>
|
	go: anAmount		| newx newy |
		newx <- (direction radians sin * anAmount) rounded + x.
		newy <- (direction radians cos * anAmount) rounded + y.
		self goTo: newx @ newy
|
	goTo: aPoint
		up ifFalse: [<primitive 177 x y (aPoint x) (aPoint y)>].
		x <- aPoint x.
		y <- aPoint y.
|
	isUp
		^ up
|
	location
		^ x @ y
|
	turn: radians
		direction <- direction + radians
|
	up
		up <- true.
]

" penSave - a way to save the drawings made by a pen "
Class PenSave	:Pen
| saveForm |
[
	setForm: aForm
		saveForm <- aForm
|
	goTo: aPoint
		(self isUp)
			ifTrue: [ super goTo: aPoint ]
			ifFalse: [ saveForm add: self location to: aPoint.
					self up.
					super goTo: aPoint.
					self down ]
]

" Form - a collection of lines "
Class Form
| lines |
[
	new
		lines <- Bag new
|
	add: startingPoint to: endingPoint
		lines add: ( Point new ;
				x: startingPoint ;
				y: endingPoint )
|
	with: aPen displayAt: location	| xOffset yOffset sPoint ePoint |
		xOffset <- location x.
		yOffset <- location y.
		lines do: [:pair |
			sPoint <- pair x.
			ePoint <- pair y.
			aPen up.
			aPen goTo: 
				(sPoint x + xOffset) @ (sPoint y + yOffset).
			aPen down.
			aPen goTo: 
				(ePoint x + xOffset) @ (ePoint y + yOffset).
			].
]
"
	pen show - show off some of the capabilities of pens.
"
Class PenShow
| bic |
[
	withPen: aPen
		bic <- aPen
|
	poly: nSides length: length

		nSides timesRepeat:
			[ bic go: length ;
				turn: 2 pi / nSides ]
|
	spiral: n angle: a
		( 1 to: n ) do:
			[:i | bic go: i ; turn: a]
]
