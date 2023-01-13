Class Main
[
	testChars
		($A max: $a) print.
		(4 between: 3.1 and: (17/3)) print.
		($A < $0) print.
		$A asciiValue print.
		$A asString print.
		$A printString print.
		$A isVowel print.
		$A digitValue print
|
	testNums
		3 + 4.1 ; print.
		3.14159 exp print.
		1 pi exp print.
		3.5 radians print.
		13 roundTo: 5 ; print.
		13 truncateTo: 5 ; print.
		(smalltalk perform: #+ withArguments: #(3 4.1) ) print.
		(smalltalk doPrimitive: 10 withArguments: #(3 4) ) print
|
	testInts
		5 allMask: 4 ; print.
		4 allMask: 5 ; print.
		5 anyMask: 4 ; print.
		5 bitAnd: 3 ; print.
		5 bitOr: 3 ; print.
		5 bitInvert print.
		254 radix: 16 ; print.
		5 reciprocal print.
		-5 // 4 ; print.
		-5 quo: 4 ; print.
		-5 \\ 4 ; print.
		-5 rem: 4 ; print.
		4 factorial print.
|
	testFloats
		2.1 ^ 4 ; print.
		0.5 arcSin print.
		4.3 sqrt print.
		256 log: 10 ; print.
		16rC.ABC print.
		(14.5408 radix: 16) print.
		0.5236 radians sin print.
		(100 @ 12) transpose print.
|
	main
		self testChars.
		self testNums.
		self testInts.
		self testFloats.
]
