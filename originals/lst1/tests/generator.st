Class Generator :Collection
[
	, aGenerator
		^ DyadicControlGenerator new;
			firstGen: self
			secondGen: aGenerator
			control: [:x :y |
				(x isNil)
					ifTrue:
						[(y isNil)
							ifTrue:  [2r01000000]
							ifFalse: [2r00000101]
						]
					ifFalse: [2r00001010] ]
			compute: [:x | x ]
|
  	collect: xformBlock
    		^ MonadicControlGenerator new;
       			initGen: self deepCopy
       			control: [ :x | 
				(x isNil) 
					ifTrue:  [2r1000] 
					ifFalse: [2r0101] 
				 ]
       			init: []
       			compute: [:x | xformBlock value: x]
|
	first: limit     | count |
    		count <- 0.
		^ MonadicControlGenerator new;
       			initGen: self deepCopy
       			control: [ :x |
                 			(x isNil)
                  			ifTrue:  [2r1000]
                  			ifFalse: [((count <- count + 1) > limit)
                             				ifTrue:  [2r1000]
                             				ifFalse: [2r0101]
                           			 ]
                		 ]
       			init: [count <- 0]
       			compute: [:x | x]
|
  	select: condBlock
    		^ MonadicControlGenerator new;
       			initGen: self deepCopy
       			control: [ :x |
                 		(x isNil)
                  			ifTrue:  [2r1000]
                  			ifFalse: [(condBlock value: x)
                             				ifTrue:  [2r0101]
                             				ifFalse: [2r0001]
                           			 ]
                		 ]
       			init: []
       			compute: [:x | x]
|
  	until: condBlock
    		^ MonadicControlGenerator new;
       			initGen: self deepCopy
       			control: [ :x |
                 		(x isNil)
                  			ifTrue:  [2r1000]
                  			ifFalse: [(condBlock value: x)
                             				ifTrue:  [2r1000]
                             				ifFalse: [2r0101]
                           			 ]
                		 ]
       			init: []
       			compute: [:x | x]
|
	with: aGenerator when: conditionBlock
		^ DyadicControlGenerator new ;
			firstGen: self
			secondGen: aGenerator
			control: [:x :y |
				(x isNil)
					ifTrue: [(y isNil)
						ifTrue:  [2r01000000]
						ifFalse: [2r00000101] ]
					ifFalse: [(y isNil)
						ifTrue:  [2r00001010]
						ifFalse: [(conditionBlock
							value: x value: y)
							ifTrue:  [2r00001010]
							ifFalse: [2r00000101]
							] ] ]
			compute: [:x | x ]
]

Class MonadicControlGenerator :Generator
| subGenerator  currentValue  controlBlock  initBlock  computeBlock |
[
  	initGen: aGenerator 
	control: conBlk 
	init: iniBlk 
	compute: cmpBlk
    		subGenerator <- aGenerator.
    		controlBlock <- conBlk.
    		initBlock <- iniBlk.
    		computeBlock <- cmpBlk.
    		currentValue <- nil
|
  	first
    		(currentValue <- subGenerator first) isNil
      			ifTrue: [^ nil].
    		initBlock value.
    		^ self next
|
  	next     | control  returnedValue |
    		control <- 0.
    		[control anyMask: 2r0100] whileFalse:
      			[
        			control <- controlBlock value: currentValue.

        			(control anyMask: 2r1000) ifTrue:
          				[^ nil].
        			(control anyMask: 2r0100) ifTrue:
          				[returnedValue <- 
						computeBlock value: currentValue].
        			(control anyMask: 2r0010) ifTrue:
          				[currentValue <- subGenerator first].
        			(control anyMask: 2r0001) ifTrue:
				    [currentValue <- subGenerator next]
  			].
    		^ returnedValue
]
Class DyadicControlGenerator :Generator
| firstGenerator secondGenerator
  currentFirst currentSecond
  controlBlock computeBlock |
[
	firstGen: firstGen
	secondGen: secondGen
	control: contBlock
	compute: compBlock

                firstGenerator <- firstGen.
                secondGenerator <- secondGen.
                controlBlock <- contBlock.
                computeBlock <- compBlock

|       first
                currentFirst <- firstGenerator first.
                currentSecond <- secondGenerator first.
                (currentFirst isNil & currentSecond isNil) ifTrue: [^ nil].
                ^ self next

|       next    	| control returnedValue |
                control <- 0.
                [ control anyMask: 2r00001100] whileFalse: [
                  control <- controlBlock value: currentFirst
                                          value: currentSecond.
                   (control allMask: 2r01000000) ifTrue: [^nil].
                   (control allMask: 2r00100000) ifTrue:
                                [currentFirst <- firstGenerator first].
                   (control allMask: 2r00010000) ifTrue:
                                [currentSecond <- secondGenerator first].
                   (control allMask: 2r00001100)
                      ifTrue:
                          [returnedValue <- computeBlock
                               value: currentFirst value: currentSecond]
                      ifFalse: [
                         (control allMask: 2r00001000) ifTrue:
                           [returnedValue <- computeBlock value: currentFirst].
                         (control allMask: 2r00000100) ifTrue:
                           [returnedValue <- computeBlock value: currentSecond].
                         ].
                   (control allMask: 2r00000010) ifTrue:
                           [currentFirst <- firstGenerator next].
                   (control allMask: 2r00000001) ifTrue:
                           [currentSecond <- secondGenerator next].
                  ].
                ^ returnedValue
]
