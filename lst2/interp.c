/*
	Little Smalltalk version 2
	Written by Tim Budd, Oregon State University, July 1987

	bytecode interpreter module

	execute bytecodes for a given method until one of six events occur
	1. A message must be sent to another object
	2. A message must be sent to super
	3. A return from a method occurs
	4. An explicit return from a block occurs (backs up the process chain)
	5. A block must be created
	6. A block must begin execution

	the global variable finalTask indicates which of the six events is to
	be performed.  Various other global variables (described in process.h)
	give other information to be used in performing the called for task.

	Note that the interpreter is called as part of the
	main instruction sequence (single process) and (via a primitive call)
	as part of the multi-process scheduler loop (class Scheduler, Process,
	et al)
*/

# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"
# include "process.h"
# include "interp.h"

extern object unSyms[], binSyms[], keySyms[];
extern boolean primitive();

# define nextByte byteToInt(bytecodes[byteCounter++])
# define ipush(x) incr(stack[stacktop++] = x)
/* note that ipop leaves a ref count on the popped object */
# define ipop(x)  x=stack[--stacktop]; stack[stacktop]=nilobj

execute(method, byteCounter, stack, stacktop, arguments, temporaries)
object method, *stack, *arguments, *temporaries;
register int byteCounter;
register int stacktop;
{
	int i, low, high;
	object receiver, *instance, *literals;
	object newobj;
	byte  *bytecodes;
	boolean done;
	double f;

	/* do initialization */
	receiver = arguments[0];
	if (isInteger(receiver))
		instance = (object *) 0;
	else
		instance = memoryPtr(receiver);
	bytecodes = bytePtr(basicAt(method, bytecodesInMethod));
	literals = memoryPtr(basicAt(method, literalsInMethod));
	done = false;


	while( ! done ) {
		low = (high = nextByte) & 0x0F;
		high >>= 4;
		if (high == 0) {
			high = low;
			low = nextByte;
			}
/*fprintf(stderr,"executing %d %d\n", high, low);*/

		switch(high) {
			case PushInstance:
				ipush(instance[low]);
				break;

			case PushArgument:
				ipush(arguments[low]);
				break;

			case PushTemporary:
				ipush(temporaries[low]);
				break;

			case PushLiteral:
				ipush(literals[low]);
				break;

			case PushConstant:
				if (low == 3)
					low = -1;
				if (low < 3) {
					ipush(newInteger(low));
					}
				else
					switch(low) {
						case 4: 
							ipush(nilobj);
							break;

						case 5:
							ipush(trueobj);
							break;

						case 6:
							ipush(falseobj);
							break;

						case 7:
							ipush(smallobj);
							break;

						default:
					sysError("not done yet","pushConstant");
						}
				break;

			case PushGlobal:
				newobj = nameTableLookup(globalNames, 
					literals[low]);
				if (newobj == nilobj) {
					/* send message instead */
					ipush(smallobj);
					ipush(literals[low]);
					argumentsOnStack = stacktop - 2;
					messageToSend = 
						newSymbol("cantFindGlobal:");
					finalTask = sendMessageTask;
					done = true;
					}
				else
					ipush(newobj);
				break;
	
			case PopInstance:
				decr(instance[low]);
				/* we transfer reference count to instance */
				ipop(instance[low]);
				break;

			case PopTemporary:
				decr(temporaries[low]);
				/* we transfer reference count to temporaries */
				ipop(temporaries[low]);
				break;

			case SendMessage:
				argumentsOnStack = stacktop - (low + 1);
				messageToSend = literals[nextByte];
				finalTask = sendMessageTask;
				done = true;
				break;

			case SendUnary:
				/* we optimize a couple common messages */
				if (low == 0) {		/* isNil */
					ipop(newobj);
					if (newobj == nilobj) {
						ipush(trueobj);
						}
					else {
						decr(newobj);
						ipush(falseobj);
						}
					}
				else if (low == 1) {	/* notNil */
					ipop(newobj);
					if (newobj == nilobj) {
						ipush(falseobj);
						}
					else {
						decr(newobj);
						ipush(trueobj);
						}
					}
				else {
					argumentsOnStack = stacktop - 1;
					messageToSend = unSyms[low];
					finalTask = sendMessageTask;
					done = true;
					}
				break;

			case SendBinary:
				/* optimize arithmetic as long as no */
				/* conversions are necessary */
				if (low <= 12) {
					if (isInteger(stack[stacktop-1]) &&
				    	    isInteger(stack[stacktop-2])) {
						ipop(newobj);
						i = intValue(newobj);
						ipop(newobj);
						ignore intBinary(low, intValue(newobj), i);
						ipush(returnedObject);
						break;
						}
					if (isFloat(stack[stacktop-1]) &&
					    isFloat(stack[stacktop-2])) {
						ipop(newobj);
						f = floatValue(newobj);
						decr(newobj);
						ipop(newobj);
						ignore floatBinary(low, floatValue(newobj), f);
						decr(newobj);
						ipush(returnedObject);
						break;
						}
					}
				argumentsOnStack = stacktop - 2;
				messageToSend = binSyms[low];
				finalTask = sendMessageTask;
				done = true;
				break;

			case SendKeyword:
				argumentsOnStack = stacktop - 3;
				messageToSend = keySyms[low];
				finalTask = sendMessageTask;
				done = true;
				break;

			case DoPrimitive:
				i = nextByte;
				done = primitive(i, &stack[stacktop - low], low);
				incr(returnedObject);
				/* pop off arguments */
				for (i = low; i > 0; i--) {
					ipop(newobj);
					decr(newobj);
					}
				if (! done) {
					ipush(returnedObject);
					decr(returnedObject);
					}
				break;

			case CreateBlock:
				/* we do most of the work in making the block */
				/* leaving it to the caller to fill in */
				/* the context information */
				newobj = allocObject(blockSize);
				setClass(newobj, blockclass);
				basicAtPut(newobj, argumentCountInBlock, newInteger(low));
				i = (low > 0) ? nextByte : 0;
				basicAtPut(newobj, argumentLocationInBlock, 
					newInteger(i));
				basicAtPut(newobj, bytecountPositionInBlock,
					newInteger(byteCounter + 1));
				incr(returnedObject = newobj);
				/* avoid a subtle side effect here */
				i = nextByte;
				byteCounter = i;
				finalTask = BlockCreateTask;
				done = true;
				break;

			case DoSpecial:
				switch(low) {
					case SelfReturn:
						incr(returnedObject = receiver);
						finalTask = ReturnTask;
						done = true;
						break;

					case StackReturn:
						ipop(returnedObject);
						finalTask = ReturnTask;
						done = true;
						break;

					case BlockReturn:
						ipop(returnedObject);
						finalTask = BlockReturnTask;
						done = true;
						break;

					case Duplicate:
						ipop(newobj);
						ipush(newobj);
						ipush(newobj);
						decr(newobj);
						break;

					case PopTop:
						ipop(newobj);
						decr(newobj);
						break;

					case Branch:
						/* avoid a subtle bug here */
						i = nextByte;
						byteCounter = i;
						break;

					case BranchIfTrue:
						ipop(newobj);
						i = nextByte;
						if (newobj == trueobj) {
							++stacktop;
							byteCounter = i;
							}
						decr(newobj);
						break;

					case BranchIfFalse:
						ipop(newobj);
						i = nextByte;
						if (newobj == falseobj) {
							++stacktop;
							byteCounter = i;
							}
						decr(newobj);
						break;

					case AndBranch:
						ipop(newobj);
						i = nextByte;
						if (newobj == falseobj) {
							ipush(newobj);
							byteCounter = i;
							}
						decr(newobj);
						break;

					case OrBranch:
						ipop(newobj);
						i = nextByte;
						if (newobj == trueobj) {
							ipush(newobj);
							byteCounter = i;
							}
						decr(newobj);
						break;

					case SendToSuper:
						argumentsOnStack = stacktop -
							(nextByte + 1);
						messageToSend = 
							literals[nextByte];
						finalTask = sendSuperTask;
						done = true;
						break;

					default:
						sysError("invalid doSpecial","");
						break;
				}
				break;

			default:
				sysError("invalid bytecode","");
				break;
		}
	}

	/* when done, save stack top and bytecode counter */
	/* before we exit */

	finalStackTop = stacktop;
	finalByteCounter = byteCounter;
}

