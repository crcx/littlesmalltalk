/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	constants and types used by process manager. 
	See process.c and interp.c for more details.
*/
/*
	if there are no enumerated types, make tasks simply integer constants
*/
# ifdef NOENUMS
# define taskType int

# define sendMessageTask 1
# define sendSuperTask   2
# define ReturnTask	 3
# define BlockReturnTask 4
# define BlockCreateTask 5
# define ContextExecuteTask 6

#endif

# ifndef NOENUMS

typedef enum {sendMessageTask, sendSuperTask, ReturnTask, BlockReturnTask,
		BlockCreateTask, ContextExecuteTask} taskType;

# endif

extern int finalStackTop;	/* stack top when finished with interpreter */
extern int finalByteCounter;	/* bytecode counter when finished with interpreter */
extern int argumentsOnStack;	/* position of arguments on stack for mess send */
extern object messageToSend;	/* message to send */
extern object returnedObject;	/* object returned from message */
extern taskType  finalTask;		/* next task to do (see below) */


