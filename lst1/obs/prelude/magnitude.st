Class Magnitude
[
	<= arg
		^ (self < arg) or: [self = arg]

|	< arg
		^ (arg > self)

|	= arg
		^ (self > arg or: [self < arg]) not

|	~= arg
		^ (self = arg) not

|	>= arg
		^ (self > arg) or: [self = arg]

|	> arg
		^ arg < self

|	between: low and: high
		^ (self >= low) and: [self <= high]

|	min: arg
		^ (self < arg) ifTrue: [self] ifFalse: [arg]

|	max: arg
		^ (self > arg) ifTrue: [self] ifFalse: [arg]
]
