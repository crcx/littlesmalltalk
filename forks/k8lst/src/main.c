/*
 * Main module, some primitives and global functions
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

/* Notes on the indentation:
 * Use GNU indent to format the C source code of the LittleSmalltalk virtual machine.
 * Use these options for the formatter:
 * -bad -bap -bbb -sob -cdb -bli0 -ncdw -cbi0 -cli2 -ss -npcs -nbs
 * -nsaf -nsai -nsaw -nprs -di16 -bc -bls -npsl -lp -ts2 -ppi3 -i2 -nut -sbi0
 */

/*
  Little Smalltalk main program

  starting point, primitive handler for unix
  version of the little smalltalk system
*/
#include "lstcore/k8lst.h"
#include "primlib/lst_primitives.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifndef LST_ON_WINDOWS
# include <signal.h>
# include <time.h>
#else
# include <windows.h>
#endif


#ifdef USE_FLTK
# include "lstfltk.h"
#endif

#ifdef LST_USE_IUP
# include "lstiup.h"
#endif


/* the following defaults must be set */
#ifndef DEFAULT_IMAGE_FILE
# define DEFAULT_IMAGE_FILE "lst.image"
# define DEFAULT_WORK_IMAGE_FILE "workimage.image"
#endif

#ifdef __LP64__
# define DEFAULT_STATIC_SIZE   1024*1024
# define DEFAULT_DYNAMIC_SIZE  1024*1024*2
#else
# define DEFAULT_STATIC_SIZE   512*1024
# define DEFAULT_DYNAMIC_SIZE  1024*1024*1
#endif


/* declared as extern in lst_glob.h */
void lstFatal (const char *a, unsigned int b) {
  fprintf(stderr, "unrecoverable system error: %s 0x%x\n", a, b);
  exit(1);
}


/* declared as extern in lst_interp.h */
void lstBackTrace (lstObject *aContext) {
  char nbuf[257];
  printf("=============\nbacktrace:\n");
  while (aContext && aContext != lstNilObj) {
    lstObject *args, *dd;
    LstUInt i; int pc;
    dd = aContext->data[lstIVmethodInContext];
    if (LST_CLASS(dd) != lstMethodClass) break;
    lstGetString(nbuf, 256, dd->data[lstIVnameInMethod]);
    if (LST_IS_SMALLINT(aContext->data[lstIVbytePointerInContext])) {
      printf(" %04d: ", (pc = lstIntValue(aContext->data[lstIVbytePointerInContext])));
    } else pc = 0;
    dd = dd->data[lstIVdebugInfoInMethod];
    /*printf("!00 %p, %p, %p\n", dd, LST_CLASS(dd), lstByteArrayClass);*/
    if (LST_CLASS(dd) == lstByteArrayClass) {
      //printf("!01\n");
      int line = 0, ostart = 0, oend = LST_SIZE(dd);
      for (i = 0; i+5 < LST_SIZE(dd); i += 6) {
        int s = lstBytePtr(dd)[i+2] + 256*lstBytePtr(dd)[i+3];
        int e = lstBytePtr(dd)[i+4] + 256*lstBytePtr(dd)[i+5];
        /*
        int l = lstBytePtr(dd)[i+0] + 256*lstBytePtr(dd)[i+1];
        printf("s=%d; e=%d; l=%d\n", s, e, l);
        */
        if (pc >= s && pc <= e) {
          if (s > ostart || e < oend) {
            line = lstBytePtr(dd)[i+0] + 256*lstBytePtr(dd)[i+1];
            ostart = s;
            oend = e;
          }
        }
      }
      if (line > 0) printf("(%d) ", line);
    }
    printf("%s ", nbuf);
    args = aContext->data[lstIVargumentsInContext];
    if (args && args != lstNilObj) {
      printf(" (");
      for (i = 0; i < LST_SIZE(args); i++) {
        lstObject *ao = args->data[i];
        if (ao) {
          if (LST_IS_SMALLINT(ao)) strcpy(nbuf, "SmallInt");
          else lstGetString(nbuf, 256, ao->stclass->data[lstIVnameInClass]);
        }
        printf("%s%s", (i == 0) ? "" : ", ", nbuf);
      }
      printf(")");
    }
    printf("\n");
    aContext = aContext->data[lstIVpreviousContextInContext];
  }
}


static void showHelp (void) {
  printf(
    "LST options:\n"
    "  -h     show this help\n"
    "  -v     show version\n"
    "  -s     set staticSize\n"
    "  -d     set dynamicSize\n"
    "  -g     turn on debugging mode\n"
    "  -G     turn on debugging mode before REPL/file execute\n"
    "  -Z     do not load 'init.st'\n"
    "  -I fn  use this image file\n"
    "  -S     show GC (and other) statistics\n"
  );
}


static char imageFileName[8192];

static int findImageFile (const char *iname, const char *argv0) {
  FILE *fp;
  strcpy(imageFileName, iname);
  if ((fp = fopen(imageFileName, "rb")) == NULL) {
    strcpy(imageFileName, argv0);
#ifdef LST_ON_WINDOWS
    char *p = strrchr(imageFileName, '\\');
    if (p) p[1] = '\0'; else strcpy(imageFileName, "");
#else
    char *p = strrchr(imageFileName, '/');
    if (p) p[1] = '\0'; else strcpy(imageFileName, "./");
#endif
    strcat(imageFileName, iname);
    if ((fp = fopen(imageFileName, "rb")) == NULL) {
      return 0;
    }
  }
  fclose(fp);
  return 1;
}


static int sysQuit = 0;
static int sysQuitCode = 0;
#ifndef LST_ON_WINDOWS
static volatile int alarmSignal = 0;

static void sigHandler (int sigNo) {
  switch (sigNo) {
    case SIGALRM:
      ++alarmSignal;
      break;
    case SIGINT:
      if (++lstExecUserBreak >= 3) {
        fprintf(stderr, "\nFORCED BREAK!\n");
        exit(1);
      }
      lstResetEvtCheckLeft();
      break;
    case SIGTERM:
      if (++lstExecUserBreak >= 2) {
        fprintf(stderr, "\nFORCED TERMINATION!\n");
        exit(1);
      }
      lstResetEvtCheckLeft();
      break;
  }
}


static int isKeyHit (void) {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  if (select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) <= 0) return 0;
  return FD_ISSET(STDIN_FILENO, &fds);
}


static int sigCheck (int *ticksLeft) {
#ifdef LST_USE_IUP
  guiLoopStep();
#else
# ifdef LST_USE_X11
  x11LoopStep();
# endif
#endif
#ifdef LST_USE_IUP
  if (guiHasEvent()) return 1100; /* 1100: GUI event */
#endif
#ifdef LST_USE_X11
  if (x11HasEvent()) return 1200; /* 1200: X11 event */
#endif
  if (isKeyHit()) return 1001; /* 1001: has something in input buffer */
  if (alarmSignal) { alarmSignal = 0; return 1; }
  return 0;
}


LST_PRIMFN(lpNextAlarm) {
  if (LST_PRIMARGC != 1 || !LST_IS_SMALLINT(LST_PRIMARG(0))) return NULL;
  alarm(lstIntValue(LST_PRIMARG(0)));
  return lstTrueObj;
}

#else

static BOOL WINAPI windozeCCHandler (DWORD evt) {
  switch (evt) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
      if (++lstExecUserBreak >= 3) {
        fprintf(stderr, "\nFORCED BREAK!\n");
        exit(1);
      }
      return TRUE;
      break;
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
      /*fprintf(stderr, "\nFORCED DEATH!\n");*/
      ++lstExecUserBreak;
      break;
  }
  return FALSE;
}
#endif


/*
LST_FINALIZER(lpIntHandleFinalizer) {
  int val = lstLIntValue(obj);
  fprintf(stderr, "FINALIZING %d\n", val);
}


LST_PRIMFN(lpNewIntHandle) {
  int val;
  if (LST_PRIMARGC < 1) val = rand();
  else {
    lstObject *o = LST_PRIMARG(0);
    if (LST_IS_SMALLINT(o)) val = lstIntValue(o);
    else if (o == lstIntegerClass) val = lstLIntValue(o);
    else return NULL;
  }
  lstObject *res = lstNewLongInt(val);
  lstSetFinalizer(res, lpIntHandleFinalizer, NULL);
  return res;
}
*/

LST_PRIMFN(lpSystemQuit) {
  int res = 0;
  if (LST_PRIMARGC > 0) {
    lstObject *o = LST_PRIMARG(0);
    if (LST_IS_SMALLINT(o)) res = lstIntValue(o);
    else if (o == lstIntegerClass) res = lstLIntValue(o);
    else res = 42;
  }
#ifdef DEBUG
  fprintf(stderr, "SYSQUIT=%d!\n", res);
  lstBackTrace(lstPrimCtx);
#endif
  sysQuitCode = res;
  sysQuit = 1;
  lstExecUserBreak = 666;
  lstResetEvtCheckLeft();
  return lstTrueObj;
}

static const LSTExtPrimitiveTable primTbl[] = {
/*
{"NewHandle", lpNewIntHandle, NULL},
*/
#ifndef LST_ON_WINDOWS
{"NextAlarm", lpNextAlarm, NULL},
#endif
#ifdef USE_FLTK
{"FLTKAction", lpFLTKAction, NULL},
#endif
{"SystemQuit", lpSystemQuit, NULL},
{0}};


static char stFileName[8192];


static int loadFile (const char *name) {
  lstObject *o = lstNewString(name);
  if (lstRunMethodWithArg(lstLoadMethod, NULL, o, NULL, 0) != lstReturnReturned) return -1;
  return 0;
}


static int doString (const char *str) {
  lstObject *o = lstNewString(str);
  if (lstRunMethodWithArg(lstDoStrMethod, NULL, o, NULL, 0) != lstReturnReturned) return -1;
  return 0;
}


static uint64_t getTicksMS (void) {
#ifndef _WIN32
  uint64_t res;
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  res = ((uint64_t)ts.tv_sec)*1000UL;
  res += ((uint64_t)ts.tv_nsec)/1000000UL; //1000000000
  return res;
#else
  return GetTickCount();
#endif
}


int main (int argc, char **argv) {
  int f, staticSize, dynamicSize, res, apos, wasMinusMinus = 0;
  FILE *fp;
  int ifnChanged = 0, optLoadInitSt = 1, retVal = 0, optShowRunFile = 0;
  int optShowStatistics = 0, optVerboseML = 0, optDebugBeforeRun = 0, optDoBench = 0, optMethodStats = 0;
  uint64_t stTime = 0;
  stFileName[0] = '\0';

  strcpy(imageFileName, DEFAULT_IMAGE_FILE);
  staticSize = DEFAULT_STATIC_SIZE;
  dynamicSize = DEFAULT_DYNAMIC_SIZE;

  /* first parse arguments */
  for (f = 1, apos = 1; f < argc; ++f) {
    if (!wasMinusMinus) {
      if (!strcmp(argv[f], "--")) {
        wasMinusMinus = 1;
        continue;
      } else if (argv[f][0] == '-') {
        int c;
        const char *aa = argv[f];
        for (c = 1; aa[c]; ++c) {
          switch (aa[c]) {
            case 'h': showHelp(); return 1;
            case 'v': printf(LST_VERSION_STRING "\n"); return 1;
            case 's': staticSize = atoi(argv[++f]); break;
            case 'd': dynamicSize = atoi(argv[++f]); break;
            case 'g': lstDebugFlag = 1; break;
            case 'G': optDebugBeforeRun = 1; break;
            case 'Z': optLoadInitSt = 0; break;
            case 'R': optShowRunFile = 1; break;
            case 'S': optShowStatistics = 1; break;
            case 'V': optVerboseML = 1; break;
            case 'B': optDoBench = 1; break;
            case 'M': optMethodStats = 1; break;
            case 'I':
              /*if (ifnChanged) { fprintf(stderr, "ERROR: image file name specified twice\n"); return 1; }*/
              ifnChanged = 1;
              strcpy(imageFileName, argv[++f]);
              break;
            default:
              fprintf(stderr, "ERROR: unknown option: '%c'\n", argv[f][c]);
              return 1;
          }
        }
        continue;
      } else if (!stFileName[0]) {
        strcpy(stFileName, argv[f]);
        wasMinusMinus = 1;
        continue;
      }
    }
    /* pack args */
    if (apos != f) argv[apos++] = argv[f]; else apos++;
  }

  if (staticSize < 1000 || dynamicSize < 1000) {
    fprintf(stderr, "ERROR: invalid memory sizes\n");
    return 1;
  }

  if (!ifnChanged) {
    /* search for default image */
    if (!findImageFile(DEFAULT_WORK_IMAGE_FILE, argv[0])) {
      if (!findImageFile(DEFAULT_IMAGE_FILE, argv[0])) {
        fprintf(stderr, "ERROR: cannot find default image file: %s\n", DEFAULT_IMAGE_FILE);
        return 1;
      }
    }
  }

  lstMemoryInit(staticSize, dynamicSize);
  lstInitPrimitivesStdLib();
  lstInitPrimitivesFiles();
  lstInitPrimitivesExec();
  lstInitPrimitivesFFI();
  lstInitPrimitivesSocket();
  lstInitPrimitivesRegExp();
  /*
  lstInitPrimitivesSQLite();
  */
#ifdef LST_USE_IUP
  lstInitPrimitivesIUP();
#endif
  lstRegisterExtPrimitiveTable(primTbl);
#ifndef LST_ON_WINDOWS
# ifdef LST_USE_X11
  lstInitPrimitivesX11();
# endif
#endif

  /* read image file */
  fp = fopen(imageFileName, "rb");
  if (!fp) {
    fprintf(stderr, "cannot open image file: %s\n", imageFileName);
    return 1;
  }
  if (optShowStatistics && isatty(1)) printf("loading image: %s\n", imageFileName);
  res = lstReadImage(fp);
  fclose(fp);
  if (optShowStatistics && isatty(1)) printf("%d objects in image\n", res);

#ifdef DEBUG
  fprintf(stderr, "calling image init method\n");
#endif
  /* call initialization method */
  res = lstRunMethodWithArg(lstInitMethod, NULL, NULL, NULL, 0);
#ifdef DEBUG
  fprintf(stderr, "image init method complete; res=%d\n", res);
#endif
  if (res != lstReturnReturned) {
    fprintf(stderr, "ERROR: can't initialize system!\n");
    retVal = 2;
    goto quit;
  }

#ifndef LST_ON_WINDOWS
# ifdef LST_USE_X11
  lstInitX11Library();
# endif
#endif

#ifdef DEBUG
  fprintf(stderr, "setting up CLI args\n");
#endif
  /* setup cliArgs array */
  /*FIXME: we'll better allocate this in static memory */
  {
    LST_ENTER_BLOCK();
    LST_NEW_TEMP(cliArgs);
    LST_NEW_TEMP(s);
    cliArgs = lstNewArray(apos);
    if (lstSetGlobal("cliArgs", cliArgs)) {
      fprintf(stderr, "BARF!\n");
      retVal = 2;
      goto quit;
    }
    s = stFileName[0] ? lstNewString(stFileName) : lstNilObj;
    cliArgs->data[0] = s;
    for (f = 1; f < apos; ++f) {
      /*printf("%d: [%s]\n", f, argv[f]);*/
      s = lstNewString(argv[f]);
      cliArgs->data[f] = s;
    }
    LST_LEAVE_BLOCK();
  }
#ifdef DEBUG
  fprintf(stderr, "CLI args set\n");
#endif

  if (!optVerboseML) {
#ifdef DEBUG
    fprintf(stderr, "turning off 'verbose'\n");
#endif
    doString("System verbose: false");
#ifdef DEBUG
    fprintf(stderr, "'verbose' turned off\n");
#endif
  }

#ifndef LST_ON_WINDOWS
  signal(SIGALRM, sigHandler);
  lstEventCheck = sigCheck;
#endif

#ifdef LST_USE_IUP
  guiInit(&argc, &argv);
#endif
  if (optLoadInitSt) {
    static char initName[8192], *home;
    FILE *fl = NULL;
    strcpy(initName, "./init.st");
    if ((fl = fopen(initName, "rb"))) goto doLoadInitSt;
    if ((home = getenv("HOME"))) {
      strcpy(initName, "%s/init.st");
      if ((fl = fopen(initName, "rb"))) goto doLoadInitSt;
    }
    sprintf(initName, "%sinit.st", lstBinDir());
    if ((fl = fopen(initName, "rb"))) goto doLoadInitSt;
doLoadInitSt:
    if (fl) {
      fclose(fl);
      if (optVerboseML && isatty(1)) printf("loading %s\n", initName);
      loadFile(initName);
      if (optVerboseML && isatty(1)) printf("'init.st' loaded\n");
      if (sysQuit) { retVal = sysQuitCode; goto quit; }
    }
  }

#ifndef LST_ON_WINDOWS
  signal(SIGINT, sigHandler);
  signal(SIGTERM, sigHandler);
#else
  SetConsoleCtrlHandler(windozeCCHandler, TRUE);
  SetProcessShutdownParameters(0x280, SHUTDOWN_NORETRY); /* default, but without the box */
#endif

  /*guiModalMessage("IUP Message", "hello from IUP");*/
  if (optDebugBeforeRun) lstDebugFlag = 1;
  if (stFileName[0]) {
    if (optShowRunFile) printf("loading: [%s]\n", stFileName);
    stTime = getTicksMS();
    res = loadFile(stFileName);
    if (res) {
      fprintf(stderr, "ERROR loading %s\n", stFileName);
      retVal = 2;
    }
    if (sysQuit) retVal = sysQuitCode;
  } else {
    stTime = getTicksMS();
/*runAgain:*/
    retVal = 0;
    res = lstRunMethodWithArg(lstReplMethod, NULL, NULL, NULL, 0);
    if (sysQuit) { retVal = sysQuitCode; goto quit; }
    switch (res) {
      case lstReturnError:
      case lstReturnBadMethod:
        retVal = 2;
        break;
      case lstReturnReturned:
        if (optShowStatistics && isatty(1)) printf("normal return\n");
        break;
      case lstReturnTimeExpired:
        /*printf("time out\n");*/
        retVal = 2;
        break;
      case lstReturnBreak:
        /*printf("breakpoint\n");*/
        retVal = 2;
        break;
      case lstReturnAPISuspended:
        lstExecUserBreak = 0;
#if 0
#ifndef LST_ON_WINDOWS
        signal(SIGINT, sigHandler);
#endif
        goto runAgain;
#else
        break;
#endif
      default:
        printf("unknown return code\n");
        break;
    }
  }

quit:
#ifdef LST_USE_IUP
  guiShutdown();
#endif

  lstCompleteFinalizers();

  stTime = getTicksMS()-stTime;
  if (optDoBench && stTime > 0) {
    printf("method call ratio: %d methods per second\n",
      (int)((double)(lstInfoCacheHit+lstInfoCacheMiss)/((double)stTime/1000)));
  }
  if (optShowStatistics && isatty(1)) {
    printf("ret-literal method hits: %u\n", lstInfoLiteralHit);
    printf("ret-ivar method hits: %u\n", lstInfoIVarHit);
    printf("cache hit %u miss %u", lstInfoCacheHit, lstInfoCacheMiss);
#define SCALE (1000)
    while ((lstInfoCacheHit > INT_MAX/SCALE) || (lstInfoCacheMiss > INT_MAX/SCALE)) {
      lstInfoCacheHit /= 10;
      lstInfoCacheMiss /= 10;
    }
    f = (SCALE*lstInfoCacheHit)/(lstInfoCacheHit+lstInfoCacheMiss);
    printf(" ratio %u.%u%%\n", f / 10, f % 10);
    printf("%u garbage collections\n", lstGCCount);
  }
  if (optMethodStats) lstShowCalledMethods();

  lstPrimitivesClear();
  lstMemoryDeinit();

  return retVal;
}
