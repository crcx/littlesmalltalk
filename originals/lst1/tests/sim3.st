"
	Simple Minded simulation from Chapter 6 of book

	IceCream Store -
		multiple event queue
"
Class Main
[
	main		| i |
		i <- IceCreamStore new.
		[i time < 60] whileTrue: [ i proceed ].
		i reportProfits
]

Class Simulation
| currentTime eventQueue |
[
	new
		eventQueue <- Dictionary new.
		currentTime <- 0
|
	time
		^ currentTime
|
	addEvent: event at: eventTime
		(eventQueue includesKey: eventTime)
			ifTrue: [(eventQueue at: eventTime) add: event]
			ifFalse: [eventQueue at: eventTime
					put: (Set new ; add: event)]
|	
	addEvent: event next: timeIncrement
		self addEvent: event at: currentTime + timeIncrement
|
	proceed		| minTime eventset event |
		minTime <- 99999.
		eventQueue keysDo:
			[:x | (x < minTime) ifTrue: [minTime <- x]].
		currentTime <- minTime.
		eventset <- eventQueue at: minTime ifAbsent: [^nil].
		event <- eventset first.
		eventset remove: event.
		(eventset isEmpty) ifTrue: [eventQueue removeKey: minTime].
		self processEvent: event
]

Class IceCreamStore :Simulation
| profit arrivalDistribution rand scoopDistribution remainingChairs |
[
	new
		profit <- 0.
		remainingChairs <- 15.
		rand <- Random new.
		(arrivalDistribution <- Normal new)
			setMean: 3.0 deviation: 1.0.
		(scoopDistribution <- DiscreteProbability new)
			defineWeights: #(65 25 10).
		self scheduleArrival
|
	scheduleArrival			| newcustomer  time |
		newcustomer <- Customer new.
		time <- self time + (arrivalDistribution next).
		(time < 15) ifTrue: [
			self addEvent: [self customerArrival: newcustomer]
				at: time ]
|
	processEvent: event
		('event received at ', self time printString) print.
		event value.
		self scheduleArrival
|
	customerArrival: customer	| size |
		size <- customer groupSize.
		('group of size ', size printString , ' arrives') print.
		(size < remainingChairs)
			ifTrue: [remainingChairs <- remainingChairs - size.
				 'take chairs, schedule order' print.
				 self addEvent: 
					[self customerOrder: customer]
					next: (rand randInteger: 3).
				]
			ifFalse: ['finds no chairs, leave' print]
|
	customerOrder: customer		| size numScoops |
		size <- customer groupSize.
		numScoops <- 0.
		size timesRepeat: 
			[numScoops <- numScoops + scoopDistribution next].
		('group of size ', size printString, ' orders ' ,
		numScoops printString, ' scoops') print.
		profit <- profit + (numScoops * 0.17).
		self addEvent:
			[self customerLeave: customer]
			next: (rand randInteger: 5)
|
	customerLeave: customer		| size |
		size <- customer groupSize.
		('group of size ', size printString, ' leaves') print.
		remainingChairs <- remainingChairs + customer groupSize
|
	reportProfits
		('profits are ', profit printString) print
]

Class Customer
| groupSize |
[
	new
		groupSize <- (Random new "randomize") randInteger: 8
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

Class Normal :Random
| mean deviation |
[
	new
		self setMean: 1.0 deviation: 0.5
|
	setMean: m deviation: s
		mean <- m.
		deviation <- s
|
	next		| v1 v2 s u |
		s <- 1.
		[s >= 1] whileTrue:
			[v1 <- (2 * super next) - 1.
			 v2 <- (2 * super next) - 1.
			  s <- v1 squared + v2 squared ].
		u <- (-2.0 * s ln / s) sqrt.
		^ mean + (deviation * v1 * u)
]
