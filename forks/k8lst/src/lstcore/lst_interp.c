/*
 * The LittleSmalltalk byte code interpreter
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

/*
 * bytecode interpreter module
 *
 * given a process object, execute bytecodes in a tight loop.
 *
 * performs subroutine calls for
 *   a) garbage collection
 *   b) finding a non-cached method
 *   c) executing a primitive
 *   d) creating an integer
 *
 * otherwise simply loops until time slice has ended
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "k8lst.h"
#include "primlib/lst_primitives.h"


#ifdef DEBUG
lstObject *lstPrimCtx = NULL;
#endif


/*
#define COLLECT_METHOD_STATISTICS
*/

#define MARKARG_INLINER_CHECK
#define INLINER_ACTIVE

#define INLINE_SOME_METHODS

/*
#define DEBUG_INLINER
*/


// windoze msvcrt.dll is idiotic
#ifndef _WIN32
# ifdef __LP64__
#  define PRINTF_LLD  "%ld"
# else
#  define PRINTF_LLD  "%lld"
# endif
#else
# define PRINTF_LLD  "%I64d"
#endif

#define BETTER_CACHE_CONTROL

LstEventCheckFn lstEventCheck = NULL;

int lstExecUserBreak = 0;

unsigned int lstDebugFlag = 0;

unsigned int lstInfoCacheHit = 0;
unsigned int lstInfoCacheMiss = 0;

unsigned int lstInfoLiteralHit = 0;
unsigned int lstInfoIVarHit = 0;

static int lstSuspended = 0;


#define FNV_PRIME         16777619
#define FNV_OFFSET_BASIS  2166136261uL
#define SIZE_TH           256

static uint32_t fnvHash (const void *str, int slen) {
  const unsigned char *buf = (const unsigned char *)str;
  uint32_t acc = FNV_OFFSET_BASIS;
  if (slen > SIZE_TH) {
    int left = slen-SIZE_TH;
    if (left > SIZE_TH) left = SIZE_TH;
    const unsigned char *buf1 = (const unsigned char *)str;
    buf1 = buf1+slen-left;
    while (slen-- > 0) { acc *= FNV_PRIME; acc ^= *buf++; }
    while (left-- > 0) { acc *= FNV_PRIME; acc ^= *buf1++; }
  } else {
    while (slen-- > 0) { acc *= FNV_PRIME; acc ^= *buf++; }
  }
  return acc;
}


static inline int LST_RSTACK_NSP (void) {
  if (lstRootTop >= LST_ROOTSTACK_LIMIT) lstFatal("out of root stack", 0);
  return lstRootTop++;
}


/* The following are roots for the file out */
lstObject *lstNilObj = NULL;
lstObject *lstTrueObj = NULL;
lstObject *lstFalseObj = NULL;
lstObject *lstBooleanClass = NULL;
lstObject *lstSmallIntClass = NULL;
lstObject *lstCharClass = NULL;
lstObject *lstArrayClass = NULL;
lstObject *lstBlockClass = NULL;
lstObject *lstContextClass = NULL;
lstObject *lstProcessClass = NULL;
lstObject *lstStringClass = NULL;
lstObject *lstSymbolClass = NULL;
lstObject *lstByteArrayClass = NULL;
lstObject *lstByteCodeClass = NULL;
lstObject *lstMethodClass = NULL;
lstObject *lstGlobalObj = NULL;
lstObject *lstBinMsgs[LST_MAX_BIN_MSG] = { NULL };
lstObject *lstIntegerClass = NULL;
lstObject *lstFloatClass = NULL;
lstObject *lstNumberClass = NULL;
lstObject *lstBadMethodSym = NULL;
lstObject *lstInitMethod = NULL;
lstObject *lstLoadMethod = NULL;
lstObject *lstDoStrMethod = NULL;
lstObject *lstReplMethod = NULL;
lstObject *lstNewSymMethod = NULL;
lstObject *lstSetGlobMethod = NULL;


#ifdef INLINE_SOME_METHODS
static lstObject *lstMetaCharClass = NULL;

static lstObject *lstArrayAtMethod = NULL;
static lstObject *lstArraySizeMethod = NULL;
static lstObject *lstMetaCharNewMethod = NULL;
static lstObject *lstStringAtIfAbsentMethod = NULL;
static lstObject *lstStringAtMethod = NULL;
static lstObject *lstStringBasicAtPutMethod = NULL;
static lstObject *lstStringPrintStringMethod = NULL;
static lstObject *lstSymbolPrintStringMethod = NULL;
static lstObject *lstBlockValue1Method = NULL;

static struct {
  int argc;
  const char *name;
  lstObject **mtclass;
  lstObject **method;
} lstInlineMethodList[] = {
  {2, "at:", &lstArrayClass, &lstArrayAtMethod},
  {1, "size", &lstArrayClass, &lstArraySizeMethod},
  {2, "at:", &lstStringClass, &lstStringAtMethod},
  {1, "printString", &lstStringClass, &lstStringPrintStringMethod},
  {1, "printString", &lstSymbolClass, &lstSymbolPrintStringMethod},
  {3, "basicAt:put:", &lstStringClass, &lstStringBasicAtPutMethod},
  {2, "new:", &lstMetaCharClass, &lstMetaCharNewMethod},
  {3, "at:ifAbsent:", &lstStringClass, &lstStringAtIfAbsentMethod},
  {2, "value:", &lstBlockClass, &lstBlockValue1Method},
  {0}
};
#endif


#define DBGCHAN  stderr

/*
 * Debugging
 */
#if defined(DEBUG)
static void indent (lstObject *ctx) {
  static int oldlev = 0;
  int lev = 0;
  while (ctx && (ctx != lstNilObj)) {
    ++lev;
    fputc(' ', DBGCHAN);
    ctx = ctx->data[lstIVpreviousContextInContext];
  }
  /* this lets you use your editor's brace matching to match up opening and closing indentation levels */
  /*
  if (lev < oldlev) {
    int x;
    for (x = lev; x < oldlev; ++x) fputc('}', DBGCHAN);
  } else if (lev > oldlev) {
    int x;
    for (x = oldlev; x < lev; ++x) fputc('{', DBGCHAN);
  }
  */
  oldlev = lev;
}


# define PC  (curIP-1)
# define DBG0(msg) if (lstDebugFlag) {indent(context); fprintf(DBGCHAN, "%d: %s\n", PC, msg);}
# define DBG1(msg, arg) if (lstDebugFlag) {indent(context); fprintf(DBGCHAN, "%d: %s %d\n", PC, msg, arg);}
# define DBG2(msg, arg, arg1) if (lstDebugFlag) {indent(context); fprintf(DBGCHAN, "%d: %s %d %d\n", PC, msg, arg, arg1);}
# define DBGS(msg, cl, sel) \
   if (lstDebugFlag) { \
     indent(context); \
      char clnm[1024], selnm[1024]; \
      lstGetString(clnm, sizeof(clnm), (lstObject *) cl); \
      lstGetString(selnm, sizeof(selnm), (lstObject *) sel); \
     fprintf(DBGCHAN, "%d: %s %s %s\n", PC, msg, clnm, selnm); }
#else
# define DBG0(msg)
# define DBG1(msg, arg)
# define DBG2(msg, arg, arg1)
# define DBGS(msg, cl, sel)
#endif


#ifdef DEBUG
# define dprintf(...)  fprintf(DBGCHAN, __VA_ARGS__)
#else
# define dprintf(...)
#endif

#ifdef DEBUG_INLINER
# define iprintf(...)  fprintf(DBGCHAN, __VA_ARGS__)
#else
# define iprintf(...)
#endif


static int symbolcomp (lstObject *left, lstObject *right) {
  int leftsize = LST_SIZE(left);
  int rightsize = LST_SIZE(right);
  int minsize = leftsize;
  int i;
  if (rightsize < minsize) minsize = rightsize;
  /* use faster comparison */
  if (minsize > 0) {
    if ((i = memcmp(lstBytePtr(left), lstBytePtr(right), minsize))) return i;
  }
  return leftsize-rightsize;
}


/* method lookup routine, used when cache miss occurs */
static lstObject *lookupMethod (lstObject *selector, lstObject *stclass) {
  lstObject *dict, *keys, *vals, *val;
  LstInt low, high, mid;
  /* scan upward through the class hierarchy */
  for (; stclass && stclass != lstNilObj; stclass = stclass->data[lstIVparentClassInClass]) {
    /* consider the Dictionary of methods for this Class */
#if 0 & defined(DEBUG)
    {
      static char tb[1024];
      fprintf(stderr, "st=%p; u=%p; sz=%d\n", stclass, lstNilObj, LST_SIZE(stclass));
      lstGetString(tb, sizeof(tb), stclass->data[lstIVnameInClass]);
      fprintf(stderr, "  [%s]\n", tb);
    }
#endif
#if !defined(NDEBUG) || defined(DEBUG)
    if (LST_IS_SMALLINT(stclass)) lstFatal("lookupMethod: looking in SmallInt instance", 0);
    if (LST_IS_BYTES(stclass)) lstFatal("lookupMethod: looking in binary object", 0);
    if (LST_SIZE(stclass) < lstClassSize) lstFatal("lookupMethod: looking in non-class object", 0);
#endif
    dict = stclass->data[lstIVmethodsInClass];
#if !defined(NDEBUG) || defined(DEBUG)
#define lstFatalX(msg, dummy) {\
  fprintf(stderr, "ERROR: %s\n", msg);\
  fprintf(stderr, "class: %s\n", lstGetStringPtr(stclass->data[lstIVnameInClass]));\
  if (dict && !LST_IS_SMALLINT(dict)) {\
    while (dict && dict != lstNilObj) {\
      if (LST_IS_SMALLINT(dict)) { fprintf(stderr, "dict: number\n"); break; }\
      if (LST_SIZE(dict) < lstClassSize) { fprintf(stderr, "dict: instance (%p)\n", dict); }\
      else {\
        fprintf(stderr, "dict: %s\n", lstGetStringPtr(dict->data[lstIVnameInClass]));\
        break;\
      }\
      dict = dict->stclass;\
    }\
  }\
  /* *((char *)0) = 0; */\
  lstFatal(msg, dummy);\
}
    if (!dict) lstFatalX("lookupMethod: NULL dictionary", 0);
    if (LST_IS_SMALLINT(dict)) lstFatalX("lookupMethod: SmallInt dictionary", 0);
    if (dict->stclass != lstFindGlobal("Dictionary")) lstFatalX("lookupMethod: method list is not a dictionary", 0);
#endif
    keys = dict->data[0];
    low = 0;
    high = LST_SIZE(keys);
    /* do a binary search through its keys, which are Symbol's. */
    while (low < high) {
      mid = (low+high)/2;
      val = keys->data[mid];
      /* if we find the selector, return the method lstObject. */
      if (val == selector) {
        vals = dict->data[1];
        return vals->data[mid];
      }
      /* otherwise continue the binary search */
      if (symbolcomp(selector, val) < 0) high = mid; else low = mid+1;
    }
  }
  /* sorry, couldn't find a method */
  return NULL;
}


/* method cache for speeding method lookup */
/* why 703? we have two primes: 701, 709, 719; let's try 719 */
#define MTD_CACHE_SIZE   719
#define MTD_CACHE_EXTRA  4
#define MTD_BAD_HIT_MAX  16
static struct {
  lstObject *name;
  lstObject *stclass;
  lstObject *method;
  int badHits; /* after MTD_BAD_HIT_MAX this cache item will be cleared */
  int goodHits;
  int analyzed;
  lstObject *mConst; /* constant for methods returning constant */
  int ivarNum; /* ivar number for methods returning ivar */
} cache[MTD_CACHE_SIZE+MTD_CACHE_EXTRA];


/* flush dynamic methods when GC occurs */
void lstFlushMethodCache (void) {
  memset(cache, 0, sizeof(cache));
}


/* run contexts */
typedef struct LstRunContext LstRunContext;
struct LstRunContext {
  /* ticks and locks fields will be filled only on process suspension */
  int ticksLeft;
  int lockCount;
  lstObject *process;
  LstRunContext *prev; /* previous process in group */
};

typedef struct LstRunGroup LstRunGroup;
struct LstRunGroup {
  LstRunGroup *prev; /* prev group */
  LstRunGroup *next; /* next group */
  LstRunContext *group; /* next group */
  int ticks; /* for the whole group; used on sheduling */
  int ewait; /* >0: normal process waiting for the event */
  int finalizer;
};

static LstRunContext *rsFree = NULL; /*TODO: free when too many*/
static LstRunGroup *runGroups = NULL; /* list of all process groups */
static LstRunGroup *curGroup = NULL; /* current run group */
static int finGroupCount = 0;
static int runOnlyFins = 0;

/* allocate new run context in the current group */
static LstRunContext *allocRunContext (void) {
  LstRunContext *res = rsFree;
  if (res) {
    rsFree = res->prev;
  } else {
    res = calloc(1, sizeof(LstRunContext));
  }
  res->prev = curGroup->group;
  curGroup->group = res;
  return res;
}


/* release top context in the current group; return previous one */
static LstRunContext *releaseRunContext (void) {
  LstRunContext *c = curGroup->group;
  if (c) {
    curGroup->group = c->prev;
    c->prev = rsFree;
    rsFree = c;
  }
  return curGroup->group;
}


static void lstCreateFinalizePGroup (lstObject *prc) {
  LstRunGroup *g = calloc(1, sizeof(LstRunGroup)), *p = curGroup?curGroup:runGroups;
  LstRunContext *c = calloc(1, sizeof(LstRunContext));
  g->prev = p;
  g->next = p->next;
  p->next = g; /* can't be first group anyway */
  if (g->next) g->next->prev = g;
  g->group = c;
  /* note that we can't allocate objects here, 'cause this thing will be called from inside GC */
  c->ticksLeft = 10000;
  c->process = prc;
  g->finalizer = 1;
  ++finGroupCount;
}


/*
 * note that process locks locks all groups now;
 * this MUST be changed: we have to use fine-grained locks,
 * mutexes and other cool things
 */

/* events */
typedef struct LstEventHandler LstEventHandler;
struct LstEventHandler {
  LstEventHandler *next;
  /*lstObject *process;*/
  LstRunGroup *grp;
  int eid;
};
static LstEventHandler *ehList = NULL;


static LstRunGroup *findEventHandler (int eid) {
  LstEventHandler *cur, *prev;
  for (cur = ehList, prev = NULL; cur; prev = cur, cur = cur->next) {
    if (cur->eid == eid) {
      LstRunGroup *grp = cur->grp;
      /* remove from the list */
      if (prev) prev->next = cur->next; else ehList = cur->next;
      free(cur);
      return grp;
    }
  }
  return NULL;
}


static void addOneShotEventHandler (int eid, LstRunGroup *grp) {
  LstEventHandler *cur = calloc(1, sizeof(LstEventHandler));
  cur->eid = eid;
  cur->next = ehList;
  ehList = cur;
  cur->grp = grp;
}


#include "lst_memory.c"


static int groupHasProcess (const LstRunGroup *g, const lstObject *prc) {
  const LstRunContext *c;
  for (c = g->group; c; c = c->prev) if (c->process == prc) return 1;
  return 0;
}


#define CHECK_MSTACK

#ifdef CHECK_MSTACK
# define POPIT      (stack->data[--stackTop])
# define PUSHIT(n)  if (stackTop >= LST_SIZE(stack)) { lstBackTrace(context); lstFatal("method stack overflow", curIP); } else stack->data[stackTop++] = (n)
#else
# define POPIT      (stack->data[--stackTop])
# define PUSHIT(n)  stack->data[stackTop++] = (n)
#endif


/* Code locations are extracted as VAL's */
#define VAL      (bp[curIP] | (bp[curIP+1] << 8))
/*#define VALSIZE  2*/


#define XRETURN(value)  { LST_LEAVE_BLOCK(); return (value); }

#define GET_BCODE_OP(ip) \
  low = (high = bp[ip++])&0x0F; high >>= 4; \
  if (high == lstBCExtended) { high = low; low = bp[ip++]; }


#define CALC_CACHE_HASH(sel, cls) \
  (LstUInt)((intptr_t)(sel)+(intptr_t)(cls))%MTD_CACHE_SIZE;

int lstEvtCheckLeft = 1000;

static int resetEvtCheckLeft = 0;
void lstResetEvtCheckLeft (void) { resetEvtCheckLeft = 1; }

static int lastFailedPrim = 0;
static int lastCalledPrim = 0;

static int lstExecuteInternal (lstObject *aProcess, int ticks, int locked) {
  int low, high;
  int stackTop;
  int curIP;
  lstObject *retValue = lstNilObj;
  lstObject *context = NULL;
  lstObject *method = NULL;
  lstObject *stack = NULL;
  lstObject *arguments = NULL;
  lstObject *temporaries = NULL;
  lstObject *instanceVariables = NULL;
  lstObject *literals = NULL;
  lstObject *ptemp = NULL;
  lstObject *ptemp1 = NULL;
  lstObject *messageSelector;
  lstObject *receiverClass;
  lstObject *op, *op1;
  int lockCount = locked>0;
  const unsigned char *bp;
  char sbuf[257];
  int tmp, l0, l1, x;
  int64_t itmp;
  LstLInt ll0, ll1;
  LstFloat fop0, fop1;
  int evtCheckLeft = lstEvtCheckLeft;
  int oTicks = curGroup->ticks;
  int wasRunInWaits = 1;
  int grpTicks = 10000;
  int retGSwitch = 0;

  /* reload all the necessary vars from the current context */
  void reloadFromCtx (void) {
    method = context->data[lstIVmethodInContext];
    stack = context->data[lstIVstackInContext];
    temporaries = context->data[lstIVtemporariesInContext];
    arguments = context->data[lstIVargumentsInContext];
    literals = method->data[lstIVliteralsInMethod];
    instanceVariables = arguments->data[lstIVreceiverInArguments];
    curIP = lstIntValue(context->data[lstIVbytePointerInContext]);
    stackTop = lstIntValue(context->data[lstIVstackTopInContext]);
  }

  /* reloca current group state */
  void reloadFromGroup (void) {
    LstRunContext *rc = curGroup->group; /* current context */
    aProcess = rc->process;
    ticks = rc->ticksLeft;
    lockCount = rc->lockCount;
    context = aProcess->data[lstIVcontextInProcess];
    reloadFromCtx();
    if (curGroup->ewait > 0) { lockCount = 0; evtCheckLeft = 1; } /* force event query */
  }

  /* load new process to the current group */
  int loadNewProcess (lstObject *newProc) {
    if (!newProc || newProc == lstNilObj) return lstReturnError;
    if (newProc->data[lstIVrunningInProcess] != lstNilObj) return lstReturnError; /* already running/suspended */
    /* get current context information */
    context = newProc->data[lstIVcontextInProcess];
    if (!context || context == lstNilObj) return lstReturnError; /* terminated */
    method = context->data[lstIVmethodInContext];
    if (!method || method == lstNilObj) return lstReturnError; /* the thing that should not be */
    aProcess = newProc;
    reloadFromCtx();
    newProc->data[lstIVrunningInProcess] = lstTrueObj;
    /* now create new runnint context */
    LstRunContext *rc = allocRunContext();
    rc->process = newProc;
    rc->lockCount = lockCount;
    rc->ticksLeft = ticks;
    return 0;
  }

  /* fix process and context info */
  void saveCurrentProcess (void) {
    if (!curGroup->group) return;
    if (curGroup->ewait <= 0) {
      aProcess->data[lstIVresultInProcess] = lstNilObj;
      aProcess->data[lstIVcontextInProcess] = context;
      if (context != lstNilObj) {
        context->data[lstIVbytePointerInContext] = lstNewInt(curIP);
        context->data[lstIVstackTopInContext] = lstNewInt(stackTop);
        LstRunContext *rc = curGroup->group; /* current context */
        rc->ticksLeft = ticks;
        rc->lockCount = lockCount;
      }
    }
  }

  /* switch to next group and reload it */
  void nextGroup (int skipIt) {
    int f;
    if (skipIt && curGroup) { saveCurrentProcess(); curGroup = curGroup->next; }
    if (!curGroup) curGroup = runGroups;
    grpTicks = 10000;
    if (runOnlyFins) {
      dprintf("rof: cg=%p\n", curGroup);
      for (f = 2; f > 0; --f) {
        do {
          while (curGroup && !curGroup->group) curGroup = curGroup->next;
        } while (curGroup && !curGroup->finalizer);
        if (curGroup) break;
        curGroup = runGroups;
      }
      if (!curGroup) lstFatal("internal error in finalizing stage", 0);
      dprintf("rof: found cg=%p\n", curGroup);
      dprintf("rof: ctx=%p\n", curGroup->group);
      dprintf("rof: fin=%d\n", curGroup->finalizer);
      dprintf("rof: fin left=%d\n", finGroupCount);
      if (curGroup && !curGroup->group) {
        if (finGroupCount > 0) lstFatal("internal error in finalizing stage", 1);
        return;
      }
    } else {
      for (f = 2; f > 0; --f) {
        while (curGroup && curGroup->ewait) curGroup = curGroup->next;
        if (curGroup) break;
        curGroup = runGroups;
      }
      if (!curGroup) curGroup = runGroups;
    }
    reloadFromGroup();
  }

  /* curGroup can be NULL after returning */
  /* result==NULL: trying to kill main group */
  LstRunGroup *removeCurrentGroup (void) {
    if (curGroup == runGroups) return NULL;
    /* exclude from the list */
    curGroup->prev->next = curGroup->next; /* it's safe, 'cause we can't remove the first (main) group */
    if (curGroup->next) curGroup->next->prev = curGroup->prev;
    LstRunGroup *pg = curGroup;
    if (!(curGroup = curGroup->next)) curGroup = runGroups;
    return pg;
  }

  /* return from process */
  /* on return: low is the result; tmp!=0: switched to suspended context */
  int doReturn (int res) {
    retGSwitch = 0;
    saveCurrentProcess();
    LstRunContext *rc = curGroup->group; /* current context */
    /*saveCurrentProcess();*/
    low = res; tmp = 0;
    aProcess->data[lstIVrunningInProcess] = lstNilObj;
    aProcess->data[lstIVresultInProcess] = retValue;
    if (res == lstReturnReturned) aProcess->data[lstIVcontextInProcess] = lstNilObj;
    if ((rc = releaseRunContext())) {
      /* still marching */
      aProcess = rc->process;
      reloadFromGroup();
      tmp = (curGroup->ewait != 0);
      return 0; /* ok, the show must go on */
    }
    /* group is out of bussines now; exit if this is the main group */
    if (curGroup == runGroups) {
      /* 'main group': so get out of here */
      runGroups->ticks = oTicks;
      return res;
    }
    /* remove empty group */
    if (curGroup->finalizer) --finGroupCount;
    retGSwitch = 1;
    LstRunGroup *pg = removeCurrentGroup();
    free(pg);
    nextGroup(0);
    tmp = (curGroup->ewait != 0);
#ifdef DEBUG
    dprintf("return-switched from %p to %p\n", pg, curGroup);
    if (lstDebugFlag) {
      dprintf("ctx=%p; mth=%p; ip=%d; tmp=%d\n", context, method, curIP, tmp);
    }
#endif
    return 0; /* don't stop at the top */
  }


  lstExecUserBreak = 0;

  assert(runGroups->group == NULL);
  if (runOnlyFins) {
    lstSuspended = 0;
    curGroup = NULL;
    nextGroup(0);
  } else {
    if (lstSuspended) {
      lstSuspended = 0;
      reloadFromGroup();
    } else {
      curGroup = runGroups; /* switch to 'main' */
      runGroups->ticks = ticks;
      if (loadNewProcess(aProcess) != 0) {
        releaseRunContext(); /* drop dummy context */
        curGroup = NULL; /* restore old group */
        return lstReturnError; /* barf */
      }
    }
  }

  LST_ENTER_BLOCK();
  LST_TEMP(aProcess);
  LST_TEMP(context);
  LST_TEMP(method);
  LST_TEMP(stack);
  LST_TEMP(arguments);
  LST_TEMP(temporaries);
  LST_TEMP(instanceVariables);
  LST_TEMP(literals);
  LST_TEMP(ptemp);
  LST_TEMP(ptemp1);

  /* main loop */
  for (;;) {
doAllAgain:
    if (curGroup->ewait < 0) {
      /* new waiting process */
      saveCurrentProcess();
      curGroup->ewait = -curGroup->ewait;
      /*dprintf("%p: suspend for %d: ip=%d; sp=%d\n", curGroup, curGroup->ewait, curIP, stackTop);*/
      evtCheckLeft = 1; lockCount = 0;
    }
    /* gum solution */
    if (runOnlyFins) {
      if (finGroupCount < 1) {
        runOnlyFins = finGroupCount = 0;
        XRETURN(lstReturnAPISuspended);
      }
      if (!curGroup->finalizer) {
        nextGroup(1);
        goto doAllAgain;
      }
      if (curGroup->ewait > 0) {
        curGroup->finalizer = 0;
        --finGroupCount;
        nextGroup(1);
        goto doAllAgain;
      }
      goto skipForFin;
    }
    if (evtCheckLeft > 0 && (--evtCheckLeft == 0)) {
      evtCheckLeft = lstEvtCheckLeft;
      if (lstExecUserBreak) {
        /* C API break; get out of here */
        saveCurrentProcess();
        lstSuspended = 1;
#ifdef DEBUG
        fprintf(stderr, "FUCK! SUSPEND!\n");
        if (curGroup == runGroups) fprintf(stderr, "SUSPEND IN MAIN GROUP!\n");
#endif
        XRETURN(lstReturnAPISuspended);
      }
      if (lstEventCheck) {
        int id;
        if ((id = lstEventCheck(&ticks)) > 0) {
          LstRunGroup *grp = findEventHandler(id);
          if (grp) {
            /* save current process */
            if (curGroup->ewait == 0) saveCurrentProcess();
            /* wake up suspended process */
            /*dprintf("found process group for %d\n", id);*/
            /* switch to this context */
            assert(grp->ewait == id);
            grp->ewait = 0; /* not waiting anymore */
            curGroup = grp;
            reloadFromGroup();
            /*dprintf("%p: resume: ip=%d; sp=%d\n", curGroup, curIP, stackTop);*/
            goto doAllAgain; /* continue with the next bytecode */
          }
        }
      }
      /* other shedulers */
      if (curGroup->ewait == 0) {
        /* process group sheduling */
        if (grpTicks > 0 && (--grpTicks == 0)) {
          grpTicks = 10000;
          if (runGroups->next) {
            dprintf("GRPSHEDULE!\n");
            LstRunGroup *og = curGroup;
            nextGroup(1);
            if (og != curGroup) goto doAllAgain; /* go on with the new process */
          }
        }
        /* if we're running against a CPU tick count, shedule execution when we expire the given number of ticks */
        if (ticks > 0 && (--ticks == 0)) {
          if (lockCount) {
            /* locked; no sheduling */
            ticks = 1; /* this will slow down the process, but locks shouldn't be held for the long time */
          } else {
            dprintf("TimeExpired: lockCount=%d\n", lockCount);
            int rr = doReturn(lstReturnTimeExpired);
            if (rr) XRETURN(rr);
            if (tmp || retGSwitch) goto doAllAgain;
            goto execComplete;
          }
        }
      }
    }
    if (curGroup->ewait > 0) {
      /* this process is in the wait state */
      /*dprintf("process are waiting for: %d\n", curGroup->ewait);*/
      LstRunGroup *og = curGroup;
      nextGroup(1);
#ifdef DEBUG
      if (og != curGroup) dprintf("switched from %p to %p\n", og, curGroup);
#endif
      if (og == curGroup || !wasRunInWaits) {
        /*dprintf("  releasing time slice\n");*/
        usleep(1); /* release timeslice */
      }
      wasRunInWaits = 0;
      goto doAllAgain;
    }

skipForFin:
    wasRunInWaits = 1;
    /* decode the instruction */
    bp = (const unsigned char *)lstBytePtr(method->data[lstIVbyteCodesInMethod]);
    GET_BCODE_OP(curIP)
    /* and dispatch */
    switch (high) {
      case lstBCPushInstance:
        DBG1("PushInstance", low);
        PUSHIT(instanceVariables->data[low]);
        break;
      case lstBCPushArgument:
        DBG1("PushArgument", low);
        PUSHIT(arguments->data[low]);
        break;
      case lstBCPushTemporary:
        DBG1("PushTemporary", low);
        PUSHIT(temporaries->data[low]);
        break;
      case lstBCPushLiteral:
        DBG1("PushLiteral", low);
        PUSHIT(literals->data[low]);
        break;
      case lstBCPushConstant:
        switch (low) {
          case lstBLNilConst:
            DBG0("PushConstant nil");
            PUSHIT(lstNilObj);
            break;
          case lstBLTrueConst:
            DBG0("PushConstant true");
            PUSHIT(lstTrueObj);
            break;
          case lstBLFalseConst:
            DBG0("PushConstant false");
            PUSHIT(lstFalseObj);
            break;
          default:
            low -= 3;
            DBG1("PushConstant", low);
            PUSHIT(lstNewInt(low));
            break;
        }
        break;
      case lstBCAssignInstance:
        DBG1("AssignInstance", low);
        /* don't pop stack, leave result there */
        lstWriteBarrier(&instanceVariables->data[low], stack->data[stackTop-1]);
        break;
      case lstBCAssignArgument:
        DBG1("AssignArgument", low);
        /* don't pop stack, leave result there */
        arguments->data[low] = stack->data[stackTop-1];
        break;
      case lstBCAssignTemporary:
        DBG1("AssignTemporary", low);
        /* don't pop stack, leave result there */
        temporaries->data[low] = stack->data[stackTop-1];
        break;
      case lstBCMarkArguments:
        DBG1("MarkArguments", low);
#ifdef MARKARG_INLINER_CHECK
        if (ticks != 1 && low > 1 && low <= 3) {
          /* check if next opcode is SendMessage */
          switch (bp[curIP]/16) {
            case lstBCSendMessage:
              l0 = bp[curIP]%16;
              l1 = curIP+1;
checkForInline:
              messageSelector = literals->data[l0];
              receiverClass = stack->data[stackTop-low];
              /*iprintf("stackTop: %d; low: %d; rc: %p\n", stackTop, low, receiverClass);*/
              receiverClass = LST_CLASS(receiverClass);
              tmp = CALC_CACHE_HASH(messageSelector, receiverClass);
              if (cache[tmp].name == messageSelector && cache[tmp].stclass == receiverClass) {
checkForInlineCacheHit:
# ifdef INLINE_SOME_METHODS
                { int f; op = cache[tmp].method;
                  for (f = 0; lstInlineMethodList[f].name; ++f) {
                    if (low == lstInlineMethodList[f].argc && *(lstInlineMethodList[f].method) == op) {
                      op = stack->data[stackTop-low]; /* self */
                      if (LST_IS_SMALLINT(op)) break; /* invalid object */
                      switch (f) {
                        case 0: /* Array>>at: */
                          /*fprintf(stderr, "Array>>at: hit!\n");*/
                          if (LST_IS_BYTES(op)) break;
                          op1 = stack->data[stackTop-1]; /* index */
                          if (LST_IS_SMALLINT(op1)) {
                            l0 = lstIntValue(op1)-1;
                            if (l0 < 0 || l0 >= LST_SIZE(op)) break; /* out of range */
                            stackTop -= low;
                            retValue = op->data[l0];
                            low = -1;
                            goto markArgsInlined;
                          }
                          break;
                        case 1: /* Array>>size */
                          /*fprintf(stderr, "Array>>size hit!\n");*/
                          stackTop -= low;
                          l0 = LST_SIZE(op);
                          retValue = lstNewInt(l0);
                          low = -1;
                          goto markArgsInlined;
                        case 2: /* String>>at: */
                          if (!LST_IS_BYTES(op)) break; /* not a string */
                          op1 = stack->data[stackTop-1]; /* index */
                          if (LST_IS_SMALLINT(op1)) {
                            l0 = lstIntValue(op1)-1;
                            if (l0 < 0 || l0 >= LST_SIZE(op)) break; /* out of range */
                            stackTop -= low;
                            l0 = lstBytePtr(op)[l0];
                            retValue = lstCharClass->data[lstIVcharsInMetaChar]->data[l0];
                            low = -1;
                            goto markArgsInlined;
                          }
                          break;
                        case 3: /* String>>printString */
                          /*fprintf(stderr, "String>>printString hit!\n");*/
                          if (op->stclass == lstSymbolClass) {
                            ptemp = op;
                            l0 = LST_SIZE(ptemp);
                            retValue = (lstObject *)lstMemAllocBin(l0);
                            retValue->stclass = lstStringClass;
                            if (l0 > 0) memcpy(lstBytePtr(retValue), lstBytePtr(ptemp), l0);
                            ptemp = NULL;
                          } else if (op->stclass == lstStringClass) {
                            retValue = op;
                          } else {
                            break;
                          }
                          stackTop -= low;
                          low = -1;
                          goto markArgsInlined;
                        case 4: /* Symbol>>printString */
                          /*fprintf(stderr, "Symbol>>printString hit!\n");*/
                          if (op->stclass == lstSymbolClass) {
                            ptemp = op;
                            l0 = LST_SIZE(ptemp);
                            retValue = (lstObject *)lstMemAllocBin(l0);
                            retValue->stclass = lstStringClass;
                            if (l0 > 0) memcpy(lstBytePtr(retValue), lstBytePtr(ptemp), l0);
                            ptemp = NULL;
                          } else if (op->stclass == lstStringClass) {
                            retValue = op;
                          } else break;
                          stackTop -= low;
                          low = -1;
                          goto markArgsInlined;
                        case 5: /* String>>basicAt:put: */
                          /*fprintf(stderr, "String>>basicAt:put: hit!\n");*/
                          if (!LST_IS_BYTES(op)) break; /* not a string */
                          op1 = stack->data[stackTop-2]; /* index */
                          if (LST_IS_SMALLINT(op1)) {
                            l0 = lstIntValue(op1)-1;
                            if (l0 < 0 || l0 >= LST_SIZE(op)) break; /* out of range */
                            op1 = stack->data[stackTop-1]; /* value */
                            if (LST_IS_SMALLINT(op1)) {
                              stackTop -= low;
                              lstBytePtr(op)[l0] = lstIntValue(op1);
                              retValue = op;
                              low = -1;
                              goto markArgsInlined;
                            }
                          }
                          break;
                        case 6: /* MetaChar>>new: */
                          /*fprintf(stderr, "MetaChar>>new: hit!\n");*/
                          op1 = stack->data[stackTop-1]; /* value */
                          if (LST_IS_SMALLINT(op1)) {
                            l0 = lstIntValue(op1);
                            if (l0 < 0 || l0 >= 257) break; /* out of range */
                            stackTop -= low;
                            retValue = lstCharClass->data[lstIVcharsInMetaChar]->data[l0];
                            low = -1;
                            goto markArgsInlined;
                          }
                          break;
                        case 7: /* String>>at:ifAbsent: */
                          /*fprintf(stderr, "String>>at:ifAbsent: hit!\n");*/
                          if (!LST_IS_BYTES(op)) break; /* not a string */
                          op1 = stack->data[stackTop-2]; /* index */
                          if (LST_IS_SMALLINT(op1)) {
                            l0 = lstIntValue(op1)-1;
                            if (l0 < 0 || l0 >= LST_SIZE(op)) break; /* out of range */
                            stackTop -= low;
                            l0 = lstBytePtr(op)[l0];
                            retValue = lstCharClass->data[lstIVcharsInMetaChar]->data[l0];
                            low = -1;
                            goto markArgsInlined;
                          }
                          break;
                        case 8: /* Block>>value: */
                          /*fprintf(stderr, "Block>>value: hit!\n");*/
                          curIP = l1;
                          /* swap argumnets */
                          op1 = stack->data[stackTop-1];
                          stack->data[stackTop-1] = op;
                          stack->data[stackTop-2] = op1;
                          ptemp = lstNilObj; /* flag */
                          context->data[lstIVbytePointerInContext] = lstNewInt(curIP);
                          context->data[lstIVstackTopInContext] = lstNewInt(stackTop-2);
                          goto doBlockInvocation;
                        default:
                          fprintf(stderr, "ready to inline: %s\n", lstInlineMethodList[f].name);
                          break;
                      }
                      break;
                    }
                  }
                }
                if (low != 1 && low != 2) goto markArgsNoInlining;
# endif
                if (cache[tmp].analyzed <= 0) break;
                /*stackTop -= low;*/ /* remove all args */
                /* do inline, omit argument array creation */
markArgsInlined:
                cache[tmp].badHits = 0;
                l0 = bp[curIP = l1]; /* skip SendMessage */
                switch (l0) {
                  case lstBCDoSpecial*16+lstBXStackReturn:
                    context = context->data[lstIVpreviousContextInContext];
                    break;
                  case lstBCDoSpecial*16+lstBXBlockReturn:
                    context = context->data[lstIVcreatingContextInBlock]->data[lstIVpreviousContextInContext];
                    break;
                  default:
                    l0 = 0;
                    break;
                }
# ifdef INLINE_SOME_METHODS
                if (low < 0) {
                  if (l0) goto doReturn2;
                  stack->data[stackTop++] = retValue;
                  goto markArgsCompleteNoPush;
                }
# endif
                /* execute inline code */
                if ((l1 = cache[tmp].ivarNum) >= 0) {
                  /* instance variable */
                  if (cache[tmp].analyzed == 1) {
                    iprintf("ANALYZER: pushing ivar %d\n", l1);
                    retValue = stack->data[stackTop-1]->data[l1];
                  } else {
                    iprintf("ANALYZER: setting ivar %d\n", l1);
                    assert(low == 2);
                    (retValue = stack->data[stackTop-2])->data[l1] = stack->data[stackTop-1];
                    --stackTop; /* drop argument, return self */
                    if (l0) { ++lstInfoIVarHit; goto doReturn2; }
                    goto markArgsCompleteNoPush;
                  }
                  ++lstInfoIVarHit;
                } else {
                  /* constant */
                  iprintf("ANALYZER: pushing constant/literal\n");
                  ++lstInfoLiteralHit;
                  retValue = cache[tmp].mConst;
                }
                if (l0) goto doReturn2;
                stack->data[stackTop-1] = retValue;
                goto markArgsCompleteNoPush;
              } else if (cache[tmp+1].name == messageSelector && cache[tmp+1].stclass == receiverClass) {
                /*++cache[tmp++].badHits;*/
                ++tmp;
                goto checkForInlineCacheHit;
              }
              break;
            case lstBCExtended:
              if (bp[curIP]%16 == lstBCSendMessage) {
                l0 = bp[curIP+1];
                l1 = curIP+2;
                goto checkForInline;
              }
              break;
          }
        }
# ifdef INLINE_SOME_METHODS
markArgsNoInlining:
# endif
#endif
        /* no inlining */
        op = lstMemAlloc(low);
        op->stclass = lstArrayClass;
        /* now load new argument array */
        while (--low >= 0) op->data[low] = POPIT;
        PUSHIT(op);
markArgsCompleteNoPush:
        break;
      case lstBCPushBlock:
        DBG0("PushBlock");
        /* create a block object; low is arg location; next word is goto value; next byte is argCount */
        high = VAL;
        curIP += VALSIZE;
        tmp = bp[curIP++]; /* argCount */
        ptemp = lstNewArray(lstIntValue(method->data[lstIVstackSizeInMethod]));
        op = lstMemAlloc(lstBlockSize);
        op->stclass = lstBlockClass;
        /*op = lstAllocInstance(lstBlockSize, lstBlockClass);*/
        op->data[lstIVbytePointerInContext] = op->data[lstIVstackTopInBlock] = lstNewInt(0);
        op->data[lstIVpreviousContextInBlock] = lstNilObj;
        op->data[lstIVbytePointerInBlock] = lstNewInt(curIP);
        op->data[lstIVargumentLocationInBlock] = lstNewInt(low);
        op->data[lstIVstackInBlock] = ptemp;
        op->data[lstIVargCountInBlock] = lstNewInt(tmp);
        op->data[lstIVcreatingContextInBlock] =
          context->stclass==lstBlockClass ? context->data[lstIVcreatingContextInBlock] : context;
        op->data[lstIVprocOwnerInBlock] = aProcess;
        op->data[lstIVmethodInBlock] = method;
        op->data[lstIVargumentsInBlock] = arguments;
        op->data[lstIVtemporariesInBlock] = temporaries;
        /***/
        PUSHIT(op);
        curIP = high;
        ptemp = NULL;
        break;
      case lstBCSendUnary: /* optimize certain unary messages */
        DBG1("SendUnary", low);
        op = POPIT;
        switch (low) {
          case 0: /* isNil */
            retValue = op==lstNilObj ? lstTrueObj : lstFalseObj;
            break;
          case 1: /* notNil */
            retValue = op==lstNilObj ? lstFalseObj : lstTrueObj;
            break;
          default:
            lstFatal("unimplemented SendUnary", low);
        }
        PUSHIT(retValue);
        break;
      case lstBCSendBinary: /* optimize certain binary messages */
        DBG1("SendBinary", low);
        ptemp1 = POPIT;
        ptemp = POPIT;
        if (low == 13) {
          /* == */
          retValue = ptemp==ptemp1 ? lstTrueObj : lstFalseObj;
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* small integers */
        if (LST_IS_SMALLINT(ptemp) && LST_IS_SMALLINT(ptemp1)) {
          int i = lstIntValue(ptemp);
          int j = lstIntValue(ptemp1);
          switch (low) {
            case 0: /* < */
              retValue = i<j ? lstTrueObj : lstFalseObj;
              break;
            case 1: /* <= */
              retValue = i<=j ? lstTrueObj : lstFalseObj;
              break;
            case 2: /* + */
              itmp = (int64_t)i+j;
              retValue = lstNewInteger(itmp);
              break;
            case 3: /* - */
              itmp = (int64_t)i-j;
              retValue = lstNewInteger(itmp);
              break;
            case 4: /* * */
              itmp = (int64_t)i*j;
              retValue = lstNewInteger(itmp);
              break;
            case 5: /* / */
              if (j == 0) goto binoptfailed;
              retValue = lstNewInt(i/j);
              break;
            case 6: /* % */
              if (j == 0) goto binoptfailed;
              retValue = lstNewInt(i%j);
              break;
            case 7: /* > */
              retValue = i>j ? lstTrueObj : lstFalseObj;
              break;
            case 8: /* >= */
              retValue = i>=j ? lstTrueObj : lstFalseObj;
              break;
            case 9: /* ~= */
              retValue = i!=j ? lstTrueObj : lstFalseObj;
              break;
            case 10: /* = */
              retValue = i==j ? lstTrueObj : lstFalseObj;
              break;
            default: goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* chars */
        if (LST_CLASS(ptemp) == lstCharClass && LST_CLASS(ptemp1) == lstCharClass) {
          int i = lstIntValue(ptemp->data[0]);
          int j = lstIntValue(ptemp1->data[0]);
          switch (low) {
            case 0: /* < */
              retValue = i<j ? lstTrueObj : lstFalseObj;
              break;
            case 1: /* <= */
              retValue = i<=j ? lstTrueObj : lstFalseObj;
              break;
            case 7: /* > */
              retValue = i>j ? lstTrueObj : lstFalseObj;
              break;
            case 8: /* >= */
              retValue = i>=j ? lstTrueObj : lstFalseObj;
              break;
            case 9: /* ~= */
              retValue = i!=j ? lstTrueObj : lstFalseObj;
              break;
            case 10: /* = */
              retValue = i==j ? lstTrueObj : lstFalseObj;
              break;
            default: goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* logics */
        if (ptemp == lstTrueObj || ptemp == lstFalseObj) {
          /* can only do operations that won't trigger garbage collection */
          switch (low) {
            case 11: /* & */
              retValue = ptemp==lstTrueObj ? ptemp1 : lstFalseObj;
              break;
            case 12: /* | */
              retValue = ptemp==lstTrueObj ? lstTrueObj : ptemp1;
              break;
            default:
              goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* logics */
        if (ptemp == lstNilObj) {
          /* can only do operations that won't trigger garbage collection */
          switch (low) {
            case 11: /* & */
              retValue = lstFalseObj;
              break;
            case 12: /* | */
              retValue = ptemp1;
              break;
            default:
              goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* logics, not bool, not nil */
        if (LST_IS_SMALLINT(ptemp) || ptemp->stclass != lstBooleanClass) {
          switch (low) {
            case 11: /* & */
              retValue = ptemp1;
              break;
            case 12: /* | */
              retValue = ptemp;
              break;
            default:
              goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* byte arrays */
        if (LST_IS_BYTES(ptemp) && LST_IS_BYTES(ptemp1)) {
          switch (low) {
            case 0: /* < */
              retValue = symbolcomp(ptemp, ptemp1)<0 ? lstTrueObj : lstFalseObj;
              break;
            case 1: /* <= */
              retValue = symbolcomp(ptemp, ptemp1)<=0 ? lstTrueObj : lstFalseObj;
              break;
            case 2: /* + */
              if (ptemp->stclass == ptemp1->stclass &&
                  (ptemp->stclass == lstStringClass || ptemp->stclass == lstByteArrayClass ||
                   ptemp->stclass == lstByteCodeClass)) {
                /* string concatenation */
                retValue = (lstObject *)lstMemAllocBin(LST_SIZE(ptemp)+LST_SIZE(ptemp1));
                retValue->stclass = ptemp->stclass;
                tmp = LST_SIZE(ptemp);
                if (tmp) memcpy(lstBytePtr(retValue), lstBytePtr(ptemp), tmp);
                l0 = LST_SIZE(ptemp1);
                if (l0) memcpy(lstBytePtr(retValue)+tmp, lstBytePtr(ptemp1), l0);
                break;
              }
              goto binoptfailed;
            case 7: /* > */
              retValue = symbolcomp(ptemp, ptemp1)>0 ? lstTrueObj : lstFalseObj;
              break;
            case 8: /* >= */
              retValue = symbolcomp(ptemp, ptemp1)>=0 ? lstTrueObj : lstFalseObj;
              break;
            case 9: /* ~= */
              retValue = symbolcomp(ptemp, ptemp1)!=0 ? lstTrueObj : lstFalseObj;
              break;
            case 10: /* = */
              retValue = symbolcomp(ptemp, ptemp1)==0 ? lstTrueObj : lstFalseObj;
              break;
            default: goto binoptfailed;
          }
          PUSHIT(retValue);
          ptemp = ptemp1 = NULL;
          break;
        }
        /* do message send */
binoptfailed:
        arguments = lstMemAlloc(2);
        arguments->stclass = lstArrayClass;
        /* now load new argument array */
        arguments->data[0] = ptemp;
        arguments->data[1] = ptemp1;
        /* now go send message */
        messageSelector = lstBinMsgs[low];
        ptemp = ptemp1 = NULL;
        goto findMethodFromSymbol;
      case lstBCSendMessage:
        /*DBG1("SendMessage, literal", low);*/
        messageSelector = literals->data[low];
        arguments = POPIT;
findMethodFromSymbol:
        /* see if we can optimize tail call */
        if (ticks == 1) l0 = 0;
        else {
          switch (bp[curIP]) {
            case lstBCDoSpecial*16+lstBXStackReturn: l0 = 1; break;
            case lstBCDoSpecial*16+lstBXBlockReturn: l0 = 2; break;
            default: l0 = 0; break;
          }
        }
findMethodFromSymbol1:
        receiverClass = LST_CLASS(arguments->data[lstIVreceiverInArguments]);
        assert(LST_CLASS(messageSelector) == lstSymbolClass);
        DBGS("SendMessage", receiverClass->data[lstIVnameInClass], messageSelector);
checkCache:
        assert(LST_CLASS(messageSelector) == lstSymbolClass);
#if 0
        {
          char clnm[256], selnm[256];
          lstGetString(clnm, sizeof(clnm), (lstObject *)LST_CLASS(receiverClass)->data[lstIVnameInClass]);
          lstGetString(selnm, sizeof(selnm), (lstObject *)messageSelector);
          fprintf(stderr, "%04d: searching: %s>>%s\n", PC, clnm, selnm);
        }
#endif
        tmp = CALC_CACHE_HASH(messageSelector, receiverClass);
        /*tmp = (LstUInt)((intptr_t)messageSelector+(intptr_t)receiverClass)%MTD_CACHE_SIZE;*/
        if (cache[tmp].name == messageSelector && cache[tmp].stclass == receiverClass) {
          goto cacheHit;
        } else if (cache[tmp+1].name == messageSelector && cache[tmp+1].stclass == receiverClass) {
          ++cache[tmp++].badHits;
cacheHit: method = cache[tmp].method;
          ++lstInfoCacheHit;
        } else {
          ++lstInfoCacheMiss;
          if (++cache[tmp].badHits >= MTD_BAD_HIT_MAX) cache[tmp].name = NULL; /* clear this cache item */
          if (++cache[tmp+1].badHits >= MTD_BAD_HIT_MAX) cache[tmp+1].name = NULL; /* clear this cache item */
          method = lookupMethod(messageSelector, receiverClass);
          if (!method) {
            /* send 'doesNotUnderstand:args:' */
            if (messageSelector == lstBadMethodSym) lstFatal("doesNotUnderstand:args: missing", 0);
            /* we can reach this code only once */
            ptemp = receiverClass;
            ptemp1 = messageSelector;
            op = lstMemAlloc(3);
            op->stclass = lstArrayClass;
            op->data[lstIVreceiverInArguments] = arguments->data[lstIVreceiverInArguments];
            op->data[1] = ptemp1; /* selector */
            op->data[2] = arguments;
            arguments = op;
            receiverClass = ptemp; /* restore selector */
            ptemp = ptemp1 = NULL;
            messageSelector = lstBadMethodSym;
            goto findMethodFromSymbol1;
          }
          if (cache[tmp].name && cache[tmp].badHits <= MTD_BAD_HIT_MAX/2) ++tmp;
          /*if (cache[tmp].name) ++tmp;*/
          cache[tmp].name = messageSelector;
          cache[tmp].stclass = receiverClass;
          cache[tmp].method = method;
          cache[tmp].goodHits = 0; /* perfectly good cache */
          /*cache[tmp].analyzed = (LST_SIZE(arguments) != 1) ? -1 : 0*/;
#ifdef INLINER_ACTIVE
          if ((op = method->data[lstIVoptimDoneInMethod]) != lstNilObj) {
            if (op == lstFalseObj) {
              cache[tmp].analyzed = -1; /* should not be analyzed */
            } else {
              cache[tmp].analyzed = 1; /* already analyzed */
              if (LST_IS_SMALLINT(op)) {
                /* instance var */
                int f = lstIntValue(op);
                if (f < 0) {
                  cache[tmp].analyzed = 2;
                  f = (-f)-1;
                  iprintf("ANALYZER: already analyzed setter; ivar %d\n", f);
                } else {
                  iprintf("ANALYZER: already analyzed; ivar %d\n", f);
                }
                cache[tmp].ivarNum = f;
              } else {
                cache[tmp].mConst = method->data[lstIVretResInMethod];
                cache[tmp].ivarNum = -1;
                iprintf("ANALYZER: already analyzed; constant\n");
              }
            }
          } else {
            op = method->data[lstIVargCountInMethod];
            if (LST_IS_SMALLINT(op) && (lstIntValue(op) == 1 || lstIntValue(op) == 2)) {
              iprintf("ANALYZER: to be analyzed (argc=%d)\n", lstIntValue(op));
              cache[tmp].analyzed = 0; /* analyze it in the future */
            } else {
              iprintf("ANALYZER: never be analyzed; argc=%d\n", LST_IS_SMALLINT(op) ? lstIntValue(op) : -666);
              cache[tmp].analyzed = -1; /* never */
              method->data[lstIVoptimDoneInMethod] = lstFalseObj; /* 'never' flag */
            }
          }
#endif
        }
        cache[tmp].badHits = 0; /* good cache */
#ifdef INLINER_ACTIVE
        if (cache[tmp].analyzed > 0) {
analyzeSucceed:
          if (ticks == 1) goto analyzerJustDoIt;
          /* optimized */
          switch (l0) {
            case 1: context = context->data[lstIVpreviousContextInContext]; break;
            case 2: context = context->data[lstIVcreatingContextInBlock]->data[lstIVpreviousContextInContext]; break;
            default: l0 = 0; break;
          }
          /***/
          if ((l1 = cache[tmp].ivarNum) >= 0) {
            /* instance variable */
            if (cache[tmp].analyzed == 1) {
              iprintf("ANALYZER!: pushing ivar %d\n", l1);
              retValue = arguments->data[lstIVreceiverInArguments]->data[l1];
            } else {
              iprintf("ANALYZER!: setting ivar %d\n", l1);
              assert(cache[tmp].analyzed == 2);
              assert(LST_SIZE(arguments) == 2);
              (retValue = arguments->data[lstIVreceiverInArguments])->data[l1] = arguments->data[1];
            }
            ++lstInfoIVarHit;
          } else {
            /* constant */
            iprintf("ANALYZER!: pushing constant/literal\n");
            retValue = cache[tmp].mConst;
            ++lstInfoLiteralHit;
          }
          /* restore changed vars */
          if (l0) goto doReturn2;
          method = context->data[lstIVmethodInContext];
          arguments = context->data[lstIVargumentsInContext];
          PUSHIT(retValue);
          break;
        } else if (!cache[tmp].analyzed) {
          if (++cache[tmp].goodHits > 3) {
            /* analyze method */
            bp = (const unsigned char *)lstBytePtr(method->data[lstIVbyteCodesInMethod]);
            op = method->data[lstIVargCountInMethod];
            if (lstIntValue(op) == 1) {
              /* argc == 1 */
              switch (bp[0]/16) {
                case lstBCPushInstance:
                  if (bp[1] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                  cache[tmp].ivarNum = bp[0]%16;
                  break;
                case lstBCPushLiteral:
                  if (bp[1] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                  cache[tmp].mConst = method->data[lstIVliteralsInMethod]->data[bp[0]%16];
                  cache[tmp].ivarNum = -1;
                  break;
                case lstBCPushConstant:
                  if (bp[1] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                  cache[tmp].ivarNum = -1;
                  switch (bp[0]%16) {
                    case lstBLNilConst: cache[tmp].mConst = lstNilObj; break;
                    case lstBLTrueConst: cache[tmp].mConst = lstTrueObj; break;
                    case lstBLFalseConst: cache[tmp].mConst = lstFalseObj; break;
                    default: l1 = (bp[0]%16)-3; cache[tmp].mConst = lstNewInt(l1); break;
                  }
                  break;
                case lstBCExtended:
                  switch (bp[0]%16) {
                    case lstBCPushInstance:
                      if (bp[2] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                      cache[tmp].ivarNum = bp[1];
                      break;
                    case lstBCPushLiteral:
                      if (bp[2] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                      cache[tmp].mConst = method->data[lstIVliteralsInMethod]->data[bp[1]];
                      cache[tmp].ivarNum = -1;
                      break;
                    case lstBCPushConstant:
                      if (bp[2] != lstBCDoSpecial*16+lstBXStackReturn) goto analyzeFailed;
                      cache[tmp].ivarNum = -1;
                      switch (bp[1]) {
                        case lstBLNilConst: cache[tmp].mConst = lstNilObj; break;
                        case lstBLTrueConst: cache[tmp].mConst = lstTrueObj; break;
                        case lstBLFalseConst: cache[tmp].mConst = lstFalseObj; break;
                        default: l1 = bp[1]-3; cache[tmp].mConst = lstNewInt(l1); break;
                      }
                      break;
                    default: goto analyzeFailed;
                  }
                  break;
                default: goto analyzeFailed;
              }
              iprintf("ANALYZER: succeed; ivar=%d\n", cache[tmp].ivarNum);
              cache[tmp].analyzed = 1;
            } else {
              assert(lstIntValue(op) == 2);
              /* argc == 2 */
              /*
              0000: PushArgument 1
              0001: AssignInstance n
              0002: PopTop
              0003: SelfReturn
              */
              /*TODO: parse extended lstBCAssignInstance*/
              if (bp[0] == lstBCPushArgument*16+1 && bp[1]/16 == lstBCAssignInstance &&
                  bp[2] == lstBCDoSpecial*16+lstBXPopTop && bp[3] == lstBCDoSpecial*16+lstBXSelfReturn) {
                /*goto analyzeFailed;*/
                iprintf("ANALYZER: setter found; ivar=%d\n", bp[1]%16);
                cache[tmp].analyzed = 2;
                cache[tmp].ivarNum = bp[1]%16;
              } else {
                goto analyzeFailed;
              }
            }
            /* setup method info, so we can omit analyze stage in future */
            if (cache[tmp].ivarNum >= 0) {
              int f = cache[tmp].ivarNum;
              if (cache[tmp].analyzed == 2) f = -(f+1);
              method->data[lstIVoptimDoneInMethod] = lstNewInt(f);
            } else {
              method->data[lstIVoptimDoneInMethod] = lstTrueObj;
              method->data[lstIVretResInMethod] = cache[tmp].mConst;
            }
            goto analyzeSucceed;
analyzeFailed:
            cache[tmp].analyzed = -1;
            method->data[lstIVoptimDoneInMethod] = lstFalseObj;
          }
        }
#endif
analyzerJustDoIt:
#ifdef COLLECT_METHOD_STATISTICS
        l1 = lstIntValue(method->data[lstIVinvokeCountInMethod])+1;
        if (LST_64FITS_SMALLINT(l1)) method->data[lstIVinvokeCountInMethod] = lstNewInt(l1);
#endif
        ptemp = context;
        /* save current IP and SP */
        context->data[lstIVstackTopInContext] = lstNewInt(stackTop);
        context->data[lstIVbytePointerInContext] = lstNewInt(curIP);
        /*context->data[lstIVprocOwnerInContext] = aProcess;*/
        /* build environment for new context */
        low = lstIntValue(method->data[lstIVtemporarySizeInMethod]);
        stack = lstNewArray(lstIntValue(method->data[lstIVstackSizeInMethod]));
        temporaries = low>0 ? lstNewArray(low) : lstNilObj;
        /* build the new context */
        context = lstMemAlloc(lstContextSize);
        context->stclass = lstContextClass;
        /*context = lstAllocInstance(lstContextSize, lstContextClass);*/
        /*context->data[lstIVpreviousContextInContext] = ptemp;*/
        switch (l0) {
          case 1:
            context->data[lstIVpreviousContextInContext] = ptemp->data[lstIVpreviousContextInContext];
            break;
          case 2:
            context->data[lstIVpreviousContextInContext] =
              ptemp->data[lstIVcreatingContextInBlock]->data[lstIVpreviousContextInContext];
            break;
          default:
            context->data[lstIVpreviousContextInContext] = ptemp;
            break;
        }
        ptemp = NULL;
        context->data[lstIVprocOwnerInContext] = aProcess;
        context->data[lstIVtemporariesInContext] = temporaries;
        context->data[lstIVstackInContext] = stack;
        context->data[lstIVstackTopInContext] =
          context->data[lstIVbytePointerInContext] = lstNewInt(0);
        context->data[lstIVmethodInContext] = method;
        context->data[lstIVargumentsInContext] = arguments;
        literals = method->data[lstIVliteralsInMethod];
        instanceVariables = arguments->data[lstIVreceiverInArguments];
        stackTop = 0;
        curIP = 0;
        /* now go execute new method */
        break;
      /* execute primitive */
      case lstBCDoPrimitive:
        /* low is argument count; next byte is primitive number */
        high = bp[curIP++]; /* primitive number */
#ifdef DEBUG
        /*DBG2("DoPrimitive", high, low);*/
        if (lstDebugFlag) {
          const char *pn = lstFindPrimitiveName(high);
          char tmsg[1024];
          sprintf(tmsg, "DoPrimitive %s; argc=%d", pn, low);
          DBG0(tmsg);
        }
#endif
        lastCalledPrim = high;
        switch (high) {
          case 1: /* NewObject class size */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* size */
            op1 = POPIT; /* class */
            if (!LST_IS_SMALLINT(op)) goto failPrimitive;
            tmp = lstIntValue(op); /* size */
            if (tmp < 0) goto failPrimitive;
            retValue = lstAllocInstance(tmp, op1);
            break;
          case 2: /* NewByteArray class size */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* size */
            op1 = POPIT; /* class */
            if (!LST_IS_SMALLINT(op)) goto failPrimitive;
            tmp = lstIntValue(op); /* size */
            if (tmp < 0) goto failPrimitive;
            retValue = (lstObject *)lstMemAllocBin(tmp);
            retValue->stclass = op1;
            if (tmp > 0) memset(lstBytePtr(retValue), 0, tmp);
            break;
          case 3: /* ObjectIdentity */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT;
            op1 = POPIT;
            retValue = op==op1 ? lstTrueObj : lstFalseObj;
            break;
          case 4: /* ObjectClass */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            retValue = LST_CLASS(op);
            break;
          case 5: /* ObjectSize */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            tmp = LST_IS_SMALLINT(op) ? 0 : LST_SIZE(op); /* SmallInt has no size at all; it's ok */
            retValue = lstNewInt(tmp);
            break;
          case 6: /* Array#at: obj index */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* index */
            op1 = POPIT; /* obj */
            if (!LST_IS_SMALLINT(op) || LST_IS_SMALLINT(op1) || LST_IS_BYTES(op1)) goto failPrimitive;
            tmp = lstIntValue(op)-1;
            /* bounds check */
            if (tmp < 0 || (LstUInt)tmp >= LST_SIZE(op1)) goto failPrimitive;
            if (LST_IS_SMALLINT(op1) || LST_IS_BYTES(op1)) goto failPrimitive;
            retValue = op1->data[tmp];
            break;
          case 7: /* Array#at:put:  value obj index */
            if (low != 3) goto failPrimitiveArgs;
            op = POPIT; /* index */
            retValue = POPIT; /* obj */
            op1 = POPIT; /* value */
            if (!LST_IS_SMALLINT(op) || LST_IS_SMALLINT(retValue) || LST_IS_BYTES(retValue)) goto failPrimitive;
            tmp = lstIntValue(op)-1;
            /* bounds check */
            if (tmp < 0 || (LstUInt)tmp >= LST_SIZE(retValue)) goto failPrimitive;
            if (LST_IS_SMALLINT(retValue) || LST_IS_BYTES(retValue)) goto failPrimitive;
            lstWriteBarrier(&retValue->data[tmp], op1);
            break;
          case 8: /* String#at: */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* index */
            op1 = POPIT; /* object */
            if (!LST_IS_SMALLINT(op) || !LST_IS_BYTES_EX(op1)) goto failPrimitive;
            tmp = lstIntValue(op)-1;
            /* bounds check */
            if (tmp < 0 || (LstUInt)tmp >= LST_SIZE(op1)) goto failPrimitive;
            tmp = lstBytePtr(op1)[tmp];
            retValue = lstNewInt(tmp);
            break;
          case 9: /* String#at:put:  value obj index */
            if (low != 3) goto failPrimitiveArgs;
            op = POPIT; /* index */
            retValue = POPIT; /* obj */
            op1 = POPIT; /* value */
            if (!LST_IS_SMALLINT(op) || !LST_IS_BYTES_EX(retValue) || !LST_IS_SMALLINT(op1)) goto failPrimitive;
            tmp = lstIntValue(op)-1;
            /* bounds check */
            if (tmp < 0 || (LstUInt)tmp >= LST_SIZE(retValue)) goto failPrimitive;
            lstBytePtr(retValue)[tmp] = lstIntValue(op1);
            break;
          case 10: /* String#clone: what class */
            if (low != 2) goto failPrimitiveArgs;
            /*TODO: check args */
            ptemp = POPIT; /* class */
            ptemp1 = POPIT; /* obj */
            if (!LST_IS_BYTES_EX(ptemp1)) { ptemp = ptemp1 = NULL; goto failPrimitive; }
            tmp = LST_SIZE(ptemp1);
            retValue = (lstObject *)lstMemAllocBin(tmp);
            retValue->stclass = ptemp;
            if (tmp > 0) memcpy(lstBytePtr(retValue), lstBytePtr(ptemp1), tmp);
            ptemp = ptemp1 = NULL;
            break;
          case 11: /* String#Position: aString from: pos; match substring in a string; return index of substring or nil */
          case 12: /* String#LastPosition: aString from: pos; match substring in a string; return index of substring or nil */
            if (low != 3) goto failPrimitiveArgs;
            /* from */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) tmp = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) tmp = lstLIntValue(op);
            else { stackTop -= 2; goto failPrimitive; }
            if (tmp < 1) tmp = 1;
            tmp--;
            /* what */
            op1 = POPIT;
            if (!LST_IS_BYTES_EX(op1)) {
              x = -1;
              if (LST_IS_SMALLINT(op1)) {
                x = lstIntValue(op1);
              } else if (op1->stclass == lstCharClass) {
                op1 = op1->data[0];
                if (LST_IS_SMALLINT(op1)) x = lstIntValue(op1);
              }
              if (x < 0 || x > 255) { --stackTop; goto failPrimitive; }
              sbuf[0] = x; sbuf[1] = '\0';
              op1 = NULL;
            }
            /* where */
            op = POPIT;
            if (!LST_IS_BYTES_EX(op)) goto failPrimitive;
            l0 = LST_SIZE(op);
            l1 = op1 ? LST_SIZE(op1) : strlen(sbuf);
            /*FIXME: tmp can be too big and cause the overflow*/
            retValue = lstNilObj;
            if (tmp >= l0 || l0 < 1 || l1 < 1 || l1 > l0-tmp) {
              /* can't be found, do nothing */
            } else {
              const unsigned char *s0 = lstBytePtr(op);
              const unsigned char *s1 = op1 ? (const unsigned char *)lstBytePtr(op1) : (const unsigned char *)sbuf;
              s0 += tmp; l0 -= tmp;
              /*FIXME: this can be faster, especially for LastPosition; rewrite it! */
              for (; l0 >= l1; l0--, s0++, tmp++) {
                if (memcmp(s0, s1, l1) == 0) {
                  retValue = lstNewInt(tmp+1);
                  if (high == 11) break; /* early exit for Position */
                }
              }
            }
            break;
          case 13: /* StringCopyFromTo */
            if (low != 3) goto failPrimitiveArgs;
            /* tmp: to */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) tmp = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) tmp = lstLIntValue(op);
            else if (op->stclass == lstFloatClass) tmp = lstFloatValue(op);
            else { stackTop -= 2; goto failPrimitive; }
            if (tmp < 1) { stackTop -= 2; goto failPrimitive; }
            /* x: from */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) x = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) x = lstLIntValue(op);
            else if (op->stclass == lstFloatClass) x = lstFloatValue(op);
            else { --stackTop; goto failPrimitive; }
            if (x < 1) { --stackTop; goto failPrimitive; }
            /* op: string */
            op = POPIT;
            if (!LST_IS_BYTES_EX(op)) goto failPrimitive;
            low = LST_SIZE(op);
            /*printf("size=%d; from=%d; to=%d\n", low, x, tmp);*/
            --x; --tmp;
            if (tmp < x || x >= low) low = 0;
            else {
              low -= x;
              tmp -= x-1;
              low = tmp<low ? tmp : low;
            }
            ptemp = op;
            retValue = (lstObject *)lstMemAllocBin(low);
            op = ptemp;
            retValue->stclass = op->stclass;
            /*printf("copying from %d, %d bytes\n", x, low);*/
            if (low > 0) memcpy(lstBytePtr(retValue), lstBytePtr(op)+x, low);
            break;
          case 14: /* BulkObjectExchange */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT;
            if (op->stclass != lstArrayClass) { --stackTop; goto failPrimitive; }
            retValue = POPIT;
            if (retValue->stclass != lstArrayClass) goto failPrimitive;
            if (LST_SIZE(op) != LST_SIZE(retValue)) goto failPrimitive;
            lstSwapObjects(op, retValue, LST_SIZE(op));
            break;
          case 15: { /* replaceFrom:... */ /* <replaceFrom:to:with:startingAt: start stop replacement repStart self> */
            if (low != 5) goto failPrimitiveArgs;
            /*TODO: check args */
            retValue = POPIT; /* object */
            lstObject *tmpRepStart = POPIT; /* startingAt */
            lstObject *tmpSrc = POPIT; /* with */
            lstObject *tmpStop = POPIT; /* to */
            lstObject *tmpStart = POPIT; /* from */
            if (lstBulkReplace(retValue, tmpStart, tmpStop, tmpSrc, tmpRepStart)) goto failPrimitive;
            } break;

          case 16: /* BlockInvocation: (args)* block */
            if (ptemp != NULL) abort();
doBlockInvocation:
            if (low < 1) goto failPrimitiveArgs;
            /* low holds number of arguments */
            op = POPIT; /* block */
            --low;
            /*if (op->data[lstIVbytePointerInContext] != lstNilObj) fprintf(stderr, "CALLING ALREADY CALLED BLOCK!\n");*/
            if (LST_IS_SMALLINT(op) || LST_IS_BYTES(op)) goto failPrimitiveArgs;
            if (op->stclass != lstBlockClass && !lstIsKindOf(op, lstBlockClass)) goto failPrimitiveArgs;
            /*if (op->stclass != lstBlockClass) { stackTop -= (low-1); goto failPrimitiveArgs; }*/
            /* put arguments in place */
            /* get arguments location (tmp) */
            op1 = op->data[lstIVargumentLocationInBlock];
            if (!LST_IS_SMALLINT(op1)) goto failPrimitiveArgs;
            tmp = lstIntValue(op1);
            /* get max argument count (l0) */
            op1 = op->data[lstIVargCountInBlock];
            if (!LST_IS_SMALLINT(op1)) goto failPrimitiveArgs;
            l0 = lstIntValue(op1);
            /* setup arguments */
            temporaries = op->data[lstIVtemporariesInBlock];
            /* do not barf if there are too many args; just ignore */
            /*fprintf(stderr, "block: args=%d; passed=%d\n", l0, low);*/
            if (low > l0) { stackTop -= (low-l0); low = l0; } /* drop extra args */
            for (l1 = low; l1 < l0; ++l1) temporaries->data[tmp+l1] = lstNilObj;
            while (--low >= 0) temporaries->data[tmp+low] = POPIT;
            for (; low >= 0; --low) temporaries->data[tmp+low] = POPIT;
            if (!ptemp) {
              op->data[lstIVpreviousContextInBlock] = context->data[lstIVpreviousContextInContext];
            } else {
              /*ptemp = NULL;*/
              op->data[lstIVpreviousContextInBlock] = context;
            }
            context = /*aProcess->data[lstIVcontextInProcess] =*/ op;
            context->data[lstIVtemporariesInContext] = temporaries;
            reloadFromCtx();
            stackTop = 0;
            curIP = lstIntValue(context->data[lstIVbytePointerInBlock]);
            goto endPrimitive;

          case 17: /* flush method cache; invalidate cache for class */
            /*
             * <#FlushMethodCache>: flush everything
             * <#FlushMethodCache oldclass>: flush the cache for the given class
             * <#FlushMethodCache oldmethod true>: flush the cache for the given method
             */
#ifdef BETTER_CACHE_CONTROL
            switch (low) {
              case 1: /* for class */
                dprintf("FLUSHCLASSCACHE\n");
                op = POPIT; /* old class */
                for (l0 = MTD_CACHE_SIZE+MTD_CACHE_EXTRA-1; l0 >= 0; --l0) {
                  if (cache[l0].name && cache[l0].stclass == op) cache[l0].name = NULL;
                }
                break;
              case 2: /* for method */
                dprintf("FLUSHMETHODCACHE\n");
                --stackTop; /* drop flag */
                op = POPIT; /* old method */
                for (l0 = MTD_CACHE_SIZE+MTD_CACHE_EXTRA-1; l0 >= 0; --l0) {
                  if (cache[l0].name && cache[l0].method == op) cache[l0].name = NULL;
                }
                break;
              default:
                dprintf("FLUSHCACHE\n");
                stackTop -= low;
                lstFlushMethodCache();
                break;
            }
#else
            /*if (low == 1 || low > 3) { stackTop -= low; low = 0; }*/
            stackTop -= low;
            lstFlushMethodCache();
#endif
            break;

          case 18: /* SmallIntToInteger */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) retValue = lstNewLongInt(lstIntValue(op));
            else if (op->stclass == lstIntegerClass) retValue = lstNewLongInt(lstLIntValue(op));
            else goto failPrimitive;
            break;
          case 19: /* NumberToFloat */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) retValue = lstNewFloat(lstIntValue(op));
            else if (op->stclass == lstIntegerClass) retValue = lstNewFloat(lstLIntValue(op));
            else if (op->stclass == lstFloatClass) retValue = lstNewFloat(lstFloatValue(op));
            else goto failPrimitive;
            break;
          case 20: /* FloatToInteger */
            if (low < 1 || low > 2) goto failPrimitiveArgs;
            op = POPIT; /* float */
            if (low > 1) {
              op1 = POPIT; /* opcode */
              if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
              if (!LST_IS_BYTES_EX(op) || op->stclass != lstFloatClass) goto failPrimitive;
              fop0 = lstFloatValue(op);
              switch (lstIntValue(op1)) {
                case 1: fop0 = trunc(fop0); break;
                case 2: fop0 = round(fop0); break;
                case 3: fop0 = floor(fop0); break;
                case 4: fop0 = ceil(fop0); break;
                default: goto failPrimitive;
              }
              ll0 = fop0;
              retValue = lstNewInteger(ll0);
            } else {
              if (LST_IS_SMALLINT(op)) retValue = lstNewLongInt(lstIntValue(op));
              else if (op->stclass == lstIntegerClass) retValue = lstNewLongInt(lstLIntValue(op));
              else if (op->stclass == lstFloatClass) retValue = lstNewLongInt((LstLInt)lstFloatValue(op));
              else goto failPrimitive;
            }
            break;
          case 21: /* IntegerToSmallInt (low order of Integer -> SmallInt) */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) ll0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll0 = lstLIntValue(op);
            else goto failPrimitive;
            tmp = (int)ll0;
            if (!LST_64FITS_SMALLINT(tmp)) goto failPrimitive;
            retValue = lstNewInt(tmp);
            break;
          case 22: /* IntegerToSmallIntTrunc */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) retValue = op;
            else if (op->stclass == lstIntegerClass) {
              ll0 = lstLIntValue(op);
              tmp = (int)ll0;
              retValue = lstNewInt(tmp);
            } else if (op->stclass == lstFloatClass) {
              ll0 = (LstLInt)(lstFloatValue(op));
              tmp = (int)ll0;
              retValue = lstNewInt(tmp);
            } else goto failPrimitive;
            break;

          case 23: /* bit2op: bitOr: bitAnd: bitXor: */
            if (low != 3) goto failPrimitiveArgs;
            /* operation type */
            op = POPIT;
            if (!LST_IS_SMALLINT(op)) { stackTop -= 2; goto failPrimitive; }
            tmp = lstIntValue(op); /* operation */
            /* second arg */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) ll1 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll1 = lstLIntValue(op);
            else { --stackTop; goto failPrimitive; }
            /* first arg */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) ll0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll0 = lstLIntValue(op);
            else goto failPrimitive;
            switch (tmp) {
              case 0: ll0 = ll0 | ll1; break;
              case 1: ll0 = ll0 & ll1; break;
              case 2: ll0 = ll0 ^ ll1; break;
              default: goto failPrimitive;
            }
            retValue = lstNewInteger(ll0);
            break;
          case 24: /* bitNot */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) ll0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll0 = lstLIntValue(op);
            else goto failPrimitive;
            retValue = lstNewInteger(~ll0);
            break;
          case 25: /* bitShift: */
            if (low != 2) goto failPrimitiveArgs;
             /* by */
            op = POPIT;
            if (!LST_IS_SMALLINT(op)) { --stackTop; goto failPrimitive; }
            tmp = lstIntValue(op); /* shift count */
            /* what */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) ll0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll0 = lstLIntValue(op);
            else goto failPrimitive;
            if (tmp < 0) {
              /* negative means shift right */
              ll0 >>= (-tmp);
            } else {
              /* shift left */
              ll0 <<= tmp;
            }
            retValue = lstNewInteger(ll0);
            break;

          case 26: /* SmallIntAdd */
          case 27: /* SmallIntSub */
          case 28: /* SmallIntMul */
          case 29: /* SmallIntDiv */
          case 30: /* SmallIntMod */
          case 31: /* SmallIntLess */
          case 32: /* SmallLessEqu */
          case 33: /* SmallIntGreat */
          case 34: /* SmallIntGreatEqu */
          case 35: /* SmallIntEqu */
          case 36: /* SmallIntNotEqu */
            if (low != 2) goto failPrimitiveArgs;
            op1 = POPIT;
            op = POPIT;
            if (!LST_IS_SMALLINT(op) || !LST_IS_SMALLINT(op1)) goto failPrimitive;
            l1 = lstIntValue(op1);
            l0 = lstIntValue(op);
            if (high <= 30) {
              switch (high) {
                case 26: itmp = (int64_t)l0+l1; retValue = lstNewInteger(itmp); break;
                case 27: itmp = (int64_t)l0-l1; retValue = lstNewInteger(itmp); break;
                case 28: itmp = (int64_t)l0*l1; retValue = lstNewInteger(itmp); break;
                case 29: if (l1 == 0) goto failPrimitive; l0 /= l1; retValue = lstNewInt(l0); break;
                case 30: if (l1 == 0) goto failPrimitive; l0 %= l1; retValue = lstNewInt(l0); break;
              }
            } else {
              switch (high) {
                case 31: retValue = l0<l1 ? lstTrueObj : lstFalseObj; break;
                case 32: retValue = l0<=l1 ? lstTrueObj : lstFalseObj; break;
                case 33: retValue = l0>l1 ? lstTrueObj : lstFalseObj; break;
                case 34: retValue = l0>=l1 ? lstTrueObj : lstFalseObj; break;
                case 35: retValue = l0==l1 ? lstTrueObj : lstFalseObj; break;
                case 36: retValue = l0!=l1 ? lstTrueObj : lstFalseObj; break;
              }
            }
            break;
          case 37: /* IntegerAdd */
          case 38: /* IntegerSub */
          case 39: /* IntegerMul */
          case 40: /* IntegerDiv */
          case 41: /* IntegerMod */
          case 42: /* IntegerLess */
          case 43: /* IntegerLessEqu */
          case 44: /* IntegerGreat */
          case 45: /* IntegerGreatEqu */
          case 46: /* IntegerEqu */
          case 47: /* IntegerNotEqu */
            if (low != 2) goto failPrimitiveArgs;
            op1 = POPIT;
            op = POPIT;
            if (LST_IS_SMALLINT(op1)) ll1 = lstIntValue(op1);
            else if (op1->stclass == lstIntegerClass) ll1 = lstLIntValue(op1);
            else goto failPrimitive;
            if (LST_IS_SMALLINT(op)) ll0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) ll0 = lstLIntValue(op);
            else goto failPrimitive;
            switch (high) {
              case 37: retValue = lstNewLongInt(ll0+ll1); break;
              case 38: retValue = lstNewLongInt(ll0-ll1); break;
              case 39: retValue = lstNewLongInt(ll0*ll1); break;
              case 40: if (ll1 == 0) goto failPrimitive; retValue = lstNewLongInt(ll0/ll1); break;
              case 41: if (ll1 == 0) goto failPrimitive; retValue = lstNewLongInt(ll0%ll1); break;
              case 42: retValue = ll0<ll1 ? lstTrueObj : lstFalseObj; break;
              case 43: retValue = ll0<=ll1 ? lstTrueObj : lstFalseObj; break;
              case 44: retValue = ll0>ll1 ? lstTrueObj : lstFalseObj; break;
              case 45: retValue = ll0>=ll1 ? lstTrueObj : lstFalseObj; break;
              case 46: retValue = ll0==ll1 ? lstTrueObj : lstFalseObj; break;
              case 47: retValue = ll0!=ll1 ? lstTrueObj : lstFalseObj; break;
            }
            break;
          case 48: /* FloatAdd */
          case 49: /* FloatSub */
          case 50: /* FloatMul */
          case 51: /* FloatDiv */
          case 52: /* FloatLess */
          case 53: /* FloatLessEqu */
          case 54: /* FloatGreat */
          case 55: /* FloatGreatEqu */
          case 56: /* FloatEqu */
          case 57: /* FloatNotEqu */
            if (low != 2) goto failPrimitiveArgs;
            /* arg1 */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) fop1 = (LstFloat)lstIntValue(op);
            else if (op->stclass == lstIntegerClass) fop1 = (LstFloat)lstLIntValue(op);
            else if (op->stclass == lstFloatClass) fop1 = lstFloatValue(op);
            else { --stackTop; goto failPrimitive; }
            /* arg 0 */
            op = POPIT;
            if (LST_IS_SMALLINT(op)) fop0 = (LstFloat)lstIntValue(op);
            else if (op->stclass == lstIntegerClass) fop0 = (LstFloat)lstLIntValue(op);
            else if (op->stclass == lstFloatClass) fop0 = lstFloatValue(op);
            else goto failPrimitive;
            switch (high) {
              case 48: retValue = lstNewFloat(fop0+fop1); break;
              case 49: retValue = lstNewFloat(fop0-fop1); break;
              case 50: retValue = lstNewFloat(fop0*fop1); break;
              case 51: if (fop0 == 0.0) goto failPrimitive; retValue = lstNewFloat(fop0/fop1); break;
              case 52: retValue = fop0<fop1 ? lstTrueObj : lstFalseObj; break;
              case 53: retValue = fop0<=fop1 ? lstTrueObj : lstFalseObj; break;
              case 54: retValue = fop0>fop1 ? lstTrueObj : lstFalseObj; break;
              case 55: retValue = fop0>=fop1 ? lstTrueObj : lstFalseObj; break;
              case 56: retValue = fop0==fop1 ? lstTrueObj : lstFalseObj; break;
              case 57: retValue = fop0!=fop1 ? lstTrueObj : lstFalseObj; break;
            }
            break;
          case 58: /* FloatToString */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) sprintf(sbuf, "%d", lstIntValue(op));
            else if (op->stclass == lstIntegerClass) sprintf(sbuf, PRINTF_LLD, lstLIntValue(op));
            else if (op->stclass == lstFloatClass) sprintf(sbuf, "%.15g", lstFloatValue(op));
            else goto failPrimitive;
            retValue = lstNewString(sbuf);
            break;
          case 59: /* FloatNegate */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT;
            if (LST_IS_SMALLINT(op)) fop0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) fop0 = lstLIntValue(op);
            else if (op->stclass == lstFloatClass) fop0 = lstFloatValue(op);
            else goto failPrimitive;
            retValue = lstNewFloat(-fop0);
            break;

          case 60: /* PrimIdxName op arg */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* arg */
            op1 = POPIT; /* opno */
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            tmp = lstIntValue(op1);
            switch (tmp) {
              case 0: /* index by name */
                if (op->stclass != lstStringClass && op->stclass != lstSymbolClass) goto failPrimitive;
                if (LST_SIZE(op) > 126) {
                  retValue = lstNilObj;
                } else {
                  lstGetString(sbuf, 256, op);
                  int ix = lstFindPrimitiveIdx(sbuf);
                  retValue = ix>=0 ? lstNewInt(ix) : lstNilObj;
                }
                break;
              case 1: /* name by index */
                if (LST_IS_SMALLINT(op)) tmp = lstIntValue(op);
                else if (op == lstIntegerClass) tmp = lstLIntValue(op);
                else goto failPrimitive;
                {
                  const char *n = lstFindPrimitiveName(tmp);
                  retValue = n ? lstNewString(n) : lstNilObj;
                }
                break;
              default: goto failPrimitive;
            }
            break;

          case 61: /* GetCurrentProcess */
            if (low != 0) goto failPrimitiveArgs;
            retValue = aProcess;
            break;

          case 62: /* error trap / yield -- halt process; no args: error; else: suspend (yield) */
            if (low > 1) goto failPrimitiveArgs;
            if (low > 0) {
              /* yield */
              retValue = POPIT;
              stackTop -= (low-1); /* drop other args */
              tmp = lstReturnYield; /* no-error flag */
            } else {
              /* error */
              retValue = lstNilObj;
              tmp = lstReturnError; /* error flag */
            }
            int rr = doReturn(tmp);
            if (rr) XRETURN(rr);
            if (tmp || retGSwitch) goto doAllAgain;
            goto execComplete;

          case 63: /* ExecuteNewProcessAndWait proc tics */
            if (low != 2) goto failPrimitiveArgs;
            op1 = POPIT; /* ticks */
            op = POPIT; /* new process */
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            if (!lstIsKindOf(op, lstProcessClass)) goto failPrimitive;
            tmp = lstIntValue(op1);
            saveCurrentProcess();
            if (loadNewProcess(op) == 0) {
              /* new process succesfully loaded */
              ticks = tmp<1 ? 0 : tmp;
              lockCount = lockCount>0; /* start locked if locked */
              goto doAllAgain; /* go on with the new process */
            }
            reloadFromGroup(); /* restore old process */
            /* result */
            low = lstReturnError;
execComplete: /* low is the result */
            retValue = lstNewInt(low);
            goto doReturn;

          case 64: /* LockUnlockSheduler */
            if (low > 1) goto failPrimitiveArgs;
            if (low > 0) {
              op = POPIT;
              stackTop -= (low-1); /* drop other args */
              if (op == lstFalseObj) {
                /* unlock */
                if (--lockCount < 0) {
                  lockCount = 0;
                  /*goto failPrimitive;*/
                }
              } else {
                /* lock */
                ++lockCount;
              }
            }
            /* query lock state */
            retValue = lockCount ? lstTrueObj : lstFalseObj;
            break;
          case 65: /* TicksGetSet */
            if (low > 1) goto failPrimitiveArgs;
            if (low > 0) {
              op = POPIT;
              stackTop -= (low-1); /* drop other args */
              if (LST_IS_SMALLINT(op)) tmp = lstIntValue(op);
              else if (op == lstIntegerClass) tmp = lstLIntValue(op);
              else goto failPrimitive;
              if (tmp < 1) tmp = 1;
              ticks = tmp;
            }
            retValue = LST_FITS_SMALLINT(ticks) ? lstNewInt(ticks) : lstNewLongInt(ticks);
            break;
          case 66: /* RunGC */
            if (low != 0) goto failPrimitiveArgs;
            lstGC();
            retValue = lstNilObj;
            break;
          case 67: /* UserBreakSignal */
            if (low != 0) goto failPrimitiveArgs;
            ++lstExecUserBreak;
            retValue = lstNilObj;
            break;
          case 68: /* EventHandlerCtl */
            if (low == 0) {
              grpTicks = 1;
            } else {
              if (low != 2) goto failPrimitiveArgs;
              /*
               * <EventHandlerCtl eid true> -- suspend this process; wait for the event
               */
              op1 = POPIT;
              op = POPIT;
              if (!LST_IS_SMALLINT(op)) goto failPrimitive;
              tmp = lstIntValue(op);
              if (tmp < 1 || tmp > 65535) goto failPrimitive;
              if (op1 != lstTrueObj) goto failPrimitive;
              /*dprintf("eventWaitFor: %d\n", tmp);*/
              addOneShotEventHandler(tmp, curGroup);
              curGroup->ewait = -tmp; /* sheduler will save and skip this process */
            }
            retValue = lstTrueObj;
            break;
          case 69: /* ProcessGroupCtl */
            /*
             * <ProcessGroupCtl 0 process [ticks]> -- create new process group
             */
            if (low < 2 || low > 3) goto failPrimitiveArgs;
            if (low == 3) {
              op = POPIT; --low;
              if (!LST_IS_SMALLINT(op)) goto failPrimitiveArgs;
              tmp = lstIntValue(op);
              if (tmp < 1) tmp = 10000;
            } else tmp = 10000;
            op = POPIT;
            op1 = POPIT;
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            if (lstIntValue(op1) != 0) goto failPrimitive;
            if (!lstIsKindOf(op, lstProcessClass)) goto failPrimitive;
            if (op->data[lstIVrunningInProcess] != lstNilObj) goto failPrimitive;
            else {
              saveCurrentProcess();
              /* create new process group */
              LstRunGroup *ng = calloc(1, sizeof(LstRunGroup)); /*TODO: reuse free groups*/
              LstRunGroup *pg = curGroup;
              /* and switch */
              ng->ticks = tmp;
              curGroup = ng;
              if (loadNewProcess(op) == 0) {
                /* new process succesfully loaded, insert group in list (after current) */
                /*fprintf(stderr, "OK!\n");*/
                saveCurrentProcess();
                ng->prev = pg;
                ng->next = pg->next;
                pg->next = ng;
                if (ng->next) ng->next->prev = ng;
              } else {
                /* remove this group */
                free(ng);
                ng = NULL;
              }
              /* restore old process */
              curGroup = pg;
              reloadFromGroup();
              if (!ng) goto failPrimitive;
            }
            break;

          case 70: /* PrintObject */
            if (low == 0) {
              fflush(stdout);
            } else {
              if (low > 2) goto failPrimitiveArgs;
              op1 = low==2 ? POPIT : lstNilObj;
              op = POPIT;
              if (LST_IS_SMALLINT(op)) {
                tmp = lstIntValue(op);
                if (tmp >= 0 && tmp <= 255) fputc(tmp, stdout);
              } else if (LST_IS_BYTES(op)) {
                fwrite(lstBytePtr(op), LST_SIZE(op), 1, stdout);
              } else if (op->stclass == lstCharClass) {
                op = op->data[0];
                if (!LST_IS_SMALLINT(op)) goto failPrimitive;
                tmp = lstIntValue(op);
                if (tmp >= 0 && tmp <= 255) fputc(tmp, stdout);
              } else goto failPrimitive;
              if (op1 != lstNilObj) fputc('\n', stdout);
            }
            retValue = lstNilObj;
            break;
          case 71: /* ReadCharacter */
            if (low != 0) goto failPrimitiveArgs;
            tmp = fgetc(stdin);
            retValue = tmp==EOF ? lstNilObj : lstNewInt((int)(((unsigned int)tmp)&0xff));
            break;

          case 72: /* FloatBAIO opcode num */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* num */
            op1 = POPIT; /* opcode */
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            tmp = lstIntValue(op1);
            if (tmp < 0 || tmp > 1) goto failPrimitive;
            if (tmp == 0) {
              /* to byte array */
              if (LST_CLASS(op) != lstFloatClass) goto failPrimitive;
              ptemp = op;
              retValue = lstNewBinary(lstBytePtr(ptemp), sizeof(LstFloat));
              ptemp = NULL;
            } else {
              /* from byte array */
              LstFloat n;
              if (LST_CLASS(op) != lstByteArrayClass) goto failPrimitive;
              if (LST_SIZE(op) != sizeof(n)) goto failPrimitive;
              memcpy(&n, lstBytePtr(op), sizeof(n));
              retValue = lstNewFloat(n);
            }
            break;
          case 73: /* IntegerBAIO opcode num */
            if (low != 2) goto failPrimitiveArgs;
            op = POPIT; /* num */
            op1 = POPIT; /* opcode */
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            tmp = lstIntValue(op1);
            if (tmp < 0 || tmp > 1) goto failPrimitive;
            if (tmp == 0) {
              /* to byte array */
              if (LST_CLASS(op) != lstIntegerClass) goto failPrimitive;
              ptemp = op;
              retValue = lstNewBinary(lstBytePtr(ptemp), sizeof(LstLInt));
              ptemp = NULL;
            } else {
              /* from byte array */
              LstLInt n;
              if (LST_CLASS(op) != lstByteArrayClass) goto failPrimitive;
              if (LST_SIZE(op) != sizeof(n)) goto failPrimitive;
              memcpy(&n, lstBytePtr(op), sizeof(n));
              retValue = lstNewLongInt(n);
            }
            break;

          case 74: /* ExecuteContext ctx */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT; /* ctx */
            if (LST_CLASS(op) != lstContextClass && !lstIsKindOf(op, lstContextClass)) goto failPrimitive;
            op->data[lstIVpreviousContextInContext] = context->data[lstIVpreviousContextInContext];
            context = op;
            reloadFromCtx();
            goto doAllAgain;

          case 75: /* StFinalizeCtl obj add-remove-flag */
            if (low != 2) goto failPrimitiveArgs;
            op1 = POPIT; /* flag */
            op = POPIT; /* object */
            if (LST_IS_SMALLINT(op)) goto failPrimitive; /* SmallInt can't have finalizer */
            if (op1 == lstNilObj || op1 == lstFalseObj) {
              /* remove from list */
              if (LST_IS_STFIN(op)) {
                LST_RESET_STFIN(op);
                lstRemoveFromFList(&stFinListHead, op->fin);
                free(op->fin);
              }
            } else {
              /* add to list */
              if (!LST_IS_STFIN(op)) {
                if (op->fin) goto failPrimitive; /* object can have either C or ST finalizer, but not both */
                op->fin = calloc(1, sizeof(LstFinLink));
                if (!op->fin) lstFatal("out of memory is StFinalizeCtl", 0x29a);
                LST_SET_STFIN(op);
                op->fin->obj = op; /* owner */
                lstAddToFList(&stFinListHead, op->fin);
              }
            }
            retValue = lstNilObj;
            break;

          case 76: /* StWeakCtl obj */
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT; /* object */
            if (LST_IS_SMALLINT(op)) goto failPrimitive; /* SmallInt can't have finalizer */
            /* add to list */
            if (!LST_IS_WEAK(op)) {
              if (op->fin) goto failPrimitive; /* object can have either C or ST finalizer, or marked as weak, but not all */
              op->fin = calloc(1, sizeof(LstFinLink));
              if (!op->fin) lstFatal("out of memory is StWeakCtl", 0x29a);
              LST_SET_WEAK(op);
              op->fin->obj = op; /* owner */
              lstAddToFList(&stWeakListHead, op->fin);
            }
            retValue = lstNilObj;
            break;

          case 77: /* FloatFunc float idx */
            if (low != 2) goto failPrimitiveArgs;
            op1 = POPIT; /* idx */
            op = POPIT; /* float */
            if (!LST_IS_SMALLINT(op1)) goto failPrimitive;
            tmp = lstIntValue(op1);
            if (LST_IS_SMALLINT(op)) fop0 = lstIntValue(op);
            else if (op->stclass == lstIntegerClass) fop0 = lstLIntValue(op);
            else if (op->stclass == lstFloatClass) fop0 = lstFloatValue(op);
            else goto failPrimitive;
            switch (tmp) {
              case 0: fop0 = log2(fop0); break;
              default: goto failPrimitive;
            }
            retValue = lstNewFloat(fop0);
            break;

          case 78: /* LastFailedPrim */
            stackTop -= low;
            retValue = lstNewInt(lastFailedPrim);
            break;

          case 79: {/* FNVHash byteobj */
            uint32_t h;
            if (low != 1) goto failPrimitiveArgs;
            op = POPIT; /* obj */
            if (!LST_IS_BYTES_EX(op)) goto failPrimitive;
            h = fnvHash(lstBytePtr(op), LST_SIZE(op));
            tmp = (h%(INT_MAX/2));
            retValue = lstNewInt(tmp);
            break; }

          default:
            /* save stack pointers */
            l0 = lstRootTop;
            l1 = lstTempSP;
            {
#ifdef DEBUG
              lstPrimCtx = context;
              saveCurrentProcess();
#endif
              resetEvtCheckLeft = 0;
              LSTPrimitiveFn pfn = lstFindExtPrimitiveFn(high);
              retValue = pfn ? pfn(high, &(stack->data[stackTop-low]), low) : NULL;
              if (resetEvtCheckLeft) { evtCheckLeft = 1; }
            }
            stackTop -= low; /* remove primitive args */
            /* restore stacks */
            if (lstRootTop < l0) lstFatal("root stack error in primitive", high);
            if (lstTempSP < l1) lstFatal("temp stack error in primitive", high);
            lstRootTop = l0;
            lstTempSP = l1;
            if (!retValue) goto failPrimitive;
            break;
        }
        /* force a stack return due to successful primitive */
        ptemp = NULL;
        goto doReturn;
failPrimitiveArgs:
        stackTop -= low;
failPrimitive:
        lastFailedPrim = lastCalledPrim;
        /* supply a return value for the failed primitive */
        PUSHIT(lstNilObj);
endPrimitive:
        /* done with primitive, continue execution loop */
        ptemp = NULL;
        break;

      case lstBCDoSpecial:
        switch (low) {
          case lstBXSelfReturn:
            DBG0("DoSpecial: SelfReturn");
            retValue = arguments->data[lstIVreceiverInArguments];
            goto doReturn;
          case lstBXStackReturn:
            DBG0("DoSpecial: StackReturn");
            retValue = POPIT;
doReturn:   /*context->data[lstIVbytePointerInContext] = lstNilObj;*/
            context = context->data[lstIVpreviousContextInContext];
doReturn2:  if (context == lstNilObj) {
              /*aProcess->data[lstIVcontextInProcess] = lstNilObj;*/ /* 'complete' flag */
              int rr = doReturn(lstReturnReturned);
              if (rr) XRETURN(rr);
              if (tmp || retGSwitch) goto doAllAgain;
              goto execComplete;
            }
doReturn3:  aProcess->data[lstIVcontextInProcess] = context;
            reloadFromCtx();
            PUSHIT(retValue);
            break;
          case lstBXBlockReturn:
            DBG0("DoSpecial: BlockReturn");
            /* the very bad thing is that this can be inter-group return */
            retValue = POPIT;
            /*context->data[lstIVbytePointerInContext] = lstNilObj;*/
            /*dprintf("cp=%p\n", aProcess);*/
            context = context->data[lstIVcreatingContextInBlock]->data[lstIVpreviousContextInContext];
            if (context == lstNilObj) {
              if (curGroup->group->prev) {
                /* not the last process */
                goto doReturn2;
              }
              /* return from the process of the group */
              /* if this is return from the main group, we have to return from executor */
              if (curGroup == runGroups) {
                aProcess = runGroups->group->process; /* initial process */
                aProcess->data[lstIVresultInProcess] = retValue;
                aProcess->data[lstIVcontextInProcess] = lstNilObj;
                /* clear the current run group */
                while (curGroup->group) releaseRunContext();
                XRETURN(lstReturnReturned); /* done */
              } else {
                /* just kill the current run group */
                while (curGroup->group) releaseRunContext();
                free(removeCurrentGroup());
              }
              /* the current group is dead, go on with the next */
              nextGroup(0);
              goto doAllAgain;
            }
            /* check if we should do unwinding and possibly group switching */
            if (context->data[lstIVprocOwnerInContext] != aProcess) {
              /* yes, this is inter-process return; do unwinding */
              op = context->data[lstIVprocOwnerInContext];
              dprintf(" ct=%p\n", context);
              dprintf(" op=%p\n", op);
              dprintf(" nl=%p\n", lstNilObj);
              /* first try our own process group */
              if (groupHasProcess(curGroup, op)) {
                /* unwinding in current process group */
                while (curGroup->group->process != op) releaseRunContext();
                goto doReturn3;
              }
              /* not in the current group; this means that the current group is effectively dead */
              /* remove current group */
              if (curGroup == runGroups) {
                /* main group */
                while (curGroup->group->prev) releaseRunContext();
                aProcess = runGroups->group->process; /* initial process */
                aProcess->data[lstIVresultInProcess] = retValue;
                aProcess->data[lstIVcontextInProcess] = lstNilObj;
                /* clear the current run group */
                while (curGroup->group) releaseRunContext();
                XRETURN(lstReturnReturned); /* done */
              }
              while (curGroup->group) releaseRunContext();
              free(removeCurrentGroup());
              /* inter-group communications should be done with events, so just shedule to the next process */
              nextGroup(0);
              goto doAllAgain;
            }
            goto doReturn2;
          case lstBXDuplicate:
            DBG0("DoSpecial: Duplicate");
            assert(stackTop > 0);
            retValue = stack->data[stackTop-1];
            PUSHIT(retValue);
            break;
          case lstBXPopTop:
            DBG0("DoSpecial: PopTop");
            assert(stackTop > 0);
            --stackTop;
            break;
          case lstBXBranch:
            DBG0("DoSpecial: Branch");
            low = VAL;
            curIP = low;
            break;
          case lstBXBranchIfTrue:
            DBG0("DoSpecial: BranchIfTrue");
            low = VAL;
            retValue = POPIT;
            if (retValue == lstTrueObj) curIP = low; else curIP += VALSIZE;
            break;
          case lstBXBranchIfFalse:
            DBG0("DoSpecial: BranchIfFalse");
            low = VAL;
            retValue = POPIT;
            if (retValue == lstFalseObj) curIP = low; else curIP += VALSIZE;
            break;
          case lstBXBranchIfNil:
            DBG0("DoSpecial: BranchIfNil");
            low = VAL;
            retValue = POPIT;
            if (retValue == lstNilObj) curIP = low; else curIP += VALSIZE;
            break;
          case lstBXBranchIfNotNil:
            DBG0("DoSpecial: BranchIfNotNil");
            low = VAL;
            retValue = POPIT;
            if (retValue != lstNilObj) curIP = low; else curIP += VALSIZE;
            break;
          case lstBXSendToSuper:
            DBG0("DoSpecial: SendToSuper");
            /* next byte has literal selector number */
            low = bp[curIP++];
            messageSelector = literals->data[low];
            receiverClass = method->data[lstIVclassInMethod]->data[lstIVparentClassInClass];
            arguments = POPIT;
            l0 = bp[curIP];
            goto checkCache;
          case lstBXThisContext:
            DBG0("DoSpecial: ThisContext");
            PUSHIT(context);
            break;
          case lstBXBreakpoint:
            DBG0("DoSpecial: Breakpoint");
            /*fprintf(stderr, "BP\n");*/
            /* back up on top of the breaking location */
            --curIP;
            /* return to our master process */
            /*aProcess->data[lstIVresultInProcess] = lstNilObj;*/
            retValue = lstNilObj;
            if (doReturn(lstReturnBreak)) XRETURN(lstReturnBreak);
            if (tmp || retGSwitch) goto doAllAgain;
            goto execComplete;
          default:
            lstFatal("invalid doSpecial", low);
            break;
        }
        break;
      default:
        if (curGroup == runGroups) {
          retValue = lstNilObj;
          if (doReturn(lstReturnError)) XRETURN(lstReturnError);
          fprintf(stderr, "invalid bytecode: %d\n", high);
          if (tmp || retGSwitch) goto doAllAgain;
          goto execComplete;
        }
        lstFatal("invalid bytecode", high);
        break;
    }
  }
}


int lstExecute (lstObject *aProcess, int ticks, int locked) {
  lstResetResume();
  return lstExecuteInternal(aProcess, ticks, locked);
}


int lstResume (void) {
  if (!lstSuspended) return -1; /* very fatal error */
  return lstExecuteInternal(NULL, 0, 0);
}


int lstCanResume (void) {
  return lstSuspended != 0;
}


void lstResetResume (void) {
  if (lstSuspended) {
    lstSuspended = 0;
    curGroup = runGroups;
    while (curGroup->group) releaseRunContext();
  }
}


#define RARG      (lstRootStack[otop+0])
#define RMETHOD   (lstRootStack[otop+1])
#define RPROCESS  (lstRootStack[otop+2])
#define RCONTEXT  (lstRootStack[otop+3])
int lstRunMethodWithArg (lstObject *method, lstObject *inClass, lstObject *arg, lstObject **result, int locked) {
  lstObject *o;
  int otop = lstRootTop, x;
  if (result) *result = NULL;
  /* save method and arguments */
  if (!method || method->stclass != lstMethodClass) return lstReturnError;
  lstRootStack[LST_RSTACK_NSP()] = arg;
  lstRootStack[LST_RSTACK_NSP()] = method;
  /* create Process object */
  lstRootStack[LST_RSTACK_NSP()] = lstAllocInstance(lstProcessSize, lstProcessClass); /*lstStaticAlloc(lstProcessSize);*/
  /* create Context object (must be dynamic) */
  lstRootStack[LST_RSTACK_NSP()] = lstAllocInstance(lstContextSize, lstContextClass);
  RPROCESS->data[lstIVcontextInProcess] = RCONTEXT;
  x = lstIntValue(RMETHOD->data[lstIVstackSizeInMethod]);
  o = lstRootStack[LST_RSTACK_NSP()] = RCONTEXT->data[lstIVstackInContext] = lstAllocInstance(x, lstArrayClass);
  /*if (x) memset(lstBytePtr(o), 0, x*LST_BYTES_PER_WORD);*/
  /* build arguments array */
  o = lstAllocInstance(arg ? 2 : 1, lstArrayClass);
  /*o->data[0] = RCONTEXT;*/
  o->data[0] = inClass ? inClass : lstNilObj->stclass;
  if (arg) o->data[1] = arg;
  RCONTEXT->data[lstIVprocOwnerInContext] = RPROCESS;
  RCONTEXT->data[lstIVargumentsInContext] = o;
  RCONTEXT->data[lstIVtemporariesInContext] = lstAllocInstance(lstIntValue(RMETHOD->data[lstIVtemporarySizeInMethod]), lstArrayClass);
  RCONTEXT->data[lstIVbytePointerInContext] = lstNewInt(0);
  RCONTEXT->data[lstIVstackTopInContext] = lstNewInt(0);
  RCONTEXT->data[lstIVpreviousContextInContext] = lstNilObj;
  RCONTEXT->data[lstIVmethodInContext] = RMETHOD;
  /* now go do it */
  int res = lstExecute(RPROCESS, 0, locked>0);
  if (res == lstReturnReturned && result) *result = RPROCESS->data[lstIVresultInProcess];
  /*printf("OTOP: %d; TOP: %d\n", otop, lstRootTop);*/
  switch (res) {
    case lstReturnBadMethod:
      fprintf(stderr, "can't find method in call\n");
      o = RPROCESS->data[lstIVresultInProcess];
      fprintf(stderr, "Unknown method: %s\n", lstBytePtr(o));
      lstBackTrace(RPROCESS->data[lstIVcontextInProcess]);
      break;
    case lstReturnAPISuspended:
      if (lstExecUserBreak != 666) {
        fprintf(stderr, "\nuser break\n");
        o = RPROCESS->data[lstIVresultInProcess];
        lstBackTrace(RPROCESS->data[lstIVcontextInProcess]);
      }
      break;
  }
  if (lstRootTop > otop) lstRootTop = otop;
  return res;
}


void lstCompleteFinalizers (void) {
  for (;;) {
    lstGC();
    if (finGroupCount < 1) break;
    runOnlyFins = 1;
    dprintf("%d finalizers left\n", finGroupCount);
    lstExecuteInternal(NULL, 10000, 0);
  }
}
