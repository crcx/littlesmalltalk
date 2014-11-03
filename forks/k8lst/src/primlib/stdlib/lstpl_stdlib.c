/*
 * Primitives of the LittleSmalltalk system
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
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lstcore/k8lst.h"


#ifdef LST_ON_WINDOWS
# include "windows.h" /* for Sleep() */
#else
# include <termios.h>
#endif

#include "lstpl_stdlib.h"


static const char *fileId = "FileHandle";

typedef struct {
  const char *type;
  FILE *fl;
  int isPOpen;
} FileInfo;


LST_FINALIZER(lpFileHandleFinalizer) {
  FileInfo *fi = (FileInfo *)udata;
  if (fi) {
    if (fi->fl) {
      if (fi->isPOpen) pclose(fi->fl); else fclose(fi->fl);
    }
    free(fi);
  }
}


static lstObject *newFileHandle (FILE *fl, int isPOpen) {
  FileInfo *fi;
  lstObject *res = lstNewBinary(NULL, 0);
  fi = malloc(sizeof(FileInfo));
  if (!fi) { fclose(fl); return lstNilObj; }
  fi->type = fileId;
  fi->fl = fl;
  fi->isPOpen = isPOpen;
  lstSetFinalizer(res, lpFileHandleFinalizer, fi);
  return res;
}


static FILE *getFile (lstObject *o) {
  if (!LST_IS_BYTES_EX(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  FileInfo *fi = (FileInfo *)o->fin->udata;
  if (fi->type != fileId) return NULL;
  return fi->fl;
}


LST_PRIMFN(lpFileExists) {
  if (LST_PRIMARGC != 1) return NULL;
  lstObject *o = LST_PRIMARG(0);
  if (!LST_IS_BYTES_EX(o)) return NULL;
  FILE *fp = fopen(lstGetStringPtr(LST_PRIMARG(0)), "rb");
  if (fp == NULL) return lstFalseObj;
  fclose(fp);
  return lstTrueObj;
}


LST_PRIMFN(lpFileOpen) {
  if (LST_PRIMARGC != 2) return NULL;
  if (!LST_IS_BYTES_EX(LST_PRIMARG(0))) return NULL;
  if (!LST_IS_BYTES_EX(LST_PRIMARG(1))) return NULL;
  FILE *fp = fopen(lstGetStringPtr(LST_PRIMARG(0)), lstGetStringPtr(LST_PRIMARG(1)));
  if (fp == NULL) return lstNilObj;
  return newFileHandle(fp, 0);
}


LST_PRIMFN(lpFilePOpen) {
  if (LST_PRIMARGC != 2) return NULL;
  if (!LST_IS_BYTES_EX(LST_PRIMARG(0))) return NULL;
  if (!LST_IS_BYTES_EX(LST_PRIMARG(1))) return NULL;
  FILE *fp = popen(lstGetStringPtr(LST_PRIMARG(0)), lstGetStringPtr(LST_PRIMARG(1)));
  if (fp == NULL) return lstNilObj;
  return newFileHandle(fp, 1);
}


#define TAKE_FHFP \
  FILE *fp = getFile(LST_PRIMARG(0)); \
  if (!fp) return NULL;


LST_PRIMFN(lpFileGetChar) {
  /*fprintf(stderr, "FileGetChar\n");*/
  if (LST_PRIMARGC != 1) return NULL;
  TAKE_FHFP
  /*fprintf(stderr, " FileGetChar (%p)\n", fp);*/
  int ch = fgetc(fp);
  /*fprintf(stderr, "  FileGetChar=%d; %d\n", ch, ch==EOF);*/
  return ch==EOF ? lstNilObj : lstNewInt(ch);
}


LST_PRIMFN(lpFilePutChar) {
  if (LST_PRIMARGC != 2) return NULL;
  TAKE_FHFP
  if (!LST_IS_SMALLINT(LST_PRIMARG(1))) return NULL;
  return fputc(lstIntValue(LST_PRIMARG(1)), fp)==EOF ? lstFalseObj : lstTrueObj;
}


LST_PRIMFN(lpFileClose) {
  if (LST_PRIMARGC != 1) return NULL;
  TAKE_FHFP
  FileInfo *fi = (FileInfo *)((LST_PRIMARG(0))->fin->udata);
  fclose(fp);
  fi->fl = NULL;
  return lstTrueObj;
}


LST_PRIMFN(lpFileWriteImage) {
  int noSrc = 0;
  if (LST_PRIMARGC != 2) return NULL;
  TAKE_FHFP
  if (LST_IS_SMALLINT(LST_PRIMARG(1))) noSrc = lstIntValue(LST_PRIMARG(1));
  else if (LST_PRIMARG(1) == lstTrueObj) noSrc = 1;
  return lstNewInt(lstWriteImage(fp, noSrc));
}


LST_PRIMFN(lpFileReadByteArray) {
  if (LST_PRIMARGC != 3) return NULL;
  TAKE_FHFP
  /* make sure we're populating an array of bytes */
  lstObject *res = LST_PRIMARG(1);
  if (!LST_IS_BYTES_EX(res)) return NULL;
  /* sanity check on I/O count */
  if (!LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
  int len = lstIntValue(LST_PRIMARG(2));
  if (len < 0 || len > LST_SIZE(res)) return NULL;
  /* do the I/O */
  if (len > 0) len = fread(lstBytePtr(res), 1, len, fp);
  if (len < 0) return lstNilObj;
  return lstNewInt(len);
}


LST_PRIMFN(lpFileWriteByteArray) {
  if (LST_PRIMARGC != 3) return NULL;
  TAKE_FHFP
  /* make sure we're writing an array of bytes */
  lstObject *res = LST_PRIMARG(1);
  if (!LST_IS_BYTES_EX(res)) return NULL;
  /* sanity check on I/O count */
  if (!LST_IS_SMALLINT(LST_PRIMARG(2))) return NULL;
  int len = lstIntValue(LST_PRIMARG(2));
  if (len < 0 || len > LST_SIZE(res)) return NULL;
  /* do the I/O */
  if (len > 0) len = fwrite(lstBytePtr(res), 1, len, fp);
  if (len < 0) return lstNilObj;
  return lstNewInt(len);
}


LST_PRIMFN(lpFileSeek) {
  long ofs;
  if (LST_PRIMARGC != 2) return NULL;
  TAKE_FHFP
  /* file position */
  if (LST_IS_SMALLINT(LST_PRIMARG(1))) ofs = lstIntValue(LST_PRIMARG(1));
  else if (LST_PRIMARG(1)->stclass == lstIntegerClass) ofs = lstLIntValue(LST_PRIMARG(1));
  else return NULL;
  if (ofs < 0 || fseek(fp, ofs, SEEK_SET)) return NULL;
  /* return position as our value */
  return LST_PRIMARG(1);
}


LST_PRIMFN(lpFileSize) {
  long size, opos;
  if (LST_PRIMARGC != 1) return NULL;
  TAKE_FHFP
  if ((opos = ftell(fp)) < 0) return NULL;
  if (fseek(fp, 0, SEEK_END) < 0) return NULL;
  size = ftell(fp);
  fseek(fp, opos, SEEK_SET);
  if (size < 0) return NULL;
  return lstNewInteger(size);
}


LST_PRIMFN(lpFileGetFD) {
  if (LST_PRIMARGC != 1) return NULL;
  TAKE_FHFP
  return lstNewInt(fileno(fp));
}


LST_PRIMFN(lpFileReadLine) {
  static char buf[8192]; /*FIXME*/
  if (LST_PRIMARGC != 1) return NULL;
  TAKE_FHFP
  if (!fgets(buf, sizeof(buf), fp)) return lstNilObj;
  int len = strlen(buf)-1;
  if (len >= 0 && buf[len] == '\n') {
    if (len > 0 && buf[len-1] == '\r') buf[len-1] = '\0'; else buf[len] = '\0';
  }
  return lstNewString(buf);
}


LST_PRIMFN(lpSystemGetOSName) {
  if (LST_PRIMARGC != 0) return NULL;
  return lstNewString(lstOSName);
}


LST_PRIMFN(lpSystemIsATTY) {
  if (LST_PRIMARGC != 1) return NULL;
  return isatty(lstIntValue(LST_PRIMARG(0))) ? lstTrueObj : lstFalseObj;
}


LST_PRIMFN(lpSystemSleep) {
  lstObject *o;
  int pauseMs = 0;
  if (LST_PRIMARGC != 1) return NULL;
  o = LST_PRIMARG(0);
  if (LST_IS_SMALLINT(o)) pauseMs = lstIntValue(o)*1000;
  else if (o->stclass == lstIntegerClass) pauseMs = lstLIntValue(o)*1000;
  else if (o->stclass == lstFloatClass) {
    LstFloat fv = lstFloatValue(o);
    pauseMs = (int)(fv*1000.0);
  } else pauseMs = -1;
  if (pauseMs < 0) return lstNilObj;
#ifndef _WIN32
  if (pauseMs%1000) {
    //2000000 == 2 seconds == 2000 milliseconds
    int sec = pauseMs/1000;
    if (sec > 0) sleep(sec);
    usleep((pauseMs%1000)*1000);
  } else {
    sleep(pauseMs/1000);
  }
#else
  Sleep(pauseMs);
#endif
  return lstNilObj;
}


LST_PRIMFN(lpSystemUnixTime) {
  time_t t = time(NULL);
  return LST_64FITS_SMALLINT(t) ? lstNewInt(t) : lstNewLongInt(t);
}


LST_PRIMFN(lpSystemLocalTime) {
  time_t t;
  if (LST_PRIMARGC > 0) {
    lstObject *o = LST_PRIMARG(0);
    if (LST_IS_SMALLINT(o)) t = lstIntValue(o);
    else if (o->stclass == lstIntegerClass) t = lstLIntValue(o);
    else if (o->stclass == lstFloatClass) {
      LstFloat fv = lstFloatValue(o);
      t = (time_t)(fv);
    } else return NULL;
  } else {
    t = time(NULL);
  }
  struct tm *lt = localtime(&t);
  lstObject *res = lstNewArray(9);
  res->data[0] = lstNewInt(lt->tm_sec);
  res->data[1] = lstNewInt(lt->tm_min);
  res->data[2] = lstNewInt(lt->tm_hour);
  res->data[3] = lstNewInt(lt->tm_mday);
  res->data[4] = lstNewInt(lt->tm_mon+1); /* convert to 1..12 */
  res->data[5] = lstNewInt(lt->tm_year+1900);
  res->data[6] = lstNewInt(lt->tm_wday); /* 0: sunday */
  res->data[7] = lstNewInt(lt->tm_yday);
  res->data[8] = lt->tm_isdst ? lstTrueObj : lstFalseObj;
  return res;
}


static char lstMyPath[8192];
static void initMyPath (void) {
#ifndef _WIN32
  char buf[128];
  pid_t pid = getpid();
  sprintf(buf, "/proc/%u/exe", (unsigned int)pid);
  if (readlink(buf, lstMyPath, sizeof(lstMyPath)-1) < 0) strcpy(lstMyPath, "./");
  else {
    char *p = (char *)strrchr(lstMyPath, '/');
    if (!p) strcpy(lstMyPath, "./"); else p[1] = '\0';
  }
#else
  char *p;
  memset(lstMyPath, 0, sizeof(lstMyPath));
  GetModuleFileName(GetModuleHandle(NULL), lstMyPath, sizeof(lstMyPath)-1);
  p = strrchr(lstMyPath, '\\');
  if (!p) strcpy(lstMyPath, ".\\"); else p[1] = '\0';
#endif
}


LST_PRIMFN(lpSystemBinPath) {
  return lstNewString(lstMyPath);
}


LST_PRIMFN(lpStringToURLEncoding) {
  if (LST_PRIMARGC != 1) return NULL;
  unsigned char *res, *src; int dp, slen;
  lstObject *s = LST_PRIMARG(0);
  if (LST_IS_SMALLINT(s) || s->stclass != lstStringClass) return NULL;
  slen = LST_SIZE(s);
  res = malloc(slen*3);
  src = (unsigned char *)lstBytePtr(s);
  for (dp = 0; slen > 0; src++, slen--) {
    unsigned char ch = *src;
    if (isalnum(ch)) {
      res[dp++] = ch;
    } else {
      char buf[5];
      sprintf(buf, "%%%02X", ch);
      memcpy(res+dp, buf, 3);
      dp += 3;
    }
  }
  /*FIXME: memory leak on fail*/
  s = (lstObject *)lstMemAllocBin(dp);
  s->stclass = lstStringClass;
  if (dp > 0) memcpy(lstBytePtr(s), res, dp);
  free(res);
  return s;
}


LST_PRIMFN(lpStringFromURLEncoding) {
  if (LST_PRIMARGC < 1) return NULL;
  unsigned char *res, *src; int dp, slen;
  lstObject *s = LST_PRIMARG(0);
  if (LST_IS_SMALLINT(s) || s->stclass != lstStringClass) return NULL;
  slen = LST_SIZE(s);
  res = malloc(slen);
  src = (unsigned char *)lstBytePtr(s);
  int doPlus = LST_PRIMARGC > 1;
  for (dp = 0; slen > 0; src++, slen--) {
    unsigned char ch = *src;
    if (ch == '%' && slen >= 3 && isxdigit(src[1]) && isxdigit(src[2])) {
      int n = toupper(src[1])-'0'; if (n > 9) n -= 7;
      ch = n*16;
      n = toupper(src[2])-'0'; if (n > 9) n -= 7;
      ch += n;
      res[dp++] = ch;
      slen -= 2;
      src += 2;
    } else if (doPlus && ch == '+') {
      res[dp++] = ' ';
    } else {
      res[dp++] = ch;
    }
  }
  /*FIXME: memory leak on fail*/
  s = (lstObject *)lstMemAllocBin(dp);
  s->stclass = lstStringClass;
  if (dp > 0) memcpy(lstBytePtr(s), res, dp);
  free(res);
  return s;
}


/*
LST_PRIMFN(lpStringOpsExt) {
  int action;
  if (LST_PRIMARGC != 2) return NULL;
  lstObject *op = LST_PRIMARG(0);
  if (!LST_IS_SMALLINT(op)) return NULL;
  action = lstIntValue(op);
  lstObject *op = LST_PRIMARG(1);
  if (LST_CLASS(op) != lstStringClass) return NULL;
  switch (action) {
    case 0: return lpStringToURLEncoding(op);
    case 1: return lpStringFromURLEncoding(op);
  }
  return NULL;
}
*/


LST_PRIMFN(lpSystemSystem) {
  if (LST_PRIMARGC != 1) return NULL;
  lstObject *o = LST_PRIMARG(0);
  if (!LST_IS_BYTES_EX(o)) return NULL;
  int size = LST_SIZE(o);
  if (size < 1) return lstNilObj;
  int res = system(lstGetStringPtr(LST_PRIMARG(0)));
  if (res < 0) return lstNilObj;
  return lstNewInteger(res);
}


LST_PRIMFN(lpSystemTickCountMS) {
  int64_t res;
  if (LST_PRIMARGC != 0) return NULL;
#ifndef _WIN32
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  res = ((uint64_t)ts.tv_sec)*1000UL;
  res += ((uint64_t)ts.tv_nsec)/1000000UL; //1000000000
#else
  res = GetTickCount();
#endif
  return lstNewInteger(res);
}


#ifndef _WIN32
static int termInitialized = 0;
static struct termios origTIOS;
static int inRawMode = 0;


static void disableRawMode (void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTIOS);
  inRawMode = 0;
}


static void termCleanAtExit (void) {
  disableRawMode();
}


static int enableRawMode (void) {
  struct termios raw;
  if (!isatty(STDIN_FILENO)) goto fatal;
  atexit(termCleanAtExit);
  if (tcgetattr(STDIN_FILENO, &origTIOS) == -1) goto fatal;
  raw = origTIOS; /* modify the original mode */
  /* input modes: no break, no CR to NL, no parity check, no strip char, no start/stop output control */
  /*raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);*/
  /* input modes: no break, no parity check, no strip char, no start/stop output control */
  raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
  /* output modes - disable post processing */
  raw.c_oflag &= ~(OPOST);
  raw.c_oflag |= ONLCR;
  raw.c_oflag = OPOST | ONLCR;
  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);
  /* local modes - choing off, canonical off, no extended functions, no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* control chars - set return condition: min number of bytes and timer;
   * we want read to return every single byte, without timeout */
  raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 byte, no timer */
  /* put terminal in raw mode after flushing */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) goto fatal;
  inRawMode = 1;
  return 0;
fatal:
  /*errno = ENOTTY;*/
  return -1;
}


static inline void setRawMode (int rmFlag) {
  if (rmFlag) enableRawMode(); else disableRawMode();
}


static int isKeyHit (void) {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}


static void setCanonical (int cnFlag) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  if (!cnFlag) {
    ttystate.c_lflag &= ~ICANON; /* turn off canonical mode */
    ttystate.c_cc[VMIN] = 1; /* minimum of number input read */
  } else {
    ttystate.c_lflag |= ICANON; /* turn on canonical mode */
  }
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ttystate);
}


static void setEcho (int cnFlag) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  if (!cnFlag) {
    ttystate.c_lflag &= ~ECHO;
  } else {
    ttystate.c_lflag |= ECHO;
  }
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ttystate);
}


static int isCanonical (void) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  return ttystate.c_lflag&ICANON ? 1 : 0;
}


static int isEcho (void) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  return ttystate.c_lflag&ECHO ? 1 : 0;
}


LST_PRIMFN(lpSystemIsKeyHit) {
  if (LST_PRIMARGC != 0) return NULL;
  return isKeyHit() ? lstTrueObj : lstFalseObj;
}


LST_PRIMFN(lpSystemTermMode) {
  lstObject *o;
  int oldv, newv = -1;
  if (LST_PRIMARGC < 1) return NULL;
  o = LST_PRIMARG(0);
  if (!LST_IS_SMALLINT(o)) return NULL;
  if (LST_PRIMARGC > 1) {
    lstObject *o = LST_PRIMARG(1);
    if (o == lstNilObj || o == lstFalseObj) newv = 0;
    else if (o == lstTrueObj) newv = 1;
    else if (LST_IS_SMALLINT(o)) newv = lstIntValue(o) ? 1 : 0;
    else return NULL;
  }
  switch (lstIntValue(o)) {
    case 0: /* canonical */
      oldv = isCanonical();
      if (newv != -1) setCanonical(newv);
      break;
    case 1: /* echo */
      oldv = isEcho();
      if (newv != -1) setEcho(newv);
      break;
    case 2: /* raw */
      oldv = inRawMode;
      if (newv != -1) setRawMode(newv);
      break;
    default: return NULL;
  }
  return oldv ? lstTrueObj : lstFalseObj;
}


LST_PRIMCLEARFN(lpSystemKeyClean) {
  disableRawMode();
}
#endif


static const LSTExtPrimitiveTable primFilesTbl[] = {
/*TODO: turn this into one primitive with numeric action */
{"FileExists", lpFileExists, NULL},
{"FileOpen", lpFileOpen, NULL},
{"FilePOpen", lpFilePOpen, NULL},
{"FileGetChar", lpFileGetChar, NULL},
{"FilePutChar", lpFilePutChar, NULL},
{"FileClose", lpFileClose, NULL},
{"FileWriteImage", lpFileWriteImage, NULL},
{"FileReadByteArray", lpFileReadByteArray, NULL},
{"FileWriteByteArray", lpFileWriteByteArray, NULL},
{"FileSeek", lpFileSeek, NULL},
{"FileSize", lpFileSize, NULL},
{"FileGetFD", lpFileGetFD, NULL},
{"FileReadLine", lpFileReadLine, NULL},
{0}};

static const LSTExtPrimitiveTable primTbl[] = {
{"SystemGetOSName", lpSystemGetOSName, NULL},
{"SystemIsATTY", lpSystemIsATTY, NULL},
{"SystemSleep", lpSystemSleep, NULL},
{"SystemUnixTime", lpSystemUnixTime, NULL},
{"SystemLocalTime", lpSystemLocalTime, NULL},
{"SystemTickCountMS", lpSystemTickCountMS, NULL},
/* */
{"SystemBinPath", lpSystemBinPath, NULL},
#ifndef _WIN32
{"SystemIsKeyHit", lpSystemIsKeyHit, NULL},
{"SystemTermMode", lpSystemTermMode, lpSystemKeyClean},
#endif
/* */
{"StringFromURLEncoding", lpStringFromURLEncoding, NULL},
{"StringToURLEncoding", lpStringToURLEncoding, NULL},
{0}};

static const LSTExtPrimitiveTable primExecTbl[] = {
{"SystemSystem", lpSystemSystem, NULL},
{0}};


const char *lstBinDir (void) {
  return lstMyPath;
}


void lstInitPrimitivesStdLib (void) {
  initMyPath();
#ifndef _WIN32
  tcgetattr(STDIN_FILENO, &origTIOS);
  if (!termInitialized) {
    termInitialized = 1;
    atexit(termCleanAtExit);
  }
#endif
  lstRegisterExtPrimitiveTable(primTbl);
}


void lstInitPrimitivesFiles (void) {
  lstRegisterExtPrimitiveTable(primFilesTbl);
}


void lstInitPrimitivesExec (void) {
  lstRegisterExtPrimitiveTable(primExecTbl);
}
