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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include "lstcore/k8lst.h"
#include "lstpl_x11.h"


#ifdef DEBUG
# define dprintf(...)  fprintf(stderr, __VA_ARGS__)
#else
# define dprintf(...)
#endif


/*#define DEBUG_EVENTS*/


static const char *x11ObjId = "X11Handle";
static const char *x11SubId[] = {
  "Window",
  "GC",
  "XftFont",
  "XftDraw",
  "XftColor",
  0
};


enum {
  X11_WINDOW,
  X11_GC,
  X11_XFTFONT,
  X11_XFTDRAW,
  X11_XFTCOLOR,
  X11_MAXSUB
};


static int initialized = 0;
static Display *curDisp = NULL;
static Colormap curCMap;
/*
static unsigned long blackPixel;
static unsigned long whitePixel;
*/
static int screen;
static int rootWin;
static Visual *vis = NULL;
/*static Window intrWin;*/
static Atom wmDeleteWindow;


#define MAX_QUEUE 128
static XEvent eQueue[MAX_QUEUE];
static int eqHead = 0;
static int eqTail = 0;
static int eqCount = 0;

static int aliveObjects = 0;


static void deinitializeX11Subsystem (void) {
  if (initialized) {
    if (aliveObjects == 0) {
      /*XFreeColormap(curDisp, curCMap);*/
      /*XDestroyWindow(curDisp, intrWin);*/
      XCloseDisplay(curDisp);
      curDisp = NULL;
      eqHead = eqTail = eqCount = 0;
    }
    initialized = 0;
  }
}


static int initializeX11Subsystem (const char *dispName) {
  if (initialized) {
    if (aliveObjects != 0) return -1;
    deinitializeX11Subsystem();
  }
  if (!(curDisp = XOpenDisplay(dispName))) return -1;
  screen = DefaultScreen(curDisp);
  rootWin = DefaultRootWindow(curDisp);
  /*
  blackPixel = BlackPixel(curDisp, screen);
  whitePixel = WhitePixel(curDisp, screen);
  */
  vis = DefaultVisual(curDisp, screen);
  curCMap = DefaultColormap(curDisp, screen);
  /*intrWin = XCreateSimpleWindow(curDisp, rootWin, 0, 0, 1, 1, 0, blackPixel, blackPixel);*/
  /*curCMap = XCreateColormap(curDisp, intrWin, vis, AllocNone);*/
  eqHead = eqTail = eqCount = 0;
  wmDeleteWindow = XInternAtom(curDisp, "WM_DELETE_WINDOW", False);
  initialized = 1;
  return 0;
}


typedef struct {
  const char *type;
  int subtype;
  int inited;
  int dontKill;
  union {
    Window win;
    GC gc;
    XftFont *font;
    XftDraw *xd;
    XftColor tc;
  };
} X11Object;


static void deinitX11Obj (X11Object *x11o) {
  if (x11o) {
    if (x11o->inited) {
      if (!x11o->dontKill) {
        /*dprintf("deinitializing X11 object; subtype=%s\n", x11SubId[x11o->subtype]);*/
        /*fprintf(stderr, "deinitializing X11 object; subtype=%s\n", x11SubId[x11o->subtype]);*/
        switch (x11o->subtype) {
          case X11_WINDOW: XDestroyWindow(curDisp, x11o->win); break;
          case X11_GC: XFreeGC(curDisp, x11o->gc); break;
          case X11_XFTFONT: XftFontClose(curDisp, x11o->font); break;
          case X11_XFTDRAW: XftDrawDestroy(x11o->xd); break;
          case X11_XFTCOLOR: XftColorFree(curDisp, vis, curCMap, &x11o->tc);
        }
      }
      if (--aliveObjects <= 0) {
        aliveObjects = 0;
        if (!initialized) {
          dprintf("closing X11\n");
          XCloseDisplay(curDisp); curDisp = NULL;
        }
      } else {
        dprintf("X11: %d objects left\n", aliveObjects);
      }
      x11o->inited = 0;
    }
  }
}


LST_FINALIZER(lpX11Finalizer) {
  X11Object *x11o = (X11Object *)udata;
  if (x11o) {
    deinitX11Obj(x11o);
    free(x11o);
  }
}


static lstObject *newX11 (X11Object **x11o, int subtype) {
  if (x11o) *x11o = NULL;
  if (subtype < 0 || subtype >= X11_MAXSUB) return lstNewString("internal error");
  lstObject *res = lstNewBinary(NULL, 0);
  X11Object *xo = calloc(1, sizeof(X11Object));
  if (!xo) return lstNewString("out of memory");
  xo->type = x11ObjId;
  xo->subtype = subtype;
  xo->dontKill = 0;
  xo->inited = 1; /* MUST! be initialized by caller */
  lstSetFinalizer(res, lpX11Finalizer, xo);
  ++aliveObjects;
  if (x11o) *x11o = xo;
  return res;
}


static X11Object *getX11 (lstObject *o) {
  if (!LST_IS_BYTES_EX(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  X11Object *x11o = (X11Object *)o->fin->udata;
  if (x11o->type != x11ObjId) return NULL;
  return x11o;
}


static X11Object *getX11As (lstObject *o, int subtype) {
  X11Object *xo = getX11(o);
  if (!xo || !xo->inited || xo->subtype != subtype) return NULL;
  return xo;
}


static int getColor (lstObject *o, unsigned short *c) {
  int cc;
  if (LST_IS_SMALLINT(o)) {
    cc = lstIntValue(o);
    if (cc < 0) cc = 0; else if (cc > 65535) cc = 65535;
  } else if (o->stclass == lstIntegerClass) {
    LstLInt n = lstLIntValue(o);
    if (n < 0) cc = 0; else if (n > 65535) cc = 65535; else cc = n;
  } else if (o->stclass == lstFloatClass) {
    LstFloat n = lstFloatValue(o);
    if (n < 0.0) cc = 0; else if (n >= 1.0) cc = 65535; else cc = (int)(n*65535);
  } else return -1;
  *c = cc;
  return 0;
}


#define GETINT(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (LST_IS_SMALLINT(op)) (dest) = lstIntValue(op);\
  else if (op->stclass == lstIntegerClass) (dest) = (int)lstLIntValue(op);\
  else if (op->stclass == lstFloatClass) (dest) = (int)lstFloatValue(op);\
  else return NULL;\
}

#define GETSTR(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (!LST_IS_BYTES_EX(op)) return NULL;\
  (dest) = lstGetStringPtr(op);\
}

#define GETWIN(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (op == lstNilObj || op == lstFalseObj) (dest) = rootWin;\
  else {\
    if (!(xi = getX11As(op, X11_WINDOW))) return NULL;\
    (dest) = xi->win;\
  }\
}

#define GETGC(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (!(xi = getX11As(op, X11_GC))) return NULL;\
  (dest) = xi->gc;\
}

#define GETFONT(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (!(xi = getX11As(op, X11_XFTFONT))) return NULL;\
  (dest) = xi->font;\
}

#define GETDRAW(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (!(xi = getX11As(op, X11_XFTDRAW))) return NULL;\
  (dest) = xi->xd;\
}

#define GETTC(dest, argn) {\
  op = LST_PRIMARG(argn);\
  if (!(xi = getX11As(op, X11_XFTCOLOR))) return NULL;\
  (dest) = xi->tc;\
}


LST_PRIMFN(lpX11Do) {
  if (LST_PRIMARGC < 1 || !LST_IS_SMALLINT(LST_PRIMARG(0))) return NULL;
  int action = lstIntValue(LST_PRIMARG(0)), tmp, act;
  lstObject *res = lstNilObj, *op;
  const char *str = NULL;
  static char buf[128];
  X11Object *xi;
  Window win, parent;
  GC gc;
  int x, y, w, h, bc;
  int x0, y0, x1, y1;
  unsigned int wdt, hgt, border, depth;
  unsigned long upix;
  XColor cc;
  XftDraw *xd;
  XftFont *font;
  XRenderColor rcc;
  XftColor tc;
  XGlyphInfo info;
  LstLInt ll0;
  XWMHints *wmh;

  /*fprintf(stderr, "X11Do: %d\n", action);*/
  switch (action) {
    case 0: /* get subtype name; return string or nil */
      if (LST_PRIMARGC < 1) return NULL;
      if ((xi = getX11(LST_PRIMARG(1))) && xi->inited) res = lstNewString(x11SubId[xi->subtype]);
      break;
    case 1: /* wid2str */
      if (LST_PRIMARGC < 2) return NULL;
      GETWIN(win, 1);
      sprintf(buf, "%p", (void *)win);
      res = lstNewString(buf);
      break;
    case 2: /* SameWID w0 w1 */
      if (LST_PRIMARGC < 3) return NULL;
      GETWIN(win, 1);
      GETWIN(parent, 2);
      res = win==parent ? lstTrueObj : lstFalseObj;
      break;
    case 5: /* info type */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      GETINT(tmp, 1);
      switch (tmp) {
        case 0: /* black pixel */
          ll0 = XBlackPixel(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        case 1: /* white pixel */
          ll0 = XWhitePixel(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        case 2: /* all planes */
          ll0 = XAllPlanes();
          res = lstNewInteger(ll0);
          break;
        case 3: /* connection number */
          ll0 = XConnectionNumber(curDisp);
          res = lstNewInteger(ll0);
          break;
        case 4: /* depth */
          ll0 = XDefaultDepth(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        case 5: /* planes */
          ll0 = XDisplayPlanes(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        case 6: /* display name */
          res = lstNewString(XDisplayString(curDisp));
          break;
        case 7: /* vendor name */
          res = lstNewString(XServerVendor(curDisp));
          break;
        case 8: /* release */
          ll0 = XVendorRelease(curDisp);
          res = lstNewInteger(ll0);
          break;
        case 9: /* width */
          ll0 = XDisplayHeight(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        case 10: /* height */
          ll0 = XDisplayWidth(curDisp, screen);
          res = lstNewInteger(ll0);
          break;
        default: return NULL;
      }
      break;

    case 8: /* flush */
      if (curDisp != NULL) XFlush(curDisp);
      break;

    case 10: /* init [dispname]; return true or false */
      if (LST_PRIMARGC > 1) {
        op = LST_PRIMARG(1);
        if (op != lstNilObj) {
          if (!LST_IS_BYTES_EX(op)) return NULL;
          str = lstGetStringPtr(op);
        }
      }
      res = initializeX11Subsystem(str) ? lstFalseObj : lstTrueObj;
      break;
    case 11: /* deinit */
      deinitializeX11Subsystem();
      break;

    case 14: /* GetEvent */
      if (!initialized) return NULL;
      x11LoopStep();
      if (eqCount > 0) res = x11GetEvent();
      break;

    case 15: /* XSelectInput window interestFlag (no other args for now) */
      /* interestFlag is not nil, not false, not true: do not get motion events */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETWIN(win, 1);
      if (win == rootWin) return NULL;
      op = LST_PRIMARG(2);
      XSelectInput(curDisp, win, op==lstNilObj||op==lstFalseObj ? 0 :
        ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
        ExposureMask | FocusChangeMask | KeymapStateMask | KeyPressMask | KeyReleaseMask |
        StructureNotifyMask | /*SubstructureNotifyMask |*/ VisibilityChangeMask |
        (op==lstTrueObj ? ButtonMotionMask | PointerMotionMask : 0));
      break;

    case 20: /* XCreateSimpleWindow parent x y wdt hgt bgcolor */
      if (LST_PRIMARGC < 7 || !initialized) return NULL;
      GETWIN(parent, 1);
      GETINT(x, 2);
      GETINT(y, 3);
      GETINT(w, 4);
      GETINT(h, 5);
      GETINT(bc, 6);
      win = XCreateSimpleWindow(curDisp, parent, x, y, w, h, 0, bc, bc);
      if (win <= LastExtensionError) return NULL;
      XSetWMProtocols(curDisp, win, &wmDeleteWindow, 1);
      res = newX11(&xi, X11_WINDOW);
      if (xi) {
        xi->win = win;
        if ((wmh = XGetWMHints(curDisp, win))) {
          wmh->flags = InputHint;
          wmh->input = False;
          XSetWMHints(curDisp, win, wmh);
          XFree(wmh);
        }
      } else XDestroyWindow(curDisp, win);
      break;
    case 21: /* XDestroyWindow win */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      if (!(xi = getX11As(LST_PRIMARG(1), X11_WINDOW))) return NULL;
      /*deinitX11Obj(xi);*/
      if (xi->inited && !xi->dontKill) {
        XDestroyWindow(curDisp, xi->win);
        xi->dontKill = 1;
      }
      break;
    case 22: /* XStoreName win name */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETWIN(win, 1);
      if (LST_PRIMARGC > 2) {
        op = LST_PRIMARG(2);
        if (op != lstNilObj) {
          if (!LST_IS_BYTES_EX(op)) return NULL;
          str = lstGetStringPtr(op);
        }
      }
      XStoreName(curDisp, win, str?str:"");
      break;
    case 23: /* map/unmap window: win mapflag */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETWIN(win, 1);
      op = LST_PRIMARG(2);
      if (op == lstNilObj || op == lstFalseObj) XUnmapWindow(curDisp, win);
      else XMapWindow(curDisp, win);
      break;
    case 24: /* XGetGeometry wid type */
      if (LST_PRIMARGC < 3) return NULL;
      GETWIN(win, 1);
      GETINT(act, 2);
      if (XGetGeometry(curDisp, win, &parent, &x, &y, &wdt, &hgt, &border, &depth) == BadDrawable) return NULL;
      switch (act) {
        case 0: {
          LST_ENTER_BLOCK();
          LST_NEW_TEMP(nfo);
          nfo = lstNewArray(6);
          nfo->data[0] = lstNewInt((int)x);
          nfo->data[1] = lstNewInt((int)y);
          nfo->data[2] = lstNewInt((int)wdt);
          nfo->data[3] = lstNewInt((int)hgt);
          nfo->data[4] = lstNewInt((int)border);
          nfo->data[5] = lstNewInt((int)depth);
          LST_LEAVE_BLOCK();
          res = nfo;
          } break;
        case 1: res = lstNewInteger((int)x); break;
        case 2: res = lstNewInteger((int)y); break;
        case 3: res = lstNewInteger((int)wdt); break;
        case 4: res = lstNewInteger((int)hgt); break;
        case 5: res = lstNewInteger((int)border); break;
        case 6: res = lstNewInteger((int)depth); break;
        default: return NULL;
      }
      break;
    case 25: /* XSetWindowBackground win color */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETWIN(win, 1);
      GETINT(tmp, 2);
      upix = tmp;
      XSetWindowBackground(curDisp, win, upix);
      break;
    case 26: /* XClearWindow wid */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      GETWIN(win, 1);
      XClearWindow(curDisp, win);
      break;
    case 27: /* window movement: type wid nx ny [nw nh] */
      if (LST_PRIMARGC < 5 || !initialized) return NULL;
      GETWIN(win, 1);
      GETINT(tmp, 2);
      GETINT(x, 3);
      GETINT(y, 4);
      switch (tmp) {
        case 1: /* move */
          XMoveWindow(curDisp, win, x, y);
          break;
        case 2: /* resize */
          XResizeWindow(curDisp, win, x, y);
          break;
        case 3: /* both */
          if (LST_PRIMARGC < 7) return NULL;
          GETINT(w, 5);
          GETINT(h, 6);
          XMoveResizeWindow(curDisp, win, x, y, w, h);
          break;
        default: return NULL;
      }
      break;
    case 28: /* InputFocus win[true: query] */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      op = LST_PRIMARG(1);
      if (op == lstNilObj || op == lstFalseObj) win = None;
      else if (op == lstTrueObj) {
        if (XGetInputFocus(curDisp, &win, &tmp)) break;
        if (win != None && win != PointerRoot) {
          res = newX11(&xi, X11_WINDOW);
          xi->dontKill = 1;
          xi->win = win;
        }
      } else {
        if (!(xi = getX11As(op, X11_WINDOW))) return NULL;
        win = xi->win;
        /*fprintf(stderr, "SETTING FOCUS TO: %p\n", (void *)win);*/
        /*XFlush(curDisp);*/
        XSetInputFocus(curDisp, win, RevertToParent, CurrentTime);
      }
      break;

    case 30: /* XCreateGC window [fgcolor] */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      GETWIN(win, 1);
      if (LST_PRIMARGC > 2) {
        GETINT(tmp, 2);
      } else {
        tmp = XWhitePixel(curDisp, screen);
      }
      gc = XCreateGC(curDisp, win, 0, NULL);
      XSetForeground(curDisp, gc, tmp);
      res = newX11(&xi, X11_GC);
      if (xi) xi->gc = gc; else XFreeGC(curDisp, gc);
      break;
    case 31: /* XFreeGC gc */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      if (!(xi = getX11As(LST_PRIMARG(1), X11_GC))) return NULL;
      deinitX11Obj(xi);
      break;
    case 32: /* XNewColor r g b */
      if (LST_PRIMARGC < 4 || !initialized) return NULL;
      if (getColor(LST_PRIMARG(1), &cc.red)) return NULL;
      if (getColor(LST_PRIMARG(2), &cc.green)) return NULL;
      if (getColor(LST_PRIMARG(3), &cc.blue)) return NULL;
      Status rc = XAllocColor(curDisp, curCMap, &cc);
      if (rc) {
        ++aliveObjects;
        tmp = cc.pixel;
        res = lstNewInteger(tmp);
      }
      break;
    case 33: /* XFreeColor int */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      GETINT(tmp, 1);
      upix = tmp;
      tmp = XFreeColors(curDisp, curCMap, &upix, 1, 0);
      if (tmp == 1) {
        if (--aliveObjects <= 0 && !initialized) {
          XCloseDisplay(curDisp); curDisp = NULL;
        }
      }
      break;
    case 34: /* XSetColor gc color [backflag] */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETGC(gc, 1);
      GETINT(tmp, 2);
      if (LST_PRIMARGC > 3) op = LST_PRIMARG(3); else op = lstNilObj;
      if (op != lstFalseObj && op != lstNilObj) XSetBackground(curDisp, gc, tmp);
      else XSetForeground(curDisp, gc, tmp);
      break;
    case 35: /* XSetLineAttr gc width line cap join */
      if (LST_PRIMARGC < 6 || !initialized) return NULL;
      GETGC(gc, 1);
      GETINT(w, 2);
      GETINT(tmp, 3);
      GETINT(x, 4);
      GETINT(y, 5);
      if (w < 0) w = 0;
      switch (tmp) {
        case 1: tmp = LineDoubleDash; break;
        case 2: tmp = LineOnOffDash; break;
        default: tmp = LineSolid; break;
      }
      switch (x) {
        case 0: x = CapNotLast; break;
        /*case 1: x = CapButt; break;*/
        case 2: x = CapRound; break;
        case 3: x = CapProjecting; break;
        default: x = CapButt; break;
      }
      switch (y) {
        case 1: y = JoinRound; break;
        case 2: y = JoinBevel; break;
        default: y = JoinMiter; break;
      }
      XSetLineAttributes(curDisp, gc, w, tmp, x, y);
      break;

    case 40: /* XDrawPoint wid gc x y */
      if (LST_PRIMARGC < 5 || !initialized) return NULL;
      GETWIN(win, 1);
      GETGC(gc, 2);
      GETINT(x, 3);
      GETINT(y, 4);
      XDrawPoint(curDisp, win, gc, x, y);
      break;
    case 41: /* XDrawLine wid gc x0 y0 x1 y1 */
      if (LST_PRIMARGC < 7 || !initialized) return NULL;
      GETWIN(win, 1);
      GETGC(gc, 2);
      GETINT(x0, 3);
      GETINT(y0, 4);
      GETINT(x1, 5);
      GETINT(y1, 6);
      XDrawLine(curDisp, win, gc, x0, y0, x1, y1);
      break;
    case 42: /* XDrawRect wid gc x0 y0 wdt hgt */
      if (LST_PRIMARGC < 7 || !initialized) return NULL;
      GETWIN(win, 1);
      GETGC(gc, 2);
      GETINT(x, 3);
      GETINT(y, 4);
      GETINT(w, 5);
      GETINT(h, 6);
      XDrawRectangle(curDisp, win, gc, x, y, w, h);
      break;
    case 43: /* XFillRect wid gc x y w h */
      if (LST_PRIMARGC < 7 || !initialized) return NULL;
      GETWIN(win, 1);
      GETGC(gc, 2);
      GETINT(x, 3);
      GETINT(y, 4);
      GETINT(w, 5);
      GETINT(h, 6);
      XFillRectangle(curDisp, win, gc, x, y, w, h);
      break;

    case 50: /* XftFontOpen name */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      GETSTR(str, 1);
      font = XftFontOpenName(curDisp, screen, str);
      if (!font) return NULL;
      res = newX11(&xi, X11_XFTFONT);
      if (xi) xi->font = font; else XftFontClose(curDisp, font);
      break;
    case 51: /* XftFontClose font */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      if (!(xi = getX11As(LST_PRIMARG(1), X11_XFTFONT))) return NULL;
      deinitX11Obj(xi);
      break;
    case 52: /* XftDrawCreate win */
      if (LST_PRIMARGC < 2 || !initialized) return NULL;
      GETWIN(win, 1);
      xd = XftDrawCreate(curDisp, win, vis, curCMap);
      if (!xd) return NULL;
      res = newX11(&xi, X11_XFTDRAW);
      if (xi) xi->xd = xd; else XftDrawDestroy(xd);
      break;
    case 53: /* XftDrawDestroy drw */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      if (!(xi = getX11As(LST_PRIMARG(1), X11_XFTDRAW))) return NULL;
      deinitX11Obj(xi);
      break;
    case 54: /* XNewTextColor r g b */
      if (LST_PRIMARGC < 4 || !initialized) return NULL;
      if (getColor(LST_PRIMARG(1), &rcc.red)) return NULL;
      if (getColor(LST_PRIMARG(2), &rcc.green)) return NULL;
      if (getColor(LST_PRIMARG(3), &rcc.blue)) return NULL;
      rcc.alpha = 0;
      XftColorAllocValue(curDisp, vis, curCMap, &rcc, &tc);
      res = newX11(&xi, X11_XFTCOLOR);
      if (xi) xi->tc = tc; else XftColorFree(curDisp, vis, curCMap, &tc);
      break;
    case 55: /* XFreeTextColor tc */
      if (LST_PRIMARGC < 2 || !curDisp) return NULL;
      if (!(xi = getX11As(LST_PRIMARG(1), X11_XFTCOLOR))) return NULL;
      deinitX11Obj(xi);
      break;
    case 56: /* XftFontDraw drw color font x y text */
      if (LST_PRIMARGC < 7 || !initialized) return NULL;
      GETDRAW(xd, 1);
      GETTC(tc, 2);
      GETFONT(font, 3);
      GETINT(x, 4);
      GETINT(y, 5);
      GETSTR(str, 6);
      XftDrawStringUtf8(xd, &tc, font, x, y, (const XftChar8 *)(str), strlen(str)); /*FIXME: use object size here*/
      break;
    case 57: /* XftTextExtents font text [what] */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETFONT(font, 1);
      GETSTR(str, 2);
      XftTextExtentsUtf8(curDisp, font, (const XftChar8 *)(str), strlen(str), &info); /*FIXME: use object size here*/
      if (LST_PRIMARGC > 3) {
        op = LST_PRIMARG(3); if (!LST_IS_SMALLINT(op)) return NULL;
        switch (lstIntValue(op)) {
          case 0: goto xftTextExtentsGetAll;
          case 1: res = lstNewInt((int)info.width); break;
          case 2: res = lstNewInt((int)info.height); break;
          case 3: res = lstNewInt(info.x); break;
          case 4: res = lstNewInt(info.y); break;
          case 5: res = lstNewInt(info.xOff); break;
          case 6: res = lstNewInt(info.yOff); break;
          default: return NULL;
        }
      } else {
xftTextExtentsGetAll: (void)0;
        LST_ENTER_BLOCK();
        LST_NEW_TEMP(nfo);
        nfo = lstNewArray(6);
        nfo->data[0] = lstNewInt((int)info.width);
        nfo->data[1] = lstNewInt((int)info.height);
        nfo->data[2] = lstNewInt(info.x);
        nfo->data[3] = lstNewInt(info.y);
        nfo->data[4] = lstNewInt(info.xOff);
        nfo->data[5] = lstNewInt(info.yOff);
        LST_LEAVE_BLOCK();
        res = nfo;
      }
      break;
    case 80: /* withdraw/iconify win true:withdraw */
      if (LST_PRIMARGC < 3 || !initialized) return NULL;
      GETWIN(win, 1);
      op = LST_PRIMARG(2);
      if (op == lstTrueObj) XWithdrawWindow(curDisp, win, screen);
      else XIconifyWindow(curDisp, win, screen);
      break;
    case 81: /* wmhints win hintid val ... */
      if (LST_PRIMARGC < 4 || !initialized) return NULL;
      GETWIN(win, 1);
      if (win == rootWin) return NULL;
      GETINT(act, 2);
      op = LST_PRIMARG(3);
      wmh = XGetWMHints(curDisp, win);
      if (!wmh) return NULL;
      switch (act) {
        case 0: /* input type */
          wmh->flags |= InputHint;
          wmh->input = op==lstNilObj||op==lstFalseObj ? False : True;
          break;
        case 1: /* urgency */
          if (op == lstNilObj || op == lstFalseObj) wmh->flags &= ~(XUrgencyHint);
          else wmh->flags |= XUrgencyHint;
          break;
        default: XFree(wmh); wmh = NULL; res = NULL; break;
      }
      if (wmh) {
        XSetWMHints(curDisp, win, wmh);
        XFree(wmh);
      }
      break;
    default: return NULL;
  }
  /*if (curDisp) XFlush(curDisp);*/
  return res;
}


void x11LoopStep (void) {
  if (!curDisp) return;
  /*x11_fd = ConnectionNumber(dis);*/
  while (XPending(curDisp) > 0) {
    if (eqCount >= MAX_QUEUE) return;
    XNextEvent(curDisp, &eQueue[eqTail++]);
    if (eqTail == MAX_QUEUE) eqTail = 0;
    ++eqCount;
  }
}


int x11HasEvent (void) {
  return eqCount > 0;
}


#ifdef DEBUG_EVENTS
const struct {
  const char *name;
  int id;
} dbgEventNames[] = {
  {"KeyPress", 2},
  {"KeyRelease", 3},
  {"ButtonPress", 4},
  {"ButtonRelease", 5},
  {"MotionNotify", 6},
  {"EnterNotify", 7},
  {"LeaveNotify", 8},
  {"FocusIn", 9},
  {"FocusOut", 10},
  {"KeymapNotify", 11},
  {"Expose", 12},
  {"GraphicsExpose", 13},
  {"NoExpose", 14},
  {"VisibilityNotify", 15},
  {"CreateNotify", 16},
  {"DestroyNotify", 17},
  {"UnmapNotify", 18},
  {"MapNotify", 19},
  {"MapRequest", 20},
  {"ReparentNotify", 21},
  {"ConfigureNotify", 22},
  {"ConfigureRequest", 23},
  {"GravityNotify", 24},
  {"ResizeRequest", 25},
  {"CirculateNotify", 26},
  {"CirculateRequest", 27},
  {"PropertyNotify", 28},
  {"SelectionClear", 29},
  {"SelectionRequest", 30},
  {"SelectionNotify", 31},
  {"ColormapNotify", 32},
  {"ClientMessage", 33},
  {"MappingNotify", 34},
  {"GenericEvent", 35},
  {0}
};


const char *dbgEentName (int id) {
  int f;
  for (f = 0; dbgEventNames[f].name; ++f) {
    if (dbgEventNames[f].id == id) return dbgEventNames[f].name;
  }
  return "UnknownEventType";
}
#endif


static struct {
  const char *name;
  lstObject *sym; /* not need to register as root, 'cause symbols will not disappear */
} eventNames[] = {
  /* 0*/{"MotionNotify", 0},
  /* 1*/{"ButtonPress", 0},
  /* 2*/{"ButtonRelease", 0},
  /* 3*/{"EnterNotify", 0},
  /* 4*/{"LeaveNotify", 0},
  /* 5*/{"Expose", 0},
  /* 6*/{"FocusIn", 0},
  /* 7*/{"FocusOut", 0},
  /* 8*/{"KeymapNotify", 0},
  /* 9*/{"KeyPress", 0},
  /*10*/{"KeyRelease", 0},
  /*11*/{"MapNotify", 0},
  /*12*/{"UnmapNotify", 0},
  /*13*/{"VisibilityNotify", 0},
  /*14*/{"DestroyNotify", 0},
  /*15*/{"ConfigureNotify", 0},
  /* WM specials */
  /*16*/{"WMCloseRequest", 0},
  {0}
};

lstObject *x11GetEvent (void) {
  LST_ENTER_BLOCK();
  LST_NEW_TEMP(res);
  LST_NEW_TEMP(win);
  while (eqCount > 0) {
    X11Object *xi;
    XEvent *e = &eQueue[eqHead++];
    if (eqHead == MAX_QUEUE) eqHead = 0;
    --eqCount;
#ifdef DEBUG_EVENTS
    fprintf(stderr, "GETEVENT: event=%d; win=%p (%s)\n", e->type, (void *)e->xany.window, dbgEentName(e->type));
#endif
    win = newX11(&xi, X11_WINDOW);
    xi->dontKill = 1;
    xi->win = e->xany.window;
    switch (e->type) {
      case MotionNotify: {
        XMotionEvent *ee = (XMotionEvent *)e;
        /* type, window, x, y, keymask, xroot, yroot */
        res = lstNewArray(7);
        res->data[0] = eventNames[0].sym;
        res->data[1] = win;
        res->data[2] = lstNewInteger(ee->x);
        res->data[3] = lstNewInteger(ee->y);
        res->data[4] = lstNewInteger(ee->state);
        res->data[5] = lstNewInteger(ee->x_root);
        res->data[6] = lstNewInteger(ee->y_root);
        LST_LEAVE_BLOCK();
        return res; }
      case ButtonPress:
      case ButtonRelease: {
        /* type, window, x, y, keymask, xroot, yroot, button */
        XButtonEvent *ee = (XButtonEvent *)e;
        res = lstNewArray(8);
        res->data[0] = e->type==ButtonPress ? eventNames[1].sym : eventNames[2].sym;
        res->data[1] = win;
        res->data[2] = lstNewInteger(ee->x);
        res->data[3] = lstNewInteger(ee->y);
        res->data[4] = lstNewInteger(ee->state);
        res->data[5] = lstNewInteger(ee->x_root);
        res->data[6] = lstNewInteger(ee->y_root);
        res->data[7] = lstNewInteger(ee->button);
        LST_LEAVE_BLOCK();
        return res; }
      case EnterNotify:
      case LeaveNotify: {
        /* type window focus */
        XCrossingEvent *ee = (XCrossingEvent *)e;
        res = lstNewArray(3);
        res->data[0] = e->type==EnterNotify ? eventNames[3].sym : eventNames[4].sym;
        res->data[1] = win;
        res->data[2] = ee->focus ? lstTrueObj : lstFalseObj;
        LST_LEAVE_BLOCK();
        return res; }
      case Expose: {
        /* type window x y wdt hgt count */
        XExposeEvent *ee = (XExposeEvent *)e;
        res = lstNewArray(7);
        res->data[0] = eventNames[5].sym;
        res->data[1] = win;
        res->data[2] = lstNewInteger(ee->x);
        res->data[3] = lstNewInteger(ee->y);
        res->data[4] = lstNewInteger(ee->width);
        res->data[5] = lstNewInteger(ee->height);
        res->data[6] = lstNewInteger(ee->count);
        LST_LEAVE_BLOCK();
        return res; }
      case FocusIn:
      case FocusOut: {
        /* type window */
        /*XFocusChangeEvent *ee = (XFocusChangeEvent *)e;*/
        res = lstNewArray(2);
        res->data[0] = e->type==FocusIn ? eventNames[6].sym : eventNames[7].sym;
        res->data[1] = win;
        LST_LEAVE_BLOCK();
        return res; }
      case KeymapNotify: {
/*
        res = lstNewArray(2);
        res->data[0] = eventNames[8].sym;
        res->data[1] = win;
        LST_LEAVE_BLOCK();
        return res;
*/
        break; }
      case KeyPress:
      case KeyRelease: {
        /*TODO: i18n*/
        static char keyName[32];
        KeySym key;
        /* type window x, y, keymask, xroot, yroot, key */
        XKeyEvent *ee = (XKeyEvent *)e;
        res = lstNewArray(9);
        res->data[0] = e->type==KeyPress ? eventNames[9].sym : eventNames[10].sym;
        res->data[1] = win;
        res->data[2] = lstNewInteger(ee->x);
        res->data[3] = lstNewInteger(ee->y);
        res->data[4] = lstNewInteger(ee->state);
        res->data[5] = lstNewInteger(ee->x_root);
        res->data[6] = lstNewInteger(ee->y_root);
        res->data[7] = lstNewInteger(ee->keycode);
        int xslen = XLookupString(ee, keyName, sizeof(keyName), &key, 0);
        if (xslen < 0) xslen = 0;
        keyName[xslen] = '\0';
        /*fprintf(stderr, "[%s]\n", keyName);*/
        if (xslen == 1) {
          res->data[8] = lstNewChar(keyName[0]);
        } else {
          res->data[8] = lstNewString(keyName);
        }
        LST_LEAVE_BLOCK();
        return res; }
        break;
      case MapNotify: {
        /* type, window */
        /*XMapEvent *ee = (XMapEvent *)e;*/
        res = lstNewArray(2);
        res->data[0] = eventNames[11].sym;
        res->data[1] = win;
        LST_LEAVE_BLOCK();
        return res; }
      case UnmapNotify: {
        /* type, window */
        /*XMapEvent *ee = (XMapEvent *)e;*/
        res = lstNewArray(2);
        res->data[0] = eventNames[12].sym;
        res->data[1] = win;
        LST_LEAVE_BLOCK();
        return res; }
      case VisibilityNotify: {
        /* type, window visflag */
        XVisibilityEvent *ee = (XVisibilityEvent *)e;
        res = lstNewArray(3);
        res->data[0] = eventNames[13].sym;
        res->data[1] = win;
        res->data[2] = ee->state==VisibilityFullyObscured ? lstNilObj :
          (ee->state==VisibilityPartiallyObscured ? lstFalseObj : lstTrueObj);
        LST_LEAVE_BLOCK();
        return res; }
      case DestroyNotify: {
        /* type, window */
        /*XMapEvent *ee = (XMapEvent *)e;*/
        res = lstNewArray(2);
        res->data[0] = eventNames[14].sym;
        res->data[1] = win;
        LST_LEAVE_BLOCK();
        return res; }
      case ConfigureNotify: {
        /* type, window */
        XConfigureEvent *ee = (XConfigureEvent *)e;
        res = lstNewArray(6);
        res->data[0] = eventNames[15].sym;
        res->data[1] = win;
        res->data[2] = lstNewInteger(ee->x);
        res->data[3] = lstNewInteger(ee->y);
        res->data[4] = lstNewInteger(ee->width);
        res->data[5] = lstNewInteger(ee->height);
        LST_LEAVE_BLOCK();
        return res; }
      case ClientMessage: {
        XClientMessageEvent *ee = (XClientMessageEvent *)e;
        /* Window Manager or something else */
        if ((Atom)ee->data.l[0] == wmDeleteWindow) {
          res = lstNewArray(2);
          res->data[0] = eventNames[16].sym;
          res->data[1] = win;
          LST_LEAVE_BLOCK();
          return res;
        }
      }
    }
  }
  LST_LEAVE_BLOCK();
  return lstNilObj;
}


static const LSTExtPrimitiveTable primTbl[] = {
{"X11Do", lpX11Do, NULL},
{0}};


void lstInitX11Library (void) {
  int f;
  for (f = 0; eventNames[f].name; ++f) {
    eventNames[f].sym = lstNewSymbol(eventNames[f].name);
    lstAddStaticRoot(&eventNames[f].sym);
  }
}

void lstInitPrimitivesX11 (void) {
  lstRegisterExtPrimitiveTable(primTbl);
}
