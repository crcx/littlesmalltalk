/*
 * The memory management and garbage collection module
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
/* Uses baker two-space garbage collection algorithm */
#include "lst_dpname.c"

/*#define GC_TIMINGS*/


/* WARNING! LST_MEM_ALIGN MUST BE POWER OF 2! */
#define LST_MEM_ALIGN  2
#if LST_MEM_ALIGN != 1
# define lstAlignAddr(n)  ((void *)(((uintptr_t)n+(LST_MEM_ALIGN-1))&(~(LST_MEM_ALIGN-1))))
#else
# define lstAlignAddr(n)  ((void *)n)
#endif

static const char *imgSign = LST_IMAGE_SIGNATURE;


unsigned int lstGCCount = 0;

typedef struct {
  unsigned char *realStart; /* can be unaligned */
  unsigned char *start; /* starting address */
  unsigned char *end;   /* first byte after the end */
  unsigned char *cur;   /* current free */
} LstMemSpace;

#define STATIC_SPACE  2
/* 0 and 1: dynamic; 2: static */
static LstMemSpace memSpaces[3];
static LstMemSpace *curSpace;
static int curSpaceNo; /* to avoid checks */

static LstFinLink *finListHead = NULL; /* list of objects with finalizers */
static LstFinLink *aliveFinListHead = NULL; /* list of alive objects with finalizers */

static LstFinLink *stFinListHead = NULL; /* list of all known st-finalizable objects */
static LstFinLink *stAliveFinListHead = NULL; /* st-finalizable objects, alive */

static LstFinLink *stWeakListHead = NULL; /* list of all known weak objects */
static LstFinLink *stAliveWeakListHead = NULL; /* weak objects, alive */

static int lstGCSpecialLock = 0;
/*
 * what we will do with 'st-finalized' is this:
 *   a) include them in list (to faster scanning);
 *   b) after GC cycle, create new process group for each
 *      object that must be finalized (if it has 'finalize' method);
 *   c) that's all (well, not such easy, but...)
 * so interpreter will execute all finalizers in the normal sheduling process
 * (and 'finalizable' object will be kept alive either until it's process
 * group is alive, or if it anchors itself in some way)
 */

/*
 * roots for memory access
 * used as bases for garbage collection algorithm
 */
lstObject *lstRootStack[LST_ROOTSTACK_LIMIT];
LstUInt lstRootTop = 0;

/* 4096 should be more than enough to recompile the whole image */
#define STATICROOTLIMIT  4096
static lstObject **staticRoots[STATICROOTLIMIT];
static LstUInt staticRootTop = 0;


lstObject **lstTempStack[LST_TS_LIMIT];
int lstTempSP = 0;

/* test to see if a pointer is in dynamic memory area or not */
#define LST_IS_STATIC(x) \
  ((const unsigned char *)(x) >= memSpaces[STATIC_SPACE].start && \
   (const unsigned char *)(x) < memSpaces[STATIC_SPACE].end)


static void allocMemSpace (LstMemSpace *spc, int size) {
  if (spc->start) {
    unsigned char *n = realloc(spc->start, size);
    if (!n) free(spc->start);
    spc->start = n ? n : malloc(size);
  } else {
    spc->start = malloc(size);
  }
  spc->realStart = spc->start;
  spc->start = lstAlignAddr(spc->start);
  spc->end = spc->start+size;
  spc->cur = spc->start;
}


typedef struct {
  lstObject *obj;
  const char *mname;
} LstObjToFinInfo;
static LstObjToFinInfo *objToFin = NULL;
static int objToFinSize = 0;
static int objToFinUsed = 0;


static void addToFin (lstObject *obj, const char *mname) {
  if (objToFinUsed >= objToFinSize) {
    if (objToFinSize >= 100000) lstFatal("too many objects to finalize", 0x29a);
    int newSz = objToFinSize+1024;
    LstObjToFinInfo *n = realloc(objToFin, sizeof(LstObjToFinInfo)*newSz);
    if (!n) lstFatal("out of memory to finalize info", 0x29a);
    objToFin = n;
    objToFinSize = newSz;
  }
  objToFin[objToFinUsed].obj = obj;
  objToFin[objToFinUsed].mname = mname;
  ++objToFinUsed;
}


/* initialize the memory management system */
void lstMemoryInit (int staticsz, int dynamicsz) {
  /* allocate the memory areas */
  memset(memSpaces, sizeof(memSpaces), 0);
  allocMemSpace(&memSpaces[0], dynamicsz);
  allocMemSpace(&memSpaces[1], dynamicsz);
  allocMemSpace(&memSpaces[2], staticsz);
  if (!memSpaces[0].start || !memSpaces[1].start || !memSpaces[2].start) lstFatal("not enough memory for space allocations\n", 0);
  lstRootTop = 0;
  staticRootTop = 0;
  lstTempSP = 0;
  lstExecUserBreak = 0;
  lstSuspended = 0;
  finGroupCount = 0;
  runOnlyFins = 0;
  curSpaceNo = 1;
  curSpace = &memSpaces[curSpaceNo];
  finListHead = stFinListHead = stWeakListHead = NULL;
  ehList = NULL;
  rsFree = NULL;
  lstGCSpecialLock = 0;
  objToFinUsed = 0;
  /* allocate 'main' process group; this group is for sheduler, for example */
  /* note that this group should be ALWAYS alive */
  runGroups = curGroup = calloc(1, sizeof(LstRunGroup));
  lstFlushMethodCache();
}


void lstMemoryDeinit (void) {
  /*fprintf(stderr, "staticRootTop: %d\n", staticRootTop);*/
  /* finalize all unfinalized objects */
  while (finListHead) {
    LstFinLink *n = finListHead->next;
    if (finListHead->fin) finListHead->fin(finListHead->obj, finListHead->udata);
    free(finListHead);
    finListHead = n;
  }
  /* free all st-finalizable objects */
  while (stFinListHead) {
    LstFinLink *n = stFinListHead->next;
    free(stFinListHead);
    stFinListHead = n;
  }
  while (stWeakListHead) {
    LstFinLink *n = stWeakListHead->next;
    free(stWeakListHead);
    stWeakListHead = n;
  }
  /* free groups and other structures */
  while (ehList) {
    LstEventHandler *n = ehList->next;
    free(ehList);
    ehList = n;
  }
  while (runGroups) {
    LstRunGroup *n = runGroups->next;
    LstRunContext *ctx = runGroups->group;
    while (ctx) {
      LstRunContext *p = ctx->prev;
      free(ctx);
      ctx = p;
    }
    free(runGroups);
    runGroups = n;
  }
  while (rsFree) {
    LstRunContext *p = rsFree->prev;
    free(rsFree);
    rsFree = p;
  }
  if (objToFin) free(objToFin); objToFin = NULL;
  objToFinSize = 0;
  lstRootTop = 0;
  staticRootTop = 0;
  curSpace = NULL;
  free(memSpaces[2].realStart);
  free(memSpaces[1].realStart);
  free(memSpaces[0].realStart);
}


static inline void lstRemoveFromFList (LstFinLink **head, LstFinLink *item) {
  if (item->prev) item->prev->next = item->next; else *head = item->next;
  if (item->next) item->next->prev = item->prev;
}


static inline void lstAddToFList (LstFinLink **head, LstFinLink *item) {
  item->prev = NULL;
  if ((item->next = *head)) item->next->prev = item;
  *head = item;
}


static inline int LST_IS_NEW (const lstObject *o) {
  return
    (const unsigned char *)o >= curSpace->start &&
    (const unsigned char *)o < curSpace->end;
}


/*
 * garbage collector
 * this is a classic Cheney two-finger collector
 */

/* curSpace and curSpaceNo should be set to the new space */
/* copy object to the new space, return new address */
static lstObject *gcMoveObject (lstObject *o) {
  lstObject *res;
  unsigned char *next;
  int size;
  if (!o || LST_IS_SMALLINT(o)) return o; /* use as-is */
  if (LST_IS_MOVED(o)) return o->stclass; /* already relocated */
  if (LST_IS_STATIC(o)) return o; /* static object, use as-is */
  if (LST_IS_NEW(o)) {
    /*fprintf(stderr, "DOUBLE MOVING!\n");*/
    /* *((char *)0) = 0; */
    return o;
  }
  if (o->fin) {
    /* move to 'alive with finalizers' list */
    if (LST_IS_STFIN(o)) {
      dprintf("STFIN!\n");
      lstRemoveFromFList(&stFinListHead, o->fin);
      lstAddToFList(&stAliveFinListHead, o->fin);
    } else if (LST_IS_WEAK(o)) {
      dprintf("STWEAK!\n");
      lstRemoveFromFList(&stWeakListHead, o->fin);
      lstAddToFList(&stAliveWeakListHead, o->fin);
    } else {
      /*dprintf("CFIN!\n");*/
      lstRemoveFromFList(&finListHead, o->fin);
      lstAddToFList(&aliveFinListHead, o->fin);
    }
  }
  /* copy object to another space and setup redirection pointer */
  size = LST_SIZE(o);
  res = (lstObject *)curSpace->cur;
  memcpy(res, o, sizeof(lstObject));
  if (o->fin) o->fin->obj = res; /* fix finowner */
  /* setup redirection pointer */
  LST_MARK_MOVED(o);
  o->stclass = res;
  next = curSpace->cur+sizeof(lstObject);
  if (!LST_IS_BYTES(o)) size *= LST_BYTES_PER_WORD; else ++size; /* byte objects are always has 0 as the last item */
  if (size > 0) memcpy(&res->data, &o->data, size);
  next += size;
  curSpace->cur = lstAlignAddr(next);
  return res;
}


/* return Process object */
static lstObject *lstCreateMethodCall (lstObject *obj, const char *mname) {
  lstObject *method = lstFindMethod(obj->stclass, mname);
  if (!method) lstFatal("no #finalize or #mourn method found for object", 0x29a);
  lstObject *process = lstAllocInstance(lstProcessSize, lstProcessClass);
  lstObject *context = lstAllocInstance(lstContextSize, lstContextClass);
  process->data[lstIVcontextInProcess] = context;
  process->data[lstIVrunningInProcess] = lstTrueObj;
  /***/
  context->data[lstIVmethodInContext] = method;
  /* build arguments array */
  lstObject *args = lstAllocInstance(1, lstArrayClass);
  args->data[0] = obj; /* self */
  context->data[lstIVargumentsInContext] = args;
  context->data[lstIVtemporariesInContext] = lstAllocInstance(lstIntValue(method->data[lstIVtemporarySizeInMethod]), lstArrayClass);
  context->data[lstIVstackInContext] = lstAllocInstance(lstIntValue(method->data[lstIVstackSizeInMethod]), lstArrayClass);
  context->data[lstIVbytePointerInContext] = lstNewInt(0);
  context->data[lstIVstackTopInContext] = lstNewInt(0);
  context->data[lstIVpreviousContextInContext] = lstNilObj;
  context->data[lstIVprocOwnerInContext] = process;
  return process;
}


/*
static int lstIsKindOfForGC (const lstObject *obj, const lstObject *aclass) {
  const lstObject *pclass = obj;
  int stC = 0;
  if (!obj || !aclass) return 0;
  if (obj == aclass) return 1;
  if (LST_IS_SMALLINT(obj)) {
    if (LST_IS_SMALLINT(aclass)) return 1;
    obj = lstSmallIntClass;
  } else {
    if (LST_IS_SMALLINT(aclass)) aclass = lstSmallIntClass;
    if (!LST_IS_SMALLINT(obj)
    obj = obj->stclass;
  }
  while (obj && obj != lstNilObj) {
    if (obj == aclass) return 1;
    obj = obj->data[lstIVparentClassInClass];
    if (stC) { if (pclass && pclass != lstNilObj) pclass = pclass->data[lstIVparentClassInClass]; }
    else if (pclass == obj) return 0;
    stC ^= 1;
  }
  return 0;
}
*/


/* res>0: at least one object is dead */
static int processWeakData (lstObject *o) {
  int hitCount = 0;
  int size = LST_SIZE(o);
  while (--size >= 0) {
    lstObject *d = o->data[size];
    if (LST_IS_SMALLINT(d) || LST_IS_STATIC(d)) continue; /* nothing to fix */
    if (LST_IS_MOVED(d)) {
      /* object is alive */
      o->data[size] = d->stclass;
    } else {
      /* do not throw out numbers */
      /*FIXME: we can have infinite loop here */
      lstObject *cls = d->stclass;
      while (cls && cls != lstNilObj) {
        while (LST_IS_MOVED(cls)) cls = cls->stclass;
        if (cls == lstNumberClass) {
          /* save the number */
          o->data[size] = gcMoveObject(d);
          continue;
        }
        cls = cls->data[lstIVparentClassInClass];
      }
      /* object is dead; add #mourn call for it and replace link to nil */
      dprintf("found someone to mourn\n");
      ++hitCount;
      gcMoveObject(d);
      o->data[size] = lstNilObj;
    }
  }
  return hitCount;
}


#ifdef GC_TIMINGS
# ifndef LST_ON_WINDOWS
#  include <signal.h>
#  include <time.h>
# else
#  include <windows.h>
# endif
static uint64_t getTicksMS (void) {
#ifndef _WIN32
  uint64_t res;
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  res = ((uint64_t)ts.tv_sec)*100000UL;
  res += ((uint64_t)ts.tv_nsec)/10000UL; //1000000000
  return res;
#else
  return GetTickCount()*100;
#endif
}
#endif


#define GC_KEEP_METHOD_CACHE

/* garbage collection entry point */
void lstGC (void) {
  LstFinLink *weakAlive = NULL;
  int f;
#ifdef GC_TIMINGS
  uint64_t gcTime = getTicksMS();
#endif
#ifdef DEBUG
  int saved = 0;
#endif
  if (curSpace->cur == curSpace->start) return; /* nothing to do */
  if (lstGCSpecialLock) lstFatal("out of memory for finalizer groups", lstGCSpecialLock);
  lstGCCount++;
  objToFinUsed = 0;
  aliveFinListHead = stAliveFinListHead = stAliveWeakListHead = NULL;
  /* first change spaces */
  unsigned char *scanPtr;
  curSpaceNo ^= 1;
  curSpace = &memSpaces[curSpaceNo];
  curSpace->cur = curSpace->start;

  /* move all roots */
  lstNilObj = gcMoveObject(lstNilObj);
  lstTrueObj = gcMoveObject(lstTrueObj);
  lstFalseObj = gcMoveObject(lstFalseObj);
  lstBadMethodSym = gcMoveObject(lstBadMethodSym);
  for (f = 0; f < LST_MAX_BIN_MSG; ++f) lstBinMsgs[f] = gcMoveObject(lstBinMsgs[f]);
  lstGlobalObj = gcMoveObject(lstGlobalObj);

  for (f = 0; f < lstRootTop; ++f) lstRootStack[f] = gcMoveObject(lstRootStack[f]);
  for (f = 0; f < staticRootTop; ++f) (*staticRoots[f]) = gcMoveObject(*staticRoots[f]);
  for (f = 0; f < lstTempSP; ++f) (*lstTempStack[f]) = gcMoveObject(*lstTempStack[f]);
  /* the following are mostly static, but who knows... */
  for (f = 0; clInfo[f].name; ++f) (*(clInfo[f].eptr)) = gcMoveObject(*(clInfo[f].eptr));
  for (f = 0; epInfo[f].name; ++f) (*(epInfo[f].eptr)) = gcMoveObject(*(epInfo[f].eptr));

  /* mark process groups */
  {
    LstRunGroup *grp;
    for (grp = runGroups; grp; grp = grp->next) {
      LstRunContext *ctx;
      for (ctx = grp->group; ctx; ctx = ctx->prev) ctx->process = gcMoveObject(ctx->process);
    }
  }
#ifdef GC_KEEP_METHOD_CACHE
  /* fix method cache; this have some sense, as many method calls are stdlib calls */
  /* when we'll do generational GC, this will have even better impact on cache hits */
  /* note that this is completely safe: SendMessage will do the necessary checks */
  for (f = 0; f < MTD_CACHE_SIZE+MTD_CACHE_EXTRA; ++f) {
    if (cache[f].name) {
      /* fix method */
      cache[f].name = gcMoveObject(cache[f].name);
      cache[f].stclass = gcMoveObject(cache[f].stclass);
      cache[f].method = gcMoveObject(cache[f].method);
      if (cache[f].analyzed == 1 && cache[f].ivarNum < 0) cache[f].mConst = gcMoveObject(cache[f].mConst);
      cache[f].badHits = MTD_BAD_HIT_MAX-2;
    }
  }
#endif
#ifdef INLINE_SOME_METHODS
  lstMetaCharClass = gcMoveObject(lstMetaCharClass);
  for (f = 0; lstInlineMethodList[f].name; ++f) {
    //if (!LST_IS_NEW(*lstInlineMethodList[f].mtclass)) abort();
    //(*lstInlineMethodList[f].mtclass) = gcMoveObject(*lstInlineMethodList[f].mtclass);
    (*lstInlineMethodList[f].method) = gcMoveObject(*lstInlineMethodList[f].method);
  }
#endif

  /* now walk thru the objects, fix pointers, move other objects, etc.
   * note that curSpace->cur will grow in the process */
  scanPtr = curSpace->start;
scanAgain:
  while (scanPtr < curSpace->cur) {
    lstObject *o = (lstObject *)scanPtr;
    scanPtr += sizeof(lstObject);
    /* fix class */
    o->stclass = gcMoveObject(o->stclass);
    int size = LST_SIZE(o);
    if (LST_IS_BYTES(o)) {
      /* nothing to scan here */
      scanPtr += size+1; /* skip zero byte too */
    } else {
      /* process object data, if this is not weak object */
      scanPtr += size*LST_BYTES_PER_WORD;
      if (!LST_IS_WEAK(o)) while (--size >= 0) o->data[size] = gcMoveObject(o->data[size]);
    }
    scanPtr = lstAlignAddr(scanPtr);
#ifdef DEBUG
    ++saved;
#endif
  }
  /* process weak objects */
  int wasWLHit = 0;
  /* alive weak objects */
  /* save 'em to another accumulation list */
  if (stAliveWeakListHead) {
    lstAddToFList(&weakAlive, stAliveWeakListHead);
    while (stAliveWeakListHead) {
      lstObject *o = stAliveWeakListHead->obj;
      assert(o->fin == stAliveWeakListHead);
      stAliveWeakListHead = stAliveWeakListHead->next;
      if (processWeakData(o)) {
        ++wasWLHit;
        addToFin(o, "mourn");
      }
    }
  }
  if (wasWLHit) goto scanAgain;
  /* process ST finalizers, if any */
  if (stFinListHead) {
    dprintf("FOUND SOME ST-FINALIZERS!\n");
    while (stFinListHead) {
      lstObject *o = stFinListHead->obj;
      assert(o->fin == stFinListHead);
      LstFinLink *n = stFinListHead->next;
      free(stFinListHead);
      stFinListHead = n;
      /* now remove the flag and create new process group */
      LST_RESET_STFIN(o);
      o->fin = NULL; /* it is already freed */
      o = gcMoveObject(o);
      dprintf("FINOBJ: %p\n", o);
      addToFin(o, "finalize");
    }
    /* scan new space to save ST-F anchors; no need to rescan process groups though */
    goto scanAgain;
  }
  stFinListHead = stAliveFinListHead;
  /* dead weak objects */
  while (stWeakListHead) {
#ifndef NDEBUG
    lstObject *o = stWeakListHead->obj;
    assert(o->fin == stWeakListHead);
#endif
    LstFinLink *n = stWeakListHead->next;
    free(stWeakListHead);
    stWeakListHead = n;
  }
  stWeakListHead = weakAlive;
  /* here we can process C finalizers, if any */
  while (finListHead) {
    LstFinLink *n = finListHead->next;
    if (finListHead->fin) finListHead->fin(finListHead->obj, finListHead->udata);
    free(finListHead);
    finListHead = n;
  }
  finListHead = aliveFinListHead; /* 'alive' list becomes the current one */
  /* now check if we have something to mourn/finalize */
  ++lstGCSpecialLock;
  for (f = 0; f < objToFinUsed; ++f) {
    dprintf("FOUND some finalizing obj(%p) method(#%s)\n", objToFin[f].obj, objToFin[f].mname);
    lstCreateFinalizePGroup(lstCreateMethodCall(objToFin[f].obj, objToFin[f].mname));
#ifdef DEBUG
    /*lstDebugFlag = 1;*/
#endif
  }
  --lstGCSpecialLock;
  /* invalidate method cache */
#ifndef GC_KEEP_METHOD_CACHE
  lstFlushMethodCache();
#endif
/*
#ifdef DEBUG
 dprintf("GC: %d objects alive; %u bytes used\n", saved, (uintptr_t)curSpace->cur-(uintptr_t)curSpace->start);
#endif
*/
#ifdef GC_TIMINGS
  gcTime = getTicksMS()-gcTime;
  fprintf(stderr, "GC TIME: %u\n", (uint32_t)gcTime);
#endif
}


typedef struct {
  char clname[258];
  char mtname[258];
  int callCount;
} LstCallInfo;

static LstCallInfo *cinfo = NULL;
static int cinfoUsed = 0;

static void lstProcessSpace (int num) {
  LstMemSpace *curSpace = &memSpaces[num];
  unsigned char *scanPtr = curSpace->start;
  while (scanPtr < curSpace->cur) {
    lstObject *o = (lstObject *)scanPtr;
    if (LST_CLASS(o) == lstMethodClass) {
      lstObject *op = o->data[lstIVinvokeCountInMethod];
      int cc = lstIntValue(op);
      if (cc > 0) {
        cinfo = realloc(cinfo, sizeof(LstCallInfo)*(cinfoUsed+1));
        lstGetString(cinfo[cinfoUsed].mtname, sizeof(cinfo[cinfoUsed].mtname), o->data[lstIVnameInMethod]);
        op = o->data[lstIVclassInMethod];
        lstGetString(cinfo[cinfoUsed].clname, sizeof(cinfo[cinfoUsed].clname), op->data[lstIVnameInClass]);
        cinfo[cinfoUsed].callCount = cc;
        ++cinfoUsed;
      }
    }
    scanPtr += sizeof(lstObject);
    int size = LST_SIZE(o);
    if (LST_IS_BYTES(o)) {
      scanPtr += size+1;
    } else {
      scanPtr += size*LST_BYTES_PER_WORD;
    }
    scanPtr = lstAlignAddr(scanPtr);
  }
}


void lstShowCalledMethods (void) {
  int f;
  int xcmp (const void *i0, const void *i1) {
    const LstCallInfo *l0 = (const LstCallInfo *)i0;
    const LstCallInfo *l1 = (const LstCallInfo *)i1;
    return l1->callCount-l0->callCount;
  }
  lstGC(); /* compact objects */
  lstProcessSpace(2); /* static */
  lstProcessSpace(curSpaceNo); /* dynamic */
  if (cinfoUsed > 0) {
    qsort(cinfo, cinfoUsed, sizeof(LstCallInfo), xcmp);
    for (f = 0; f < cinfoUsed; ++f) {
      fprintf(stderr, "[%s>>%s]: %d\n", cinfo[f].clname, cinfo[f].mtname, cinfo[f].callCount);
    }
    free(cinfo);
    cinfo = NULL;
    cinfoUsed = 0;
  }
}


#define STATIC_ALLOC \
  lstObject *res = (lstObject *)memSpaces[STATIC_SPACE].cur; \
  unsigned char *next = memSpaces[STATIC_SPACE].cur; \
  next += realSz; \
  next = lstAlignAddr(next); \
  if (next > memSpaces[STATIC_SPACE].end) lstFatal("insufficient static memory", sz); \
  memSpaces[STATIC_SPACE].cur = next; \
  LST_SETSIZE(res, sz); \
  res->fin = 0; \
  res->objFlags = 0;

/*
 * static allocation -- tries to allocate values in an area
 * that will not be subject to garbage collection
*/
lstByteObject *lstStaticAllocBin (int sz) {
  int realSz = sz+sizeof(lstObject)+1;
  STATIC_ALLOC
  LST_SETBIN(res);
  lstBytePtr(res)[sz] = '\0';
  return (lstByteObject *)res;
}


lstObject *lstStaticAlloc (int sz) {
  int realSz = sz*LST_BYTES_PER_WORD+sizeof(lstObject);
  STATIC_ALLOC
  return res;
}


#define DYNAMIC_ALLOC \
  lstObject *res = (lstObject *)curSpace->cur; \
  unsigned char *next = curSpace->cur; \
  next += realSz; \
  next = lstAlignAddr(next); \
  if (next > curSpace->end) { \
    lstGC(); \
    res = (lstObject *)curSpace->cur; \
    next = curSpace->cur; \
    next += realSz; \
    next = lstAlignAddr(next); \
    if (next > curSpace->end) lstFatal("insufficient memory", sz); \
  } \
  curSpace->cur = next; \
  LST_SETSIZE(res, sz); \
  res->fin = 0; \
  res->objFlags = 0;


lstByteObject *lstMemAllocBin (int sz) {
  int realSz = sz+sizeof(lstObject)+1;
  DYNAMIC_ALLOC
  LST_SETBIN(res);
  lstBytePtr(res)[sz] = '\0';
  return (lstByteObject *)res;
}


lstObject *lstMemAlloc (int sz) {
  int realSz = sz*LST_BYTES_PER_WORD+sizeof(lstObject);
  DYNAMIC_ALLOC
  return res;
}


#include "lst_imagerw.c"


/*
 * Add another object root off a static object
 *
 * Static objects, in general, do not get garbage collected.
 * When a static object is discovered adding a reference to a
 * non-static object, we link on the reference to our staticRoot
 * table so we can give it proper treatment during garbage collection.
 */
void lstAddStaticRoot (lstObject **objp) {
  int f, rfree = -1;
  for (f = 0; f < staticRootTop; ++f) {
    if (objp == staticRoots[f]) return;
    if (rfree < 0 && !staticRoots[f]) rfree = f;
  }
  if (rfree < 0) {
    if (staticRootTop >= STATICROOTLIMIT) lstFatal("lstAddStaticRoot: too many static references", (intptr_t)objp);
    rfree = staticRootTop++;
  }
  staticRoots[rfree] = objp;
}


void lstWriteBarrier (lstObject **dest, lstObject *src) {
  if (LST_IS_STATIC(dest) && !LST_IS_SMALLINT(src) && !LST_IS_STATIC(src)) {
    int f, rfree = -1;
    for (f = 0; f < staticRootTop; ++f) {
      if (staticRoots[f] == dest) goto doit; /* let TEH GOTO be here! */
      if (rfree < 0 && !staticRoots[f]) rfree = f;
    }
    if (rfree < 0) {
      if (staticRootTop >= STATICROOTLIMIT) lstFatal("lstWriteBarrier: too many static references", (intptr_t)dest);
      rfree = staticRootTop++;
    }
    staticRoots[rfree] = dest;
  }
doit:
  *dest = src;
}


/* fix an OOP if needed, based on values to be exchanged */
static void map (lstObject **oop, lstObject *a1, lstObject *a2, int size) {
  int x;
  lstObject *oo = *oop;
  for (x = 0; x < size; ++x) {
    if (a1->data[x] == oo) {
      *oop = a2->data[x];
      return;
    }
    if (a2->data[x] == oo) {
      *oop = a1->data[x];
      return;
    }
  }
}


/* traverse an object space */
static void walk (lstObject *base, lstObject *top, lstObject *array1, lstObject *array2, LstUInt size) {
  lstObject *op, *opnext;
  LstUInt x, sz;
  for (op = base; op < top; op = opnext) {
    /* re-map the class pointer, in case that's the object which has been remapped */
    map(&op->stclass, array1, array2, size);
    /* skip our argument arrays, since otherwise things get rather circular */
    sz = LST_SIZE(op);
    if (op == array1 || op == array2) {
      unsigned char *t = (unsigned char *)op;
      t += sz*LST_BYTES_PER_WORD+sizeof(lstObject);
      opnext = lstAlignAddr(t);
      continue;
    }
    /* don't have to worry about instance variables if it's a binary format */
    if (LST_IS_BYTES(op)) {
      LstUInt realSize = sz+sizeof(lstObject)+1;
      unsigned char *t = (unsigned char *)op;
      t += realSize;
      opnext = lstAlignAddr(t);
      continue;
    }
    /* for each instance variable slot, fix up the pointer if needed */
    for (x = 0; x < sz; ++x) map(&op->data[x], array1, array2, size);
    /* walk past this object */
    {
      LstUInt realSize = sz*LST_BYTES_PER_WORD+sizeof(lstObject);
      unsigned char *t = (unsigned char *)op;
      t += realSize;
      opnext = lstAlignAddr(t);
    }
  }
}


/*
 * Bulk exchange of object identities
 *
 * For each index to array1/array2, all references in current object
 * memory are modified so that references to the object in array1[]
 * become references to the corresponding object in array2[].  References
 * to the object in array2[] similarly become references to the
 * object in array1[].
 */
void lstSwapObjects (lstObject *array1, lstObject *array2, LstUInt size) {
  LstUInt x;
  /* Convert our memory spaces */
  walk((lstObject *)curSpace->start, (lstObject *)curSpace->cur, array1, array2, size);
  walk((lstObject *)memSpaces[STATIC_SPACE].start, (lstObject *)memSpaces[STATIC_SPACE].cur, array1, array2, size);
  /* Fix up the root pointers, too */
  for (x = 0; x < lstRootTop; x++) map(&lstRootStack[x], array1, array2, size);
  for (x = 0; x < staticRootTop; x++) map(staticRoots[x], array1, array2, size);
}


/*
 * Implement replaceFrom:to:with:startingAt: as a primitive
 *
 * Return 1 if we couldn't do it, 0 on success.
 * This routine has distinct code paths for plain old byte type arrays,
 * and for arrays of lstObject pointers; the latter must handle the
 * special case of static pointers. It looks hairy (and it is), but
 * it's still much faster than executing the block move in Smalltalk
 * VM opcodes.
 */
int lstBulkReplace (lstObject *dest, lstObject *aFrom, lstObject *aTo, lstObject *aWith, lstObject *startAt) {
  LstUInt irepStart, istart, istop, count;
  /* we only handle simple 31-bit integer indices; map the values onto 0-based C array type values */
  if (!LST_IS_SMALLINT(startAt) || !LST_IS_SMALLINT(aFrom) || !LST_IS_SMALLINT(aTo)) return 1;
  if (LST_IS_SMALLINT(dest) || LST_IS_SMALLINT(aWith)) return 1;
  irepStart = lstIntValue(startAt)-1;
  istart = lstIntValue(aFrom)-1;
  istop = lstIntValue(aTo)-1;
  count = (istop-istart)+1;
  /* defend against goofy negative indices */
  if (count <= 0) return 0;
  if (irepStart < 0 || istart < 0 || istop < 0) return 1;
  /* range check */
  if (LST_SIZE(dest) < istop || LST_SIZE(aWith) < irepStart+count) return 1;
  /* if both source and dest are binary, just copy some bytes */
  if (LST_IS_BYTES(aWith) && LST_IS_BYTES(dest)) {
    memmove(lstBytePtr(dest)+istart, lstBytePtr(aWith)+irepStart, count);
    return 0;
  }
  /* fail if only one of objects is binary */
  if (LST_IS_BYTES(aWith) || LST_IS_BYTES(dest)) return 1;
  /* if we're fiddling pointers between static and dynamic memory, register roots */
  /* note that moving from static to dynamic is ok, but the reverse needs some housekeeping */
  if (LST_IS_STATIC(dest) && !LST_IS_STATIC(aWith)) {
    LstUInt f;
    /*fprintf(stderr, "!!!: count=%u\n", count);*/
    for (f = 0; f < count; ++f) lstAddStaticRoot(&dest->data[istart+f]);
  }
  /* copy object pointer fields */
  memmove(&dest->data[istart], &aWith->data[irepStart], LST_BYTES_PER_WORD*count);
  return 0;
}


lstObject *lstNewString (const char *str) {
  int l = str ? strlen(str) : 0;
  lstByteObject *strobj = lstMemAllocBin(l);
  strobj->stclass = lstStringClass;
  if (l > 0) memcpy(lstBytePtr(strobj), str, l);
  lstBytePtr(strobj)[l] = '\0';
  return (lstObject *)strobj;
}


int lstGetString (char *buf, int bufsize, const lstObject *str) {
  int fsize = LST_SIZE(str), csz = fsize;
  if (csz > bufsize-1) csz = bufsize-1;
  if (buf && csz > 0) memcpy(buf, &str->data, csz);
  if (csz >= 0) buf[csz] = '\0'; /* put null terminator at end */
  if (fsize > bufsize-1) return fsize+1;
  return 0; /* ok */
}


char *lstGetStringPtr (const lstObject *str) {
  return (char *)(lstBytePtr(str));
}


lstObject *lstNewBinary (const void *p, LstUInt l) {
  lstByteObject *bobj = lstMemAllocBin(l);
  bobj->stclass = lstByteArrayClass;
  if (l > 0) {
    if (p) memcpy(lstBytePtr(bobj), p, l); else memset(lstBytePtr(bobj), 0, l);
  }
  return (lstObject *)bobj;
}


lstObject *lstNewBCode (const void *p, LstUInt l) {
  lstByteObject *bobj = lstMemAllocBin(l);
  bobj->stclass = lstByteCodeClass;
  if (l > 0) {
    if (p) memcpy(lstBytePtr(bobj), p, l); else memset(lstBytePtr(bobj), 0, l);
  }
  return (lstObject *)bobj;
}


lstObject *lstAllocInstance (int size, lstObject *cl) {
  int f;
  if (size < 0) return NULL;
  lstRootStack[lstRootTop++] = cl;
  lstObject *obj = lstMemAlloc(size);
  obj->stclass = lstRootStack[--lstRootTop];
  for (f = 0; f < size; ++f) obj->data[f] = lstNilObj;
  return obj;
}


/* create new Integer (64-bit) */
lstObject *lstNewLongInt (LstLInt val) {
  lstByteObject *res = lstMemAllocBin(sizeof(LstLInt));
  res->stclass = lstIntegerClass;
  memcpy(lstBytePtr(res), &val, sizeof(val));
  return (lstObject *)res;
}


lstObject *lstNewFloat (LstFloat val) {
  lstByteObject *res = lstMemAllocBin(sizeof(LstFloat));
  res->stclass = lstFloatClass;
  memcpy(lstBytePtr(res), &val, sizeof(val));
  return (lstObject *)res;
}


lstObject *lstNewArray (int size) {
  if (size < 0) return NULL;
  return lstAllocInstance(size, lstArrayClass);
}


static int symbolBareCmp (const char *left, int leftsize, const char *right, int rightsize) {
  int minsize = leftsize;
  int i;
  if (rightsize < minsize) minsize = rightsize;
  if (minsize > 0) {
    if ((i = memcmp(left, right, minsize))) return i;
  }
  return leftsize-rightsize;
}


/*
static int symbolCmp (const lstObject *s0, const lstObject *s1) {
  return symbolBareCmp((const char *)lstBytePtr(s0), LST_SIZE(s0), (const char *)lstBytePtr(s1), LST_SIZE(s1));
}
*/


lstObject *lstDictFind (const lstObject *dict, const char *name) {
  /* binary search */
  const lstObject *keys = dict->data[0];
  int l = 0, h = LST_SIZE(keys)-1, nlen = strlen(name);
  while (l <= h) {
    int mid = (l+h)/2;
    const lstObject *key = keys->data[mid];
    int res = symbolBareCmp(name, nlen, (char *)lstBytePtr(key), LST_SIZE(key));
    if (res == 0) return dict->data[1]->data[mid];
    if (res < 0) h = mid-1; else l = mid+1;
  }
  return NULL;
}


int lstIsKindOf (const lstObject *obj, const lstObject *aclass) {
  const lstObject *pclass = obj;
  int stC = 0;
  if (!obj || !aclass) return 0;
  if (obj == aclass) return 1;
  if (LST_IS_SMALLINT(obj)) {
    if (LST_IS_SMALLINT(aclass)) return 1;
    obj = lstSmallIntClass;
  } else {
    if (LST_IS_SMALLINT(aclass)) aclass = lstSmallIntClass;
    obj = obj->stclass;
  }
  while (obj && obj != lstNilObj) {
    /*printf(" : [%s]\n", lstBytePtr(obj->data[lstIVnameInClass]));*/
    if (obj == aclass) return 1;
    obj = obj->data[lstIVparentClassInClass];
    if (stC) { if (pclass && pclass != lstNilObj) pclass = pclass->data[lstIVparentClassInClass]; }
    else if (pclass == obj) return 0; /* avoid cycles */
    stC ^= 1;
  }
  return 0;
}


lstObject *lstFindGlobal (const char *name) {
  if (!name || !name[0]) return NULL;
  return lstDictFind(lstGlobalObj, name);
}


lstObject *lstFindMethod (lstObject *stclass, const char *method) {
  lstObject *dict, *res;
  /* scan upward through the class hierarchy */
  for (; stclass && stclass != lstNilObj; stclass = stclass->data[lstIVparentClassInClass]) {
    /* consider the Dictionary of methods for this Class */
#if 0 & defined(DEBUG)
    {
      fprintf(stderr, "st=%p; u=%p; sz=%d\n", stclass, lstNilObj, LST_SIZE(stclass));
      fprintf(stderr, "  [%s]\n", lstGetStringPtr(stclass->data[lstIVnameInClass]));
    }
#endif
#ifdef DEBUG
    if (LST_IS_SMALLINT(stclass)) lstFatal("lookupMethod: looking in SmallInt instance", 0);
    if (LST_IS_BYTES(stclass)) lstFatal("lookupMethod: looking in binary object", 0);
    if (LST_SIZE(stclass) < lstClassSize) lstFatal("lookupMethod: looking in non-class object", 0);
#endif
    dict = stclass->data[lstIVmethodsInClass];
#ifdef DEBUG
    if (!dict) lstFatal("lookupMethod: NULL dictionary", 0);
    if (LST_IS_SMALLINT(dict)) lstFatal("lookupMethod: SmallInt dictionary", 0);
    if (dict->stclass != lstFindGlobal("Dictionary")) lstFatal("lookupMethod: method list is not a dictionary", 0);
#endif
    res = lstDictFind(dict, method);
    if (res) return res;
  }
  return NULL;
}


lstObject *lstNewSymbol (const char *name) {
  lstObject *res = NULL;
  if (!name || !name[0]) return NULL;
  lstObject *str = lstNewString(name);
  if (lstRunMethodWithArg(lstNewSymMethod, NULL, str, &res, 1) != lstReturnReturned) return NULL;
  if (!res || res->stclass != lstSymbolClass) return NULL;
  return res;
}


int lstSetGlobal (const char *name, lstObject *val) {
  if (!name || !name[0]) return -2;
  lstRootStack[lstRootTop++] = val;
  lstObject *aa = lstNewArray(2);
  lstRootStack[lstRootTop++] = aa;
  lstRootStack[lstRootTop-1]->data[0] = lstNewString(name);
  lstRootStack[lstRootTop-1]->data[1] = lstRootStack[lstRootTop-2];
  aa = lstRootStack[--lstRootTop];
  --lstRootTop;
  if (lstRunMethodWithArg(lstSetGlobMethod, NULL, aa, NULL, 1) != lstReturnReturned) return -1;
  return 0;
}


void lstSetFinalizer (lstObject *o, LstFinalizerFn fn, void *udata) {
  if (LST_IS_SMALLINT(o) || LST_IS_STATIC(o)) return; /* note that static objects can't have finalizer */
  if (o->fin) {
    lstRemoveFromFList(&finListHead, o->fin);
  } else {
    o->fin = malloc(sizeof(LstFinLink));
  }
  o->fin->fin = fn;
  o->fin->udata = udata;
  o->fin->obj = o;
  lstAddToFList(&finListHead, o->fin);
}


void *lstGetUData (lstObject *o) {
  if (LST_IS_SMALLINT(o) || !o->fin) return NULL;
  return o->fin->udata;
}


lstObject *lstNewChar (int ch) {
  if (ch < 0 || ch > 255) return NULL;
  return lstCharClass->data[lstIVcharsInMetaChar]->data[ch];
}
