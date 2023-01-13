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
