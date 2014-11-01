/*************************************************************************
 * Little Smalltalk, Version 1
 *
 * The source code for the Little Smalltalk system may be freely copied
 * provided that the source of all files is acknowledged and that this
 * condition is copied with each file.
 *
 * The Little Smalltalk system is distributed without responsibility for
 * the performance of the program and without any guarantee of maintenance.
 *
 * All questions concerning Little Smalltalk should be addressed to:
 *
 *         Professor Tim Budd
 *         Department of Computer Science
 *         Oregon State University
 *         Corvallis, Oregon
 *         97331
 *         USA
 *
 * This copy of Little Smalltalk has been updated by Charles R. Childers,
 * http://charleschilders.com
 *************************************************************************
 * bytecode interpreter
 * timothy a. budd
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

int opcount[16], ohcount, spcount[16];
extern object *o_smalltalk;	/* value of pseudo variable smalltalk */
extern object *fnd_class();	/* used to find classes from names */

static mstruct *fr_interp = 0;	/* interpreter memory free list */
int ca_terp = 0;		/* counter for interpreter allocations */

/* cr_interpreter - create a new interpreter */
interpreter *cr_interpreter(sender, receiver, literals, bitearray, context)
interpreter *sender;
object *literals, *bitearray, *receiver, *context;
{	interpreter *new;
	class *rclass;
	int isize;

	if (fr_interp) {
		new = (interpreter *) fr_interp;
		fr_interp = fr_interp->mlink;
		}
	else {
		new = structalloc(interpreter);
		ca_terp++;
		}

	new->t_ref_count = 0;
	new->t_size = INTERPSIZE;

	new->creator = (interpreter *) 0;
	if (sender)
		sassign(new->sender, sender);
	else
		sassign(new->sender, (interpreter *) o_nil);
	sassign(new->literals, literals);
	sassign(new->bytecodes, bitearray);
	sassign(new->receiver, receiver);
	rclass = (class *) fnd_class(receiver);
	if ((! rclass) || ! is_class(rclass))
		isize = 25;
	else {
		isize = rclass->stack_max;
		}
	sassign(new->context, context);
	sassign(new->stack, new_obj((class *) 0, isize, 1));
	new->stacktop = &(new->stack)->inst_var[0];
	new->currentbyte = byte_value(new->bytecodes);
	return(new);
}

/* free_terpreter - return an unused interpreter to free list */
free_terpreter(anInterpreter)
interpreter *anInterpreter;
{
	if (! is_interpreter(anInterpreter))
		cant_happen(8);

	obj_dec((object *) anInterpreter->sender);
	obj_dec(anInterpreter->receiver);
	obj_dec(anInterpreter->bytecodes);
	obj_dec(anInterpreter->literals);
	obj_dec(anInterpreter->context);
	obj_dec(anInterpreter->stack);

	((mstruct *) anInterpreter)->mlink = fr_interp;
	fr_interp = (mstruct *) anInterpreter;
}

/* copy_arguments - copy an array of arguments into the context */
copy_arguments(anInterpreter, argLocation, argCount, argArray)
interpreter *anInterpreter;
int argLocation, argCount;
object **argArray;
{	object *context = anInterpreter->context;
	int i;

	for (i = 0; i < argCount; argLocation++, i++) {
		assign(context->inst_var[ argLocation ], argArray[i]);
		}
}

# define push(x) {assign(*(anInterpreter->stacktop), x); \
			anInterpreter->stacktop++;}

/* push_object - push a returned value on to an interpreter stack */
push_object(anInterpreter, anObject)
interpreter *anInterpreter;
object *anObject;
{
	push(anObject); /* what? no bounds checking?!? */
}

# define nextbyte(x) {x = uctoi(*anInterpreter->currentbyte);\
anInterpreter->currentbyte++;}
# define instvar(x) (anInterpreter->receiver)->inst_var[ x ]
# define tempvar(x) (anInterpreter->context)->inst_var[ x ]
# define lit(x)     (anInterpreter->literals)->inst_var[ x ]
# define popstack() (*(--anInterpreter->stacktop))
# define decstack(x) (anInterpreter->stacktop -= x)
# define skip(x)    (anInterpreter->currentbyte += x )

/* resume - resume executing bytecodes associated with an interpreter */
int resume(anInterpreter)
register interpreter *anInterpreter;
{
	int highBits;
	register int lowBits;
	object *tempobj, *receiver, *fnd_super();
	interpreter *sender;
	int i, j, numargs, arglocation;
	char *message;

	while(1) {
		nextbyte(highBits);
		lowBits = highBits % 16;
		highBits /= 16;

		switchtop:
		opcount[highBits]++;
		switch(highBits) {
			default: cant_happen(9);
				break;

			case 0:	/* two bit form */
				highBits = lowBits;
				nextbyte(lowBits);
				goto switchtop;

			case 1: /* push instance variable */
				push(instvar(lowBits));
				break;

			case 2: /* push context value */
				push(tempvar(lowBits));
				break;

			case 3: /* literals */
				push(lit(lowBits));
				break;

			case 4: /* push class */
				tempobj = lit(lowBits);
				if (! is_symbol(tempobj)) cant_happen(9);
				tempobj = primitive(FINDCLASS, 1, &tempobj);
				push(tempobj);
				break;

			case 5: /* special literals */
				if (lowBits < 10) 
					tempobj = new_int(lowBits);
				else if (lowBits == 10) 
					tempobj = new_int(-1);
				else if (lowBits == 11)
					tempobj = o_true;
				else if (lowBits == 12)
					tempobj = o_false;
				else if (lowBits == 13)
					tempobj = o_nil;
				else if (lowBits == 14)
					tempobj = o_smalltalk;
				else if (lowBits == 15)
					tempobj = (object *) runningProcess;
				else if ((lowBits >= 30) && (lowBits < 60)) {
					/* get class */
					tempobj =
					    new_sym(classpecial[lowBits-30]);
					tempobj = primitive(FINDCLASS, 1,
						&tempobj);
					}
				else tempobj = new_int(lowBits);
				push(tempobj);
				break;

			case 6: /* pop and store instance variable */
				assign(instvar(lowBits), popstack());
				break;

			case 7: /* pop and store in context */
				assign(tempvar(lowBits), popstack());
				break;

			case 8: /* send a message */
				numargs = lowBits;
				nextbyte(i);
				tempobj = lit(i);
				if (! is_symbol(tempobj)) cant_happen(9);
				message = symbol_value(tempobj);
				goto do_send;

			case 9: /* send a superclass message */
				numargs = lowBits;
				nextbyte(i);
				tempobj = lit(i);
				if (! is_symbol(tempobj)) cant_happen(9);
				message = symbol_value(tempobj);
				receiver =
					fnd_super(anInterpreter->receiver);
				goto do_send2;

			case 10: /* send a special unary message */
				numargs = 0;
				message = unspecial[lowBits];
				goto do_send;

			case 11: /* send a special binary message */
				numargs = 1;
				message = binspecial[lowBits];
				goto do_send;

			case 12: /* send a special arithmetic message */
				tempobj = *(anInterpreter->stacktop - 2);
				if (! is_integer(tempobj)) goto ohwell;
				i = int_value(tempobj);
				tempobj = *(anInterpreter->stacktop - 1);
				if (! is_integer(tempobj)) goto ohwell;
				j = int_value(tempobj);
				decstack(2);
				switch(lowBits) {
					case 0: i += j; break;
					case 1: i -= j; break;
					case 2: i *= j; break;
					case 3: if (i < 0) i = -i;
						i %= j; break;
					case 4: if (j < 0) i >>= (-j);
						else i <<= j; break;
					case 5: i &= j; break;
					case 6: i |= j; break;
					case 7: i = (i < j); break;
					case 8: i = (i <= j); break;
					case 9: i = (i == j); break;
					case 10: i = (i != j); break;
					case 11: i = (i >= j); break;
					case 12: i = (i > j); break;
					case 13: i %= j; break;
					case 14: i /= j; break;
					case 15: i = (i < j) ? i : j;
						break;
					case 16: i = (i < j) ? j : i;
						break;
					default: cant_happen(9);
					}
				if ((lowBits < 7) || (lowBits > 12))
					tempobj = new_int(i); 
				else tempobj = (i ? o_true : o_false);
				push(tempobj);
				break;

				ohwell: /* oh well, send message */
				ohcount++;
				numargs = 1;
				message = arithspecial[lowBits];
				goto do_send;

			case 13: /* send a special ternary keyword messae */
				numargs = 2;
				message = keyspecial[lowBits];
				goto do_send;

			case 14: /* block creation */
				numargs = lowBits;
				if (numargs)
					nextbyte(arglocation);
				nextbyte(i);    /* size of block */
				push(new_block(anInterpreter, numargs,
					arglocation));
				skip(i);
				break;

			case 15: /* special bytecodes */
				spcount[lowBits]++;
				switch(lowBits) {
				case 0: /* no - op */
					break;
				case 1: /* duplicate top of stack */
					push(*(anInterpreter->stacktop - 1));
					break;
				case 2: /* pop top of stack */
					anInterpreter->stacktop--;
					break;
				case 3: /* return top of stack */
					tempobj = popstack();
					goto do_return;
				case 4: /* block return */
					block_return(anInterpreter, popstack());
					return 0;
				case 5: /* self return */
					tempobj = tempvar(0);
					goto do_return;
				case 6: /* skip on true */
					nextbyte(i);
					tempobj = popstack();
					if (tempobj == o_true) {
						skip(i);
						push(o_nil);
						}
					break;
				case 7: /* skip on false */
					nextbyte(i);
					tempobj = popstack();
					if (tempobj == o_false) {
						skip(i);
						push(o_nil);
						}
					break;
				case 8: /* just skip */
					nextbyte(i);
					skip(i);
					break;
				case 9: /* skip backward */
					nextbyte(i);
					skip( - i );
					break;
				case 10: /* execute a primitive */
					nextbyte(numargs);
					nextbyte(i); /* primitive number */
					if (i == BLOCKEXECUTE)
						goto blk_execute;
					else if (i == DOPERFORM)
						goto do_perform;
					else {
						decstack(numargs);
						tempobj = primitive(i, numargs,
						anInterpreter->stacktop);
						push(tempobj);
						}
					break;
				case 11: /* skip true, push true */
					nextbyte(i);
					tempobj = popstack();
					if (tempobj == o_true) {
						skip(i);
						anInterpreter->stacktop++;
						}
					break;
				case 12: /* skip on false, push false */
					nextbyte(i);
					tempobj = popstack();
					if (tempobj == o_false) {
						skip(i);
						anInterpreter->stacktop++;
						}
					break;
				default: 
					cant_happen(9);
				}
				break;
			}
		}
	/* sorry for the unstructured gotos.
		the sins of unstructuredness seemed less bothersome than
		the problems of not doing the same thing in all places
						-tab
		*/
	do_perform:	/* process perform:withArguments: */
		tempobj = popstack();
		message = symbol_value(tempobj);
		tempobj = popstack();
		numargs = tempobj->size - 1;
		for (i = 0; i <= numargs; i++)
			push(tempobj->inst_var[i]);
		/* fall through into do_send */

		/* do_send - call courier to send a message */
	do_send:
		receiver = *(anInterpreter->stacktop - (numargs + 1));
	do_send2:
		decstack(numargs + 1);
		send_mess(anInterpreter, receiver, message,
			anInterpreter->stacktop , numargs);
		return 0;

		/* do_return - return from a message */
	do_return:
		sender = anInterpreter->sender;
		if (is_interpreter(sender)) {
			if (! is_driver(sender))
				push_object(sender, tempobj);
			link_to_process(sender);
			}
		else {
			terminate_process(runningProcess);
			}
		return 0;

		/* blk_execute - perform the block execute primitive */
	blk_execute:
		tempobj = popstack();
		if (! is_integer(tempobj)) cant_happen(9);
		numargs = int_value(tempobj);
		sender = block_execute(anInterpreter->sender, 
			(block *) tempvar(0), numargs, &tempvar(1));
		link_to_process(sender);
		return 0;
}
