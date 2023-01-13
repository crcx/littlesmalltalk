Class Object
[
 	== anObject
		^ <Equality self anObject >
|
       ~~ x
                ^ (self == x) not
|
 	= x
		^ (self == x)
|
 	~= x
		^ (self = x) not
|
 	asString
		^ self class printString
|
        asSymbol
                ^ self asString asSymbol
|
	class
		^ <Class self >
|
        copy
                ^ self shallowCopy
|
        deepCopy		| size newobj |
		size <- <Size self>.
		(size < 0) 
			ifTrue: [^ self] "if special just copy object"
			ifFalse: [ newobj <- self class new.
			(1 to: size) do: [:i |
				<AtPut newobj i
					( <At self i > copy ) > ].
				^ newobj ]
|
 	do: aBlock			| item |
		item <- self first.
		^ [item notNil] whileTrue:
			[aBlock value: item.  item <- self next]
|
	error: aString
		<Error aString self>
|
        first
                ^ self
|
        isKindOf: aClass                | objectClass |
                objectClass <- self class.
                [objectClass notNil] whileTrue:
                        [(objectClass == aClass) ifTrue: [^ true].
                         objectClass <- objectClass superClass].
                ^ false
|
        isMemberOf: aClass
                ^ aClass == self class

|
        isNil
                ^ false
|
        next
                ^ nil
|
        notNil
                ^ true
|
 	print
		<PrintWithReturn (self printString) >
|
 	printString
		^ self asString

|       respondsTo: cmd
                ^ self class respondsTo: cmd

|       shallowCopy		| size newobj |
		size <- <Size self>.
		(size < 0) 
			ifTrue: [^ self] "if special just copy object"
			ifFalse: [ newobj <- self class new.
				(1 to: size) do: [:i |
					<AtPut newobj i
						<At self i > > ].
					^ newobj ]
]
