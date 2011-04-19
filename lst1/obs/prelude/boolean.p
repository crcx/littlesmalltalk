temp <- <primitive 110 6 >
<primitive 112 temp 1		" xor: " \
	#( #[ 32 33 182 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" eqv: " \
	#( #[ 32 33 181 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" or: " \
	#( #[ 32 251 2 33 165 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" and: " \
	#( #[ 32 252 2 33 165 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" | " \
	#( #[ 32 251 1 33 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" & " \
	#( #[ 32 252 1 33 243 245] \
	#(  ) ) >

<primitive 98 #Boolean \
	<primitive 97 #Boolean #Object #.//boolean.st \
	#(  ) \
	#( #xor: #eqv: #or: #and: #| #&  ) \
	temp 2 3 > >

