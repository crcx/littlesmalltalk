temp <- <primitive 110 19 >
<primitive 112 temp 1		" values " \
	#( #[ 5 32 160 113 32 225 2 4 33 34 189 243 179 242 33 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" select: " \
	#( #[ 32 32 5 40 160 226 2 13 33 35 180 247 5 34 32 \
 171 35 208 242 34 243 215 191 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" removeKey:ifAbsent: " \
	#( #[ 32 48 188 243 245] \
	#( 'subclass should implement RemoveKey:ifAbsent:'  ) ) >

<primitive 112 temp 4		" removeKey: " \
	#( #[ 32 33 224 6 32 48 188 242 33 244 217 243 245] \
	#( 'no element associated with key'  ) ) >

<primitive 112 temp 5		" remove: " \
	#( #[ 32 48 188 243 245] \
	#( 'object must be removed with explicit key'  ) ) >

<primitive 112 temp 6		" keysSelect: " \
	#( #[ 32 32 5 40 160 226 2 14 33 35 171 180 247 5 34 \
 32 171 35 208 242 34 243 215 191 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" keysDo: " \
	#( #[ 32 225 2 5 33 32 171 180 243 179 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" keys " \
	#( #[ 5 57 160 113 32 225 2 4 33 34 189 243 11 32 242 \
 33 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" indexOf:ifAbsent: " \
	#( #[ 32 225 3 9 35 33 201 247 3 32 171 244 243 179 242 \
 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" indexOf: " \
	#( #[ 32 33 224 4 32 48 188 243 214 243 245] \
	#( 'indexOf element not found'  ) ) >

<primitive 112 temp 11		" includesKey: " \
	#( #[ 32 33 224 2 92 244 213 242 91 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" collect: " \
	#( #[ 32 32 5 40 160 226 2 10 34 32 171 33 35 180 208 \
 242 34 243 215 191 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" coerce: " \
	#( #[ 32 164 160 114 33 226 3 5 34 35 36 208 243 11 23 \
 242 34 243 245] \
	#(  ) ) >

<primitive 112 temp 14		" binaryDo: " \
	#( #[ 32 225 3 6 33 32 171 35 211 243 179 242 93 243 245 \
] \
	#(  ) ) >

<primitive 112 temp 15		" atAll:put: " \
	#( #[ 33 225 3 5 32 35 34 208 243 179 242 245] \
	#(  ) ) >

<primitive 112 temp 16		" at: " \
	#( #[ 32 33 224 10 32 33 169 48 11 17 188 242 33 244 213 \
 243 245] \
	#( ': association not found'  ) ) >

<primitive 112 temp 17		" asDictionary " \
	#( #[ 5 40 160 113 32 226 2 5 33 34 35 208 243 11 23 \
 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" addAll: " \
	#( #[ 33 226 2 5 32 34 35 208 243 11 23 242 33 243 245 \
] \
	#(  ) ) >

<primitive 112 temp 19		" add: " \
	#( #[ 32 48 188 243 245] \
	#( 'Must add with explicit key'  ) ) >

<primitive 98 #KeyedCollection \
	<primitive 97 #KeyedCollection #Collection #.//kcollection.st \
	#(  ) \
	#( #values #select: #removeKey:ifAbsent: #removeKey: #remove: #keysSelect: #keysDo: #keys #indexOf:ifAbsent: #indexOf: #includesKey: #collect: #coerce: #binaryDo: #atAll:put: #at: #asDictionary #addAll: #add:  ) \
	temp 5 9 > >

