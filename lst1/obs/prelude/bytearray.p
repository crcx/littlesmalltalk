temp <- <primitive 110 5 >
<primitive 112 temp 1		" size " \
	#( #[ 32 250 1 117 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" printString " \
	#( #[ 48 113 81 32 163 178 225 2 13 33 32 34 177 169 11 \
 17 49 11 17 241 113 243 179 242 33 50 11 17 243 245 \
] \
	#( '#[ ' ' ' ']'  ) ) >

<primitive 112 temp 3		" at:put: " \
	#( #[ 32 33 34 250 3 119 242 245] \
	#(  ) ) >

<primitive 112 temp 4		" at: " \
	#( #[ 32 33 250 2 118 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" new: " \
	#( #[ 33 250 1 116 243 245] \
	#(  ) ) >

<primitive 98 #ByteArray \
	<primitive 97 #ByteArray #ArrayedCollection #.//bytearray.st \
	#(  ) \
	#( #size #printString #at:put: #at: #new:  ) \
	temp 3 6 > >

