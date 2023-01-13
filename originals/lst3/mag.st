*
* Little Smalltalk, version 3
* Written by Tim Budd, Oregon State University, July 1988
*
* Classes dealing with objects having Magnitude
*
Class Magnitude Object
Class    Char Magnitude value
Class    Number Magnitude
Class       Integer Number
Class           LongInteger Integer negative digits
Class       Fraction Number top bottom
Class       Float Number
Class Random Object
*
Methods Object 'magnitude'
	isNumber
		^ false
|
	isFloat
		^ false
|
	isFraction
		^ false
|
	isInteger
		^ false
|
	isLongInteger
		^ false
|
	isShortInteger
		^ false
]
Methods Char 'all'
	< aValue
		" can only compare characters to characters "
		^ aValue isChar
			ifTrue: [ value < aValue asInteger ]
			ifFalse: [ smalltalk error: 'char compared to nonchar']
|
	== aValue
		^ aValue isChar
			ifTrue: [ value = aValue asInteger ]
			ifFalse: [ false ]
|
	asInteger
		^ value
|
	asString
		" make ourselves into a string "
		^ ' ' copy; at: 1 put: self
|
	digitValue
		" return an integer representing our value "
		self isDigit ifTrue: [ ^ value - $0 asInteger ].
		self isUppercase ifTrue: [ ^ value - $A asInteger + 10 ].
		^ smalltalk error: 'illegal conversion, char to digit'
|
	isAlphabetic
		^ (self isLowercase) or: [ self isUppercase ]
|
	isAlphaNumeric
		^ (self isAlphabetic) or: [ self isDigit ]
|
	isBlank
		^ value = $   " blank char "
|
	isChar
		^ true
|
	isDigit
		^ value between: $0 asInteger and: $9 asInteger
|
	isLowercase
		^ value between: $a asInteger and: $z asInteger
|
	isUppercase
		^ value between: $A asInteger and: $Z asInteger
|
	value: aValue		" private - used for initialization "
		value <- aValue
|
	printString
		^ '$', self asString
]
Methods Fraction 'all'
	= f
		f isFraction
			ifTrue: [ ^ (top = f top) and: [ bottom = f bottom ] ]
			ifFalse: [ ^ super = f ]
|
	< f
		f isFraction
			ifTrue: [ ^ (top * f bottom) < (bottom * f top) ]
			ifFalse:[ ^ super < f ]
|
	+ f
		f isFraction
			ifTrue: [ ^ ((top * f bottom) + (bottom * f top)) /
					(bottom * f bottom) ]
			ifFalse:[ ^ super + f ]
|
	- f
		f isFraction
			ifTrue: [ ^ ((top * f bottom) - (bottom * f top)) /
					(bottom * f bottom) ]
			ifFalse:[ ^ super - f ]
|
	* f
		f isFraction
			ifTrue: [ ^ (top * f top) / (bottom * f bottom) ]
			ifFalse: [ ^ super * f ]
|
	/ f
		^ self * f reciprocal
|
	abs
		^ top abs / bottom
|
	asFloat
		" convert to a floating point number "

		^ top asFloat / bottom asFloat
|
	truncated
		" convert to an integer rounded towards zero "
		^ top quo: bottom
|
	bottom
		^ bottom
|
	coerce: x
		" coerce a value into being a fraction "

		^ x asFraction
|
	generality
		" generality value - used in mixed type arithmetic "
		^ 5
|
	isFraction
		^ true
|
	ln
		^ (top ln) - (bottom ln)
|
	raisedTo: x
		^ (top raisedTo: x) / (bottom raisedTo: x)
|
	reciprocal
		^ bottom / top
|
	top
		^ top
|
	with: t over: b
		" initialization "

		top <- t.
		bottom <- b
|
	printString
		^ top printString, '/', bottom printString
]
Methods Float 'all'
	+ value
		^ value isFloat
			ifTrue: [ <110 self value> " floating add " ]
			ifFalse: [ super + value ]
|
	- value
		^ value isFloat
			ifTrue: [ <111 self value> " floating subtract " ]
			ifFalse: [ super - value ]
|
	< value
		^ value isFloat
			ifTrue: [ <112 self value> " floating comparison " ]
			ifFalse: [ super < value ]
|
	= value
		^ value isFloat
			ifTrue: [ <116 self value> ]
			ifFalse: [ super = value ]
|
	* value
		^ value isFloat
			ifTrue: [ <118 self value> ]
			ifFalse: [ super * value ]
|
	/ value	
		^ value isFloat
			ifTrue: [ (value = 0.0)
					ifTrue: [ smalltalk error:
						'float division by zero' ]
					ifFalse: [ <119 self value> ]]
			ifFalse: [ super / value ]
|
	isFloat
		^ true
|
	coerce: value
		" convert the value into a floating point number "
		^ value asFloat
|
	exp
		" return e raised to self "
		^ <103 self>
|
	generality
		" our numerical generality - used for mixed mode arithmetic"
		^ 7
|
	integerPart	| i j |
		i <- <106 self>. j <- i basicAt: 2. i <- i basicAt: 1.
		j < 0 ifTrue: [ ^ 0 ] ifFalse: [ ^ i * (2 raisedTo: j)]
|
	ln
		" natural log of self "
		^ <102 self>
|
	new
		^ smalltalk error: 'cannot create floats with new'
|
	printString
		^ <101 self>
|
	quo: value
		^ (self / value) truncated
|
	rounded
		^ (self + 0.5) floor
|
	truncated	| result f i |
		" truncate to an integer rounded towards zero"
		f <- self. result <- 0.
		[ i <- f integerPart. i > 0] whileTrue:
			[ result <- result + i. f <- f - i ].
		^ result
]
Methods Integer 'all'
	+ value		| r |
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ r <- <60 self value>.
				  "primitive will return nil on overflow"
				  r notNil ifTrue: [ r ]
				ifFalse: [ self asLongInteger + value asLongInteger ]]
			ifFalse: [ super + value ]
|
	- value		| r |
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ r <- <61 self value>.
				  "primitive will return nil on overflow"
				r notNil ifTrue: [ r ]
				ifFalse: [ self asLongInteger - value asLongInteger ]]
			ifFalse: [ super - value ]
|
	< value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ <62 self value> ]
			ifFalse: [ super < value ]
|
	> value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ <63 self value> ]
			ifFalse: [ super > value ]
|
	= value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ self == value ]
			ifFalse: [ super = value ]
|
	* value		| r |
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ r <- <68 self value>.
				  "primitive will return nil on overflow"
				  r notNil ifTrue: [ r ]
				  ifFalse: [ self asLongInteger * value asLongInteger ]]
			ifFalse: [ super * value ]
|
	/ value		| t b |
		value = 0 ifTrue: [ ^ smalltalk error: 'division by zero'].

		value isInteger
			ifTrue: [ b <- self gcd: value .
				  t <- self quo: b.
				  b <- value quo: b.
				  b negative
					ifTrue: [ t <- t negated. 
						  b <- b negated ].
				  (b = 1) ifTrue: [ ^ t ].
				  ^ Fraction new; with: t over: b ]
			ifFalse: [ ^ super / value ]
|
	, value
		" used to make long integer constants "
		^ self * 1000 + value
|
	allMask: value
		" see if all bits in argument are on"
		^ value = (self bitAnd: value)
|
	anyMask: value
		" see if any bits in argument are on"
		^ 0 ~= (self bitAnd: value)
|
	asCharacter
		^ Char new; value: self
|
	asDigit
		" return as character digit "
		(self >= 0)
			ifTrue: [ (self <= 9) ifTrue: 
					[ ^ (self + $0 asInteger) asCharacter ].
				  (self < 36) ifTrue:
					[ ^ (self + $A asInteger - 10) asCharacter ] ].
		^ smalltalk error: 'illegal conversion, integer to digit'
|
	asFloat
		" should be redefined by any subclasses "
		self isShortInteger ifTrue: [ ^ <51 self> ]
|
	asFraction
		^ Fraction new ; with: self over: 1
|
	asLongInteger	| newList i |
		newList <- List new.
		i = 0 ifTrue: [ newList add: 0 ]
			ifFalse: [ i <- self abs.
				   [ i ~= 0 ] whileTrue: 
					[ newList addLast: (i rem: 100).
					i <- i quo: 100 ] ].
		^ LongInteger new; sign: i negative digits: newList asArray
|
	asString
		^ self radix: 10
|
	bitAnd: value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ <71 self value > ]
			ifFalse: [ smalltalk error: 
				'arguments to bit operation must be short integer']
|
	bitAt: value
		^ (self bitShift: 1 - value) bitAnd: 1
|
	bitInvert
		"invert all bits in self"
		^ self bitXor: -1
|
	bitOr: value
		^ (self bitXor: value) bitXor: (self bitAnd: value)
|
	bitXor: value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ <72 self value > ]
			ifFalse: [ smalltalk error: 
				'argument to bit operation must be integer']
|
	bitShift: value
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ <79 self value > ]
			ifFalse: [ smalltalk error: 
				'argument to bit operation must be integer']
|
	even
		^ (self rem: 2) = 0
|
	factorial
		^ (2 to: self) inject: 1 into: [:x :y | x * y ]
|
	gcd: value
		(value = 0) ifTrue: [ ^ self ].
		(self negative) ifTrue: [ ^ self negated gcd: value ].
		(value negative) ifTrue: [ ^ self gcd: value negated ].
		(value > self) ifTrue: [ ^ value gcd: self ].
		^ value gcd: (self rem: value)
|
	generality
		" generality value - used in mixed class arithmetic "
		^ 2
|
	isShortInteger
		^ true
|
	lcm: value
		^ (self quo: (self gcd: value)) * value
|
	new
		^ smalltalk error: 'cannot create integers with new'
|
	odd
		^ (self rem: 2) ~= 0
|
	quo: value	| r |
		^ (self isShortInteger and: [value isShortInteger])
			ifTrue: [ r <- <69 self value>.
				(r isNil)
					ifTrue: [ smalltalk error:
						'quo: or rem: with argument 0']
					ifFalse: [ r ]]
			ifFalse: [ ^ super quo: value ]
|
	radix: base 	| sa text |
		" return a printed representation of self in given base"
		sa <- self abs.
		text <- (sa \\ base) asDigit asString.
		^ (sa < base)
			ifTrue: [ (self negative)
					ifTrue: [ '-' , text ]
					ifFalse: [ text ]]
			ifFalse: [ ((self quo: base) radix: base), text ]
|
	truncated
		^ self
|
	printString
		^ self asString
|
	timesRepeat: aBlock	| i |
		" use while, which is optimized, not to:, which is not"
		i <- 0.
		[ i < self ] whileTrue:
			[ aBlock value. i <- i + 1]
]
Methods LongInteger 'all'
	< n		| result |
		n isLongInteger
			ifFalse: [ ^ super < n ].
		(negative == n negative) ifFalse: [ ^ negative ].
		" now either both positive or both negative "
		result <- false.
		self with: n bitDo: 
			[:x :y | (x ~= y) ifTrue: [ result <- x < y]].
		negative ifTrue: [ result <- result not ].
		^ result
|
	= n
		n isLongInteger
			ifFalse: [ ^ super = n ].
		(negative == n negative) ifFalse: [ ^ false ].
		^ digits = n digits
|
	+ n		| newDigits z carry |
		n isLongInteger
			ifFalse: [ ^ super + n ].
		negative ifTrue: [ ^ n - self negated ].
		n negative ifTrue: [ ^ self - n negated ].
		" reduced to positive + positive case "
		newDigits <- List new.  carry <- 0.
		self with: n bitDo:
			[:x :y | z <- x + y + carry.
				(z >= 100) ifTrue: [ carry <- 1. z <- z - 100]
					 ifFalse: [ carry <- 0 ].
				newDigits addLast: z ].
		carry > 0 ifTrue: [ newDigits addLast: carry ].
		^ LongInteger new; sign: false digits: newDigits asArray
|
	- n		| result newDigits z borrow |
		n isLongInteger
			ifFalse: [ ^ super - n ].
		negative ifTrue: [ ^ (self negated + n) negated ].
		n negative ifTrue: [ ^ self + n negated ].
		(self < n) ifTrue: [ ^ (n - self) negated ].
		" reduced to positive - smaller positive "
		newDigits <- List new. borrow <- 0.
		self with: n bitDo:
			[:x :y | z <- (x - borrow) - y.
				(z >= 0) ifTrue: [ borrow <- 0]
				ifFalse: [ z <- z + 100. borrow <- 1].
				newDigits addLast: z ].
		result <- 0. "now normalize result by multiplication "
		newDigits reverseDo: [:x | result <- result * 100 + x ].
		^ result
|
	* n		| result |
		n isShortInteger ifTrue: [ ^ self timesShort: n ].
		n isLongInteger  ifFalse: [ ^ super * n ].
		result <- 0 asLongInteger.
		digits reverseDo: 
			[:x | result <- (result timesShort: 100) +
				(n timesShort: x)].
		negative ifTrue: [ result <- result negated ].
		^ result
|
	abs
		negative ifTrue: [ ^ self negated] 
|
	asFloat		| r |
		r <- 0.0 .
		digits reverseDo: [ :x | r <- r * 100.0 + x asFloat].
		negative ifTrue: [ r <- r negated ].
		^ r.
|
	bitShift: n
		(n >= 0)
			ifTrue: [ ^ self * (2 raisedTo: n) ]
			ifFalse: [ ^ self quo: (2 raisedTo: n negated)]
|
	coerce: n
		^ n asLongInteger
|
	digits
		^ digits
|
	generality
		^ 4 "generality value - used in mixed type arithmetic "
|
	isLongInteger
		^ true
|
	isShortInteger
		" override method in class Integer "
		^ false
|
	negated
		^ LongInteger new; sign: negative not digits: digits
|
	negative
		^ negative
|
	new
		"override restriction from class Integer"
		^ self
|
	quo: value	| a b quo result |
		result <- 0.
		a <- self abs. b <- value abs.
		[a > b] whileTrue:
			[ quo <- (a asFloat quo: b). result <- result + quo.
				a <- a - (b * quo) ].
		^ result
|
	sign: s digits: d
		negative <- s.
		digits <- d.
|
	printString	| str |
		str <- negative ifTrue: [ '-' ] ifFalse: [ '' ].
		digits reverseDo: [:x | str <- str , 
			(x quo: 10) printString , (x rem: 10) printString ].
		^ str
|
	timesShort: value	| y z carry newDigits |
		y <- value abs.
		carry <- 0.
		newDigits <- digits collect:
			[:x | z <- x * y + carry. 
				carry <- z quo: 100. 
				z - (carry * 100)].
		(carry > 0) ifTrue: [ newDigits <- newDigits grow: carry ].
		^ LongInteger new; sign: (negative xor: value negative) 
					digits: newDigits
|
	with: n bitDo: aBlock	| d di dj |
		" run down two digits lists in parallel doing block "
		di <- digits size.
		d <- n digits.
		dj <- d size.
		(1 to: (di max: dj)) do: [:i |
			aBlock value: 
			   ((i <= di) ifTrue: [ digits at: i] ifFalse: [0])
				value:
			   ((i <= dj) ifTrue: [ d at: i] ifFalse: [0]) ]
]
Methods Magnitude 'all'
	<= value
		^ (self < value) or: [ self = value ]
|
	< value
		^ (self <= value) and: [ self ~= value ]
|
	>= value
		^ value <= self
|
	> value
		^ (value < self)
|
	= value
		^ (self == value)
|
	~= value
		^ (self = value) not
|
	between: low and: high
		^ (low <= self) and: [ self <= high ]
|
	isChar
		^ false
|
	max: value
		^ (self < value)
			ifTrue: [ value ]
			ifFalse: [ self ]
|
	min: value
		^ (self < value)
			ifTrue: [ self ]
			ifFalse: [ value ]
]
Methods Number 'all'
	isNumber
		^ true
|
	maxgen: value
		(self isNumber and: [ value isNumber ])
			ifFalse: [ ^ smalltalk error: 
				'arithmetic on non-numbers' ].
		^ (self generality > value generality)
			ifTrue: [ self ]
			ifFalse: [ value coerce: self ]
|
	+ value
		^ (self maxgen: value) + (value maxgen: self)
|
	- value
		^ (self maxgen: value) - (value maxgen: self)
|
	< value
		^ (self maxgen: value) < (value maxgen: self)
|
	= value
		^ value isNumber
			ifTrue: [ (self maxgen: value) = (value maxgen: self) ]
			ifFalse: [ false ]
|
	* value
		^ (self maxgen: value) * (value maxgen: self)
|
	/ value
		^ (self maxgen: value) / (value maxgen: self)
|
	// value
		" integer division, truncate towards negative infinity"
		" see quo: "
		^ (self / value) floor
|
	\\ value
		" remainder after integer division "
		^ self - (self // value * value)
|
	abs
		^ (self < 0)
			ifTrue: [ 0 - self ]
			ifFalse: [ self ]
|
	ceiling		| i |
		i <- self truncated.
		^ ((self positive) and: [ self ~= i ])
			ifTrue: [ i + 1 ]
			ifFalse: [ i ]
|
	copy
		^ self
|
	exp
		^ self asFloat exp
|
	floor		| i |
		i <- self truncated.
		^ ((self negative) and: [ self ~= i ])
			ifTrue: [ i - 1 ]
			ifFalse: [ i ]
|
	fractionalPart
		^ self - self truncated
|
	isInteger
		^ self isLongInteger or: [ self isShortInteger ]
|
	ln
		^ self asFloat ln
|
	log: value
		^ self ln / value ln
|
	negated
		^ 0 - self
|
	negative
		^ self < 0
|
	positive
		^ self >= 0
|
	quo: value
		^ (self maxgen: value) quo: (value maxgen: self)
|
	raisedTo: x	| y |
		x negative 
			ifTrue: [ ^ 1 / (self raisedTo: x negated) ].
		x isShortInteger 
			ifTrue: [ (x = 0) ifTrue: [ ^ 1 ].
				  y <- (self raisedTo: (x quo: 2)) squared.
				  x odd ifTrue: [ y <- y * self ].
				  ^ y ]
				"use logrithms to do exponeneation"
			ifFalse: [ ^ ( x * self ln ) exp ]
|
	reciprocal
		^ 1 / self
|
	rem: value
		^ self - ((self quo: value) * value)
|
	roundTo: value
		^ (self / value ) rounded * value
|
	sign
		^ (self = 0) ifTrue: [ 0 ]
			ifFalse: [ self / self abs ]
|
	sqrt
		^ (self negative)
			ifTrue: [ smalltalk error: 'sqrt of negative']
			ifFalse: [ self raisedTo: 0.5 ]
|
	squared
		^ self * self
|
	strictlyPositive
		^ self > 0
|
	to: value
		^ Interval new; lower: self; upper: value; step: 1
|
	to: value by: step
		^ Interval new; lower: self; upper: value; step: step
|
	trucateTo: value
		^ (self / value) trucated * value
]
Methods Random 'all'
	between: low and: high
		" return random number in given range "
		^ (self next * (high - low)) + low
|
	next
		" convert rand integer into float between 0 and 1 "
		^ (<3> rem: 1000) / 1000
|
	next: value	| list |
		" return a list of random numbers of given size "
		list <- List new.
		value timesRepeat: [ list add: self next ].
		^ list
|
	randInteger: value
		^ 1 + (<3> rem: value)
|
	set: value
		" set seed for random number generator "
		<55 value>
]
