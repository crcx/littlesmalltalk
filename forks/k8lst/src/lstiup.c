#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lstiup.h"


#ifdef USE_IUP
#include "iup.h"
#include "iupcontrols.h"
#include "iupim.h"

void guiInit (int *argc, char ***argv) {
  IupOpen(argc, argv);
  IupControlsOpen();
  IupImageLibOpen();
}


void guiShutdown (void) {
  /*IupImageLibClose();*/
  IupControlsClose();
  IupClose();
}


void guiModalMessage (const char *title, const char *message) {
  IupMessage((char *)title, (char *)message);
}


static const char *iupHId = "IUPHandle";

typedef struct {
  const char *type;
  Ihandle *ih;
  int uid; /* unique id */
} IUPInfo;
static int iupUID = 1;


LST_FINALIZER(lpIUPHandleFinalizer) {
  IUPInfo *fi = (IUPInfo *)udata;
  if (fi) {
    if (fi->ih) {
      static char tbuf0[128];
      sprintf(tbuf0, ":%08d", fi->uid);
      IupSetFunction(tbuf0, NULL);
      IupDestroy(fi->ih);
    }
    free(fi);
  }
}


static lstObject *newIUPHandle (Ihandle *ih) {
  IUPInfo *hh;
  lstObject *res = lstNewBinary(NULL, 0);
  hh = malloc(sizeof(IUPInfo));
  if (!hh) { return NULL; }
  hh->type = iupHId;
  hh->ih = ih;
  hh->uid = iupUID++;
  lstSetFinalizer(res, lpIUPHandleFinalizer, hh);
  return res;
}


static IUPInfo *getIUPInfo (lstObject *o) {
  if (!LST_IS_BYTES_EX(o) || LST_SIZE(o) || !o->fin || !o->fin->udata) return NULL;
  IUPInfo *fi = (IUPInfo *)o->fin->udata;
  if (fi->type != iupHId) return NULL;
  return fi;
}


static void setIUPHandle (lstObject *o, Ihandle *h) {
  IUPInfo *fi = getIUPInfo(o);
  if (fi) fi->ih = h;
}


static Ihandle *getIUPHandle (lstObject *o) {
  IUPInfo *fi = getIUPInfo(o);
  return fi ? fi->ih : NULL;
}


static int getIUPUID (lstObject *o) {
  IUPInfo *fi = getIUPInfo(o);
  return fi ? fi->uid : 0;
}


static void clearIUPHandle (lstObject *o) {
  IUPInfo *fi = getIUPInfo(o);
  if (!fi) return;
  if (fi->ih) {
    static char tbuf0[128];
    sprintf(tbuf0, ":%08d", fi->uid);
    IupSetFunction(tbuf0, NULL);
    IupDestroy(fi->ih);
    fi->uid = 0;
  }
  fi->ih = NULL;
}


typedef struct LstIUPEvent LstIUPEvent;
struct LstIUPEvent {
  char *name; /* dynamic string */
  LstIUPEvent *next;
};

static LstIUPEvent *head = NULL;
static LstIUPEvent *tail = NULL;


static lstObject *getEvent (void) {
  if (!head) return NULL;
  LstIUPEvent *i = head;
  if ((head = head->next) == NULL) tail = NULL;
  lstObject *res = lstNewString(i->name);
  free(i->name);
  free(i);
  return res;
}


/*FIXME: check for 'out of memory'*/
static void addEvent (const char *str) {
  LstIUPEvent *i = malloc(sizeof(LstIUPEvent));
  i->name = strdup(str);
  i->next = NULL;
  if (tail) tail->next = i; else head = i;
  tail = i;
}


int guiHasEvent (void) {
  return (head != 0);
}


static int iupCallback (Ihandle *iHandle) {
  /*fprintf(stderr, "ACTION: %s\n", IupGetActionName());*/
  addEvent(IupGetActionName());
  return IUP_DEFAULT;
}


static int iupListCallback (Ihandle *iHandle, const char *text, int index, int selectionFlag) {
  static char buf[1024];
  sprintf(buf, "%s %d %d", IupGetActionName(), index, selectionFlag);
  addEvent(buf);
  return IUP_DEFAULT;
}


LST_PRIMFN_NONSTATIC(lpIUPDispatcher) {
  static char tbuf0[8192], tbuf1[8192]; /*FIXME*/
  if (LST_PRIMARGC < 1) return NULL;
  lstObject *op = LST_PRIMARG(0);
  if (!LST_IS_SMALLINT(op)) return NULL;
  int action = lstIntValue(op);
  switch (action) {
    case 1: /* show message; title text */
      if (LST_PRIMARGC < 3) return NULL;
      op = LST_PRIMARG(1);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      op = LST_PRIMARG(2);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf1, sizeof(tbuf1), op);
      IupMessage(tbuf0, tbuf1);
      break;

    case 2: { /* create control; type targ0 [targ1] */
      int controlType;
      lstObject *res = NULL;
      Ihandle *iHandle = 0, *ih1;
      if (LST_PRIMARGC < 2) return NULL;
      op = LST_PRIMARG(1);
      if (!LST_IS_SMALLINT(op)) return NULL;
      controlType = lstIntValue(op);
      if (controlType < 0) return NULL;
      /***/
      if (controlType <= 6) {
        res = newIUPHandle(iHandle);
        if (LST_PRIMARGC < 3) return NULL;
        /* 2nd arg is a text -- action name; can be nil */
        op = LST_PRIMARG(2);
        if (op == lstTrueObj) {
          sprintf(tbuf0, ":%08d", getIUPUID(res));
        } else if (op == lstNilObj) {
          tbuf0[0] = '\0';
        } else {
          if (!LST_IS_BYTES_EX(op)) return NULL;
          lstGetString(tbuf0, sizeof(tbuf0), op);
        }
        if (LST_PRIMARGC > 3) {
          op = LST_PRIMARG(3);
          if (!LST_IS_BYTES_EX(op)) return NULL;
          lstGetString(tbuf1, sizeof(tbuf1), op);
        } else tbuf1[0] = '\0';
        /* register event */
        if (controlType != 6 && tbuf0[0]) {
          if (controlType == 5) IupSetFunction(tbuf0, (Icallback)iupListCallback);
          else IupSetFunction(tbuf0, iupCallback);
        }
        switch (controlType) {
          case 0: /* create a canvas; actionname */
            setIUPHandle(res, IupCanvas(tbuf0[0] ? tbuf0 : NULL));
            break;
          case 1: /* create a button; actionname title */
            setIUPHandle(res, IupButton(tbuf1, tbuf0[0] ? tbuf0 : NULL));
            break;
          case 2: /* create a toggle; actionname title */
            setIUPHandle(res, IupToggle(tbuf1, tbuf0[0] ? tbuf0 : NULL));
            break;
          case 3: /* create an editable text field; actionname */
            setIUPHandle(res, IupText(tbuf0[0] ? tbuf0 : NULL));
            break;
          case 4: /* create a multi line text control; actionname */
            setIUPHandle(res, IupMultiLine(tbuf0[0] ? tbuf0 : NULL));
            break;
          case 5: /* create a list; actionname */
            setIUPHandle(res, IupList(tbuf0[0] ? tbuf0 : NULL));
            break;
          case 6: /* create a label; title */
            setIUPHandle(res, IupLabel(tbuf0));
            break;
        }
        return res;
      }
      /***/
      if (controlType <= 9) {
        switch (controlType) {
          case 7: /* create a progress bar */
            iHandle = IupProgressBar();
            break;
          case 8: /* create a spin */
            iHandle = IupSpin();
            break;
          case 9: /* create a tree */
            iHandle = IupTree();
            break;
        }
        return newIUPHandle(iHandle);
      }

      if (controlType == 10) {
        /* create a frame; elid */
        ih1 = NULL;
        if (LST_PRIMARGC >= 3) {
          op = LST_PRIMARG(2);
          if (op != lstNilObj) {
            if ((ih1 = getIUPHandle(op)) == NULL) return NULL;
          }
        }
        return newIUPHandle(IupFrame(ih1));
      }
      /***/
      if (controlType == 11) {
        /* create a dialog; elid */
        lstObject *res;
        if (LST_PRIMARGC < 3) return NULL;
        if ((ih1 = getIUPHandle(LST_PRIMARG(2))) == NULL) return NULL;
        res = newIUPHandle(IupDialog(ih1));
        sprintf(tbuf0, ":%08d", getIUPUID(res));
        IupSetFunction(tbuf0, iupCallback);
        /*IupSetAttribute(getIUPHandle(res), "CLOSE_CB", tbuf0);*/
        return res;
      }
      /***/
      if (controlType == 12) {
        /* create a trackbar; vertflag */
        lstObject *res;
        if (LST_PRIMARGC < 3) return NULL;
        op = LST_PRIMARG(2);
        res = newIUPHandle(IupVal(op==lstNilObj||op==lstFalseObj ? "HORIZONTAL" : "VERTICAL"));
        sprintf(tbuf0, ":%08d", getIUPUID(res));
        IupSetFunction(tbuf0, iupCallback);
        /*IupSetAttribute(getIUPHandle(res), "VALUECHANGED_CB", tbuf0);*/
        return res;
      }
      return NULL;
      }

    case 3: {
      int ctype;
      Ihandle *iHandle = 0, *ih0, *ih1;
      if (LST_PRIMARGC < 2) return NULL;
      op = LST_PRIMARG(1);
      if (!LST_IS_SMALLINT(op)) return NULL;
      ctype = lstIntValue(op);
      switch (ctype) {
        case 0: /* fill */
          iHandle = IupFill();
          break;
        case 1: /* hbox */
          iHandle = IupHbox(NULL);
          break;
        case 2: /* vbox */
          iHandle = IupVbox(NULL);
          break;
        case 3: /* zbox */
          iHandle = IupZbox(NULL);
          break;
        case 4: /* radio box; contelem */
          if (LST_PRIMARGC < 3) return NULL;
          if ((ih1 = getIUPHandle(LST_PRIMARG(2))) == NULL) return NULL;
          iHandle = IupRadio(ih1);
          break;
        case 5: /* cbox */
          iHandle = IupCbox(NULL);
          break;
        case 6: /* sbox */
          iHandle = IupCbox(NULL);
          break;
        case 7: /* normalizer */
          iHandle = IupNormalizer(NULL);
          break;
        case 8: { /* split; elid0 elid1 */
          if (LST_PRIMARGC == 2) {
            iHandle = IupSplit(NULL, NULL);
          } else if (LST_PRIMARGC >= 4) {
            if ((ih0 = getIUPHandle(LST_PRIMARG(2))) == NULL) return NULL;
            if ((ih1 = getIUPHandle(LST_PRIMARG(3))) == NULL) return NULL;
            iHandle = IupSplit(ih0, ih1);
          }
          } break;
        default: return NULL;
      }
      if (iHandle) return newIUPHandle(iHandle);
      return NULL;
      }

    case 4: { /* append element; boxid; elementid */
      Ihandle *ih0, *ih1;
      if (LST_PRIMARGC < 3) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      if ((ih1 = getIUPHandle(LST_PRIMARG(2))) == NULL) return NULL;
      IupAppend(ih0, ih1);
      } break;

    case 5: { /* detach element; elementid */
      Ihandle *ih0;
      if (LST_PRIMARGC < 2) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      IupDetach(ih0);
      } break;

    case 6: { /* destroy a GUI element; elementid */
      Ihandle *ih0;
      if (LST_PRIMARGC < 2) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      IupDestroy(ih0);
      clearIUPHandle(LST_PRIMARG(1));
      } break;

    case 7: { /* show/hide interface element; elementid [hideflag] */
      Ihandle *ih0;
      if (LST_PRIMARGC < 2) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      op = lstNilObj;
      if (LST_PRIMARGC > 2) op = LST_PRIMARG(2);
      if (op == lstNilObj || op == lstFalseObj) IupShow(ih0); else IupHide(ih0);
      } break;

    case 8: { /* register an event; actionname */
      if (LST_PRIMARGC < 2) return NULL;
      op = LST_PRIMARG(1);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      /*fprintf(stderr, "REGISTER EVENT: [%s]\n", tbuf0);*/
      if (LST_PRIMARGC == 2) IupSetFunction(tbuf0, iupCallback);
      else IupSetFunction(tbuf0, NULL);
      } break;

    case 9: { /* register a list event; actionname */
      if (LST_PRIMARGC < 2) return NULL;
      op = LST_PRIMARG(1);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      if (LST_PRIMARGC == 2) IupSetFunction(tbuf0, (Icallback)iupListCallback);
      else IupSetFunction(tbuf0, NULL);
      } break;

    case 10: /* get next event (non blocking event polling) */
      /*fprintf(stderr, "NEXT EVENT!\n");*/
      if ((op = getEvent()) == NULL) {
        IupLoopStep();
        op = getEvent();
      }
      if (op) return op;
      return lstNilObj;
      break;

    case 11: { /* set an attribute of a GUI element; elementid attrname value */
      Ihandle *ih0;
      if (LST_PRIMARGC < 4) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      op = LST_PRIMARG(2);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      op = LST_PRIMARG(3);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf1, sizeof(tbuf1), op);
      IupStoreAttribute(ih0, tbuf0, tbuf1);
      } break;

    case 12: { /* read the value of a widget's attribute; elementid attrname */
      Ihandle *ih0;
      if (LST_PRIMARGC < 3) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      op = LST_PRIMARG(2);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      return lstNewString(IupGetAttribute(ih0, tbuf0));
      } break;

    case 13: { /* delete an attribute (by setting its value to NULL); elementid attrname */
      Ihandle *ih0;
      if (LST_PRIMARGC < 3) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      op = LST_PRIMARG(2);
      if (!LST_IS_BYTES_EX(op)) return NULL;
      lstGetString(tbuf0, sizeof(tbuf0), op);
      IupSetAttribute(ih0, tbuf0, NULL);
      } break;

    case 14: { /* popup interface element; elementid */
      Ihandle *ih0;
      if (LST_PRIMARGC < 2) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      IupPopup(ih0, IUP_MOUSEPOS, IUP_MOUSEPOS);
      } break;

    case 15: { /* show interface element; elementid */
      Ihandle *ih0;
      if (LST_PRIMARGC < 2) return NULL;
      if ((ih0 = getIUPHandle(LST_PRIMARG(1))) == NULL) return NULL;
      /*fprintf(stderr, "SHOW!\n");*/
      IupShow(ih0);
      } break;

    case 250: { /* get handle as string; elementid */
      if (LST_PRIMARGC < 2) return NULL;
      if (getIUPHandle(LST_PRIMARG(1)) == NULL) return NULL;
      sprintf(tbuf0, ":%08d", getIUPUID(LST_PRIMARG(1)));
      return lstNewString(tbuf0);
      }
    default: return NULL;
  }
  return lstTrueObj;
}


void guiLoopStep (void) {
  IupLoopStep();
}


static const LSTExtPrimitiveTable iupPrimTable[] = {
{"IUPDispatcher", lpIUPDispatcher, NULL},
{0}};


void lstInitPrimitivesIUP (void) {
  lstRegisterExtPrimitiveTable(iupPrimTable);
}

#else

void guiInit (int *argc, char ***argv) {}
void guiShutdown (void) {}
void guiModalMessage (const char *title, const char *message) {
  fprintf(stderr, "GUI MESSAGE [%s]: %s\n", title, message);
}

void lstInitPrimitivesIUP (void) {}

void guiLoopStep (void) {}
int guiHasEvent (void) { return 0; }

#endif
