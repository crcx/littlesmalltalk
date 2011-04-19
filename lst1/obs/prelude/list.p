temp <- <primitive 110 17 >
<primitive 112 temp 1		" isEmpty " \
	#( #[ 16 93 181 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" last " \
	#( #[ 16 161 247 2 93 243 242 32 10 24 10 49 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" current " \
	#( #[ 17 10 49 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" next " \
	#( #[ 17 10 50 241 97 162 247 3 17 10 49 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" first " \
	#( #[ 16 241 97 162 247 3 17 10 49 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" removeLast " \
	#( #[ 16 161 247 4 32 10 37 243 242 32 32 10 30 224 4 \
 32 10 37 243 216 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" removeFirst " \
	#( #[ 16 161 247 4 32 10 37 243 242 16 113 16 10 50 96 \
 33 10 49 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" removeError " \
	#( #[ 32 48 188 243 245] \
	#( 'cannot remove from an empty list'  ) ) >

<primitive 112 temp 9		" remove:ifAbsent: " \
	#( #[ 16 161 247 3 34 165 243 242 32 93 226 3 31 17 10 \
 49 33 181 247 21 35 161 247 7 17 10 50 241 96 248 \
 7 242 35 17 10 50 11 51 242 33 244 242 17 243 215 \
 242 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" remove: " \
	#( #[ 32 33 224 4 32 48 188 243 216 243 245] \
	#( 'cant find item'  ) ) >

<primitive 112 temp 11		" findLast " \
	#( #[ 16 241 113 161 247 2 93 243 242 33 10 50 162 247 8 \
 33 10 50 241 113 242 249 14 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" coerce: " \
	#( #[ 5 48 160 114 33 225 3 5 34 35 11 22 243 179 242 \
 34 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" addAllLast: " \
	#( #[ 33 225 2 5 32 34 11 22 243 179 242 245] \
	#(  ) ) >

<primitive 112 temp 14		" addAllFirst: " \
	#( #[ 33 225 2 5 32 34 11 21 243 179 242 245] \
	#(  ) ) >

<primitive 112 temp 15		" addLast: " \
	#( #[ 16 161 247 5 32 33 11 21 243 242 32 10 24 5 53 \
 160 33 11 50 93 11 51 11 51 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" addFirst: " \
	#( #[ 5 53 160 33 11 50 16 11 51 96 33 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" add: " \
	#( #[ 5 53 160 33 11 50 16 11 51 96 33 243 245] \
	#(  ) ) >

<primitive 98 #List \
	<primitive 97 #List #SequenceableCollection #.//list.st \
	#(  #first #current ) \
	#( #isEmpty #last #current #next #first #removeLast #removeFirst #removeError #remove:ifAbsent: #remove: #findLast #coerce: #addAllLast: #addAllFirst: #addLast: #addFirst: #add:  ) \
	temp 5 8 > >

