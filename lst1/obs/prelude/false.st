Class False :Boolean
[
        ifTrue: trueAlternativeBlock ifFalse: falseAlternativeBlock
                ^ falseAlternativeBlock value

!       ifFalse: falseAlternativeBlock ifTrue: trueAlternativeBlock
                ^ falseAlternativeBlock value

!       ifTrue: trueAlternativeBlock
                ^ nil

!       ifFalse: falseAlternativeBlock
                ^ falseAlternativeBlock value

!       not
                ^ true
]
