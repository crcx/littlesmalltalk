Class Char :Magnitude
[
	== aChar
		^ <SameTypeOfObject self aChar>
			ifTrue:  [<CharacterEquality self aChar>]
			ifFalse: [false]
|	< aChar
		^ <SameTypeOfObject self aChar>
			ifTrue:  [<CharacterLessThan self aChar>]
			ifFalse: [self compareError]
|
	= aChar
		^ <SameTypeOfObject self aChar>
			ifTrue:  [<CharacterEquality self aChar>]
			ifFalse: [self compareError]
|	> aChar
		^ <SameTypeOfObject self aChar>
			ifTrue:  [<CharacterGreaterThan self aChar>]
			ifFalse: [self compareError]
|
	asciiValue
		^ <CharacterToInteger self>
|
	asLowercase
		^ <IsUpper self>
			ifTrue:  [<ChangeCase self>]
			ifFalse: [self]
|
	asUppercase
		^ <IsLower self>
			ifTrue:  [<ChangeCase self>]
			ifFalse: [self]
|
	asString
		^ <CharacterToString self>
|
	compareError
		^ self error: 'char cannot be compared to non char'
|
	digitValue		| i |
		((i <- <DigitValue self>) isNil)
			ifTrue: [self error: 'digitValue on nondigit char'].
		^ i
|
	isAlphaNumeric
		^ <IsAlnum self>
|
	isDigit
		^ self between: $0 and: $9
|
	isLetter
		^ self isLowercase or: [self isUppercase]
|
	isLowercase
		^ self between: $a and: $z 
|
	isSeparator
		^ <IsSpace self>
|
	isUppercase
		^ (self between: $A and: $Z)
|
	isVowel
		^ <IsVowel self>
|
	printString
		^ '$' , <CharacterToString self>
]
