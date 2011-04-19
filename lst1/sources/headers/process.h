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
 *************************************************************************/
/*
	Little Smalltalk

		process definitions
		dennis a. vadner and michael t. benhase,  11/84
*/
/*
	the process

		interp = pointer to the head of the process'
			 interpreter chain
		p_state = current state of the process

		next = link to the next process in the active list
		prev = link to the previous process in the active list
*/


struct  process_struct {
	int		p_ref_count;
	int		p_size;
	interpreter	*interp;
	int		p_state;
	struct process_struct  *next;
	struct process_struct  *prev;
	} ;

typedef  struct process_struct  process;

extern int  atomcnt;			/* atomic action flag */
extern process  *runningProcess;	/* currently running process */

extern process  *cr_process();		/* create a new process */
extern int  set_state();		/* set the state on a process */


/* process states */

# define  ACTIVE	0
# define  SUSPENDED	1
# define  READY		~SUSPENDED
# define  BLOCKED	2
# define  UNBLOCKED	~BLOCKED
# define  TERMINATED	4

# define  CUR_STATE	10


# define  terminate_process(aProcess)  {set_state(aProcess, TERMINATED); \
					if (aProcess == runningProcess)  \
					    atomcnt = 0;}
