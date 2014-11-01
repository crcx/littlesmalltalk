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
 * command parser
 * timothy a. budd, 12/84
 *************************************************************************/
#include <stdio.h>
#include <string.h>

#define DRIVECODE
#include "lst.h"

extern enum lextokens token, nextlex();
extern int prntcmd;
extern int inisstd;
extern int started;
extern char toktext[];
extern char *lexptr;
extern int line_grabber();
extern tok_type t;

/* test_driver - see if the driver should be invoked */
int test_driver(int block)
{
	switch(line_grabber( block )) {
		default: cant_happen(17);
		case -1:
			/*  return end of file indication */
			return(0);
		case 0:
			/* enqueue driver process again */
			return(1);
		case 1:
			if (*lexptr == ')') {
				dolexcommand(lexptr);
				return(1);
				}
			parse();
			return(1);
		}
}

/* ---- code generation routines  -------------- */
# define CODEMAX 500
 uchar code[CODEMAX];
 int codetop = 0;

 gencode(value)
register int value;
{
	if (value >= 256)
		lexerr("code word too big: %d", value);
	if (codetop > CODEMAX)
		lexerr("too many code words: %d", codetop);
	/*if (started)
	fprintf(stderr,"code %d (%d %d)\n", value, value/16, value%16);*/
	code[codetop++] = itouc(value);
}

 genhighlow(high, low)
register int high;
register int low;
{
	if (high < 0 || high > 16)
		lexerr("genhighlow error: %d", high);
	if (low < 0)
		lexerr("genhighlow low error: %d", low);
	if (low < 16) gencode(high * 16 + low);
	else {
		gencode(TWOBIT * 16 + high);
		gencode(low);
		}
}
/*-------------------------------------------------------*/

 int errflag;

/* parse - main parser */
int parse()
{	register int i;

	errflag = 0;
	reset();

	if (nextlex() == nothing) return(1);
	if (token == NL) return(1);

	i = aprimary();
	if (i >= 0) {
		asign(i);
		if ((prntcmd > 1) && inisstd)
			genhighlow(UNSEND, PRNTCMD);
		}
	else {
		cexpression();
		if (prntcmd && inisstd)
			genhighlow(UNSEND, PRNTCMD);
		}
	genhighlow(POPINSTANCE, 0);	/* assign to ``last'' */
	if (errflag)
		return(1);
	if (token == nothing || token == NL) {
		bld_interpreter();
		return(0);
		}
	expect("end of expression");
	return(1);
}

/* asign - code for an assignment statement - leaves result on stack */
 asign(pos)
int pos;
{	int i;

	i = aprimary();
	if (i >= 0) {
		asign(i);
		}
	else {
		cexpression();
		}
	genhighlow(SPECIAL, DUPSTACK);
	genhighlow(POPINSTANCE, pos);
}

/* expression - read an expression, leaving result on stack */
 expression()
{	int i;

	i = aprimary();
	if (i >= 0) {
		asign(i);
		}
	else {
		cexpression();
		}
}

/* cexpression - code for a (possibly cascaded) expression */
 cexpression()
{
	kcontinuation();
	while (token == SEMI) {
		genhighlow(SPECIAL, DUPSTACK);
		nextlex();
		kcontinuation();
		genhighlow(SPECIAL, POPSTACK);
		}
}

/* kcontinuation - keyword continuation */
 kcontinuation()
{	char kbuf[150];
	int  kcount;

	bcontinuation();
	if (token == KEYWORD) {
		kbuf[0] = '\0';
		kcount = 0;
		while (token == KEYWORD) {
			strcat(kbuf, t.c);
			strcat(kbuf, ":");
			kcount++;
			nextlex();
			primary(1);
			bcontinuation();
			}
		gensend(kbuf, kcount);
		}
}

/* isbinary - see if the current token(s) is a binary */
 int isbinary(bbuf)
char *bbuf;
{
	if (token == BINARY || token == MINUS || 
	    token == BAR || token == PE) {
		strcpy(bbuf, t.c);
		nextlex();
		if (token == BINARY || token == MINUS || 
	    	    token == BAR || token == PE) {
			strcat(bbuf, t.c);
			nextlex();
			}
		return(1);
		}
	return(0);
}

/* bcontinuation - binary continuation */
 bcontinuation()
{	char bbuf[3];

	ucontinuation();
	while (isbinary(bbuf)) {
		primary(1);
		ucontinuation();
		gensend(bbuf, 1);
		}
}

/* ucontinuation - unary continuation */
 ucontinuation()
{
	while (token == LOWERCASEVAR) {
		gensend(t.c, 0);
		nextlex();
		}
}

/* aprimary - primary or beginning of assignment */
 int aprimary()
{	char *c;

	if (token == LOWERCASEVAR) {
		c = t.c;
		if (nextlex() == ASSIGN) {
			nextlex();
			return(findvar(c, 1));
			}
		else {
			genvar(c);
			return( -1 );
			}
		}
	primary(1);
	return( - 1 );
}

/* primary - find a primary expression */
 int primary(must)
int must;	/* must we find something ? */
{	int i, count;

	switch(token) {
		case UPPERCASEVAR:
			genhighlow(PUSHCLASS, aliteral(1));
			break;

		case LOWERCASEVAR:
			genvar(t.c);
			nextlex();
			break;

		case LITNUM:
			if (t.i >= 0 && t.i < 10) {
				genhighlow(PUSHSPECIAL, t.i);
				nextlex();
				}
			else {
				genhighlow(PUSHLIT, aliteral(1));
				}
			break;

		case MINUS:
		case LITFNUM:
		case LITCHAR:
		case LITSTR:
		case LITSYM:
		case PS:
			genhighlow(PUSHLIT, aliteral(1));
			break;

		case PSEUDO:
			switch(t.p) {
				case nilvar: i = 13; break;
				case truevar: i = 11; break;
				case falsevar: i = 12; break;
				case smallvar: i  = 14; break;
				default: lexerr("unknown pseudo var %d", t.p);
				}
			genhighlow(PUSHSPECIAL, i);
			nextlex();
			break;

		case PRIMITIVE:
			if (nextlex() != LITNUM) expect("primitive number");
			i = t.i;
			nextlex();
			count = 0;
			while (primary(0)) count++;
			if (token != PE) expect("primitive end");
			nextlex();
			genhighlow(SPECIAL, PRIMCMD);
			gencode(count);
			gencode(i);
			break;

		case LP:
			nextlex();
			expression();
			if (token != RP) expect("right parenthesis");
			nextlex();
			break;

		case LB:
			nextlex();
			do_block();
			break;

		default:
			if (must) expect("primary expression");
			return(0);
		}
	return(1);
}

 int maxtemps = 1;
 int temptop = 0;
 char *tempnames[20];

/* block - parse a block definition */
do_block()
{	int count, i, position;

	count = 0;
	if (token == COLONVAR) {
		while (token == COLONVAR) {
			tempnames[temptop++] = t.c;
			if (temptop > maxtemps) maxtemps = temptop;
			count++;
			nextlex();
			}
		if (token != BAR) 
			expect("bar following arguments in block");
		nextlex();
		}
	genhighlow(BLOCKCREATE, count);
	if (count) 		/* where arguments go in context */
		gencode(1 + (temptop - count));	
	position = codetop;
	gencode(0);

	if (token == RB) {
		genhighlow(PUSHSPECIAL, 13);
		}
	else
		while (1) {
			i = aprimary();
			if (i >= 0) {
				expression();
				if (token != PERIOD)
					genhighlow(SPECIAL, DUPSTACK);
				genhighlow(POPINSTANCE, i);
				}
			else {
				cexpression();
				if (token == PERIOD)
					genhighlow(SPECIAL, POPSTACK);
				}
			if (token != PERIOD)
				break;
			nextlex();
			}
	genhighlow(SPECIAL, RETURN);
	if (token != RB) expect("end of block");
	temptop -= count;
	nextlex();
	i = (codetop - position) - 1;
	if (i > 255)
		lexerr("block too big %d", i);
	code[position] = itouc(i);
}

# define LITMAX 100
 object *lit_array[LITMAX];
 int littop = 0;

 int addliteral(lit)
object *lit;
{
	if (littop >= LITMAX)
		cant_happen(18);
	sassign(lit_array[littop++], lit);
	return(littop - 1);
}

/* aliteral - find a literal that is part of a literal array */
 int aliteral(must)
int must;	/* must we find something ? */
{	char *c;
	object *new;
	int count;
	int bytetop;
	uchar bytes[200];

	switch(token) {
		case MINUS:
			c = t.c;
			nextlex();
			if (token == LITNUM) {
				new = new_int( - t.i );
				nextlex();
				}
			else if (token == LITFNUM) {
				new = new_float( - t.f );
				nextlex();
				}
			else {
				new = new_sym(c);
				}
			break;

		case LITNUM:
			new = new_int(t.i);
			nextlex();
			break;

		case LITFNUM:
			new = new_float(t.f);
			nextlex();
			break;

		case LITCHAR:
			new = new_char(t.i);
			nextlex();
			break;

		case LITSTR:
			new = new_str(t.c);
			nextlex();
			break;

		case LITSYM:
			new = new_sym(t.c);
			nextlex();
			break;

		case PSEUDO:
			switch(t.p) {
				case nilvar: new = o_nil; break;
				case truevar: new = o_true; break;
				case falsevar: new = o_false; break;
				case smallvar: new = o_smalltalk; break;
				default: lexerr("unknown peudo %d", t.p);
				}
			nextlex();
			break;

		case PS:
			nextlex();
			if (token == LP) goto rdarray;
			else if (token == LB) {
				bytetop = 0;
				while (nextlex() == LITNUM)
					bytes[bytetop++] = itouc(t.i);
				if (token != RB)
					expect("right bracket");
				nextlex();
				new = new_bytearray(bytes, bytetop);
				}
			else expect("array or bytearray");
			break;

		case LP: rdarray:
			count = 0;
			nextlex();
			while (aliteral(0) >= 0) {
				count++;
				}
			if (token != RP) expect("right parenthesis");
			nextlex();
			new = new_array(count, 0);
			while (count)
				new->inst_var[--count] = lit_array[--littop];
			break;

		case UPPERCASEVAR:
		case LOWERCASEVAR:
		case KEYWORD:
		case COLONVAR:
		case BINARY:
		case PE:
		case BAR:
		case SEMI:
			new = new_sym(t.c);
			nextlex();
			break;

		default:
			if (must)
				expect("literal");
			else return( - 1 );
		}
	return(addliteral(new));
}

/* gensend - generate a message send */
int gensend(message, numargs)
char *message;
int  numargs;
{	int i;
	char **p, c;
	tok_type e;

	c = *message;
	if (numargs == 0) {
		for (p = unspecial, i = 0; *p; i++, p++)
			if ((**p == c) && (strcmp(*p, message) == 0)) {
				genhighlow(UNSEND, i);
				return 0;
				}
		}
	else if (numargs == 1) {
		for (p = binspecial, i = 0; *p; i++, p++)
			if ((**p == c) && (strcmp(*p, message) == 0)) {
				genhighlow(BINSEND, i);
				return 0;
				}
		for (p = arithspecial, i = 0; *p; i++, p++)
			if ((**p == c) && (strcmp(*p, message) == 0)) {
				genhighlow(ARITHSEND, i);
				return 0;
				}
		}
	else if (numargs == 2) {
		for (p = keyspecial, i = 0; *p; i++, p++)
			if ((**p == c) && (strcmp(*p, message) == 0)) {
				genhighlow(KEYSEND, i);
				return 0;
				}
		}
	genhighlow(SEND, numargs);
	gencode(addliteral(new_sym(message)));
}

 object *var_names;
 object *var_values;

extern object *o_nil, *o_true;

 int findvar(str, make)
char *str;
int make;
{  int i;
   object *comp_obj;

   sassign(comp_obj, new_obj((class *) 0, 2, 0));
   sassign(comp_obj->inst_var[0], o_nil);
   sassign(comp_obj->inst_var[1], new_sym(str));
   for (i = 0; i < var_names->size; i++) {
	assign(comp_obj->inst_var[0], var_names->inst_var[i]);
	if (o_true == primitive(SYMEQTEST, 2, &(comp_obj->inst_var[0]))) {
		obj_dec(comp_obj);
		return(i);
		}
	}
   /* not found, perhaps it's new */
   if (make) {
	assign(comp_obj->inst_var[0], var_names);
	assign(var_names, primitive(GROW, 2, &(comp_obj->inst_var[0])));
	assign(comp_obj->inst_var[0], var_values);
	assign(comp_obj->inst_var[1], o_nil);
	assign(var_values, primitive(GROW, 2, &(comp_obj->inst_var[0])));
	}
   else {
	lexerr("unknown variable %s", str);
	i = 0;
	}
   obj_dec(comp_obj);
   return(i);
}

int genvar(name)
char *name;
{	int i;

	for (i = 0; i < temptop; i++)
		if (strcmp(name, tempnames[i]) == 0) {
			genhighlow(PUSHTEMP, i+1);
			return 0;
			}
	genhighlow(PUSHINSTANCE, findvar(name, 0));
}

/* lexerr - error printing with limited reformatting */
lexerr(s, v)
char *s, *v;
{
	char e1[500], e2[500];
	object *new;

	errflag = 1;
	sprintf(e1, s, v); /* format error message */
	sprintf(e2, "error: %s\n", e1);
	sassign(new, new_str(e2));
	primitive(ERRPRINT, 1, &new);
	obj_dec(new);
}

expect(str)
char *str;
{	char ebuf[150];

	/*fprintf(stderr,"expected %s\n", str);
	fprintf(stderr,"current token type %d\n", token);
	fprintf(stderr,"remainder of line %s\n", lexptr);
	fprintf(stderr,"current text %s\n", toktext);*/
	sprintf(ebuf,"expected %s found %s", str, toktext);
	lexerr(ebuf,"");
}

extern object *o_drive;	/* ``driver'' interpreter */

int bld_interpreter()
{  interpreter *interp;
   object *literals, *bytecodes, *context;
   int i;

   if (codetop == 0) {
	return 0;
	}
   genhighlow(SPECIAL, SELFRETURN);
   gencode(0);			/* mark end of bytecodes */
   sassign(literals, new_array(littop, 0));
   for (i = 0; i < littop; i++)
	literals->inst_var[ i ] = lit_array[i];
   sassign(bytecodes, new_bytearray(code, codetop));
   sassign(context, new_obj((class *) 0, 1 + maxtemps, 1));
   interp = cr_interpreter((interpreter *) o_drive, var_values, 
		literals, bytecodes, context);
   link_to_process(interp);
   obj_dec(context);
   obj_dec(bytecodes);
   obj_dec(literals);
}

reset(){
	codetop = littop = temptop = 0;
	maxtemps = 1;
}

/* drv_init initializes the driver, should be called only once */
drv_init() {
	sassign(var_names, new_obj((class *) 0, 0, 0));
	sassign(var_values, new_obj((class *) 0, 0, 0));
	reset();
	findvar("last", 1); 	/* create variable "last" */
	}

drv_free() {
	int i;

	for (i = 0; i < var_values->size; i++)
		assign(var_values->inst_var[ i ], o_nil);
	obj_dec(var_names);
	obj_dec(var_values);
	}
