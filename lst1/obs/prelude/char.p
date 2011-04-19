temp <- <primitive 110 18 >
<primitive 112 temp 1		" printString " \
	#( #[ 48 32 250 1 58 11 17 243 245] \
	#( '$'  ) ) >

<primitive 112 temp 2		" isVowel " \
	#( #[ 32 250 1 51 243 245] \
	#(  ) ) >

<primitive 112 temp 3		" isUppercase " \
	#( #[ 32 48 49 218 243 245] \
	#( $A $Z  ) ) >

<primitive 112 temp 4		" isSeparator " \
	#( #[ 32 250 1 55 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" isLowercase " \
	#( #[ 32 48 49 218 243 245] \
	#( $a $z  ) ) >

<primitive 112 temp 6		" isLetter " \
	#( #[ 32 10 28 251 3 32 10 29 243 245] \
	#(  ) ) >

<primitive 112 temp 7		" isDigit " \
	#( #[ 32 48 49 218 243 245] \
	#( $0 $9  ) ) >

<primitive 112 temp 8		" isAlphaNumeric " \
	#( #[ 32 250 1 56 243 245] \
	#(  ) ) >

<primitive 112 temp 9		" digitValue " \
	#( #[ 32 250 1 50 241 113 161 247 3 32 48 188 242 33 243 \
 245] \
	#( 'digitValue on nondigit char'  ) ) >

<primitive 112 temp 10		" compareError " \
	#( #[ 32 48 188 243 245] \
	#( 'char cannot be compared to non char'  ) ) >

<primitive 112 temp 11		" asString " \
	#( #[ 32 250 1 58 243 245] \
	#(  ) ) >

<primitive 112 temp 12		" asUppercase " \
	#( #[ 32 250 1 53 247 6 32 250 1 57 248 2 242 32 243 \
 245] \
	#(  ) ) >

<primitive 112 temp 13		" asLowercase " \
	#( #[ 32 250 1 54 247 6 32 250 1 57 248 2 242 32 243 \
 245] \
	#(  ) ) >

<primitive 112 temp 14		" asciiValue " \
	#( #[ 32 250 1 59 243 245] \
	#(  ) ) >

<primitive 112 temp 15		" > " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 43 248 4 242 \
 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 16		" = " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 46 248 4 242 \
 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 17		" < " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 42 248 4 242 \
 32 10 19 243 245] \
	#(  ) ) >

<primitive 112 temp 18		" == " \
	#( #[ 32 33 250 2 6 247 7 32 33 250 2 46 248 2 242 \
 92 243 245] \
	#(  ) ) >

<primitive 98 #Char \
	<primitive 97 #Char #Magnitude #.//char.st \
	#(  ) \
	#( #printString #isVowel #isUppercase #isSeparator #isLowercase #isLetter #isDigit #isAlphaNumeric #digitValue #compareError #asString #asUppercase #asLowercase #asciiValue #> #= #< #==  ) \
	temp 2 4 > >

