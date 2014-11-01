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
 * block creation and block return
 * timothy a. budd, 10/84
 *************************************************************************/
#include <stdio.h>
#include "lst.h"

extern object *o_object;	/* value of generic object */

static mstruct *fr_block = 0;	/* free list of unused blocks */

int ca_block = 0;		/* count block allocations */

/* cpyInterpreter - make a new copy of an existing interpreter */
static interpreter *cpyInterpreter(anInterpreter)
interpreter *anInterpreter;
{	interpreter *new;

	new = cr_interpreter((interpreter *) 0,
		anInterpreter->receiver,
		anInterpreter->literals,
		anInterpreter->bytecodes,
		anInterpreter->context);

	if (anInterpreter->creator)
		new->creator = anInterpreter->creator;
	else
		new->creator = anInterpreter;

	new->currentbyte = anInterpreter->currentbyte;
	return(new);
}

/* new_block - create a new instance of class Block */
object *new_block(anInterpreter, argcount, arglocation)
interpreter *anInterpreter;
int argcount, arglocation;
{	block *new;

	if (fr_block) {
		new = (block *) fr_block;
		fr_block = fr_block->mlink;
		}
	else {
		new = structalloc(block);
		ca_block++;
		}

	new->b_ref_count = 0;
	new->b_size = BLOCKSIZE;

	sassign(new->b_interpreter, cpyInterpreter(anInterpreter));
	new->b_numargs = argcount;
	new->b_arglocation = arglocation;
	return((object *) new);
}

/* free_block - return an unused block to the block free list */
free_block(b)
block *b;
{
	if (! is_block(b)) 
		cant_happen(8);

	obj_dec((object *)(b->b_interpreter));

	((mstruct *) b)->mlink = fr_block;
	fr_block = (mstruct *) b;
}

/* block_execute - queue a block interpreter for execution */
interpreter *block_execute(sender, aBlock, numargs, args)
interpreter *sender;
block *aBlock;
int numargs;
object **args;
{	interpreter *newInt;
	object *tempobj;

	if (! is_block(aBlock)) cant_happen(11);
	if (numargs != aBlock->b_numargs) {
		sassign(tempobj, 
			new_str("wrong number of arguments for block"));
		primitive(ERRPRINT, 1, &tempobj);
		obj_dec(tempobj);
		if (sender) {
			push_object(sender, o_nil);
			}
		return(sender); /* not sure about this ..... */
		}

	/* we copy the interpreter so as to not destroy the original and to
	   avoid memory pointer cycles */

	newInt = cpyInterpreter(aBlock->b_interpreter);
	if (sender)
		assign(newInt->sender, sender);
	if (numargs)
		copy_arguments(newInt, aBlock->b_arglocation, 
			numargs, args);
	return(newInt);
}

/* block_return - return an object from the context in which a block was
created */
int block_return(blockInterpreter, anObject)
interpreter *blockInterpreter;
object *anObject;
{	interpreter *backchain, *parent;
	interpreter *creatorblock;

	creatorblock = blockInterpreter->creator;
	for (backchain = blockInterpreter->sender; backchain; 
			backchain = backchain->sender) {
		if (! is_interpreter(backchain)) break;
		if (backchain == creatorblock) {
			/* found creating context, back up one more */
			parent = backchain->sender;
			if (parent) {
				if (! is_driver(parent))
					push_object(parent, anObject);
				link_to_process(parent);
				}
			else {
				terminate_process(runningProcess);
				}
			return 0;
			}
		}

	/* no block found, issue error message */
	primitive(BLKRETERROR, 1, (object **) &blockInterpreter);
	parent = blockInterpreter->sender;
	if (parent) {
		if (! is_driver(parent))
			push_object(parent, anObject);
		link_to_process(parent);
		}
	else {
		terminate_process(runningProcess);
		}
}
