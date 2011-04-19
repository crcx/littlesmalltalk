Class  Process

[  block
	(self state == #TERMINATED)
	    ifTrue: [self termErr: 'block'.  ^ nil].
	<SetProcessState  self 2>.
	^ self state

|  resume
	(self state == #TERMINATED)
	    ifTrue: [self termErr: 'resume'.  ^ nil].
	<SetProcessState  self 0>.
	^ self state

|  suspend
	(self state == #TERMINATED)
	    ifTrue: [self termErr: 'suspend'.  ^ nil].
	<SetProcessState  self 1>.
	^ self state

|  state  | pstate |
	pstate <- <ReturnProcessState  self>.
	(pstate = 0) ifTrue: [pstate <- #READY.  ^ pstate].
	(pstate = 1) ifTrue: [pstate <- #SUSPENDED.  ^ pstate].
	(pstate = 2) ifTrue: [pstate <- #BLOCKED.  ^ pstate].
	(pstate = 3) ifTrue: [pstate <- #BLOCKED.  ^ pstate].
	(pstate >= 4) ifTrue: [pstate <- #TERMINATED.  ^ pstate]

|  terminate
	<Terminate self>.
	^ self state

|  termErr: msgName
	('Cannot ',msgName,' a terminated process.') print

|  unblock
	(self state == #TERMINATED)
	    ifTrue: [self termErr: 'unblock'.  ^ nil].
	<SetProcessState  self 3>.
	^ self state

|  yield
	^ nil
]
