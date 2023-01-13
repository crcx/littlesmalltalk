*
*
* Little Smalltalk, version 2
* Written by Tim Budd, Oregon State University, July 1987
*
*  a few test cases.
*
* to use, first file in this file, then pas all to an instance of
*	class Test, for example using the messages
*
*	File new; fileIn: 'test.st'
* 	Test new all
*
Class Test Object
Class One Object
Class Two One
Class Three Two
Class Four Three
Methods One 'all'
        test
                ^ 1
|
	result1
                ^ self test
]
Methods Two 'all'
        test
                ^ 2
]
Methods Three 'all'
        result2
                ^ self result1
|
	result3
                ^ super test
]
Methods Four 'all'
        test
                ^ 4
]
Methods Test 'all'
	all
		self super.
		self conversions.
		self collections.
		self factorial.
		self filein.
		'all tests completed' print
|
	conversions
		" test a few conversion routines "
		( (#abc == #abc asString asSymbol) and: [
		($A == $A asInteger asCharacter) and: [
		(12 == 12 asDigit digitValue) and: [
		(237 == 237 asString asInteger) and: [
		(43 = 43 asFloat truncated) and: [
		$A == ($A asString at: 1) ] ] ] ] ] )
			ifFalse: [^ smalltalk error: 'conversion failure'].
		'conversion test passed' print.
|
	collections
		" test the collection classes a little"
		( (#(1 2 3 3 2 4 2) asSet = #(1 2 3 4) asSet) and: [
		(#(1 5 3 2 4) sort asArray = #(1 2 3 4 5)) and: [
		(1 "(#+ respondsTo occurrencesOf: Float)" = 1) and: [
		('First' < 'last') ] ] ] )
			ifFalse: [^smalltalk error: 'collection failure'].
		'collection test passed' print.
|
	factorial	| t |
		t <- [:x | (x = 1) ifTrue: [ 1 ] 
				ifFalse: [ x * (t value: x - 1) ] ].
		((t value: 5) = 5 factorial)
			ifFalse: [ smalltalk error: 'factorial failure'].
		'factorial test passed' print
|
	filein
		File new; name: 'queen.st'; open: 'r'; fileIn.
		"(globalNames includesKey: #Queen )
			ifFalse: [ smalltalk error: 'fileIn failure']."
		'file in test passed' print.
		self queen
|
	super2		 | x1 x2 x3 x4 |
                x1 <- One new.
                x2 <- Two new.
                x3 <- Three new.
                x4 <- Four new.
		^ List new; addLast: x1 test;
			addLast: x1 result1;
			addLast: x2 test;
			addLast: x2 result1;
			addLast: x3 test;
                	addLast: x4 result1;
                	addLast: x3 result2;
			addLast: x4 result2;
                	addLast: x3 result3;
                	addLast: x4 result3
|
	super
		(self super2 asArray = #(1 1 2 2 2 4 2 4 2 2) )
			ifTrue: ['super test passed' print]
			ifFalse: [ smalltalk error: 'super test failed']
]
