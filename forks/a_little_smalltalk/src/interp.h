/*
 Little Smalltalk, version 2
 Written by Tim Budd, Oregon State University, July 1987
 */
/*
 symbolic definitions for the bytecodes
 */

#ifndef _INTERP_H
#define _INTERP_H

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
# define DoPrimitive 13
# define DoSpecial 15

/* a few constants that can be pushed by PushConstant */
# define minusOne 3		/* the value -1 */
# define contextConst 4		/* the current context */
# define nilConst 5		/* the constant nil */
# define trueConst 6		/* the constant true */
# define falseConst 7		/* the constant false */

/* types of special instructions (opcode 15) */

# define SelfReturn 1
# define StackReturn 2
# define Duplicate 4
# define PopTop 5
# define Branch 6
# define BranchIfTrue 7
# define BranchIfFalse 8
# define AndBranch 9
# define OrBranch 10
# define SendToSuper 11

//static int messTest(object obj);

/* flush an entry from the cache (usually when its been recompiled) */
void flushCache(object messageToSend, object STclass);

/*
 findMethod
 given a message and a class to start looking in,
 find the method associated with the message
 */
//static boolean findMethod(object *methodClassLocation);

//static object growProcessStack(int top, int toadd);

boolean execute(object aProcess, int maxsteps);

extern object trueobj, falseobj;

#endif