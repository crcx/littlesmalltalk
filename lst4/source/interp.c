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

#include "memory.h"
#include "interp.h"
#include "globs.h"
#include <stdio.h>
#include <string.h>	/* For bzero() */

extern int debugging;
extern int cacheHit;
extern int cacheMiss;

/*
	The following are roots for the file out 
*/

struct object *nilObject, *trueObject, *falseObject,
	*SmallIntClass, *ArrayClass, *BlockClass, *ContextClass,
	*globalsObject, *initialMethod, *binaryMessages[3],
	*IntegerClass, *badMethodSym;

/*
 * Debugging
 */
#if defined(DEBUG) && defined(TRACE)
static void
indent(struct object *ctx)
{
	static int oldlev = 0;
	int lev = 0, x;

	while (ctx && (ctx != nilObject)) {
		lev += 1;
		putchar(' ');
		ctx = ctx->data[previousContextInContext];
	}

	/*
	 * This lets you use your editor's brace matching to
	 * match up opening and closing indentation levels.
	 */
	if (lev < oldlev) {
		for (x = lev; x < oldlev; ++x) {
			putchar('}');
		}
	} else if (lev > oldlev) {
		for (x = oldlev; x < lev; ++x) {
			putchar('{');
		}
	}

	oldlev = lev;
}
#define PC (bytePointer-1)
#define DBG0(msg) if (debugging) {indent(context); printf("%d: %s\n", PC, msg);}
#define DBG1(msg, arg) if (debugging) {indent(context); \
	printf("%d: %s %d\n", PC, msg, arg);}
#define DBGS(msg, cl, sel) \
	if (debugging) { \
		indent(context); \
		printf("%d: %s %s %s\n", PC, msg, cl, sel); }
#else
#define DBG0(msg)
#define DBG1(msg, arg)
#define DBGS(msg, cl, sel)
#endif

/*
	method lookup routine, used when cache miss occurs
*/

static int symbolcomp(struct object * left, struct object * right)
{
	int leftsize = SIZE(left);
	int rightsize = SIZE(right);
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

static struct object *
lookupMethod(struct object *selector, struct object *class)
{
	struct object *dict, *keys, *vals, *val;
	uint low, high, mid;

	/*
	 * Scan upward through the class hierarchy
	 */
	for ( ; class != nilObject; class = class->data[parentClassInClass]) {
		/*
		 * Consider the Dictionary of methods for this Class
		 */
		dict = class->data[methodsInClass];
		keys = dict->data[0];
		low = 0; high = SIZE(keys);

		/*
		 * Do a binary search through its keys, which are
		 * Symbol's.
		 */
		while (low < high) {
			mid = (low + high) / 2;
			val = keys->data[mid];

			/* 
			 * If we find the selector, return the
			 * method object.
			 */
			if (val == selector) {
				vals = dict->data[1];
				return(vals->data[mid]);
			}

			/*
			 * Otherwise continue the binary search
			 */
			if (symbolcomp(selector, val) < 0) {
				high = mid;
			} else {
				low = mid+1;
			}
		}
	}

	/*
	 * Sorry, couldn't find a method
	 */
	return(NULL);
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

/* flush dynamic methods when GC occurs */
void
flushCache(void)
{
	int i;

	for (i = 0; i < cacheSize; i++) {
		cache[i].name = 0;	/* force refill */
	}
}

/*
 * newLInteger()
 *	Create new Integer (64-bit)
 */
static struct object *
newLInteger(long long val)
{
	struct object *res;

	res = gcialloc(sizeof(long long));
	res->class = IntegerClass;
	*(long long *)bytePtr(res) = val;
	return(res);
}

/*
 * do_Integer()
 *	Implement the appropriate 64-bit Integer operation
 *
 * Returns NULL on error, otherwise the resulting Integer or
 * Boolean (for comparisons) object.
 */
static struct object *
do_Integer(int op, struct object *low, struct object *high)
{
	long long l, h;

	l = *(long long *)bytePtr(low);
	h = *(long long *)bytePtr(high);
	switch (op) {
	case 25:	/* Integer division */
		if (h == 0LL) {
			return(NULL);
		}
		return(newLInteger(l/h));

	case 26:	/* Integer remainder */
		if (h == 0LL) {
			return(NULL);
		}
		return(newLInteger(l%h));

	case 27:	/* Integer addition */
		return(newLInteger(l+h));

	case 28:	/* Integer multiplication */
		return(newLInteger(l*h));

	case 29:	/* Integer subtraction */
		return(newLInteger(l-h));

	case 30:	/* Integer less than */
		return((l < h) ? trueObject : falseObject);

	case 31:	/* Integer equality */
		return((l == h) ? trueObject : falseObject);

	default:
		sysError("Invalid op table jump", op);
	}
	return(NULL);
}

/*
 * bulkReplace()
 *	Implement replaceFrom:to:with:startingAt: as a primitive
 *
 * Return 1 if we couldn't do it, 0 on success.  This routine has
 * distinct code paths for plain old byte type arrays, and for
 * arrays of object pointers; the latter must handle the special
 * case of static pointers.  It looks hairy (and it is), but it's
 * still much faster than executing the block move in Smalltalk
 * VM opcodes.
 */
static int
bulkReplace(struct object *dest, struct object *start,
	struct object *stop, struct object *src,
	struct object *repStart)
{
	int irepStart, istart, istop, count;

	/*
	 * We only handle simple 31-bit integer indices.  Map the
	 * values onto 0-based C array type values.
	 */
	if (!IS_SMALLINT(repStart) || !IS_SMALLINT(start) ||
			!IS_SMALLINT(stop)) {
		return(1);
	}
	irepStart = integerValue(repStart)-1;
	istart = integerValue(start)-1;
	istop = integerValue(stop)-1;
	count = (istop-istart) + 1;

	/*
	 * Defend against goofy negative indices.
	 */
	if ((irepStart < 0) || (istart < 0) || (istop < 0) ||
			(count < 1)) {
		return(1);
	}

	/*
	 * Range check
	 */
	if ((SIZE(dest) < istop) ||
			(SIZE(src) < (irepStart + count))) {
		return(1);
	}

	/*
	 * If both source and dest are binary, do a bcopy()
	 */
	if ((src->size & FLAG_BIN) && (dest->size & FLAG_BIN)) {
		/*
		 * Do it.
		 */
		bcopy(bytePtr(src) + irepStart, bytePtr(dest) + istart,
			count);
		return(0);
	}

	/*
	 * If not both regular storage, fail
	 */
	if ((src->size & FLAG_BIN) || (dest->size & FLAG_BIN)) {
		return(1);
	}

	/*
	 * If we're fiddling pointers between static and dynamic memory,
	 * let the VM-based implementation deal with it.
	 */
	if (isDynamicMemory(src) != isDynamicMemory(dest)) {
		return(1);
	}

	/*
	 * Copy object pointer fields
	 */
	bcopy(&src->data[irepStart], &dest->data[istart],
		BytesPerWord * count);
	return(0);
}

/* Code locations are extracted as VAL's */
#define VAL (bp[bytePointer] | (bp[bytePointer+1] << 8))
#define VALSIZE 2

int
execute(struct object *aProcess, int ticks)
{   
    int low, high, x, stackTop, bytePointer;
    struct object *context, *method, *arguments, *temporaries,
	    *instanceVariables, *literals, *stack,
	    *returnedValue = nilObject, *messageSelector,
	    *receiverClass, *op;
    unsigned char *bp;
    long long l;

    /* push process, so as to save it */
    rootStack[rootTop++] = aProcess;

    /* get current context information */
    context = aProcess->data[contextInProcess];

    method = context->data[methodInContext];

	/* load byte pointer */
    bp = (unsigned char *)bytePtr(method->data[byteCodesInMethod]);
    bytePointer = integerValue(context->data[bytePointerInContext]);

	/* load stack */
    stack = context->data[stackInContext];
    stackTop = integerValue(context->data[stackTopInContext]);

	/* everything else can wait, as maybe won't be needed at all */
    temporaries = instanceVariables = arguments = literals = 0;


    for (;;) {
	/*
	 * If we're running against a CPU tick count, stop execution
	 * when we expire the given number of ticks.
	 */
	if (ticks && (--ticks == 0)) {
		aProcess = rootStack[--rootTop];
		aProcess->data[contextInProcess] = context;
		aProcess->data[resultInProcess] = returnedValue;
		context->data[bytePointerInContext] = newInteger(bytePointer);
		context->data[stackTopInContext] = newInteger(stackTop);
		return(ReturnTimeExpired);
	}

	/* Otherwise decode the instruction */
        low = (high = bp[bytePointer++] ) & 0x0F;
        high >>= 4;
        if (high == Extended) {
            high = low;
            low = bp[bytePointer++] ;
        }

	/* And dispatch */
        switch (high) {

	case PushInstance:
	    DBG1("PushInstance", low);
	    if (! arguments) 
		    arguments = context->data[argumentsInContext];
	    if (! instanceVariables)
		    instanceVariables = 
			    arguments->data[receiverInArguments];
	    stack->data[stackTop++] = instanceVariables->data[low];
	    break;

	case PushArgument:
	    DBG1("PushArgument", low);
	    if (! arguments) 
		    arguments = context->data[argumentsInContext];
	    stack->data[stackTop++] = arguments->data[low];
	    break;

	case PushTemporary:
	    DBG1("PushTemporary", low);
	    if (! temporaries)
		    temporaries = context->data[temporariesInContext];
	    stack->data[stackTop++] = temporaries->data[low];
	    break;

	case PushLiteral:
	    DBG1("PushLiteral", low);
	    if (! literals) 
		    literals = method->data[literalsInMethod];
	    stack->data[stackTop++] = literals->data[low];
	    break;

	case PushConstant:
	    DBG1("PushConstant", low);
	    switch(low) {
		    case 0: case 1: case 2: case 3: case 4: 
		    case 5: case 6: case 7: case 8: case 9:
			    stack->data[stackTop++] = newInteger(low);
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
	    DBG0("PushBlock");
		    /* create a block object */
		    /* low is arg location */
		    /* next byte is goto value */
	    high = VAL;
	    bytePointer += VALSIZE;
	    rootStack[rootTop++] = context;
	    op = gcalloc(x = integerValue(method->data[stackSizeInMethod]));
	    op->class = ArrayClass;
	    rootStack[rootTop++] = op;
	    bzero(bytePtr(op), x * BytesPerWord);
	    returnedValue = gcalloc(blockSize);
	    returnedValue->class = BlockClass;
	    returnedValue->data[bytePointerInContext] =
	    returnedValue->data[stackTopInBlock] = 
	    returnedValue->data[previousContextInBlock] = NULL;
	    returnedValue->data[bytePointerInBlock] =
		    newInteger(bytePointer);
	    returnedValue->data[argumentLocationInBlock] = newInteger(low);
	    returnedValue->data[stackInBlock] = rootStack[--rootTop];
	    context = rootStack[--rootTop];
	    if (CLASS(context) == BlockClass) {
		    returnedValue->data[creatingContextInBlock] =
			    context->data[creatingContextInBlock];
	    } else {
		    returnedValue->data[creatingContextInBlock] = context;
	    }
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
	    DBG1("AssignInstance", low);
	    if (!arguments)  {
		    arguments = context->data[argumentsInContext];
	    }
	    if (!instanceVariables) {
		    instanceVariables = 
			    arguments->data[receiverInArguments];
			    /* don't pop stack, leave result there */
	    }
	    instanceVariables->data[low] = stack->data[stackTop-1];

	    /*
	     * If changing a static area, need to add to roots
	     */
	    if (!isDynamicMemory(instanceVariables) 
			    && isDynamicMemory(stack->data[stackTop-1])) {
		    addStaticRoot(&instanceVariables->data[low]);
	    }
	    break;

	case AssignTemporary:
	    DBG1("AssignTemporary", low);
	    if (! temporaries)
		    temporaries = context->data[temporariesInContext];
	    temporaries->data[low] = stack->data[stackTop-1];
	    break;

	case MarkArguments:
	    DBG1("MarkArguments", low);
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
	    receiverClass = CLASS(arguments->data[receiverInArguments]);
	    DBGS("SendMessage",
		    bytePtr(receiverClass->data[nameInClass]),
		    bytePtr(messageSelector));
checkCache:
	    low = (((UINT_PTR) messageSelector) +
		    ((UINT_PTR) receiverClass)) % cacheSize;
	    if ((cache[low].name == messageSelector) &&
		(cache[low].class == receiverClass)) {
		    method = cache[low].method;
		    cacheHit++;
	    } else {
		    cacheMiss++;
		    method = lookupMethod(messageSelector, receiverClass);
		    if (!method) {
			    if (messageSelector == badMethodSym) {
				    sysError("doesNotUnderstand: missing", 0);
			    }
			    rootStack[rootTop++] = arguments;
			    rootStack[rootTop++] = messageSelector;
			    rootStack[rootTop++] = context;
			    op = gcalloc(2);
			    op->class = ArrayClass;
			    context = rootStack[--rootTop];
			    messageSelector = rootStack[--rootTop];
			    arguments = rootStack[--rootTop];

			    op->data[receiverInArguments] =
				    arguments->data[receiverInArguments];
			    op->data[1] = messageSelector;
			    arguments = op;
			    messageSelector = badMethodSym;
			    goto findMethodFromSymbol;
		    }
		    cache[low].name = messageSelector;
		    cache[low].class = receiverClass;
		    cache[low].method = method;
	    }

	    /* see if we can optimize tail call */
	    if (bp[bytePointer] == (DoSpecial * 16 + StackReturn)) {
		    high = 1;
	    } else if (bp[bytePointer] ==
			    (DoSpecial * 16 + BlockReturn)) {
		    high = 2;
	    } else {
		    high = 0;
	    }

	    /* build temporaries for new context */
	    rootStack[rootTop++] = arguments;
	    rootStack[rootTop++] = method;
	    rootStack[rootTop++] = context;
	    low = integerValue(method->data[temporarySizeInMethod]);
	    op = gcalloc(x = integerValue(method->data[stackSizeInMethod]));
	    op->class = ArrayClass;
	    rootStack[rootTop++] = op;
	    bzero(bytePtr(op), x * BytesPerWord);
	    if (low > 0) {
		    int i;

		    temporaries = gcalloc(low);
		    temporaries->class = ArrayClass;
		    for (i = 0; i < low; i++) {
			    temporaries->data[i] = nilObject;
		    }
		    rootStack[rootTop++] = temporaries; /* temporaries */
	    } else {
		    rootStack[rootTop++] = NULL;	/* why bother */
	    }
	    context = rootStack[rootTop-3];
	    context->data[stackTopInContext] = newInteger(stackTop);
	    context->data[bytePointerInContext] = newInteger(bytePointer);

	    /* now go off and build the new context */
	    context = gcalloc(contextSize);
	    context->class = ContextClass;
	    temporaries = context->data[temporariesInContext] 
		    = rootStack[--rootTop];
	    stack = context->data[stackInContext] = rootStack[--rootTop];
	    stack->class = ArrayClass;
	    context->data[stackTopInContext] = newInteger(0);
	    stackTop = 0;
	    context->data[previousContextInContext] = rootStack[--rootTop];
	    if (high == 1) {
		    context->data[previousContextInContext] =
		    context->data[previousContextInContext]->
			    data[previousContextInContext];
	    } else if (high == 2) {
		    context->data[previousContextInContext] =
		    context->data[previousContextInContext]->
			    data[creatingContextInBlock]->
			    data[previousContextInContext];
	    }
	    method = context->data[methodInContext] = rootStack[--rootTop];
	    arguments = context->data[argumentsInContext] 
		    = rootStack[--rootTop];
	    instanceVariables = literals = 0;
	    context->data[bytePointerInContext] = newInteger(0);
	    bytePointer = 0;
	    bp = (unsigned char *) method->data[byteCodesInMethod]->data;
		    /* now go execute new method */
	    break;

	case SendUnary:	/* optimize certain unary messages */
	    DBG1("SendUnary", low);
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
	    DBG1("SendBinary", low);
	    if (IS_SMALLINT(stack->data[stackTop-1])
		     && IS_SMALLINT(stack->data[stackTop-2])) {
		    int i, j;
		    j = integerValue(stack->data[--stackTop]);
		    i = integerValue(stack->data[--stackTop]);
			    /* can only do operations that won't */
			    /* trigger garbage collection */
		    switch(low) {
		    case 0:	/* < */
			    if (i < j) {
				    returnedValue = trueObject;
			    } else {
				    returnedValue = falseObject;
			    }
			    break;
		    case 1:	/* <= */
			    if (i <= j) {
				    returnedValue = trueObject;
			    } else {
				    returnedValue = falseObject;
			    }
			    break;
		    case 2:	/* + */
			    /* no possibility of garbage col */
			    returnedValue = newInteger(i+j);
			    break;
		    }
		    stack->data[stackTop++] = returnedValue;
		    break;
	    }

	    /* not integers, do as message send */
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

/*
* Pull two integers of the required class as arguments from stack
*/
#define GET_HIGH_LOW() \
    op = stack->data[--stackTop]; \
    if (!IS_SMALLINT(op)) { \
	    stackTop -= 1; \
	    goto failPrimitive; \
    } \
    low = integerValue(op); \
    op = stack->data[--stackTop]; \
    if (!IS_SMALLINT(op)) { \
	    goto failPrimitive; \
    } \
    high = integerValue(op);

	case DoPrimitive:
		    /* low is argument count */
		    /* next byte is primitive number */
	    high = bp[bytePointer++];
	    DBG1("DoPrimitive", high);
	    rootStack[rootTop++] = context;
	    switch (high) {
	    case 1:		/* object identity */
		    returnedValue = stack->data[--stackTop];
		    if (returnedValue == stack->data[--stackTop]) {
			    returnedValue = trueObject;
		    } else {
			    returnedValue = falseObject;
		    }
		    break;

	    case 2:		/* object class */
		    returnedValue = stack->data[--stackTop];
		    returnedValue = CLASS(returnedValue);
		    break;

	    case 3:	/* print a single character */
		    low = integerValue(stack->data[--stackTop]);
		    putchar(low); /* fflush(stdout); */
		    returnedValue = nilObject;
		    break;

	    case 4:	/* object size */
		    returnedValue = stack->data[--stackTop];
		    if (IS_SMALLINT(returnedValue)) {
			    high  = 0;
		    } else {
			    high = SIZE(returnedValue);
		    }
		    returnedValue = newInteger(high);
		    break;

	    case 5:		/* Array at put */
		    op = stack->data[--stackTop];
		    if (!IS_SMALLINT(op)) {
			    stackTop -= 2;
			    goto failPrimitive;
		    }
		    low = integerValue(op)-1;
		    returnedValue = stack->data[--stackTop];
		    /* Bounds check */
		    if ((low < 0) ||
		     (low >= SIZE(returnedValue))) {
			    stackTop -= 1;
			    goto failPrimitive;
		    }
		    returnedValue->data[low] 
			    = stack->data[--stackTop];
		    /*
		     * If putting a non-static pointer
		     * into an array in static memory,
		     * register it for GC.
		     */
		    if (!isDynamicMemory(returnedValue) 
				    && isDynamicMemory(
				     stack->data[stackTop])) {
			    addStaticRoot(
			     &returnedValue->data[low]);
		    }
		    break;

	    case 6:		/* new process execute */
		    low = integerValue(stack->data[--stackTop]);
		    op = stack->data[--stackTop];
		    low = execute(op, low);

		    /* return value as a SmallInt */
		    returnedValue = newInteger(low);
		    break;

	    case 7: 	/* new object allocation */
		    low = integerValue(stack->data[--stackTop]);
		    rootStack[rootTop++] = stack->data[--stackTop];
		    returnedValue = gcalloc(low);
		    returnedValue->class = rootStack[--rootTop];
		    while (low > 0) {
			    returnedValue->data[--low] = nilObject;
		    }
		    break;

	    case 8:	/* block invocation */
		    /* low holds number of arguments */
		    returnedValue = stack->data[--stackTop];
			    /* put arguments in place */
		    high = integerValue(returnedValue->data[
				    argumentLocationInBlock]);
		    temporaries = returnedValue->data[temporariesInBlock];
		    low -= 2;
		    x = (temporaries ?
			    (SIZE(temporaries) - high) : 0);
		    if (low >= x) {
			    stackTop -= (low+1);
			    goto failPrimitive;
		    }
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
		    if (low == EOF) {
			    returnedValue = nilObject;
		    } else {
			    returnedValue = newInteger(low);
		    }
		    break;

	    case 10: 	/* small integer addition */
		    GET_HIGH_LOW();
		    x = high + low;
		    if (((high > 0) && (low > 0) && (x < high)) ||
		     ((high < 0) && (low < 0) && (x > high))) {
			    /* overflow... do it with 64 bits */
			    returnedValue = newLInteger(
				    (long long)high + (long long)low);
		    } else {
			    if (!FITS_SMALLINT(x)) {
				    returnedValue = newLInteger(x);
			    } else {
				    returnedValue = newInteger(x);
			    }
		    }
		    break;
		    
	    case 11: 	/* small integer division */
		    GET_HIGH_LOW();
		    if (low == 0) {
			    goto failPrimitive;
		    }
		    high /= low;
		    returnedValue = newInteger(high);
		    break;
		    
	    case 12:	/* small integer remainder */ 
		    GET_HIGH_LOW();
		    if (low == 0) {
			    goto failPrimitive;
		    }
		    high %= low;
		    returnedValue = newInteger(high);
		    break;
		    
	    case 13:	/* small integer less than */ 
		    GET_HIGH_LOW();
		    if (high < low) {
			    returnedValue = trueObject;
		    } else {
			    returnedValue = falseObject;
		    }
		    break;

	    case 14:	/* small integer equality */ 
		    GET_HIGH_LOW();
		    if (high == low) {
			    returnedValue = trueObject;
		    } else {
			    returnedValue = falseObject;
		    }
		    break;

	    case 15:	/* small integer multiplication */ 
		    GET_HIGH_LOW();
		    x = high*low;
		    if ((low == 0) || (x/low == high)) {
			    if (!FITS_SMALLINT(x)) {
				    returnedValue = newLInteger(x);
			    } else {
				    returnedValue = newInteger(x);
			    }
		    } else {
			    /* overflow... do it with 64 bits */
			    returnedValue = newLInteger(
				    (long long)high * (long long)low);
		    }
		    break;

	    case 16:	/* small integer subtraction */ 
		    GET_HIGH_LOW();
		    x = high - low;
		    if ((low > 0) && (high < 0) && (x > high)) {
			    returnedValue = newLInteger(
				    (long long)high - (long long)low);
		    } else {
			    if (!FITS_SMALLINT(x)) {
				    returnedValue = newLInteger(x);
			    } else {
				    returnedValue = newInteger(x);
			    }
		    }
		    break;

	    case 18: 	/* turn on debugging */
		    debugging = 1;
		    returnedValue = nilObject;
		    break;

	    case 19:	/* error trap -- halt execution */
		    --rootTop; /* pop context */
		    aProcess = rootStack[--rootTop];
		    aProcess->data[contextInProcess] = context;
		    return(ReturnError);

	    case 20:	/* byteArray allocation */
		    low = integerValue(stack->data[--stackTop]);
		    rootStack[rootTop++] = stack->data[--stackTop];
		    returnedValue = gcialloc(low);
		    returnedValue->class = rootStack[--rootTop];
		    bzero(bytePtr(returnedValue), low);
		    break;

	    case 21:	/* string at */
		    low = integerValue(stack->data[--stackTop])-1;
		    returnedValue = stack->data[--stackTop];
		    if ((low < 0) ||
		     (low >= SIZE(returnedValue))) {
			    goto failPrimitive;
		    }
		    low = bytePtr(returnedValue)[low];
		    returnedValue = newInteger(low);
		    break;

	    case 22:	/* string at put */
		    low = integerValue(stack->data[--stackTop])-1;
		    returnedValue = stack->data[--stackTop];
		    if ((low < 0) ||
		     (low >= SIZE(returnedValue))) {
			    stackTop -= 1;
			    goto failPrimitive;
		    }
		    bytePtr(returnedValue)[low] =
			    integerValue(stack->data[--stackTop]);
		    break;

	    case 23:	/* string clone */
		    rootStack[rootTop++] = stack->data[--stackTop];
		    rootStack[rootTop++] = returnedValue 
			    = stack->data[--stackTop];
		    low = SIZE(returnedValue);
		    returnedValue = gcialloc(low);
		    messageSelector = rootStack[--rootTop];
		    while (low-- > 0)
			    bytePtr(returnedValue)[low] =
				    bytePtr(messageSelector)[low];
		    returnedValue->class = rootStack[--rootTop];
		    break;

	    case 24:	/* array at */
		    low = integerValue(stack->data[--stackTop])-1;
		    returnedValue = stack->data[--stackTop];
		    if ((low < 0) ||
		     (low >= SIZE(returnedValue))) {
			    goto failPrimitive;
		    }
		    returnedValue = returnedValue->data[low];
		    break;
#undef GET_HIGH_LOW

	    case 25:	/* Integer division */
	    case 26:	/* Integer remainder */
	    case 27:	/* Integer addition */
	    case 28:	/* Integer multiplication */
	    case 29:	/* Integer subtraction */
	    case 30:	/* Integer less than */
	    case 31:	/* Integer equality */
		    op = stack->data[--stackTop];
		    if (CLASS(op) != IntegerClass) {
			    stackTop -= 1;
			    goto failPrimitive;
		    }
		    returnedValue = stack->data[--stackTop];
		    if (CLASS(returnedValue) != IntegerClass) {
			    goto failPrimitive;
		    }
		    returnedValue = do_Integer(high,
			    returnedValue, op);
		    if (returnedValue == NULL) {
			    goto failPrimitive;
		    }
		    break;

	    case 32:	/* Integer allocation */
		    op = stack->data[--stackTop];
		    if (!IS_SMALLINT(op)) {
			    goto failPrimitive;
		    }
		    returnedValue = newLInteger(integerValue(op));
		    break;

	    case 33:	/* Low order of Integer -> SmallInt */
		    op = stack->data[--stackTop];
		    l = *(long long *)bytePtr(op);
		    x = l;
		    if (!FITS_SMALLINT(x)) {
			    goto failPrimitive;
		    }
		    returnedValue = newInteger(x);
		    break;

	    case 34:	/* Flush method cache */
	    	    flushCache();
		    break;

	    case 35:	/* Bulk object exchange */
	    	    op = stack->data[--stackTop];
		    if (op->class != ArrayClass) {
		    	goto failPrimitive;
		    }
		    returnedValue = stack->data[--stackTop];
		    if (returnedValue->class != ArrayClass) {
		    	goto failPrimitive;
		    }
		    if (SIZE(op) != SIZE(returnedValue)) {
		    	goto failPrimitive;
		    }
		    exchangeObjects(op, returnedValue, SIZE(op));
		    break;

	    case 36:	/* bitOr: */
	    	op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			--stackTop;
			goto failPrimitive;
		}
		high = integerValue(op);
		op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			goto failPrimitive;
		}
		returnedValue = newInteger(integerValue(op) | high);
		break;

	    case 37:	/* bitAnd: */
	    	op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			--stackTop;
			goto failPrimitive;
		}
		high = integerValue(op);
		op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			goto failPrimitive;
		}
		returnedValue = newInteger(integerValue(op) & high);
		break;

	    case 38:	/* replaceFrom:... */
		returnedValue = stack->data[--stackTop];
                stackTop -= 4;
	    	if (bulkReplace(returnedValue,
			stack->data[stackTop + 4],
			stack->data[stackTop + 3],
			stack->data[stackTop + 2],
			stack->data[stackTop + 1])) {
		    goto failPrimitive;
		}
		break;

	    case 39:	/* bitShift: */
	    	op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			--stackTop;
			goto failPrimitive;
		}
		low = integerValue(op);
		op = stack->data[--stackTop];
		if (!IS_SMALLINT(op)) {
			goto failPrimitive;
		}
		high = integerValue(op);
		if (low < 0) {
			/* Negative means shift right */
			low = high >> (-low);
		} else {
			/* Shift left--catch overflow */
			low = high << low;
			if (high > low) {
				goto failPrimitive;
			}
		}
		returnedValue = newInteger(low);
		break;

	    case 40:	/* Truncate Integer -> SmallInt */
	    	op = stack->data[--stackTop];
		l = *(long long *)bytePtr(op);
		x = l;
		returnedValue = newInteger(x);
		break;

	    default:
			    /* pop arguments, try primitive */
		    rootStack[rootTop++] = stack;
		    arguments = gcalloc(low);
		    arguments->class = ArrayClass;
		    stack = rootStack[--rootTop];
		    while (low > 0) {
			    arguments->data[--low] = 
				    stack->data[--stackTop];
		    }
		    {
			int failed;

			returnedValue =
				primitive(high, arguments, &failed);
		    	if (failed) {
				goto failPrimitive;
			}
		    }
		    arguments = 0;
		    break;
	    }

	    /*
	     * Restore our context pointer and
	     * force a stack return due to successful
	     * primitive.
	     */
	    context = rootStack[--rootTop];
	    goto doReturn;

failPrimitive:
	    /*
	     * Since we're continuing execution from a failed
	     * primitive, re-fetch context if a GC had occurred
	     * during the failed execution.  Supply a return value
	     * for the failed primitive.
	     */
	    returnedValue = nilObject;
	    if (context != rootStack[--rootTop]) {
		    context = rootStack[rootTop];
		    method = context->data[methodInContext];
		    stack = context->data[stackInContext];
		    bp = bytePtr(method->data[byteCodesInMethod]);
		    arguments = temporaries = literals = 
			    instanceVariables = 0;
	    }
	    stack->data[stackTop++] = nilObject;

endPrimitive:
	    /*
	     * Done with primitive, continue execution loop
	     */
	    break;

	case DoSpecial:
	    DBG1("DoSpecial", low);
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
			    return(ReturnReturned);
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
		    low = VAL;
		    bytePointer = low;
		    break;

		case BranchIfTrue:
		    low = VAL;
		    returnedValue = stack->data[--stackTop];
		    if (returnedValue == trueObject) {
			    bytePointer = low;
		    } else {
			    bytePointer += VALSIZE;
		    }
		    break;

		case BranchIfFalse:
		    low = VAL;
		    returnedValue = stack->data[--stackTop];
		    if (returnedValue == falseObject) {
			    bytePointer = low;
		    } else {
			    bytePointer += VALSIZE;
		    }
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

		case Breakpoint:
		    /* Back up on top of the breaking location */
		    bytePointer -= 1;

		    /* Return to our master process */
		    aProcess = rootStack[--rootTop];
		    aProcess->data[contextInProcess] = context;
		    aProcess->data[resultInProcess] = returnedValue;
		    context->data[bytePointerInContext] =
			newInteger(bytePointer);
		    context->data[stackTopInContext] = newInteger(stackTop);
		    return(ReturnBreak);

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
}
