temp <- <primitive 110 4 >
<primitive 112 temp 1		" wait " \
	#( #[ 250 0 148 242 16 80 201 247 10 32 95 11 22 242 95 \
 10 18 248 6 242 16 81 193 241 96 242 250 0 149 242 \
 245] \
	#(  ) ) >

<primitive 112 temp 2		" signal " \
	#( #[ 250 0 148 242 32 10 27 247 7 16 81 192 241 96 248 \
 6 242 32 10 38 10 48 242 250 0 149 242 245] \
	#(  ) ) >

<primitive 112 temp 3		" new: " \
	#( #[ 33 96 245] \
	#(  ) ) >

<primitive 112 temp 4		" new " \
	#( #[ 80 96 245] \
	#(  ) ) >

<primitive 98 #Semaphore \
	<primitive 97 #Semaphore #List #.//semaphore.st \
	#(  #excessSignals ) \
	#( #wait #signal #new: #new  ) \
	temp 2 4 > >

