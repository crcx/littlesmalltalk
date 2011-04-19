temp <- <primitive 110 12 >
<primitive 112 temp 1		" checkBucket: " \
	#( #[ 16 33 241 97 177 241 98 161 247 2 93 243 242 18 166 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" currentKey " \
	#( #[ 18 162 247 11 18 10 21 113 33 162 247 3 33 10 49 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" printString " \
	#( #[ 32 32 164 169 48 11 17 226 1 17 33 32 171 169 11 \
 17 49 11 17 34 169 11 17 50 11 17 243 215 51 11 \
 17 243 245] \
	#( ' ( ' ' @ ' ' ' ')'  ) ) >

<primitive 112 temp 4		" next " \
	#( #[ 18 167 241 113 162 247 4 33 10 50 243 242 17 5 17 \
 199 247 20 17 81 192 97 32 17 11 24 241 113 162 247 \
 4 33 10 50 243 242 249 26 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" first " \
	#( #[ 81 5 17 178 225 2 14 32 34 11 24 241 113 162 247 \
 4 33 10 50 244 243 179 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" findAssociation:inList: " \
	#( #[ 34 225 3 10 35 10 49 33 201 247 2 35 244 243 179 \
 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" removeKey:ifAbsent: " \
	#( #[ 32 33 11 28 115 32 33 35 222 116 36 161 247 3 34 \
 165 243 242 35 36 224 3 34 165 244 216 10 50 243 245 \
] \
	#(  ) ) >

<primitive 112 temp 8		" at:ifAbsent: " \
	#( #[ 32 33 11 28 115 32 33 35 222 116 36 161 247 3 34 \
 165 243 242 36 10 50 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" at:put: " \
	#( #[ 32 33 11 28 115 32 33 35 222 116 36 161 247 15 5 \
 53 160 33 11 50 34 11 51 116 35 36 189 248 5 242 \
 36 34 11 51 242 34 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" getList: " \
	#( #[ 32 33 11 29 115 16 35 177 114 34 161 247 8 5 48 \
 160 114 16 35 34 208 242 34 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" hashNumber: " \
	#( #[ 33 250 1 5 16 163 195 81 192 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" new " \
	#( #[ 5 30 5 17 176 96 245] \
	#(  ) ) >

<primitive 98 #Dictionary \
	<primitive 97 #Dictionary #KeyedCollection #.//dictionary.st \
	#(  #hashTable #currentBucket #currentList ) \
	#( #checkBucket: #currentKey #printString #next #first #findAssociation:inList: #removeKey:ifAbsent: #at:ifAbsent: #at:put: #getList: #hashNumber: #new  ) \
	temp 5 6 > >

