/*
 * Global definitions for the LittleSmalltalk virtual machine
 *
 * ---------------------------------------------------------------
 * Little Smalltalk, Version 5
 *
 * Copyright (C) 1987-2005 by Timothy A. Budd
 * Copyright (C) 2007 by Charles R. Childers
 * Copyright (C) 2005-2007 by Danny Reinhold
 * Copyright (C) 2010 by Ketmar // Vampire Avalon
 *
 * ============================================================================
 * This license applies to the virtual machine and to the initial image of
 * the Little Smalltalk system and to all files in the Little Smalltalk
 * packages except the files explicitly licensed with another license(s).
 * ============================================================================
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef LST_GLOBS_H
#define LST_GLOBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stdint.h>
#include <stdio.h>


#define LST_IMAGE_VERSION    6
#define LST_IMAGE_SIGNATURE  "LST5 Image File\x1a\x00"

#define LST_VERSION_STRING   "Little Smalltalk version 5.2 (pre-beta)"


typedef uint32_t LstUInt;
typedef int32_t  LstInt;
typedef int64_t  LstLInt;
typedef double   LstFloat;


#if defined(__MINGW32__) || defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
# define LST_ON_WINDOWS 1
# include <io.h>
# define lstLNull 0L
# define mkstemp mktemp
# define lstOSName "Windows"
#else
# define lstLNull 0LL
# ifdef __linux__
#  define lstOSName "GNU/Linux"
# else
#  define lstOSName "Unix"
# endif
#endif


extern unsigned int lstDebugFlag; /* true if we are debugging */

extern unsigned int lstInfoCacheHit;
extern unsigned int lstInfoCacheMiss;
extern unsigned int lstInfoLiteralHit;
extern unsigned int lstInfoIVarHit;

extern unsigned int lstGCCount;


/* symbolic definitions for the bytecodes */
enum {
  lstBCExtended,        /* 0 */
  lstBCPushInstance,    /* 1 */
  lstBCPushArgument,    /* 2 */
  lstBCPushTemporary,   /* 3 */
  lstBCPushLiteral,     /* 4 */
  lstBCPushConstant,    /* 5 */
  lstBCAssignInstance,  /* 6 */
  lstBCAssignArgument,  /* 7 */
  lstBCAssignTemporary, /* 8 */
  lstBCMarkArguments,   /* 9 */
  lstBCPushBlock,       /* 10 */
  lstBCSendUnary,       /* 11 */
  lstBCSendBinary,      /* 12 */
  lstBCSendMessage,     /* 13 */
  lstBCDoPrimitive,     /* 14 */
  lstBCDoSpecial        /* 15 */
};


/* types of special instructions (opcode 15) */
enum {
  lstBXBreakpoint,     /* 0 */
  lstBXSelfReturn,     /* 1 */
  lstBXStackReturn,    /* 2 */
  lstBXBlockReturn,    /* 3 */
  lstBXDuplicate,      /* 4 */
  lstBXPopTop,         /* 5 */
  lstBXBranch,         /* 6 */
  lstBXBranchIfTrue,   /* 7 */
  lstBXBranchIfFalse,  /* 8 */
  lstBXBranchIfNil,    /* 9 */
  lstBXBranchIfNotNil, /* 10 */
  lstBXSendToSuper,    /* 11 */
  lstBXThisContext     /* 12 */
};


/* special constants */

/* constants 0 to 9 are the integers 0 to 9 */
enum {
  lstBLNilConst,   /* 0 */
  lstBLTrueConst,  /* 1 */
  lstBLFalseConst  /* 2 */
  /* n-3: SmallInt constant */
};


/* Return values from doExecute: primitive */
enum {
  lstReturnError = 2,   /* 2: error: called */
  lstReturnBadMethod,   /* 3: Unknown method for object */
  lstReturnReturned,    /* 4: Top level method returned */
  lstReturnTimeExpired, /* 5: Time quantum exhausted */
  lstReturnBreak,       /* 6: lstBXBreakpoint instruction */
  lstReturnYield,       /* 7: process yielded (can be continued) */
  lstReturnAPISuspended /* 8: suspended from C API; Smalltalk code will never see this */
};


/* The following are the objects with ``hard-wired'' field offsets */

/* A Process has two fields
 *   * a current context
 *   * status of process (running, waiting, etc)
 */
enum {
  lstIVcontextInProcess,
  lstIVstatusInProcess,
  lstIVresultInProcess,
  lstIVrunningInProcess,
  lstProcessSize
};


/*
 * A Context has:
 *   * method (which has bytecode pointer)
 *   * bytecode offset (an integer)
 *   * arguments
 *   * temporaries and stack
 *   * stack pointer
 */
enum {
  lstIVmethodInContext,
  lstIVargumentsInContext,
  lstIVtemporariesInContext,
  lstIVstackInContext,
  lstIVbytePointerInContext,
  lstIVstackTopInContext,
  lstIVpreviousContextInContext,
  lstIVprocOwnerInContext,
  lstContextSize
};


/*
 * A Block is subclassed from Context
 * shares fields with creator, plus a couple new ones
 */
enum {
  lstIVmethodInBlock = lstIVmethodInContext,
  lstIVargumentsInBlock = lstIVargumentsInContext,
  lstIVtemporariesInBlock = lstIVtemporariesInContext,
  lstIVstackInBlock = lstIVstackInContext,
  lstIVstackTopInBlock = lstIVstackTopInContext,
  lstIVpreviousContextInBlock = lstIVpreviousContextInContext,
  lstIVprocOwnerInBlock = lstIVprocOwnerInContext,
  lstIVargumentLocationInBlock = lstContextSize,
  lstIVcreatingContextInBlock,
  /* the following are saved in different place so they don't get clobbered */
  lstIVbytePointerInBlock,
  lstIVargCountInBlock,
  lstBlockSize
};


/*
 * A Method has:
 *   * name (a Symbol)
 *   * bytecodes
 *   * literals
 *   * stack size
 *   * temp size
 */
enum {
  lstIVnameInMethod,
  lstIVbyteCodesInMethod,
  lstIVliteralsInMethod,
  lstIVstackSizeInMethod,
  lstIVtemporarySizeInMethod,
  lstIVclassInMethod,
  lstIVtextInMethod,
  lstIVargCountInMethod,
  lstIVdebugInfoInMethod,
  lstIVargNamesInMethod,
  lstIVtempNamesInMethod,
  lstIVdbgDataInMethod,
  lstIVoptimDoneInMethod,
  lstIVretResInMethod,
  lstIVcategoryInMethod,
  lstIVinvokeCountInMethod,
  lstMethodSize
};


/*
 * A Class has:
 *   * pointer to parent class
 *   * pointer to tree of methods
 */
enum {
  lstIVnameInClass,
  lstIVparentClassInClass,
  lstIVmethodsInClass,
  lstIVinstanceSizeInClass,
  lstIVvariablesInClass,
  lstIVpackageInClass,
  lstClassSize
};


/* MetaSymbol */
enum {
  lstIVsymbolsInMetaSymbol = lstClassSize,
  lstMetaSymbolSize
};


/* MetaChar */
enum {
  lstIVcharsInMetaChar = lstClassSize,
  lstMetaCharSize
};


/* MetaSmall */
enum {
  lstIVseedInMetaSmallInt = lstClassSize,
  lstMetaSmallIntSize
};


/*
 * A Package has:
 *   * name (a Symbol)
 *   * version (SmallInt)
 *   * requiremets (list of PackageRequirement objects)
 */
enum {
  lstIVclassesInPackage, /* dictionary: symbol, class */
  lstIVnameInPackage, /* symbol */
  lstIVversionInPackage, /* smallint */
  lstIVrequiresInPackage, /* dictionary: symbol(pkgname), version(smallint)/nil(any) */
  lstPackageSize
};

enum {
  lstIVpackagesInMetaPackage = lstClassSize, /* dictionary: symbol, package instance */
  lstIVcurrentInMetaPackage,
  lstMetaPackageSize
};


/* PackageRequirement */
/*
enum {
  nameInPackageReq,
  versionInPackageReq,
  PackageReqSize
};
*/


/*
 * A node in a tree has
 *   * value field
 *   * left subtree
 *   * right subtree
*/
enum {
  lstIVvalueInNode,
  lstIVleftInNode,
  lstIVrightInNode,
  lstNodeSize
};


/* misc iv defines */
enum {
  lstIVrootInTree = 0,
  lstIVreceiverInArguments = 0,
};


/* object flags */
enum {
  LST_OFLAG_BIN       = 0x0001,
  LST_OFLAG_GC_BHEART = 0x0002,
  LST_OFLAG_TOFINALZE = 0x0010, /* this object should be kept and it's 'finalize' method should be called */
  LST_OFLAG_WEAK      = 0x0020, /* this object is weak */
  LST_OFLAG_PPONED    = 0x0100, /* object already marked for processing */
  LST_OFLAG_DUMMY /* nothing, just to keep the previous comma */
};

/*
 * The fundamental data type is the object.
 * The first field in an object is a size, the low order two
 *   bits being used to maintain:
 *     * binary flag, used if data is binary
 *     * indirection flag, used if object has been relocated
 * The next data field is the class
 *
 * the initial image is loaded into static memory space --
 * space which is never garbage collected;
 * this improves speed, as these items are not moved during GC
 */
#define LST_OBJECT_HEADER \
  LstUInt size; \
  lstObject *stclass; \
  LstFinLink *fin; \
  LstUInt objFlags; /* see LST_OFLAG_XXX */


typedef struct lstObject   lstObject;
typedef struct LstFinLink  LstFinLink;


typedef void (*LstFinalizerFn) (lstObject *o, void *udata);

struct LstFinLink {
  LstFinLink *prev;
  LstFinLink *next;
  lstObject *obj;
  LstFinalizerFn fin;
  void *udata;
};


struct lstObject {
  LST_OBJECT_HEADER
  lstObject *data[0];
} __attribute__((__packed__));


/*
 * byte objects are used to represent strings and symbols; bytes per word must be correct
 */
typedef struct lstByteObject lstByteObject;
struct lstByteObject {
  LST_OBJECT_HEADER
  unsigned char bytes[0];
} __attribute__((__packed__));


#define LST_BYTES_PER_WORD  (sizeof(lstObject *))

#define lstBytePtr(x)  (((lstByteObject *)(x))->bytes)


/*
 * SmallInt objects are used to represent short integers.
 * They are encoded as 31 bits, signed, with the low bit set to 1.
 * This distinguishes them from all other objects, which are longword
 * aligned and are proper C memory pointers.
 */
#if 0
# define LST_IS_SMALLINT(x)      ((((int)(x)) & 0x01) != 0)
# define LST_FITS_SMALLINT(x)    ((((int)(x)) >= INT_MIN/2) && (((int)(x)) <= INT_MAX/2))
# define LST_64FITS_SMALLINT(x)  (((x) >= INT_MIN/2) && ((x) <= INT_MAX/2))
#else
# define LST_IS_SMALLINT(x)      ((((intptr_t)(x)) & 0x01) != 0)
# define LST_FITS_SMALLINT(x)    ((((int)(x)) >= (-(__INT_MAX__))/2) && (((int)(x)) <= __INT_MAX__/2))
# define LST_64FITS_SMALLINT(x)  (((x) >= (-(__INT_MAX__))/2) && ((x) <= __INT_MAX__/2))
#endif
#define LST_CLASS(x)            (LST_IS_SMALLINT(x) ? lstSmallIntClass : ((x)->stclass))

/*
 * The "size" field is the top 30 bits; the bottom two are flags
 */
#define LST_SIZE(op)          ((op)->size)
#define LST_SETSIZE(op, val)  ((op)->size = (val))
#define LST_IS_BYTES(x)       ((x)->objFlags & LST_OFLAG_BIN)
#define LST_IS_MOVED(x)       ((x)->objFlags & LST_OFLAG_GC_BHEART)
#define LST_MARK_MOVED(x)     ((x)->objFlags |= LST_OFLAG_GC_BHEART)
#define LST_SETBIN(x)         ((x)->objFlags |= LST_OFLAG_BIN)
#define LST_IS_STFIN(x)       ((x)->objFlags & LST_OFLAG_TOFINALZE)
#define LST_SET_STFIN(x)      ((x)->objFlags |= LST_OFLAG_TOFINALZE)
#define LST_RESET_STFIN(x)    ((x)->objFlags &= ~LST_OFLAG_TOFINALZE)
#define LST_IS_WEAK(x)        ((x)->objFlags & LST_OFLAG_WEAK)
#define LST_SET_WEAK(x)       ((x)->objFlags |= LST_OFLAG_WEAK)
#define LST_RESET_WEAK(x)     ((x)->objFlags &= ~LST_OFLAG_WEAK)
#define LST_IS_PPONED(x)      ((x)->objFlags & LST_OFLAG_PPONED)
#define LST_SET_PPONED(x)     ((x)->objFlags |= LST_OFLAG_PPONED)
#define LST_RESET_PPONED(x)   ((x)->objFlags &= ~LST_OFLAG_PPONED)
static inline int LST_IS_BYTES_EX (lstObject *x) { return !LST_IS_SMALLINT(x) && LST_IS_BYTES(x); }


/*
 * roots for the memory space
 * these are traced down during memory management
 * lstRootStack is the dynamic stack
 * staticRoots are values in static memory that point to
 * dynamic values
 */
#define LST_ROOTSTACK_LIMIT  550
extern lstObject *lstRootStack[];
extern LstUInt lstRootTop;
extern void lstAddStaticRoot (lstObject **);


#define LST_MAX_BIN_MSG  14
/* The following are roots for the file out */
extern lstObject *lstNilObj;
extern lstObject *lstTrueObj;
extern lstObject *lstFalseObj;
extern lstObject *lstBooleanClass;
extern lstObject *lstSmallIntClass;
extern lstObject *lstCharClass;
extern lstObject *lstArrayClass;
extern lstObject *lstBlockClass;
extern lstObject *lstContextClass;
extern lstObject *lstProcessClass;
extern lstObject *lstStringClass;
extern lstObject *lstSymbolClass;
extern lstObject *lstByteArrayClass;
extern lstObject *lstByteCodeClass;
extern lstObject *lstMethodClass;
extern lstObject *lstGlobalObj;
extern lstObject *lstBinMsgs[LST_MAX_BIN_MSG]; /* '<'; '<='; '+', ... */
extern lstObject *lstIntegerClass;
extern lstObject *lstFloatClass;
extern lstObject *lstNumberClass;
extern lstObject *lstBadMethodSym; /* 'doesNotUnderstand:' */
/* Smalltalk entry points */
extern lstObject *lstInitMethod;
extern lstObject *lstLoadMethod;
extern lstObject *lstDoStrMethod;
extern lstObject *lstReplMethod;
extern lstObject *lstNewSymMethod;
extern lstObject *lstSetGlobMethod;


/* must be defined by the main program */
extern void lstFatal (const char *a, unsigned int b);
extern void lstBackTrace (lstObject *aContext);


/* entry points */
extern void lstMemoryInit (int staticsz, int dynamicsz);
extern void lstMemoryDeinit (void);

extern void lstCompleteFinalizers (void);

extern int lstReadImage (FILE *fp);
extern int lstWriteImage (FILE *fp, int noSrc);

extern void lstGC (void);
extern void lstShowCalledMethods (void);

extern lstObject *lstStaticAlloc (int sz);
extern lstByteObject *lstStaticAllocBin (int sz);
extern lstObject *lstMemAlloc (int sz);
extern lstByteObject *lstMemAllocBin (int sz);

/* internal functions for interpreter */
extern void lstSwapObjects (lstObject *array1, lstObject *array2, LstUInt size);
extern int lstBulkReplace (lstObject *dest, lstObject *start, lstObject *stop, lstObject *src, lstObject *repStart);

/* do write, register static root if necessary */
extern void lstWriteBarrier (lstObject **dest, lstObject *src);


typedef lstObject *(*LSTPrimitiveFn) (int primitiveNumber, lstObject **args, int argc); /* return NULL if failed (not lstNilObj!) */
typedef void (*LSTPrimitiveClearFn) (int primitiveNumber);

extern const char *lstFindPrimitiveName (int idx);
extern int lstFindPrimitiveIdx (const char *name); /* -1: no such primitive */

extern int lstFindExtPrimitiveByName (const char *name);
extern int lstRegisterExtPrimitive (const char *name, LSTPrimitiveFn pfn, LSTPrimitiveClearFn cfn); /* -1: error; other: primitive index */
extern void lstClearExtPrimitives (void);

extern LSTPrimitiveFn lstFindExtPrimitiveFn (int idx);

extern void lstPrimitivesClear (void);


typedef struct {
  const char *name;
  LSTPrimitiveFn pfn;
  LSTPrimitiveClearFn cfn;
} LSTExtPrimitiveTable;

extern int lstRegisterExtPrimitiveTable (const LSTExtPrimitiveTable *tbl); /* return success flag */



#define lstIntValue(x)  ((int)(((intptr_t)(x)) >> 1))
#define lstNewInt(x)    ((lstObject *)((((intptr_t)(x)) << 1) | 0x01))

extern lstObject *lstNewLongInt (LstLInt val); /* create new Integer (64-bit) */
extern lstObject *lstNewFloat (LstFloat val);

#define lstNewInteger(x)  (LST_64FITS_SMALLINT(x) ? lstNewInt(x) : lstNewLongInt(x))

static inline LstLInt lstLIntValue (const lstObject *o) {
  const LstLInt *v = (const LstLInt *)o->data;
  return *v;
}
static inline LstFloat lstFloatValue (const lstObject *o) {
  const LstFloat *v = (const LstFloat *)o->data;
  return *v;
}
/*
#define lstLIntValue(o)   (*((LstLInt *)lstConstPtr(o)))
#define lstFloatValue(o)  (*((LstFloat *)lstConstPtr(o)))
*/

extern lstObject *lstNewChar (int ch);
extern lstObject *lstNewString (const char *str);
extern int lstGetString (char *buf, int bufsize, const lstObject *str); /* return 0 on ok or necessary buffer size */
extern char *lstGetStringPtr (const lstObject *str);
extern lstObject *lstNewBinary (const void *p, LstUInt l);
extern lstObject *lstNewBCode (const void *p, LstUInt l);
extern lstObject *lstNewArray (int size);

extern lstObject *lstAllocInstance (int size, lstObject *cl);

extern lstObject *lstDictFind (const lstObject *dict, const char *name);

/* WARNING: this works only when obj is instance of something, and aclass is not! */
extern int lstIsKindOf (const lstObject *obj, const lstObject *aclass); /* returns true(!0) or false(0) */

extern lstObject *lstFindGlobal (const char *name);

/* WARNING: CAN'T BE CALLED FROM PRIMITIVE! */
extern lstObject *lstNewSymbol (const char *name);

/* WARNING: CAN'T BE CALLED FROM PRIMITIVE! */
extern int lstSetGlobal (const char *name, lstObject *val);

extern lstObject *lstFindMethod (lstObject *stclass, const char *method);

extern void lstSetFinalizer (lstObject *o, LstFinalizerFn fn, void *udata);
extern void *lstGetUData (lstObject *o);

/* both returns lstReturnXXX */
extern int lstExecute (lstObject *aProcess, int ticks, int locked);
extern int lstResume (void);
extern int lstCanResume (void);
extern void lstResetResume (void);

/* arg==NULL: no argument
 * result==NULL: don't need result
 * remember, that result can be collected by GC on next allocation
 */
extern int lstRunMethodWithArg (lstObject *method, lstObject *inClass, lstObject *arg, lstObject **result, int locked);


/* types in image file */
enum {
  LST_IMGOT_NULL,
  LST_IMGOT_OBJECT,
  LST_IMGOT_SMALLINT,
  LST_IMGOT_BCODE,
  LST_IMGOT_BARRAY,
  LST_IMGOT_REF,
  LST_IMGOT_NIL,
  LST_IMGOT_WEAK,
  LST_IMGOT_MAX
};

extern void lstFlushMethodCache (void);


extern unsigned int lstImgReadWord (FILE *fp);
extern void lstImgWriteWord (FILE *fp, int i);

extern int32_t lstImgReadDWord (FILE *fp);
extern void lstImgWriteDWord (FILE *fp, int32_t i);


#define LST_FINALIZER(name) \
static void name (lstObject *obj, void *udata)

/* macro for primitives */
#define LST_PRIMFN(name) \
static lstObject *name (int primitiveNumber, lstObject **args, int argc)

#define LST_PRIMFN_NONSTATIC(name) \
lstObject *name (int primitiveNumber, lstObject **args, int argc)

#define LST_PRIMCLEARFN(name) \
static void name (int primitiveNumber)

#define LST_PRIMARGC    (argc)
#define LST_PRIMARG(n)  (args[n])


static inline void lstRootPush (lstObject *o) {
  if (lstRootTop >= LST_ROOTSTACK_LIMIT) lstFatal("out of root stack", 0);
  lstRootStack[lstRootTop++] = o;
}


static inline lstObject *lstRootPop (void) {
  if (lstRootTop < 1) lstFatal("root stack underflow", 0);
  return lstRootStack[--lstRootTop];
}


#define LST_TS_LIMIT  512
extern lstObject **lstTempStack[LST_TS_LIMIT];
extern int lstTempSP;

#define LST_ENTER_BLOCK()  int _lstCurTSP_ = lstTempSP
#define LST_LEAVE_BLOCK()  lstTempSP = _lstCurTSP_

#define LST_NEW_TEMP(name)  lstObject *name = NULL; lstTempStack[lstTempSP++] = &name
#define LST_TEMP(name)      lstTempStack[lstTempSP++] = &name


typedef int (*LstEventCheckFn) (int *ticksLeft);
/* this must return eid (!0) to call callback */
extern LstEventCheckFn lstEventCheck;

extern int lstEvtCheckLeft; /* # of executed instructions before event check */
extern void lstResetEvtCheckLeft (void); /* to be called from primitive */

extern int lstExecUserBreak; /* set this to non-zero to suspend current lstExecute() and return lstReturnAPISuspended */


#ifdef DEBUG
extern lstObject *lstPrimCtx;
#endif


#ifdef __cplusplus
}
#endif
#endif
