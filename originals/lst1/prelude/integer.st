Class Integer :Number
[
	= aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [ <IntegerEquality self aNumber> ]
			ifFalse: [ super = aNumber ]
|
	> aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [ <IntegerGreaterThan self aNumber> ]
			ifFalse: [ super > aNumber ]
|
	< aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [ <IntegerLessThan self aNumber> ]
			ifFalse: [ super < aNumber ]
|
	+ aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [ <IntegerAddition self aNumber> ]
			ifFalse: [ super + aNumber ]
|
	- aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<IntegerSubtraction self aNumber>]
			ifFalse: [ super - aNumber ]
|
	* aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<IntegerMultiplication self aNumber>]
			ifFalse: [ super * aNumber ]
|
	/ aNumber
		^ self asFloat / aNumber
|
	// aNumber
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [<IntegerSlash self aNumber>]
			ifFalse: [self opError]
|
	\\ aNumber			| i |
		^ <SameTypeOfObject self aNumber>
			ifTrue:  [i <- self * ( (self < 0)
						ifTrue:  [ -1 ] 
						ifFalse: [ 1 ] ).
				  i rem: aNumber]
			ifFalse:  [self opError]
|
	allMask: anInteger
		^ anInteger = <BitAND self anInteger>
|
	anyMask: anInteger
		^ 0 ~= <BitAND self anInteger>
|
	asCharacter
		^ <IntegerToCharacter self>
|
	asFloat
		^ <IntegerToFloat self >
|
	asInteger
		^ self
|
	asString
		^ <IntegerToString self>
|
	bitAnd: anInteger
		^ <BitAND self anInteger>
|
	bitAt: anInteger
		^ <BitAt self anInteger>
|
	bitInvert
		^ <BitInverse self>
|
	bitOr: anInteger
		^ <BitOR self anInteger>
|
	bitShift: anInteger
		^ <BitShift self anInteger>
|
	bitXor: anInteger
		^ <BitXOR self anInteger>
|
	even
		^ (self rem: 2) = 0
|
	factorial
		^ <Factorial self>
|
	gcd: anInteger
		^ <SameTypeOfObject self anInteger>
			ifTrue:  [<GCD self anInteger>]
			ifFalse: [self opError]
|
	highBit
		^ <HighBit self>
|
	lcm: anInteger
		^ <SameTypeOfObject self anInteger>
			ifTrue:  [self * anInteger quo: 
					(self gcd: anInteger)]
			ifFalse: [self opError]
|
	noMask: anInteger
		^ 0 = (self bitAnd: anInteger)
|
	odd
		^ (self rem: 2) ~= 0
|
	quo: anInteger
		^ <SameTypeOfObject self anInteger>
			ifTrue:  [<IntegerDivision self anInteger>]
			ifFalse: [self opError]
|
	radix: aNumber
		^ <RadixPrint self aNumber>
|
	rem: anInteger
		^ <SameTypeOfObject self anInteger>
			ifTrue:  [<IntegerMod self anInteger>]
			ifFalse: [self opError]
|
	timesRepeat: aBlock		| i |
		i <- 0.
		[i < self] whileTrue:
			[aBlock value. i <- i + 1]
]
