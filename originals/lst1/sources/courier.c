/*
	Little Smalltalk
		courier - message passing interface

		timothy a. budd 10/84
*/
/*
	The source code for the Little Smalltalk System may be freely
	copied provided that the source of all files is acknowledged
	and that this condition is copied with each file.

	The Little Smalltalk System is distributed without responsibility
	for the performance of the program and without any guarantee of
	maintenance.

	All questions concerning Little Smalltalk should be addressed to:

		Professor Tim Budd
		Department of Computer Science
		Oregon State University
		Corvallis, Oregon
		97331
		USA
*/
# include <stdio.h>
# include "object.h"
# include "interp.h"
# include "string.h"
# include "symbol.h"
# include "primitive.h"
# define streq(x,y) (strcmp(x,y) == 0)

/* send_mess - find the method needed to respond to a message, create the
	proper context and interpreter for executing the method */
send_mess(sender, receiver, message, args, numargs)
interpreter *sender;
object *receiver, **args;
char *message;
int numargs;
{	object *robject, *method;
	register object *message_array;
	object *context, *fnd_super(), *fnd_class();
	class  *objclass;
	interpreter *anInterpreter;
	int    i, maxc;

	for (robject = receiver; robject; ) {
		if (is_bltin(robject))
			objclass = (class *) fnd_class(robject);
		else
			objclass = robject->class;
		if ((objclass == (class *) 0) || ! is_class(objclass))  break;

		message_array = objclass->message_names;
		for (i = 0; i < message_array->size; i++) {
			if (symbol_value(message_array->inst_var[i]) ==
						message) {
				method = (objclass->methods)->inst_var[ i ];
				goto do_cmd;
				}
			}
		if (is_bltin(robject))
			robject = fnd_super(robject);
		else
			robject = robject->super_obj;
		}

/* if we reach this point then no method has been found matching message */
	sassign(robject, new_obj((class *) 0, 2, 0));
	sassign(robject->inst_var[0], receiver);
	sassign(robject->inst_var[1], new_sym(message));
	primitive(NORESPONDERROR, 2, &(robject->inst_var[0]));
	obj_dec(robject);
	/* generate a message passing trace */
	backtrace(sender);
	/* return nil by default */
	if (is_interpreter(sender))
		push_object(sender, o_nil);
	goto clean_up;

/* do an interpreted method */
/* make a context and fill it in, make an interpeter and link it into
process queue */
do_cmd:
	maxc = objclass->context_size;
	sassign(context, new_obj((class *)0, maxc, 0));
	for (i = 0; i <= numargs; i++)
		sassign(context->inst_var[i], args[i]);
	for ( ; i < maxc ; i++ )
		sassign(context->inst_var[i], o_nil);
	anInterpreter = cr_interpreter(sender, robject, method->inst_var[1],
		method->inst_var[0], context);
	link_to_process(anInterpreter);
	obj_dec(context);
	goto clean_up;

/* clean up after yourself */
clean_up:
	return;
}

/* responds_to - see if a class responds to a message */
int responds_to(message, aClass)
char *message;
class *aClass;
{	object *message_names;
	int i;

	message_names = aClass->message_names;
	for (i = 0; i < message_names->size; i++)
		if (streq(symbol_value(message_names->inst_var[i]),
				message))
			return(1);
	return(0);
}

/* backtrace - generate a backwards message passing trace */
static backtrace(current)
interpreter *current;
{
	while (is_interpreter(current->sender) &&
			! is_driver(current->sender)) {
		fnd_message(current->receiver, current->bytecodes);
		current = current->sender;
		}
}

/* fnd_message - find the message associated with an interpreter */
static fnd_message(receiver, bytecodes)
object *receiver, *bytecodes;
{	int i;
	class *oclass;
	object *messar, *temp;
	char buffer[100];

	oclass = (class *) fnd_class(receiver);

	messar = oclass->methods;
	for (i = 0; i < messar->size; i++) {
		if ((messar->inst_var[i])->inst_var[0] == bytecodes) {
			sprintf(buffer,"%s: backtrace. message  %s",
				symbol_value(oclass->class_name),
				symbol_value(
					(oclass->message_names)->inst_var[i]));
			sassign(temp, new_str(buffer));
			primitive(ERRPRINT, 1, &temp);
			obj_dec(temp);
			return;
			}
		}
	cant_happen(24);
}

/* prnt_messages - print all the messages a class responds to.
	needed because the messages names array for some of the classes is
	created before ArrayedCollection, and thus some do not respond to
	do: */
prnt_messages(aClass)
class *aClass;
{	object *message_names;
	int i;

	message_names = aClass->message_names;
	for (i = 0; i < message_names->size; i++)
		primitive(SYMPRINT, 1, &message_names->inst_var[i]);
}
