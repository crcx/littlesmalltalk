Class Form
| text |
[
	new
		text <- Array new: 0
|
	clipFrom: upperLeft to: lowerRight	
		| newForm newRow rsize left top rText |

		left <- upperLeft y - 1.  " left hand side"
		top <- upperLeft x - 1.
		rsize <- lowerRight y - left.
		newForm <- Form new.
		(upperLeft x to: lowerRight x) do: [:i |
			newRow <- String new: rsize.
			rText <- self row: i.
			(1 to: rsize) do: [:j |
				newRow at: j
					put: (rText at: (left + j)
						ifAbsent: [$ ])].
			newForm row: (i - top) put: newRow ].
		^ newForm
|
	columns
		^ text inject: 0 into: [:x :y | x max: y size ]
|
	display
		smalltalk clearScreen.
		self printAt: 1 @ 1.
		'  ' printAt: 20 @ 0
|
	eraseAt: aPoint		| location |
		location <- aPoint copy.
		text do: [:x | (String new: (x size)) printAt: location.
				location x: (location x + 1) ]
|
	extent
		^ self rows @ self columns
|
	first
		^ text first
|
	next
		^ text next
|
	overLayForm: sourceForm at: startingPoint
		| newRowNumber rowText left rowSize |

		newRowNumber <- startingPoint x.
		left <- startingPoint y - 1.
		sourceForm do: [:sourceRow |
			rowText <- self row: newRowNumber.
			rowSize <- sourceRow size.
			rowText <- rowText padTo: (left + rowSize).
			(1 to: rowSize) do: [:i |
				((sourceRow at: i) ~= $ )
				ifTrue: [ rowText at: (left + i) 
						put: (sourceRow at: i)]].
			self row: newRowNumber put: rowText.
			newRowNumber <- newRowNumber + 1]
|
	placeForm: sourceForm at: startingPoint
		| newRowNumber rowText left rowSize |

		newRowNumber <- startingPoint x.
		left <- startingPoint y - 1.
		sourceForm do: [:sourceRow |
			rowText <- self row: newRowNumber.
			rowSize <- sourceRow size.
			rowText <- rowText padTo: (left + rowSize).
			(1 to: rowSize) do: [:i |
				rowText at: (left + i) 
					put: (sourceRow at: i)].
			self row: newRowNumber put: rowText.
			newRowNumber <- newRowNumber + 1]
|
	reversed		| newForm columns newRow |
		columns <- self columns.
		newForm <- Form new.
		(1 to: self rows) do: [:i |
			newRow <- text at: i.
			newRow <- newRow , 
				(String new: (columns - newRow size)).
			newForm row: i put: newRow reversed ].
		^ newForm

|
	rotated			| newForm rows newRow |
		rows <- self rows.
		newForm <- Form new.
		(1 to: self columns) do: [:i |
			newRow <- String new: rows.
			(1 to: rows) do: [:j |
				newRow at: ((rows - j) + 1)
					put: ((text at: j)
						at: i ifAbsent: [$ ])].
			newForm row: i put: newRow ].
		^ newForm
|
	row: index
		^ text at: index ifAbsent: ['']
|
	row: index put: aString
		(index > text size)
			ifTrue: [ [text size < index] whileTrue:
					[text <- text grow: ''] ].
		text at: index put: aString
|
	rows
		^ text size
|
	printAt: aPoint		| location |
		location <- aPoint copy.
		text do: [:x | x printAt: location.
				location x: (location x + 1) ]
]
