/*
 * coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * Understanding is not required. Only obedience.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#include <stdlib.h>
#include <string.h>

#include "lstcore/k8lst.h"

#include "trex.h"
#include "hsregexp.h"


static const char *rexId = "RegExpHandle";

typedef struct {
  const char *type;
  const char *bos;
  union {
    HSRegExp *hs;
    TRex *tx;
  };
  int vmtId;
} RegExpInfo;


/* HS */
static void reClearHS (RegExpInfo *re) {
  if (re->hs) free(re->hs);
}


static const char *hsErrMsg = NULL;
void hsRegError (const char *message) {
  hsErrMsg = message;
}


static RegExpInfo *reCompHS (const char *restr, const char **error) {
  hsErrMsg = NULL;
  *error = NULL;
  HSRegExp *re = hsRegComp(restr);
  if (!re) { *error = hsErrMsg; return NULL; }
  RegExpInfo *res = malloc(sizeof(RegExpInfo));
  if (!res) { free(re); *error = "out of memory"; return NULL; }
  res->type = rexId;
  res->hs = re;
  res->vmtId = 0;
  return res;
}


static int reMatchHS (RegExpInfo *rx, const char *str) {
  if (!rx->hs) return 0;
  hsErrMsg = NULL;
  rx->bos = str;
  if (hsRegExec(rx->hs, str)) return 1;
  /* check error */
  return 0;
}


int reSubCountHS (RegExpInfo *rx) {
  int res;
  if (!rx->hs) return 0;
  for (res = HS_NSUBEXP-1; res >= 0; --res) {
    if (rx->hs->startp[res] && rx->hs->endp[res]) break;
  }
  return res+1;
}


int reSubStartHS (RegExpInfo *rx, int idx) {
  if (!rx->hs || idx < 0 || idx >= HS_NSUBEXP) return -1;
  if (!rx->hs->startp[idx] || !rx->hs->endp[idx]) return -1;
  return (intptr_t)(rx->hs->startp[idx]-rx->bos);
}


int reSubEndHS (RegExpInfo *rx, int idx) {
  if (!rx->hs || idx < 0 || idx >= HS_NSUBEXP) return -1;
  if (!rx->hs->startp[idx] || !rx->hs->endp[idx]) return -1;
  return (intptr_t)(rx->hs->endp[idx]-rx->bos);
}


/* TX */
static void reClearTX (RegExpInfo *re) {
  if (re->tx) trex_free(re->tx);
}


static RegExpInfo *reCompTX (const char *restr, const char **error) {
  *error = NULL;
  TRex *re = trex_compile(restr, error);
  if (!re) return NULL;
  RegExpInfo *res = malloc(sizeof(RegExpInfo));
  if (!res) { free(re); *error = "out of memory"; return NULL; }
  res->type = rexId;
  res->tx = re;
  res->vmtId = 1;
  return res;
}


static int reMatchTX (RegExpInfo *rx, const char *str) {
  if (!rx->tx) return 0;
  rx->bos = str;
  return trex_match(rx->tx, str)==TRex_True ? 1 : 0;
}


int reSubCountTX (RegExpInfo *rx) {
  if (!rx->tx) return 0;
  return trex_getsubexpcount(rx->tx);
}


int reSubStartTX (RegExpInfo *rx, int idx) {
  if (!rx->tx || idx < 0 || idx >= trex_getsubexpcount(rx->tx)) return -1;
  TRexMatch m;
  if (trex_getsubexp(rx->tx, idx, &m) != TRex_True) return -1;
  return (intptr_t)(m.begin-rx->bos);
}


int reSubEndTX (RegExpInfo *rx, int idx) {
  if (!rx->tx || idx < 0 || idx >= trex_getsubexpcount(rx->tx)) return -1;
  TRexMatch m;
  if (trex_getsubexp(rx->tx, idx, &m) != TRex_True) return -1;
  return (intptr_t)(m.begin-rx->bos)+m.len;
}


static struct {
  void (*reClear) (RegExpInfo *re);
  RegExpInfo *(*reComp) (const char *restr, const char **error);
  int (*reMatch) (RegExpInfo *rx, const char *str);
  int (*reSubCount) (RegExpInfo *rx);
  int (*reSubStart) (RegExpInfo *rx, int idx);
  int (*reSubEnd) (RegExpInfo *rx, int idx);
} vmt[2] = {
  {reClearHS, reCompHS, reMatchHS, reSubCountHS, reSubStartHS, reSubEndHS},
  {reClearTX, reCompTX, reMatchTX, reSubCountTX, reSubStartTX, reSubEndTX},
};


LST_FINALIZER(lpRegExpFinalizer) {
  RegExpInfo *re = (RegExpInfo *)udata;
  if (re) {
    vmt[re->vmtId].reClear(re);
    free(re);
  }
}


static lstObject *newRegExp (const char *restr, int type) {
  if (type < 0 || type > 1) return lstNewString("internal error");
  lstObject *res = lstNewBinary(NULL, 0);
  const char *error = NULL;
  RegExpInfo *re = vmt[type].reComp(restr, &error);
  if (!re) return lstNewString(error ? error : "unknown error");
  lstSetFinalizer(res, lpRegExpFinalizer, re);
  return res;
}


static RegExpInfo *getRE (lstObject *o) {
  if (LST_IS_SMALLINT(o) || !LST_IS_BYTES(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  RegExpInfo *re = (RegExpInfo *)o->fin->udata;
  if (re->type != rexId) return NULL;
  return re;
}


LST_PRIMFN(lpRegExpDo) {
  if (LST_PRIMARGC < 2 || !LST_IS_SMALLINT(LST_PRIMARG(0))) return NULL;
  int op = lstIntValue(LST_PRIMARG(0));
  lstObject *res = lstNilObj;
  char *str = NULL; int slen = 0, ti;
  if (op >= 0 && op <= 1) {
    /* 3rd arg is string */
    if (LST_PRIMARGC < 3 || !LST_IS_BYTES(LST_PRIMARG(2))) return NULL;
    slen = LST_SIZE(LST_PRIMARG(2));
    str = malloc(slen+4);
    if (!str) return NULL;
    lstGetString(str, slen+2, LST_PRIMARG(2));
  }
  if (op == 0) {
    /* create regexp: type str */
    if (LST_PRIMARGC < 3 || !LST_IS_SMALLINT(LST_PRIMARG(1))) return NULL;
    res = newRegExp(str, lstIntValue(LST_PRIMARG(1)));
    free(str);
    return res;
  }
  RegExpInfo *re = getRE(LST_PRIMARG(1));
  if (!re) return NULL;
  switch (op) {
    case 1: /* match regexp: str */
      if (LST_PRIMARGC < 3 || !LST_IS_BYTES(LST_PRIMARG(2))) return NULL;
      ti = vmt[re->vmtId].reMatch(re, str);
      free(str);
      res = ti ? lstTrueObj : lstFalseObj;
      break;
    case 2: /* # of captures */
      res = lstNewInt(vmt[re->vmtId].reSubCount(re));
      break;
    case 3: /* capture start: idx */
      if (LST_PRIMARGC < 3 || !LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
      ti = lstIntValue(LST_PRIMARG(2));
      res = lstNewInt(vmt[re->vmtId].reSubStart(re, ti));
      break;
    case 4: /* capture end: idx */
      if (LST_PRIMARGC < 3 || !LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
      ti = lstIntValue(LST_PRIMARG(2));
      res = lstNewInt(vmt[re->vmtId].reSubEnd(re, ti));
      break;
    default:
      return NULL;
  }
  return res;
}


static const LSTExtPrimitiveTable primTbl[] = {
{"RegExpDo", lpRegExpDo, NULL},
{0}};


void lstInitPrimitivesRegExp (void) {
  lstRegisterExtPrimitiveTable(primTbl);
}
