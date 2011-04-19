!Method
disassemble: indent at: initPC for: initCount | pc low high count needCR |
	pc <- initPC.
	count <- initCount.
	[ count > 0 ] whileTrue: [
		" Default, terminate line with CR "
		needCR <- true.

		" Show PC and indent listing of line "
		((pc rem: 20) = 0) ifTrue: [
			low <- String input.
			((low size > 0) and: [ (low at: 1) = $q ]) ifTrue: [
				self error: 'Disassembly halted'
			]
		].
		((pc - 1) printWidth: 4) print.
		1 to: indent do: [:x| ' ' print].

		" Fetch basic opcode fields "
		high <- byteCodes at: pc.
		pc <- pc + 1.
		count <- count - 1.
		low <- high rem: 16.
		high <- high quo: 16.
		(high = 0) ifTrue: [
			high <- low.
			low <- byteCodes at: pc.
			pc <- pc + 1.
			count <- count - 1
		].

		(high = 1) ifTrue: [
			'PushInstance ' print.
			low print
		].

		(high = 2) ifTrue: [
			'PushArgument ' print.
			low print
		].

		(high = 3) ifTrue: [
			'PushTemporary ' print.
			low print
		].

		(high = 4) ifTrue: [
			'PushLiteral ' print.
			low print
		].

		(high = 5) ifTrue: [
			'PushConstant ' print.
			((low >= 0) and: [low < 10]) ifTrue: [
				low print
			].
			(low = 10) ifTrue: [
				'nil' print
			].
			(low = 11) ifTrue: [
				'true' print
			].
			(low = 12) ifTrue: [
				'false' print
			]
		].

		(high = 6) ifTrue: [
			'AssignInstance ' print.
			low print
		].

		(high = 7) ifTrue: [
			'AssignTemporary ' print.
			low print
		].

		(high = 8) ifTrue: [
			'MarkArguments ' print.
			low print
		].

		(high = 9) ifTrue: [
			'SendMessage ' print.
			(literals at: (low+1)) print
		].

		(high = 10) ifTrue: [
			'SendUnary ' print.
			(low = 0) ifTrue: [ 'isNil' print ].
			(low = 1) ifTrue: [ 'notNil' print ]
		].

		(high = 11) ifTrue: [
			'SendBinary ' print.
			(low = 0) ifTrue: [ '<' print ].
			(low = 1) ifTrue: [ '<=' print ].
			(low = 2) ifTrue: [ '+' print ]
		].

		(high = 12) ifTrue: [
			'PushBlock' print. Char newline print.
			low <- byteCodes at: pc.
			pc <- pc + 1.
			low <- low + ((byteCodes at: pc) * 256).
			pc <- pc + 1.
			high <- low - (pc-1).
			self disassemble: indent+4 at: pc for: high.
			count <- count - (high + 2).
			pc <- pc + high.
			needCR <- false
		].

		(high = 13) ifTrue: [
			'DoPrimitive ' print.
			high <- byteCodes at: pc.
			pc <- pc + 1.
			count <- count - 1.

			" VM generic "
			(high = 1) ifTrue: [ '=' print ].
			(high = 2) ifTrue: [ 'class' print ].
			(high = 3) ifTrue: [ 'putchar' print ].
			(high = 4) ifTrue: [ 'size' print ].
			(high = 5) ifTrue: [ 'at:put:' print ].
			(high = 6) ifTrue: [ 'newProcess' print ].
			(high = 7) ifTrue: [ 'new' print ].
			(high = 8) ifTrue: [ 'block invoke' print ].
			(high = 9) ifTrue: [ 'getchar' print ].
			(high = 10) ifTrue: [ 'SmallInt +' print ].
			(high = 11) ifTrue: [ 'SmallInt /' print ].
			(high = 12) ifTrue: [ 'SmallInt %' print ].
			(high = 13) ifTrue: [ 'SmallInt <' print ].
			(high = 14) ifTrue: [ 'SmallInt =' print ].
			(high = 15) ifTrue: [ 'SmallInt *' print ].
			(high = 16) ifTrue: [ 'SmallInt -' print ].
			(high = 18) ifTrue: [ 'debug' print ].
			(high = 19) ifTrue: [ 'error' print ].
			(high = 20) ifTrue: [ 'ByteArray new' print ].
			(high = 21) ifTrue: [ 'String at:' print ].
			(high = 22) ifTrue: [ 'String at:put:' print ].
			(high = 23) ifTrue: [ 'String clone' print ].
			(high = 24) ifTrue: [ 'Array at:' print ].
			(high = 25) ifTrue: [ 'Integer /' print ].
			(high = 26) ifTrue: [ 'Integer %' print ].
			(high = 27) ifTrue: [ 'Integer +' print ].
			(high = 28) ifTrue: [ 'Integer *' print ].
			(high = 29) ifTrue: [ 'Integer -' print ].
			(high = 30) ifTrue: [ 'Integer <' print ].
			(high = 31) ifTrue: [ 'Integer =' print ].
			(high = 32) ifTrue: [ 'SmallInt asInteger' print ].
			(high = 33) ifTrue: [ 'Integer asSmallInt' print ].

			" UNIX port specific "
			(high = 100) ifTrue: [ 'UNIX fopen' print ].
			(high = 101) ifTrue: [ 'UNIX fgetc' print ].
			(high = 102) ifTrue: [ 'UNIX fputc' print ].
			(high = 103) ifTrue: [ 'UNIX fclose' print ].
			(high = 104) ifTrue: [ 'UNIX fileout' print ].
			(high = 105) ifTrue: [ 'UNIX edit' print ].
		].

		(high = 15) ifTrue: [
			'DoSpecial ' print.
			(low = 1) ifTrue: [ 'selfReturn' print ].
			(low = 2) ifTrue: [ 'stackReturn' print ].
			(low = 3) ifTrue: [ 'blockReturn' print ].
			(low = 4) ifTrue: [ 'duplicate' print ].
			(low = 5) ifTrue: [ 'popTop' print ].
			(low = 6) ifTrue: [
				'branch ' print.
				high <- byteCodes at: pc.
				pc <- pc + 1.
				high <- high + ((byteCodes at: pc) * 256).
				pc <- pc + 1.
				count <- count - 2.
				high print
			].
			(low = 7) ifTrue: [
				'branchIfTrue ' print.
				high <- byteCodes at: pc.
				pc <- pc + 1.
				high <- high + ((byteCodes at: pc) * 256).
				pc <- pc + 1.
				count <- count - 2.
				high print
			].
			(low = 8) ifTrue: [
				'branchIfFalse ' print.
				high <- byteCodes at: pc.
				pc <- pc + 1.
				high <- high + ((byteCodes at: pc) * 256).
				pc <- pc + 1.
				count <- count - 2.
				high print
			].
			(low = 11) ifTrue: [
				'sendToSuper ' print.
				low <- byteCodes at: pc.
				pc <- pc + 1.
				count <- count - 1.
				(literals at: low+1) print
			].
			(low = 12) ifTrue: [ 'breakpoint' print ].
		].
		needCR ifTrue: [
			Char newline print
		]
	]
!
!Method
disassemble
	self disassemble: 1 at: 1 for: (byteCodes size)
!
