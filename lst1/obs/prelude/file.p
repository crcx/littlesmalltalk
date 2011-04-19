temp <- <primitive 110 13 >
<primitive 112 temp 1		" write: " \
	#( #[ 32 33 250 2 132 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" size " \
	#( #[ 32 250 1 134 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" read " \
	#( #[ 32 250 1 131 243 245] \
	#(  ) ) >

<primitive 112 temp 4		" open:for: " \
	#( #[ 32 33 34 250 3 130 242 245] \
	#(  ) ) >

<primitive 112 temp 5		" open: " \
	#( #[ 32 33 48 250 3 130 242 245] \
	#( 'r'  ) ) >

<primitive 112 temp 6		" next " \
	#( #[ 32 10 36 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" first " \
	#( #[ 32 80 177 243 245] \
	#(  ) ) >

<primitive 112 temp 8		" currentKey " \
	#( #[ 32 250 1 136 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" at:put: " \
	#( #[ 32 33 250 2 135 242 32 34 11 49 243 245] \
	#(  ) ) >

<primitive 112 temp 10		" at: " \
	#( #[ 32 33 250 2 135 242 32 10 36 243 245] \
	#(  ) ) >

<primitive 112 temp 11		" modeString " \
	#( #[ 32 81 250 2 133 242 245] \
	#(  ) ) >

<primitive 112 temp 12		" modeInteger " \
	#( #[ 32 82 250 2 133 242 245] \
	#(  ) ) >

<primitive 112 temp 13		" modeCharacter " \
	#( #[ 32 80 250 2 133 242 245] \
	#(  ) ) >

<primitive 98 #File \
	<primitive 97 #File #SequenceableCollection #.//file.st \
	#(  ) \
	#( #write: #size #read #open:for: #open: #next #first #currentKey #at:put: #at: #modeString #modeInteger #modeCharacter  ) \
	temp 3 4 > >

