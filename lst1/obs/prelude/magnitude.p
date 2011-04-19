temp <- <primitive 110 9 >
<primitive 112 temp 1		" max: " \
	#( #[ 32 33 204 247 3 32 248 2 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" min: " \
	#( #[ 32 33 199 247 3 32 248 2 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" between:and: " \
	#( #[ 32 33 203 252 3 32 34 200 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" > " \
	#( #[ 33 32 199 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" >= " \
	#( #[ 32 33 204 251 3 32 33 201 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" ~= " \
	#( #[ 32 33 201 172 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" = " \
	#( #[ 32 33 204 251 3 32 33 199 172 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" < " \
	#( #[ 33 32 204 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" <= " \
	#( #[ 32 33 199 251 3 32 33 201 243 245] \
	#(  ) ) >

<primitive 98 #Magnitude \
	<primitive 97 #Magnitude #Object #.//magnitude.st \
	#(  ) \
	#( #max: #min: #between:and: #> #>= #~= #= #< #<=  ) \
	temp 3 4 > >

