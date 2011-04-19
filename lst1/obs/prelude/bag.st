Class Bag :Collection
| dict count |
[
        new
                dict <- Dictionary new

|       add: newElement
                dict at: newElement 
                     put: (1 + (dict at: newElement ifAbsent: [0]))

|       add: newObj withOccurrences: anInteger
                anInteger timesRepeat: [ self add: newObj ].
                ^ newObj

|       remove: oldElement ifAbsent: exceptionBlock   | i |
                i <- dict at: oldElement 
                          ifAbsent: [ ^ exceptionBlock value].
                (1 = i) ifTrue:  [dict removeKey: oldElement]
                        ifFalse: [dict at: oldElement put: i - 1 ]

|       size
                ^ dict inject: 0 into: [:x :y | x + y]

|       occurrencesOf: anElement
                ^ dict at: anElement ifAbsent: [0]

|       first
		(count <- dict first) isNil ifTrue: [^ nil].
		count <- count - 1.
		^ dict currentKey

|       next
		[count notNil] whileTrue:
		   [ (count > 0)
		        ifTrue: [count <- count - 1. ^ dict currentKey]
			ifFalse: [(count <- dict next) isNil
					ifTrue: [^ nil] ]].
		^ nil

]
