Class Number :Magnitude
[
	maxtype: aNumber
		^ <GeneralityTest self aNumber> 
			ifTrue:  [self]
			ifFalse: [aNumber coerce: self ]
|
	= aNumber
		^ (self maxtype: aNumber) = (aNumber maxtype: self)
|
	< aNumber
		^ (self maxtype: aNumber) < (aNumber maxtype: self)
|
	> aNumber
		^ (self maxtype: aNumber) > (aNumber maxtype: self)
|
	+ aNumber
		^ (self maxtype: aNumber) + (aNumber maxtype: self)
|
	- aNumber
		^ (self maxtype: aNumber) - (aNumber maxtype: self)
|
	* aNumber
		^ (self maxtype: aNumber) * (aNumber maxtype: self)
|
	/ aNumber
		^ (self maxtype: aNumber) / (aNumber maxtype: self)
|
	^ aNumber
		^ self asFloat ^ aNumber asFloat
|
	@ aNumber
		^ ( Point new x: self ) y: aNumber
|
	abs
		^ (self < 0)
			ifTrue:  [ 0 - self ]
			ifFalse: [ self ]
|
	exp
		^ self asFloat exp
|
	gamma
		^ self asFloat gamma
|
	ln
		^ self asFloat ln
|
	log: aNumber
		^ self ln / aNumber ln
|
	negated
		^ 0 - self
|
	negative
		^ self < 0
|
	pi
		^ self * 3.1415926
|
	positive
		^ self >= 0
|
	radians
		^ Radian new: self asFloat
|
	raisedTo: aNumber
		^ self asFloat ^ aNumber asFloat
|
	reciprocal
		^ 1.00 / self
|
	roundTo: aNumber
		^ (self / aNumber) rounded * aNumber
|
	sign
		^ (self < 0)
			ifTrue: [ -1 ]
			ifFalse: [ (self > 0)
					ifTrue: [ 1 ]
					ifFalse: [ 0 ] ]
|
	sqrt
		^ self asFloat sqrt
|
	squared
		^ self * self
|
	strictlyPositive
		^ self > 0
|
	to: highValue
		^ Interval new ; from: self to: highValue by: 1
|
	to: highValue by: stepSize
		^ Interval new ; from: self to: highValue by: stepSize
|
	truncateTo: aNumber
		^ (self / aNumber) truncated * aNumber
]
