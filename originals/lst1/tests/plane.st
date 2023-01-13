Class Main
[
	main	| i |
		i <- Plane new.
		i init.
		i fly.
		i bomb.
]
Class Plane
| plane bomb cloud |
[
	init
		plane <- Form new.
		plane row:  1 put: '                    '.
		plane row:  2 put: '   \                '.
		plane row:  3 put: '   |\       --------'.
		plane row:  4 put: '   |\\________/ /___|'.
		plane row:  5 put: '   | -- SU   / /    0'.
		plane row:  6 put: '   <--------/ /-----|'.
		plane row:  7 put: '         --------    '.
		plane row:  8 put: '            rm *'.
		bomb <- 'rm *'.
		cloud <- Form new.
		cloud row:  1 put: '   (  ) )'.
		cloud row:  2 put: ' (  *  )  )'.
		cloud row:  3 put: '( { } ) * )'.
		cloud row:  4 put: ' ( - ) ) )'.
		cloud row:  5 put: '   ( )'.
		^ plane
|
	bomb			| location bombLocation |
		smalltalk clearScreen.
		'FILES' printAt: 23 @ 60.
		cloud printAt: 1@30.
		location <- 1 @ 1.
		plane printAt: location.
		(1 to: 8) do: [:j |
			location <- j @ (j * 3).
			plane printAt: location].
		plane row: 8 put: '                 '.
		bombLocation <- (location x + 7) @ (location y + 10).
		(7 to: 2 by: -1) do: [:j |
			location <- j @ (location y + 3).
			plane printAt: location.
			'         ' printAt: bombLocation.
			bombLocation <- (bombLocation x + 1) @
					(bombLocation y + 3).
			bomb printAt: bombLocation ].
		'        ' printAt: bombLocation.
		'*****OPPS*****' printAt: 23 @ 55.
		' ' printAt: 21 @ 0.
|
	fly			| sky |
		smalltalk clearScreen.
		(10 to: 50 by: 5) do: [:i |
			sky <- Form new.
			sky placeForm: cloud at: 10 @ 40.
			sky overLayForm: plane at: 10 @ i.
			sky printAt: 1 @ 1
			].
		' ' printAt: 21 @ 0
|
	display
		plane printAt: 10@10 . '   ' print
]
