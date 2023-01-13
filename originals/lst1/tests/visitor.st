Class SimulationObject :Object	
	| sizeDist waitDist |
[
	init
		sizeDist <- Binomial new initialize events: 5 mean: 0.4.
		waitDist <- Random new	"uniform distribution"

|	size
		^ sizeDist next

|	wait: sizeGroup	  "uniform distribution from 1 to 6"
		^ waitDist next * sizeGroup * 6
]

Class Visitor	:SimulationObject
	| sizeGroup wait alreadyEaten |
[
	initialize: superClass
		sizeGroup <- superClass size.
		wait <- superClass wait: sizeGroup.
		alreadyEaten <- false

|	entering
		(alreadyEaten == false)
		     ifTrue: [alreadyEaten <- true. ^ true].
		^ false

|	time	
		^ wait

|	groupSize
		^ sizeGroup

]
