/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	Process Manager

	This module manages the stack of pending processes.
	SendMessage is called when it is desired to send a message to an
	object.  It looks up the method associated with the class of
	the receiver, then executes it.
	A block context is created only when it is necessary, and when it
	is required the routine executeFromContext is called instead of
	sendMessage.
	DoInterp is called by a primitive method to execute an interpreter,
	it returns the interpreter to which execution should continue
	following execution.
*/
# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"
# include "process.h"

# define ProcessStackMax 2000

	/* values set by interpreter when exiting */
int finalStackTop;	/* stack top when finished with interpreter */
int finalByteCounter;	/* bytecode counter when finished with interpreter */
int argumentsOnStack;	/* position of arguments on stack for mess send */
object messageToSend;	/* message to send */
object returnedObject;	/* object returned from message */
taskType finalTask;	/* next task to do (see below) */

static object blockReturnContext;

object processStack[ProcessStackMax];
int processStackTop = 0;

void sendMessage(object, object, int);
void executeFromContext();
extern void execute();
extern object getClass();


/*
	we cache recently used methods, in case we want them again
*/

# define ProcessCacheSize 101	/* a suitable prime number */

struct {
	object startClass, messageSymbol, methodClass, theMethod;
	} methodCache[ProcessCacheSize];

void prpush(newobj)
object newobj;
{
	incr(processStack[processStackTop++] = newobj);
	if (processStackTop >= ProcessStackMax)
		sysError("stack overflow","process stack");
}

/* flush out cache so new methods can be read in */
void flushMessageCache()
{	int i;

	for (i = 0; i < ProcessCacheSize; i++)
		methodCache[i].messageSymbol = nilobj;
}

static object findMethod(hash, message, startingClass)
int hash;
object message, startingClass;
{	object method, class, methodtable;

	/* first examine cache */
	if ((methodCache[hash].messageSymbol == message) &&
		(methodCache[hash].startClass == startingClass)) {
		/* found it in cache */
		method = methodCache[hash].theMethod;
		}
	else {	/* must look in methods tables */
		method = nilobj;
		class = startingClass;
		while ( class != nilobj ) {
			methodtable = basicAt(class, methodsInClass);
			if (methodtable != nilobj)
				method = nameTableLookup(methodtable, message);
			if (method != nilobj) {
				/* fill in cache */
				methodCache[hash].messageSymbol = message;
				methodCache[hash].startClass = startingClass;
				methodCache[hash].methodClass = class;
				methodCache[hash].theMethod = method;
				class = nilobj;
				}
			else
				class = basicAt(class, superClassInClass);
			}
		}

	return(method);
}

/* newContext - create a new context.  Note this returns three values,
via side effects
*/
static void newContext(method, methodClass, contextobj, argobj, tempobj)
object method, methodClass, *contextobj, argobj, *tempobj;
{	int temporarysize;

	*contextobj = allocObject(contextSize);
	incr(*contextobj);
	setClass(*contextobj, contextclass);
	basicAtPut(*contextobj, methodInContext, method);
	basicAtPut(*contextobj, methodClassInContext, methodClass);
	basicAtPut(*contextobj, argumentsInContext, argobj);
	temporarysize = intValue(basicAt(method, temporarySizeInMethod));
	*tempobj = newArray(temporarysize);
	basicAtPut(*contextobj, temporariesInContext, *tempobj);
}

void sendMessage(message, startingClass, argumentPosition)
object message, startingClass;
int argumentPosition;
{	object method, methodClass, size;
	object contextobj, tempobj, argobj, errMessage;
	int i, hash, bytecounter, temporaryPosition, errloc;
	int argumentsize, temporarySize=0;
	boolean done;

	/* compute size of arguments part of stack */
	argumentsize = processStackTop - argumentPosition;

	hash = (message + startingClass) % ProcessCacheSize;
	method = findMethod(hash, message, startingClass);
/*fprintf(stderr,"sending message %s class %s\n", charPtr(message), charPtr(basicAt(startingClass, nameInClass)));*/

	if (method == nilobj) {		/* didn't find it */
		errMessage = newSymbol("class:doesNotRespond:");
		if (message == errMessage)
			/* better give up */
			sysError("didn't find method", charPtr(message));
		else {
			errloc = processStackTop;
			prpush(smallobj);
			prpush(startingClass);
			prpush(message);
			sendMessage(errMessage, getClass(smallobj), errloc);
			}
		}
	else {			/* found it, start execution */
		/* initialize things for execution */
		bytecounter = 0;
		done = false;

		/* allocate temporaries */
		temporaryPosition = processStackTop;
		size = basicAt(method, temporarySizeInMethod);
		if (! isInteger(size))
			sysError("temp size not integer","in method");
		else
			for (i = temporarySize = intValue(size); i > 0; i--)
				prpush(nilobj);
		methodClass = methodCache[hash].methodClass;

		while( ! done ) {
			execute(method, bytecounter, 
				processStack, processStackTop,
				&processStack[argumentPosition],
				&processStack[temporaryPosition]);
			bytecounter = finalByteCounter;
			processStackTop = finalStackTop;

			switch(finalTask) {
				case sendMessageTask:
					sendMessage(messageToSend, 
						getClass(processStack[argumentsOnStack]),
						argumentsOnStack);
					if (finalTask == BlockReturnTask)
						done = true;
					break;

				case sendSuperTask:
					sendMessage(messageToSend,
						basicAt(methodCache[hash].methodClass, superClassInClass),
						argumentsOnStack);
					if (finalTask == BlockReturnTask)
						done = true;
					break;


				case ContextExecuteTask:
					contextobj = messageToSend;
					executeFromContext(contextobj,
						argumentsOnStack);
					decr(contextobj);
					if (finalTask == ReturnTask)
						processStack[processStackTop++] = returnedObject;
					else
						done = true;
					break;

				case BlockCreateTask:
					/* block is in returnedObject, we just add */
					/* context info  but first we must */
					/* create the context */
					argobj = newArray(argumentsize);
					newContext(method, methodClass, &contextobj, argobj, &tempobj);
					for (i = 1; i <= argumentsize; i++) {
						basicAtPut(argobj, i, processStack[argumentPosition + i - 1]);
						}
					for (i = 1; i <= temporarySize; i++) {
						basicAtPut(tempobj, i, processStack[temporaryPosition + i - 1]);
						}
					basicAtPut(returnedObject, contextInBlock, contextobj);
					processStack[processStackTop++] = returnedObject;
					/* we now execute using context - */
					/* so that changes to temp will be */
					/* recorded properly */
					executeFromContext(contextobj, bytecounter);
					while (processStackTop > argumentPosition) {
						decr(processStack[--processStackTop]);
						processStack[processStackTop] = nilobj;
						}

					/* if it is a block return, */
					/* see if it is our context */
					/* if so, make into a simple return */
					/* otherwise pass back to caller */
					/* we can decr, since only nums are */
					/* important */
					decr(contextobj);
					if (finalTask == BlockReturnTask) {
						if (blockReturnContext != contextobj)
							return;
						}
					finalTask = ReturnTask;
					/* fall into return code */

				case ReturnTask:
					while (processStackTop > argumentPosition) {
						decr(processStack[--processStackTop]);
						processStack[processStackTop] = nilobj;
						}
					/* note that ref count is picked up */
					/* from the interpreter */
					processStack[processStackTop++] = returnedObject;
					done = true;
					break;

				default:
					sysError("unknown task","in sendMessage");
				}
			}
		}
/*fprintf(stderr,"returning from message %s\n", charPtr(message));*/
}

/*
	execute from a context rather than from the process stack
*/
void executeFromContext(context, bytecounter)
object context;
int bytecounter;
{	object method, methodclass, arguments, temporaries;
	boolean done = false;

	method = basicAt(context, methodInContext);
	methodclass = basicAt(context, methodClassInContext);
	arguments = basicAt(context, argumentsInContext);
	temporaries = basicAt(context, temporariesInContext);

	while (! done) {
		execute(method, bytecounter, processStack, processStackTop,
			memoryPtr(arguments), memoryPtr(temporaries));
		bytecounter = finalByteCounter;
		processStackTop = finalStackTop;

		switch(finalTask) {
			case sendMessageTask:
				sendMessage(messageToSend, 
					getClass(processStack[argumentsOnStack]),
					argumentsOnStack);
				if (finalTask == BlockReturnTask)
					done = true;
				break;

			case sendSuperTask:
				sendMessage(messageToSend,
					basicAt(methodclass, superClassInClass),
					argumentsOnStack);
				if (finalTask == BlockReturnTask)
					done = true;
				break;

			case BlockCreateTask:
				/* block is in returnedObject already */
				/* just add our context to it */
				basicAtPut(returnedObject, contextInBlock, context);
				processStack[processStackTop++] = returnedObject;
				break;

			case BlockReturnTask:
				blockReturnContext = context;
				/* fall into next case and return */

			case ReturnTask:
				/* exit and let caller handle it */
				done = true;
				break;
	
			default:
				sysError("unknown task","in context execute");
		}
	}
}

void flushstack()
{
	while (processStackTop > 0) {
		decr(processStack[--processStackTop]);
		processStack[processStackTop] = nilobj;
		}
}

static void interpush(interp, value)
object interp, value;
{
	int stacktop;
	object stack;

	stacktop = 1 + intValue(basicAt(interp, stackTopInInterpreter));
	stack = basicAt(interp, stackInInterpreter);
	basicAtPut(stack, stacktop, value);
	basicAtPut(interp, stackTopInInterpreter, newInteger(stacktop));
}

object doInterp(interpreter)
object interpreter;
{	object context, method, arguments, temporaries, stack;
	object prev, contextobj, obj, argobj, class, newinterp, tempobj;
	int i, hash, argumentSize, bytecounter, stacktop;

	context = basicAt(interpreter, contextInInterpreter);
	method = basicAt(context, methodInContext);
	arguments = basicAt(context, argumentsInContext);
	temporaries = basicAt(context, temporariesInContext);
	stack = basicAt(interpreter, stackInInterpreter);
	stacktop = intValue(basicAt(interpreter, stackTopInInterpreter));
	bytecounter = intValue(basicAt(interpreter, byteCodePointerInInterpreter));

	execute(method, bytecounter, memoryPtr(stack), stacktop,
		memoryPtr(arguments), memoryPtr(temporaries));
	basicAtPut(interpreter, stackTopInInterpreter, newInteger(finalStackTop));
	basicAtPut(interpreter, byteCodePointerInInterpreter, newInteger(finalByteCounter));

	switch(finalTask) {
		case sendMessageTask:
		case sendSuperTask:
			/* first gather up arguments */
			argumentSize = finalStackTop - argumentsOnStack;
			argobj = newArray(argumentSize);
			for (i = argumentSize; i >= 1; i--) {
				obj = basicAt(stack, finalStackTop);
				basicAtPut(argobj, i, obj);
				basicAtPut(stack, finalStackTop, nilobj);
				finalStackTop--;
				}

			/* now go look up method */
			if (finalTask == sendMessageTask)
				class = getClass(basicAt(argobj, 1));
			else 
				class = basicAt(basicAt(context, 
					methodClassInContext), superClassInClass);
			hash = (messageToSend + class) % ProcessCacheSize;
			method = findMethod(hash, messageToSend, class);

			if (method == nilobj) {
				/* didn't find it, change message */
				incr(argobj);	/* get rid of old args */
				decr(argobj);
				argobj = newArray(3);
				basicAtPut(argobj, 1, smallobj);
				basicAtPut(argobj, 2, class);
				basicAtPut(argobj, 3, messageToSend);
				class = getClass(smallobj);
				messageToSend = newSymbol("class:doesNotRespond:");
				hash = (messageToSend + class) % ProcessCacheSize;
				method = findMethod(hash, messageToSend, class);
				if (method == nilobj)	/* oh well */
					sysError("cant find method",charPtr(messageToSend));
				}
			newContext(method, methodCache[hash].methodClass, &contextobj, argobj, &tempobj);
			basicAtPut(interpreter, stackTopInInterpreter, newInteger(finalStackTop));
			argumentsOnStack = 0;
			/* fall into context execute */

		case ContextExecuteTask:
			if (finalTask == ContextExecuteTask) {
				contextobj = messageToSend;
				}
			newinterp = allocObject(InterpreterSize);
			setClass(newinterp, intrclass);
			basicAtPut(newinterp, contextInInterpreter, contextobj);
			basicAtPut(newinterp, previousInterpreterInInterpreter, interpreter);
			/* this shouldn't be 15, but what should it be?*/
			basicAtPut(newinterp, stackInInterpreter, newArray(15));
			basicAtPut(newinterp, stackTopInInterpreter, newInteger(0));
			basicAtPut(newinterp, byteCodePointerInInterpreter, newInteger(argumentsOnStack));
			decr(contextobj);
			return(newinterp);
			break;

		case BlockCreateTask:
			basicAtPut(returnedObject, contextInBlock, context);
			prev = basicAt(interpreter, creatingInterpreterInInterpreter);
			if (prev == nilobj)
				prev = interpreter;
			basicAtPut(returnedObject, creatingInterpreterInBlock, prev);
			interpush(interpreter, returnedObject);
			decr(returnedObject);
			return(interpreter);

		case BlockReturnTask:
			interpreter = basicAt(interpreter, creatingInterpreterInInterpreter);
			/* fall into return task */

		case ReturnTask:
			prev = basicAt(interpreter, previousInterpreterInInterpreter);
			if (prev != nilobj) {
				interpush(prev, returnedObject);
				}
			/* get rid of excess ref count */
			decr(returnedObject);
			return(prev);

		default:
			sysError("unknown final task","doInterp");
		}
	return(nilobj);
}
