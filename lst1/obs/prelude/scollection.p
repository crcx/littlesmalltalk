temp <- <primitive 110 21 >
<primitive 112 temp 1		" with:do: " \
	#( #[ 32 166 115 33 166 116 35 162 247 15 34 35 36 211 242 \
 32 167 115 33 167 241 116 242 249 19 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" sort: " \
	#( #[ 32 174 116 82 36 163 178 225 5 51 37 81 193 114 34 \
 81 203 252 11 33 36 34 177 36 34 81 192 177 211 172 \
 247 28 36 34 177 115 36 34 36 34 81 192 177 208 242 \
 36 34 81 192 35 208 242 34 81 193 241 114 242 249 46 \
 243 179 242 32 36 191 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" sort " \
	#( #[ 32 226 1 4 33 34 200 243 11 46 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" select: " \
	#( #[ 32 32 5 48 160 226 2 12 33 35 180 247 4 34 35 \
 11 22 242 34 243 215 191 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" reversed " \
	#( #[ 5 30 32 163 241 114 176 113 32 225 3 11 33 34 35 \
 208 242 34 81 193 241 114 243 179 242 32 33 191 243 245 \
] \
	#(  ) ) >

<primitive 112 temp 6		" reverseDo: " \
	#( #[ 32 10 31 114 32 10 25 115 34 35 203 247 20 32 34 \
 11 52 247 5 33 32 34 177 180 242 34 81 193 241 114 \
 242 249 25 242 93 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" replaceFrom:to:with:startingAt: " \
	#( #[ 80 117 33 37 192 34 200 247 19 32 33 37 192 35 37 \
 36 192 177 208 242 37 81 192 241 117 242 249 26 242 245 \
] \
	#(  ) ) >

<primitive 112 temp 8		" replaceFrom:to:with: " \
	#( #[ 35 33 226 4 9 32 36 37 208 242 36 81 192 243 215 \
 242 245] \
	#(  ) ) >

<primitive 112 temp 9		" last " \
	#( #[ 80 32 163 201 246 5 32 32 10 31 177 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" indexOfSubCollection:startingAt: " \
	#( #[ 32 33 34 224 6 32 48 188 242 93 243 131 1 243 245 \
] \
	#( 'element not found' #indexOfSubCollection:startingAt:ifAbsent:  ) ) >

<primitive 112 temp 11		" indexOfSubCollection:startingAt:ifAbsent: " \
	#( #[ 34 116 32 163 33 163 193 117 36 37 200 247 17 33 32 \
 36 221 247 2 36 243 242 36 81 192 241 116 242 249 22 \
 242 35 165 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" findLast:ifAbsent: " \
	#( #[ 32 225 3 9 33 35 180 247 3 32 171 244 243 11 42 \
 242 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" findLast: " \
	#( #[ 32 33 224 4 32 48 188 243 220 242 245] \
	#( 'last element not found'  ) ) >

<primitive 112 temp 14		" findFirst:ifAbsent: " \
	#( #[ 32 225 3 9 33 35 180 247 3 32 171 244 243 179 242 \
 34 165 243 245] \
	#(  ) ) >

<primitive 112 temp 15		" findFirst: " \
	#( #[ 32 33 224 4 32 48 188 243 219 243 245] \
	#( 'first element not found'  ) ) >

<primitive 112 temp 16		" equals:startingAt: " \
	#( #[ 80 115 32 225 4 22 36 33 34 35 192 224 2 92 244 \
 213 201 246 2 92 244 242 35 81 192 241 115 243 179 242 \
 91 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" copyWithout: " \
	#( #[ 5 48 160 114 32 225 3 10 35 33 181 246 4 34 35 \
 11 22 243 179 242 32 34 191 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" copyWith: " \
	#( #[ 32 5 48 160 241 32 11 20 242 241 33 11 22 242 191 \
 243 245] \
	#(  ) ) >

<primitive 112 temp 19		" copyFrom:to: " \
	#( #[ 5 48 160 115 33 34 178 225 4 7 35 32 36 177 11 \
 22 243 179 242 32 35 191 243 245] \
	#(  ) ) >

<primitive 112 temp 20		" collect: " \
	#( #[ 32 32 5 48 160 226 2 9 34 33 35 180 11 22 242 \
 34 243 215 191 243 245] \
	#(  ) ) >

<primitive 112 temp 21		" , " \
	#( #[ 32 5 48 160 241 32 11 20 242 241 33 11 20 242 191 \
 243 245] \
	#(  ) ) >

<primitive 98 #SequenceableCollection \
	<primitive 97 #SequenceableCollection #KeyedCollection #.//scollection.st \
	#(  ) \
	#( #with:do: #sort: #sort #select: #reversed #reverseDo: #replaceFrom:to:with:startingAt: #replaceFrom:to:with: #last #indexOfSubCollection:startingAt: #indexOfSubCollection:startingAt:ifAbsent: #findLast:ifAbsent: #findLast: #findFirst:ifAbsent: #findFirst: #equals:startingAt: #copyWithout: #copyWith: #copyFrom:to: #collect: #,  ) \
	temp 6 9 > >

