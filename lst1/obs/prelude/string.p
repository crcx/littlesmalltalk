temp <- <primitive 110 22 >
<primitive 112 temp 1		" sameAs: " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 102 248 4 242 \
 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" size " \
	#( #[ 32 250 1 100 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" printNoReturn " \
	#( #[ 32 250 1 120 242 245] \
	#(  ) ) >

<primitive 112 temp 4		" print " \
	#( #[ 32 250 1 121 242 245] \
	#(  ) ) >

<primitive 112 temp 5		" printString " \
	#( #[ 32 250 1 109 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" printAt: " \
	#( #[ 32 33 10 49 33 10 50 250 3 126 242 245] \
	#(  ) ) >

<primitive 112 temp 7		" new: " \
	#( #[ 33 250 1 115 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" deepCopy " \
	#( #[ 32 250 1 107 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" copyFrom:length: " \
	#( #[ 32 33 34 250 3 106 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" copyFrom:to: " \
	#( #[ 32 33 34 33 193 81 192 250 3 106 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" compareError " \
	#( #[ 32 48 188 243 245] \
	#( 'strings can only be compared to strings'  ) ) >

<primitive 112 temp 12		" at:put: " \
	#( #[ 32 33 34 250 3 105 242 245] \
	#(  ) ) >

<primitive 112 temp 13		" at: " \
	#( #[ 32 33 250 2 104 243 245] \
	#(  ) ) >

<primitive 112 temp 14		" asSymbol " \
	#( #[ 32 250 1 108 243 245] \
	#(  ) ) >

<primitive 112 temp 15		" asFloat " \
	#( #[ 32 250 1 165 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" asInteger " \
	#( #[ 32 250 1 164 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" > " \
	#( #[ 32 33 250 2 6 247 9 32 33 250 2 101 80 204 248 \
 4 242 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" >= " \
	#( #[ 32 33 250 2 6 247 9 32 33 250 2 101 80 203 248 \
 4 242 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 19		" <= " \
	#( #[ 32 33 250 2 6 247 9 32 33 250 2 101 80 200 248 \
 4 242 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 20		" < " \
	#( #[ 32 33 250 2 6 247 9 32 33 250 2 101 80 199 248 \
 4 242 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 21		" = " \
	#( #[ 32 33 250 2 6 247 9 32 33 250 2 101 80 201 248 \
 4 242 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 22		" , " \
	#( #[ 32 32 33 250 2 6 247 3 33 248 3 242 33 169 250 \
 2 103 243 245] \
	#(  ) ) >

<primitive 98 #String \
	<primitive 97 #String #ArrayedCollection #.//string.st \
	#(  ) \
	#( #sameAs: #size #printNoReturn #print #printString #printAt: #new: #deepCopy #copyFrom:length: #copyFrom:to: #compareError #at:put: #at: #asSymbol #asFloat #asInteger #> #>= #<= #< #= #,  ) \
	temp 3 5 > >

