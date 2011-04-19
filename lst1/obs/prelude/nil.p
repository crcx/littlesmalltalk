temp <- <primitive 110 3 >
<primitive 112 temp 1		" printString " \
	#( #[ 48 243 245] \
	#( 'nil'  ) ) >

<primitive 112 temp 2		" notNil " \
	#( #[ 92 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" isNil " \
	#( #[ 91 243 245] \
	#(  ) ) >

<primitive 98 #UndefinedObject \
	<primitive 97 #UndefinedObject #Object #.//nil.st \
	#(  ) \
	#( #printString #notNil #isNil  ) \
	temp 1 2 > >

