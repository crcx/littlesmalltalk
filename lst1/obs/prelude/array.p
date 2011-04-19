temp <- <primitive 110 6 >
<primitive 112 temp 1		" size " \
	#( #[ 32 250 1 4 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" printString " \
	#( #[ 48 113 32 250 1 4 114 34 80 204 247 21 32 34 250 \
 2 111 169 49 11 17 33 11 17 113 34 81 193 241 114 \
 242 249 26 242 50 33 11 17 243 245] \
	#( ')' ' ' '#( '  ) ) >

<primitive 112 temp 3		" grow: " \
	#( #[ 32 33 250 2 113 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" at:put: " \
	#( #[ 33 81 199 251 6 33 32 250 1 4 204 247 6 32 48 \
 188 242 93 243 242 32 33 34 250 3 112 242 34 243 245 \
] \
	#( 'index error'  ) ) >

<primitive 112 temp 5		" at: " \
	#( #[ 33 81 199 251 6 33 32 250 1 4 204 247 6 32 48 \
 188 242 93 243 242 32 33 250 2 111 243 245] \
	#( 'index error'  ) ) >

<primitive 112 temp 6		" new: " \
	#( #[ 33 250 1 114 243 245] \
	#(  ) ) >

<primitive 98 #Array \
	<primitive 97 #Array #ArrayedCollection #.//array.st \
	#(  ) \
	#( #size #printString #grow: #at:put: #at: #new:  ) \
	temp 3 4 > >

