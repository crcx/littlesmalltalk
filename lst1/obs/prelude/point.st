Class Point :Magnitude
| xvalue yvalue |
[
	< aPoint
		^ (xvalue < aPoint x) and: [yvalue < aPoint y]
|
	<= aPoint
		^ (xvalue <= aPoint x) and: [yvalue < aPoint y]
|
	>= aPoint
		^ (xvalue >= aPoint x) and: [yvalue >= aPoint y]
|
	= aPoint
		^ (xvalue = aPoint x) and: [yvalue = aPoint y]
|
	* scale
		^ (Point new x: (xvalue * scale)) y: (yvalue * scale)
|
	+ delta
		^ (Point new x: (xvalue + delta x)) y: (yvalue + delta y)
|
	- delta
		^ (Point new x: (xvalue - delta x)) y: (yvalue - delta y)
|
	/ scale
		^ (Point new x: (xvalue / scale)) y: (yvalue / scale)
|
	// scale
		^ (Point new x: (xvalue // scale)) y: (yvalue // scale)
|
	abs
		^ (Point new x: xvalue abs) y: (yvalue abs)
|
	asString
		^ xvalue asString , ' @ ' , (yvalue asString)
|
	dist: aPoint
		^ ((xvalue - aPoint x) squared + 
			(yvalue - aPoint y) squared) sqrt
|
	max: aPoint
		^ (Point new x: (xvalue max: aPoint x))
			y: (yvalue max: aPoint y)
|
	min: aPoint
		^ (Point new x: (xvalue min: aPoint x))
			y: (yvalue min: aPoint y)
|
	printString
		^ xvalue printString , ' @ ' , (yvalue printString)
|
	transpose
		^ (Point new x: yvalue) y: xvalue
|
	x
		^ xvalue
|
	x: aValue
		xvalue <- aValue
|
	x: xValue y: yValue
		xvalue <- xValue.
		yvalue <- yValue
|
	y
		^ yvalue
|
	y: aValue
		yvalue <- aValue
]
