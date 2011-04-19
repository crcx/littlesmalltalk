"
	Simple Minded simulation from Chapter 6 of book
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
| profit rand |
[
	new
		profit <- 0.
		rand <- Random new.
		"rand randomize.  taken out so results remain the same"
		self scheduleArrival
|
	scheduleArrival
		self addEvent: Customer new
			at: (self time + (rand randInteger: 5))
|
	processEvent: event
		('customer received at ', self time printString) print.
		profit <- profit + ( event numberOfScoops * 0.17 ).
		self scheduleArrival
|
	reportProfits
		('profits are ', profit printString) print
]

Class Customer
| rand |
[
	new
		(rand <- Random new) "--randomize (taken out)"
|
	numberOfScoops		| number |
		number <- rand randInteger: 3.
		('customer has ', number printString , ' scoops ') print.
		^ number
]
