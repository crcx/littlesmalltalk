temp <- <primitive 110 23 >
<primitive 112 temp 1		" size " \
	#( #[ 80 113 32 225 2 6 33 81 192 241 113 243 179 242 33 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" shallowCopy " \
	#( #[ 5 48 160 113 32 225 2 5 33 34 11 22 243 179 242 \
 32 33 191 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" select: " \
	#( #[ 32 32 164 160 226 2 11 33 35 180 247 3 34 35 189 \
 242 34 243 215 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" remove:ifAbsent: " \
	#( #[ 32 33 11 30 247 6 32 33 11 39 248 3 242 34 165 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" remove: " \
	#( #[ 32 33 224 4 32 48 188 244 216 242 33 243 245] \
	#( 'attempt to remove object not found in collection'  ) ) >

<primitive 112 temp 6		" reject: " \
	#( #[ 32 225 2 5 33 34 180 172 243 11 44 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" printString " \
	#( #[ 32 32 164 169 48 11 17 226 1 9 33 49 11 17 34 \
 169 11 17 243 215 50 11 17 243 245] \
	#( ' (' ' ' ' )'  ) ) >

<primitive 112 temp 8		" occurrencesOf: " \
	#( #[ 32 80 226 2 13 35 33 201 247 5 34 81 192 248 2 \
 242 34 243 215 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" isEmpty " \
	#( #[ 32 163 80 201 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" inject:into: " \
	#( #[ 33 115 32 225 4 7 34 35 36 211 241 115 243 179 242 \
 35 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" includes: " \
	#( #[ 32 225 2 8 34 33 181 247 2 91 244 243 179 242 92 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" first " \
	#( #[ 32 48 188 243 245] \
	#( 'subclass should implement first'  ) ) >

<primitive 112 temp 13		" detect:ifAbsent: " \
	#( #[ 32 225 3 8 33 35 180 247 2 35 244 243 179 242 34 \
 165 243 245] \
	#(  ) ) >

<primitive 112 temp 14		" detect: " \
	#( #[ 32 33 224 4 32 48 188 243 223 243 245] \
	#( 'no object found matching detect'  ) ) >

<primitive 112 temp 15		" deepCopy " \
	#( #[ 5 48 160 113 32 225 2 7 33 34 10 20 11 22 243 \
 179 242 32 33 191 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" collect: " \
	#( #[ 32 32 164 160 226 2 8 34 33 35 180 189 242 34 243 \
 215 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" coerce: " \
	#( #[ 32 164 160 114 33 225 3 4 34 35 189 243 179 242 34 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" asString " \
	#( #[ 5 58 32 163 176 241 81 32 163 32 131 0 242 243 245 \
] \
	#( #replaceFrom:to:with:  ) ) >

<primitive 112 temp 19		" asList " \
	#( #[ 5 48 160 32 11 20 243 245] \
	#(  ) ) >

<primitive 112 temp 20		" asSet " \
	#( #[ 5 57 160 32 11 19 243 245] \
	#(  ) ) >

<primitive 112 temp 21		" asBag " \
	#( #[ 5 32 160 32 11 19 243 245] \
	#(  ) ) >

<primitive 112 temp 22		" asArray " \
	#( #[ 5 30 32 163 176 241 81 32 163 32 131 0 242 243 245 \
] \
	#( #replaceFrom:to:with:  ) ) >

<primitive 112 temp 23		" addAll: " \
	#( #[ 33 225 2 4 32 34 189 243 179 242 245] \
	#(  ) ) >

<primitive 98 #Collection \
	<primitive 97 #Collection #Object #.//collection.st \
	#(  ) \
	#( #size #shallowCopy #select: #remove:ifAbsent: #remove: #reject: #printString #occurrencesOf: #isEmpty #inject:into: #includes: #first #detect:ifAbsent: #detect: #deepCopy #collect: #coerce: #asString #asList #asSet #asBag #asArray #addAll:  ) \
	temp 5 7 > >

