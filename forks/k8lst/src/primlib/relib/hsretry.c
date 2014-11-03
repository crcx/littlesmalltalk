/*
 * Simple test program for HSRegExp(3) stuff.  Knows about debugging hooks.
 * Usage: try re [string [output [-]]]
 * The re is compiled and dumped, regexeced against the string, the result
 * is applied to output using hsRegSub().  The - triggers a running narrative
 * from hsRegExec().  Dumping and narrative don't happen unless DEBUG.
 *
 * If there are no arguments, stdin is assumed to be a stream of lines with
 * five fields:  a r.e., a string to match it against, a result code, a
 * source string for hsRegSub, and the proper result.  Result codes are 'c'
 * for compile failure, 'y' for match success, 'n' for match failure.
 * Field separator is tab.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hsregexp.h"


#ifdef DEBUG
extern int regnarrate;
#endif

static char buf[BUFSIZ];

static int errreport = 0;    /* Report errors via errseen? */
static const char *errseen = NULL;   /* Error message. */
static int status = 0;     /* Exit status. */

void hsRegError (const char *s) {
  if (errreport) errseen = s; else perror(s);
  exit(1);
}


static int lineno = 0;
static HSRegExp badregexp = {{0}};


static void complain (char *s1, char *s2) {
  fprintf(stderr, "try: %d: ", lineno);
  fprintf(stderr, s1, s2);
  fprintf(stderr, " (%s)\n", (errseen != NULL) ? errseen : "");
  status = 1;
}


static void try (char **fields) {
  HSRegExp *r;
  char dbuf[BUFSIZ];
  errseen = NULL;
  r = hsRegComp(fields[0]);
  if (r == NULL) {
    if (*fields[2] != 'c') complain("hsRegComp failure in `%s'", fields[0]);
    return;
  }
  if (*fields[2] == 'c') {
    complain("unexpected hsRegComp success in `%s'", fields[0]);
    free(r);
    return;
  }
  if (!hsRegExec(r, fields[1])) {
    if (*fields[2] != 'n') complain("hsRegExec failure in `%s'", fields[0]);
    free(r);
    return;
  }
  if (*fields[2] == 'n') {
    complain("unexpected hsRegExec success", "");
    free(r);
    return;
  }
  errseen = NULL;
  hsRegSub(r, fields[3], dbuf);
  if (errseen != NULL) {
    complain("hsRegSub complaint", "");
    free(r);
    return;
  }
  if (strcmp(dbuf, fields[4]) != 0) complain("hsRegSub result `%s' wrong", dbuf);
  free(r);
}


static void multiple (void) {
  char rbuf[BUFSIZ];
  char *field[5];
  char *scan;
  int i;
  HSRegExp *r;
  errreport = 1;
  lineno = 0;
  printf("testing...\n");
  while (fgets(rbuf, sizeof(rbuf), stdin) != NULL) {
    rbuf[strlen(rbuf)-1] = '\0';  /* Dispense with \n. */
    lineno++;
    scan = rbuf;
    for (i = 0; i < 5; i++) {
      field[i] = scan;
      if (field[i] == NULL) {
        complain("bad testfile format", "");
        exit(1);
      }
      scan = strchr(scan, '\t');
      if (scan != NULL)
        *scan++ = '\0';
    }
    try(field);
  }
  /* And finish up with some internal testing... */
  lineno = 9990;
  errseen = NULL;
  if (hsRegComp((char *)NULL) != NULL || errseen == NULL) complain("hsRegComp(NULL) doesn't complain", "");
  lineno = 9991;
  errseen = NULL;
  if (hsRegExec((HSRegExp *)NULL, "foo") || errseen == NULL) complain("hsRegExec(NULL, ...) doesn't complain", "");
  lineno = 9992;
  r = hsRegComp("foo");
  if (r == NULL) {
    complain("hsRegComp(\"foo\") fails", "");
    return;
  }
  lineno = 9993;
  errseen = NULL;
  if (hsRegExec(r, (char *)NULL) || errseen == NULL) complain("hsRegExec(..., NULL) doesn't complain", "");
  lineno = 9994;
  errseen = NULL;
  hsRegSub((HSRegExp *)NULL, "foo", rbuf);
  if (errseen == NULL) complain("hsRegSub(NULL, ..., ...) doesn't complain", "");
  lineno = 9995;
  errseen = NULL;
  hsRegSub(r, (char *)NULL, rbuf);
  if (errseen == NULL) complain("hsRegSub(..., NULL, ...) doesn't complain", "");
  lineno = 9996;
  errseen = NULL;
  hsRegSub(r, "foo", (char *)NULL);
  if (errseen == NULL) complain("hsRegSub(..., ..., NULL) doesn't complain", "");
  lineno = 9997;
  errseen = NULL;
  if (hsRegExec(&badregexp, "foo") || errseen == NULL) complain("hsRegExec(nonsense, ...) doesn't complain", "");
  lineno = 9998;
  errseen = NULL;
  hsRegSub(&badregexp, "foo", rbuf);
  if (errseen == NULL) complain("hsRegSub(nonsense, ..., ...) doesn't complain", "");
  printf("testing complete.\n");
}


int main (int argc, char *argv[]) {
  HSRegExp *r;
  int i;

  if (argc == 1) {
    multiple();
    exit(status);
  }

  r = hsRegComp(argv[1]);
  if (r == NULL) { perror("hsRegComp failure"); exit(1); }
#ifdef DEBUG
  regdump(r);
  if (argc > 4) regnarrate++;
#endif
  if (argc > 2) {
    i = hsRegExec(r, argv[2]);
    printf("%d", i);
    for (i = 1; i < HS_NSUBEXP; i++) if (r->startp[i] != NULL && r->endp[i] != NULL) printf(" \\%d", i);
    printf("\n");
  }
  if (argc > 3) {
    hsRegSub(r, argv[3], buf);
    printf("%s\n", buf);
  }
  return status;
}
