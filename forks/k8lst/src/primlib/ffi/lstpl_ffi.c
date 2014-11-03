/*
 * The foreign function interface primitives of the LittleSmalltalk system
 *
 * ---------------------------------------------------------------
 * Little Smalltalk, Version 5
 *
 * Copyright (C) 1987-2005 by Timothy A. Budd
 * Copyright (C) 2007 by Charles R. Childers
 * Copyright (C) 2005-2007 by Danny Reinhold
 *
 * ============================================================================
 * This license applies to the virtual machine and to the initial image of
 * the Little Smalltalk system and to all files in the Little Smalltalk
 * packages.
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lstcore/k8lst.h"
#include "lstpl_ffi.h"


/******************************************************************************/
typedef struct LibInfo LibInfo;

typedef int (*LoadLibFn) (int);


typedef enum {
  RSS_DL_LAZY,
  RSS_DL_NOW
} FfiLoadLibMode;


#ifdef LST_ON_WINDOWS
# include "lstpl_ffi_win.c"
#else
# include "lstpl_ffi_nix.c"
#endif


static const char *ffiLibId = "FfiLibraryHandle";

typedef struct {
  const char *type;
  LibInfo *h;
} LibCellInfo;

LST_FINALIZER(lpLibHandleFinalizer) {
  LibCellInfo *ud = (LibCellInfo *)udata;
  if (ud) {
    ffiCloseLib(ud->h);
    free(ud);
  }
}


static lstObject *newLibHandle (LibInfo *h) {
  LibCellInfo *ud;
  if (!h) return lstNilObj;
  if (!h->handle) { free(h); return lstNilObj; }
  lstObject *res = lstNewBinary(NULL, 0);
  ud = malloc(sizeof(LibCellInfo));
  if (!ud) { ffiCloseLib(h); return lstNilObj; }
  ud->type = ffiLibId;
  ud->h = h;
  lstSetFinalizer(res, lpLibHandleFinalizer, ud);
  return res;
}


static LibInfo *getLib (lstObject *o) {
  if (!LST_IS_BYTES_EX(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  LibCellInfo *ud = (LibCellInfo *)o->fin->udata;
  if (ud->type != ffiLibId) return NULL;
  return ud->h;
}


static const char *ffiFuncId = "FfiFunctionHandle";

typedef struct {
  const char *type;
  void *addr;
} LibFuncCellInfo;

LST_FINALIZER(lpLibFuncFinalizer) {
  LibFuncCellInfo *ud = (LibFuncCellInfo *)udata;
  if (ud) free(ud);
}


static lstObject *newLibFunc (void *addr) {
  LibFuncCellInfo *ud;
  if (!addr) return lstNilObj;
  lstObject *res = lstNewBinary(NULL, 0);
  ud = malloc(sizeof(LibFuncCellInfo));
  if (!ud) return lstNilObj;
  ud->type = ffiFuncId;
  ud->addr = addr;
  lstSetFinalizer(res, lpLibFuncFinalizer, ud);
  return res;
}


static void *getFunc (lstObject *o) {
  if (!LST_IS_BYTES_EX(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  LibFuncCellInfo *ud = (LibFuncCellInfo *)o->fin->udata;
  if (ud->type != ffiFuncId) return NULL;
  return ud->addr;
}


/*
#define LstF0 int(*)(void)
#define LstF1 int(*)(int)
#define LstF2 int(*)(int,int)
#define LstF3 int(*)(int,int,int)
#define LstF4 int(*)(int,int,int,int)
#define LstF5 int(*)(int,int,int,int,int)
#define LstF6 int(*)(int,int,int,int,int,int)
#define LstF7 int(*)(int,int,int,int,int,int,int)
*/


/******************************************************************************/
LST_PRIMFN(lpFFILoadLib) {
  if (LST_PRIMARGC != 1) return NULL;
  const char *libName = lstGetStringPtr(LST_PRIMARG(0));
  LibInfo *h = ffiLoadLib(libName, RSS_DL_LAZY);
  if (!h) return lstNilObj;
  return newLibHandle(h);
}


LST_PRIMFN(lpFFICloseLib) {
  if (LST_PRIMARGC != 1) return NULL;
  LibInfo *h = getLib(LST_PRIMARG(0));
  if (!h) return lstFalseObj;
  ffiCloseLib(h);
  LibCellInfo *ud = (LibCellInfo *)((LST_PRIMARG(0))->fin->udata);
  ud->h = NULL;
  return lstTrueObj;
}


LST_PRIMFN(lpFFIErrorMessage) {
  if (LST_PRIMARGC != 0) return NULL;
  const char *str = ffiLibError();
  return lstNewString(str ? str : "");
}


/* tries to resolve a function symbol */
/* LST_PRIMARG(0): the handle of the shared object */
/* LST_PRIMARG(1): the name of the function */
/* returns a function handle that can be used to call the function or lstNilObj */
LST_PRIMFN(lpFFIResolveName) {
  if (LST_PRIMARGC != 2) return NULL;
  LibInfo *h = getLib(LST_PRIMARG(0));
  if (!h) return NULL;
  if (!LST_IS_BYTES_EX(LST_PRIMARG(1))) return NULL;
  void *addr = ffiLibSym(h, lstGetStringPtr(LST_PRIMARG(1)));
  if (!addr) return NULL;
  /*fprintf(stderr, "%s resolved to %p\n", lstGetStringPtr(LST_PRIMARG(1)), addr);*/
  return newLibFunc(addr);
}


static intptr_t lstMakeArg (lstObject *obj) {
  if (obj == lstNilObj || obj == lstFalseObj) return 0;
  if (obj == lstTrueObj) return 1;
  if (LST_IS_SMALLINT(obj)) return lstIntValue(obj);
  if (obj->stclass == lstStringClass || obj->stclass == lstByteArrayClass) return (intptr_t)lstGetStringPtr(obj);
  if (obj->stclass == lstIntegerClass) return lstLIntValue(obj);
  if (obj->stclass == lstCharClass) return lstIntValue(obj->data[0]);
  return 0;
}


#include "lstpl_ffi_fdef.c"

/* execute the specified function */
/* LST_PRIMARG(0): the handle of the function */
/* LST_PRIMARG(1): the type of returning value (SmallInt/Integer class; String class; ByteArray class; nil, BooleanClass */
/* LST_PRIMARG(2)...: arguments */
LST_PRIMFN(lpFFICall) {
  if (LST_PRIMARGC < 2) return NULL;
  void *func = getFunc(LST_PRIMARG(0));
  if (!func) return NULL;
  lstObject *resType = LST_PRIMARG(1);
  if (LST_IS_SMALLINT(resType) || LST_IS_BYTES(resType)) return NULL;
  if (resType != lstNilObj && resType != lstBooleanClass &&
      resType != lstCharClass &&
      resType != lstStringClass &&
      /*resType != lstByteArrayClass &&*/
      resType != lstSmallIntClass &&
      resType != lstIntegerClass &&
      resType != lstNumberClass) return NULL;
  int i = LST_PRIMARGC-2; /* # of args */
  void *fres = NULL;
  /*fprintf(stderr, "calling %p\n", func);*/
  switch (i) {
#include "lstpl_ffi_switch.c"
    default: return NULL;
  }
  if (resType == lstNilObj) return lstNilObj;
  if (resType == lstBooleanClass) return fres ? lstTrueObj : lstFalseObj;
  if (resType == lstCharClass) {
    intptr_t ch = (intptr_t)fres;
    if (ch < 0 || ch > 255) ch = 0;
    return lstNewChar(ch);
  }
  if (resType == lstSmallIntClass || resType == lstIntegerClass || resType == lstNumberClass) {
    intptr_t i = (intptr_t)fres;
    int64_t r = i;
    if (resType == lstIntegerClass) return lstNewLongInt(r);
    if (resType == lstNumberClass) return lstNewInteger(r);
    return lstNewInt(r); /*FIXME!*/
  }
  if (resType == lstStringClass) {
    if (!fres) return lstNewString("");
    return lstNewString((const char *)fres);
  }
  return lstNilObj;
}


/* get value */
/* LST_PRIMARG(0): the handle of the value */
/* LST_PRIMARG(1): the type of returning value (SmallInt/Integer class; String class; ByteArray class; nil, BooleanClass */
LST_PRIMFN(lpFFIGetVal) {
  if (LST_PRIMARGC != 2) return NULL;
  char *addr = getFunc(LST_PRIMARG(0));
  if (!addr) return NULL;
  lstObject *resType = LST_PRIMARG(1);
  if (LST_IS_SMALLINT(resType) || LST_IS_BYTES(resType)) return NULL;
  if (resType == lstNilObj) return lstNilObj;
  if (resType == lstBooleanClass) return *addr ? lstTrueObj : lstFalseObj;
  if (resType == lstCharClass) {
    char ch = (char)(*addr);
    if (ch < 0 || ch > 255) ch = 0;
    return lstNewChar(ch);
  }
  if (resType == lstSmallIntClass || resType == lstIntegerClass || resType == lstNumberClass) {
    int i = (int)(*addr);
    int64_t r = i;
    if (resType == lstIntegerClass) return lstNewLongInt(r);
    if (resType == lstNumberClass) return lstNewInteger(r);
    return lstNewInt(r); /*FIXME!*/
  }
  if (resType == lstStringClass) {
    return lstNewString((const char *)addr);
  }
  return lstNilObj;
}


static const LSTExtPrimitiveTable primTbl[] = {
{"FFILoadLib", lpFFILoadLib, NULL},
{"FFICloseLib", lpFFICloseLib, NULL},
{"FFIErrorMessage", lpFFIErrorMessage, NULL},
{"FFIResolveName", lpFFIResolveName, NULL},
{"FFICall", lpFFICall, NULL},
{"FFIGetVal", lpFFIGetVal, NULL},
{0}};


void lstInitPrimitivesFFI (void) {
  lstRegisterExtPrimitiveTable(primTbl);
}
