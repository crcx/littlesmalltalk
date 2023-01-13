/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	names and sizes of internally object used internally in the system
*/

# define classSize 6
# define nameInClass 1
# define sizeInClass 2
# define methodsInClass 3
# define superClassInClass 4
# define variablesInClass 5

# define methodSize 6
# define textInMethod 1
# define messageInMethod 2
# define bytecodesInMethod 3
# define literalsInMethod 4
# define stackSizeInMethod 5
# define temporarySizeInMethod 6

# define contextSize 6
# define methodInContext 1
# define methodClassInContext 2
# define argumentsInContext 3
# define temporariesInContext 4

# define blockSize 6
# define contextInBlock 1
# define argumentCountInBlock 2
# define argumentLocationInBlock 3
# define bytecountPositionInBlock 4
# define creatingInterpreterInBlock 5

# define InterpreterSize 6
# define contextInInterpreter 1
# define previousInterpreterInInterpreter 2
# define creatingInterpreterInInterpreter 3
# define stackInInterpreter 4
# define stackTopInInterpreter 5
# define byteCodePointerInInterpreter 6

extern object nameTableLookup();

# define globalSymbol(s) nameTableLookup(globalNames, newSymbol(s))

extern object trueobj;		/* the pseudo variable true */
extern object falseobj;		/* the pseudo variable false */
extern object smallobj;		/* the pseudo variable smalltalk */
extern object blockclass;	/* the class ``Block'' */
extern object contextclass;	/* the class ``Context'' */
extern object intclass;		/* the class ``Integer'' */
extern object intrclass;	/* the class ``Interpreter'' */
extern object symbolclass;	/* the class ``Symbol'' */
extern object stringclass;	/* the class ``String'' */

extern object newSymbol();	/* new smalltalk symbol */
extern object newArray();	/* new array */
extern object newStString();	/* new smalltalk string */
extern object newFloat();	/* new floating point number */
