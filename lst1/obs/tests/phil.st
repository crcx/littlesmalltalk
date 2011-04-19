Class  Main
[
	main
		( DiningPhilosophers new: 5 ) dine: 4
]

Class  DiningPhilosophers
| diners  forks  philosophers |
[
	new: aNumber
	    diners <- aNumber.
	    forks <- Array new: aNumber.
	    philosophers <- Array new: aNumber.
	    (1 to: diners) do:
		[ :p | forks at: p put: (Semaphore new: 1).
		       philosophers at: p put: (Philosopher new: p)]

|
	dine: time
	    (1 to: diners) do:
		[ :p | (philosophers at: p)
			    leftFork: (forks at: p)
			    rightFork: (forks at: ((p \\ diners) + 1))].
	    time timesRepeat:
		[(1 to: diners) do: [ :p | (philosophers at: p) philosophize]].
	    (1 to: diners) do:
		[ :p | (philosophers at: p) sleep]
]

Class  Philosopher
| leftFork  rightFork  myName  myPhilosophy |
[
	new:  name
	    myName <- name.
	    myPhilosophy <- [[true] whileTrue:
				[self think.
				 self getForks.
				 self eat.
				 self releaseForks.
				 selfProcess suspend]
			    ] newProcess

|
	leftFork: lfork  rightFork: rfork
	    leftFork <- lfork.
	    rightFork <- rfork
|
	getForks
	    ((myName \\ 2) == 0)
		ifTrue: [leftFork wait.  rightFork wait]
		ifFalse: [rightFork wait.  leftFork wait]
|
	releaseForks
	    leftFork signal.
	    rightFork signal

|
	think
	    ('Philosopher ',(myName asString),' is thinking.') print.
	    10 timesRepeat: [selfProcess yield]
|
	eat
	    ('Philosopher ',(myName asString),' is eating.') print.
	    10 timesRepeat: [selfProcess yield]

|
	philosophize
	    myPhilosophy resume
|
	sleep
	    myPhilosophy terminate.
	    ('Philosopher ',(myName asString),' is sleeping.') print.
	    myPhilosophy <- nil
]
