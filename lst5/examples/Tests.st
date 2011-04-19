
# This is a shell archive.  Remove anything before this line,
# then unpack it by saving it in a file and typing "sh file".
#
# Wrapped by Tim Budd <budd@ada> on Fri Dec 16 10:16:47 1994
#
# This archive contains:
#	concordance.stt	palendrome.stt	queen.stt	test1.stt	
#	while.stt	
#

LANG=""; export LANG
PATH=/bin:/usr/bin:$PATH; export PATH

echo x - concordance.stt
cat >concordance.stt <<'@EOF'
/  Concordance program
+Object subclass: #Concordance variables: #(dict)
!Concordance
word: word occursOnLine: line
	(dict includes: word)
		ifFalse: [ dict at: word put: List new ].
	((dict at: word) includes: line)
		ifFalse: [ (dict at: word) addLast: line]
!
!Concordance
fileIn	| text lineNumber words |
	lineNumber <- 0.
	[ text <- String input.  text notNil ]
		whileTrue: [ 
			text <- text collect: [ :c | c lowerCase ].
			lineNumber <- lineNumber + 1.
			words <- text break: ' .,'.
			words do: [ :word | self word: word 
				occursOnLine: lineNumber ] ]
!
!Concordance
displayDictionary
	dict binaryDo: [ :word :lines |
		word print.
		lines do: [ :line | ' ' print. line print ].
		Char newline print ]
!
!Concordance
run
	dict <- Dictionary new.
	self fileIn.
	self displayDictionary
!
+'enter test lines, followed by end-of-file'
+Concordance new run
/ end
@EOF

chmod 600 concordance.stt

echo x - palendrome.stt
cat >palendrome.stt <<'@EOF'
/ A simple palendrome tester
/ first, add a method to class String
!String
isPalendrome | string |
	string <- self select: [:c | c isAlphabetic ].
	string <- string collect: [:c | c lowerCase ].
	^ string = string reverse
!
/ next, add a new class with a few test cases
+Object subclass: #Pal
!Pal
test: aString
	aString print.
	' : ' print.
	aString isPalendrome print.
	Char newline print.
!
!Pal
runTests
	self test: 'A Man, A Plan, A Canal, Panama'.
	self test: 'Rats live on no Evil star'.
	self test: 'This is not a palendrome'
!
+Pal new runTests
/ end of file
@EOF

chmod 600 palendrome.stt

echo x - queen.stt
cat >queen.stt <<'@EOF'
/ eight queens exercise
+Object subclass: #NullQueen 
!NullQueen
first
	^ true
!
!NullQueen
next
	^ false
!
!NullQueen
checkRow: row column: column
	^ false
!
!NullQueen
result
	^ List new
!
+Object subclass: #Queen variables: #(row column neighbor)
!Queen
setColumn: aNumber neighbor: aQueen
	column <- aNumber.
	neighbor <- aQueen
!
!Queen
checkRow: testRow column: testColumn | columnDifference |
	columnDifference <- testColumn - column.
	(((row = testRow) or:
		[ row + columnDifference = testRow]) or:
		[ row - columnDifference = testRow])
			ifTrue: [ ^ true ].
	^ neighbor checkRow: testRow column: testColumn
!
!Queen
first
	neighbor first.
	row <- 1.
	^ self testPosition
!
!Queen
next
	^ self advance and: [ self testPosition ]
!
!Queen
advance
	(row = 8)
		ifTrue: [ neighbor next ifFalse: [ ^ false ].
			row <- 0 ].
	row <- row + 1.
	^ true
!
!Queen
testPosition
	[ neighbor checkRow: row column: column ]
		whileTrue: [ self advance ifFalse: [ ^ false ]].
	^ true
!
!Queen
result	
	^ neighbor result; addLast: row
!
+Object subclass: #Main
!Main
run	| lastQueen |
	lastQueen <- NullQueen new.
	1 to: 8 do: [:i | lastQueen <- Queen new; 
		setColumn: i neighbor: lastQueen ].
	lastQueen first.
	'got a result' print.
	lastQueen result do: [:x | x print. ' ' print ].
	Char newline print.
	[ lastQueen next ] whileTrue:
		[
	lastQueen result do: [:x | x print. ' ' print ].
	Char newline print ]
!
+Main new run
/end
@EOF

chmod 600 queen.stt

echo x - test1.stt
cat >test1.stt <<'@EOF'
/ test class instance variables
+Object subclass: #A variables: #( #a )
+A subclass: #B variables: #( #b )
!A
seta: x
	a <- x
!
!A
geta
   ^ a
!
!B
setb: y
   b <- y.
   a <- y + 1.
!
!B
getb
   ^ b
!
!B
test
   self seta: 3.
   self geta print.
   self setb: 7.
   self geta print.
   self getb print.
   Char newline print
!
+B new test
@EOF

chmod 600 test1.stt

echo x - while.stt
cat >while.stt <<'@EOF'
!Object
isKindOf: aClass	| clas |
	clas <- self class.
	[ clas notNil ] whileTrue:
		[ clas == aClass ifTrue: [ ^ true ].
		  clas <- clas superclass ].
	^ false
!
!Undefined
main	| command |
	[ '-> ' print. command <- String input. command notNil ]
		whileTrue: [ command isEmpty
			ifFalse: [ command doIt print. Char newline print ] ]
!
=String
input	| value c nl |
	" read a line from input "
	value <- ''. nl <- Char newline.
	[ c <- Char input. 
	  c isNil ifTrue: [ ^ nil ]. c ~= nl ] whileTrue:
		[ value <- value + c asString ].
	^ value
!
!Number
to: limit do: aBlock  | i |
		" optimize arithmetic loops "
	i <- self.
	[ i <= limit ] whileTrue: [ aBlock value: i. i <- i + 1 ]
!
!File
readLine	| value  c|
	" read a line from input "
	value <- ''.
	[ c <- self readChar. 
	  c isNil ifTrue: [ ^ nil ]. c ~= Char newline ] whileTrue:
		[ value <- value + c asString ].
	^ value
!
!Parser
skipBlanks  | cc |
	[ cc <- self currentChar.
	  cc notNil and: [ cc isBlank ] ]
			whileTrue: [ index <- index + 1 ].
	(cc notNil and: [ cc = $" ] )
		ifTrue: [ self skipComment ]
!
!Parser
skipComment  | cc |
	[ index <- index + 1.
	  cc <- self currentChar.
	  cc isNil 
		ifTrue: [ ^ self error: 'unterminated comment'].
	  cc ~= $" ] whileTrue: [ nil ].
	self nextChar. self skipBlanks
!
+1 to: 15 do: [:x| x factorial print. Char newline print]
+(5 to: 1 by: -1) do: [:x| x printNl]
/ end of input
@EOF

chmod 600 while.stt

exit 0
