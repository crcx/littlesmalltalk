/*
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
typedef struct EPInfo {
  const char *name;
  lstObject **eptr;
} EPInfo;


static const EPInfo clInfo[] = {
  {"Boolean", &lstBooleanClass},
  {"SmallInt", &lstSmallIntClass},
  {"Integer", &lstIntegerClass},
  {"Char", &lstCharClass},
  {"Float", &lstFloatClass},
  {"Array", &lstArrayClass},
  {"Block", &lstBlockClass},
  {"Context", &lstContextClass},
  {"Process", &lstProcessClass},
  {"String", &lstStringClass},
  {"Symbol", &lstSymbolClass},
  {"ByteArray", &lstByteArrayClass},
  {"ByteCode", &lstByteCodeClass},
  {"Method", &lstMethodClass},
  {"Number", &lstNumberClass},
  {0}
};

static const EPInfo epInfo[] = {
  {"initialize", &lstInitMethod},
  {"loadFile:", &lstLoadMethod},
  {"doString:", &lstDoStrMethod},
  {"runREPL", &lstReplMethod},
  {"newSymbol:", &lstNewSymMethod},
  {"setGlobal:", &lstSetGlobMethod},
  {0}
};


typedef struct {
  int idx;
  const char *name;
} LSTPrimDef;

static const LSTPrimDef lstPrimList[] = {

{  1, "NewObject"},
{  2, "NewByteArray"},
{  3, "ObjectIdentity"},  /* are two args represents the same object? (address compare) */
{  4, "ObjectClass"},
{  5, "ObjectSize"},
{  6, "Array#at:"},
{  7, "Array#at:put:"},
{  8, "String#at:"},
{  9, "String#at:put:"},
{ 10, "String#clone:"},
{ 11, "String#Position:from:"},
{ 12, "String#LastPosition:from:"},
{ 13, "String#CopyFromTo"},

{ 14, "BulkObjectExchange"}, /*???*/
{ 15, "replaceFrom:to:with:startingAt:"},

{ 16, "BlockInvocation"},

{ 17, "FlushMethodCache"},

{ 18, "SmallIntToInteger"},
{ 19, "NumberToFloat"},
{ 20, "FloatToInteger"},
{ 21, "IntegerToSmallInt"},
{ 22, "IntegerToSmallIntTrunc"},

{ 23, "bit2op:"},
{ 24, "bitNot"},
{ 25, "bitShift:"},

{ 26, "SmallIntAdd"},
{ 27, "SmallIntSub"},
{ 28, "SmallIntMul"},
{ 29, "SmallIntDiv"},
{ 30, "SmallIntMod"},
{ 31, "SmallIntLess"},
{ 32, "SmallIntLessEqu"},
{ 33, "SmallIntGreat"},
{ 34, "SmallIntGreatEqu"},
{ 35, "SmallIntEqu"},
{ 36, "SmallIntNotEqu"},

{ 37, "IntegerAdd"},
{ 38, "IntegerSub"},
{ 39, "IntegerMul"},
{ 40, "IntegerDiv"},
{ 41, "IntegerMod"},
{ 42, "IntegerLess"},
{ 43, "IntegerLessEqu"},
{ 44, "IntegerGreat"},
{ 45, "IntegerGreatEqu"},
{ 46, "IntegerEqu"},
{ 47, "IntegerNotEqu"},

{ 48, "FloatAdd"},
{ 49, "FloatSub"},
{ 50, "FloatMul"},
{ 51, "FloatDiv"},
{ 52, "FloatLess"},
{ 53, "FloatLessEqu"},
{ 54, "FloatGreat"},
{ 55, "FloatGreatEqu"},
{ 56, "FloatEqu"},
{ 57, "FloatNotEqu"},

{ 58, "FloatToString"},
{ 59, "FloatNegate"},

{ 60, "PrimIdxName"},

{ 61, "GetCurrentProcess"},
{ 62, "ErrorOrYield"},
{ 63, "ExecuteNewProcessAndWait"},

{ 64, "LockUnlockSheduler"},
{ 65, "TicksGetSet"},
{ 66, "RunGC" },
{ 67, "UserBreakSignal" },
{ 68, "EventHandlerCtl" },
{ 69, "ProcessGroupCtl" },

{ 70, "PrintObject"},
{ 71, "ReadCharacter"}, /* returns nil or int */

{ 72, "FloatBAIO"},
{ 73, "IntegerBAIO"},

{ 74, "ExecuteContext"},

{ 75, "StFinalizeCtl"},
{ 76, "StWeakCtl"},

{ 77, "FloatFunc"},

{ 78, "LastFailedPrim"},

{ 79, "FNVHash"},
{0}
};

#define LST_FIRST_UNUSED_PRIM  80


static const LSTPrimDef *lstPDefByIdx (int idx) {
  const LSTPrimDef *pd;
  for (pd = lstPrimList; pd->name; pd++) if (pd->idx == idx) return pd;
  return NULL;
}


static const LSTPrimDef *lstPDefByName (const char *name) {
  const LSTPrimDef *pd;
  for (pd = lstPrimList; pd->name; pd++) if (!strcmp(pd->name, name)) return pd;
  return NULL;
}


static int lstPDGetInstr (const void *bcode, int pc, int *op, int *arg) {
  const uint8_t *bc = (const uint8_t *)bcode;
  uint8_t h, l;
  l = (h = bc[pc++])&0x0f;
  h = (h>>4)&0x0f;
  if (h == 0) {
    h = l;
    l = bc[pc++];
  }
  *op = h;
  *arg = l;
  return pc;
}


#define VALSIZE  2
static int lstPDGetInstrSize (const void *bcode, int pc) {
  int res = pc;
  const uint8_t *bc = (const uint8_t *)bcode;
  uint8_t high, low;
  low = (high = bc[pc++])&0x0f;
  high = (high>>4)&0x0f;
  if (high == 0) { high = low; low = bc[pc++]; }
  res = pc-res;
  switch (high) {
    case lstBCPushBlock:
      res += VALSIZE;
      ++res;
      break;
    case lstBCDoPrimitive:
      /* low is argument count; next byte is primitive number */
      ++res;
      break;
    case lstBCDoSpecial:
      switch (low) {
        case lstBXBranch:
        case lstBXBranchIfTrue:
        case lstBXBranchIfFalse:
        case lstBXBranchIfNil:
        case lstBXBranchIfNotNil:
          res += VALSIZE;
          break;
        case lstBXSendToSuper:
          /* next byte has literal selector number */
          ++res;
          break;
      }
      break;
  }
  return res;
}


#define GETVAL(pc)      (bc[pc] | (bc[(pc)+1] << 8))
static __attribute((unused)) int lstPDGetHighestJump (const void *bcode, int bcsize) {
  const uint8_t *bc = (const uint8_t *)bcode;
  int zjmp = -1, pc = 0, jdst;
  while (pc < bcsize) {
    uint8_t high, low;
    low = (high = bc[pc++])&0x0f;
    high = (high>>4)&0x0f;
    if (high == 0) { high = low; low = bc[pc++]; }
    jdst = -1;
    switch (high) {
      case lstBCPushBlock:
        jdst = GETVAL(pc);
        pc += VALSIZE;
        ++pc;
        break;
      case lstBCDoPrimitive:
        /* low is argument count; next byte is primitive number */
        ++pc;
        break;
      case lstBCDoSpecial:
        switch (low) {
          case lstBXBranch:
          case lstBXBranchIfTrue:
          case lstBXBranchIfFalse:
          case lstBXBranchIfNil:
          case lstBXBranchIfNotNil:
            jdst = GETVAL(pc);
            pc += VALSIZE;
            break;
          case lstBXSendToSuper:
            /* next byte has literal selector number */
            ++pc;
            break;
        }
        break;
    }
    if (jdst > zjmp) zjmp = jdst;
  }
  return zjmp;
}


static void lstWritePrimPatches (FILE *fp, lstByteObject *bobj) {
  static struct {
    int pccount;
    int pc[256];
  } primPatch[256];
  int size = LST_SIZE(bobj);
  memset(primPatch, 0, sizeof(primPatch));
  int pc = 0, primRC = 0;
  while (pc < size) {
    int op, arg;
    int apc = lstPDGetInstr(bobj->bytes, pc, &op, &arg);
    if (op == lstBCDoPrimitive) {
      int idx = bobj->bytes[apc];
      if (lstFindPrimitiveName(idx)) {
        if (primPatch[idx].pccount == 0) primRC++;
        if (primPatch[idx].pccount >= 255) lstFatal("too many primitive calls in bytecode", (intptr_t)bobj);
        primPatch[idx].pc[primPatch[idx].pccount++] = apc;
      } else {
        lstFatal("unknown primitive in image writer", idx);
      }
    }
    int len = lstPDGetInstrSize(bobj->bytes, pc);
    if (len < 1) lstFatal("internal error in bytecode tracer", (intptr_t)bobj);
    pc += len;
  }
  lstImgWriteWord(fp, primRC);
  if (primRC) {
    int f, c;
    for (f = 0; f <= 255; f++) {
      if (primPatch[f].pccount) {
        const char *pname = lstFindPrimitiveName(f);
        if (pname) {
          uint8_t len = strlen(pname);
          if (fwrite(&len, 1, 1, fp) != 1) lstFatal("can't write str8", (intptr_t)bobj);
          if (len > 0) {
            if (fwrite(pname, len, 1, fp) != 1) lstFatal("can't write str8", (intptr_t)bobj);
          }
          lstImgWriteWord(fp, primPatch[f].pccount);
          for (c = 0; c < primPatch[f].pccount; c++) lstImgWriteWord(fp, primPatch[f].pc[c]);
        }
      }
    }
  }
}


typedef struct {
  char *name;
  LSTPrimitiveFn pfn;
  LSTPrimitiveClearFn cfn;
} LSTExtPrimDef;


static LSTExtPrimDef lstExtPrimList[256];
static int lstExtPrimFree = LST_FIRST_UNUSED_PRIM;


int lstFindExtPrimitiveByName (const char *name) {
  int f;
  for (f = LST_FIRST_UNUSED_PRIM; f < lstExtPrimFree; ++f) {
    if (lstExtPrimList[f].name && !strcmp(lstExtPrimList[f].name, name)) return f;
  }
  return -1;
}


int lstFindPrimitiveIdx (const char *name) {
  const LSTPrimDef *pd = lstPDefByName(name);
  if (pd) return pd->idx;
  return lstFindExtPrimitiveByName(name);
}


LSTPrimitiveFn lstFindExtPrimitiveFn (int idx) {
  if (idx < LST_FIRST_UNUSED_PRIM || idx >= lstExtPrimFree) return NULL;
  if (!lstExtPrimList[idx].name) return NULL;
  return lstExtPrimList[idx].pfn;
}


const char *lstFindPrimitiveName (int idx) {
  if (idx < 0 || idx >= lstExtPrimFree) return NULL;
  if (idx < LST_FIRST_UNUSED_PRIM) {
    const LSTPrimDef *pd = lstPDefByIdx(idx);
    return pd ? pd->name : NULL;
  }
  return lstExtPrimList[idx].name;
}


int lstRegisterExtPrimitive (const char *name, LSTPrimitiveFn pfn, LSTPrimitiveClearFn cfn) {
  /*fprintf(stderr, "registering: [%s]\n", name);*/
  if (!name || !name[0] || strlen(name) > 255) return -1; /* invalid name */
  int idx = lstFindExtPrimitiveByName(name);
  if (idx < 0) {
    if (lstExtPrimFree > 255) return -1; /* out of space for primitives */
    idx = lstExtPrimFree++;
    lstExtPrimList[idx].name = strdup(name);
  }
  lstExtPrimList[idx].pfn = pfn;
  lstExtPrimList[idx].cfn = cfn;
  return idx;
}


void lstClearExtPrimitives (void) {
  int f;
  for (f = LST_FIRST_UNUSED_PRIM; f < lstExtPrimFree; ++f) {
    if (lstExtPrimList[f].name) {
      if (lstExtPrimList[f].cfn) lstExtPrimList[f].cfn(f);
      free(lstExtPrimList[f].name);
      lstExtPrimList[f].name = NULL;
    }
  }
  lstExtPrimFree = LST_FIRST_UNUSED_PRIM;
}


void lstPrimitivesClear (void) {
  lstClearExtPrimitives();
}


int lstRegisterExtPrimitiveTable (const LSTExtPrimitiveTable *tbl) {
  while (tbl->name) {
    int px = lstRegisterExtPrimitive(tbl->name, tbl->pfn, tbl->cfn);
    if (px < 0) return 0;
    tbl++;
  }
  return 1;
}
