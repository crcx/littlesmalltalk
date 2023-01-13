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
# include <stdio.h>
# include "env.h"
# include "memory.h"
# include "names.h"
# include "interp.h"
# include "lex.h"

		/* all of the following limits could be increased (up to
			256) without any trouble.  They are kept low 
			to keep memory utilization down */

# define codeLimit 256		/* maximum number of bytecodes permitted */
# define literalLimit 32	/* maximum number of literals permitted */
# define temporaryLimit 16	/* maximum number of temporaries permitted */
# define argumentLimit 16	/* maximum number of arguments permitted */
# define instanceLimit 16	/* maximum number of instance vars permitted */
# define methodLimit 32		/* maximum number of methods permitted */

extern object binSyms[];
extern object keySyms[];
extern char *unStrs[], *binStrs[], *keyStrs[];

static boolean parseok;			/* parse still ok? */
static int codeTop;			/* top position filled in code array */
static byte codeArray[codeLimit];	/* bytecode array */
static int literalTop;			/*  ... etc. */
static object literalArray[literalLimit];
static int temporaryTop;
static char *temporaryName[temporaryLimit];
static int argumentTop;
static char *argumentName[argumentLimit];
static int instanceTop;
static char *instanceName[instanceLimit];

static int maxTemporary;		/* highest temporary see so far */
static char selector[80];		/* message selector */

static boolean inBlock;			/* true if compiling a block */
static boolean optimizedBlock;		/* true if compiling optimized block */

setInstanceVariables(aClass)
object aClass;
{	int i, limit;
	object vars;

	if (aClass == nilobj)
		instanceTop = 0;
	else {
		setInstanceVariables(basicAt(aClass, superClassInClass));
		vars = basicAt(aClass, variablesInClass);
		if (vars != nilobj) {
			limit = objectSize(vars);
			for (i = 1; i <= limit; i++)
				instanceName[++instanceTop] = charPtr(basicAt(vars, i));
			}
		}
}

compilError(str1, str2)
char *str1, *str2;
{
	fprintf(stderr,"compiler error: %s %s\n", str1, str2);
	parseok = false;
}

static object newChar(value)
int value;
{	object newobj;

	newobj = allocObject(1);
	basicAtPut(newobj, 1, newInteger(value));
	setClass(newobj, globalSymbol("Char"));
	return(newobj);
}

static object newByteArray(size)
int size;
{	object newobj;

	newobj = allocByte(size);
	setClass(newobj, globalSymbol("ByteArray"));
	return(newobj);
}

static genCode(value)
int value;
{
	if (codeTop >= codeLimit)
		compilError("too many bytecode instructions in method","");
	else
		codeArray[codeTop++] = value;
}

static genInstruction(high, low)
int high, low;
{
	if (low >= 16) {
		genInstruction(0, high);
		genCode(low);
		}
	else
		genCode(high * 16 + low);
}

static int genLiteral(aLiteral)
object aLiteral;
{
	if (literalTop >= literalLimit)
		compilError("too many literals in method","");
	else
		literalArray[++literalTop] = aLiteral;
	return(literalTop - 1);
}

static char *glbsyms[] = {"nil", "true", "false", "smalltalk", 0 };

static boolean nameTerm(name)
char *name;
{	int i;
	boolean done = false;
	boolean isSuper = false;

	/* it might be self or super */
	if (streq(name, "self") || streq(name, "super")) {
		genInstruction(PushArgument, 0);
		done = true;
		if (streq(name,"super")) isSuper = true;
		}

	/* or it might be a temporary */
	if (! done)
		for (i = 1; (! done) && ( i <= temporaryTop ) ; i++)
			if (streq(name, temporaryName[i])) {
				genInstruction(PushTemporary, i-1);
				done = true;
				}

	/* or it might be an argument */
	if (! done)
		for (i = 1; (! done) && (i <= argumentTop ) ; i++)
			if (streq(name, argumentName[i])) {
				genInstruction(PushArgument, i);
				done = true;
				}

	/* or it might be an instance variable */
	if (! done)
		for (i = 1; (! done) && (i <= instanceTop); i++) {
			if (streq(name, instanceName[i])) {
				genInstruction(PushInstance, i-1);
				done = true;
				}
			}

	/* or it might be a global constant */
	if (! done)
		for (i = 0; (! done) && glbsyms[i]; i++)
			if (streq(name, glbsyms[i])) {
				genInstruction(PushConstant, i+4);
				done = true;
				}

	/* not anything else, it must be a global */
	if (! done) {
		genInstruction(PushGlobal, genLiteral(newSymbol(name)));
		}

	return(isSuper);
}

static int parseArray()
{	int i, size, base;
	object newLit, obj;

	base = literalTop;
	ignore nextToken();
	while (parseok && (token != closing)) {
		switch(token) {
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

			case name: case namecolon: case symconst:
				ignore genLiteral(newSymbol(tokenString));
				ignore nextToken();
				break;

			case binary:
				if (streq(tokenString, "(")) {
					ignore parseArray();
					}
				else {
					ignore genLiteral(newSymbol(tokenString));
					ignore nextToken();
					}
				break;

			case charconst:
				ignore genLiteral(newChar(
					newInteger(tokenInteger)));
				ignore nextToken();
				break;

			case strconst:
				ignore genLiteral(newStString(tokenString));
				ignore nextToken();
				break;

			default:
				compilError("illegal text in literal array",
					tokenString);
				ignore nextToken();
				break;
		}
	}

	if (parseok)
		if (! streq(tokenString, ")"))
			compilError("array not terminated by right parenthesis",
				tokenString);
		else
			ignore nextToken();
	size = literalTop - base;
	newLit = newArray(size);
	for (i = size; i >= 1; i--) {
		obj = literalArray[literalTop];
		basicAtPut(newLit, i, obj);
		decr(obj);
		literalArray[literalTop] = nilobj;
		literalTop = literalTop - 1;
		}
	return(genLiteral(newLit));
}

static boolean term()
{	boolean superTerm = false;	/* true if term is pseudo var super */

	if (token == name) {
		superTerm = nameTerm(tokenString);
		ignore nextToken();
		}
	else if (token == intconst) {
		if ((tokenInteger >= 0) && (tokenInteger <= 2))
			genInstruction(PushConstant, tokenInteger);
		else
			genInstruction(PushLiteral, 
				genLiteral(newInteger(tokenInteger)));
		ignore nextToken();
		}
	else if (token == floatconst) {
		genInstruction(PushLiteral, genLiteral(newFloat(tokenFloat)));
		ignore nextToken();
		}
	else if ((token == binary) && streq(tokenString, "-")) {
		if (nextToken() != intconst)
			compilError("negation not followed",
				"by integer");

		if (tokenInteger == 1)
			genInstruction(PushConstant, 3);
		else
			genInstruction(PushLiteral, 
				genLiteral(newInteger( - tokenInteger)));
		ignore nextToken();
		}
	else if (token == charconst) {
		genInstruction(PushLiteral,
			genLiteral(newChar(tokenInteger)));
		ignore nextToken();
		}
	else if (token == symconst) {
		genInstruction(PushLiteral,
			genLiteral(newSymbol(tokenString)));
		ignore nextToken();
		}
	else if (token == strconst) {
		genInstruction(PushLiteral,
			genLiteral(newStString(tokenString)));
		ignore nextToken();
		}
	else if (token == arraybegin) {
		genInstruction(PushLiteral, parseArray());
		}
	else if ((token == binary) && streq(tokenString, "(")) {
		ignore nextToken();
		expression();
		if (parseok)
			if ((token != closing) || ! streq(tokenString, ")"))
				compilError("Missing Right Parenthesis","");
			else
				ignore nextToken();
		}
	else if ((token == binary) && streq(tokenString, "<"))
		parsePrimitive();
	else if ((token == binary) && streq(tokenString, "["))
		block();
	else
		compilError("invalid expression start", tokenString);

	return(superTerm);
}

static parsePrimitive()
{	int primitiveNumber, argumentCount;

	if (nextToken() != intconst)
		compilError("primitive number missing","");
	primitiveNumber = tokenInteger;
	ignore nextToken();
	argumentCount = 0;
	while (parseok && ! ((token == binary) && streq(tokenString, ">"))) {
		(void) term();
		argumentCount++;
		}
	genInstruction(DoPrimitive, argumentCount);
	genCode(primitiveNumber);
	ignore nextToken();
}

static genMessage(toSuper, argumentCount, messagesym)
boolean toSuper;
int argumentCount;
object messagesym;
{
	if (toSuper) {
		genInstruction(DoSpecial, SendToSuper);
		genCode(argumentCount);
		}
	else
		genInstruction(SendMessage, argumentCount);
	genCode(genLiteral(messagesym));
}

static boolean unaryContinuation(superReceiver)
boolean superReceiver;
{	int i;
	boolean sent;
	object messagesym;

	while (parseok && (token == name)) {
		sent = false;
		messagesym = newSymbol(tokenString);
		/* check for built in messages */
		if (! superReceiver)
			for (i = 0; i < 9; i++)
				if (streq(tokenString, unStrs[i])) {
					genInstruction(SendUnary, i);
					sent = true;
					}
		if (! sent) {
			genMessage(superReceiver, 0, messagesym);
			}
		/* once a message is sent to super, reciever is not super */
		superReceiver = false;
		ignore nextToken();
		}
	return(superReceiver);
}

static boolean binaryContinuation(superReceiver)
boolean superReceiver;
{	int i;
	boolean sent, superTerm;
	object messagesym;

	superReceiver = unaryContinuation(superReceiver);
	while (parseok && (token == binary)) {
		messagesym = newSymbol(tokenString);
		ignore nextToken();
		superTerm = term();
		ignore unaryContinuation(superTerm);
		sent = false;
		/* check for built in messages */
		if (! superReceiver) {
			for (i = 0; (! sent) && binStrs[i]; i++)
				if (messagesym == binSyms[i]) {
					genInstruction(SendBinary, i);
					sent = true;
					}

			}
		if (! sent) {
			genMessage(superReceiver, 1, messagesym);
			}
		superReceiver = false;
		}
	return(superReceiver);
}

static int optimizeBlock(instruction, dopop)
int instruction;
boolean dopop;
{	int location;
	boolean saveOB;

	genInstruction(DoSpecial, instruction);
	location = codeTop;
	genCode(0);
	if (dopop)
		genInstruction(DoSpecial, PopTop);
	ignore nextToken();
	if (! streq(tokenString, "["))
		compilError("block needed","following optimized message");
	ignore nextToken();
	saveOB = optimizedBlock;
	optimizedBlock = true;
	body();
	optimizedBlock = saveOB;
	if (! streq(tokenString, "]"))
		compilError("missing close","after block");
	ignore nextToken();
	codeArray[location] = codeTop;
	return(location);
}

static boolean keyContinuation(superReceiver)
boolean superReceiver;
{	int i, j, argumentCount, savetop;
	boolean sent, superTerm;
	object messagesym;
	char pattern[80];

	savetop = codeTop;
	superReceiver = binaryContinuation(superReceiver);
	if (token == namecolon) {
		if (streq(tokenString, "ifTrue:")) {
			i = optimizeBlock(BranchIfFalse, false);
			if (streq(tokenString, "ifFalse:")) {
				codeArray[i] = codeTop + 3;
				ignore optimizeBlock(Branch, true);
				}
			}
		else if (streq(tokenString, "ifFalse:")) {
			i = optimizeBlock(BranchIfTrue, false);
			if (streq(tokenString, "ifTrue:")) {
				codeArray[i] = codeTop + 3;
				ignore optimizeBlock(Branch, true);
				}
			}
		else if (streq(tokenString, "whileTrue:")) {
			genInstruction(SendUnary, 3 /* value command */);
			i = optimizeBlock(BranchIfFalse, false);
			genInstruction(DoSpecial, PopTop);
			genInstruction(DoSpecial, Branch);
			for (j = codeTop - 1; j > 0; j--)
				if ((codeArray[j] == savetop) &&
				    (codeArray[j-1] == CreateBlock*16)) {
					genCode(j-1);
					break;
					}
			if (i == 0)
				compilError("block needed before","whileTrue:");
			codeArray[i] = codeTop;
			}
		else if (streq(tokenString, "and:"))
			ignore optimizeBlock(AndBranch, false);
		else if (streq(tokenString, "or:"))
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
			if (! superReceiver) {
				for (i = 0; (! sent) && binStrs[i]; i++)
					if (messagesym == binSyms[i]) {
						sent = true;
						genInstruction(SendBinary, i);
						}

				for (i = 0; (! sent) && keyStrs[i]; i++)
					if (messagesym == keySyms[i]) {
						genInstruction(SendKeyword, i);
						sent = true;
						}
				}

			if (! sent) {
				genMessage(superReceiver, argumentCount, messagesym);
				}
			}
		superReceiver = false;
		}
	return(superReceiver);
}

static continuation(superReceiver)
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

static expression()
{	boolean superTerm;

	superTerm = term();
	if (parseok)
		continuation(superTerm);
}

static assignment(name)
char *name;
{	int i;
	boolean done;

	done = false;

	/* it might be a temporary */
	for (i = 1; (! done) && (i <= temporaryTop); i++)
		if (streq(name, temporaryName[i])) {
			genInstruction(PopTemporary, i-1);
			done = true;
			}

	/* or it might be an instance variable */
	for (i = 1; (! done) && (i <= instanceTop); i++)
		if (streq(name, instanceName[i])) {
			genInstruction(PopInstance, i-1);
			done = true;
			}

	if (! done)
		compilError("assignment to unknown name", name);
}

static statement()
{	char assignname[80];
	boolean superReceiver = false;

	if ((token == binary) && streq(tokenString, "^")) {
		ignore nextToken();
		expression();
		if (inBlock)
			genInstruction(DoSpecial, BlockReturn);
		else
			genInstruction(DoSpecial, StackReturn);
		}
	else if (token == name) {	/* possible assignment */
		ignore strcpy(assignname, tokenString);
		ignore nextToken();
		if ((token == binary) && streq(tokenString, "<-")) {
			ignore nextToken();
			expression();
			if (inBlock || optimizedBlock)
				if ((token == closing) && streq(tokenString,"]"))
					genInstruction(DoSpecial, Duplicate);
			assignment(assignname);
			if (inBlock && (token == closing) &&
				streq(tokenString, "]"))
				genInstruction(DoSpecial, StackReturn);
			}
		else {		/* not an assignment after all */
			superReceiver = nameTerm(assignname);
			continuation(superReceiver);
			if (! optimizedBlock)
				if (inBlock && (token == closing) &&
					streq(tokenString, "]"))
					genInstruction(DoSpecial, StackReturn);
				else
					genInstruction(DoSpecial, PopTop);
			}
		}
	else {
		expression();
		if (! optimizedBlock)
			if (inBlock && (token == closing) &&
				streq(tokenString, "]"))
				genInstruction(DoSpecial, StackReturn);
			else
				genInstruction(DoSpecial, PopTop);
		}
}

static body()
{
	do {
		statement();
		if ((token == closing) && streq(tokenString, "."))
			ignore nextToken();
		} while (parseok && (token != closing) && (token != inputend));
}

static block()
{	int saveTemporary, argumentCount, fixLocation;
	boolean saveInBlock, saveOB;
	object tempsym;

	saveTemporary = temporaryTop;
	argumentCount = 0;
	ignore nextToken();
	if ((token == binary) && streq(tokenString, ":")) {
		while (parseok && (token == binary) && streq(tokenString,":")) {
			if (nextToken() != name)
				compilError("name must follow colon",
					"in block argument list");
		        if (++temporaryTop > maxTemporary)
				maxTemporary = temporaryTop;
			argumentCount++;
			if (temporaryTop > temporaryLimit)
				compilError("too many temporaries in method","");
			else {
				tempsym = newSymbol(tokenString);
				temporaryName[temporaryTop] = charPtr(tempsym);
				}
			ignore nextToken();
			}
		if ((token != binary) || ! streq(tokenString, "|"))
			compilError("block argument list must be terminated",
					"by |");
		ignore nextToken();
		}
	genInstruction(CreateBlock, argumentCount);
	if (argumentCount != 0){
		genCode(saveTemporary + 1);
		}
	fixLocation = codeTop;
	genCode(0);
	saveInBlock = inBlock;
	saveOB = optimizedBlock;
	inBlock = true;
	optimizedBlock = false;
	body();
	if ((token == closing) && streq(tokenString, "]"))
		ignore nextToken();
	else
		compilError("block not terminated by ]","");
	codeArray[fixLocation] = codeTop;
	inBlock = saveInBlock;
	optimizedBlock = saveOB;
	temporaryTop = saveTemporary;
}

static temporaries()
{	object tempsym;

	temporaryTop = 0;
	if ((token == binary) && streq(tokenString, "|")) {
		ignore nextToken();
		while (token == name) {
			if (++temporaryTop > maxTemporary)
				maxTemporary = temporaryTop;
			if (temporaryTop > temporaryLimit)
				compilError("too many temporaries in method","");
			else {
				tempsym = newSymbol(tokenString);
				temporaryName[temporaryTop] = charPtr(tempsym);
				}
			ignore nextToken();
			}
		if ((token != binary) || ! streq(tokenString, "|"))
			compilError("temporary list not terminated by bar","");
		else
			ignore nextToken();
		}
}

static messagePattern()
{	object argsym;

	argumentTop = 0;
	ignore strcpy(selector, tokenString);
	if (token == name)		/* unary message pattern */
		ignore nextToken();
	else if (token == binary) {	/* binary message pattern */
		ignore nextToken();
		if (token != name) 
			compilError("binary message pattern not followed by name",selector);
		argsym = newSymbol(tokenString);
		argumentName[++argumentTop] = charPtr(argsym);
		ignore nextToken();
		}
	else if (token == namecolon) {	/* keyword message pattern */
		selector[0] = '\0';
		while (parseok && (token == namecolon)) {
			ignore strcat(selector, tokenString);
			ignore nextToken();
			if (token != name)
				compilError("keyword message pattern",
					"not followed by a name");
			if (++argumentTop > argumentLimit)
				compilError("too many arguments in method","");
			argsym = newSymbol(tokenString);
			argumentName[argumentTop] = charPtr(argsym);
			ignore nextToken();
			}
		}
	else
		compilError("illegal message selector", tokenString);
}

boolean parse(method, text)
object method;
char *text;
{	int i;
	object bytecodes, theLiterals;
	byte *bp;

	lexinit(text);
	parseok = true;
	codeTop = 0;
	literalTop = temporaryTop = argumentTop =0;
	maxTemporary = 0;
	inBlock = optimizedBlock = false;

	messagePattern();
	if (parseok)
		temporaries();
	if (parseok)
		body();
	if (parseok)
		genInstruction(DoSpecial, SelfReturn);

	if (! parseok)
		basicAtPut(method, bytecodesInMethod, nilobj);
	else {
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
				}
			basicAtPut(method, literalsInMethod, theLiterals);
			}
		else
			basicAtPut(method, literalsInMethod, nilobj);
		basicAtPut(method, stackSizeInMethod, newInteger(6));
		basicAtPut(method, temporarySizeInMethod,
			newInteger(1 + maxTemporary));
		basicAtPut(method, textInMethod, newStString(text));
		return(true);
		}
	return(false);
}
