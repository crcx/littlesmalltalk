/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#ifndef HSREGEXP_H
#define HSREGEXP_H


#define HS_NSUBEXP  10

typedef struct HSRegExp {
  char *startp[HS_NSUBEXP];
  char *endp[HS_NSUBEXP];
  char regstart;   /* Internal use only. */
  char reganch;    /* Internal use only. */
  char *regmust;   /* Internal use only. */
  int regmlen;     /* Internal use only. */
  char program[0]; /* Unwarranted chumminess with compiler. */
} HSRegExp;


extern HSRegExp *hsRegComp (const char *re);
extern int hsRegExec (HSRegExp *rp, const char *s);
extern void hsRegSub (const HSRegExp *rp, const char *src, char *dst);

extern void hsRegError (const char *message);


#ifdef DEBUG
extern int regnarrate;
void regdump (HSRegExp *r);
char *regprop (char *op);
#endif


#endif
