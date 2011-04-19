/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987

	Method parser - parses the textual description of a method,
	generating bytecodes and literals.

	This parser is based around a simple minded recursive descent
	parser.
	It is used both by the module that builds the initial virtual image,
	and by a primitive when invoked from a running Smalltalk system.

	The latter case could, if the bytecode interpreter were fast enough,
	be replaced by a parser written in Smalltalk.  This would be preferable,
	but not if it slowed down the system too terribly.

	To use the parser the routine setInstanceVariables must first be
	called with a class object.  This places the appropriate instance
	variables into the memory buffers, so that references to them
	can be correctly encoded.

	As this is recursive descent, you should read it SDRAWKCAB !
		(from bottom to top)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "interp.h"
#include "lex.h"

		/* all of the following limits could be increased (up to
		   256) without any trouble.  They are kept low 
		   to keep memory utilization down */

#define codeLimit 256		/* maximum number of bytecodes permitted */
#define literalLimit 128	/* maximum number of literals permitted */
#define temporaryLimit 32	/* maximum number of temporaries permitted */
#define argumentLimit 32	/* maximum number of arguments permitted */
#define instanceLimit 32	/* maximum number of instance vars permitted */
#define methodLimit 64		/* maximum number of methods permitted */

boolean parseok;		/* parse still ok? */
extern char peek();
int codeTop;			/* top position filled in code array */
byte codeArray[codeLimit];	/* bytecode array */
int literalTop;			/*  ... etc. */
object literalArray[literalLimit];
int temporaryTop;
char *temporaryName[temporaryLimit];
int argumentTop;
char *argumentName[argumentLimit];
int instanceTop;
char *instanceName[instanceLimit];

int maxTemporary;		/* highest temporary see so far */
char selector[80];		/* message selector */

enum blockstatus { NotInBlock, InBlock, OptimizedBlock } blockstat;

void block();
void body();
void assignment(char *name);
void expression();
void genMessage(boolean toSuper, int argumentCount, object messagesym);
void genInteger(int val);

void compilWarn(char *selector, char *str1, char *str2);
void compilError(char *selector, char *str1, char *str2);
void parsePrimitive();
void sysDecr(object z);


void setInstanceVariables(aClass)
object aClass;
{
    int i, limit;
    object vars;

    if (aClass == nilobj)
	instanceTop = 0;
    else {
	setInstanceVariables(basicAt(aClass, superClassInClass));
	vars = basicAt(aClass, variablesInClass);
	if (vars != nilobj) {
	    limit = sizeField(vars);
	    for (i = 1; i <= limit; i++)
		instanceName[++instanceTop] = charPtr(basicAt(vars, i));
	}
    }
}

void genCode(value)
int value;
{
    if (codeTop >= codeLimit)
	compilError(selector, "too many bytecode instructions in method",
		    "");
    else
	codeArray[codeTop++] = value;
}

void genInstruction(high, low)
int high, low;
{
    if (low >= 16) {
	genInstruction(Extended, high);
	genCode(low);
    } else
	genCode(high * 16 + low);
}

int genLiteral(aLiteral)
object aLiteral;
{
    if (literalTop >= literalLimit)
	compilError(selector, "too many literals in method", "");
    else {
	literalArray[++literalTop] = aLiteral;
	incr(aLiteral);
    }
    return (literalTop - 1);
}

void genInteger(val)		/* generate an integer push */
int val;
{
    if (val == -1)
	genInstruction(PushConstant, minusOne);
    else if ((val >= 0) && (val <= 2))
	genInstruction(PushConstant, val);
    else
	genInstruction(PushLiteral, genLiteral(newInteger(val)));
}

char *glbsyms[] = { "currentInterpreter", "nil", "true", "false",
    0
};

boolean nameTerm(name)
char *name;
{
    int i;
    boolean done = false;
    boolean isSuper = false;

    /* it might be self or super */
    if (streq(name, "self") || streq(name, "super")) {
	genInstruction(PushArgument, 0);
	done = true;
	if (streq(name, "super"))
	    isSuper = true;
    }

    /* or it might be a temporary (reverse this to get most recent first) */
    if (!done)
	for (i = temporaryTop; (!done) && (i >= 1); i--)
	    if (streq(name, temporaryName[i])) {
		genInstruction(PushTemporary, i - 1);
		done = true;
	    }

    /* or it might be an argument */
    if (!done)
	for (i = 1; (!done) && (i <= argumentTop); i++)
	    if (streq(name, argumentName[i])) {
		genInstruction(PushArgument, i);
		done = true;
	    }

    /* or it might be an instance variable */
    if (!done)
	for (i = 1; (!done) && (i <= instanceTop); i++) {
	    if (streq(name, instanceName[i])) {
		genInstruction(PushInstance, i - 1);
		done = true;
	    }
	}

    /* or it might be a global constant */
    if (!done)
	for (i = 0; (!done) && glbsyms[i]; i++)
	    if (streq(name, glbsyms[i])) {
		genInstruction(PushConstant, i + 4);
		done = true;
	    }

    /* not anything else, it must be a global */
    /* must look it up at run time */
    if (!done) {
	genInstruction(PushLiteral, genLiteral(newSymbol(name)));
	genMessage(false, 0, newSymbol("value"));
    }

    return (isSuper);
}

int parseArray()
{
    int i, size, base;
    object newLit, obj;

    base = literalTop;
    ignore nextToken();
    while (parseok && (token != closing)) {
	switch (token) {
	case arraybegin:
	    ignore parseArray();
	    break;

	case intconst:
	    ignore genLiteral(newInteger(tokenInteger));
	    ignore nextToken();
	    break;

	case floatconst:
	    ignore genLiteral(newFloat(tokenFloat));
	    ignore nextToken();
	    break;

	case nameconst:
	case namecolon:
	case symconst:
	    ignore genLiteral(newSymbol(tokenString));
	    ignore nextToken();
	    break;

	case binary:
	    if (streq(tokenString, "(")) {
		ignore parseArray();
		break;
	    }
	    if (streq(tokenString, "-") && isdigit(peek())) {
		ignore nextToken();
		if (token == intconst)
		    ignore genLiteral(newInteger(-tokenInteger));
		else
	    if (token == floatconst) {
		ignore genLiteral(newFloat(-tokenFloat));
	    } else
		compilError(selector, "negation not followed",
			    "by number");
	    ignore nextToken();
	    break;
	    }
	    ignore genLiteral(newSymbol(tokenString));
	    ignore nextToken();
	    break;

	case charconst:
	    ignore genLiteral(newChar(tokenInteger));
	    ignore nextToken();
	    break;

	case strconst:
	    ignore genLiteral(newStString(tokenString));
	    ignore nextToken();
	    break;

	default:
	    compilError(selector, "illegal text in literal array",
			tokenString);
	    ignore nextToken();
	    break;
	}
    }

    if (parseok) {
	if (!streq(tokenString, ")"))
	    compilError(selector,
			"array not terminated by right parenthesis",
			tokenString);
	else
	    ignore nextToken();
    }
    size = literalTop - base;
    newLit = newArray(size);
    for (i = size; i >= 1; i--) {
	obj = literalArray[literalTop];
	basicAtPut(newLit, i, obj);
	decr(obj);
	literalArray[literalTop] = nilobj;
	literalTop = literalTop - 1;
    }
    return (genLiteral(newLit));
}

boolean term()
{
    boolean superTerm = false;	/* true if term is pseudo var super */

    if (token == nameconst) {
	superTerm = nameTerm(tokenString);
	ignore nextToken();
    } else if (token == intconst) {
	genInteger(tokenInteger);
	ignore nextToken();
    } else if (token == floatconst) {
	genInstruction(PushLiteral, genLiteral(newFloat(tokenFloat)));
	ignore nextToken();
    } else if ((token == binary) && streq(tokenString, "-")) {
	ignore nextToken();
	if (token == intconst)
	    genInteger(-tokenInteger);
	else if (token == floatconst) {
	    genInstruction(PushLiteral, genLiteral(newFloat(-tokenFloat)));
	} else
	    compilError(selector, "negation not followed", "by number");
	ignore nextToken();
    } else if (token == charconst) {
	genInstruction(PushLiteral, genLiteral(newChar(tokenInteger)));
	ignore nextToken();
    } else if (token == symconst) {
	genInstruction(PushLiteral, genLiteral(newSymbol(tokenString)));
	ignore nextToken();
    } else if (token == strconst) {
	genInstruction(PushLiteral, genLiteral(newStString(tokenString)));
	ignore nextToken();
    } else if (token == arraybegin) {
	genInstruction(PushLiteral, parseArray());
    } else if ((token == binary) && streq(tokenString, "(")) {
	ignore nextToken();
	expression();
	if (parseok) {
	    if ((token != closing) || !streq(tokenString, ")"))
		compilError(selector, "Missing Right Parenthesis", "");
	    else
		ignore nextToken();
	}
    } else if ((token == binary) && streq(tokenString, "<"))
	parsePrimitive();
    else if ((token == binary) && streq(tokenString, "["))
	block();
    else
	compilError(selector, "invalid expression start", tokenString);

    return (superTerm);
}

void parsePrimitive()
{
    int primitiveNumber, argumentCount;

    if (nextToken() != intconst)
	compilError(selector, "primitive number missing", "");
    primitiveNumber = tokenInteger;
    ignore nextToken();
    argumentCount = 0;
    while (parseok && !((token == binary) && streq(tokenString, ">"))) {
	ignore term();
	argumentCount++;
    }
    genInstruction(DoPrimitive, argumentCount);
    genCode(primitiveNumber);
    ignore nextToken();
}

void genMessage(toSuper, argumentCount, messagesym)
boolean toSuper;
int argumentCount;
object messagesym;
{
    boolean sent = false;
    int i;

    if ((!toSuper) && (argumentCount == 0))
	for (i = 0; (!sent) && unSyms[i]; i++)
	    if (messagesym == unSyms[i]) {
		genInstruction(SendUnary, i);
		sent = true;
	    }

    if ((!toSuper) && (argumentCount == 1))
	for (i = 0; (!sent) && binSyms[i]; i++)
	    if (messagesym == binSyms[i]) {
		genInstruction(SendBinary, i);
		sent = true;
	    }

    if (!sent) {
	genInstruction(MarkArguments, 1 + argumentCount);
	if (toSuper) {
	    genInstruction(DoSpecial, SendToSuper);
	    genCode(genLiteral(messagesym));
	} else
	    genInstruction(SendMessage, genLiteral(messagesym));
    }
}

boolean unaryContinuation(superReceiver)
boolean superReceiver;
{
    int i;
    boolean sent;

    while (parseok && (token == nameconst)) {
	/* first check to see if it could be a temp by mistake */
	for (i = 1; i < temporaryTop; i++)
	    if (streq(tokenString, temporaryName[i]))
		compilWarn(selector, "message same as temporary:",
			   tokenString);
	for (i = 1; i < argumentTop; i++)
	    if (streq(tokenString, argumentName[i]))
		compilWarn(selector, "message same as argument:",
			   tokenString);
	/* the next generates too many spurious messages */
	/* for (i=1; i < instanceTop; i++)
	   if (streq(tokenString, instanceName[i]))
	   compilWarn(selector,"message same as instance",
	   tokenString); */

	sent = false;

	if (!sent) {
	    genMessage(superReceiver, 0, newSymbol(tokenString));
	}
	/* once a message is sent to super, reciever is not super */
	superReceiver = false;
	ignore nextToken();
    }
    return (superReceiver);
}

boolean binaryContinuation(superReceiver)
boolean superReceiver;
{
    boolean superTerm;
    object messagesym;

    superReceiver = unaryContinuation(superReceiver);
    while (parseok && (token == binary)) {
	messagesym = newSymbol(tokenString);
	ignore nextToken();
	superTerm = term();
	ignore unaryContinuation(superTerm);
	genMessage(superReceiver, 1, messagesym);
	superReceiver = false;
    }
    return (superReceiver);
}

int optimizeBlock(instruction, dopop)
int instruction;
boolean dopop;
{
    int location;
    enum blockstatus savebstat;

    savebstat = blockstat;
    genInstruction(DoSpecial, instruction);
    location = codeTop;
    genCode(0);
    if (dopop)
	genInstruction(DoSpecial, PopTop);
    ignore nextToken();
    if (streq(tokenString, "[")) {
	ignore nextToken();
	if (blockstat == NotInBlock)
	    blockstat = OptimizedBlock;
	body();
	if (!streq(tokenString, "]"))
	    compilError(selector, "missing close", "after block");
	ignore nextToken();
    } else {
	ignore binaryContinuation(term());
	genMessage(false, 0, newSymbol("value"));
    }
    codeArray[location] = codeTop + 1;
    blockstat = savebstat;
    return (location);
}

boolean keyContinuation(superReceiver)
boolean superReceiver;
{
    int i, j, argumentCount;
    boolean sent, superTerm;
    object messagesym;
    char pattern[80];

    superReceiver = binaryContinuation(superReceiver);
    if (token == namecolon) {
	if (streq(tokenString, "ifTrue:")) {
	    i = optimizeBlock(BranchIfFalse, false);
	    if (streq(tokenString, "ifFalse:")) {
		codeArray[i] = codeTop + 3;
		ignore optimizeBlock(Branch, true);
	    }
	} else if (streq(tokenString, "ifFalse:")) {
	    i = optimizeBlock(BranchIfTrue, false);
	    if (streq(tokenString, "ifTrue:")) {
		codeArray[i] = codeTop + 3;
		ignore optimizeBlock(Branch, true);
	    }
	} else if (streq(tokenString, "whileTrue:")) {
	    j = codeTop;
	    genInstruction(DoSpecial, Duplicate);
	    genMessage(false, 0, newSymbol("value"));
	    i = optimizeBlock(BranchIfFalse, false);
	    genInstruction(DoSpecial, PopTop);
	    genInstruction(DoSpecial, Branch);
	    genCode(j + 1);
	    codeArray[i] = codeTop + 1;
	    genInstruction(DoSpecial, PopTop);
	} else if (streq(tokenString, "and:"))
	    ignore optimizeBlock(AndBranch, false);
	else
    if (streq(tokenString, "or:"))
	ignore optimizeBlock(OrBranch, false);
	else {
	    pattern[0] = '\0';
	    argumentCount = 0;
	    while (parseok && (token == namecolon)) {
		ignore strcat(pattern, tokenString);
		argumentCount++;
		ignore nextToken();
		superTerm = term();
		ignore binaryContinuation(superTerm);
	    }
	    sent = false;

	    /* check for predefined messages */
	    messagesym = newSymbol(pattern);

	    if (!sent) {
		genMessage(superReceiver, argumentCount, messagesym);
	    }
	}
	superReceiver = false;
    }
    return (superReceiver);
}

void continuation(superReceiver)
boolean superReceiver;
{
    superReceiver = keyContinuation(superReceiver);

    while (parseok && (token == closing) && streq(tokenString, ";")) {
	genInstruction(DoSpecial, Duplicate);
	ignore nextToken();
	ignore keyContinuation(superReceiver);
	genInstruction(DoSpecial, PopTop);
    }
}

void expression()
{
    boolean superTerm;
    char assignname[60];

    if (token == nameconst) {	/* possible assignment */
	ignore strcpy(assignname, tokenString);
	ignore nextToken();
	if ((token == binary) && streq(tokenString, "<-")) {
	    ignore nextToken();
	    assignment(assignname);
	} else {		/* not an assignment after all */
	    superTerm = nameTerm(assignname);
	    continuation(superTerm);
	}
    } else {
	superTerm = term();
	if (parseok)
	    continuation(superTerm);
    }
}

void assignment(name)
char *name;
{
    int i;
    boolean done;

    done = false;

    /* it might be a temporary */
    for (i = temporaryTop; (!done) && (i > 0); i--)
	if (streq(name, temporaryName[i])) {
	    expression();
	    genInstruction(AssignTemporary, i - 1);
	    done = true;
	}

    /* or it might be an instance variable */
    for (i = 1; (!done) && (i <= instanceTop); i++)
	if (streq(name, instanceName[i])) {
	    expression();
	    genInstruction(AssignInstance, i - 1);
	    done = true;
	}

    if (!done) {		/* not known, handle at run time */
	genInstruction(PushArgument, 0);
	genInstruction(PushLiteral, genLiteral(newSymbol(name)));
	expression();
	genMessage(false, 2, newSymbol("assign:value:"));
    }
}

void statement()
{

    if ((token == binary) && streq(tokenString, "^")) {
	ignore nextToken();
	expression();
	if (blockstat == InBlock) {
	    /* change return point before returning */
	    genInstruction(PushConstant, contextConst);
	    genMessage(false, 0, newSymbol("blockReturn"));
	    genInstruction(DoSpecial, PopTop);
	}
	genInstruction(DoSpecial, StackReturn);
    } else {
	expression();
    }
}

void body()
{
    /* empty blocks are same as nil */
    if ((blockstat == InBlock) || (blockstat == OptimizedBlock))
	if ((token == closing) && streq(tokenString, "]")) {
	    genInstruction(PushConstant, nilConst);
	    return;
	}

    while (parseok) {
	statement();
	if (token == closing)
	    if (streq(tokenString, ".")) {
		ignore nextToken();
		if (token == inputend)
		    break;
		else		/* pop result, go to next statement */
		    genInstruction(DoSpecial, PopTop);
	    } else
		break;		/* leaving result on stack */
	else if (token == inputend)
	    break;		/* leaving result on stack */
	else {
	    compilError(selector, "invalid statement ending; token is ",
			tokenString);
	}
    }
}

void block()
{
    int saveTemporary, argumentCount, fixLocation;
    object tempsym, newBlk;
    enum blockstatus savebstat;

    saveTemporary = temporaryTop;
    savebstat = blockstat;
    argumentCount = 0;
    ignore nextToken();
    if ((token == binary) && streq(tokenString, ":")) {
	while (parseok && (token == binary) && streq(tokenString, ":")) {
	    if (nextToken() != nameconst)
		compilError(selector, "name must follow colon",
			    "in block argument list");
	    if (++temporaryTop > maxTemporary)
		maxTemporary = temporaryTop;
	    argumentCount++;
	    if (temporaryTop > temporaryLimit)
		compilError(selector, "too many temporaries in method",
			    "");
	    else {
		tempsym = newSymbol(tokenString);
		temporaryName[temporaryTop] = charPtr(tempsym);
	    }
	    ignore nextToken();
	}
	if ((token != binary) || !streq(tokenString, "|"))
	    compilError(selector, "block argument list must be terminated",
			"by |");
	ignore nextToken();
    }
    newBlk = newBlock();
    basicAtPut(newBlk, argumentCountInBlock, newInteger(argumentCount));
    basicAtPut(newBlk, argumentLocationInBlock,
	       newInteger(saveTemporary + 1));
    genInstruction(PushLiteral, genLiteral(newBlk));
    genInstruction(PushConstant, contextConst);
    genInstruction(DoPrimitive, 2);
    genCode(29);
    genInstruction(DoSpecial, Branch);
    fixLocation = codeTop;
    genCode(0);
    /*genInstruction(DoSpecial, PopTop); */
    basicAtPut(newBlk, bytecountPositionInBlock, newInteger(codeTop + 1));
    blockstat = InBlock;
    body();
    if ((token == closing) && streq(tokenString, "]"))
	ignore nextToken();
    else
    compilError(selector, "block not terminated by ]", "");
    genInstruction(DoSpecial, StackReturn);
    codeArray[fixLocation] = codeTop + 1;
    temporaryTop = saveTemporary;
    blockstat = savebstat;
}

void temporaries()
{
    object tempsym;

    temporaryTop = 0;
    if ((token == binary) && streq(tokenString, "|")) {
	ignore nextToken();
	while (token == nameconst) {
	    if (++temporaryTop > maxTemporary)
		maxTemporary = temporaryTop;
	    if (temporaryTop > temporaryLimit)
		compilError(selector, "too many temporaries in method",
			    "");
	    else {
		tempsym = newSymbol(tokenString);
		temporaryName[temporaryTop] = charPtr(tempsym);
	    }
	    ignore nextToken();
	}
	if ((token != binary) || !streq(tokenString, "|"))
	    compilError(selector, "temporary list not terminated by bar",
			"");
	else
	    ignore nextToken();
    }
}

void messagePattern()
{
    object argsym;

    argumentTop = 0;
    ignore strcpy(selector, tokenString);
    if (token == nameconst)	/* unary message pattern */
	ignore nextToken();
    else
if (token == binary) {		/* binary message pattern */
    ignore nextToken();
    if (token != nameconst)
	compilError(selector,
		    "binary message pattern not followed by name",
		    selector);
    argsym = newSymbol(tokenString);
    argumentName[++argumentTop] = charPtr(argsym);
    ignore nextToken();
} else if (token == namecolon) {	/* keyword message pattern */
    selector[0] = '\0';
    while (parseok && (token == namecolon)) {
	ignore strcat(selector, tokenString);
	ignore nextToken();
	if (token != nameconst)
	    compilError(selector, "keyword message pattern",
			"not followed by a name");
	if (++argumentTop > argumentLimit)
	    compilError(selector, "too many arguments in method", "");
	argsym = newSymbol(tokenString);
	argumentName[argumentTop] = charPtr(argsym);
	ignore nextToken();
    }
} else
    compilError(selector, "illegal message selector", tokenString);
}

boolean parse(method, text, savetext)
object method;
char *text;
boolean savetext;
{
    int i;
    object bytecodes, theLiterals;
    byte *bp;

    lexinit(text);
    parseok = true;
    blockstat = NotInBlock;
    codeTop = 0;
    literalTop = temporaryTop = argumentTop = 0;
    maxTemporary = 0;

    messagePattern();
    if (parseok)
	temporaries();
    if (parseok)
	body();
    if (parseok) {
	genInstruction(DoSpecial, PopTop);
	genInstruction(DoSpecial, SelfReturn);
    }

    if (!parseok) {
	basicAtPut(method, bytecodesInMethod, nilobj);
    } else {
	bytecodes = newByteArray(codeTop);
	bp = bytePtr(bytecodes);
	for (i = 0; i < codeTop; i++) {
	    bp[i] = codeArray[i];
	}
	basicAtPut(method, messageInMethod, newSymbol(selector));
	basicAtPut(method, bytecodesInMethod, bytecodes);
	if (literalTop > 0) {
	    theLiterals = newArray(literalTop);
	    for (i = 1; i <= literalTop; i++) {
		basicAtPut(theLiterals, i, literalArray[i]);
		decr(literalArray[i]);
	    }
	    basicAtPut(method, literalsInMethod, theLiterals);
	} else {
	    basicAtPut(method, literalsInMethod, nilobj);
	}
	basicAtPut(method, stackSizeInMethod, newInteger(6));
	basicAtPut(method, temporarySizeInMethod,
		   newInteger(1 + maxTemporary));
	if (savetext) {
	    basicAtPut(method, textInMethod, newStString(text));
	}
	return (true);
    }
    return (false);
}
