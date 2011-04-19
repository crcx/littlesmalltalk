temp <- <primitive 110 8 >
<primitive 112 temp 1		" printString " \
	#( #[ 16 10 16 48 11 17 243 245] \
	#( ' radians'  ) ) >

<primitive 112 temp 2		" asFloat " \
	#( #[ 16 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" tan " \
	#( #[ 16 250 1 81 16 250 1 82 190 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" cos " \
	#( #[ 16 250 1 82 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" sin " \
	#( #[ 16 250 1 81 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" = " \
	#( #[ 16 33 175 201 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" < " \
	#( #[ 16 33 175 199 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" new: " \
	#( #[ 33 175 250 1 80 96 245] \
	#(  ) ) >

<primitive 98 #Radian \
	<primitive 97 #Radian #Magnitude #.//radian.st \
	#(  #value ) \
	#( #printString #asFloat #tan #cos #sin #= #< #new:  ) \
	temp 2 3 > >

