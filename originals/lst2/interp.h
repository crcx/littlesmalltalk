/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	symbolic definitions for the bytecodes
*/

# define Extended 0
# define PushInstance 1
# define PushArgument 2
# define PushTemporary 3
# define PushLiteral 4
# define PushConstant 5
# define PushGlobal 6
# define PopInstance 7
# define PopTemporary 8
# define SendMessage 9
# define SendUnary 10
# define SendBinary 11
# define SendKeyword 12
# define DoPrimitive 13
# define CreateBlock 14
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
# define AndBranch 9
# define OrBranch 10
# define SendToSuper 11
