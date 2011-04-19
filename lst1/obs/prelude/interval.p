temp <- <primitive 110 13 >
<primitive 112 temp 1		" shallowCopy " \
	#( #[ 16 17 18 212 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" deepCopy " \
	#( #[ 16 17 18 212 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" removeKey:ifAbsent: " \
	#( #[ 32 48 188 242 34 165 243 245] \
	#( 'cannot remove from Interval'  ) ) >

<primitive 112 temp 4		" add: " \
	#( #[ 32 48 188 243 245] \
	#( 'cannot store into Interval'  ) ) >

<primitive 112 temp 5		" at:put: " \
	#( #[ 32 48 188 243 245] \
	#( 'cannot store into Interval'  ) ) >

<primitive 112 temp 6		" coerce: " \
	#( #[ 33 174 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" printString " \
	#( #[ 48 16 169 11 17 49 11 17 17 169 11 17 50 11 17 \
 18 169 11 17 243 245] \
	#( 'Interval ' ' to ' ' by '  ) ) >

<primitive 112 temp 8		" at:ifAbsent: " \
	#( #[ 16 18 33 81 193 194 192 115 32 35 11 31 247 3 35 \
 248 3 242 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" next " \
	#( #[ 19 18 192 99 32 19 11 31 247 1 19 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" first " \
	#( #[ 16 99 32 19 11 31 247 1 19 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" inRange: " \
	#( #[ 18 170 247 10 33 16 203 252 3 33 17 200 248 9 242 \
 33 17 203 252 3 33 16 200 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" size " \
	#( #[ 18 170 247 5 17 16 199 248 4 242 16 17 199 247 3 \
 80 248 9 242 17 16 193 18 11 18 81 192 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" from:to:by: " \
	#( #[ 33 241 96 99 34 97 35 98 245] \
	#(  ) ) >

<primitive 98 #Interval \
	<primitive 97 #Interval #SequenceableCollection #.//interval.st \
	#(  #lower #upper #step #current ) \
	#( #shallowCopy #deepCopy #removeKey:ifAbsent: #add: #at:put: #coerce: #printString #at:ifAbsent: #next #first #inRange: #size #from:to:by:  ) \
	temp 4 5 > >

