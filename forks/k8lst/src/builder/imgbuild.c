/*
 * This tool is used to create an initial system image from Smalltalk sources
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
/* image building utility */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lstcore/k8lst.h"


#ifdef DEBUG
# define dprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
# define dprintf(...)
#endif


#define lstFatal(str, num)  { fprintf(stderr, "ERROR: %s (%d)\n", str, (int)num); exit(1); }
#include "../lstcore/lst_imgio.c"
#define writeWord  lstImgWriteWord

#include "lstcore/lst_dpname.c"


static int optDumpMethodNames = 0;
static int optDumpClassNames = 0;
static int optStoreSource = 1;
static int optStoreDebugInfo = 1;


#define defaultImageName "lst.image"
static FILE *fin;
static char inputBuffer[32768], *ppos, tokenBuffer[8192];

static lstObject *lookupGlobal (const char *name, int ok_missing);
static int parseStatement (void);
static int parseExpression (void);
static int parseTerm (void);
static lstObject *newOrderedArray (void);
static lstObject *newArray (int size);


static void sysError2Str (const char *a, const char *b) {
  fprintf(stderr, "in syserror %s %s\n", a, b);
  exit(1);
}


/* the following are roots for the file out */
lstObject *lstNilObj;
lstObject *lstTrueObj;
lstObject *lstFalseObj;
lstObject *lstGlobalObj;
lstObject *lstSmallIntClass;
lstObject *lstArrayClass;
lstObject *lstBlockClass;
lstObject *lstIntegerClass;
lstObject *lstStringClass;
lstObject *lstByteArrayClass;
lstObject *lstByteCodeClass;
lstObject *lstMethodClass;
lstObject *lstSymbolClass;
lstObject *lstCharClass;
lstObject *PackageClass = NULL;
lstObject *lstNumberClass = NULL;
/* */
lstObject *lstInitMethod = NULL;
lstObject *lstLoadMethod = NULL;
lstObject *lstDoStrMethod = NULL;
lstObject *lstReplMethod = NULL;
lstObject *lstNewSymMethod = NULL;
lstObject *lstSetGlobMethod = NULL;

/* unused */
lstObject *lstBooleanClass = NULL;
lstObject *lstFloatClass = NULL;
lstObject *lstContextClass = NULL;
lstObject *lstProcessClass = NULL;

lstObject *currentPackage = NULL;


static char lastMethodName[256];


static void newExtPrim (const char *name) {
  static char buf[257];
  int f = 0;
  while (*name && (unsigned char)(*name) <= ' ') name++;
  int len = strlen(name);
  while (len > 0 && (unsigned char)(name[len-1]) <= ' ') len--;
  if (len < 1) {
    fprintf(stderr, "ERROR: empty external primitive name!\n");
    exit(1);
  }
  if (len > 255) {
    fprintf(stderr, "ERROR: external primitive name too long! [%s]\n", name);
    exit(1);
  }
  memcpy(buf, name, len);
  buf[len] = '\0';
  for (f = 0; buf[f]; ++f) {
    if (buf[f] == '>' || (unsigned char)(buf[f]) <= ' ') {
      fprintf(stderr, "ERROR: invalid external primitive name! [%s]\n", buf);
      exit(1);
    }
  }
  /*fprintf(stderr, "EXTPRIM: [%s]\n", buf);*/
  if ((f = lstRegisterExtPrimitive(buf, NULL, NULL)) < 0) {
    fprintf(stderr, "ERROR: too many external primitives! [%s]\n", buf);
    exit(1);
  }
  /*printf("prim #%d: %s\n", f, buf);*/
}


/* -------------------------------------------------------------
 *  allocators
 * ------------------------------------------------------------- */
lstObject *lstMemAlloc (int size) {
  lstObject *result = calloc(1, sizeof(lstObject)+size*sizeof(lstObject *)+4);
  if (result == 0) sysError2Str("out of memory", "lstMemAlloc");
  LST_SETSIZE(result, size);
  while (size > 0) result->data[--size] = lstNilObj;
  return result;
}


static lstByteObject *binaryAlloc (int size) {
#if 0
  int osize = (size+LST_BYTES_PER_WORD-1)&(-LST_BYTES_PER_WORD);
  lstByteObject *result = (lstByteObject *)lstMemAlloc(osize); /*FIXME: why -1?*/
#else
  lstByteObject *result = (lstByteObject *)lstMemAlloc(size+1);
#endif
  LST_SETSIZE(result, size);
  LST_SETBIN(result);
  /*if (size >= 0) memset(result->bytes, 0, size+1);*/
  return result;
}


/* -------------------------------------------------------------
 *  Errors
 * ------------------------------------------------------------- */
static int parseError (const char *msg) {
  const char *q;
  for (q = inputBuffer; q != ppos;) fputc(*q++, stderr);
  fprintf(stderr, "\n***ERROR: %s\n", msg);
  while (*q) fputc(*q++, stderr);
  fputc('\n', stderr);
  exit(1);
  return 0;
}


/* -------------------------------------------------------------
 *  names
 * ------------------------------------------------------------- */
#define lstMaxGlobals 10000
static int globalTop = 0;
static char *globalNames[lstMaxGlobals];
static lstObject *globals[lstMaxGlobals];


static void addGlobalName (const char *name, lstObject *value) {
  char *newName = strdup(name);
  if (!newName) sysError2Str("out of memory", "newname in add global");
  globalNames[globalTop] = newName;
  globals[globalTop] = value;
  globalTop++;
}


static lstObject *lookupGlobal (const char *name, int ok_missing) {
  int f;
  for (f = 0; f < globalTop; f++) if (strcmp(name, globalNames[f]) == 0) return globals[f];
  /* not found, return 0 */
  if (!ok_missing) sysError2Str("Missing global", name);
  return 0;
}


/* -------------------------------------------------------------
 *  Lexical Analysis
 * ------------------------------------------------------------- */

/*
 * read one character from the input stream fin.
 * ignore and skip carriage returns ('\r', ascii 13)
 */
static LstInt lstReadOneCharacter (void) {
  int c = '\r';
  while (c == '\r') c = fgetc(fin);
  return c;
}


static void pposTrimTrailingBlanks (void) {
  while (ppos > inputBuffer && (ppos[-1] == ' ' || ppos[-1] == '\t' || ppos[-1] == '\r')) --ppos;
  *ppos++ = '\n';
}

static void imtErrText (void) {
  const char *p = inputBuffer;
  while (*p && *p != '\n') fputc(*p++, stderr);
  fputc('\n', stderr);
}

static void inputMethodText (int readFirst) {
  int newStyle = 0;
  char c;
  ppos = inputBuffer;
  /* first line */
  if (readFirst) {
    while ((c = lstReadOneCharacter()) != '\n') {
      if (!newStyle && c == '[') {
        newStyle = 1;
        continue;
      }
      if (c == ']') {
        imtErrText();
        sysError2Str("invalid new-style method definition", "");
      }
      if (newStyle) {
        if (c == '\t' || c == '\r' || c == ' ') continue;
        imtErrText();
        sysError2Str("invalid new-style method definition", "");
      }
      /*putc(c, stdout);*/
      *ppos++ = c;
    }
  } else {
    while (*ppos) {
      if (!newStyle && *ppos == '[') {
        newStyle = 1;
        strcpy(ppos, ppos+1);
        while (ppos[0] == '\t' || ppos[0] == '\r' || ppos[0] == ' ') strcpy(ppos, ppos+1);
        if (ppos[0] != '\n') {
          imtErrText();
          sysError2Str("invalid new-style method definition", "");
        }
        continue;
      }
      if (ppos[0] == '\n') {
        if (ppos[1]) {
          imtErrText();
          sysError2Str("invalid method definition", "");
        }
        *ppos = '\0';
        break;
      }
      /*putc(*ppos, stdout);*/
      ppos++;
    }
  }
  /*putc('\n', stdout);*/
  pposTrimTrailingBlanks();
  /**ppos++ = '\n';*/
  if (newStyle) {
    char inQC = 0, brCnt = 1;
    for (;;) {
      c = lstReadOneCharacter();
      if (c == '\n') {
        if (!inQC || inQC == '"') pposTrimTrailingBlanks(); else *ppos++ = c;
        continue;
      }
      *ppos++ = c;
      if (inQC) {
        if (inQC == '\'' && c == '\\') {
          c = lstReadOneCharacter();
          *ppos++ = c;
          continue;
        }
        if (c == inQC) inQC = 0;
        continue;
      }
      switch (c) {
        case '"': case '\'':
          inQC = c;
          break;
        case '$':
          c = lstReadOneCharacter();
          *ppos++ = c;
          c = 0;
          break;
        case '[':
          ++brCnt;
          break;
        case ']':
          break;
      }
      if (c == ']') {
        if (--brCnt == 0) {
          while ((c = lstReadOneCharacter()) != '\n') {
            if (c != '\r' && c != '\t' && c != ' ') {
              imtErrText();
              sysError2Str("invalid completion of new-style method", "");
            }
          }
          --ppos;
          break;
        }
      }
    }
  } else {
    for (;;) {
      if ((c = lstReadOneCharacter()) == '!') {
        if ((c = lstReadOneCharacter()) == '\n') { *ppos = '\0'; break; }
        *ppos++ = '!';
      }
      while (c != '\n') {
        *ppos++ = c;
        c = lstReadOneCharacter();
      }
      pposTrimTrailingBlanks();
    }
  }
  while (ppos > inputBuffer && (ppos[-1] == ' ' || ppos[-1] == '\t' || ppos[-1] == '\r' || ppos[-1] == '\n')) --ppos;
  *ppos++ = '\n';
  *ppos = '\0';
}


static void skipSpaces (void) {
  for (;;) {
    while ((*ppos == ' ') || (*ppos == '\t') || (*ppos == '\n') || (*ppos == '\r')) ppos++;
    if (*ppos == '"') {
      ppos++;
      while (*ppos && (*ppos != '"')) ppos++;
      if (*ppos != '"') parseError("unterminated comment");
      ppos++;
      continue;
    }
    break;
  }
}


static int isDigit (char p) {
  return (p >= '0' && p <= '9')?1:0;
}


static int isIdentifierChar (char p) {
  if (p >= 'a' && p <= 'z') return 1;
  if (p >= 'A' && p <= 'Z') return 1;
  return 0;
}


static int isBinary (char p) {
  switch (p) {
    case '+': case '*': case '-': case '/': case '<': case '=': case '>': case '@': case '~': case ',':
    case '%': case '\\': case '|': case '&':
      return 1;
  }
  return 0;
}


static void readBinary (void) {
  int pos = 0;
  do { tokenBuffer[pos++] = *ppos++; } while (isBinary(*ppos));
  tokenBuffer[pos] = '\0';
  skipSpaces();
}


static int readIdentifier (void) {
  int keyflag;
  char *q = tokenBuffer;
  while (isIdentifierChar(*ppos) || isDigit(*ppos)) *q++ = *ppos++;
  *q = '\0';
  if (*ppos == ':') {
    /* it's a keyword identifier */
    keyflag = 1;
    *q++ = ':';
    *q = '\0';
    ppos++;
  } else {
    keyflag = 0;
  }
  skipSpaces();
  return keyflag;
}


static int readInteger (void) {
  int val, neg = 0;
  if (*ppos == '-') {
    neg = 1;
    ++ppos;
  }
  val = *ppos++ - '0';
  while (isDigit(*ppos)) val = 10*val+(*ppos++ - '0');
  skipSpaces();
  return neg ? -val : val;
}


/* -------------------------------------------------------------
 *  new instances of standard things
 * ------------------------------------------------------------- */
#define lstMaxSymbols 5000
static LstUInt symbolTop = 0;
static lstObject *oldSymbols[lstMaxSymbols];


static int symbolBareCmp (const char *left, int leftsize, const char *right, int rightsize) {
  int minsize = leftsize;
  int f;
  if (rightsize < minsize) minsize = rightsize;
#if 0
  for (f = 0; f < minsize; f++) {
    if (left[f] != right[f]) {
      if (left[f] < right[f]) return -1; else return 1;
    }
  }
#else
  if (minsize > 0) {
    if ((f = memcmp(left, right, minsize))) return f;
  }
#endif
  return leftsize-rightsize;
}


static int symbolCmp (lstObject *left, lstObject *right) {
  return symbolBareCmp((char *)lstBytePtr(left), LST_SIZE(left), (char *)lstBytePtr(right), LST_SIZE(right));
}


static lstObject *newSymbol (const char *text) {
  LstUInt f;
  lstByteObject *result;
  /* first see if it is already a symbol */
  for (f = 0; f < symbolTop; f++) {
    if (symbolBareCmp((char *)text, strlen(text), (char *)lstBytePtr(oldSymbols[f]), LST_SIZE(oldSymbols[f])) == 0) {
      return oldSymbols[f];
    }
  }
  /* not there, make a new one */
  result = binaryAlloc(strlen(text)); /* FIXME: Shouldn't we allocate one more byte? */
  result->stclass = lstSymbolClass;
  strcpy((char *)result->bytes, text);
  oldSymbols[symbolTop++] = (lstObject *)result;
  return (lstObject *)result;
}


static lstObject *newNode (lstObject *v, lstObject *l, lstObject *r) {
  lstObject *result = lstMemAlloc(3);
  result->stclass = lookupGlobal("Node", 0);
  result->data[0] = v;
  result->data[1] = l;
  result->data[2] = r;
  return result;
}


static lstObject *newTree (void) {
  lstObject *result = lstMemAlloc(1);
  result->stclass = lookupGlobal("Tree", 0);
  return result;
}


static lstObject *newDictionary (void) {
  lstObject *result = lstMemAlloc(2);
  result->stclass = lookupGlobal("Dictionary", 0);
  result->data[0] = newOrderedArray();
  result->data[1] = newArray(0);
  return result;
}


/*
 * Allocate a new array
 *
 * All slots are initialized to nil
 */
static lstObject *newArray (int size) {
  int f;
  lstObject *result = lstMemAlloc(size);
  result->stclass = lstArrayClass;/*lookupGlobal("Array", 0);*/
  for (f = 0; f < size; ++f) result->data[f] = lstNilObj;
  return result;
}


/*
 * Return a new, empty ordered array
 */
static lstObject *newOrderedArray (void) {
  lstObject *result = lstMemAlloc(0);
  result->stclass = lookupGlobal("OrderedArray", 0);
  return result;
}


typedef struct StrList StrList;
struct StrList {
  char *str;
  int len;
  lstObject *o;
  StrList *next;
};
static StrList *slist = NULL;

static lstObject *newString (const char *text) {
  StrList *c;
  int size = strlen(text);
  for (c = slist; c; c = c->next) {
    if (size != c->len) continue;
    if (size == 0 || memcmp(text, c->str, size) == 0) return c->o;
  }
  /* new string */
  lstByteObject *o = binaryAlloc(size);
  o->stclass = lstStringClass;
  if (size > 0) memcpy(o->bytes, text, size);
  c = calloc(1, sizeof(StrList));
  c->str = calloc(size+1, sizeof(char));
  if (size > 0) memcpy(c->str, text, size);
  c->len = size;
  c->o = (lstObject *)o;
  c->next = slist;
  slist = c;
  return (lstObject *)o;
}


/*
 * Insert an element in the array at the given position
 *
 * Creates a new Array-ish lstObject of the same class as "array",
 * and returns it filled in as requested.
 */
static lstObject *arrayInsert (lstObject *array, LstUInt index, lstObject *val) {
  LstUInt f, j;
  lstObject *o;
  /* clone the current lstObject, including class; make one extra slot in the Array storage */
  o = lstMemAlloc(LST_SIZE(array)+1);
  o->stclass = array->stclass;
  /* Copy up to the index */
  for (f = 0; f < index; ++f) o->data[f] = array->data[f];
  /* Put in the new element at this position */
  j = f;
  o->data[f++] = val;
  /* Now copy the rest */
  for (; j < LST_SIZE(array); ++j) o->data[f++] = array->data[j];
  return o;
}


/*
 * Insert a key/value pair into the Dictionary
 */
static void dictionaryInsert (lstObject *dict, lstObject *index, lstObject *value) {
  lstObject *keys = dict->data[0], *vals = dict->data[1];
  int f, lim, res;
  /* Scan the OrderedArray "keys" to find where we fit in */
  for (f = 0, lim = LST_SIZE(keys); f < lim; ++f) {
    res = symbolCmp(index, keys->data[f]);
    /* We should go in before this node */
    if (res < 0) {
      dict->data[0] = arrayInsert(keys, f, index);
      dict->data[1] = arrayInsert(vals, f, value);
      return;
    } else if (res > 0) {
      continue;
    } else {
      sysError2Str("dictionary insert:", "duplicate key");
    }
  }
  /* The new element goes at the end */
  dict->data[0] = arrayInsert(keys, f, index);
  dict->data[1] = arrayInsert(vals, f, value);
}


static lstObject *dictionaryFind (lstObject *dict, const char *name) {
  /* binary search */
  lstObject *keys = dict->data[0];
  int l = 0, h = LST_SIZE(keys)-1, nlen = strlen(name);
  while (l <= h) {
    int mid = (l+h)/2;
    lstObject *key = keys->data[mid];
    int res = symbolBareCmp(name, nlen, (char *)lstBytePtr(key), LST_SIZE(key));
    if (res == 0) return dict->data[1]->data[mid];
    if (res < 0) h = mid-1; else l = mid+1;
  }
  return NULL;
}


static lstObject *enterPackageEx (const char *name, int version) {
  char buf[256];
  /*if (version < 0) { sprintf(buf, "%d", version); sysError2Str("invalid package version", buf); }*/
  lstObject *pkg = dictionaryFind(PackageClass->data[lstIVpackagesInMetaPackage], name);
  if (pkg) {
    int ver = lstIntValue(pkg->data[lstIVversionInPackage]);
    if (version >= 0) {
      if (ver != version) {
        sprintf(buf, "old: %d; new: %d", ver, version);
        sysError2Str("conflicting package version", buf);
      }
    }
    /*printf("OLD PACKAGE %s version %d\n", name, ver);*/
  } else {
    /*printf("NEW PACKAGE %s version %d\n", name, version);*/
    /* new instance */
    pkg = lstMemAlloc(lstPackageSize);
    pkg->stclass = PackageClass;
    pkg->data[lstIVclassesInPackage] = newDictionary();
    pkg->data[lstIVnameInPackage] = newSymbol(name);
    pkg->data[lstIVversionInPackage] = lstNewInt(version);
    pkg->data[lstIVrequiresInPackage] = newDictionary();
    dictionaryInsert(PackageClass->data[lstIVpackagesInMetaPackage], newSymbol(name), pkg);
  }
  return pkg;
}


static lstObject *enterPackage (const char *name, int version) {
  return enterPackageEx(name, version);
  /*
  return NULL;
  return enterPackageEx(name, version);
  */
}


static void insertClassInPackage (lstObject *pkg, const char *name, lstObject *stclass) {
  if (pkg) {
    lstObject *o = dictionaryFind(pkg, name);
    if (o) {
      if (o != stclass) sysError2Str("double inclusion", name);
      printf("already in package: %s\n", name);
      return;
    }
    /*dictionaryReplace(pkg, name, class);*/
    dictionaryInsert(pkg->data[lstIVclassesInPackage], newSymbol(name), stclass);
    /*printf("%s in package %s\n", name, (char *)lstBytePtr(pkg->data[lstIVnameInPackage]));*/
  }
}


static lstObject *newClass (const char *name) {
  lstObject *newC = lstMemAlloc(lstClassSize);
  newC->stclass = NULL;
  newC->data[lstIVnameInClass] = newSymbol(name);
  newC->data[lstIVpackageInClass] = currentPackage ? currentPackage : lstNilObj;
  insertClassInPackage(currentPackage, name, newC);
  return newC;
}


/*
 * first calculate number of instance vars in all our superclasses;
 * then check our own variables;
 * then check superclass variables
 */
static int lookupInstance (lstObject *stclass, const char *text) {
  lstObject *var, *cc;
  int size = 0, f;
  /* first calc # of instance vars in superclasses */
  cc = stclass->data[lstIVparentClassInClass];
  while (cc && cc != lstNilObj) {
    var = cc->data[lstIVvariablesInClass];
    if (var && var != lstNilObj) size += LST_SIZE(var);
    cc = cc->data[lstIVparentClassInClass];
  }
  /* check our own list of variables */
  var = stclass->data[lstIVvariablesInClass];
  if (var && var != lstNilObj) {
    int len = LST_SIZE(var);
    for (f = 0; f < len; f++) {
      if (symbolBareCmp(text, strlen(text), (char *)lstBytePtr(var->data[f]), (LST_SIZE(var->data[f]))) == 0) return size+f;
    }
  }
  /* check superclasses */
  var = stclass->data[lstIVparentClassInClass];
  if (var && var != lstNilObj) return lookupInstance(var, text);
  return -1;
}


/* -------------------------------------------------------------
 *  Code Generation
 * ------------------------------------------------------------- */
#define ByteBufferTop  8192
static unsigned char byteBuffer[ByteBufferTop];
static int byteTop = 0;

static int maxStackUsage;
static int stackUsage;


/* ah, we are soooo fast! */
static int findCurrentLineNo (void) {
  int res = 1;
  const char *p = inputBuffer;
  while (*p && p < ppos) if (*p++ == '\n') ++res;
  return res;
}


static int dbgInfoUsed = 0;

#define MAX_DEBUG_INFO  8192
static struct {
  /*int line;*/
  int pcStart;
  int pcEnd;
} debugInfo[MAX_DEBUG_INFO];


static void dbgAdd (void) {
  int lno = findCurrentLineNo();
  while (dbgInfoUsed < lno) {
    ++dbgInfoUsed;
    debugInfo[dbgInfoUsed].pcStart = 9999999;
    debugInfo[dbgInfoUsed].pcEnd = -1;
  }
  if (byteTop < debugInfo[lno].pcStart) debugInfo[lno].pcStart = byteTop;
  if (byteTop > debugInfo[lno].pcEnd) debugInfo[lno].pcEnd = byteTop;
}


/*static int cgGenerateCode = 1;*/


static void stkPushArgs (int n) {
  if (n > 0) {
    stackUsage += n;
    if (stackUsage > maxStackUsage) maxStackUsage = stackUsage;
  }
}

static void stkPopArgs (int n) {
  if (n > 0) {
    stackUsage -= n;
    if (stackUsage < 0) { fprintf(stderr, "ERROR: unbalanced stack!\n"); exit(1); }
  }
}


static void genByte (int v) {
  /*if (!cgGenerateCode) return;*/
  dbgAdd();
  byteBuffer[byteTop++] = v;
  if (byteTop >= ByteBufferTop) sysError2Str("too many bytecodes", "");
}


static void genVal (int v) {
  if (v < 0 || v > 0xFFFF) sysError2Str("illegal value", "");
  dbgAdd();
  genByte(v & 0xFF);
  genByte(v >> 8);
}


static void genPatch (int pos, int v) {
  if (v < 0 || v > 0xFFFF) sysError2Str("illegal value", "");
  /*if (!cgGenerateCode) return;*/
  byteBuffer[pos] = v & 0xFF;
  byteBuffer[pos+1] = v >> 8;
}


static int lastWasStackRet = 0;
static void genInstruction (int a, int b) {
  /* printf("gen instruction %d %d\n", a, b); */
  lastWasStackRet = (a == lstBCDoSpecial && b == lstBXStackReturn) ||
                    (a == lstBCDoSpecial && b == lstBXSelfReturn);
  if (b < 16) genByte(a*16+b);
  else {
    genInstruction(0, a);
    genByte(b);
  }
}


static lstObject *buildByteArray (int asCode) {
  /*LstUInt f;*/
  lstByteObject *newObj = binaryAlloc(byteTop);
  newObj->stclass = asCode ? lstByteCodeClass : lstByteArrayClass;
  if (byteTop > 0) memcpy(newObj->bytes, byteBuffer, byteTop);
  return (lstObject *)newObj;
}


#define LiteralBufferTop 100
static lstObject *litBuffer[LiteralBufferTop];
static unsigned litTop = 0;

/*
static void xprStr (lstObject *a, int doNl) {
  int f = LST_SIZE(a);
  const char *c = (const char *)lstBytePtr(a);
  printf("(%d)", f);
  for (; f > 0; --f, ++c) fputc(*c, stdout);
  if (doNl) fputc('\n', stdout);
}
*/

static int addLiteral (lstObject *a) {
  int f;
  if (!a) sysError2Str("addLiteral: literal is NULL", "");
  for (f = 0; f < litTop; f++) {
    lstObject *o = litBuffer[f];
    if (!o) sysError2Str("addLiteral: known literal is NULL", "");
    if (o == a) return f;
    if (LST_IS_SMALLINT(o) || LST_IS_SMALLINT(a)) continue;
    if (a->stclass == lstStringClass && o->stclass == lstStringClass && LST_SIZE(a) == LST_SIZE(o)) {
      /*printf("checking string literal: "); xprStr(a, 1);*/
      if (memcmp(lstBytePtr(a), lstBytePtr(o), LST_SIZE(a)) == 0) {
        /*printf("STRING LITERAL SAVED: "); xprStr(o, 1);*/
        return f;
      }
    }
  }
  if (litTop >= LiteralBufferTop) sysError2Str("too many literals", "");
  litBuffer[litTop++] = a;
  return litTop-1;
}


static lstObject *buildLiteralArray (void) {
  LstUInt f;
  lstObject *result;
  if (litTop == 0) return lstNilObj;
  result = lstMemAlloc(litTop);
  result->stclass = lstArrayClass;/*lookupGlobal("Array", 0);*/
  for(f = 0; f < litTop; f++) result->data[f] = litBuffer[f];
  return result;
}


#define ArgumentBufferTop 30
static char *argumentNames[ArgumentBufferTop];
static int argumentTop;


static void addArgument (const char *name) {
  char *p = strdup(name);
  if(!p) sysError2Str("malloc failure", "addArguments");
  argumentNames[argumentTop++] = p;
}


#define TempBufferTop 500
static char *tempBuffer[TempBufferTop];
static int tempTop, maxTemp;


static void addTemporary (const char *name) {
  char *p = strdup(name);
  if (!p) sysError2Str("malloc failure", "addTemporary");
  tempBuffer[tempTop++] = p;
  if (tempTop > maxTemp) maxTemp = tempTop;
}


static lstObject *currentClass = NULL;

/* -------------------------------------------------------------
 *  big bang
 * ------------------------------------------------------------- */
static void bigBang (void) {
  int f;
  lstObject *ObjectClass;
  lstObject *MetaObjectClass;
  lstObject *ClassClass;
  lstObject *NilClass;
  lstObject *TrueClass;
  lstObject *FalseClass;
  lstObject *TreeClass;
  lstObject *DictionaryClass;
  lstObject *OrderedArrayClass;
  lstObject *MetaClassClass;
  lstObject *a;

  /* first, make the nil (undefined) lstObject; notice its class is wrong */
  lstNilObj = lstMemAlloc(0);

  /* second, make class for Symbol; this will allow newClass to work correctly */
  lstSymbolClass = lstMemAlloc(lstMetaSymbolSize);
  addGlobalName("Symbol", lstSymbolClass);
  lstSymbolClass->data[lstIVnameInClass] = newSymbol("Symbol");

  /* now we can fix up nil's class */
  NilClass = newClass("UndefinedObject");
  addGlobalName("UndefinedObject", NilClass);
  lstNilObj->stclass = NilClass;
  addGlobalName("nil", lstNilObj);

  /* make up the object / metaobject mess */
  ObjectClass = newClass("Object");
  addGlobalName("Object", ObjectClass);
  MetaObjectClass = newClass("MetaObject");
  addGlobalName("MetaObject", MetaObjectClass);
  ObjectClass->stclass = MetaObjectClass;
  ObjectClass->data[lstIVparentClassInClass] = lstNilObj;

  /* And the Class/MetaClass mess */
  ClassClass = newClass("Class");
  addGlobalName("Class", ClassClass);
  MetaClassClass = newClass("MetaClass");
  addGlobalName("MetaClass", MetaClassClass);
  ClassClass->stclass = MetaClassClass;

  /* now make up a bunch of other classes */
  lstBlockClass = newClass("Block");
  addGlobalName("Block", lstBlockClass);

  /* create Char with extra slot for instance var */
  lstCharClass = lstMemAlloc(lstMetaCharSize);
  addGlobalName("Char", lstCharClass);
  lstCharClass->data[lstIVnameInClass] = newSymbol("Char");

  /* SmallInt has an extra class variable, just like Symbol and Char */
  lstSmallIntClass = lstMemAlloc(lstMetaSmallIntSize);
  addGlobalName("SmallInt", lstSmallIntClass);
  lstSmallIntClass->data[lstIVnameInClass] = newSymbol("SmallInt");

  addGlobalName("Number", newClass("Number"));

  lstIntegerClass = newClass("Integer");
  addGlobalName("Integer", lstIntegerClass);

  {
    /* create Package class with extra slot */
    lstObject *o = lstMemAlloc(lstMetaPackageSize);
    addGlobalName("Package", o);
    o->data[lstIVnameInClass] = newSymbol("Package");
    PackageClass = o;
  }

  TrueClass = newClass("True");
  addGlobalName("True", TrueClass);
  lstTrueObj = lstMemAlloc(0);
  lstTrueObj->stclass = TrueClass;
  addGlobalName("true", lstTrueObj);

  FalseClass = newClass("False");
  addGlobalName("False", FalseClass);
  lstFalseObj = lstMemAlloc(0);
  lstFalseObj->stclass = FalseClass;
  addGlobalName("false", lstFalseObj);

  lstArrayClass = newClass("Array");
  addGlobalName("Array", lstArrayClass);

  lstByteArrayClass = newClass("ByteArray");
  addGlobalName("ByteArray", lstByteArrayClass);

  lstByteCodeClass = newClass("ByteCode");
  addGlobalName("ByteCode", lstByteCodeClass);

  lstMethodClass = newClass("Method");
  addGlobalName("Method", lstMethodClass);

  OrderedArrayClass = newClass("OrderedArray");
  addGlobalName("OrderedArray", OrderedArrayClass);

  lstStringClass = newClass("String");
  addGlobalName("String", lstStringClass);

  TreeClass = newClass("Tree");
  addGlobalName("Tree", TreeClass);

  DictionaryClass = newClass("Dictionary");
  addGlobalName("Dictionary", DictionaryClass);

  /* finally, we can fill in the fields in class Object */
  ObjectClass->data[lstIVmethodsInClass] = newDictionary();
  ObjectClass->data[lstIVinstanceSizeInClass] = lstNewInt(0);
  /* in class Class */
  ClassClass->data[lstIVinstanceSizeInClass] = lstNewInt(0);
  /* in class Package */
  PackageClass->data[lstIVpackagesInMetaPackage] = newDictionary();
  PackageClass->data[lstIVinstanceSizeInClass] = lstNewInt(lstPackageSize);

  /* create array of char instances; this doubles kernel code */
  lstCharClass->data[lstIVcharsInMetaChar] = (a = newArray(257));
  for (f = 0; f <= 256; ++f) {
    lstObject *c = lstMemAlloc(1);
    c->stclass = lstCharClass;
    c->data[0] = lstNewInt(f);
    a->data[f] = c;
  }

  lstProcessClass = newClass("Process");
  addGlobalName("Process", lstProcessClass);
  lstContextClass = newClass("Context");
  addGlobalName("Context", lstContextClass);

  {
    /* create System class with 4 extra slots */
    lstObject *o = lstMemAlloc(lstClassSize+4);
    addGlobalName("System", o);
    o->data[lstIVnameInClass] = newSymbol("System");
    /*o->data[lstIVpackageInClass] = enterPackage("HostOS", 1);
    insertClassInPackage(currentPackage, name, newC);*/
  }
  /*enterPackage("System");*/

  /* can make global name, but can't fill it in */
  lstGlobalObj = lstMemAlloc(2);
  addGlobalName("globals", lstGlobalObj);
}


/* -------------------------------------------------------------
 *  Parsing
 * ------------------------------------------------------------- */
static int parseInteger (void) {
  int64_t f = readInteger();
  if (f >= 0 && f <= 252) genInstruction(lstBCPushConstant, f+3);
  else if (LST_64FITS_SMALLINT(f)) genInstruction(lstBCPushLiteral, addLiteral(lstNewInt(f)));
  else {
    /* create new Integer (64-bit) */
    /*printf("number too big: %lld\n", f);*/
    LstLInt i = f;
    lstObject *iv = (lstObject *)binaryAlloc(sizeof(LstLInt));
    iv->stclass = lookupGlobal("Integer", 0);
    memcpy(lstBytePtr(iv), &i, sizeof(i));
    genInstruction(lstBCPushLiteral, addLiteral(iv));
  }
  return 1;
}


static int primsUsed[256];

static int parsePrimitive (void) {
  int primitiveNumber = -1, argumentCount;
  /* skip over the left bracket */
  ++ppos;
  if (*ppos != '#') {
    return parseError("can't parse type annotations for now");
  }
  ++ppos; /* skip '#' */
  /*skipSpaces();*/
  /* then read the primitive number */
  if (!isDigit(*ppos)) {
    /* parse primitive name */
    char *pstart = ppos, och;
    while (*ppos && *ppos != '>' && *ppos != '\t' && *ppos != '\n' && *ppos != '\r' && *ppos != ' ') ppos++;
    och = *ppos; *ppos = '\0';
    /* find primitive */
    primitiveNumber = lstFindPrimitiveIdx(pstart);
    if (primitiveNumber < 0) {
      fprintf(stderr, "ERROR: unknown primitive: [%s]\n", pstart);
      *ppos = och;
      return parseError("invalid primitive name");
    }
    /*fprintf(stderr, "primitive [%s] is %d\n", pstart, primitiveNumber);*/
    *ppos = och;
    skipSpaces();
  } else {
    primitiveNumber = readInteger();
    fprintf(stderr, "primitive #%d: should be named!\n", primitiveNumber);
  }
  ++primsUsed[primitiveNumber];
  /* then read the arguments */
  for (argumentCount = 0; *ppos && (*ppos != '>'); argumentCount++) if (!parseTerm()) return 0;
  /* make sure we ended correctly */
  if (*ppos != '>') return parseError("missing > at end of primitive");
  ppos++; /* skip '>' */
  skipSpaces();
  if (*ppos && *ppos != '.' && *ppos != ']') {
    fprintf(stderr, "WARNING: missing dot after primitive call in method '%s'\n", lastMethodName);
  }
  /* generate instructions */
  stkPushArgs(argumentCount);
  genInstruction(lstBCDoPrimitive, argumentCount);
  genByte(primitiveNumber);
  stkPopArgs(argumentCount);
  if (argumentCount < 1) {
    stkPushArgs(1);
    stkPopArgs(1);
  }
  return 1;
}


static int parseString (void) {
  char *q;
  ppos++;
  for (q = tokenBuffer; *ppos; ) {
    if (*ppos == '\\') {
      char ch;
      ++ppos;
      if (!ppos[0]) parseError("missing end of string");
      switch ((ch = *ppos++)) {
        case 'r': *q++ = '\r'; break;
        case 'n': *q++ = '\n'; break;
        case 't': *q++ = '\t'; break;
        default: *q++ = ch; break;
      }
    } else {
      if (ppos[0] == '\'') {
        if (ppos[1] != '\'') break;
        ++ppos;
      }
      *q++ = *ppos++;
    }
  }
  if (*ppos != '\'') return parseError("missing end of string");
  ppos++;
  skipSpaces();
  *q = '\0';
  genInstruction(lstBCPushLiteral, addLiteral(newString(tokenBuffer)));
  return 1;
}


#if 0
static void showFirstLineOfIB (void) {
  const char *p = inputBuffer;
  while (*p && *p != '\n') fputc(*p++, stdout);
  fputc('\n', stdout);
}
#else
static void showFirstLineOfIB (void) {
  const char *q;
  for (q = inputBuffer; q != ppos;) fputc(*q++, stderr);
  fputc('\n', stderr);
}
#endif


static void checkVarName (const char *name, int tempStart) {
  int f;
  /* see if temporary */
  for (f = tempTop-1; f >= tempStart; --f) {
    if (strcmp(name, tempBuffer[f]) == 0) {
      parseError("method variable already exists");
      return;
    }
  }
  /* see if argument */
  for (f = 0; f < argumentTop; f++) {
    if (strcmp(name, argumentNames[f]) == 0) {
      showFirstLineOfIB();
      fprintf(stderr, "WARNING: method variable '%s' shadows argument\n", name);
      return;
    }
  }
  /* see if instance variable */
  if (currentClass) {
    f = lookupInstance(currentClass, name);
    if (f >= 0) {
      showFirstLineOfIB();
      fprintf(stderr, "WARNING: method variable '%s' shadows instance variable\n", name);
      return;
    }
  }
}


static int superMessage = 0;

static const char *lowConstants[4] = { "nil", "true", "false", 0 };

static int nameTerm (const char *name) {
  int f;
  /* see if temporary */
  /* BUG: for (f = 0; f < tempTop; f++)
   * should look from the last one to pick up block temps first
   */
  for (f = tempTop-1; f >= 0; --f) {
    if (strcmp(name, tempBuffer[f]) == 0) {
      genInstruction(lstBCPushTemporary, f);
      return 1;
    }
  }
  /* see if argument */
  for (f = 0; f < argumentTop; f++) {
    if (strcmp(name, argumentNames[f]) == 0) {
      genInstruction(lstBCPushArgument, f);
      return 1;
    }
  }
  /* see if super */
  if (strcmp(name, "super") == 0) {
    genInstruction(lstBCPushArgument, 0);
    superMessage = 1;
    return 1;
  }
  /* see if thisContext */
  if (strcmp(name, "thisContext") == 0) {
    genInstruction(lstBCDoSpecial, lstBXThisContext);
    return 1;
  }
  /* see if low constant */
  for (f = 0; lowConstants[f]; f++) {
    if (strcmp(lowConstants[f], name) == 0) {
      genInstruction(lstBCPushConstant, f);
      return 1;
    }
  }
  /* see if instance variable */
  if (currentClass) {
    f = lookupInstance(currentClass, name);
    if (f >= 0) {
      genInstruction(lstBCPushInstance, f);
      return 1;
    }
  }
  /* see if global */
  {
    lstObject *glob = lookupGlobal(name, 1);
    if (glob) {
      genInstruction(lstBCPushLiteral, addLiteral(glob));
      return 1;
    }
  }
  return parseError("unknown identifier");
}


static int returnOp;
static char *blockbackup;
static int blockbackupPC;


static struct {
  const char *condName;
  const char *nextName;
  int branchCode;
  int retConst;
} condOpts[] = {
  {"ifTrue:", "ifFalse:", lstBXBranchIfFalse, lstBLFalseConst},
  {"ifFalse:", "ifTrue:", lstBXBranchIfTrue, lstBLTrueConst},
  {"ifNil:", "ifNotNil:", lstBXBranchIfNotNil, -1},
  {"ifNotNil:", "ifNil:", lstBXBranchIfNil, -1},
  {"and:", "", lstBXBranchIfFalse, lstBLFalseConst},
  {"or:", "", lstBXBranchIfTrue, lstBLTrueConst},
  {0}
};

static struct {
  const char *loopName;
  int branchCode;
} loopOpts[] = {
  {"whileTrue:", lstBXBranchIfFalse},
  {"whileFalse:", lstBXBranchIfTrue},
  {"whileNil:", lstBXBranchIfNotNil},
  {"whileNotNil:", lstBXBranchIfNil},
  {0}
};


static int parseBlock (void) {
  /*int oldGC = cgGenerateCode;*/
  int savedLocation, saveTop, argCount, blockPC;
  char *savestart = ppos;
  ppos++;
  skipSpaces();
  blockPC = byteTop;
  genInstruction(lstBCPushBlock, tempTop);
  savedLocation = byteTop;
  genVal(0);
  genByte(0); /* argCount */
  saveTop = tempTop;
  argCount = 0;
  if (*ppos == ':') {
    for (;;) {
      ppos++;
      skipSpaces();
      if (!isIdentifierChar(*ppos)) return parseError("missing identifier");
      if (readIdentifier()) return parseError("keyword illegal");
      checkVarName(tokenBuffer, saveTop);
      addTemporary(tokenBuffer);
      argCount++;
      if (*ppos == '|') break;
      if (*ppos != ':') return parseError("missing colon:");
    }
    ppos++;
    skipSpaces();
  }
  if (argCount > 255) parseError("too many arguments in block");
  /*if (cgGenerateCode)*/ byteBuffer[savedLocation+2] = argCount;
  if (*ppos == ']') {
    genInstruction(lstBCPushConstant, lstBLNilConst);
  } else {
    int saveReturnOp = returnOp;
    returnOp = lstBXBlockReturn;
    for (;;) {
      int wasDot = 0;
      if (!parseStatement()) parseError("Statement syntax inside block");
      if (*ppos == '.') { ppos++; skipSpaces(); wasDot = 1; }
      if (*ppos == ']') break;
      genInstruction(lstBCDoSpecial, lstBXPopTop);
      if (!wasDot) {
        showFirstLineOfIB();
        fprintf(stderr, "WARNING: [%s] %c\n", lastMethodName, *ppos);
      }
    }
    returnOp = saveReturnOp;
  }
  ppos++; /* skip over ] */
  skipSpaces();
  genInstruction(lstBCDoSpecial, lstBXStackReturn);
  lastWasStackRet = 0;
  genPatch(savedLocation, byteTop);
  tempTop = saveTop;
  /* set blockbackup to textual start of block */
  blockbackup = savestart;
  blockbackupPC = blockPC;
  /*cgGenerateCode = oldGC;*/
  return 1;
}


static int parseSymbol (void) {
  char *q;
  ppos++;
  if (ppos[0] == '{') {
    /* special array; used in compiler */
    int itemCount = 0, f;
    lstObject **adata = NULL, *a;
    ppos++;
    skipSpaces();
    while (ppos[0] && ppos[0] != '}') {
      q = tokenBuffer;
      while (ppos[0] && ppos[0] != '}' && ppos[0] != '\t' && ppos[0] != ' ' && ppos[0] != '\r' && ppos[0] != '\n') *q++ = *ppos++;
      *q = '\0';
      skipSpaces();
      /*printf("[%s]\n", tokenBuffer);*/
      itemCount++;
      adata = realloc(adata, sizeof(lstObject *)*itemCount);
      adata[itemCount-1] = newSymbol(tokenBuffer);
    }
    if (ppos[0] != '}') parseError("no end parentesis for array");
    ppos++;
    skipSpaces();
    a = newArray(itemCount);
    for (f = 0; f < itemCount; f++) a->data[f] = adata[f];
    free(adata);
    genInstruction(lstBCPushLiteral, addLiteral(a));
    return 1;
  }
  for (q = tokenBuffer; isIdentifierChar(*ppos) || (*ppos == ':');) *q++ = *ppos++;
  *q = '\0';
  skipSpaces();
  genInstruction(lstBCPushLiteral, addLiteral(newSymbol(tokenBuffer)));
  return 1;
}


static int parseChar (void) {
  lstObject *o;
  ppos++;
  int c = ((intptr_t)*ppos)&0xff;
  o = lstCharClass->data[lstIVcharsInMetaChar]->data[c];
  genInstruction(lstBCPushLiteral, addLiteral(o));
  ppos++;
  skipSpaces();
  return 1;
}


static int parseTerm (void) {
  /* make it so anything other than a block zeros out backup var */
  blockbackup = 0;
  blockbackupPC = 0;
  superMessage = 0;
  if (*ppos == '(') {
    ppos++;
    skipSpaces();
    if (!parseExpression()) return 0;
    if (*ppos != ')') return parseError("unbalanced parenthesis");
    ppos++;
    skipSpaces();
    return 1;
  }
  if (*ppos == '<') return parsePrimitive();
  if (*ppos == '$') return parseChar();
  if (isDigit(*ppos) || *ppos == '-') return parseInteger();
  if (*ppos == '\'') return parseString();
  if (isIdentifierChar(*ppos)) {
    readIdentifier();
    return nameTerm(tokenBuffer);
  }
  if (*ppos == '[') return parseBlock();
  if (*ppos == '#') return parseSymbol();
  return parseError("illegal start of expression");
}


static const char *unaryBuiltIns[] = { "isNil", "notNil", 0 };
static const char *binaryBuiltIns[] = { "<", "<=", "+", "-", "*", "/", "%", ">", ">=", "~=", "=", "&", "|", "==", 0 };


static int parseUnaryContinuation (void) {
  int litNumber, done;
  char *q;
  while (isIdentifierChar(*ppos)) {
    q = ppos;
    if (readIdentifier()) {
      ppos = q; /* oops, was a keyword */
      break;
    }
    done = 0;
    stkPushArgs(1);
    if (!superMessage) {
      int f;
      for (f = 0; unaryBuiltIns[f]; f++) {
        if (strcmp(tokenBuffer, unaryBuiltIns[f]) == 0) {
          genInstruction(lstBCSendUnary, f);
          done = 1;
        }
      }
    }
    if (!done) {
      genInstruction(lstBCMarkArguments, 1);
      litNumber = addLiteral(newSymbol(tokenBuffer));
      /*printf("unary %s\n", tokenBuffer);*/
      if (superMessage) {
        genInstruction(lstBCDoSpecial, lstBXSendToSuper);
        genByte(litNumber);
      } else genInstruction(lstBCSendMessage, litNumber);
      superMessage = 0;
    }
    stkPopArgs(1);
  }
  return 1;
}


static int parseBinaryContinuation (void) {
  int messLiteral, f, done;
  char messbuffer[80];
  if (!parseUnaryContinuation()) return 0;
  while (isBinary(*ppos)) {
    readBinary();
    if (!strcmp(tokenBuffer, "<-") || !strcmp(tokenBuffer, ":=")) {
      parseError("binary assign -- previous statement is not terminated");
    }
    /*printf("binary symbol %s\n", tokenBuffer);*/
    strcpy(messbuffer, tokenBuffer);
    if (!parseTerm()) return 0;
    if (!parseUnaryContinuation()) return 0;
    done = 0;
    stkPushArgs(2);
    if (!superMessage) {
      for (f = 0; binaryBuiltIns[f]; f++) {
        if (strcmp(messbuffer, binaryBuiltIns[f]) == 0) {
          genInstruction(lstBCSendBinary, f);
          done = 1;
        }
      }
    }
    if (!done) {
      messLiteral = addLiteral(newSymbol(messbuffer));
      genInstruction(lstBCMarkArguments, 2);
      if (superMessage) {
        genInstruction(lstBCDoSpecial, lstBXSendToSuper);
        genByte(messLiteral);
        superMessage = 0;
      } else {
        genInstruction(lstBCSendMessage, messLiteral);
      }
      stkPopArgs(2);
    }
  }
  return 1;
}


static int optimizeBlock (void) {
  /*int oldGC = cgGenerateCode;*/
  /* just in case; reserve on stack slot for result */
  stkPushArgs(1);
  if (*ppos != '[') {
    if (!parseTerm()) return 0;
    parseError("missing block as optimized block argument");
  } else {
    ppos++;
    skipSpaces();
    if (*ppos == ']') {
      genInstruction(lstBCPushConstant, lstBLNilConst);
      ppos++;
      skipSpaces();
      /*cgGenerateCode = oldGC;*/
      return 1;
    }
    for (;;) {
      int wasDot = 0;
      if (!parseStatement()) return 0;
      if (*ppos == '.') { ppos++; skipSpaces(); wasDot = 1; }
      if (*ppos == ']') break;
      genInstruction(lstBCDoSpecial, lstBXPopTop);
      if (!wasDot) {
        showFirstLineOfIB();
        fprintf(stderr, "WARNING: [%s] %c\n", lastMethodName, *ppos);
      }
    }
    ppos++;
    skipSpaces();
    /* just leave last expression on stack */
  }
  stkPopArgs(1);
  /*cgGenerateCode = oldGC;*/
  return 1;
}


/* ppos at first apostrophe */
static void parserSkipString (void) {
doAllAgain:
  ++ppos; /* skip starting apostrophe */
  while (*ppos != '\'') {
    if (!ppos[0]) parseError("unterminated string");
    if (*ppos++ == '\\') {
      switch (*ppos++) {
        case 'x': case 'X':
          if (isxdigit(*ppos)) ++ppos;
          if (isxdigit(*ppos)) ++ppos;
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7':
          if (*ppos >= '0' && *ppos <= '7') ++ppos;
          if (*ppos >= '0' && *ppos <= '7') ++ppos;
          break;
      }
    }
  }
  ++ppos; /* skip ending apostrophe */
  if (*ppos == '\'') goto doAllAgain;
}


static void parserSkipBlock (void) {
  int sqCnt = 0;
  skipSpaces();
  if (*ppos != '[') parseError("block expected in control flow");
  for (;;) {
    skipSpaces();
    switch (*ppos) {
      case '\0': parseError("unterminated block in control flow");
      case '[': ++ppos; ++sqCnt; break;
      case ']':
        ++ppos;
        if (--sqCnt == 0) return;
        break;
      case '\'': parserSkipString(); break;
      case '$':
        if (!ppos[1]) parseError("invalid char literal");
        ppos += 2;
        break;
      default: /* skip other shit */
        while (*ppos && *ppos != '[' && *ppos != ']' && *ppos != '\'' && *ppos != '"' && *ppos != '$') ++ppos;
        break;
    }
  }
}


static void showMethodSrc (const char *st0, const char *ed0, char l0, char r0,
  const char *st1, const char *ed1, char l1, char r1)
{
#if 0
  const char *t = inputBuffer;
  const char *mstr = isatty(STDOUT_FILENO) ? "\x1b[1m{%c}\x1b[0m" : "{%c}";
  printf("==========\n");
  for (; *t; ++t) {
    if (t == st0) printf(mstr, l0);
    if (t == ed0) printf(mstr, r0);
    if (t == st1) printf(mstr, l1);
    if (t == ed1) printf(mstr, r1);
    printf("%c", *t);
  }
  printf("----------\n");
#endif
}


static int controlFlow (int opt1, const char *rest, int resConst) {
  int save1, save2, mustBe2;
  char *q, *q1, *ppos0;
  /*if (resConst < 0) resConst = lstBLNilConst;*/
  stkPushArgs(1);
  /* check for 'control cascade' */
  q = ppos;
  parserSkipBlock();
  /*if (resConst < 0) showMethodSrc(q, ppos, '<', '>');*/
  ppos0 = ppos;
  skipSpaces();
  q1 = ppos;
  if (isIdentifierChar(*ppos) && readIdentifier() && (strcmp(tokenBuffer, rest) == 0)) {
    /* skip another block */
    mustBe2 = 1;
    q1 = ppos;
    parserSkipBlock();
    if (resConst < 0) {
      showMethodSrc(q, ppos0, '<', '>', q1, ppos, '[', ']');
    }
    skipSpaces();
    if (resConst < 0) resConst = lstBLNilConst;
  } else {
    mustBe2 = 0;
    ppos = q1;
    /*if (resConst < 0) resConst = lstBLNilConst;*/
    if (resConst < 0) {
      /* nil-filter, duplicate argument */
      showMethodSrc(q, ppos, '(', ')', NULL, NULL, 0, 0);
    }
  }
  if (*ppos && *ppos != ')' && *ppos != ';' && *ppos != '.' && *ppos != ']') {
    if (*ppos == '^') {
      showFirstLineOfIB();
      fprintf(stderr, "WARNING: no dot after conditional\n");
    } else {
      parseError("no dot after conditional");
    }
  }
  ppos = q;
  /***/
  if (resConst < 0) {
    /* nil-filter, duplicate argument */
    showMethodSrc(q, ppos, '(', ')', NULL, NULL, 0, 0);
    genInstruction(lstBCDoSpecial, lstBXDuplicate);
    stkPushArgs(1);
  }
  /* jump over 'true' block */
  genInstruction(lstBCDoSpecial, opt1);
  save1 = byteTop; genVal(0);
  if (resConst < 0) {
    /* drop saved value for nil-filter */
    genInstruction(lstBCDoSpecial, lstBXPopTop);
    stkPopArgs(1);
  }
  if (!optimizeBlock()) parseError("syntax error in control flow");
  if (resConst >= 0) {
    /* not a nil-filter, generate jump over 'false/constant' block */
    genInstruction(lstBCDoSpecial, lstBXBranch);
    save2 = byteTop; genVal(0);
  } else {
    /* nil-filter, no second jump */
    /*printf("NILFILTER\n");*/
    save2 = -1;
  }
  genPatch(save1, byteTop); /* patch first jump */
  skipSpaces(); /* just in case */
  q = ppos;
  if (isIdentifierChar(*ppos) && readIdentifier() && (strcmp(tokenBuffer, rest) == 0)) {
    /* we have second control flow message */
    if (!mustBe2) parseError("internal error in control flow parser (x0)");
    if (resConst < 0) parseError("internal error in control flow parser (x1)");
    if (!optimizeBlock()) parseError("syntax error in control cascade");
  } else {
    /* no second control-flow message */
    if (mustBe2) parseError("internal error in control flow parser (x2)");
    ppos = q;
    if (resConst >= 0) {
      /* not a nil-filter */
      if (save2 < 0) parseError("internal error in control flow parser (x3)");
      genInstruction(lstBCPushConstant, resConst);
    }
  }
  if (save2 > 0) genPatch(save2, byteTop);
  stkPopArgs(1);
  return 1;
}


static int optimizeLoop (int branchInstruction) {
  int L1, L2;
  /* back up to start of block and try again */
  stkPushArgs(1);
  ppos = blockbackup;
  byteTop = blockbackupPC;
  L1 = byteTop;
  optimizeBlock();
  genInstruction(lstBCDoSpecial, branchInstruction);
  L2 = byteTop;
  genVal(0);
  if (!(isIdentifierChar(*ppos) && readIdentifier())) return parseError("can't get message again in optimized block");
  /* now read the body */
  optimizeBlock();
  genInstruction(lstBCDoSpecial, lstBXPopTop);
  genInstruction(lstBCDoSpecial, lstBXBranch);
  genVal(L1);
  genPatch(L2, byteTop);
  genInstruction(lstBCPushConstant, lstBLNilConst);
  stkPopArgs(1);
  return 1;
}


static int parseKeywordContinuation (void) {
  int argCount, f, done, saveSuper;
  char messageBuffer[100];
  saveSuper = superMessage;
  if (!parseBinaryContinuation()) return 0;
  strcpy(messageBuffer, "");
  argCount = 0;
  if (isIdentifierChar(*ppos) && readIdentifier()) {
    for (f = 0; condOpts[f].condName; ++f) {
      if (!strcmp(tokenBuffer, condOpts[f].condName)) {
        return controlFlow(condOpts[f].branchCode, condOpts[f].nextName, condOpts[f].retConst);
      }
    }
    if (blockbackup) {
      for (f = 0; loopOpts[f].loopName; ++f) {
        if (!strcmp(tokenBuffer, loopOpts[f].loopName)) return optimizeLoop(loopOpts[f].branchCode);
      }
    }
    do {
      strcat(messageBuffer, tokenBuffer);
      argCount++;
      stkPushArgs(argCount+1);
      if (!parseTerm()) return 0;
      if (!parseBinaryContinuation()) return 0;
      stkPopArgs(argCount+1);
    } while (isIdentifierChar(*ppos) && readIdentifier());
  }
  if (argCount > 0) {
    /*
    if (!strcmp(lastMethodName, "aheadChar")) {
      fprintf(stderr, "'%s': sending: '%s' %d\n", lastMethodName, messageBuffer, argCount+1);
    }
    */
    stkPushArgs(argCount+1);
    done = 0;
    if (!saveSuper) {
      for (f = 0; binaryBuiltIns[f]; f++) {
        if (strcmp(messageBuffer, binaryBuiltIns[f]) == 0) {
          genInstruction(lstBCSendBinary, f);
          done = 1;
        }
      }
    }
    if (!done) {
      genInstruction(lstBCMarkArguments, argCount+1);
      if (saveSuper) {
        genInstruction(lstBCDoSpecial, lstBXSendToSuper);
        genByte(addLiteral(newSymbol(messageBuffer)));
        superMessage = 0;
      } else {
        genInstruction(lstBCSendMessage, addLiteral(newSymbol(messageBuffer)));
      }
    }
    stkPopArgs(argCount+1);
    skipSpaces();
    if (*ppos && *ppos != '.' && *ppos != ']' && *ppos != ')' && *ppos != ';') {
      showFirstLineOfIB();
      fprintf(stderr, "WARNING: [%s] %c\n", lastMethodName, *ppos);
    }
  }
  return 1;
}


static int doAssignment (const char *name) {
  int f;
  /* see if temporary */
  /* BUG: for (f = 0; f < tempTop; f++)
   * should look from the last one to pick up block temps first
   */
  for (f = tempTop-1; f >= 0; --f) {
    if (strcmp(name, tempBuffer[f]) == 0) {
      genInstruction(lstBCAssignTemporary, f);
      return 1;
    }
  }
  /* see if argument; 0 is self, so skip it */
  for (f = 1; f < argumentTop; f++) {
    if (strcmp(name, argumentNames[f]) == 0) {
      genInstruction(lstBCAssignArgument, f);
      return 1;
    }
  }
  if (currentClass) {
    f = lookupInstance(currentClass, name);
    if (f >= 0) {
      genInstruction(lstBCAssignInstance, f);
      return 1;
    }
  }
  return parseError("unknown target of assignment");
}


static int parseExpression (void) {
  char nameBuffer[60];
  if (isIdentifierChar(*ppos)) {
    readIdentifier();
    if ((*ppos == '<' && ppos[1] == '-') || (*ppos == ':' && ppos[1] == '=')) {
      ppos += 2;
      skipSpaces();
      strcpy(nameBuffer, tokenBuffer);
      if (!parseExpression()) return 0;
      return doAssignment(nameBuffer);
    }
    if (!nameTerm(tokenBuffer)) return 0;
  } else {
    if (!parseTerm()) return 0;
  }
  if (!parseKeywordContinuation()) return 0;
  while (*ppos == ';') {
    /* just in case; reserve on stack slot for result */
    stkPushArgs(1);
    ppos++;
    skipSpaces();
    genInstruction(lstBCDoSpecial, lstBXDuplicate);
    if (!parseKeywordContinuation()) return 0;
    genInstruction(lstBCDoSpecial, lstBXPopTop);
    stkPopArgs(1);
    skipSpaces();
  }
  skipSpaces();
  if (*ppos && *ppos != '.' && *ppos != ']' && *ppos != ')' && *ppos != ';') {
    showFirstLineOfIB();
    fprintf(stderr, "WARNING: [%s] %c\n", lastMethodName, *ppos);
  }
  return 1;
}


static int parseStatement (void) {
  if (*ppos == '^') {
    /* return statement */
    ppos++;
    skipSpaces();
    if (!parseExpression()) return 0;
    genInstruction(lstBCDoSpecial, returnOp);
    /*cgGenerateCode = 0;*/
    return 1;
  }
  /* otherwise just an expression */
  if (!parseExpression()) return 0;
  return 1;
}


static int parseBody (void) {
  /*cgGenerateCode = 1;*/
  returnOp = lstBXStackReturn;
  lastWasStackRet = 0;
  int doPopTop = 0;
  while (*ppos) {
    if (doPopTop) genInstruction(lstBCDoSpecial, lstBXPopTop); else doPopTop = 1;
    if (!parseStatement()) return 0;
    /*int sss = lastWasStackRet;
    genInstruction(lstBCDoSpecial, lstBXPopTop);
    lastWasStackRet = sss;*/
    if (*ppos == '.') {
      ppos++;
      skipSpaces();
    }
  }
  if (!lastWasStackRet) {
    if (doPopTop) genInstruction(lstBCDoSpecial, lstBXPopTop);
    genInstruction(lstBCDoSpecial, lstBXSelfReturn);
  }
  int lj = lstPDGetHighestJump(byteBuffer, byteTop);
  /*printf("lj=%d; byteTop=%d\n", lj, byteTop);*/
  if (lj >= byteTop) {
    if (lj > byteTop) parseError("internal error in codegen");
    /*printf("NOTE: added dummy return to %s\n", lastMethodName);*/
    genInstruction(lstBCDoSpecial, lstBXSelfReturn);
  }
  return 1;
}


static int parseMethodHeader (lstObject *theMethod) {
  char messageBuffer[100], *q;
  static lstObject *argNames[256];
  int aCnt = 0;
  int keyflag, isMeta = (ppos[0] == '^');
  if (isMeta) {
    if (!currentClass) parseError("metamethod for class-less?");
    const char *clName = (const char *)lstBytePtr(currentClass->data[lstIVnameInClass]);
    if (strncmp(clName, "Meta", 4) == 0) return parseError("MetaMeta?");
    /*if (ppos == inputBuffer) strcpy(inputBuffer, inputBuffer+1); else ppos++;*/
    /*ppos++;*/
    strcpy(ppos, ppos+1);
    sprintf(messageBuffer, "Meta%s", clName);
    currentClass = lookupGlobal(messageBuffer, 0);
  }
  if (isIdentifierChar(*ppos)) {
    if (readIdentifier()) {
      /* keyword message */
      strcpy(messageBuffer, "");
      keyflag = 1;
      while (keyflag) {
        strcat(messageBuffer, tokenBuffer);
        if (isIdentifierChar(*ppos) && !readIdentifier()) {
          addArgument(tokenBuffer);
          argNames[aCnt++] = newSymbol(tokenBuffer);
        } else {
          return parseError("missing argument after keyword");
        }
        q = ppos;
        if (isIdentifierChar(*ppos) && readIdentifier()) {
          keyflag = 1;
        } else {
          ppos = q;
          keyflag = 0;
        }
      }
    } else {
      /* unary message */
      strcpy(messageBuffer, tokenBuffer);
    }
  } else if (isBinary(*ppos)) {
    /* binary message */
    readBinary();
    strcpy(messageBuffer, tokenBuffer);
    if (!isIdentifierChar(*ppos)) return parseError("missing argument");
    readIdentifier();
    addArgument(tokenBuffer);
  } else {
    return parseError("ill formed method header");
  }
  theMethod->data[lstIVnameInMethod] = newSymbol(messageBuffer);
  if (aCnt > 0) {
    lstObject *aa = newArray(aCnt);
    while (--aCnt >= 0) aa->data[aCnt] = argNames[aCnt];
    theMethod->data[lstIVargNamesInMethod] = aa;
  } else {
    theMethod->data[lstIVargNamesInMethod] = lstNilObj;
  }
  strcpy(lastMethodName, messageBuffer);
  if (optDumpMethodNames) {
    if (currentClass) {
      printf("Method %s>>%s\n", lstBytePtr(currentClass->data[lstIVnameInClass]), messageBuffer);
    } else {
      printf("EntryPoint %s\n", messageBuffer);
    }
  }
  return 1;
}


static int parseTemporaries (lstObject *theMethod) {
  static lstObject *tempNames[256];
  int tCnt = 0;
  tempTop = maxTemp = 0;
  if (*ppos != '|') return 1;
  ppos++;
  skipSpaces();
  while (*ppos != '|') {
    if (!isIdentifierChar(*ppos)) return parseError("need identifier");
    if (readIdentifier()) return parseError("keyword illegal");
    checkVarName(tokenBuffer, 0);
    addTemporary(tokenBuffer);
    tempNames[tCnt++] = newSymbol(tokenBuffer);
  }
  if (tCnt > 0) {
    lstObject *aa = newArray(tCnt);
    while (--tCnt >= 0) aa->data[tCnt] = tempNames[tCnt];
    theMethod->data[lstIVtempNamesInMethod] = aa;
  } else {
    theMethod->data[lstIVtempNamesInMethod] = lstNilObj;
  }
  ppos++;
  skipSpaces();
  return 1;
}


static void showDebugInfo (void) {
/*
  int f;
  if (currentClass) {
    fprintf(stderr, "'%s>>%s' maxStackUsage=%d\n", lstBytePtr(currentClass->data[lstIVnameInClass]), lastMethodName, maxStackUsage);
  } else {
    fprintf(stderr, "'%s' maxStackUsage=%d\n", lastMethodName, maxStackUsage);
  }
  for (f = 1; f <= dbgInfoUsed; ++f) {
    fprintf(stderr, "  :%2d: line %2d; code: %d to %d\n", f, f, debugInfo[f].pcStart, debugInfo[f].pcEnd);
  }
*/
}


static void buildDebugInfo (lstObject *mth) {
  int cnt = 0, f;
  for (f = 1; f <= dbgInfoUsed; ++f) if (debugInfo[f].pcEnd >= 0) ++cnt;
  lstObject *a;
  mth->data[lstIVdebugInfoInMethod] = lstNilObj;
  /*printf(":cnt=%d\n", cnt);*/
  if (cnt < 1) a = lstNilObj;
  else {
    int f;
    lstByteObject *ba = binaryAlloc(cnt*(3*2)); /* record: 3 items by 2 bytes each */
    ba->stclass = lstByteArrayClass;
    int pos = 0;
    for (f = 1; f <= dbgInfoUsed; ++f) {
      if (debugInfo[f].pcEnd < 0) continue;
      int t = f;
      ba->bytes[pos+0] = (t&0xff);
      ba->bytes[pos+1] = ((t>>8)&0xff);
      t = debugInfo[f].pcStart;
      ba->bytes[pos+2] = (t&0xff);
      ba->bytes[pos+3] = ((t>>8)&0xff);
      t = debugInfo[f].pcEnd;
      ba->bytes[pos+4] = (t&0xff);
      ba->bytes[pos+5] = ((t>>8)&0xff);
      pos += 6;
    }
    a = (lstObject *)ba;
  }
  mth->data[lstIVdebugInfoInMethod] = a;
}


static int parseMethod (lstObject *theMethod, lstObject **realClass) {
  lstObject *oldCClass = currentClass;
  int res = 0;
  byteTop = 0;
  litTop = 0;
  argumentTop = 1;
  tempTop = maxTemp = 0;
  maxStackUsage = stackUsage = 1;
  dbgInfoUsed = 0;
  /*cgGenerateCode = 1;*/
  if (parseMethodHeader(theMethod)) {
    if (parseTemporaries(theMethod)) {
      if (parseBody()) {
        /*
        if (!strcmp(lastMethodName, "parse:with:") || !strcmp(lastMethodName, "aheadChar")) {
          printf("'%s' maxStackUsage=%d\n", lastMethodName, maxStackUsage);
        }
        */
        /*
        if (currentClass) {
          fprintf(stderr, "'%s>>%s' maxStackUsage=%d\n", lstBytePtr(currentClass->data[lstIVnameInClass]), lastMethodName, maxStackUsage);
        } else {
          fprintf(stderr, "'%s' maxStackUsage=%d\n", lastMethodName, maxStackUsage);
        }
        */
        theMethod->data[lstIVliteralsInMethod] = buildLiteralArray();
        theMethod->data[lstIVbyteCodesInMethod] = buildByteArray(1);
        /*fprintf(stderr, "STACK: %d\n", maxStackUsage);*/
        theMethod->data[lstIVstackSizeInMethod] = lstNewInt(maxStackUsage); /*FIXME: WTF is this 24?!*/
        theMethod->data[lstIVtemporarySizeInMethod] = lstNewInt(maxTemp);
        theMethod->data[lstIVclassInMethod] = currentClass;
        theMethod->data[lstIVtextInMethod] = newString(optStoreSource ? inputBuffer : "");
        theMethod->data[lstIVargCountInMethod] = lstNewInt(argumentTop);
        theMethod->data[lstIVdbgDataInMethod] = lstNilObj;
        theMethod->data[lstIVoptimDoneInMethod] = lstNilObj;
        theMethod->data[lstIVretResInMethod] = lstNilObj;
        theMethod->data[lstIVinvokeCountInMethod] = lstNewInt(0);
        if (optStoreDebugInfo) buildDebugInfo(theMethod); else theMethod->data[lstIVdebugInfoInMethod] = lstNilObj;
        res = 1;
        showDebugInfo();
      }
    }
  }
  *realClass = currentClass;
  currentClass = oldCClass;
  return res;
}


/* -------------------------------------------------------------
 *  Input Processing
 * ------------------------------------------------------------- */
static void MethodCommand (void) {
  lstObject *theMethod, *realClass;
  int meta = 0;
  char mname[257];
  /* read class name */
  if (ppos[0] == '^') { meta = 1; ppos++; }
  readIdentifier();
  if (!tokenBuffer[0]) sysError2Str("class name expected after METHOD", "");
  sprintf(mname, "%s%s", meta ? "Meta" : "", tokenBuffer);
  currentClass = lookupGlobal(mname, 1);
  if (!currentClass) sysError2Str("unknown class in Method", mname);
  strcpy(lastMethodName, mname);
  /*printf(":[%s]\n", lastMethodName);*/
  inputMethodText(1);
  ppos = inputBuffer;
  skipSpaces();
  theMethod = lstMemAlloc(lstMethodSize);
  /*theMethod->stclass = lookupGlobal("Method", 0);*/
  theMethod->stclass = lstMethodClass;
  /* fill in method class */
  theMethod->data[lstIVinvokeCountInMethod] = lstNewInt(0);
  theMethod->data[lstIVcategoryInMethod] = newString("");
  /* If successful compile, insert into the method dictionary */
  if (parseMethod(theMethod, &realClass)) {
    dictionaryInsert(realClass->data[lstIVmethodsInClass], theMethod->data[lstIVnameInMethod], theMethod);
  }
}


static void RawClassCommand (void) {
  lstObject *nClass, *supClass, *instClass;
  int instsize;
  /* read the class */
  readIdentifier();
  nClass = lookupGlobal(tokenBuffer, 1);
  if (optDumpClassNames) {
    printf("Class %s\n", tokenBuffer);
  }
  if (!nClass) {
    nClass = newClass(tokenBuffer);
    nClass->data[lstIVnameInClass] = newSymbol(tokenBuffer);
    addGlobalName(tokenBuffer, nClass);
  } else {
    /*printf("Duplicate class declaration: %s\n", tokenBuffer);*/
  }
  /* now read the instance class */
  readIdentifier();
  instClass = lookupGlobal(tokenBuffer, 1);
  if (!instClass) sysError2Str("can't find instance class", tokenBuffer);
  nClass->stclass = instClass;
  /* now read the super class */
  readIdentifier();
  supClass = lookupGlobal(tokenBuffer, 1);
  if (!supClass) sysError2Str("can't find super class", tokenBuffer);
  nClass->data[lstIVparentClassInClass] = supClass;
  /* rest are instance variables */
  litTop = 0;
  /* Now parse the new instance variables */
  while (*ppos) {
    if (!isIdentifierChar(*ppos)) sysError2Str("looking for var", ppos);
    readIdentifier();
    addLiteral(newSymbol(tokenBuffer));
  }
  /* That's the total of our instance variables */
  instsize = litTop;
  /* Add on size of superclass space */
  if (supClass != lstNilObj) instsize += lstIntValue(supClass->data[lstIVinstanceSizeInClass]);
  nClass->data[lstIVinstanceSizeInClass] = lstNewInt(instsize);
  nClass->data[lstIVvariablesInClass] = buildLiteralArray();
  /* make a tree for new methods */
  nClass->data[lstIVmethodsInClass] = newDictionary();
}


/*
 * ClassCommand()
 *  Build the base and meta classes automatically
 *
 * Doesn't support class variables, but handles most of imageSource
 * cases.
 */
static void ClassCommand (void) {
  char *stclass, *super, *ivars;
  /* Read the class and superclass */
  readIdentifier();
  stclass = strdup(tokenBuffer);
  readIdentifier();
  super = strdup(tokenBuffer);
  /* Stash away the instance variable string */
  skipSpaces();
  ivars = strdup(ppos);
  /* Build the metaclass */
  sprintf(inputBuffer, "RAWCLASS Meta%s Class Meta%s", stclass, super);
  ppos = inputBuffer+9;
  RawClassCommand();
  /* Now the instance class */
  sprintf(inputBuffer, "RAWCLASS %s Meta%s %s %s", stclass, stclass, super, ivars);
  ppos = inputBuffer+9;
  RawClassCommand();
  free(stclass);
  free(super);
  free(ivars);
}


/*
 * subclassCommand()
 *  Build the base and meta classes automatically
 */
static void subclassCommand (const char *super) {
  char *stclass, *cvars, *ivars;
  /* read new class name */
  readIdentifier();
  stclass = strdup(tokenBuffer);
  /* check for class variables */
  skipSpaces();
  if (ppos[0] != '|') {
    /* has something */
    char *start = ppos;
    while (ppos[0] && ppos[0] != '|') ppos++;
    char op = *ppos; *ppos = '\0';
    cvars = strdup(start);
    *ppos = op;
  } else {
    cvars = strdup("");
  }
  /* read instance variables */
  skipSpaces();
  if (ppos[0] == '|') { ++ppos; skipSpaces(); }
  ivars = strdup(ppos);
  /* Build the metaclass */
  /*
  printf("super: %s|\n", super);
  printf("class: %s|\n", stclass);
  printf("cvars: %s|\n", cvars);
  printf("ivars: %s|\n", ivars);
  */
  sprintf(inputBuffer, "RAWCLASS Meta%s Class Meta%s %s", stclass, super, cvars);
  ppos = inputBuffer+9;
  RawClassCommand();
  /* Now the instance class */
  sprintf(inputBuffer, "RAWCLASS %s Meta%s %s %s", stclass, stclass, super, ivars);
  ppos = inputBuffer+9;
  RawClassCommand();
  free(stclass);
  free(cvars);
  free(ivars);
}


/* -------------------------------------------------------------
 *  writing image
 * ------------------------------------------------------------- */
#define imageMaxNumberOfObjects  10000
static lstObject *writtenObjects[imageMaxNumberOfObjects];
static int imageTop = 0;


static void imageOut (FILE *fp, lstObject *obj) {
  int f, size;
  if (imageTop > imageMaxNumberOfObjects) {
    fprintf(stderr, "too many indirect objects\n");
    exit(1);
  }
  /* printf("writing out lstObject %d\n", obj); */
  if (obj == 0) {
    sysError2Str("writing out null lstObject", 0);
    return;
  }
  /* Integer objects are simply encoded as the binary value */
  if (LST_IS_SMALLINT(obj)) {
    int val = lstIntValue(obj);
    writeWord(fp, LST_IMGOT_SMALLINT);
    /*fwrite(&val, sizeof(val), 1, fp);*/
    lstImgWriteDWord(fp, val);
    return;
  }
  /* see if already written */
  for (f = 0; f < imageTop; f++) {
    if (obj == writtenObjects[f]) {
      if (f == 0) {
        writeWord(fp, LST_IMGOT_NIL);
      } else {
        writeWord(fp, LST_IMGOT_REF);
        writeWord(fp, f);
      }
      return;
    }
  }
  /* not written, do it now */
  writtenObjects[imageTop++] = obj;
  if (LST_IS_BYTES(obj)) {
    /* byte objects */
    int type;
    if (obj->stclass == 0) printf("byte object 0x%x has null class\n", (unsigned int)((uintptr_t)obj));
    lstByteObject *bobj = (lstByteObject *)obj;
    size = LST_SIZE(obj);
    if (obj->stclass == lstByteArrayClass) {
      /*fprintf(stderr, "BARRAY!\n");*/
      type = LST_IMGOT_BARRAY;
    } else if (obj->stclass == lstByteCodeClass) {
      /*fprintf(stderr, "BCODE!\n");*/
      type = LST_IMGOT_BCODE;
    } else if (obj->stclass == lstStringClass) {
      /*fprintf(stderr, "STRING!\n");*/
      type = LST_IMGOT_BARRAY;
    } else if (obj->stclass == lstSymbolClass) {
      /*fprintf(stderr, "SYMBOL!\n");*/
      type = LST_IMGOT_BARRAY;
    } else {
      type = LST_IMGOT_BARRAY;
      /*fprintf(stderr, "SHIT!\n"); exit(1);*/
      /* this can be Integer of Float */
    }
    writeWord(fp, type);
    writeWord(fp, size);
    /* fprintf(fp, "3 %d \n", size); */
    if (size > 0) {
      if (fwrite(bobj->bytes, size, 1, fp) != 1) {
        fprintf(stderr, "ERROR: error writing image file!\n");
        exit(1);
      }
    }
    if (type == LST_IMGOT_BCODE) lstWritePrimPatches(fp, bobj);
    imageOut(fp, obj->stclass);
    return;
  }
  /* ordinary objects */
  size = LST_SIZE(obj);
  /* fprintf(fp,"1 %d ", size); */
  writeWord(fp, LST_IMGOT_OBJECT);
  writeWord(fp, size);
  if (obj->stclass == 0) printf("object 0x%x has null class\n", (unsigned int)((uintptr_t)obj));
  /*printf("writing: %s\n", lstBytePtr(obj->stclass->data[lstIVnameInClass]));*/
  imageOut(fp, obj->stclass);
  for (f = 0; f < size; f++) imageOut(fp, obj->data[f]);
}


/* -------------------------------------------------------------
 *  fix up symbol tables
 * ------------------------------------------------------------- */
static lstObject *symbolTreeInsert (lstObject *base, lstObject *symNode) {
  if (base == lstNilObj) return symNode;
  if (symbolCmp(symNode->data[lstIVvalueInNode], base->data[lstIVvalueInNode]) < 0)
    base->data[lstIVleftInNode] = symbolTreeInsert(base->data[lstIVleftInNode], symNode);
  else
    base->data[lstIVrightInNode] = symbolTreeInsert(base->data[lstIVrightInNode], symNode);
  return base;
}


static lstObject *fixSymbols (void) {
  LstUInt f;
  lstObject *t = newTree();
  for (f = 0; f < symbolTop; f++) t->data[0] = symbolTreeInsert(t->data[0], newNode(oldSymbols[f], lstNilObj, lstNilObj));
  return t;
}


static void checkPackageO (void) {
  int f;
  lstObject *psys = enterPackageEx("System", -1);
  lstObject *phos = enterPackageEx("HostOS", -1);
  /*printf("checking packages...\n");*/
  for (f = 0; f < globalTop; f++) {
    if (strcmp(globalNames[f], "globals") == 0) continue;
    if (strcmp(globalNames[f], "Smalltalk") == 0) continue;
    if (globalNames[f][0] < 'A' || globalNames[f][0] > 'Z') continue;
    lstObject *o = lookupGlobal(globalNames[f], 0);
    if (LST_IS_SMALLINT(o)) abort();
    if (LST_IS_BYTES(o)) abort();
    if (LST_SIZE(o) < lstClassSize) abort();
    lstObject *pk = o->data[lstIVpackageInClass];
    /*if (!o) abort();*/
    if (pk != lstNilObj) {
      /*printf("%s in %s\n", globalNames[f], lstBytePtr(pk->data[lstIVnameInPackage]));*/
      continue;
    }
    /*printf("G: [%s] %p; sz=%d\n", globalNames[f], o, LST_SIZE(o));*/
    /*lstObject *o = */
    /*printf("***NOT IN PACKAGE: [%s]; goes to 'System'\n", globalNames[f]);*/
    if (strcmp(globalNames[f], "System") == 0) {
      o->data[lstIVpackageInClass] = phos;
      insertClassInPackage(phos, globalNames[f], o);
    } else {
      o->data[lstIVpackageInClass] = psys;
      insertClassInPackage(psys, globalNames[f], o);
    }
  }
}

static void fixGlobals (void) {
  int f;
  lstObject *t = lstGlobalObj;
  t->stclass = lookupGlobal("Dictionary", 0);
  t->data[0] = newOrderedArray();
  t->data[1] = newArray(0);
  /* Insert each class name as a reference to the class lstObject itself */
  for (f = 0; f < globalTop; f++) {
    if (strncmp(globalNames[f], "Meta", 4) == 0) continue;
    dictionaryInsert(t, newSymbol(globalNames[f]), globals[f]);
  }
  /* Insert this object itself under the name "Smalltalk" */
  dictionaryInsert(t, newSymbol("Smalltalk"), t);
  checkPackageO();
  /*checkPackageO();*/
}


/* -------------------------------------------------------------
 *  checkGlobals
 * ------------------------------------------------------------- */
static void checkGlobals (void) {
  int f;
  for (f = 0; f < globalTop; f++) {
    lstObject *o = globals[f];
    if (!o->stclass) sysError2Str("Never defined", globalNames[f]);
  }
}


enum {
  MAX_FILE_STACK=128
};
static FILE *fileStack[MAX_FILE_STACK];
static char *fileNames[MAX_FILE_STACK];
static int fSP = 0; /* first free item */


static void openNewSrcFile (const char *name) {
  char *buf = calloc(1, strlen(name)+32);
  strcpy(buf, name);
  FILE *fl = fopen(buf, "rb");
  if (!fl) {
    sprintf(buf, "%s.st", name);
    if ((fl = fopen(buf, "rb")) == NULL) {
      free(buf);
      fprintf(stderr, "ERROR: no input file: %s\n", name);
      exit(1);
    }
  }
  if (fSP >= MAX_FILE_STACK) {
    free(buf);
    fprintf(stderr, "ERROR: too many INCLUDEs\n");
    exit(1);
  }
  fileNames[fSP] = buf;
  fileStack[fSP++] = fl;
  printf("parsing %s\n", buf);
  fin = fl;
}


/* res!=0: not last */
static int closeSrcFile (void) {
  if (fSP < 1) {
    fprintf(stderr, "ERROR: INCLUDE stack underflow\n");
    exit(1);
  }
  /*printf("CLOSING: %s\n", fileNames[fSP-1]);*/
  free(fileNames[fSP-1]);
  fclose(fileStack[--fSP]);
  if (fSP > 0) {
    /*printf("CONTINUE: %s\n", fileNames[fSP-1]);*/
    fin = fileStack[fSP-1];
  } else {
    fin = NULL;
  }
  return fSP>0;
}


static void messageCommand (void) {
  printf("MSG: %s", ppos);
}


static char *parseIncModArg (void) {
  static char buf[8192];
  while (*ppos && *ppos != '\n' && (unsigned char)(*ppos) <= ' ') ppos++;
  while (ppos[0] && (unsigned char)(ppos[strlen(ppos)-1]) <= ' ') ppos[strlen(ppos)-1] = '\0';
  strcpy(buf, ppos);
  return buf;
}


static void includeCommand (void) {
  static char buf[8192];
  char *fname = parseIncModArg();
  if (!fname[0]) {
    fprintf(stderr, "ERROR: INCLUDE without file name\n");
    exit(1);
  }
  strcpy(buf, fileNames[fSP-1]);
  char *sep = strrchr(buf, '/');
  if (sep) strcpy(sep+1, fname); else strcpy(buf, fname);
  openNewSrcFile(buf);
}


static void moduleCommand (void) {
  static char buf[8192];
  char *fname = parseIncModArg();
  if (!fname[0]) {
    fprintf(stderr, "ERROR: MODULE without module name\n");
    exit(1);
  }
  strcpy(buf, fileNames[fSP-1]);
  char *sep = strrchr(buf, '/');
  if (sep) strcpy(sep+1, fname); else strcpy(buf, fname);
  strcat(buf, "/ModuleMain");
  openNewSrcFile(buf);
}


static void extPrimCommand (void) {
  newExtPrim(ppos);
}


static void packageCommand (void) {
  char *pkgName;
  int ver;
  readIdentifier(); /* package name */
  pkgName = strdup(tokenBuffer);
  ver = readInteger();
  /*printf("PACKAGE %s version %d\n", pkgName, ver);*/
  currentPackage = enterPackage(pkgName, ver);
  free(pkgName);
}


static void dumpUnusedPrims (void) {
  const LSTPrimDef *pd;
  for (pd = lstPrimList; pd->name; pd++) {
    if (primsUsed[pd->idx]) continue;
    printf("primitive #%d: NEVER USED!  [%s]\n", pd->idx, pd->name);
  }
}


static int isEmptyLine (void) {
  ppos = inputBuffer;
  skipSpaces();
  if (!ppos[0]) return 1;
  readIdentifier();
  if (strcmp(tokenBuffer, "COMMENT") == 0) return 1;
  /*TODO: COMMENDS ... ENDCOMMENTS*/
  return 0;
}


static int isMFCEnd (void) {
  ppos = inputBuffer;
  skipSpaces();
  if (!ppos[0]) return 1;
  if (ppos[0] == '!') {
    ppos++;
    while (*ppos && (*ppos == '\t' || *ppos == '\r' || *ppos == '\n' || *ppos == ' ')) ppos++;
    if (!ppos[0]) return 1;
  }
  return 0;
}


/* -------------------------------------------------------------
 *  main program
 * ------------------------------------------------------------- */
int main (int argc, char *argv[]) {
  int f, wasInName = 0;

  memset(primsUsed, 0, sizeof(primsUsed));

  /* big bang -- create the first classes */
  bigBang();
  addArgument("self");

  for (f = 1; f < argc; f++) {
    char *arg = argv[f];
    if (arg[0] == '-') {
      int c;
      for (c = 1; arg[c]; c++) {
        switch (arg[c]) {
          case 'm':
            optDumpMethodNames = 1;
            break;
          case 'c':
            optDumpClassNames = 1;
            break;
          case 'S':
            optStoreSource = 0;
            break;
          case 'D':
            optStoreDebugInfo = 0;
            break;
          case 'h':
            printf(
              "args:\n"
              " -m  dump method names\n"
              " -c  dump class names\n"
              " -S  do not store image sources\n"
              " -D  do not store debug info\n"
            );
            exit(1);
        }
      }
    } else {
      if (wasInName) {
        fprintf(stderr, "ERROR: too many image source files\n");
        exit(1);
      }
      wasInName = 1;
      openNewSrcFile(argv[f]);
    }
  }
  if (!wasInName) {
    openNewSrcFile("imgsrc/main");
  }


  int inComments = 0, inMethodsFor = 0;
  char mfcName[256];
  do {
    /* then read the image source file */
    while (fgets((char *)inputBuffer, 1000, fin)) {
againLine:
      if (inputBuffer[0] == '"') {
        /*FIXME: do it better!*/
        char *t = strchr(inputBuffer, '"'), *p;
        p = inputBuffer;
        while (p < t) { if ((unsigned char)*t <= ' ') ++t; }
        if (p == t) {
          p = strchr(p+1, '"');
          if (p) {
            strcpy(inputBuffer, p+1);
            goto againLine;
          }
          /* multi-line comment */
          while (fgets((char *)inputBuffer, 1000, fin)) {
            p = strchr(inputBuffer, '"');
            if (!p) continue;
            strcpy(inputBuffer, p+1);
            goto againLine;
          }
          sysError2Str("unterminated comment", "");
        }
      }
      for (;;) {
        char ib2[1024];
        char *e = inputBuffer+strlen(inputBuffer)-1;
        while (e >= inputBuffer && (isblank(*e) || *e == '\n' || *e == '\r')) --e;
        if (e >= inputBuffer && *e == '\\') {
          /* line continuation */
          while (e > inputBuffer && isblank(e[-1])) --e;
          *e++ = ' ';
          if (!fgets(ib2, 1000, fin)) break;
          while (ib2[0] && isblank(ib2[0])) strcpy(ib2, ib2+1);
          strcpy(e, ib2);
          /*printf(":%s", inputBuffer);*/
          continue;
        }
        break;
      }
      if (inMethodsFor) {
        if (isEmptyLine()) continue;
        if (inComments) {
          if (strcmp(tokenBuffer, "ENDCOMMENTS") == 0) { inComments = 0; continue; }
          continue;
        }
        /* */
        if (strcmp(tokenBuffer, "COMMENTS") == 0) { inComments = 1; continue; }
        if (strcmp(tokenBuffer, "RAWCLASS") == 0 ||
            strcmp(tokenBuffer, "CLASS") == 0 ||
            strcmp(tokenBuffer, "METHOD") == 0 ||
            strcmp(tokenBuffer, "METHODS") == 0 ||
            strcmp(tokenBuffer, "END") == 0 ||
            strcmp(tokenBuffer, "MODULE") == 0 ||
            strcmp(tokenBuffer, "ENTRY") == 0 ||
            strcmp(tokenBuffer, "INCLUDE") == 0 ||
            strcmp(tokenBuffer, "PACKAGE") == 0) {
          sysError2Str("directive inside METHODS FOR ", tokenBuffer);
        }
        if (strcmp(tokenBuffer, "EXTPRIM") == 0 || strcmp(tokenBuffer, "EXTPRIMITIVE") == 0) {
          extPrimCommand();
          continue;
        } else if (strcmp(tokenBuffer, "MESSAGE") == 0) {
          messageCommand();
          continue;
        }
        /* */
        if (isMFCEnd()) { inMethodsFor = 0; continue; }
        /* method */
        /*ppos = inputBuffer+strlen(inputBuffer);*/
        strcpy(lastMethodName, "");
        inputMethodText(0);
        ppos = inputBuffer;
        skipSpaces();
        lstObject *theMethod = lstMemAlloc(lstMethodSize), *realClass;
        /*theMethod->stclass = lookupGlobal("Method", 0);*/
        theMethod->stclass = lstMethodClass;
        /* fill in method class */
        theMethod->data[lstIVinvokeCountInMethod] = lstNewInt(0);
        /* If successful compile, insert into the method dictionary */
        if (parseMethod(theMethod, &realClass)) {
          dictionaryInsert(realClass->data[lstIVmethodsInClass], theMethod->data[lstIVnameInMethod], theMethod);
        }
        continue;
      }
      /* not in class methods */
      ppos = inputBuffer;
      skipSpaces();
      readIdentifier();
      if (inComments) {
        if (strcmp(tokenBuffer, "ENDCOMMENTS") == 0) {
          inComments = 0;
        }
      } else {
        if (strcmp(tokenBuffer, "") == 0) {
          /* ignore empty line */
        } else if (strcmp(tokenBuffer, "RAWCLASS") == 0) {
          RawClassCommand();
        } else if (strcmp(tokenBuffer, "CLASS") == 0) {
          ClassCommand();
        } else if (strcmp(tokenBuffer, "COMMENT") == 0) {
          /* nothing */ ;
        } else if (strcmp(tokenBuffer, "COMMENTS") == 0) {
          inComments = 1;
        } else if (strcmp(tokenBuffer, "METHOD") == 0) {
          readIdentifier();
          if (strcmp(tokenBuffer, "FOR")) sysError2Str("FOR expected after METHOD", "");
          MethodCommand();
        } else if (strcmp(tokenBuffer, "END") == 0) {
          goto parsedone;
        } else if (strcmp(tokenBuffer, "INCLUDE") == 0) {
          includeCommand();
        } else if (strcmp(tokenBuffer, "MODULE") == 0) {
          moduleCommand();
        } else if (strcmp(tokenBuffer, "EXTPRIM") == 0 || strcmp(tokenBuffer, "EXTPRIMITIVE") == 0) {
          extPrimCommand();
        } else if (strcmp(tokenBuffer, "MESSAGE") == 0) {
          messageCommand();
        } else if (strcmp(tokenBuffer, "PACKAGE") == 0) {
          packageCommand();
        } else if (strcmp(tokenBuffer, "METHODS") == 0) {
          int meta = 0;
          readIdentifier();
          if (strcmp(tokenBuffer, "FOR")) sysError2Str("FOR expected after METHODS", "");
          skipSpaces();
          if (ppos[0] == '^') { meta = 1; ppos++; }
          readIdentifier();
          if (!tokenBuffer[0]) sysError2Str("class name expected after METHODS FOR", "");
          sprintf(mfcName, "%s%s", meta ? "Meta" : "", tokenBuffer);
          currentClass = lookupGlobal(mfcName, 1);
          if (!currentClass) sysError2Str("unknown class in METHODS FOR", mfcName);
          inMethodsFor = 1;
          /*fprintf(stderr, "methods for %s\n", mfcName);*/
        } else if (tokenBuffer[0] && lookupGlobal(tokenBuffer, 1)) {
          /* maybe XXX subclass: yyy? */
          static char clName[257];
          strcpy(clName, tokenBuffer);
          skipSpaces();
          readIdentifier();
          if (strcmp(tokenBuffer, "subclass:") == 0) {
            subclassCommand(clName);
          } else {
            sysError2Str("'subclass:' expected, but got", tokenBuffer);
          }
        } else {
          sysError2Str("unknown command ", tokenBuffer);
        }
      }
    }
    if (inComments) sysError2Str("unterminated COMMENTS", "");
    if (inMethodsFor) sysError2Str("unterminated METHODS FOR", "");
  } while (closeSrcFile());
parsedone:
  if (fSP > 1) sysError2Str("END inside included file", "");
  if (inComments) sysError2Str("unterminated COMMENTS", "");
  if (inMethodsFor) sysError2Str("unterminated METHODS FOR", "");

  /* default package for user-defined classes */
  PackageClass->data[lstIVcurrentInMetaPackage] = enterPackageEx("User", 0);
  /*
  {
    lstObject *o = enterPackageEx("System", 1);
    insertClassInPackage(o, "Package", PackageClass);
  }
  */

  /* then create the tree of symbols */
  lstSymbolClass->data[lstIVsymbolsInMetaSymbol] = fixSymbols();
  fixGlobals();
  /* see if anything was never defined in the class source */
  checkGlobals();

  dumpUnusedPrims();

  FILE *fd;
  if ((fd = fopen(defaultImageName, "wb")) == NULL) sysError2Str("file out error", "image");
  printf("starting to file out\n");
  {
    uint8_t ver = LST_IMAGE_VERSION;
    const char *imgSign = LST_IMAGE_SIGNATURE;
    fwrite(imgSign, strlen(imgSign)+1, 1, fd);
    fwrite(&ver, sizeof(ver), 1, fd);
  }
  dprintf("writing 'nil'\n");
  imageOut(fd, lstNilObj);
  dprintf("writing 'true'\n");
  imageOut(fd, lstTrueObj);
  dprintf("writing 'false'\n");
  imageOut(fd, lstFalseObj);
  dprintf("writing globals\n");
  imageOut(fd, lstGlobalObj);
  imageOut(fd, newSymbol("doesNotUnderstand:args:"));
  {
    int f;
    for (f = 0; binaryBuiltIns[f]; ++f) ;
    writeWord(fd, f);
    for (f = 0; binaryBuiltIns[f]; ++f) imageOut(fd, newSymbol(binaryBuiltIns[f]));
  }
  fclose(fd);
  printf("%d objects written\n", imageTop);
  printf("bye for now!\n");
/*
  printf("%d\n", LST_SIZE(lookupGlobal("Context", 0)));
  printf("%d\n", lstIntValue(lookupGlobal("Context", 0)->data[lstIVinstanceSizeInClass]));
*/
/*
  printf("%d\n", LST_SIZE(lookupGlobal("Char", 0)));
  printf("%d\n", lstIntValue(lookupGlobal("Char", 0)->data[lstIVinstanceSizeInClass]));
*/
/*
  printf("%d\n", LST_SIZE(lookupGlobal("Block", 0)));
  printf("%d\n", lstIntValue(lookupGlobal("Block", 0)->data[lstIVinstanceSizeInClass]));
*/
  return 0;
}
