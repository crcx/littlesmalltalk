temp <- <primitive 110 5 >
<primitive 112 temp 1		" not " \
	#( #[ 92 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" ifFalse: " \
	#( #[ 93 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" ifTrue: " \
	#( #[ 33 165 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" ifFalse:ifTrue: " \
	#( #[ 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" ifTrue:ifFalse: " \
	#( #[ 33 165 243 245] \
	#(  ) ) >

<primitive 98 #True \
	<primitive 97 #True #Boolean #.//true.st \
	#(  ) \
	#( #not #ifFalse: #ifTrue: #ifFalse:ifTrue: #ifTrue:ifFalse:  ) \
	temp 3 2 > >

