Class Queen
| myrow mycolumn neighbor boardsize |
[
        build: aQueen col: aNumber size: brdmax

                neighbor <- aQueen.
                mycolumn <- aNumber.
                myrow <- 1.
                boardsize <- brdmax.
                neighbor first.
                ^ self

|       checkCol: colNumber row: rowNumber      | cd |
                (rowNumber = myrow) ifTrue: [ ^ false ].
                cd <- colNumber - mycolumn.
                ((myrow + cd) = rowNumber) ifTrue: [ ^ false ].
                ((myrow - cd) = rowNumber) ifTrue: [ ^ false ].
                (neighbor isNil) ifFalse:
                        [ ^ neighbor checkCol: colNumber row: rowNumber ].
                ^ true

|       first
                myrow <- 1.
                ^ self checkrow

|       next
                myrow <- myrow + 1.
                ^ self checkrow

|       checkrow
                (neighbor isNil) ifTrue: [^ myrow].
                [myrow <= boardsize] whileTrue:
                        [(neighbor checkCol: mycolumn row: myrow)
                                ifTrue: [^ myrow]
                                ifFalse: [myrow <- myrow + 1] ].
                ((neighbor next) isNil) ifTrue: [^ nil].
                ^ self first

|       printboard
                (neighbor isNil) ifFalse: [ neighbor printboard].
                ('Col ', mycolumn asString , ' Row ' ,
                    myrow asString) print
]

Class Main
| lastq |
[
        main | size |

                size <- 4.
                lastq <- nil.
                (1 to: size) do: [:x |
                         lastq <- Queen new build: lastq col: x size: size ].
                lastq first.
                lastq printboard
]
