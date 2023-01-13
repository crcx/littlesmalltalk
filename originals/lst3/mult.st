*
* Little Smalltalk, version 3
* Written by Tim Budd, Oregon State University, July 1988
*
* multiprocess scheduler 
*
* if event driven interface (stdwin) is used the event manager sits
*  below the multiprocess scheduler
*
Class Process Object stack stackTop linkPointer
Class Scheduler Object notdone processList currentProcess
Class Semaphore Object count processList
Methods Block 'forks'
	newProcess 
		" create a new process to execute block "
		^ Process new; context: context ; startAt: bytePointer.
|
	newProcessWith: args
		(self checkArgumentCount: args size)
			ifTrue: [ (1 to: args size) do: [:i |
				   context at: (argLoc + i - 1) 
					put: (args at: i)]].
		^ self newProcess
|
	fork
		self newProcess resume
|
	forkWith: args
		(self newProcessWith: args) resume
]
Methods Process 'all'
	execute 
		" execute for time slice, terminating if all over "
		(stack size > 1500)
			ifTrue: [ smalltalk error:
				'process stack overflow, probable loop'].
		<19 self> ifTrue: [] ifFalse: [ self terminate ].
|
	context
		^ stack at: 3
|
	resume
		" resume current process "
		scheduler addProcess: self
|
	terminate
		" kill current process "
		scheduler removeProcess: self. scheduler yield.
|
	trace		| link m r s |
		" first yield scheduler, forceing store of linkPointer"
		scheduler yield.
		link <- linkPointer.
		link <- stack at: link+1.
		" then trace back chain "
		[ link notNil ] whileTrue:
			[ m <- stack at: link+3. 
			  m notNil 
				ifTrue: [ s <- m signature, ' ('.
			  		  r <- stack at: link+2.
			  		  (r to: link-1) do: 
						[:x | s <- s, ' ', 
							(stack at: x) class asString].
					  (s, ')') print ].
			  link <- stack at: link ]
]
Methods Scheduler 'all'
	new
		"create a new scheduler with empty process list "
		notdone <- true.
		processList <- Set new.
|
	addProcess: aProcess
		" add a process to the process list "
		processList add: aProcess
|
	critical: aBlock
		"set time slice counter high to insure bytecodes are
		executed before continuing "
		<53 10000>.
		aBlock value.
		"then yield processor "
		<53 0>.
|
	currentProcess
		" return the currently executing process "
		^ currentProcess
|
	removeProcess: aProcess
		" remove a given process from the process list "
		processList remove: aProcess.
|
	run
		" run as long as process list is non empty "
		[ notdone ] whileTrue:
			[ processList size = 0 ifTrue: 
				[ self initialize ].
			  processList do: 
				[ :x | currentProcess <- x.
					x execute  ] ]
|
	yield
		" set time slice counter to zero, thereby
		yielding to next process "
		<53 0>
]
Methods Process 'creation'
	new
		stack <- Array new: 50.
		stackTop <- 10.
		linkPointer <- 2.
		stack at: 4 put: 1. "return point"
		stack at: 6 put: 1. "bytecode counter"
|
	method: x 
		stack at: 5 put: x.
|
	context: ctx
		stack at: 3 put: ctx.
|
	startAt: x
		stack at: 6 put: x. "starting bytecode value"
]
Methods Semaphore 'all'
	new
		count <- 0.
		processList <- List new
|
	critical: aBlock
		self wait.
		aBlock value.
		self signal
|
	set: aNumber
		count <- aNumber
|
	signal
		(processList size = 0)
			ifTrue: [ count <- count + 1]
			ifFalse: [ scheduler critical:
				[ processList first resume.
					processList removeFirst ]]
|
	wait		| process |
		(count = 0)
			ifTrue: [ scheduler critical:
					[ process <- scheduler currentProcess.
					  processList add: process.
					  scheduler removeProcess: process].
				  scheduler yield ]
			ifFalse: [ count <- count - 1]
]
