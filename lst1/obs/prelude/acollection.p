temp <- <primitive 110 13 >
<primitive 112 temp 1		" shallowCopy " \
	#( #[ 32 164 32 163 176 113 81 32 163 178 225 2 7 33 34 \
 32 34 177 208 243 179 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" padTo: " \
	#( #[ 32 163 33 199 247 12 32 32 164 33 32 163 193 176 11 \
 17 248 2 242 32 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" next " \
	#( #[ 16 81 192 96 16 32 163 200 247 3 32 16 177 243 245 \
] \
	#(  ) ) >

<primitive 112 temp 4		" lastKey " \
	#( #[ 32 163 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" firstKey " \
	#( #[ 81 243 245] \
	#(  ) ) >

<primitive 112 temp 6		" first " \
	#( #[ 81 96 16 32 163 200 247 3 32 16 177 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" do: " \
	#( #[ 81 32 163 178 225 2 8 34 96 33 32 34 177 180 243 \
 179 242 245] \
	#(  ) ) >

<primitive 112 temp 8		" deepCopy " \
	#( #[ 32 164 32 163 176 113 81 32 163 178 225 2 9 33 34 \
 32 34 177 10 20 208 243 179 242 33 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" currentKey " \
	#( #[ 16 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" copyFrom:to: " \
	#( #[ 34 33 193 81 192 115 32 164 35 176 116 36 81 35 32 \
 33 132 0 242 36 243 245] \
	#( #replaceFrom:to:with:startingAt:  ) ) >

<primitive 112 temp 11		" coerce: " \
	#( #[ 32 164 33 163 176 114 34 81 33 163 33 131 0 242 34 \
 243 245] \
	#( #replaceFrom:to:with:  ) ) >

<primitive 112 temp 12		" at:ifAbsent: " \
	#( #[ 33 80 200 251 4 33 32 163 204 247 3 34 165 243 242 \
 32 33 177 243 245] \
	#(  ) ) >

<primitive 112 temp 13		" = " \
	#( #[ 32 163 33 163 202 247 2 92 243 242 80 114 32 225 3 \
 14 35 33 34 81 192 241 114 177 202 247 2 92 244 243 \
 179 242 91 243 245] \
	#(  ) ) >

<primitive 98 #ArrayedCollection \
	<primitive 97 #ArrayedCollection #SequenceableCollection #.//acollection.st \
	#(  #current ) \
	#( #shallowCopy #padTo: #next #lastKey #firstKey #first #do: #deepCopy #currentKey #copyFrom:to: #coerce: #at:ifAbsent: #=  ) \
	temp 5 7 > >

