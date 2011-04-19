temp <- <primitive 110 7 >
<primitive 112 temp 1		" next " \
	#( #[ 16 167 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" first " \
	#( #[ 16 166 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" occurrencesOf: " \
	#( #[ 16 33 11 30 247 3 81 248 2 242 80 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" size " \
	#( #[ 16 163 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" remove:ifAbsent: " \
	#( #[ 16 33 34 216 242 245] \
	#(  ) ) >

<primitive 112 temp 6		" add: " \
	#( #[ 16 33 11 30 246 3 16 33 189 242 245] \
	#(  ) ) >

<primitive 112 temp 7		" new " \
	#( #[ 5 48 160 96 245] \
	#(  ) ) >

<primitive 98 #Set \
	<primitive 97 #Set #Collection #.//set.st \
	#(  #list ) \
	#( #next #first #occurrencesOf: #size #remove:ifAbsent: #add: #new  ) \
	temp 3 4 > >

