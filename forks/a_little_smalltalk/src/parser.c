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
#include <ctype.h>
#include "env.h"
#include "memory.h"
#include "names.h"
#include "interp.h"
#include "lex.h"
#include <string.h>
#include "tty.h"
#include "news.h"
#include "parser.h"

/* all of the following limits could be increased (up to
 256) without any trouble.  They are kept low
 to keep memory utilization down */

# define codeLimit      256 /* maximum number of bytecodes permitted */
# define literalLimit   256 /* maximum number of literals permitted */
# define temporaryLimit 256 /* maximum number of temporaries permitted */
# define argumentLimit  256 /* maximum number of arguments permitted */
# define instanceLimit  256 /* maximum number of instance vars permitted */
# define methodLimit    256 /* maximum number of methods permitted */

boolean parseok; /* parse still ok? */
extern char peek();
static int codeTop; /* top position filled in code array */
static byte codeArray[codeLimit]; /* bytecode array */
static int literalTop; /*  ... etc. */
static object literalArray[literalLimit];
static int temporaryTop;
static char *temporaryName[temporaryLimit];
static int argumentTop;
static char *argumentName[argumentLimit];
static int instanceTop;
static char *instanceName[instanceLimit];

static int maxTemporary; /* highest temporary see so far */
static char selector[80]; /* message selector */

//Forward decleration
static void parsePrimitive();
static void genMessage(boolean toSuper, int argumentCount, object messagesym);
static void expression();
static void assignment(char *name);
static void body();

enum blockstatus
{
    NotInBlock, InBlock, OptimizedBlock
} blockstat;

void setInstanceVariables(object aClass)
{
    int i, limit;
    object vars;

    if (aClass == nilobj)
        instanceTop = 0;
    else
    {
        setInstanceVariables(basicAt(aClass, superClassInClass));
        vars = basicAt(aClass, variablesInClass);
        if (vars != nilobj)
        {
            limit = sizeField(vars);
            for (i = 1; i <= limit; i++)
                instanceName[++instanceTop] = charPtr(basicAt(vars, i));
        }
    }
}

static void genCode(int value)
{
    if (codeTop >= codeLimit)
        compilError(selector, "too many bytecode instructions in method", "");
    else
        codeArray[codeTop++] = value;
}

static void genInstruction(int high, int low)
{
    if (low >= 16)
    {
        genInstruction(Extended, high);
        genCode(low);
    }
    else
        genCode(high * 16 + low);
}

static int genLiteral(object aLiteral)
{
    if (literalTop >= literalLimit)
        compilError(selector, "too many literals in method", "");
    else
    {
        literalArray[++literalTop] = aLiteral;
        incr(aLiteral);
    }
    return (literalTop - 1);
}

/* generate an integer push */
static void genInteger(int val)
{
    if (val == -1)
        genInstruction(PushConstant, minusOne);
    else if ((val >= 0) && (val <= 2))
        genInstruction(PushConstant, val);
    else
        genInstruction(PushLiteral, genLiteral(newInteger(val)));
}

static const char *glbsyms[] = { "currentInterpreter", "nil", "true", "false", 0 };

static boolean nameTerm(char *name)
{
    int i;
    boolean done = false;
    boolean isSuper = false;

    /* it might be self or super */
    if ((strcmp(name, "self")==0) || (strcmp(name, "super")==0))
    {
        genInstruction(PushArgument, 0);
        done = true;
        if (strcmp(name,"super")==0)
            isSuper = true;
    }

    /* or it might be a temporary (reverse this to get most recent first)*/
    if (!done)
        for (i = temporaryTop; (!done) && (i >= 1); i--)
            if (strcmp(name, temporaryName[i])==0)
            {
                genInstruction(PushTemporary, i - 1);
                done = true;
            }

    /* or it might be an argument */
    if (!done)
        for (i = 1; (!done) && (i <= argumentTop); i++)
            if (strcmp(name, argumentName[i])==0)
            {
                genInstruction(PushArgument, i);
                done = true;
            }

    /* or it might be an instance variable */
    if (!done)
        for (i = 1; (!done) && (i <= instanceTop); i++)
        {
            if (strcmp(name, instanceName[i])==0)
            {
                genInstruction(PushInstance, i - 1);
                done = true;
            }
        }

    /* or it might be a global constant */
    if (!done)
        for (i = 0; (!done) && glbsyms[i]; i++)
            if (strcmp(name, glbsyms[i])==0)
            {
                genInstruction(PushConstant, i + 4);
                done = true;
            }

    /* not anything else, it must be a global */
    /* must look it up at run time */
    if (!done)
    {
        genInstruction(PushLiteral, genLiteral(newSymbol(name)));
        genMessage(false, 0, newSymbol("value"));
    }

    return (isSuper);
}

static int parseArray()
{
    int i, size, base;
    object newLit, obj;

    base = literalTop;
    nextToken();
    while (parseok && (token != closing))
    {
        switch (token)
        {
        case arraybegin:
            parseArray();
            break;

        case intconst:
            genLiteral(newInteger(tokenInteger));
            nextToken();
            break;

        case floatconst:
            genLiteral(newFloat(tokenFloat));
            nextToken();
            break;

        case nameconst:
        case namecolon:
        case symconst:
            genLiteral(newSymbol(tokenString));
            nextToken();
            break;

        case binary:
            if (strcmp(tokenString, "(")==0)
            {
                parseArray();
                break;
            }
            if ((strcmp(tokenString, "-")==0) && isdigit(peek()))
            {
                nextToken();
                if (token == intconst)
                    genLiteral(newInteger(- tokenInteger));
                else if (token == floatconst)
                {
                    genLiteral(newFloat(-tokenFloat));
                }
                else
                    compilError(selector, "negation not followed", "by number");
                nextToken();
                break;
            }
            genLiteral(newSymbol(tokenString));
            nextToken();
            break;

        case charconst:
            genLiteral(newChar(tokenInteger));
            nextToken();
            break;

        case strconst:
            genLiteral(newStString(tokenString));
            nextToken();
            break;

        default:
            compilError(selector, "illegal text in literal array", tokenString);
            nextToken();
            break;
        }
    }

    if (parseok)
    {
        if (!strcmp(tokenString, ")")==0)
            compilError(selector, "array not terminated by right parenthesis",
                             tokenString);
        else
            nextToken();
    }
    size = literalTop - base;
    newLit = newArray(size);
    for (i = size; i >= 1; i--)
    {
        obj = literalArray[literalTop];
        basicAtPut(newLit, i, obj);
        decr(obj);
        literalArray[literalTop] = nilobj;
        literalTop = literalTop - 1;
    }
    return (genLiteral(newLit));
}

static void block()
{
    int saveTemporary, argumentCount, fixLocation;
    object tempsym, newBlk;
    enum blockstatus savebstat;

    saveTemporary = temporaryTop;
    savebstat = blockstat;
    argumentCount = 0;
    nextToken();
    if ((token == binary) && (strcmp(tokenString, ":")==0))
    {
        while (parseok && (token == binary) && (strcmp(tokenString,":")==0))
        {
            if (nextToken() != nameconst)
                compilError(selector, "name must follow colon",
                                 "in block argument list");
            if (++temporaryTop > maxTemporary)
                maxTemporary = temporaryTop;
            argumentCount++;
            if (temporaryTop > temporaryLimit)
                compilError(selector, "too many temporaries in method", "");
            else
            {
                tempsym = newSymbol(tokenString);
                temporaryName[temporaryTop] = charPtr(tempsym);
            }
            nextToken();
        }
        if ((token != binary) || (!strcmp(tokenString, "|")==0))
            compilError(selector, "block argument list must be terminated",
                             "by |");
        nextToken();
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
    /*genInstruction(DoSpecial, PopTop);*/
    basicAtPut(newBlk, bytecountPositionInBlock, newInteger(codeTop+1));
    blockstat = InBlock;
    body();
    if ((token == closing) && (strcmp(tokenString, "]")==0))
        nextToken();
    else
        compilError(selector, "block not terminated by ]", "");
    genInstruction(DoSpecial, StackReturn);
    codeArray[fixLocation] = codeTop + 1;
    temporaryTop = saveTemporary;
    blockstat = savebstat;
}

static boolean term()
{
    boolean superTerm = false; /* true if term is pseudo var super */

    if (token == nameconst)
    {
        superTerm = nameTerm(tokenString);
        nextToken();
    }
    else if (token == intconst)
    {
        genInteger(tokenInteger);
        nextToken();
    }
    else if (token == floatconst)
    {
        genInstruction(PushLiteral, genLiteral(newFloat(tokenFloat)));
        nextToken();
    }
    else if ((token == binary) && (strcmp(tokenString, "-")==0))
    {
        nextToken();
        if (token == intconst)
            genInteger(-tokenInteger);
        else if (token == floatconst)
        {
            genInstruction(PushLiteral, genLiteral(newFloat(-tokenFloat)));
        }
        else
            compilError(selector, "negation not followed", "by number");
        nextToken();
    }
    else if (token == charconst)
    {
        genInstruction(PushLiteral, genLiteral(newChar(tokenInteger)));
        nextToken();
    }
    else if (token == symconst)
    {
        genInstruction(PushLiteral, genLiteral(newSymbol(tokenString)));
        nextToken();
    }
    else if (token == strconst)
    {
        genInstruction(PushLiteral, genLiteral(newStString(tokenString)));
        nextToken();
    }
    else if (token == arraybegin)
    {
        genInstruction(PushLiteral, parseArray());
    }
    else if ((token == binary) && (strcmp(tokenString, "(")==0))
    {
        nextToken();
        expression();
        if (parseok)
        {
            if ((token != closing) || (!strcmp(tokenString, ")")==0))
                compilError(selector, "Missing Right Parenthesis", "");
            else
                nextToken();
        }
    }
    else if ((token == binary) && (strcmp(tokenString, "<")==0))
        parsePrimitive();
    else if ((token == binary) && (strcmp(tokenString, "[")==0))
        block();
    else
        compilError(selector, "invalid expression start", tokenString);

    return (superTerm);
}

static void parsePrimitive()
{
    int primitiveNumber, argumentCount;

    if (nextToken() != intconst)
        compilError(selector, "primitive number missing", "");
    primitiveNumber = tokenInteger;
    nextToken();
    argumentCount = 0;
    while (parseok && !((token == binary) && (strcmp(tokenString, ">")==0)))
    {
        term();
        argumentCount++;
    }
    genInstruction(DoPrimitive, argumentCount);
    genCode(primitiveNumber);
    nextToken();
}

static void genMessage(boolean toSuper, int argumentCount, object messagesym)
{
    boolean sent = false;
    int i;

    if ((!toSuper) && (argumentCount == 0))
        for (i = 0; (!sent) && unSyms[i]; i++)
            if (messagesym == unSyms[i])
            {
                genInstruction(SendUnary, i);
                sent = true;
            }

    if ((!toSuper) && (argumentCount == 1))
        for (i = 0; (!sent) && binSyms[i]; i++)
            if (messagesym == binSyms[i])
            {
                genInstruction(SendBinary, i);
                sent = true;
            }

    if (!sent)
    {
        genInstruction(MarkArguments, 1 + argumentCount);
        if (toSuper)
        {
            genInstruction(DoSpecial, SendToSuper);
            genCode(genLiteral(messagesym));
        }
        else
            genInstruction(SendMessage, genLiteral(messagesym));
    }
}

static boolean unaryContinuation(boolean superReceiver)
{
    int i;
    boolean sent;

    while (parseok && (token == nameconst))
    {
        /* first check to see if it could be a temp by mistake */
        for (i = 1; i < temporaryTop; i++)
            if (strcmp(tokenString, temporaryName[i])==0)
                compilWarn(selector, "message same as temporary:", tokenString);
        for (i = 1; i < argumentTop; i++)
            if (strcmp(tokenString, argumentName[i])==0)
                compilWarn(selector, "message same as argument:", tokenString);
        /* the next generates too many spurious messages */
        /* for (i=1; i < instanceTop; i++)
         if (streq(tokenString, instanceName[i]))
         compilWarn(selector,"message same as instance",
         tokenString); */

        sent = false;

        if (!sent)
        {
            genMessage(superReceiver, 0, newSymbol(tokenString));
        }
        /* once a message is sent to super, reciever is not super */
        superReceiver = false;
        nextToken();
    }
    return (superReceiver);
}

static boolean binaryContinuation(boolean superReceiver)
{
    boolean superTerm;
    object messagesym;

    superReceiver = unaryContinuation(superReceiver);
    while (parseok && (token == binary))
    {
        messagesym = newSymbol(tokenString);
        nextToken();
        superTerm = term();
        unaryContinuation(superTerm);
        genMessage(superReceiver, 1, messagesym);
        superReceiver = false;
    }
    return (superReceiver);
}

static int optimizeBlock(int instruction, boolean dopop)
{
    int location;
    enum blockstatus savebstat;

    savebstat = blockstat;
    genInstruction(DoSpecial, instruction);
    location = codeTop;
    genCode(0);
    if (dopop)
        genInstruction(DoSpecial, PopTop);
    nextToken();
    if (strcmp(tokenString, "[")==0)
    {
        nextToken();
        if (blockstat == NotInBlock)
            blockstat = OptimizedBlock;
        body();
        if (!strcmp(tokenString, "]")==0)
            compilError(selector, "missing close", "after block");
        nextToken();
    }
    else
    {
        binaryContinuation(term());
        genMessage(false, 0, newSymbol("value"));
    }
    codeArray[location] = codeTop + 1;
    blockstat = savebstat;
    return (location);
}

static boolean keyContinuation(boolean superReceiver)
{
    int i, j, argumentCount;
    boolean sent, superTerm;
    object messagesym;
    char pattern[80];

    superReceiver = binaryContinuation(superReceiver);
    if (token == namecolon)
    {
        if (strcmp(tokenString, "ifTrue:")==0)
        {
            i = optimizeBlock(BranchIfFalse, false);
            if (strcmp(tokenString, "ifFalse:")==0)
            {
                codeArray[i] = codeTop + 3;
                optimizeBlock(Branch, true);
            }
        }
        else if (strcmp(tokenString, "ifFalse:")==0)
        {
            i = optimizeBlock(BranchIfTrue, false);
            if (strcmp(tokenString, "ifTrue:")==0)
            {
                codeArray[i] = codeTop + 3;
                optimizeBlock(Branch, true);
            }
        }
        else if (strcmp(tokenString, "whileTrue:")==0)
        {
            j = codeTop;
            genInstruction(DoSpecial, Duplicate);
            genMessage(false, 0, newSymbol("value"));
            i = optimizeBlock(BranchIfFalse, false);
            genInstruction(DoSpecial, PopTop);
            genInstruction(DoSpecial, Branch);
            genCode(j + 1);
            codeArray[i] = codeTop + 1;
            genInstruction(DoSpecial, PopTop);
        }
        else if (strcmp(tokenString, "and:")==0)
            optimizeBlock(AndBranch, false);
        else if (strcmp(tokenString, "or:")==0)
            optimizeBlock(OrBranch, false);
        else
        {
            pattern[0] = '\0';
            argumentCount = 0;
            while (parseok && (token == namecolon))
            {
                strcat(pattern, tokenString);
                argumentCount++;
                nextToken();
                superTerm = term();
                binaryContinuation(superTerm);
            }
            sent = false;

            /* check for predefined messages */
            messagesym = newSymbol(pattern);

            if (!sent)
            {
                genMessage(superReceiver, argumentCount, messagesym);
            }
        }
        superReceiver = false;
    }
    return (superReceiver);
}

static void continuation(boolean superReceiver)
{
    superReceiver = keyContinuation(superReceiver);

    while (parseok && (token == closing) && (strcmp(tokenString, ";")==0))
    {
        genInstruction(DoSpecial, Duplicate);
        nextToken();
        keyContinuation(superReceiver);
        genInstruction(DoSpecial, PopTop);
    }
}

static void expression()
{
    boolean superTerm;
    char assignname[60];

    if (token == nameconst)   /* possible assignment */
    {
        strcpy(assignname, tokenString);
        nextToken();
        if ((token == binary) && (strcmp(tokenString, "<-")==0))
        {
            nextToken();
            assignment(assignname);
        }
        else     /* not an assignment after all */
        {
            superTerm = nameTerm(assignname);
            continuation(superTerm);
        }
    }
    else
    {
        superTerm = term();
        if (parseok)
            continuation(superTerm);
    }
}

static void assignment(char *name)
{
    int i;
    boolean done;

    done = false;

    /* it might be a temporary */
    for (i = temporaryTop; (!done) && (i > 0); i--)
        if (strcmp(name, temporaryName[i])==0)
        {
            expression();
            genInstruction(AssignTemporary, i - 1);
            done = true;
        }

    /* or it might be an instance variable */
    for (i = 1; (!done) && (i <= instanceTop); i++)
        if (strcmp(name, instanceName[i])==0)
        {
            expression();
            genInstruction(AssignInstance, i - 1);
            done = true;
        }

    if (!done)   /* not known, handle at run time */
    {
        genInstruction(PushArgument, 0);
        genInstruction(PushLiteral, genLiteral(newSymbol(name)));
        expression();
        genMessage(false, 2, newSymbol("assign:value:"));
    }
}

static void statement()
{

    if ((token == binary) && (strcmp(tokenString, "^")==0))
    {
        nextToken();
        expression();
        if (blockstat == InBlock)
        {
            /* change return point before returning */
            genInstruction(PushConstant, contextConst);
            genMessage(false, 0, newSymbol("blockReturn"));
            genInstruction(DoSpecial, PopTop);
        }
        genInstruction(DoSpecial, StackReturn);
    }
    else
    {
        expression();
    }
}

static void body()
{
    /* empty blocks are same as nil */
    if ((blockstat == InBlock) || (blockstat == OptimizedBlock))
        if ((token == closing) && (strcmp(tokenString, "]")==0))
        {
            genInstruction(PushConstant, nilConst);
            return;
        }

    while (parseok)
    {
        statement();
        if (token == closing)
            if (strcmp(tokenString,".")==0)
            {
                nextToken();
                if (token == inputend)
                    break;
                else
                    /* pop result, go to next statement */
                    genInstruction(DoSpecial, PopTop);
            }
            else
                break; /* leaving result on stack */
        else if (token == inputend)
            break; /* leaving result on stack */
        else
        {
            compilError(selector, "invalid statement ending; token is ",
                             tokenString);
        }
    }
}

static void temporaries()
{
    object tempsym;

    temporaryTop = 0;
    if ((token == binary) && (strcmp(tokenString, "|")==0))
    {
        nextToken();
        while (token == nameconst)
        {
            if (++temporaryTop > maxTemporary)
                maxTemporary = temporaryTop;
            if (temporaryTop > temporaryLimit)
                compilError(selector, "too many temporaries in method", "");
            else
            {
                tempsym = newSymbol(tokenString);
                temporaryName[temporaryTop] = charPtr(tempsym);
            }
            nextToken();
        }
        if ((token != binary) || (!strcmp(tokenString, "|")==0))
            compilError(selector, "temporary list not terminated by bar", "");
        else
            nextToken();
    }
}

static void messagePattern()
{
    object argsym;

    argumentTop = 0;
    strcpy(selector, tokenString);
    if (token == nameconst) /* unary message pattern */
        nextToken();
    else if (token == binary)   /* binary message pattern */
    {
        nextToken();
        if (token != nameconst)
            compilError(selector,
                             "binary message pattern not followed by name", selector);
        argsym = newSymbol(tokenString);
        argumentName[++argumentTop] = charPtr(argsym);
        nextToken();
    }
    else if (token == namecolon)     /* keyword message pattern */
    {
        selector[0] = '\0';
        while (parseok && (token == namecolon))
        {
            strcat(selector, tokenString);
            nextToken();
            if (token != nameconst)
                compilError(selector, "keyword message pattern",
                                 "not followed by a name");
            if (++argumentTop > argumentLimit)
                compilError(selector, "too many arguments in method", "");
            argsym = newSymbol(tokenString);
            argumentName[argumentTop] = charPtr(argsym);
            nextToken();
        }
    }
    else
        compilError(selector, "illegal message selector", tokenString);
}

boolean parse(object method, const char *text, boolean savetext)
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
    if (parseok)
    {
        genInstruction(DoSpecial, PopTop);
        genInstruction(DoSpecial, SelfReturn);
    }

    if (!parseok)
    {
        basicAtPut(method, bytecodesInMethod, nilobj);
    }
    else
    {
        bytecodes = newByteArray(codeTop);
        bp = bytePtr(bytecodes);
        for (i = 0; i < codeTop; i++)
        {
            bp[i] = codeArray[i];
        }
        basicAtPut(method, messageInMethod, newSymbol(selector));
        basicAtPut(method, bytecodesInMethod, bytecodes);
        if (literalTop > 0)
        {
            theLiterals = newArray(literalTop);
            for (i = 1; i <= literalTop; i++)
            {
                basicAtPut(theLiterals, i, literalArray[i]);
                decr(literalArray[i]);
            }
            basicAtPut(method, literalsInMethod, theLiterals);
        }
        else
        {
            basicAtPut(method, literalsInMethod, nilobj);
        }
        basicAtPut(method, stackSizeInMethod, newInteger(6));
        basicAtPut(method, temporarySizeInMethod,
                   newInteger(1 + maxTemporary));
        if (savetext)
        {
            basicAtPut(method, textInMethod, newStString(text));
        }
        return (true);
    }
    return (false);
}
