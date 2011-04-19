temp <- <primitive 110 8 >
<primitive 112 temp 1		" yield " \
	#( #[ 93 243 245] \
	#(  ) ) >

<primitive 112 temp 2		" unblock " \
	#( #[ 32 10 45 48 181 247 7 32 49 11 47 242 93 243 242 \
 32 83 250 2 145 242 32 10 45 243 245] \
	#( #TERMINATED 'unblock'  ) ) >

<primitive 112 temp 3		" termErr: " \
	#( #[ 48 33 11 17 49 11 17 168 242 245] \
	#( 'Cannot ' ' a terminated process.'  ) ) >

<primitive 112 temp 4		" terminate " \
	#( #[ 32 250 1 142 242 32 10 45 243 245] \
	#(  ) ) >

<primitive 112 temp 5		" state " \
	#( #[ 32 250 1 146 113 33 80 201 247 4 48 113 33 243 242 \
 33 81 201 247 4 49 113 33 243 242 33 82 201 247 4 \
 50 113 33 243 242 33 83 201 247 4 50 113 33 243 242 \
 33 84 203 247 4 51 113 33 243 242 245] \
	#( #READY #SUSPENDED #BLOCKED #TERMINATED  ) ) >

<primitive 112 temp 6		" suspend " \
	#( #[ 32 10 45 48 181 247 7 32 49 11 47 242 93 243 242 \
 32 81 250 2 145 242 32 10 45 243 245] \
	#( #TERMINATED 'suspend'  ) ) >

<primitive 112 temp 7		" resume " \
	#( #[ 32 10 45 48 181 247 7 32 49 11 47 242 93 243 242 \
 32 80 250 2 145 242 32 10 45 243 245] \
	#( #TERMINATED 'resume'  ) ) >

<primitive 112 temp 8		" block " \
	#( #[ 32 10 45 48 181 247 7 32 49 11 47 242 93 243 242 \
 32 82 250 2 145 242 32 10 45 243 245] \
	#( #TERMINATED 'block'  ) ) >

<primitive 98 #Process \
	<primitive 97 #Process #Object #.//process.st \
	#(  ) \
	#( #yield #unblock #termErr: #terminate #state #suspend #resume #block  ) \
	temp 2 4 > >

