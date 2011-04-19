Class Boolean
[
        &    aBoolean   
		^ self and: [aBoolean]

|       |    aBoolean  
		^ self or: [aBoolean]

|       and: aBlock   
		^ self and: [aBlock value]

|       or:  aBlock  
		^ self or: [aBlock value]

|	eqv: aBoolean
		^ self == aBoolean

|	xor: aBoolean
		^ self ~~ aBoolean
]
