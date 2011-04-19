temp <- <primitive 110 25 >
<primitive 112 temp 1		" truncated " \
	#( #[ 32 48 199 247 6 32 250 1 73 248 5 242 32 250 1 \
 72 243 245] \
	#( 0.0  ) ) >

<primitive 112 temp 2		" sqrt " \
	#( #[ 32 250 1 71 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" rounded " \
	#( #[ 32 48 192 250 1 72 243 245] \
	#( 0.5  ) ) >

<primitive 112 temp 4		" radix: " \
	#( #[ 32 33 250 2 89 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" ln " \
	#( #[ 32 250 1 70 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" integerPart " \
	#( #[ 32 250 1 75 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" gamma " \
	#( #[ 32 250 1 77 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" fractionPart " \
	#( #[ 32 250 1 76 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" floor " \
	#( #[ 32 250 1 72 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" exp " \
	#( #[ 32 250 1 79 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" coerce: " \
	#( #[ 33 175 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" ceiling " \
	#( #[ 32 250 1 73 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" asString " \
	#( #[ 32 250 1 78 243 245] \
	#(  ) ) >

<primitive 112 temp 14		" asFloat " \
	#( #[ 32 243 245] \
	#(  ) ) >

<primitive 112 temp 15		" arcTan " \
	#( #[ 5 54 32 250 1 86 176 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" arcSin " \
	#( #[ 5 54 32 250 1 84 176 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" arcCos " \
	#( #[ 5 54 32 250 1 85 176 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" ^ " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 88 248 5 242 \
 32 33 145 0 243 245] \
	#( #raisedTo:  ) ) >

<primitive 112 temp 19		" / " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 69 248 5 242 \
 32 33 145 0 243 245] \
	#( #/  ) ) >

<primitive 112 temp 20		" * " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 68 248 5 242 \
 32 33 145 0 243 245] \
	#( #*  ) ) >

<primitive 112 temp 21		" - " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 61 248 5 242 \
 32 33 145 0 243 245] \
	#( #-  ) ) >

<primitive 112 temp 22		" + " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 60 248 5 242 \
 32 33 145 0 243 245] \
	#( #+  ) ) >

<primitive 112 temp 23		" > " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 63 248 5 242 \
 32 33 145 0 243 245] \
	#( #>  ) ) >

<primitive 112 temp 24		" < " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 62 248 5 242 \
 32 33 145 0 243 245] \
	#( #<  ) ) >

<primitive 112 temp 25		" = " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 66 248 5 242 \
 32 33 145 0 243 245] \
	#( #=  ) ) >

<primitive 98 #Float \
	<primitive 97 #Float #Number #.//float.st \
	#(  ) \
	#( #truncated #sqrt #rounded #radix: #ln #integerPart #gamma #fractionPart #floor #exp #coerce: #ceiling #asString #asFloat #arcTan #arcSin #arcCos #^ #/ #* #- #+ #> #< #=  ) \
	temp 2 4 > >

