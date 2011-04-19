temp <- <primitive 110 7 >
<primitive 112 temp 1		" next: " \
	#( #[ 5 30 33 176 114 81 33 178 225 3 6 34 35 32 167 \
 208 243 179 242 34 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" randInteger: " \
	#( #[ 32 167 33 194 10 47 81 192 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" between:and: " \
	#( #[ 32 167 34 33 193 194 33 192 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" next " \
	#( #[ 16 250 1 35 241 96 250 1 32 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" first " \
	#( #[ 16 250 1 35 241 96 250 1 32 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" randomize " \
	#( #[ 250 0 161 96 245] \
	#(  ) ) >

<primitive 112 temp 7		" new " \
	#( #[ 81 96 245] \
	#(  ) ) >

<primitive 98 #Random \
	<primitive 97 #Random #Object #.//random.st \
	#(  #seed ) \
	#( #next: #randInteger: #between:and: #next #first #randomize #new  ) \
	temp 4 6 > >

