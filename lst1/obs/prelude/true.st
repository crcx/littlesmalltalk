Class True :Boolean
[
        ifTrue: trueAlternativeBlock ifFalse: falseAlternativeBlock
                ^ trueAlternativeBlock value

!       ifFalse: falseAlternativeBlock ifTrue: trueAlternativeBlock
                ^ trueAlternativeBlock value

!       ifTrue: trueAlternativeBlock
                ^ trueAlternativeBlock value

!       ifFalse: falseAlternativeBlock
                ^ nil

|       not
                ^ false
]
