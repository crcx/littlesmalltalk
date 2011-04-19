+Set subclass: #Dict
+Dict subclass: #RotorDict variables: #(rotor)
!Dict
at: key ifAbsent: aBlock | pos assoc |
	pos <- self location: key.
	((pos isNil) or: [(assoc <- members at: pos) isNil]) ifTrue: [
		^ aBlock value
	].
	^ assoc value
!
!Dict
at: key put: value | assoc pos |
	" Create the Association "
	assoc <- Association key: key value: value.

	" Insert it using Set's method "
	self add: assoc.

	^ value
!
!Dict
binaryDo: aBlock
	members do: [:assoc|
		assoc notNil ifTrue: [ aBlock value: (assoc key)
			value: (assoc value) ]
	]
!
!Dict
keysDo: aBlock
	members do: [:assoc|
		assoc notNil ifTrue: [ aBlock value: (assoc key) ]
	]
!
!Dict
associationsDo: aBlock
	members do: [:assoc|
		assoc notNil ifTrue: [ aBlock value: assoc ]
	]
!
!Dict
basicDo: aBlock
	^ self associationsDo: aBlock
!
!Dict
keysAsArray | i ret |
	ret <- Array new: self size.
	i <- 1.
	members do: [:assoc| ret at: i put: assoc key. i <- i + 1 ].
	^ ret
!
!Dict
isEmpty
	members do: [:assoc| assoc notNil ifTrue: [ ^ false ]].
	^ true
!
!Dict
do: aBlock
	members do: [:assoc| assoc notNil ifTrue:
		[ aBlock value: (assoc value) ] ]
!
!Dict
removeKey: key ifAbsent: aBlock | assoc |
	assoc <- Association key: key value: nil.
	super remove: assoc ifAbsent: [ ^ aBlock value ].
	^ key
!
!Dict
removeKey: key
	^ self removeKey: key ifAbsent: [ self noElement ]
!
!Dict
remove: value ifAbsent: ignore
	self error: 'Can only remove by key'
!
!RotorDict
rotorDo: aBlock | start lim assoc |
	lim <- members size.
	rotor isNil ifTrue: [ rotor <- 1 ].
	start <- rotor.
	[true] whileTrue: [
		assoc <- members at: rotor.
		rotor <- rotor + 1.
		(rotor > lim) ifTrue: [ rotor <- 1 ].
		assoc notNil ifTrue: [ aBlock value: assoc ].
		(rotor = start) ifTrue: [ ^ self ]
	]
!
!RotorDict
do: aBlock
	^self rotorDo: [:assoc| aBlock value: assoc value]
!
!RotorDict
keysDo: aBlock
	^self rotorDo: [:assoc| aBlock value: assoc key]
!
!RotorDict
binaryDo: aBlock
	^self rotorDo: [:assoc| aBlock value: assoc key value: assoc value]
!
