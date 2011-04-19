temp <- <primitive 110 8 >
<primitive 112 temp 1		" next " \
	#( #[ 17 162 247 27 17 80 204 247 9 17 81 193 97 16 171 \
 243 248 10 242 16 167 241 97 161 247 2 93 243 242 249 \
 31 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" first " \
	#( #[ 16 166 241 97 161 247 2 93 243 242 17 81 193 97 16 \
 171 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" occurrencesOf: " \
	#( #[ 16 33 224 2 80 243 213 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" size " \
	#( #[ 16 80 226 1 4 33 34 192 243 215 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" remove:ifAbsent: " \
	#( #[ 16 33 224 3 34 165 244 213 115 81 35 201 247 6 16 \
 33 11 40 248 7 242 16 33 35 81 193 208 242 245] \
	#(  ) ) >

<primitive 112 temp 6		" add:withOccurrences: " \
	#( #[ 34 224 4 32 33 189 243 183 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" add: " \
	#( #[ 16 33 81 16 33 224 2 80 243 213 192 208 242 245] \
	#(  ) ) >

<primitive 112 temp 8		" new " \
	#( #[ 5 40 160 96 245] \
	#(  ) ) >

<primitive 98 #Bag \
	<primitive 97 #Bag #Collection #.//bag.st \
	#(  #dict #count ) \
	#( #next #first #occurrencesOf: #size #remove:ifAbsent: #add:withOccurrences: #add: #new  ) \
	temp 4 8 > >

