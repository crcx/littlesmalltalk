Class StandardWindows Object
Class Window Object number title menus size
Class   TextWindow Window text
Class   GraphicsWindow Window
Class      DictionaryWindow GraphicsWindow dict select action
Class         BrowserWindow DictionaryWindow class method mw tw
Class EventManager Process responses
Class Menu Object number title itemtitles items enablestatus
Methods Window 'all'
	new
		title <- ''.
		menus <- List new.
		(1 to: 15) do: [:i | (windows at: i) isNil
			ifTrue: [ windows at: i put: self.
				    number <- i.  ^ self ] ]
|
	attachMenu: menu
		menus addLast: menu.
		<162 number 2 (menu number)>
|
	activate
		^ nil
|
	deactivate
		^ nil
|
	drawEvent
		" overridden in subclasses "
		^ nil
|
	mouseMoveTo: mouseLocation
		" mouse moved with button down "
		^ nil
|
	mouseDownAt: mouseLocation
		" mouse down, do nothing "
		^ nil
|
	mouseUpAt: mouseLocation
		" mouse up "
		^ nil
|
	command: n
		(n = 1) ifTrue: [ self close ]
|
	reSized
		size <- <161 number 6>
|
	open
		" open our window, unless already opened "
		<160 number title 0>.
		menus do: [:m | <162 number 2 (m number)> ].
		self reSized.
|
	charTyped: c
		smalltalk beep
|
	title: text
		title <- text.
		<164 number title>
|
	close
		" close up shop "
		<161 number 1>.
		windows at: number put: nil
]
Methods TextWindow 'all'
	open
		"open the window with implicit text buffer"
		<160 number title 1>.
		" now do other initialization "
		super open
|
	activate
		super activate.
		printer <- self.
|
	deactivate
		super deactivate.
		printer <- stdout.
|
	text
		" read updated text and store it"
		^ text <- <165 number>
|
	print: text
		<166 number text>
|
	draw
		"redraw window"
		<161 number 2>.
		<161 number 5>.
		<161 number 3>
]
Methods GraphicsWindow 'all'
	startDrawing
		<161 number 2>
|
	endDrawing
		<161 number 3>
|
	drawEvent
		self startDrawing.
		self draw.
		self endDrawing.
|
	draw
		" done by subclasses "
		^ nil
|
	at: x and: y print: text
		<190 x y text>
]
Methods DictionaryWindow 'all'
	action: aBlock
		action <- aBlock
|
	dictionary: d
		dict <- d.
		<163 number 2 40 (12* d size)>
|
	draw		| loc |
		loc <- 0.
		dict binaryDo: [:x :y |
			self at: 0 and: loc print: x asString.
			loc <- loc + 12 ].
		<163 number 2 40 loc >.
		select notNil
			ifTrue: [ select invert ].
|
	mouseDownAt: mouseLocation	| y loc |
		self invertSelection.
		y <- mouseLocation y.
		loc <- 0.
		dict binaryDo: [:a :b |
			loc <- loc + 12.
			(loc > y) ifTrue: [ 
				select <- 0@(loc - 10) to: size x@loc.
				self invertSelection. 
				action value: b. ^ nil ]]
|
	invertSelection
		self startDrawing.
		(select notNil)
			ifTrue: [ select invert ].
		self endDrawing.
]
Methods BrowserWindow 'all'
	new
		super new.
		dict <- classes.
		action <- [:c | self selectClass: c ].
		self makeBrowserMenu.
|
	close
		" close all our windows "
		tw notNil ifTrue: [ tw close ].
		mw notNil ifTrue: [ mw close ].
		super close.
|
	selectClass: c
		class <- c.
		browserMenu enableItem: 2.
		browserMenu disableItem: 3.
		browserMenu disableItem: 4.
		tw notNil ifTrue: [ tw close ].
		mw notNil ifTrue: [ mw close ].
		self openMethodWindow
|
	openMethodWindow
		tw notNil ifTrue: [ tw close ].
		browserMenu disableItem: 3.
		mw notNil ifTrue: [ mw close ].
		browserMenu enableItem: 2.
		mw <- DictionaryWindow new; 
			title: class printString,  ' Methods';
			dictionary: class methods;
			action: [:c | self selectMethod: c ];
			attachMenu: browserMenu;
			open.
|
	selectMethod: m
		method <- m.
		tw notNil ifTrue: [ tw close ].
		tw <- TextWindow new; 
			title: class printString , ' ', m asString;
			attachMenu: browserMenu;
			open.
		browserMenu enableItem: 3.
		tw print: m text
|
	makeBrowserMenu
		browserMenu isNil ifTrue: 
			[ browserMenu <- Menu new; title: 'Browser'; create.
			browserMenu addItem: 'add class'
				action: [:w | self addClass ].
			browserMenu addItem: 'add method'
				action: [:w | self addMethod ].
			browserMenu addItem: 'compile'
				action: [:w | self compile ].
			browserMenu addItem: 'command'
				action: [:w | self doCommand ] ].
		browserMenu disableItem: 2.
		browserMenu disableItem: 3.
		browserMenu disableItem: 4.
		self attachMenu: browserMenu
|
	addClass	
		" add a new class "
		tw notNil ifTrue: [ tw close ].
		browserMenu enableItem: 4.
		tw <- TextWindow new; title: 'New Class Information';
			open; attachMenu: browserMenu;
			print: 'superClass addSubClass: #nameOfClass ',
				'instanceVariableNames: ''var1 var2'' '
|
	addMethod
		method <- Method new.
		tw notNil ifTrue: [ tw close ].
		tw <- TextWindow new; 
			title: class printString , ' new method'.
		tw open; attachMenu: browserMenu.
		browserMenu enableItem: 3.
|
	compile
		method text: tw text.
		(method compileWithClass: class)
			ifTrue: [ class methods at: method name put: method.
				mw drawEvent ].
|
	doCommand
		" accept tw command "
		[ tw text execute. tw close. self drawEvent ] fork.
]
Methods Menu 'all'
	new
		items <- Array new: 0.
		itemtitles <- Array new: 0.
		enablestatus <- Array new: 0.
		(1 to: 15) do: [:i | (menus at: i) isNil
			ifTrue: [ menus at: i put: self.
				    number <- i.  ^ self ] ]
|
	number
		^ number
|
	addItem: name action: aBlock
		items <- items with: aBlock.
		itemtitles <- itemtitles with: name.
		enablestatus <- enablestatus with: true.
		<181 number name nil>
|
	enableItem: n
		enablestatus at: n put: true.
		<182 number n 1 1>
|
	disableItem: n
		enablestatus at: n put: false.
		<182 number n 1 0>
|
	selectItem: n inWindow: w
		" execute the selected menu item "
		(items at: n) value: w
|
	title: aString
		" give the title to a menu item"
		title <- aString
|
	create
		"create menu"
		<180 number title>.
		" reinstate any old items "
		(1 to: items size) do:
			[:i | <181 number (itemtitles at: i) nil>. 
				(enablestatus at: i) 
					ifFalse: [ self disableItem: i]]
]
Methods EventManager 'all'
	new
		responses <- Array new: 12.
		responses at: 1 put: [:w | w activate ].
		responses at: 2 put: [:w | w charTyped: (Char new; value: <171 4>) ].
		responses at: 3 put: [:w | w command: <171 9> ].
		responses at: 4 put: [:w | w mouseDownAt: self mouseLocation ].
		responses at: 5 put: [:w | w mouseMoveTo: self mouseLocation ].
		responses at: 6 put: [:w | w mouseUpAt: self mouseLocation ].
		responses at: 7 put: [:w | self eventMenu 
			selectItem: self menuItem inWindow: w ].
		responses at: 8 put: [:w | w reSized ].
		responses at: 9 put: [:w | w moved ].
		responses at: 10 put: [:w | w drawEvent ].
		responses at: 11 put: [:w | w timer ].
		responses at: 12 put: [:w | w deactivate ].
|
	eventWindow
		^ windows at: <171 1>
|
	eventMenu
		^ menus at: <171 2>
|
	menuItem
		^ <171 3>
|
	mouseLocation
		" return the current location of the mouse "
		^ <172 1>
|
	execute		| i |
		" process one event "
		i <- <170>.  (i = 0)
		ifFalse: [ (responses at: i) value: self eventWindow ]
]
Methods StandardWindows 'all'
	makeSystemMenu
		systemMenu isNil ifTrue:
			[ systemMenu <- Menu new; title: 'System'; create.
			systemMenu addItem: 'browser' 
				action: [:w | BrowserWindow new; title: 'Browser'; open ].
			systemMenu addItem: 'file in'
				action: [:w | [ File new; 
					fileIn: (smalltalk askFile: 'file name:')] fork ].
			systemMenu addItem: 'save image'
				action: [:w | [ smalltalk saveImage: 
					(smalltalk askNewFile: 'image file:') ] fork ].
			systemMenu addItem: 'quit'
				action: [:w | scheduler quit ]
			]
|
	makeWorkspaceMenu
		workspaceMenu isNil ifTrue: [
			workspaceMenu <- Menu new; title: 'Workspace'; create.
			workspaceMenu addItem: 'print it'
				action: [:w | [ w print:  w text value asString ] fork ].
			workspaceMenu addItem: 'do it'
				action: [:w | [ w text execute ] fork ]]
|
	makeWorkspace
		TextWindow new; title: 'Workspace';
			open; attachMenu: systemMenu; attachMenu: workspaceMenu.
]
*
* initialization code
* this is executed once, by the initial image maker
*
*
Methods Smalltalk 'doit'
	error: aString	| ew |
		" print a message, and remove current process "
		scheduler currentProcess trace.
		<204 aString>.
		scheduler currentProcess terminate
]
Methods Scheduler 'get commands'
	initialize
		stdwin makeSystemMenu.
		stdwin makeWorkspaceMenu.
		stdwin makeWorkspace.
		eventManager <- EventManager new.
		scheduler addProcess: eventManager
|
	quit
		" all done - really quit "
		" should probably verify first "
		notdone <- false
]
Methods UndefinedObject 'initial image'
	createGlobals
		" create global variables in initial image "
		true <- True new.
		false <- False new.
		smalltalk <- Smalltalk new.
		files <- Array new: 15.
		stdin <- File new; name: 'stdin'; mode: 'r'; open.
		stdout <- File new; name: 'stdout'; mode: 'w'; open.
		stderr <- File new; name: 'stderr'; mode: 'w'; open.
		printer <- stdout.
		" create a dictionary of classes "
		classes <- Dictionary new.
		symbols binaryDo: [:x :y | 
			(y class == Class)
				ifTrue: [ classes at: x put: y ] ].
		scheduler <- Scheduler new.
		stdwin <- StandardWindows new.
		windows <- Array new: 15.
		menus <- Array new: 15.
		windows <- Array new: 15.
|
	initialize	| aBlock |
		" initialize the initial object image "
		self createGlobals.
		" create the initial system process "
		" note the delayed recursive call "
		aBlock <- [ files do: [:f | f notNil ifTrue: [ f open ]].
			    menus do: [:m | m notNil ifTrue: [ m create ]].
			    windows do: [:w | w notNil ifTrue: [ w open ]].
			    systemProcess <- aBlock newProcess.
			    scheduler run ].
		systemProcess <- aBlock newProcess.
		File new;
			name: 'systemImage';
			open: 'wb';
			saveImage;
			close.
]
Methods String 'test'
	print
		^ printer print: self
]
Methods Smalltalk 'interface'
	getPrompt: aString
		^ <201 aString ''>
|
	askNewFile: prompt
		" ask for a new file name "
		^ <203 prompt '' 1>
|
	askFile: prompt
		^ <203 prompt '' 0>
|
	inquire: aString
		^ <202 aString 1>
]
Methods Rectangle 'drawing'
	frame
		<194 1 left top right bottom>
|
	paint
		<194 2 left top right bottom>
|
	erase
		<194 3 left top right bottom>
|
	invert
		<194 4 left top right bottom>
|
	shade: aPercent
		<195 1 left top right bottom aPercent>
]
Methods Smalltalk 'beep'
	beep
		<205>
]
Methods Circle 'drawing'
	frame
		<193 1 (center x) (center y) radius>
]
