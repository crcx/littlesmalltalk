temp <- <primitive 110 3 >
<primitive 112 temp 1		" asString " \
	#( #[ 32 250 1 93 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" printString " \
	#( #[ 32 250 1 92 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" == " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 91 248 2 242 \
 92 243 245] \
	#(  ) ) >

<primitive 98 #Symbol \
	<primitive 97 #Symbol #Object #.//symbol.st \
	#(  ) \
	#( #asString #printString #==  ) \
	temp 2 4 > >

