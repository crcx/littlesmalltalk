Class Float :Number
[
	= aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatEquality self aNumber>]
			ifFalse: [super = aNumber]
|
	< aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatLessThan self aNumber>]
			ifFalse: [super < aNumber]
|
	> aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatGreaterThan self aNumber>]
			ifFalse: [ super > aNumber]
|
	+ aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatAddition self aNumber>]
			ifFalse: [super + aNumber]
|
	- aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatSubtraction self aNumber>]
			ifFalse: [super - aNumber]
|
	* aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatMultiplication self aNumber>]
			ifFalse: [super * aNumber]
|
	/ aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<FloatDivision self aNumber>]
			ifFalse: [super / aNumber]
|
	^ aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<Power self aNumber>]
			ifFalse: [super raisedTo: aNumber]
|
	arcCos
		^ Radian new: <ArcCos self>
|
	arcSin
		^ Radian new: <ArcSin self>
|
	arcTan
		^ Radian new: <ArcTan self>
|
	asFloat
		^ self
|
	asString
		^ <FloatToString self>
|
	ceiling
		^ <Ceiling self>
|
	coerce: aNumber
		^ aNumber asFloat
|
	exp
		^ <Exponent self>
|
	floor
		^ <Floor self>
|
	fractionPart
		^ <FractionalPart self>
|
	gamma
		^ <Gamma self>
|
	integerPart
		^ <IntegerPart self>
|
	ln
		^ <Log self>
|
	radix: aNumber
		^ <FloatRadixPrint self aNumber>
|
	rounded
		^ <Floor (self + 0.5)>
|
	sqrt
		^ <SquareRoot self>
|
	truncated
		^ (self < 0.0)  ifTrue:  [<Ceiling self>]
				ifFalse: [<Floor self>]
]
