"
	Simple Minded simulation from Chapter 6 of book

	IceCream Store -
		single event queue
		multiple group size
		discrete probability on number of scoops selected
"
Class Main
[
	main		| i |
		i <- IceCreamStore new.
		[i time < 25] whileTrue: [ i proceed ].
		i reportProfits
]

Class Simulation
| currentTime nextEvent nextEventTime |
[
	new
		currentTime <- 0
|
	time
		^ currentTime
|
	addEvent: event at: eventTime
		nextEvent <- event.
		nextEventTime <- eventTime
|
	proceed
		currentTime <- nextEventTime.
		self processEvent: nextEvent
]

Class IceCreamStore :Simulation
| profit rand scoopDistribution |
[
	new
		profit <- 0.
		rand <- Random new.
		(scoopDistribution <- DiscreteProbability new)
			defineWeights: #(65 25 10).
		self scheduleArrival
|
	scheduleArrival
		self addEvent: Customer new
			at: (self time + (rand randInteger: 5))
|
	processEvent: event
		('customer received at ', self time printString) print.
		profit <- profit + ((self scoopsFor: event groupSize)  * 0.17 ).
		self scheduleArrival
|	
	scoopsFor: group		| number |
		number <- 0.
		group timesRepeat:
			[number <- number + scoopDistribution next].
		('group of ', group printString, ' have ', number
		printString, ' scoops ') print.
		^ number

|
	reportProfits
		('profits are ', profit printString) print
]

Class Customer
| groupSize |
[
	new
		groupSize <- (Random new "randomize" ) randInteger: 8
|
	groupSize
		^ groupSize
]

Class DiscreteProbability
| weights rand max |
[
	defineWeights: anArray
		weights <- anArray.
		(rand <- Random new) "randomize".
		max <- anArray inject: 0 into: [:x :y | x + y]
|
	next	| index value |
		value <- rand randInteger: max.
		index <- 1.
		[value > (weights at: index)]
			whileTrue: [value <- value - (weights at: index).
					index <- index + 1].
		^ index
]

