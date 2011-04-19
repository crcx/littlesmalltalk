/*
	Little Smalltalk

	The following very common commands are given a concise description
	in bytecodes.

*/

static char *unspecial[] = {"new", "isNil", "notNil", "size", "class",
		"value", "first", "next", "print", "printString",
		"strictlyPositive", "currentKey", "not",

		/* after the first 16 - which should be the most common
		messages, order doesn't make as much difference so we
		might as well list things in alphabetical order */

		"abs",
		"asArray", 
		"asFloat", 
		"asString", 
		"asSymbol", 
		"block",
		"compareError", 
		"copy", 
		"current",
		"deepCopy",
		"exp", 
		"findLast",
		"firstKey",
		"gamma", 
		"isEmpty",
		"isLowercase",
		"isUppercase",
		"last",
		"lastKey",
		"ln", 
		"newProcess",
		"not", 
		"opError",
		"read",
		"removeError",
		"removeFirst",
		"removeLast",
		"resume",
		"rounded", 
		"shallowCopy", 
		"sqrt",
		"squared", 
		"state",
		"superClass",
		"truncated", 
		"unblock",
		"x", 
		"y", 
		0 };

# define VALUECMD 5
# define PRNTCMD  8

static char *binspecial[] = {"new:", "at:", "to:", "do:", "value:",
                      "==", "~~", "timesRepeat:", "whileTrue:", "whileFalse:",
                      "ifTrue:", "ifFalse:", "error:", "add:", 
		      "/", "coerce:",

		      "^", 
		      ",", 
		      "//",
		      "addAll:", 
		      "addAllLast:", 
		      "addFirst:", 
		      "addLast:", 
		      "binaryDo:", 
		      "checkBucket:",
		      "collect:",
		      "deepCopy:", 
		      "gcd:", 
		      "getList:",
		      "hashNumber:",
		      "includes:", 
		      "inRange:", 
		      "keysDo:", 
		      "log:", 
		      "maxtype:", 
		      "newProcessWith:",
		      "occurrencesOf:",
		      "raisedTo:",
		      "reject:",
		      "remove:", 
		      "removeKey:", 
		      "respondsTo:",
		      "reverseDo:",
		      "roundTo:", 
		      "select:",
		      "shallowCopy:", 
		      "sort:", 
		      "termErr:",
		      "truncateTo:",
		      "write:",
		      "x:", 
		      "y:", 
		      "includesKey:",
		      0};

static char *arithspecial[] = {"+", "-", "*", "\\\\",
                        "bitShift:", "bitAnd:", "bitOr:",
                        "<", "<=", "=", "~=", ">=", ">", 
			"rem:", "quo:", "min:", "max:", 
			0};

static char *keyspecial[] = {"at:put:", "ifTrue:ifFalse:", "ifFalse:ifTrue:",
                        "value:value:", "to:by:", "at:ifAbsent:",
			"indexOf:ifAbsent:", "inject:into:", 
			"remove:ifAbsent:", "removeKey:ifAbsent:", 
			"between:and:", 
			"findFirst:ifAbsent:", "findLast:ifAbsent:",
			"equals:startingAt:",
			"findAssociation:inList:",
			"detect:ifAbsent:",
			0};

/*	The classes included in the standard prelude
	also have a very concise description in bytecode representation
*/

static char *classpecial[] = {"Array", "ArrayedCollection", 
	"Bag", "Block", "Boolean", "ByteArray",
	"Char", "Class", "Collection", "Complex",
	"Dictionary", "False", "File", "Float",
	"Integer", "Interpreter", "Interval",
	"KeyedCollection", "List", "Magnitude", "Number",
	"Object", "OrderedCollection", "Point",
	"Radian", "Random",
	"SequenceableCollection", "Set", "String", "Symbol",
	"True", "UndefinedObject",
	0 };
