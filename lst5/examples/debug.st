"debug.st - a simple interactive debugger for Tiny Smalltalk
 by Andy Valencia, May 2001

 To use the debugger, fileIn: this file.  Then do:

 Debug run: 'command...'

 The given command line will be compiled (much as it would if you had
 typed it interactively).  Then the debugger will be started on this
 command itself.  Generally, you'll do a 's' (or 'step') to step down
 from the command line into the first method call.

 Command available are:
 's' or 'step'		Single step to the next line
 'stepi'		Single step one VM instruction
 'n' or 'next'		Step over method calls on the current line
 'b' or 'break'		Argument is <class>/<method>[/<line #>]
 			Set a breakpoint at the named method.
			Meta<class> accesses class methods.
			A plain line number applies to the current
			 class/method.
 'c' or 'cont'		Continue running until error, completion,
 			or breakpoint.
 'd' or 'delete'	Argument is an integer; delete that breakpoint
 'lb' or 'listbreak'	List breakpoints
 'p' or 'print'		Print variable(s).  Arguments are one or more
 			temporary, argument, or instance variables.
			You may also use $pc (VM instruction pointer)
			and $sp (VM stack top pointer)
 'quit'			Leave the debugger (abandon the executing
 			target code)
 'where' or 'bt'	Show stack backtrace
 'l' or 'list'		List source (can provide a line # as argument)
 'whatis'		Describe variable more thoroughly
 'up', 'down'		Move up and down the call stack for purposes
 			of accessing variables.
 'debug'		Argument is a Class... compiles all Method's
 			for that class in their debuggable form.
 'br' or 'browse'	Argument is variable as in 'print'; invokes
			system data structure browser.
 (blank line)		Re-enter previous command.  Useful for single
 			stepping statements in a row.
"
+Encoder subclass: #DebugEncoder variables: #(lines oldLine oldRange)
+Method subclass: #DebugMethod variables: #(lines textlines vars bpoints active)
+Object subclass: #Debug variables: #(proc bpoints prevList selctx)
!DebugEncoder
name: n
	" Hook to set up our instance variables "
	lines <- Dictionary new.
	oldLine <- 0.
	^ super name: n
!
!DebugEncoder
genCode: byte
	" Record code generated versus line number "
	oldRange isNil ifTrue: [
		oldRange <- lines at: oldLine ifAbsent: [nil].
		oldRange isNil ifTrue: [
			oldRange <- Set new.
			lines at: oldLine put: oldRange
		]
	].
	oldRange add: index.
	^ super genCode: byte
!
!DebugEncoder
lineNum: l
	" Note when the line number changes "
	(l ~= oldLine) ifTrue: [
		oldLine <- l.
		" We lazily insert the range, only at the point we
		  actually see some code generated for this source line."
		oldRange <- nil
	]
!
!DebugEncoder
method: maxTemps class: c text: text | ret |
	" Create the compiled Method from the generated code "
	ret <- DebugMethod name: name byteCodes: byteCodes
		literals: literals stackSize: maxStack
		temporarySize: maxTemps class: c text: text.
	ret debug: lines.
	^ ret
!
!Class
setDebug: sel | meth cl dict |
	" Recompile a Method into a DebugMethod, recording additional
	  debug information. "

	" Chase up the superclass chain, trying to find our Method "
	cl <- self.
	[meth isNil] whileTrue: [
		dict <- cl methods.
		meth <- dict at: sel ifAbsent: [ cl <- cl superclass. nil].
		cl isNil ifTrue: [
			('Undefined method ' + sel printString +
				' for class ' + self printString) printNl.
			^ nil
		]
	].

	" If we haven't already fixed it up, build a DebugMethod version "
	(meth isKindOf: DebugMethod) ifFalse: [
		'Compiling ' print. sel printString print.
		' for class ' print. cl print.
		'...' printNl.
		meth <- (Parser new text: meth text
			instanceVars: self instanceVariables)
			 parse: self with: DebugEncoder.
		meth notNil
		 ifTrue: [
			cl methods at: sel put: meth.
			Method flushCache
		 ]
		 ifFalse: [
			'Compilation failed.' printNl.
		 ]
	].
	^ meth
!
!Debug
runIt: count | ret |
	" Blow away any selected context when we run "
	selctx <- nil.

	" Execute for one instruction.  Return whether or not the return
	 was 'normal' (i.e., VM stopped due to debugger control, not
	 something else.  Spit out a message for a breakpoint. "
	ret <- proc doExecute: count+1.
	(ret = 5) ifTrue: [ ^ true ].
	(ret = 6) ifTrue: [ self onBreak. ^ true ].
	(ret = 2) ifTrue: [ 'Error trap' printNl ].
	(ret = 3) ifTrue: [ 'Message not understood' printNl ].
	(ret = 4) ifTrue: [ 'Method returned' printNl ].
	^ false
!
!Context
method
	^ method
!
!Context
bytePointer
	^ bytePointer
!
!Context
stack
	^ stack
!
!Context
stackTop
	^ stackTop
!
!Context
temporaries
	^ temporaries
!
!Context
arguments
	^ arguments
!
!Method
forClass
	^ class
!
!Debug
srcLine: ctx | meth line |
	" Get source line corresponding to current byte pointer "
	meth <- ctx method.

	" Can't show it if this isn't a debuggable method "
	(meth isKindOf: DebugMethod) ifFalse: [
		'Not debuggable' printNl.
		^ nil
	].

	" Return source line pointer or nil "
	^ meth srcLine: ctx bytePointer.
!
" Show current source line of a given context "
!Debug
showLine: ctx | line meth |
	" Show source line corresponding to current VM instruction "
	ctx isNil ifTrue: [ ^ nil ].
	meth <- ctx method.
	line <- self srcLine: ctx.
	line isNil
		ifTrue: [
			'Method ' print.  meth name print.
			' for class ' print.  meth forClass print.
			': no source displayed.' printNl
		]
		ifFalse: [
			" Show the text "
			(line printString printWidth: 8) print.
			(meth textlines at: line ifAbsent: ['']) printNl
		]
!
" Display current line of active procedure "
!Debug
showLine
	^ self showLine: self curContext
!
=Debug
run: line | meth ret ctx proc |
	" Run a command line under the debugger"

	meth <- (Parser new text: ('debugCmd ^' + line) instanceVars: #())
		 parse: Undefined with: DebugEncoder.
	meth notNil ifTrue: [
		ret <- super new.
		ctx <- Context new.
		ctx setup: meth withArguments: (Array new: 1).
		proc <- Process new.
		proc context: ctx.
		ret proc: proc.
		ret run
	]
!
!Debug
proc: p
	" Initialize our new debug session "
	proc <- p.
	bpoints <- Array new: 0
!
!Debug
atCall | ret meth ctx pc low high |
	" Tell if the VM instruction pointer is at a method invocation "

	" Get the next instruction "
	ctx <- proc context.
	meth <- ctx method.
	pc <- ctx bytePointer.
	high <- meth byteCodes at: (pc + 1) ifAbsent: [ ^ nil ].
	pc <- pc + 1.
	low <- high rem: 16.
	high <- high quo: 16.
	(high = 0) ifTrue: [
		high <- low.
		low <- meth byteCodes at: (pc + 1) ifAbsent: [ ^ nil ].
		pc <- pc + 1
	].

	" Return nil if it isn't a SendMessage "
	(high = 9) ifFalse: [ ^ nil ].

	" Otherwise return the selector and return address "
	ret <- Array new: 2.
	ret at: 1 put: (meth literals at: (low + 1)).
	ret at: 2 put: pc.
	^ ret
!
!Debug
stepCall: sel | ctx stack sp args target meth |
	" Set up to step into a new method "
	ctx <- proc context.
	stack <- ctx stack.
	sp <- ctx stackTop.
	args <- stack at: sp.
	target <- args at: 1.
	meth <- target class setDebug: sel.
	meth isNil ifTrue: [ ^ true ].
	(self runIt: 1) ifFalse: [
		'Execution done in ' print.
		meth name print.
		' of class ' print.
		target class printNl.
		^ true
	].
	^ false
!
!Debug
onBreak | ctx meth rec |
	" Tell if we're at a breakpoint.  As a side effect, display
	  this fact to the user."
	ctx <- proc context.
	ctx isNil ifTrue: [ ^ false ].
	meth <- ctx method.
	1 to: bpoints size do: [:idx|
		rec <- bpoints at: idx.
		(((rec at: 1) = meth) and:
				[(rec at: 2) = (self srcLine: ctx)])
		ifTrue: [
			'Breakpoint ' print. idx print. ' hit in ' print.
			meth name printString print. '/' print.
			(rec at: 2) printNl.
			^ true
		]
	].
	^ false
!
!Debug
overCall: pc | ctx old res meth |
	" Set a breakpoint at the instruction beyond the SendMessage "
	ctx <- proc context.
	meth <- ctx method.

	" If we're within a non-debug Method, just limp forward "
	(meth isKindOf: DebugMethod) ifFalse: [
		'Can''t step over calls in ' print. meth name print.
		', single stepping.' printNl
		self runIt: 1.
		^ false
	].

	" Otherwise break beyond the call "
	meth setBreak: pc.

	" Now let it run until it hits the breakpoint, and clear
	  the breakpoint. "
	self breakActive: true. meth breakActive: true.
	res <- self runIt: -1.
	self breakActive: false.  meth clearBreak: pc.
	res ifTrue: [
		" Should be stopped at the expected location "
		((proc context = ctx) and:
				[ ctx bytePointer = pc ]) ifTrue: [
			^ false
		].

		" Or hit some other breakpoint "
		(self onBreak) ifTrue: [
			^ false
		].

		" Otherwise, what's going on? "
		'Unexpected run completion' printNl.
		^ true
	].

	" Some other error killed us "
	'Execution aborted' printNl.
	^ true
!
!Debug
doStep: intoCalls | srcl ret ctx |
	" Implement a single step, stepping either over or into calls
	  (method invocations) depending on the intoCalls argument. "
	ctx <- proc context.
	srcl <- self srcLine: ctx.
	[(proc context == ctx) and:
			[srcl == (self srcLine: ctx)]] whileTrue: [
		" If dropping into a new method, deal with it "
		ret <- self atCall.
		ret notNil ifTrue: [
			" Stepping into the call "
			intoCalls ifTrue: [
				^ self stepCall: (ret at: 1)
			].

			" Stepping over call "
			(self overCall: (ret at: 2)) ifTrue: [
				^ true
			]
		] ifFalse: [
			" Otherwise run a single VM operation "
			(self runIt: 1) ifFalse: [
				'Execution done at line ' print.
				srcl printString printNl.
				^ true
			]
		]
	].
	^ false
!
!Debug
printReg: reg in: ctx
	" Print a VM register "
	(reg = '$pc') ifTrue: [
		ctx bytePointer print. ^ self
	].
	(reg = '$sp') ifTrue: [
		ctx stackTop print. ^ self
	].
	'Unknown register: ' print. reg print
!
!Debug
curContext
	selctx isNil ifTrue: [ ^ proc context ].
	^ selctx
!
!Debug
whatis: args | ctx meth |
	" Display arguments, temporaries, instance variables, and
	  VM registers. "

	" Get the DebugMethod, which has symbolic information for variables "
	ctx <- self curContext.
	meth <- ctx method.
	(meth isKindOf: DebugMethod) ifFalse: [
		'No debug information for ' print.
		meth name printNl.
		^ nil
	].

	" Walk each variable, printing its value "
	args do: [:var|
		var print. ': ' print.
		((var at: 1) = $$)
		ifTrue: [
			var print. ' is a register variable' printNl.
		] ifFalse: [
			meth whatis: var in: ctx
		]
	]
!
!Debug
examine: args | ctx meth |
	" Display arguments, temporaries, instance variables, and
	  VM registers. "

	" Get the DebugMethod, which has symbolic information for variables "
	ctx <- self curContext.
	meth <- ctx method.
	(meth isKindOf: DebugMethod) ifFalse: [
		'No debug information for ' print.
		meth name printNl.
		^ nil
	].

	" Walk each variable, printing its value "
	args do: [:var|
		var print. ': ' print.
		((var at: 1) = $$)
		ifTrue: [
			self printReg: var in: ctx
		] ifFalse: [
			meth print: var in: ctx
		].
		Char newline print
	]
!
!Debug
setBreak: args | s cl clname meth methname i rec lineNum inClass arg loc |
	" Set a breakpoint "

	" Map straight line # to current class/method "
	arg <- args at: 1.
	((arg at: 1) isDigit) ifTrue: [
		lineNum <- arg asNumber.
		lineNum isNil ifTrue: [
			'Bad line #' print. arg printNl.
			^ nil
		].
		meth <- self curContext method.
		arg <- (meth forClass printString) + '/' +
			(meth name printString) + '/' +
			lineNum printString
	].

	" Parse <class>:<method> "
	s <- arg break: '/'.
	(s size < 2) ifTrue: [
		'Format is <class>/<method>' printNl.
		^ nil
	].

	" Look up in instance methods unless it's Meta<class>,
	  in which case trim the 'Meta' and look up in class
	  methods."
	clname <- s at: 1.
	((clname from: 1 to: 4) = 'Meta') ifTrue: [
		inClass <- true.
		clname <- clname from: 5 to: clname size
	] ifFalse: [
		inClass <- false
	].
	clname <- clname asSymbol.
	methname <- (s at: 2) asSymbol.

	" Parse line number "
	(s size > 2) ifTrue: [
		lineNum <- (s at: 3) asNumber.
		lineNum isNil ifTrue: [
			'Bad line #' print. (s at: 3) printNl.
			^ nil
		]
	] ifFalse: [
		lineNum <- 1
	].

	" Find class "
	cl <- Smalltalk at: clname ifAbsent: [
		('Unknown class: ' + clname printString) printNl.
		^ nil
	].

	" Convert to metaclass if needed "
	inClass ifTrue: [
		cl <- cl class
	].

	" Now get method, in its debuggable format "
	meth <- cl setDebug: methname.
	meth isNil ifTrue: [
		('Unknown method: ' + methname printString) printNl.
		^ nil
	].

	" If it's already set, don't do it again "
	rec <- Array with: meth with: lineNum.
	i <- bpoints indexOfVal: rec.
	i notNil ifTrue: [
		'Already set as breakpoint ' print.
		i printNl.
		^ nil
	].

	" See if we can turn line # into a code location "
	loc <- meth codeLoc: lineNum.
	loc isNil ifTrue: [
		'No code for source line ' print. lineNum printNl.
		^ nil
	].

	" Set & record the breakpoint "
	meth setBreak: loc.
	bpoints <- bpoints with: rec
!
!Debug
clearBreak: args | arg n rec meth lineNum |
	" Delete an existing breakpoint "

	arg <- args at: 1 ifAbsent: ['Missing argument' printNl. ^ nil].
	n <- arg asNumber.
	n isNil ifTrue: [
		('Invalid argument: ' + arg) printNl
	] ifFalse: [
		((n < 1) or: [n > bpoints size]) ifTrue: [
			('No such breakpoint: ' + arg) printNl
		] ifFalse: [
			rec <- bpoints at: n.
			meth <- rec at: 1.
			lineNum <- rec at: 2.
			meth clearBreak: (meth codeLoc: lineNum).
			bpoints <- bpoints removeIndex: n.
			n print. ': deleted' printNl
		]
	]
!
!Debug
listBreak | rec meth lineNum |
	" List breakpoints "

	'Breakpoints:' printNl.
	1 to: bpoints size do: [:x|
		x print. ': ' print.
		rec <- bpoints at: x.
		meth <- rec at: 1.
		lineNum <- rec at: 2.
		meth name printString print. '/' print.
		lineNum printNl
	]
!
!Debug
breakActive: flag | meths |
	" Make all our breakpoints active or inactive, depending
	  on flag's value. "
	meths <- Set new.
	bpoints do: [:rec|
		meths add: (rec at: 1)
	].
	meths do: [:meth| meth breakActive: flag]
!
!Debug
list: args | meth where src ctx |
	" List source code "

	" Get the method we're going to display "
	ctx <- self curContext.
	meth <- ctx method.
	(meth isKindOf: DebugMethod) ifFalse: [
		'No debug information' printNl.
		^ self
	].

	" Either continue listing, or start from the given place "
	(args size < 1) ifTrue: [
		prevList isNil ifTrue: [
			" List around where we're currently executing "
			where <- (self srcLine: ctx) - 5
		] ifFalse: [
			where <- prevList + 1
		]
	] ifFalse: [
		where <- (args at: 1) asNumber.
		where isNil ifTrue: [
			'Invalid line number: ' print.
			(args at: 1) printNl.
			^ self
		]
	].

	" Show 9 lines "
	src <- meth textlines.
	where to: (where + 8) do: [:x|
		((x > 0) and: [x <= src size]) ifTrue: [
			(x printString printWidth: 8) print.
			(src at: x) printNl.
			prevList <- x
		]
	]
!

" nextContext:
	Return next context deeper in context stack

  Because contexts are only forward linked, we have to search from
  the top inward, then return the next one out.
"
!Debug
nextContext: ctx | c prev |
	c <- proc context.
	[(prev <- c previousContext) ~= ctx] whileTrue: [
		prev isNil ifTrue: [ ^ nil ].
		c <- prev
	].
	^ c
!

" upDown:count:
	Move up or down the stack frames
"
!Debug
upDown: up count: args | c count |
	" If nothing selected, start from bottom of stack "
	selctx <- self curContext.

	" Get count, default 1 "
	(args size > 0) ifTrue: [
		count <- (args at: 1) asNumber
	] ifFalse: [
		count <- 1
	].

	" Walk the context chain "
	1 to: count do: [:ignore|
		" Get next/prev context depending on step direction "
		up ifTrue: [
			c <- selctx previousContext
		] ifFalse: [
			c <- self nextContext: selctx
		].

		" Just ignore running off the end "
		c isNil ifFalse: [ selctx <- c ]
	]
!

" makeDebug:
	Convert Class methods to DebugMethod's
"
!Debug
makeDebug: args | cl meta n |
	args do: [:clname|
		" Map MetaFOO -> FOO class "
		((clname from: 1 to: 4) = 'Meta') ifTrue: [
			n <- clname from: 5.
			meta <- true
		] ifFalse: [
			n <- clname.
			meta <- false
		].

		" Look up class "
		cl <- Smalltalk at: n asSymbol ifAbsent: [ nil ].
		cl isNil
		 ifTrue: [ ('Unknown class: ' + clname) printNl ]
		 ifFalse: [
			" Map to metaclass if needed "
			meta ifTrue: [ cl <- cl class ].

			" Convert methods "
			cl methods keysDo: [:k| cl setDebug: k ]
		 ]
	]
!

!Debug
run | prev did cmd done line |
	" Main command loop for the debugger session "

	prev <- ''.
	done <- false.
	[true] whileTrue: [
		" Show where we are "
		self showLine.

		" Get command "
		'Debug> ' print.
		line <- String input.

		" Re-insert previous command if empty line "
		(line isEmpty) ifTrue: [ line <- prev ].
		prev <- line.

		" Parse into words "
		line <- line break: ' '.

		" Command is first, arguments follow "
		cmd <- line at: 1.
		line <- line from: 2 to: line size.

		" Set flag to indicate command hasn't matched yet "
		did <- false.

		" Step a single VM instruction "
		(cmd = 'stepi') ifTrue: [
			done
			 ifTrue: [ 'Not runnable' printNl ]
			 ifFalse: [
				prevList <- nil.
				(self runIt: 1) ifFalse: [
					done <- true
				]
			].
			did <- true
		].

		" Step a source line "
		((cmd = 'step') or: [cmd = 's']) ifTrue: [
			done
			 ifTrue: [ 'Not runnable' printNl ]
			 ifFalse: [
				 prevList <- nil.
				 done <- self doStep: true
			 ].
			did <- true
		].

		" Step a source line, stepping over message sends "
		((cmd = 'next') or: [cmd = 'n']) ifTrue: [
			done
			 ifTrue: [ 'Not runnable' printNl ]
			 ifFalse: [
				 prevList <- nil.
				 done <- self doStep: false
			 ].
			did <- true.
		].

		" Examine variables "
		((cmd = 'p') or: [cmd = 'print']) ifTrue: [
			self examine: line.
			did <- true
		].

		" Describe variable "
		(cmd = 'whatis') ifTrue: [
			self whatis: line.
			did <- true
		].

		" Set a breakpoint "
		((cmd = 'b') or: [cmd = 'break']) ifTrue: [
			self setBreak: line.
			did <- true
		].

		" Clear breakpoint(s) "
		((cmd = 'd') or: [cmd = 'delete']) ifTrue: [
			self clearBreak: line.
			did <- true
		].

		" List breakpoints "
		((cmd = 'lb') or: [cmd = 'listbreak']) ifTrue: [
			self listBreak.
			did <- true
		].

		" Just let it run "
		((cmd = 'cont') or: [cmd = 'c']) ifTrue: [
			" Clear previous listing position "
			prevList <- nil.

			" Step forward once, even over a breakpoint "
			done <- (self runIt: 1) not.

			" Now run until completion or next break "
			done ifFalse: [
				" Activate, run, and deactivate "
				self breakActive: true.
				done <- (self runIt: -1) not.
				self breakActive: false.

				" Display a message if hit a breakpoint "
				done ifFalse: [ self onBreak ].
			].

			did <- true
		].

		" Source listing "
		((cmd = 'l') or: [cmd = 'list']) ifTrue: [
			self list: line.
			did <- true
		].

		" Abandon the method "
		(cmd = 'quit') ifTrue: [
			^nil
		].

		" Stack backtrace "
		((cmd = 'where') or: [cmd = 'bt']) ifTrue: [
			proc context isNil
			ifTrue: [
				'Process has terminated' printNl
			] ifFalse: [
				proc context backtrace
			].
			did <- true
		].

		" Go up or down the stack chain "
		((cmd = 'up') or: [cmd = 'down']) ifTrue: [
			self upDown: (cmd = 'up') count: line.
			did <- true
		].

		" Make all procedures of the named class debuggable "
		(cmd = 'debug') ifTrue: [
			self makeDebug: line.
			did <- true
		].

		" Hook to data structure browser "
		((cmd = 'br') or: [cmd = 'browse']) ifTrue: [
			self browse: line.
			did <- true
		].

		" Error "
		did ifFalse: [ 'Unknown command.' printNl ].
	]
!
!String
asLines | c ret slot idx base top nl |
	" Convert a string with embedded newlines into an Array
	  with one slot per line.  The newlines are not a part of
	  these lines. "

	" Size the array of lines which will result "
	nl <- Char newline.
	ret <- Array new: (self occurencesOf: nl) + 1.

	" Walk the elements, assembling lines "
	slot <- base <- idx <- 1.
	top <- self size.
	[idx < top] whileTrue: [
		c <- self at: idx.
		(c = nl) ifTrue: [
			ret at: slot put: (self from: base to: idx - 1).
			slot <- slot + 1.
			base <- idx + 1
		].
		idx <- idx + 1
	].
	(idx > top) ifTrue: [ idx <- top ].
	(idx > base) ifTrue: [
		ret at: slot put: (self from: base to: idx)
	].
	^ ret
!
!DebugMethod
debug: l
	" Hook to initialize our DebugMethod instance "
	lines <- l.
	textlines <- text asLines.
	bpoints <- Dictionary new.
	active <- false
!
!DebugMethod
debug
	^ lines
!
!DebugMethod
textlines
	^ textlines
!
!DebugMethod
args: argNames inst: instNames temp: tempNames
	" Record debug data "
	vars <- Array new: 3.
	vars at: 1 put: argNames.
	vars at: 2 put: instNames.
	vars at: 3 put: tempNames
!
!DebugMethod
srcLine: bp
	" Map the VM instruction byte pointer onto a source line # "
	lines binaryDo: [:src :range |
		(range includes: bp) ifTrue: [
			^ src
		]
	].
	'No match for ' print. bp printString printNl.
	^nil
!
!DebugMethod
whatis: var in: ctx | idx obj |
	" Describe a variable in this Method "

	" Name of an argument "
	idx <- (vars at: 1) indexOfVal: var.
	idx notNil ifTrue: [
		var print. ' is an argument' print.
		obj <- ctx arguments at: idx
	].

	" Instance variable "
	obj isNil ifTrue: [
		idx <- (vars at: 2) indexOfVal: var.
		idx notNil ifTrue: [
			var print. ' is an instance variable' print.
			obj <- ctx arguments at: 1.
			obj <- (Object class in: obj at: idx)
		]
	].

	" Temporary "
	obj isNil ifTrue: [
		idx <- (vars at: 3) indexOfVal: var.
		idx notNil ifTrue: [
			var print. ' is a temporary variable' print.
			obj <- ctx temporaries at: idx
		]
	].

	" If we found it, display the generic information "
	obj notNil ifTrue: [
		' (index ' print. idx print. ')' printNl.
		' Value: ' print. obj printNl.
		' Class: ' print. obj class printString print.
		' basicSize: ' print. obj basicSize printNl.
	] ifFalse: [
		" Couldn't find it... "
		'Unknown variable' print. var printNl
	]
!
!DebugMethod
getVar: var in: ctx | idx |
	" Get a variable in this Method, return its value "

	" Name of an argument "
	idx <- (vars at: 1) indexOfVal: var.
	idx notNil ifTrue: [ ^ (ctx arguments at: idx) ].

	" Instance variable "
	idx <- (vars at: 2) indexOfVal: var.
	idx notNil ifTrue: [
		^ (Object in: (ctx arguments at: 1) at: idx)
	].

	" Temporary "
	idx <- (vars at: 3) indexOfVal: var.
	idx notNil ifTrue: [ ^ (ctx temporaries at: idx) ].

	" Couldn't find it... "
	^ nil
!
!DebugMethod
print: var in: ctx | obj |
	" Print a variable in this Method "
	obj <- self getVar: var in: ctx.
	obj isNil ifTrue: [ ('Unknown variable: ' + var) printNl ]
	 ifFalse: [ obj printNl ]
!
!DebugMethod
setBreak: bp | old |
	" Set a breakpoint in this Method "

	" If already set, ignore "
	(bpoints includes: bp) ifTrue: [ ^ self ].

	" Record current opcode at code location "
	old <- byteCodes at: (bp + 1).

	" Update the code location if it's already active "
	active ifTrue: [
		self patchBreak: bp active: true
	].

	" Record this breakpoint "
	bpoints at: bp put: old
!
!DebugMethod
clearBreak: bp
	" Remove a breakpoint in this Method "

	" If not set, ignore "
	(bpoints includes: bp) ifFalse: [ ^ self ].

	" Restore code contents "
	self patchBreak: bp active: false.

	" Remove record of this breakpoint "
	bpoints removeKey: bp
!
!DebugMethod
patchBreak: bp active: flag
	" Set or clear the breakpoint instruction in the code"

	flag ifTrue: [
		" Patch in a DoSpecial operation 12 (breakpoint) "
		byteCodes at: (bp + 1) put: ((15*16) + 12)
	] ifFalse: [
		" Restore old code at this location "
		byteCodes at: (bp + 1) put: (bpoints at: bp)
	]
!
!DebugMethod
breakActive: flag
	" Activate or deactivate breakpoints for this Method "

	" Skip all this if we aren't changing settings "
	(active = flag) ifTrue: [ ^ self ].

	" For each location with a breakpoint, update it "
	bpoints keysDo: [:bp|
		self patchBreak: bp active: flag
	].
	active <- flag
!
!DebugMethod
codeLoc: line | set ret |
	" Map source line # to a code location "

	" Get code generated for that line #, or return nil "
	set <- lines at: line ifAbsent: [ ^ nil ].

	" Return lowest code location "
	ret <- set anyOne.
	set do: [:elem| ret <- ret min: elem].
	^ ret
!
!Debug
browse: args | meth br ctx |
	" Get the DebugMethod, which has symbolic information for variables "
	ctx <- self curContext.
	meth <- ctx method.
	(meth isKindOf: DebugMethod) ifFalse: [
		'No debug information for ' print.
		meth name printNl.
		^ nil
	].

	" See if Browser is installed "
	br <- Smalltalk at: #Browser ifAbsent: [
		'Browser not installed.' printNl.
		^ nil
	].

	^ br on: (meth getVar: (args at: 1) in: ctx)
!
