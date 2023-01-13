"
     control the values produced by a generator
"
Class ControlGenerator :Generator
| firstGenerator secondGenerator
  currentFirst currentSecond
  controlBlock computeBlock |
[
        initA: fGen b: sGen control: aBlock compute: anotherBlock
                firstGenerator <- fGen.
                secondGenerator <- sGen.
                controlBlock <- aBlock.
                computeBlock <- anotherBlock

|       first
                currentFirst <- firstGenerator first.
                currentSecond <- secondGenerator first.
                (currentFirst isNil & currentSecond isNil) ifTrue: [^ nil].
                ^ self controlGeneratorNext

|       next
                ^ self controlGeneratorNext

|       controlGeneratorNext    | control returnedValue |
                control <- 0.
                [ control anyMask: 12] whileFalse: [
                  control <- controlBlock value: currentFirst
                                          value: currentSecond.
                   (control allMask: 64) ifTrue: [^nil].
                   (control allMask: 32) ifTrue:
                                [currentFirst <- firstGenerator first].
                   (control allMask: 16) ifTrue:
                                [currentSecond <- secondGenerator first].
                   (control allMask: 12)
                      ifTrue:
                          [returnedValue <- computeBlock
                               value: currentFirst value: currentSecond]
                      ifFalse: [
                         (control allMask: 8) ifTrue:
                           [returnedValue <- computeBlock value: currentFirst].
                         (control allMask: 4) ifTrue:
                           [returnedValue <- computeBlock value: currentSecond].
                         ].
                   (control allMask: 2) ifTrue:
                           [currentFirst <- firstGenerator next].
                   (control allMask: 1) ifTrue:
                           [currentSecond <- secondGenerator next].
                  ].
                ^ returnedValue
]
