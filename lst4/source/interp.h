/*
	Little Smalltalk, version 4
	Written by Tim Budd, Oregon State University, budd@cs.orst.edu
	All rights reserved, no guarantees given whatsoever.
	May be freely redistributed if not for profit.
*/

int execute(struct object *aProcess, int ticks);

/*
	symbolic definitions for the bytecodes
*/

# define Extended 0
# define PushInstance 1
# define PushArgument 2
# define PushTemporary 3
# define PushLiteral 4
# define PushConstant 5
# define AssignInstance 6
# define AssignTemporary 7
# define MarkArguments 8
# define SendMessage 9
# define SendUnary 10
# define SendBinary 11
# define PushBlock 12
# define DoPrimitive 13
# define DoSpecial 15

/* types of special instructions (opcode 15) */

# define SelfReturn 1
# define StackReturn 2
# define BlockReturn 3
# define Duplicate 4
# define PopTop 5
# define Branch 6
# define BranchIfTrue 7
# define BranchIfFalse 8
# define SendToSuper 11
# define Breakpoint 12

/* special constants */

/* constants 0 to 9 are the integers 0 to 9 */
# define nilConst 10
# define trueConst 11
# define falseConst 12

/* Return values from doExecute: primitive */
#define ReturnError 2		/* error: called */
#define ReturnBadMethod 3	/* Unknown method for object */
#define ReturnReturned 4	/* Top level method returned */
#define ReturnTimeExpired 5	/* Time quantum exhausted */
#define ReturnBreak 6		/* Breakpoint instruction */

/*
	The following are the objects with ``hard-wired'' 
	field offsets
*/
/*
	A Process has two fields
		* a current context
		* status of process (running, waiting, etc)
*/

# define contextInProcess 0
# define statusInProcess 1
# define resultInProcess 2

/*
	A Context has:
		* method (which has bytecode pointer)
		* bytecode offset (an integer)
		* arguments
		* temporaries and stack
		* stack pointer
*/

# define contextSize 7
# define methodInContext 0
# define argumentsInContext 1
# define temporariesInContext 2
# define stackInContext 3
# define bytePointerInContext 4
# define stackTopInContext 5
# define previousContextInContext 6

/*
	A Block is subclassed from Context
	shares fields with creator, plus a couple new ones
*/

# define blockSize 10
# define methodInBlock methodInContext
# define argumentsInBlock argumentsInContext
# define temporariesInBlock temporariesInContext
# define stackInBlock stackInContext
# define stackTopInBlock stackTopInContext
# define previousContextInBlock previousContextInContext
# define argumentLocationInBlock 7
# define creatingContextInBlock 8
/* the following are saved in different place so they don't get clobbered*/
# define bytePointerInBlock 9

/*
	A Method has:
		* name (a Symbol)
		* bytecodes
		* literals
		* stack size
		* temp size
*/

# define methodSize 7
# define nameInMethod 0
# define byteCodesInMethod 1
# define literalsInMethod 2
# define stackSizeInMethod 3
# define temporarySizeInMethod 4
# define classInMethod 5
# define textInMethod 6

/*
	A Class has:
		* pointer to parent class
		* pointer to tree of methods
*/

# define ClassSize 5
# define nameInClass 0
# define parentClassInClass 1
# define methodsInClass 2
# define instanceSizeInClass 3
# define variablesInClass 4

/*
	A node in a tree has
		* value field
		* left subtree
		* right subtree
*/

# define valueInNode 0
# define leftInNode 1
# define rightInNode 2

/*
	misc defines
*/

# define rootInTree 0
# define receiverInArguments 0
# define symbolsInSymbol 5
