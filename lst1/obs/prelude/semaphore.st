Class  Semaphore :List
| excessSignals |

[  new
	excessSignals <- 0

|  new: aNumber
	excessSignals <- aNumber

|  signal
	<StartAtomic>.	"start atomic action"
	(self isEmpty)
	  ifTrue: [excessSignals <- excessSignals + 1]
	  ifFalse: [self removeFirst unblock].
	<EndAtomic>		"end atomic action"

|  wait
	<StartAtomic>.	"start atomic actions"
	(excessSignals = 0)
	  ifTrue: [self addLast: selfProcess.
		   selfProcess block]
	  ifFalse: [excessSignals <- excessSignals - 1].
	<EndAtomic>		"end atomic actions"
]
