/*
	image building utility
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "interp.h"

FILE *fin;
char inputBuffer[1500], *p, tokenBuffer[80];

struct object *lookupGlobal(char * name, int ok_missing);
int parseStatement(void), parseExpression(void), parseTerm(void);
struct object *newOrderedArray(void), *newArray(int size);

void
sysError(char * a, char * b)
{
	printf("in syserror %s %s\n", a, b);
	exit(1);
}

/*
	The following are roots for the file out
*/

struct object *nilObject, *trueObject, *falseObject,
	*globalValues, *SmallIntClass, *ArrayClass, *BlockClass,
	*IntegerClass;

struct object *SymbolClass, *CharClass;

#ifdef gcalloc
# undef gcalloc
#endif

struct object *
gcalloc(int size)
{
	struct object * result;

	result = 
		malloc(sizeof(struct object) + size * sizeof(struct object *));
	if (result == 0) {
		sysError("out of memory", "gcalloc");
	}
	SETSIZE(result, size);
	while (size > 0) {
		result->data[--size] = nilObject;
	}
	return result;
}

struct byteObject *
binaryAlloc(int size)
{
	int osize;
	struct byteObject * result;

	osize = (size + BytesPerWord - 1) / BytesPerWord;
	result = (struct byteObject *) gcalloc(osize);
	SETSIZE(result, size);
	result->size |= FLAG_BIN;
	return result;
}

/* ------------------------------------------------------------- */
/*	Errors   */
/* ------------------------------------------------------------- */

int
parseError(char * msg)
{
	char * q;

	for ( q = inputBuffer; q != p; )
		printf("%c", *q++);
	printf("\n%s\n", msg);
	while (*q)
		printf("%c", *q++);
	printf("\n");
	exit(1);
	return 0;
}

/* ------------------------------------------------------------- */
/*	names   */
/* ------------------------------------------------------------- */

int globalTop = 0;
char *globalNames[100];
struct object *globals[100];

void
addGlobalName(char * name, struct object * value)
{
	char * newName;

	newName = strdup(name);
	if (!newName) {
		sysError("out of memory", "newname in add global");
	}
	globalNames[globalTop] = newName;
	globals[globalTop] = value;
	globalTop++;
}

struct object *
lookupGlobal(char *name, int ok_missing)
{
	int i;

	for (i = 0; i < globalTop; i++) {
		if (strcmp(name, globalNames[i]) == 0) {
			return globals[i];
		}
	}
	/* not found, return 0 */
	if (!ok_missing) {
		sysError("Missing global", name);
	}
	return 0;
}

/* ------------------------------------------------------------- */
/*	Lexical Analysis  */
/* ------------------------------------------------------------- */

void
inputMethodText()
{
	char c;

	p = inputBuffer;
	while (1) {
		while ((c = fgetc(fin)) != '\n')
			*p++ = c;
		*p++ = '\n';
		if ((c = fgetc(fin)) == '!') {
			if ((c = fgetc(fin)) == '\n') {
				*p = '\0';
				return;
				}
			*p++ = '!';
			*p++ = c;
			}
		else
			*p++ = c;
		}
}

void
skipSpaces()
{
	while ((*p == ' ') || (*p == '\t') || (*p == '\n')) p++;
	if (*p == '\"') {
		p++;
		while (*p && (*p != '\"')) p++;
		if (*p != '\"') parseError("unterminated comment");
		p++; skipSpaces();
		}
}

int
isDigit(char p)
{
	if ((p >= '0') && (p <= '9')) return 1;
	return 0;
}

int
isIdentifierChar(char p)
{
	if ((p >= 'a') && (p <= 'z')) return 1;
	if ((p >= 'A') && (p <= 'Z')) return 1;
	return 0;
}

int
isBinary(char p)
{
	switch(p) {
		case '+': case '*': case '-': case '/': case '<': case '=':
		case '>': case '@': case '~': case ',':
			return 1;
		}
	return 0;
}

void
readBinary()
{
	tokenBuffer[0] = *p++;
	if (isBinary(*p)) {
		tokenBuffer[1] = *p++;
		tokenBuffer[2] = '\0';
		}
	else
		tokenBuffer[1] = '\0';
	skipSpaces();
}

int
readIdentifier()
{
	int keyflag;
	char *q = tokenBuffer;
	while (isIdentifierChar(*p) || isDigit(*p)) *q++ = *p++;
	*q = '\0';
	if (*p == ':') {	/* It's a keyword identifier */
		keyflag = 1;
		*q++ = ':';
		*q = '\0';
		p++;
		}
	else
		keyflag = 0;
	skipSpaces();
	return keyflag;
}

int
readInteger()
{
	int val, neg = 0;

	if (*p == '-') {
		neg = 1;
		++p;
	}
	val = *p++ - '0';
	while (isDigit(*p)) {
		val = 10 * val + (*p++ - '0');
	}
	skipSpaces();
	return neg ? -val : val;

}

/* ------------------------------------------------------------- */
/*	new instances of standard things   */
/* ------------------------------------------------------------- */

int symbolTop = 0;
struct object *oldSymbols[500];

int
symbolBareCmp(unsigned char * left, int leftsize, unsigned char * right, int rightsize)
{
	int minsize = leftsize;
	int i;

	if (rightsize < minsize) minsize = rightsize;
	for (i = 0; i < minsize; i++) {
		if (left[i] != right[i]) {
			if (left[i] < right[i]) {
			    return -1;
			} else {
			    return 1;
			}
		}
	}
	return leftsize - rightsize;
}

int
symbolCmp(struct object * left, struct object * right)
{
	return symbolBareCmp(bytePtr(left), SIZE(left),
		bytePtr(right), SIZE(right));
}

struct object *
newSymbol(char * text)
{
	int i;
	struct byteObject *result;

		/* first see if it is already a symbol */
	for (i = 0; i < symbolTop; i++) {
		if (symbolBareCmp((unsigned char *)text, strlen(text),
			bytePtr(oldSymbols[i]), SIZE(oldSymbols[i])) == 0) {
			return oldSymbols[i];
		}
	}

		/* not there, make a new one */
	result = binaryAlloc(strlen(text));
	for (i = 0; i < strlen(text); i++) {
		result->bytes[i] = text[i];
	}
	result->class = lookupGlobal("Symbol", 0);
	oldSymbols[symbolTop++] = (struct object *)result;
	return (struct object *) result;
}

struct object *
newClass(char * name)
{
	struct object *newC;

	newC = gcalloc(ClassSize);
	newC->data[nameInClass] = newSymbol(name);
	return newC;
}

struct object *
newNode(struct object *v, struct object *l, struct object *r)
{
	struct object * result;

	result = gcalloc(3);
	result->class = lookupGlobal("Node", 0);
	result->data[0] = v;
	result->data[1] = l;
	result->data[2] = r;
	return result;
}

struct object *
newTree(void)
{
	struct object * result;

	result = gcalloc(1);
	result->class = lookupGlobal("Tree", 0);
	return result;
}

struct object *
newDictionary(void)
{
	struct object *result;

	result = gcalloc(2);
	result->class = lookupGlobal("Dictionary", 0);
	result->data[0] = newOrderedArray();
	result->data[1] = newArray(0);
	return result;
}

/* ------------------------------------------------------------- */
/*	Code Generation   */
/* ------------------------------------------------------------- */

# define ByteBufferTop 512
unsigned char byteBuffer[ByteBufferTop];
unsigned byteTop;

void
genByte(int v)
{
	byteBuffer[byteTop++] = v;
	if (byteTop >= ByteBufferTop) {
		sysError("too many bytecodes", "");
	}
}

void
genVal(int v)
{
	if ((v < 0) || (v > 0xFFFF)) {
		sysError("illegal value", "");
	}
	genByte(v & 0xFF);
	genByte(v >> 8);
}

void
genValPos(int pos, int v)
{
	if ((v < 0) || (v > 0xFFFF)) {
		sysError("illegal value", "");
	}
	byteBuffer[pos] = v & 0xFF;
	byteBuffer[pos+1] = v >> 8;
}

void
genInstruction(int a, int b)
{
	/*printf("gen instruction %d %d\n", a, b);*/
	if (b < 16) {
		genByte(a * 16 + b);
	} else {
		genInstruction(0, a);
		genByte(b);
	}
}

struct object *
buildByteArray()
{
	struct byteObject * newObj;
	int i;

	newObj = binaryAlloc(byteTop);
	for (i = 0; i < byteTop; i++)
		newObj->bytes[i] = byteBuffer[i];
	newObj->class = lookupGlobal("ByteArray", 0);
	return (struct object *) newObj;
}

# define LiteralBufferTop 60
struct object * litBuffer[LiteralBufferTop];
unsigned litTop = 0;

int
addLiteral(struct object *a)
{
	litBuffer[litTop++] = a;
	if (litTop >= LiteralBufferTop) {
		sysError("too many literals", "");
	}
	return litTop-1;
}

struct object *
buildLiteralArray(void)
{
	int i;
	struct object * result;

	if (litTop == 0)
		return nilObject;
	result = gcalloc(litTop);
	result->class = lookupGlobal("Array", 0);
	for (i = 0; i < litTop; i++)
		result->data[i] = litBuffer[i];
	return result;
}

# define ArgumentBufferTop 30
char * argumentNames[ArgumentBufferTop];
int argumentTop;

void
addArgument(char *name)
{
	char *p;

	p = strdup(name);
	if (!p) {
		sysError("malloc failure", "addArguments");
	}
	argumentNames[argumentTop++] = p;
}

# define TempBufferTop 500
char *tempBuffer[TempBufferTop];
int tempTop, maxTemp;

void
addTemporary(char *name)
{
	char *p;

	p = strdup(name);
	if (!p) {
		sysError("malloc failure", "addTemporary");
	}
	tempBuffer[tempTop++] = p;
	if (tempTop > maxTemp) {
		maxTemp = tempTop;
	}
}

struct object * currentClass;

/* ------------------------------------------------------------- */
/*	big bang   */
/* ------------------------------------------------------------- */

void
bigBang(void)
{
	struct object *ObjectClass, *MetaObjectClass, *ClassClass,
		*NilClass, *TrueClass, *FalseClass, *StringClass,
		*TreeClass, *DictionaryClass, *OrderedArrayClass,
		*MetaClassClass, *ByteArrayClass;

	/*
	 * First, make the nil (undefined) object;
	 * notice its class is wrong
	 */
	nilObject = gcalloc(0);

	/*
	 * Second, make class for Symbol;
	 * this will allow newClass to work correctly
	 */
	SymbolClass = gcalloc(ClassSize + 1);
	addGlobalName("Symbol", SymbolClass);
	SymbolClass->data[nameInClass] = newSymbol("Symbol");

	/* now we can fix up nil's class */
	NilClass = newClass("Undefined");
	addGlobalName("Undefined", NilClass);
	nilObject->class = NilClass;
	addGlobalName("nil", nilObject);

	/* make up the object / metaobject mess */
	ObjectClass = newClass("Object");
	addGlobalName("Object", ObjectClass);
	MetaObjectClass = newClass("MetaObject");
	addGlobalName("MetaObject", MetaObjectClass);
	ObjectClass->class = MetaObjectClass;
	ObjectClass->data[parentClassInClass] = nilObject;

	/* And the Class/MetaClass mess */
	ClassClass = newClass("Class");
	addGlobalName("Class", ClassClass);
	MetaClassClass = newClass("MetaClass");
	addGlobalName("MetaClass", MetaClassClass);
	ClassClass->class = MetaClassClass;

	/* now make up a bunch of other classes */
	BlockClass = newClass("Block");
	addGlobalName("Block", BlockClass);

	/* create Char with extra slot for instance var */
	CharClass = gcalloc(ClassSize + 1);
	addGlobalName("Char", CharClass);
	CharClass->data[nameInClass] = newSymbol("Char");

	/* SmallInt has an extra class variable, just like Symbol and Char */
	SmallIntClass = gcalloc(ClassSize + 1);
	addGlobalName("SmallInt", SmallIntClass);
	SmallIntClass->data[nameInClass] = newSymbol("SmallInt");

	IntegerClass = newClass("Integer");
	addGlobalName("Integer", IntegerClass);

	TrueClass = newClass("True");
	addGlobalName("True", TrueClass);
	trueObject = gcalloc(0);
	trueObject->class = TrueClass;
	addGlobalName("true", trueObject);

	FalseClass = newClass("False");
	addGlobalName("False", FalseClass);
	falseObject = gcalloc(0);
	falseObject->class = FalseClass;
	addGlobalName("false", falseObject);

	ArrayClass = newClass("Array");
	addGlobalName("Array", ArrayClass);
	ByteArrayClass = newClass("ByteArray");
	addGlobalName("ByteArray", ByteArrayClass);

	OrderedArrayClass = newClass("OrderedArray");
	addGlobalName("OrderedArray", OrderedArrayClass);

	StringClass = newClass("String");
	addGlobalName("String", StringClass);

	TreeClass = newClass("Tree");
	addGlobalName("Tree", TreeClass);

	DictionaryClass = newClass("Dictionary");
	addGlobalName("Dictionary", DictionaryClass);

	/* finally, we can fill in the fields in class Object */
	ObjectClass->data[methodsInClass] = newDictionary();
	ObjectClass->data[instanceSizeInClass] = newInteger(0);
	ClassClass->data[instanceSizeInClass] = newInteger(0);

	/* can make global name, but can't fill it in */
	globalValues = gcalloc(2);
	addGlobalName("globals", globalValues);
}

/* ------------------------------------------------------------- */
/*	Parsing */
/* ------------------------------------------------------------- */

int
parseInteger(void)
{
	int i;

	i = readInteger();
	if ((i >= 0) && (i < 10)) {
		genInstruction(PushConstant, i);
	} else {
		genInstruction(PushLiteral, addLiteral(newInteger(i)));
	}
	return 1;
}

int
parsePrimitive(void)
{
	int primitiveNumber, argumentCount;

	/* skip over the left bracket */
	p++; skipSpaces();

	/* then read the primitive number */
	if (isDigit(*p))
		primitiveNumber = readInteger();
	else
		return parseError("missing primitive number");

	/* then read the arguments */
	for (argumentCount = 0; *p && (*p != '>'); argumentCount++)
		if (!parseTerm()) {
			return 0;
		}

	/* make sure we ended correctly */
	if (*p == '>') {
		p++; skipSpaces();
		}
	else
		return parseError("missing > at end of primitive");

	/* generate instructions */
	genInstruction(DoPrimitive, argumentCount);
	genByte(primitiveNumber);

	/* Success */
	return(1);
}

struct object *
newString(char * text)
{
	int size, i;
	struct byteObject *newObj;

	size = strlen(text);
	newObj = binaryAlloc(size);
	for (i = 0; i < size; i++)
		newObj->bytes[i] = text[i];
	newObj->class = lookupGlobal("String", 0);
	return (struct object *) newObj;
}

int
parseString(void)
{
	char *q;

	p++;
	for (q = tokenBuffer; *p && *p != '\''; )
		*q++ = *p++;
	if (*p != '\'')
		return parseError("missing end of string");
	p++; skipSpaces(); *q = '\0';
	genInstruction(PushLiteral, addLiteral(newString(tokenBuffer)));
	return 1;
}

int
lookupInstance(struct object *class, char *text, int *low)
{
	struct object *var;
	int size, i;


 	/* first check superclasses */
 	var = class->data[parentClassInClass];
 	if (var && var != nilObject) {
 		size = lookupInstance(var, text, low);
 		if (size >= 0) return size;
	} else {	/* no superclass */
 		*low = 0;
	}
 
	/* Check our own list of variables */
	var = class->data[variablesInClass];
	if (var && var != nilObject) {
		size = SIZE(var);
	} else {
		size = 0;
	}
	for (i = 0; i < size; i++) {
		if (symbolBareCmp((unsigned char *)text, strlen(text),
		 bytePtr(var->data[i]), (SIZE(var->data[i]))) == 0) {
			return(*low);
		}
		*low += 1;
	}
	return(-1);
}

int superMessage = 0;

char *lowConstants[4] = {"nil", "true", "false", 0};

int
nameTerm(char *name)
{
	int i;

	/* see if temporary */
	for (i = 0; i < tempTop; i++) {
		if (strcmp(name, tempBuffer[i]) == 0) {
			genInstruction(PushTemporary, i);
			return 1;
		}
	}

	/* see if argument */
	for (i = 0; i < argumentTop; i++) {
		if (strcmp(name, argumentNames[i]) == 0) {
			genInstruction(PushArgument, i);
			return 1;
		}
	}

	/* see if super */
	if (strcmp(name, "super") == 0) {
		genInstruction(PushArgument, 0);
		superMessage = 1;
		return 1;
	}

	/* see if low constant */
	for (i = 0; lowConstants[i]; i++) {
		if (strcmp(lowConstants[i], name) == 0) {
			genInstruction(PushConstant, 10+i);
			return 1;
		}
	}

	/* see if instance variable */
	if (currentClass) {
		int low;

		i = lookupInstance(currentClass, name, &low);
		if (i >= 0) {
			genInstruction(PushInstance, i);
			return 1;
		}
	}

	/* see if global */
	{
		struct object *glob = lookupGlobal(name, 1);

		if (glob) {
			genInstruction(PushLiteral, addLiteral(glob));
			return 1;
		}
	}

	return(parseError("unknown identifier"));
}

int returnOp;
char * blockbackup;

int
parseBlock(void)
{
	int savedLocation, saveTop, argCount;
	char * savestart;

	savestart = p;
	p++; skipSpaces();
	genInstruction(PushBlock, tempTop);
	savedLocation = byteTop;
	genVal(0);

	saveTop = tempTop; argCount = 0;
	if (*p == ':') {
		while(1) {
			p++; skipSpaces();
			if (! isIdentifierChar(*p))
				return parseError("missing identifier");
			if (readIdentifier())
				return parseError("keyword illegal");
			addTemporary(tokenBuffer); argCount++;
			if (*p == '|') break;
			if (*p != ':') return parseError("missing colon:");
		}
		p++; skipSpaces();
	}
	if (*p == ']') {
		genInstruction(PushConstant, nilConst);
	} else {
		int saveReturnOp = returnOp;

		returnOp = BlockReturn;
		while (1) {
			if (! parseStatement()) {
				parseError("Statement syntax inside block");
			}
			if (*p == '.') {
				p++;
				skipSpaces();
			}
			if (*p == ']') {
				break;
			} else {
				genInstruction(DoSpecial, PopTop);
			}
		}
		returnOp = saveReturnOp;
	}
	p++; skipSpaces();	/* skip over ] */
	genInstruction(DoSpecial, StackReturn);
	genValPos(savedLocation, byteTop);
	tempTop = saveTop;

	/* set blockbackup to textual start of block */
	blockbackup = savestart;
	return 1;
}

int
parseSymbol(void)
{
	char *q;

	p++;
	for (q=tokenBuffer; isIdentifierChar(*p) || (*p == ':'); )
		*q++ = *p++;
	*q = '\0'; skipSpaces();
	genInstruction(PushLiteral, addLiteral(newSymbol(tokenBuffer)));
	return 1;
}

int
parseChar(void)
{
	struct object * newObj;

	p++;
	newObj = gcalloc(1);
	newObj->class = lookupGlobal("Char", 0);
	newObj->data[0] = newInteger((int) *p);
	genInstruction(PushLiteral, addLiteral(newObj));
	p++; skipSpaces();
	return 1;
}

int
parseTerm(void)
{
	/* make it so anything other than a block zeros out backup var */
	blockbackup = 0;
	superMessage = 0;

	if (*p == '(') {
		p++; skipSpaces();
		if (! parseExpression()) return 0;
		if (*p != ')')
			return parseError("unbalanced parenthesis");
		p++; skipSpaces();
		return 1;
		}
	if (*p == '<') return parsePrimitive();
	if (*p == '$') return parseChar();
	if (isDigit(*p) || (*p == '-')) return parseInteger();
	if (*p == '\'') return parseString();
	if (isIdentifierChar(*p)) {
		readIdentifier();
		return nameTerm(tokenBuffer);
		}
	if (*p == '[') return parseBlock();
	if (*p == '#') return parseSymbol();
	return parseError("illegal start of expression");
}

char *unaryBuiltIns[] = {"isNil", "notNil", 0};
char *binaryBuiltIns[] = {"<", "<=", "+", 0};

int
parseUnaryContinuation(void)
{
	int litNumber, done;
	char *q;

	while (isIdentifierChar(*p)) {
		q = p;
		if (readIdentifier()) {
			p = q;	/* oops, was a keyword */
			break;
			}
		done = 0;
		if (! superMessage) {	int i;
			for (i = 0; unaryBuiltIns[i]; i++)
				if (strcmp(tokenBuffer, unaryBuiltIns[i]) == 0) {
					genInstruction(SendUnary, i);
					done = 1;
					}
			}
		if (! done) {
			genInstruction(MarkArguments, 1);
			litNumber = addLiteral(newSymbol(tokenBuffer));
/*printf("unary %s\n", tokenBuffer);*/
			if (superMessage) {
				genInstruction(DoSpecial, SendToSuper);
				genByte(litNumber);
				}
			else
				genInstruction(SendMessage, litNumber);
			superMessage = 0;
			}
		}
	return 1;
}

int
parseBinaryContinuation(void)
{
	int messLiteral, i, done;
	char messbuffer[80];

	if (! parseUnaryContinuation()) return 0;
	while (isBinary(*p)) {
		readBinary();
/*printf("binary symbol %s\n", tokenBuffer);*/
		strcpy(messbuffer, tokenBuffer);
		if (! parseTerm()) return 0;
		if (! parseUnaryContinuation()) return 0;

		done = 0;
		if (! superMessage)
			for (i = 0; binaryBuiltIns[i]; i++)
				if (strcmp(messbuffer, binaryBuiltIns[i]) == 0) {
					genInstruction(SendBinary, i);
					done = 1;
					}

		if (! done) {
			messLiteral = addLiteral(newSymbol(messbuffer));
			genInstruction(MarkArguments, 2);
			if (superMessage) {
				genInstruction(DoSpecial, SendToSuper);
				genByte(messLiteral);
				superMessage = 0;
				}
			else
				genInstruction(SendMessage, messLiteral);
			}
		}
	return 1;
}

int
optimizeBlock(void)
{
	if (*p != '[') {
		if (! parseTerm()) return 0;
		parseError("missing block as optimized block argument");
	} else {
		p++; skipSpaces();
		if (*p == ']') {
			genInstruction(PushConstant, 0);
			p++; skipSpaces();
			return 1;
			}
		while(1) {
			if (! parseStatement()) return 0;
			if (*p == '.') p++, skipSpaces();
			if (*p == ']') break;
			genInstruction(DoSpecial, PopTop);
			}
		p++; skipSpaces();
		/* just leave last expression on stack */
		}
	return 1;
}

int
controlFlow(int opt1, char * rest, int opt2)
{
	int save1, save2;
	char *q;

	genInstruction(DoSpecial, opt1);
	save1 = byteTop;
	genVal(0);
	if (!optimizeBlock()) {
		parseError("syntax error in control flow");
	}
	genInstruction(DoSpecial, Branch);
	save2 = byteTop;
	genVal(0);
	genValPos(save1, byteTop);
	q = p;
	if (isIdentifierChar(*p) && readIdentifier() && (strcmp(tokenBuffer, rest) == 0)) {
		if (!optimizeBlock()) {
			parseError("syntax error in control cascade");
		}
	} else {
		p = q;
		genInstruction(PushConstant, opt2);
	}
	genValPos(save2, byteTop);
	return 1;
}

int
optimizeLoop(int branchInstruction)
{
	int L1, L2;

	/* back up to start of block and try again */
	p = blockbackup;
	L1 = byteTop;
	optimizeBlock();
	genInstruction(DoSpecial, branchInstruction);
	L2 = byteTop;
	genVal(0);
	if (!(isIdentifierChar(*p) && readIdentifier()))
		return parseError("can't get message again in optimized block");
	/* now read the body */
	optimizeBlock();
	genInstruction(DoSpecial, PopTop);
	genInstruction(DoSpecial, Branch);
	genVal(L1);
	genValPos(L2, byteTop);
	genInstruction(PushConstant, 0);
	return 1;
}

int
parseKeywordContinuation(void)
{
	int argCount, i, done, saveSuper;
	char messageBuffer[100];

	saveSuper = superMessage;
	if (! parseBinaryContinuation()) return 0;
	strcpy(messageBuffer,"");
	argCount = 0;
	if (isIdentifierChar(*p) && readIdentifier()) {
		if (strcmp(tokenBuffer, "ifTrue:") == 0)
			return controlFlow(BranchIfFalse, "ifFalse:", nilConst);
		else if (strcmp(tokenBuffer, "ifFalse:") == 0)
			return controlFlow(BranchIfTrue, "ifTrue:", nilConst);
		else if (strcmp(tokenBuffer, "and:") == 0)
			return controlFlow(BranchIfFalse, "", falseConst);
		else if (strcmp(tokenBuffer, "or:") == 0)
			return controlFlow(BranchIfTrue, "", trueConst);
		else if ((strcmp(tokenBuffer, "whileTrue:") == 0) &&
				blockbackup)
			return optimizeLoop(BranchIfFalse);
		else if ((strcmp(tokenBuffer, "whileFalse:") == 0) &&
				blockbackup)
			return optimizeLoop(BranchIfTrue);
		else
		do{
			strcat(messageBuffer, tokenBuffer);
			argCount++;
			if (! parseTerm()) return 0;
			if (! parseBinaryContinuation()) return 0;
			} while (isIdentifierChar(*p) && readIdentifier());
		}
	if (argCount > 0) {
/*printf("keywork message %s\n", messageBuffer);*/
		done = 0;
		if (! saveSuper)
			for (i = 0; binaryBuiltIns[i]; i++)
				if (strcmp(messageBuffer, binaryBuiltIns[i]) == 0) {
					genInstruction(SendBinary, i);
					done = 1;
					}
		if (! done) {
			genInstruction(MarkArguments, argCount+1);
			if (saveSuper) {
				genInstruction(DoSpecial, SendToSuper);
				genByte(addLiteral(newSymbol(messageBuffer)));
				superMessage = 0;
				}
			else
				genInstruction(SendMessage, addLiteral(newSymbol(messageBuffer)));
			}
		}
	return 1;
}

int
doAssignment(char * name)
{
	int i;

	for (i = 0; i < tempTop; i++)
		if (strcmp(name, tempBuffer[i]) == 0) {
			genInstruction(AssignTemporary, i);
			return 1;
			}

	if (currentClass) {int low;
		i = lookupInstance(currentClass, name, &low);
		if (i >= 0) {
			genInstruction(AssignInstance, i);
			return 1;
			}
		}

	return parseError("unknown target of assignment");
}

int
parseExpression(void)
{
	char nameBuffer[60];

	if (isIdentifierChar(*p)) {
		readIdentifier();
		if ((*p == '<') && (*(p+1) == '-')) {
			p++; p++; skipSpaces();
			strcpy(nameBuffer, tokenBuffer);
			if (! parseExpression()) return 0;
			return doAssignment(nameBuffer);
			}
		if (! nameTerm(tokenBuffer)) return 0;
	} else {
		if (! parseTerm()) {
			return 0;
		}
	}
	if (!parseKeywordContinuation()) {
		return 0;
	}
	while (*p == ';') {
		p++; skipSpaces();
		genInstruction(DoSpecial, Duplicate);
		if (! parseKeywordContinuation()) return 0;
	}
	return 1;
}

int
parseStatement(void)
{
	if (*p == '^') {	/* return statement */
		p++; skipSpaces();
		if (! parseExpression()) return 0;
		genInstruction(DoSpecial, returnOp);
		return 1;
		}
	/* otherwise just an expression */
	if (! parseExpression()) return 0;
	return 1;
}

int
parseBody(void)
{
	returnOp = StackReturn;
	while (*p) {
		if (! parseStatement()) return 0;
		genInstruction(DoSpecial, PopTop);
		if (*p == '.') {
			p++;
			skipSpaces();
			}
		}
	genInstruction(DoSpecial, SelfReturn);
	return 1;
}

int
parseMethodHeader(struct object * theMethod)
{
	char messageBuffer[100], *q;
	int keyflag;

	if (isIdentifierChar(*p)) {
		if (readIdentifier()) {					/* keyword message */
			strcpy(messageBuffer,"");
			keyflag = 1;
			while (keyflag) {
				strcat(messageBuffer,tokenBuffer);
				if (isIdentifierChar(*p) && ! readIdentifier())
					addArgument(tokenBuffer);
				else
					return parseError("missing argument after keyword");
				q = p;
				if (isIdentifierChar(*p) && readIdentifier())
					keyflag = 1;
				else {
					p = q;
					keyflag = 0;
					}
				}
			}
		else
			strcpy(messageBuffer, tokenBuffer);	/* unary message */
		}
	else if (isBinary(*p)) {					/* binary message */
		readBinary();
		strcpy(messageBuffer, tokenBuffer);
		if (! isIdentifierChar(*p))
			return parseError("missing argument");
		readIdentifier();
		addArgument(tokenBuffer);
		}
	else return parseError("ill formed method header");
	theMethod->data[nameInMethod] = newSymbol(messageBuffer);
	printf("Method %s\n", messageBuffer);
	return 1;
}

int
parseTemporaries(void)
{
	tempTop = 0; maxTemp = 0;
	if (*p != '|') return 1;
	p++; skipSpaces();
	while (*p != '|') {
		if (! isIdentifierChar(*p)) return parseError("need identifier");
		if (readIdentifier()) return parseError("keyword illegal");
		addTemporary(tokenBuffer);
		}
	p++; skipSpaces();
	return 1;
}

int
parseMethod(struct object * theMethod)
{
	if (! parseMethodHeader(theMethod))
		return 0;
	if (! parseTemporaries())
		return 0;
	if (parseBody()) {
		theMethod->data[literalsInMethod] = buildLiteralArray();
		theMethod->data[byteCodesInMethod] = buildByteArray();
		theMethod->data[stackSizeInMethod] = newInteger(19);
		theMethod->data[temporarySizeInMethod] = newInteger(maxTemp);
		theMethod->data[classInMethod] = currentClass;
		theMethod->data[textInMethod] = newString(inputBuffer);
		return 1;
	}
	return 0;
}


/* ------------------------------------------------------------- */
/*	Input Processing   */
/* ------------------------------------------------------------- */

/*	read the expression beyond the begin statement */
struct object *
BeginCommand(void)
{
	struct object * bootMethod;

	byteTop = 0;
	litTop = 0;
	argumentTop = 0;
	currentClass = 0;
	tempTop = 0; maxTemp = 0;

	if (!parseBody()) {
		parseError("building begin method");
	}
	printf("parsed begin command ok\n");
        bootMethod = gcalloc(methodSize);
        bootMethod->class = lookupGlobal("Method", 0);
        bootMethod->data[nameInMethod] = newSymbol("boot");
        bootMethod->data[literalsInMethod] = buildLiteralArray();
        bootMethod->data[byteCodesInMethod] = buildByteArray();
        bootMethod->data[stackSizeInMethod] = newInteger(12);
	return bootMethod;
}

/*
 * insert()
 *	Insert an element in the array at the given position
 *
 * Creates a new Array-ish object of the same class as "array",
 * and returns it filled in as requested.
 */
struct object *
insert(struct object *array, int index, struct object *val)
{
	int i, j;
	struct object *o;

	/*
	 * Clone the current object, including class.  Make one
	 * extra slot in the Array storage.
	 */
	o = gcalloc(SIZE(array) + 1);
	o->class = array->class;

	/*
	 * Copy up to the index
	 */
	for (i = 0; i < index; ++i) {
		o->data[i] = array->data[i];
	}

	/*
	 * Put in the new element at this position
	 */
	j = i;
	o->data[i++] = val;

	/*
	 * Now copy the rest
	 */
	for (; j < SIZE(array); ++j) {
		o->data[i++] = array->data[j];
	}
	return(o);
}

/*
 * dictionaryInsert()
 *	Insert a key/value pair into the Dictionary
 */
void
dictionaryInsert(struct object *dict, struct object *index,
		struct object *value)
{
	struct object *keys = dict->data[0], *vals = dict->data[1];
	int i, lim, res;

	/*
	 * Scan the OrderedArray "keys" to find where we fit in
	 */
	for (i = 0, lim = SIZE(keys); i < lim; ++i) {
		res = symbolCmp(index, keys->data[i]);

		/*
		 * We should go in before this node
		 */
		if (res < 0) {
			dict->data[0] = insert(keys, i, index);
			dict->data[1] = insert(vals, i, value);
			return;
		} else if (res > 0) {
			continue;
		} else {
			sysError("dictionary insert:", "duplicate key");
		}
	}

	/*
	 * The new element goes at the end
	 */
	dict->data[0] = insert(keys, i, index);
	dict->data[1] = insert(vals, i, value);
}

/*
 * newArray()
 *	Allocate a new array
 *
 * All slots are initialized to nil
 */
struct object *
newArray(int size)
{
	struct object *result;
	int i;

	result = gcalloc(size);
	result->class = lookupGlobal("Array", 0);
	for (i = 0; i < size; ++i) {
		result->data[i] = nilObject;
	}
	return(result);
}

/*
 * newOrderedArray()
 *	Return a new, empty ordered array
 */
struct object *
newOrderedArray(void)
{
	struct object *result;

	result = gcalloc(0);
	result->class = lookupGlobal("OrderedArray", 0);
	return(result);
}

void
MethodCommand(void)
{
	struct object *theMethod;

	/* read class name */
	readIdentifier();
	currentClass = lookupGlobal(tokenBuffer, 1);
	if (! currentClass) {
		sysError("unknown class in Method", tokenBuffer);
	}

	inputMethodText();

	p = inputBuffer; skipSpaces();

	theMethod = gcalloc(methodSize);
	theMethod->class = lookupGlobal("Method", 0);

	/* fill in method class */
	byteTop = 0;
	litTop = 0;
	argumentTop = 1;

	/*
	 * If successful compile, insert into the method dictionary
	 */
	if (parseMethod(theMethod)) {
		dictionaryInsert(currentClass->data[methodsInClass],
			theMethod->data[nameInMethod], theMethod);
	}
}

void
RawClassCommand(void)
{
	struct object *nClass, *supClass, *instClass;
	int instsize;

	/* read the class */
	readIdentifier();
	nClass = lookupGlobal(tokenBuffer, 1);
	printf("Class %s\n", tokenBuffer);
	if (!nClass) {
		nClass = newClass(tokenBuffer);
		nClass->data[nameInClass] = newSymbol(tokenBuffer);
		addGlobalName(tokenBuffer, nClass);
	}

	/* now read the instance class */
	readIdentifier();
	instClass = lookupGlobal(tokenBuffer, 1);
	if (! instClass) {
		sysError("can't find instance class", tokenBuffer);
	}
	nClass->class = instClass;

	/* now read the super class */
	readIdentifier();
	supClass = lookupGlobal(tokenBuffer, 1);
	if (! supClass)  {
		sysError("can't find super class", tokenBuffer);
	}
	nClass->data[parentClassInClass] = supClass;

	/* rest are instance variables */
	litTop = 0;

	/* Now parse the new instance variables */
	while (*p) {
		if (!isIdentifierChar(*p)) {
			sysError("looking for var", p);
		}
		readIdentifier();
		addLiteral(newSymbol(tokenBuffer));
	}

	/* That's the total of our instance variables */
	instsize = litTop;

	/* Add on size of superclass space */
 	if (supClass != nilObject) {
 		instsize +=
 			integerValue(supClass->data[instanceSizeInClass]);
 	}

	nClass->data[instanceSizeInClass] = newInteger(instsize);
	nClass->data[variablesInClass] = buildLiteralArray();
			/* make a tree for new methods */
	nClass->data[methodsInClass] = newDictionary();
}

/*
 * ClassCommand()
 *	Build the base and meta classes automatically
 *
 * Doesn't support class variables, but handles most of imageSource
 * cases.
 */
void
ClassCommand(void)
{
	char *class, *super, *ivars;

	/* Read the class and superclass */
	readIdentifier();
	class = strdup(tokenBuffer);
	readIdentifier();
	super = strdup(tokenBuffer);

	/* Stash away the instance variable string */
	skipSpaces();
	ivars = strdup(p);

	/* Build the metaclass */
	sprintf(inputBuffer, "RAWCLASS Meta%s Class Meta%s", class, super);
	p = inputBuffer + 9;
	RawClassCommand();

	/* Now the instance class */
	sprintf(inputBuffer, "RAWCLASS %s Meta%s %s %s", class, class,
		super, ivars);
	p = inputBuffer + 9;
	RawClassCommand();
	free(class); free(super); free(ivars);
}

/* ------------------------------------------------------------- */
/*	writing image   */
/* ------------------------------------------------------------- */

# define imageMaxNumberOfObjects 5000
struct object * writtenObjects[imageMaxNumberOfObjects];
int imageTop = 0;

void
writeWord(int i, FILE * fp)
{
	if (i < 0) {
		sysError("writeWord: negative value", (void *)(INT_PTR)i);
	}
	if (i >= 255) {
		fputc(255, fp);
		writeWord(i - 255, fp);
	} else {
		fputc(i, fp);
	}
}

void
imageOut(FILE *fp, struct object *obj)
{
	int i, size;

	if (imageTop > imageMaxNumberOfObjects) {
		fprintf(stderr,"too many indirect objects\n");
		exit(1);
	}

	/*printf("writing out object %d\n", obj);*/

	if (obj == 0) {
		sysError("writing out null object", 0);
		return;
	}

	/* Integer objects are simply encoded as the binary value */
	if (IS_SMALLINT(obj)) {
		int val = integerValue(obj);

		writeWord(2, fp);
		fwrite(&val, sizeof(val), 1, fp);
		return;
	}

	/* see if already written */
	for (i = 0; i < imageTop; i++) {
		if (obj == writtenObjects[i]) {
			if (i == 0)
				writeWord(5, fp);
			else {
				writeWord(4, fp);
				writeWord(i, fp);
			}
			return;
		}
	}

	/* not written, do it now */
	writtenObjects[imageTop++] = obj;

	if (obj->size & FLAG_BIN) {	/* byte objects */
		struct byteObject * bobj = (struct byteObject *) obj;
		size = SIZE(obj);
		writeWord(3, fp);
		writeWord(size, fp);
		/*fprintf(fp, "3 %d \n", size);*/
		for (i = 0; i < size; i++)
			/*fprintf(fp," %d ", bobj->bytes[i]);*/
			writeWord(bobj->bytes[i], fp);
		/*fprintf(fp,"\n");*/
		if (obj->class == 0) {
			printf("byte object 0x%p has null class\n",
				obj);
		}
		imageOut(fp, obj->class);
		return;
	}

	/* ordinary objects */
	size = SIZE(obj);
	/*fprintf(fp,"1 %d ", size);*/
	writeWord(1, fp);
	writeWord(size, fp);
	if (obj->class == 0) {
		printf("object 0x%p has null class\n", obj);
	}
	imageOut(fp, obj->class);
	for (i = 0; i < size; i++) {
		imageOut(fp, obj->data[i]);
	}
	/*fprintf(fp,"\n");*/
}

/* ------------------------------------------------------------- */
/*	fix up symbol tables   */
/* ------------------------------------------------------------- */

struct object *
symbolTreeInsert(struct object * base, struct object * symNode)
{
	if (base == nilObject)
		return symNode;
	if (symbolCmp(symNode->data[valueInNode], base->data[valueInNode])  < 0)
		base->data[leftInNode] =
			symbolTreeInsert(base->data[leftInNode], symNode);
	else
		base->data[rightInNode] =
			symbolTreeInsert(base->data[rightInNode], symNode);
	return base;
}

struct object *
fixSymbols(void)
{
	struct object * t;
	int i;

	t = newTree();
	for (i = 0; i < symbolTop; i++)
		t->data[0] = symbolTreeInsert(t->data[0],
			newNode(oldSymbols[i], nilObject, nilObject));
	return t;
}

void
fixGlobals(void)
{
	struct object *t;
	int i;

	t = globalValues;
	t->class = lookupGlobal("Dictionary", 0);
	t->data[0] = newOrderedArray();
	t->data[1] = newArray(0);

	/*
	 * Insert each class name as a reference to the class
	 * object itself.
	 */
	for (i = 0; i < globalTop; i++) {
		if (strncmp(globalNames[i], "Meta", 4) == 0) {
			continue;
		}
		dictionaryInsert(t, newSymbol(globalNames[i]),
			globals[i]);
	}

	/*
	 * Insert this object itself under the name "Smalltalk"
	 */
	dictionaryInsert(t, newSymbol("Smalltalk"), t);
}

/* ------------------------------------------------------------- */
/*	checkGlobals   */
/* ------------------------------------------------------------- */
void
checkGlobals(void)
{
	int i;
	struct object *o;

	for (i = 0; i < globalTop; i++) {
		o = globals[i];
		if (!o->class) {
			sysError("Never defined", globalNames[i]);
		}
	}
}

/* ------------------------------------------------------------- */
/*	main program   */
/* ------------------------------------------------------------- */

int
main(void)
{
	FILE *fd;
	struct object *bootMethod = 0;

	/* big bang -- create the first classes */
	bigBang();
	addArgument("self");

	if ((fin = fopen("imageSource", "r")) == NULL)
		sysError("file in error", "imageSource");

	/* then read the image source file */
	while(fgets((char *) inputBuffer, 1000, fin)) {
		p = inputBuffer; skipSpaces();
		readIdentifier();

		if (strcmp(tokenBuffer, "BEGIN") == 0) {
			bootMethod = BeginCommand();
		} else if (strcmp(tokenBuffer, "RAWCLASS") == 0) {
			RawClassCommand();
		} else if (strcmp(tokenBuffer, "CLASS") == 0) {
			ClassCommand();
		} else if (strcmp(tokenBuffer, "COMMENT") == 0) {
			/* nothing */ ;
		} else if (strcmp(tokenBuffer, "METHOD") == 0) {
			MethodCommand();
		} else if (strcmp(tokenBuffer, "END") == 0) {
			break;
		} else {
			sysError("unknown command ", tokenBuffer);
		}
	}

	fclose(fin);

	/* then create the tree of symbols */
	SymbolClass->data[symbolsInSymbol] = fixSymbols();
	fixGlobals();

	/* see if anything was never defined in the class source */
	checkGlobals();

	if ((fd = fopen("image", "w")) == NULL) {
		sysError("file out error", "image");
	}
	printf("starting to file out\n");
	imageOut(fd, nilObject);
	imageOut(fd, trueObject);
	imageOut(fd, falseObject);
	imageOut(fd, globalValues);
	imageOut(fd, SmallIntClass);
	imageOut(fd, IntegerClass);
	imageOut(fd, ArrayClass);
	imageOut(fd, BlockClass);
	imageOut(fd, lookupGlobal("Context", 0));
	imageOut(fd, bootMethod);
	imageOut(fd, newSymbol("<"));
	imageOut(fd, newSymbol("<="));
	imageOut(fd, newSymbol("+"));
	imageOut(fd, newSymbol("doesNotUnderstand:"));
	fclose(fd);
	printf("%d objects written\n", imageTop);
	printf("bye for now!\n");
	return(0);
}
