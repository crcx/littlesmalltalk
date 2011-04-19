temp <- <primitive 110 32 >
<primitive 112 temp 1		" timesRepeat: " \
	#( #[ 80 114 34 32 199 247 11 33 165 242 34 81 192 241 114 \
 242 249 16 242 245] \
	#(  ) ) >

<primitive 112 temp 2		" rem: " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 29 248 4 242 \
 32 10 35 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" radix: " \
	#( #[ 32 33 250 2 26 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" quo: " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 28 248 4 242 \
 32 10 35 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" odd " \
	#( #[ 32 82 205 80 202 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" noMask: " \
	#( #[ 80 32 33 197 201 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" lcm: " \
	#( #[ 32 33 250 2 6 247 10 32 33 194 32 33 11 27 206 \
 248 4 242 32 10 35 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" highBit " \
	#( #[ 32 250 1 34 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" gcd: " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 20 248 4 242 \
 32 10 35 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" factorial " \
	#( #[ 32 250 1 38 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" even " \
	#( #[ 32 82 205 80 201 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" bitXor: " \
	#( #[ 32 33 250 2 24 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" bitShift: " \
	#( #[ 32 33 250 2 25 243 245] \
	#(  ) ) >

<primitive 112 temp 14		" bitOr: " \
	#( #[ 32 33 250 2 22 243 245] \
	#(  ) ) >

<primitive 112 temp 15		" bitInvert " \
	#( #[ 32 250 1 33 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" bitAt: " \
	#( #[ 32 33 250 2 21 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" bitAnd: " \
	#( #[ 32 33 250 2 23 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" asString " \
	#( #[ 32 250 1 37 243 245] \
	#(  ) ) >

<primitive 112 temp 19		" asInteger " \
	#( #[ 32 243 245] \
	#(  ) ) >

<primitive 112 temp 20		" asFloat " \
	#( #[ 32 250 1 39 243 245] \
	#(  ) ) >

<primitive 112 temp 21		" asCharacter " \
	#( #[ 32 250 1 36 243 245] \
	#(  ) ) >

<primitive 112 temp 22		" anyMask: " \
	#( #[ 80 32 33 250 2 23 202 243 245] \
	#(  ) ) >

<primitive 112 temp 23		" allMask: " \
	#( #[ 33 32 33 250 2 23 201 243 245] \
	#(  ) ) >

<primitive 112 temp 24		" \\ " \
	#( #[ 32 33 250 2 6 247 18 32 32 80 199 247 3 90 248 \
 2 242 81 194 114 34 33 205 248 4 242 32 10 35 243 \
 245] \
	#(  ) ) >

<primitive 112 temp 25		" // " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 19 248 4 242 \
 32 10 35 243 245] \
	#(  ) ) >

<primitive 112 temp 26		" / " \
	#( #[ 32 175 33 190 243 245] \
	#(  ) ) >

<primitive 112 temp 27		" * " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 18 248 5 242 \
 32 33 145 0 243 245] \
	#( #*  ) ) >

<primitive 112 temp 28		" - " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 11 248 5 242 \
 32 33 145 0 243 245] \
	#( #-  ) ) >

<primitive 112 temp 29		" + " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 10 248 5 242 \
 32 33 145 0 243 245] \
	#( #+  ) ) >

<primitive 112 temp 30		" < " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 12 248 5 242 \
 32 33 145 0 243 245] \
	#( #<  ) ) >

<primitive 112 temp 31		" > " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 13 248 5 242 \
 32 33 145 0 243 245] \
	#( #>  ) ) >

<primitive 112 temp 32		" = " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 16 248 5 242 \
 32 33 145 0 243 245] \
	#( #=  ) ) >

<primitive 98 #Integer \
	<primitive 97 #Integer #Number #.//integer.st \
	#(  ) \
	#( #timesRepeat: #rem: #radix: #quo: #odd #noMask: #lcm: #highBit #gcd: #factorial #even #bitXor: #bitShift: #bitOr: #bitInvert #bitAt: #bitAnd: #asString #asInteger #asFloat #asCharacter #anyMask: #allMask: #\\ #// #/ #* #- #+ #< #> #=  ) \
	temp 3 5 > >

