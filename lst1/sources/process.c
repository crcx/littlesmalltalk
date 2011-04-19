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
 * process manager
 * dennis a. vadner and michael t. benhase, 11/84
 * modified by timothy a. budd 4/85
 *************************************************************************/
#include <stdio.h>
#include "lst.h"
#include <signal.h>
#include <setjmp.h>

extern int  test_driver();	/* routine to test for user keystrokes*/

 process  *currentProcess;	/* current process */
 process  *fr_process = 0;	/* process memory free list */

int  atomcnt = 0;			/* atomic action flag */
process  *runningProcess;		/* currently running process,
					   may be different from
					   currentProcess during process
					   termination */

# define PROCINITMAX 6
 process prcinit[PROCINITMAX];	/* initial process free list */


/* init_process - initialize the process module */
init_process ()
{	process *p;
	int i;

	/* first make the initial process free list */
	for (p = prcinit, i = 0; i < PROCINITMAX; i++, p++) {
		p->next = fr_process;
		fr_process = p;
		}

	/* make the process associated with the driver */
	currentProcess = cr_process(o_drive);
	assign(currentProcess->next, currentProcess);
	assign(currentProcess->prev, currentProcess);
	currentProcess->p_state = ACTIVE;
}

/* cr_process - create a new process with the given interpreter */
process  *cr_process (anInterpreter)
interpreter  *anInterpreter;
{	process  *new;

	if (fr_process) {
	    new = (process *) fr_process;
	    fr_process = fr_process->next;
	    }
	else
	    new = structalloc(process);

	new->p_ref_count = 0;
	new->p_size = PROCSIZE;

	sassign(new->interp, anInterpreter);
	new->p_state = SUSPENDED;
	sassign(new->next, (process *) o_nil);
	sassign(new->prev, (process *) o_nil);

	return(new);
}


/* free_process - return an unused process to free list */
free_process (aProcess)
process  *aProcess;
{
	obj_dec((object *) aProcess->interp);
	obj_dec((object *) aProcess->next);
	obj_dec((object *) aProcess->prev);
	aProcess->p_state = TERMINATED;
	aProcess->next = fr_process;
	fr_process = aProcess;
}

/* flush_processes - flush out any remaining process from queue */
flush_processes ()
{
	while (currentProcess != currentProcess->next)
	   remove_process(currentProcess);

	/* prev link and next link should point to the same place now.
	   In order to avoid having memory recovered while we are
	   manipulating pointers, we increment reference count, then change
	   pointers, then decrement reference counts */

	obj_inc((object *) currentProcess);
	safeassign(currentProcess->prev, (process *) o_nil);
	safeassign(currentProcess->next, (process *) o_nil);
	obj_dec((object *) currentProcess);
}


/* link_to_process - change the interpreter for the current process */
link_to_process (anInterpreter)
interpreter  *anInterpreter;
{	object *temp;

	safeassign(runningProcess->interp, anInterpreter);
}


/* remove_process - remove a process from process queue */
 remove_process (aProcess)
process  *aProcess;
{
	if (aProcess == aProcess->next)
	    cant_happen(15);		/* removing last active process */

	/* currentProcess must always point to a process that is on the
	   process queue, make sure this remains true */

	if (aProcess == currentProcess)
	    currentProcess = currentProcess->prev;

	/* In order to avoid having memory recovered while we are changing
	pointers, we increment the reference counts on both processes,
	change pointers, then decrement reference counts */

	obj_inc((object *) currentProcess); obj_inc((object *) aProcess);
	safeassign(aProcess->next->prev, aProcess->prev);
	safeassign(aProcess->prev->next, aProcess->next);
	obj_dec((object *) currentProcess); obj_dec((object *) aProcess);
}


/* schedule_process - add a new process to the process queue */
 schedule_process (aProcess)
process  *aProcess;
{
	safeassign(aProcess->next, currentProcess);
	safeassign(aProcess->prev, currentProcess->prev);
	safeassign(aProcess->prev->next, aProcess);
	safeassign(currentProcess->prev, aProcess);
}

/* set_state - set the state on a process, which may involve inserting or
removing it from the process queue */
int  set_state (aProcess, state)
process  *aProcess;
int  state;
{
	switch (state) {
	    case BLOCKED:
	    case SUSPENDED:
	    case TERMINATED:	if (aProcess->p_state == ACTIVE)
				    remove_process(aProcess);
				aProcess->p_state |= state;
				break;

	    case READY:
	    case UNBLOCKED:	if ((aProcess->p_state ^ state) == ~ACTIVE)
				    schedule_process(aProcess);
				aProcess->p_state &= state;
				break;

	    case CUR_STATE:	break;
	    default:		cant_happen(17);
	    }
	return(aProcess->p_state);
}

# ifdef SETJUMP
 jmp_buf intenv;
# endif

/* brkfun - what to do on a break key */
void brkfun()
{	 int warn = 1;

# ifndef SETJUMP
	exit(1);
# endif
	if (warn) {
		fprintf(stderr,"warning: recovery from interrupt may cause\n");
		fprintf(stderr,"reference counts to be incorrect, and\n");
		fprintf(stderr,"some memory to be inaccessible\n");
		warn = 0;
		}
# ifdef SETJUMP
	longjmp(intenv, 1);
# endif
}

/* start_execution - main execution loop */
start_execution ()
{	interpreter  *presentInterpreter;

	atomcnt = 0;

#ifdef SIGS
	/* trap user interrupt signals and recover */
	signal(SIGINT, brkfun);
#endif

#ifdef SETJUMP
	if (setjmp(intenv)) {
		atomcnt = 0;
		link_to_process(o_drive);
		}
#endif

	while (1) {
	    /* unless it is an atomic action get the next process */
	    if (! atomcnt)
		runningProcess = currentProcess = currentProcess->next;

	    if (! is_driver(runningProcess->interp)) {
		sassign(presentInterpreter, runningProcess->interp);
		resume(presentInterpreter);
		obj_dec((object *) presentInterpreter);
		}
	    else if (! test_driver((currentProcess == currentProcess->next) ||
				   (atomcnt > 0)))
		break;
	    }
}
