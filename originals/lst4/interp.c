/* 
	Little Smalltalk version 4 

	Written by Tim Budd, Oregon State University, July 1994

    	budd@cs.orst.edu

    bytecode interpreter module

    given a process object, execute bytecodes in a tight loop.

    performs subroutine calls for
	a) garbage collection
        b) finding a non-cached method
        c) executing a primitive
	d) creating an integer

    otherwise simply loops until time slice has ended
*/

# include "memory.h"
# include "interp.h"
# include <stdio.h>

extern int debugging;
extern int cacheHit;
extern int cacheMiss;


extern struct object * nwInteger(int);
# define newInteger(x) ((x<10)?smallInts[x]:nwInteger(x))

extern struct object * primitive(int, struct object *);

/*
	The following are roots for the file out 
*/

struct object * nilObject;
struct object * smallInts[10];
struct object * trueObject;
struct object * falseObject;
struct object * SmallIntClass;
struct object * ArrayClass;
struct object * BlockClass;
struct object * ContextClass;
struct object * globalsObject;
struct object * initialMethod;
struct object * binaryMessages[3];

/*
	method lookup routine, used when cache miss occurs
*/

static int symbolcomp(struct object * left, struct object * right)
{
	int leftsize = left->size >> 2;
	int rightsize = right->size >> 2;
	int minsize = leftsize;
	register int i;

	if (rightsize < minsize) minsize = rightsize;
	for (i = 0; i < minsize; i++) {
		if (bytePtr(left)[i] != bytePtr(right)[i]) {
			return bytePtr(left)[i]-bytePtr(right)[i];
			}
		}
	return leftsize - rightsize;
}

static struct object * lookupMethod
	(struct object * selector, struct object * class)
{
	struct object * dict;
	struct object * tree;
	struct object * node;
	struct object * aMeth;
	struct object * symb;
	struct object * anAssoc;

	for ( ; class != nilObject; class = class->data[parentClassInClass]) {
		dict = class->data[methodsInClass];
		tree = dict->data[0];
		node = tree->data[rootInTree];
		while (node != nilObject) {
			anAssoc = node->data[valueInNode];
			symb = anAssoc->data[0];
			if (symb == selector)
				return anAssoc->data[1];
			if (symbolcomp(selector, symb) < 0)
				node = node->data[leftInNode];
			else
				node = node->data[rightInNode];
			}
		}
	return 0;
}

/*
	method cache for speeding method lookup
*/

# define cacheSize 703

static struct {
	struct object * name;
	struct object * class;
	struct object * method;
	} cache[cacheSize];

void flushCache() {	/* flush dynamic methods when GC occurs */
	int i;
	for (i = 0; i < cacheSize; i++)
		if (isDynamicMemory(cache[i].method)
			|| isDynamicMemory(cache[i].class)
			|| isDynamicMemory(cache[i].name))
				cache[i].name = 0;	/* force refill */
}

int execute (struct object * aProcess)
{   
    register int low;
    register int high;
    register struct object * context;
    struct object * method;
    struct object * arguments;
    struct object * temporaries;
    struct object * instanceVariables;
    struct object * literals;
    register struct object * stack;

    struct object * returnedValue;
    struct object * messageSelector;
    struct object * receiverClass;

    unsigned char * bp;
    int stackTop;
    int bytePointer;

    int lastPrimitiveNumber;

    /* push process, so as to save it */
    rootStack[rootTop++] = aProcess;

    /* get current context information */
    context = aProcess->data[contextInProcess];

    method = context->data[methodInContext];

	/* load byte pointer */
    bp = (unsigned char *) bytePtr(method->data[byteCodesInMethod]);
    bytePointer = integerValue(context->data[bytePointerInContext]);

	/* load stack */
    stack = context->data[stackInContext];
    stackTop = integerValue(context->data[stackTopInContext]);

	/* everything else can wait, as maybe won't be needed at all */
    temporaries = instanceVariables = arguments = literals = 0;


    while (1) {
        low = (high = bp[bytePointer++] ) & 0x0F;
        high >>= 4;
        if (high == 0) {
            high = low;
            low = bp[bytePointer++] ;
            }

        switch(high) {

            case PushInstance:
		if (! arguments) 
			arguments = context->data[argumentsInContext];
		if (! instanceVariables)
			instanceVariables = 
				arguments->data[receiverInArguments];
		stack->data[stackTop++] = instanceVariables->data[low];
                break;

            case PushArgument:
		if (! arguments) 
			arguments = context->data[argumentsInContext];
		stack->data[stackTop++] = arguments->data[low];
                break;

	    case PushTemporary:
		if (! temporaries)
			temporaries = context->data[temporariesInContext];
		stack->data[stackTop++] = temporaries->data[low];
		break;

            case PushLiteral:
		if (! literals) 
			literals = method->data[literalsInMethod];
		stack->data[stackTop++] = literals->data[low];
                break;

            case PushConstant:
		switch(low) {
			case 0: case 1: case 2: case 3: case 4: 
			case 5: case 6: case 7: case 8: case 9:
				stack->data[stackTop++] = smallInts[low];
				break;
			case nilConst: 
				stack->data[stackTop++] = nilObject; break;
			case trueConst: 
				stack->data[stackTop++] = trueObject; break;
			case falseConst: 
				stack->data[stackTop++] = falseObject; break;
			default:
				sysError("unknown push constant", low);
			}
                break;

	    case PushBlock:
			/* create a block object */
			/* low is arg location */
			/* next byte is goto value */
		high = bp[bytePointer++];
		rootStack[rootTop++] = context;
		rootStack[rootTop++] = 
			gcalloc(integerValue(method->data[stackSizeInMethod]));
		rootStack[rootTop++] = newInteger(low);
		rootStack[rootTop++] = newInteger(bytePointer);
		returnedValue = gcalloc(blockSize);
		returnedValue->class = BlockClass;
		returnedValue->data[bytePointerInBlock] = rootStack[--rootTop];
		returnedValue->data[argumentLocationInBlock] 
			= rootStack[--rootTop];
		returnedValue->data[stackInBlock] = rootStack[--rootTop];
		context = rootStack[--rootTop];
		if (context->class == BlockClass)
			returnedValue->data[creatingContextInBlock] =
				context->data[creatingContextInBlock];
		else
			returnedValue->data[creatingContextInBlock] = context;
		method = returnedValue->data[methodInBlock] = 
			context->data[methodInBlock];
		arguments = returnedValue->data[argumentsInBlock] =
			context->data[argumentsInBlock];
		temporaries = returnedValue->data[temporariesInBlock] =
			context->data[temporariesInBlock];
		stack = context->data[stackInContext];
		bp = bytePtr(method->data[byteCodesInMethod]);
		stack->data[stackTop++] = returnedValue;
			/* zero these out just in case GC occurred */
		literals = instanceVariables = 0;
		bytePointer = high;
		break;

            case AssignInstance:
		if (! arguments) 
			arguments = context->data[argumentsInContext];
		if (! instanceVariables)
			instanceVariables = 
				arguments->data[receiverInArguments];
				/* don't pop stack, leave result there */
		instanceVariables->data[low] = stack->data[stackTop-1];
				/* if changing a static area, need
					to add to roots */
		if ((! isDynamicMemory(instanceVariables)) 
			&& isDynamicMemory(stack->data[stackTop-1]))
			{
			staticRoots[staticRootTop++] = 
				& instanceVariables->data[low];
			}
                break;

            case AssignTemporary:
		if (! temporaries)
			temporaries = context->data[temporariesInContext];
		temporaries->data[low] = stack->data[stackTop-1];
                break;

            case MarkArguments:
		rootStack[rootTop++] = context;
		arguments = gcalloc(low);
		arguments->class = ArrayClass;
		if (context != rootStack[--rootTop]) { /* gc has occurred */
				/* reload context */
			instanceVariables = temporaries = literals = 0;
			context = rootStack[rootTop];
			method = context->data[methodInContext];
			bp = bytePtr(method->data[byteCodesInMethod]);
			stack = context->data[stackInContext];
			}
			/* now load new argument array */
		while (low > 0) {
			arguments->data[--low] = stack->data[--stackTop];
			}
		stack->data[stackTop++] = arguments;
		arguments = 0;
                break;

            case SendMessage: 
		if (! literals) 
			literals = method->data[literalsInMethod];
		messageSelector = literals->data[low];
		arguments = stack->data[--stackTop];

	     findMethodFromSymbol:
		receiverClass = arguments->data[receiverInArguments]->class;
	     checkCache:
		low = (((unsigned int) messageSelector) +
			((unsigned int) receiverClass)) % cacheSize;
		if ((cache[low].name == messageSelector) &&
		    (cache[low].class == receiverClass)) {
			method = cache[low].method;
			cacheHit++;
			}
		else {
			cacheMiss++;
			method = lookupMethod(messageSelector, receiverClass);
			if (! method) {
				aProcess = rootStack[--rootTop];
				aProcess->data[contextInProcess] = context;
				aProcess->data[resultInProcess] = messageSelector;
				return 3;
				}
			cache[low].name = messageSelector;
			cache[low].class = receiverClass;
			cache[low].method = method;
			}

	      buildContextFromMethod:
			/* see if we can optimize tail call */
		if (bp[bytePointer] == (DoSpecial * 16 + StackReturn))
			high = 1;
		else if (bp[bytePointer] == (DoSpecial * 16 + BlockReturn))
			high = 2;
		else    high = 0;
			/* build temporaries for new context */
		rootStack[rootTop++] = arguments;
		rootStack[rootTop++] = method;
		rootStack[rootTop++] = context;
		low = integerValue(method->data[temporarySizeInMethod]);
		rootStack[rootTop++] = 
			gcalloc(integerValue(method->data[stackSizeInMethod]));
		if (low > 0) {int i;
			temporaries = gcalloc(low);
			temporaries->class = ArrayClass;
			for (i = 0; i < low; i++)
				temporaries->data[i] = nilObject;
			rootStack[rootTop++] = temporaries; /* temporaries */
			}
		else
			rootStack[rootTop++] = 0;	/* why bother */
			/* the following silly gyrations are just in case */
			/* gc occurs while we are building a couple of */
			/* integers to save the current state */
		rootStack[rootTop++] = newInteger(bytePointer);
		rootStack[rootTop++] = newInteger(stackTop);
		context = rootStack[rootTop-5];
		context->data[stackTopInContext] = rootStack[--rootTop];
		context->data[bytePointerInContext] = rootStack[--rootTop];
			/* now go off and build the new context */
		context = gcalloc(contextSize);
		context->class = ContextClass;
		temporaries = context->data[temporariesInContext] 
			= rootStack[--rootTop];
		stack = context->data[stackInContext] = rootStack[--rootTop];
		stack->class = ArrayClass;
		stackTop = 0;
		context->data[previousContextInContext] = rootStack[--rootTop];
		if (high == 1)
			context->data[previousContextInContext] =
			context->data[previousContextInContext]->
				data[previousContextInContext];
		else if (high == 2)
			context->data[previousContextInContext] =
			context->data[previousContextInContext]->
				data[creatingContextInBlock]->
				data[previousContextInContext];
			
		method = context->data[methodInContext] = rootStack[--rootTop];
		arguments = context->data[argumentsInContext] 
			= rootStack[--rootTop];
		instanceVariables = literals = 0;
		bytePointer = 0;
		bp = (char *) method->data[byteCodesInMethod]->data;
			/* now go execute new method */
		break;

	    case SendUnary:	/* optimize certain unary messages */
		returnedValue = stack->data[--stackTop];
		switch(low) {
			case 0:	/* isNil */
				if (returnedValue == nilObject)
					returnedValue = trueObject;
				else
					returnedValue = falseObject;
				break;
			case 1: /* notNil */
				if (returnedValue == nilObject)
					returnedValue = falseObject;
				else
					returnedValue = trueObject;
				break;
			default:
				sysError("unimplemented SendUnary", low);
			}
		stack->data[stackTop++] = returnedValue;
		break;

	    case SendBinary:	/* optimize certain binary messages */
		if ((stack->data[stackTop-1]->class == SmallIntClass)
			&& (stack->data[stackTop-2]->class == SmallIntClass)) {
			int i, j;
			j = integerValue(stack->data[--stackTop]);
			i = integerValue(stack->data[--stackTop]);
				/* can only do operations that won't */
				/* trigger garbage collection */
			switch(low) {
				case 0:	/* < */
					if (i < j)
						returnedValue = trueObject;
					else
						returnedValue = falseObject;
					break;
				case 1:	/* <= */
					if (i <= j)
						returnedValue = trueObject;
					else
						returnedValue = falseObject;
					break;
				case 2:	/* + */
					/* can only optimize if there is */
					/* no possibility of garbage col */
					if (i + j < 10)
						returnedValue = smallInts[i+j];
					else {
						/* undo the stack pop */
						stackTop += 2;
						goto longway;
						}
					break;
				}
			stack->data[stackTop++] = returnedValue;
			break;
			}
			/* not integers, do as message send */
		longway:
		rootStack[rootTop++] = context;
		arguments = gcalloc(2);
		arguments->class = ArrayClass;
		if (context != rootStack[--rootTop]) { /* gc has occurred */
				/* reload context */
			instanceVariables = temporaries = literals = 0;
			context = rootStack[rootTop];
			method = context->data[methodInContext];
			bp = bytePtr(method->data[byteCodesInMethod]);
			stack = context->data[stackInContext];
			}
			/* now load new argument array */
		arguments->data[1] = stack->data[--stackTop];
		arguments->data[0] = stack->data[--stackTop];
			/* now go send message */
		messageSelector = binaryMessages[low];
		goto findMethodFromSymbol;

            case DoPrimitive:
			/* low is argument count */
			/* next byte is primitive number */
		high = bp[bytePointer++];
		rootStack[rootTop++] = context;
		lastPrimitiveNumber = high;
		switch (high) {
			case 1:		/* object identity */
				returnedValue = stack->data[--stackTop];
				if (returnedValue == stack->data[--stackTop])
					returnedValue = trueObject;
				else
					returnedValue = falseObject;
				break;

			case 2:		/* object class */
				returnedValue = stack->data[--stackTop]->class;
				break;

			case 3:	/* print a single character */
				low = integerValue(stack->data[--stackTop]);
				putchar(low); fflush(stdout);
				returnedValue = nilObject;
				break;

			case 4:	/* object size */
				returnedValue = stack->data[--stackTop];
				high = returnedValue->size >> 2;
				returnedValue = newInteger(high);
				break;

			case 5:		/* object at put */
				low = integerValue(stack->data[--stackTop]);
				returnedValue = stack->data[--stackTop];
				returnedValue->data[low-1] 
					= stack->data[--stackTop];
				break;

			case 6:		/* new process execute */
				low = execute(stack->data[--stackTop]);
				/* got to load everything else */
				returnedValue = newInteger(low);
				break;

			case 7: 	/* new object allocation */
				low = integerValue(stack->data[--stackTop]);
				rootStack[rootTop++] = stack->data[--stackTop];
				returnedValue = gcalloc(low);
				returnedValue->class = rootStack[--rootTop];
				while (low > 0)
					returnedValue->data[--low] = nilObject;
				break;

			case 8:	/* block invocation */
				/* low holds number of arguments */
				returnedValue = stack->data[--stackTop];
					/* put arguments in place */
				high = integerValue(
					returnedValue->data[
						argumentLocationInBlock]);
				temporaries = 
					returnedValue->data[temporariesInBlock];
				low -= 2;
				while (low >= 0) {
					temporaries->data[high + low] =
						stack->data[--stackTop];
					low--;
					}
				returnedValue->data[previousContextInBlock] =
					context->data[previousContextInContext];
				context = returnedValue;
				arguments = instanceVariables =
					literals = 0;
				stack = context->data[stackInContext];
				stackTop = 0;
				method = context->data[methodInBlock];
				bp = bytePtr(method->data[byteCodesInMethod]);
				bytePointer = integerValue(
					context->data[bytePointerInBlock]);
				--rootTop;
				goto endPrimitive;

			case 9:		/* read char from input */
				low = getchar();
				if (low == EOF)
					returnedValue = nilObject;
				else returnedValue = newInteger(low);
				break;

			case 10: 	/* small integer addition */
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				high += low;
				returnedValue = newInteger(high);
				break;
				
			case 11: 	/* small integer division */
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				high /= low;
				returnedValue = newInteger(high);
				break;
				
			case 12:	/* small integer remainder */ 
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				high %= low;
				returnedValue = newInteger(high);
				break;
				
			case 13:	/* small integer less than */ 
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				if (high < low)
					returnedValue = trueObject;
				else
					returnedValue = falseObject;
				break;

			case 14:	/* small integer equality */ 
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				if (high == low)
					returnedValue = trueObject;
				else
					returnedValue = falseObject;
				break;

			case 15:	/* small integer multiplication */ 
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				high *= low;
				returnedValue = newInteger(high);
				break;

			case 16:	/* small integer subtraction */ 
				low = integerValue(stack->data[--stackTop]);
				high = integerValue(stack->data[--stackTop]);
				if (low < high) high -= low;
				else high = 0;
				returnedValue = newInteger(high);
				break;

			case 18: 	/* turn on debugging */
				debugging = 1;
				returnedValue = nilObject;
				break;

			case 19:	/* error trap -- halt execution */
				--rootTop; /* pop context */
				aProcess = rootStack[--rootTop];
				aProcess->data[contextInProcess] = context;
				return 2;

			case 20:	/* byteArray allocation */
				low = integerValue(stack->data[--stackTop]);
				rootStack[rootTop++] = stack->data[--stackTop];
				returnedValue = gcialloc(low);
				returnedValue->class = rootStack[--rootTop];
				break;

			case 21:	/* string at */
				low = integerValue(stack->data[--stackTop]);
				returnedValue = stack->data[--stackTop];
				low = bytePtr(returnedValue)[low-1];
				returnedValue = newInteger(low);
				break;

			case 22:	/* string at put */
				low = integerValue(stack->data[--stackTop]);
				returnedValue = stack->data[--stackTop];
				bytePtr(returnedValue)[low-1] =
					integerValue(stack->data[--stackTop]);
				break;

			case 23:	/* string clone */
				rootStack[rootTop++] = stack->data[--stackTop];
				rootStack[rootTop++] = returnedValue 
					= stack->data[--stackTop];
				low = returnedValue->size >> 2;
				returnedValue = gcialloc(low);
				messageSelector = rootStack[--rootTop];
				while (low-- > 0)
					bytePtr(returnedValue)[low] =
						bytePtr(messageSelector)[low];
				returnedValue->class = rootStack[--rootTop];
				break;

			case 24:	/* array at */
				low = integerValue(stack->data[--stackTop]);
				returnedValue = stack->data[--stackTop];
				returnedValue = returnedValue->data[low-1];
				break;

			default:
					/* pop arguments, try primitive */
				rootStack[rootTop++] = stack;
				arguments = gcalloc(low);
				stack = rootStack[--rootTop];
				while (low > 0)
					arguments->data[--low] = 
						stack->data[--stackTop];
				returnedValue = primitive(high, arguments);
				arguments = 0;
				break;
			}
		if (context != rootStack[--rootTop]) { /* gc has occurred */
			context = rootStack[rootTop];
			method = context->data[methodInContext];
			stack = context->data[stackInContext];
			bp = bytePtr(method->data[byteCodesInMethod]);
			arguments = temporaries = literals = 
				instanceVariables = 0;
			}
		stack->data[stackTop++] = returnedValue;
		endPrimitive:
		break;

            case DoSpecial:
                switch(low) {
                    case SelfReturn:
			if (! arguments) 
				arguments = context->data[argumentsInContext];
			returnedValue = arguments->data[receiverInArguments];
			goto doReturn;

                    case StackReturn:
			returnedValue = stack->data[--stackTop];

		    doReturn:
			context = context->data[previousContextInContext];
		    doReturn2:
			if ((context == 0) || (context == nilObject)) {
				aProcess = rootStack[--rootTop];
				aProcess->data[contextInProcess] = context;
				aProcess->data[resultInProcess] = returnedValue;
				return 4;
				}
			arguments = instanceVariables = literals 
				= temporaries = 0;
			stack = context->data[stackInContext];
			stackTop = 
				integerValue(context->data[stackTopInContext]);
			stack->data[stackTop++] = returnedValue;
			method = context->data[methodInContext];
    			bp = bytePtr(method->data[byteCodesInMethod]);
    			bytePointer = 
				integerValue(context->data[bytePointerInContext]);
			break;

		    case BlockReturn:
			returnedValue = stack->data[--stackTop];
			context = context->data[creatingContextInBlock]
				->data[previousContextInContext];
			goto doReturn2;

                    case Duplicate:
			returnedValue = stack->data[stackTop-1];
			stack->data[stackTop++] = returnedValue;
			break;

                    case PopTop:
			stackTop--;
			break;

                    case Branch:
			bytePointer = bp[bytePointer++];
			break;

                    case BranchIfTrue:
			low = bp[bytePointer++];
			returnedValue = stack->data[--stackTop];
			if (returnedValue == trueObject)
				bytePointer = low;
			break;

                    case BranchIfFalse:
			low = bp[bytePointer++];
			returnedValue = stack->data[--stackTop];
			if (returnedValue == falseObject)
				bytePointer = low;
			break;

                    case SendToSuper:
			/* next byte has literal selector number */
			low = bp[bytePointer++];
			if (! literals) {
				literals = method->data[literalsInMethod];
				}
			messageSelector = literals->data[low];
			receiverClass = 
				method->data[classInMethod]
					->data[parentClassInClass];
			arguments = stack->data[--stackTop];
			goto checkCache;

                    default:
                        sysError("invalid doSpecial", low);
                        break;
                }
                break;

            default:
                sysError("invalid bytecode", high);
                break;
        }
    }

    return 5;
}
