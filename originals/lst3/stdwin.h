/* GENERIC STDWIN -- INTERFACE HEADER FILE. */

#ifndef GENERIC_STDWIN

#define GENERIC_STDWIN
	/* So this header file is skipped if included twice */

#define CURSOR_CARET
	/* Defined so textedit will always set the caret at the start of
	   the focus -- useful only for ASCII terminals. */

#ifndef ARGS
#define ARGS(x) ()	/* replace by x for ANSI C */
#endif

#ifndef NOARGS
#define NOARGS ()	/* replace by (void) for ANSI C */
#endif

#ifndef bool
#define bool int
#endif

void winit NOARGS;
void winitnew ARGS((int *pargc, char ***pargv));
void wdone NOARGS;

void wgetscrsize ARGS((int *pwidth, int *pheight));
void wgetscrmm ARGS((int *pmmwidth, int *pmmheight));

void wsetmaxwinsize ARGS((int width, int height));
void wsetdefwinsize ARGS((int width, int height));
void wsetdefwinpos ARGS((int h, int v));

#define MENU struct menu

/* The contents of a text attributes struct are disclosed here because
   the interface allows the programmer to declare objects of this type.
   (I'm not so sure anymore that this is the right thing to do!) */

struct textattr {
	short font;
	unsigned char size;
	unsigned char style;
};

#define TEXTATTR struct textattr

#ifndef WINDOW

struct window {
	short tag;
};

#define WINDOW struct window

#endif

WINDOW *wopen ARGS((char *title,
		void (*drawproc)(/*WINDOW *win,
				int left, int top, int right, int bottom*/)));
void wclose ARGS((WINDOW *win));
#define wgettag(win) ((win)->tag)
#define wsettag(win, newtag) ((win)->tag= newtag)
void wsetactive ARGS((WINDOW *win));
WINDOW *wgetactive NOARGS;
void wgetwinsize ARGS((WINDOW *win, int *width, int *height));
void wsetdocsize ARGS((WINDOW *win, int width, int height));
void wsettitle ARGS((WINDOW *win, char *title));

void wsetorigin ARGS((WINDOW *win, int h, int v));
void wshow ARGS((WINDOW *win, int left, int top, int right, int bottom));
void wchange ARGS((WINDOW *win, int left, int top, int right, int bottom));
void wscroll ARGS((WINDOW *win, int left, int top, int right, int bottom,
	int dh, int dv));

void wfleep NOARGS;
void wmessage ARGS((char *str));
void wperror ARGS((char *name));
bool waskstr ARGS((char *prompt, char *buf, int buflen));
int waskync ARGS((char *question, int dflt));
bool waskfile ARGS((char *prompt, char *buf, int buflen, bool newfile));

void wsetcaret ARGS((WINDOW *win, int h, int v));
void wnocaret ARGS((WINDOW *win));

void wsettimer ARGS((WINDOW *win, int deciseconds));

MENU *wmenucreate ARGS((int id, char *title));
void wmenudelete ARGS((MENU *mp));
int wmenuadditem ARGS((MENU *mp, char *text, int shortcut));
void wmenusetitem ARGS((MENU *mp, int i, char *text));
void wmenusetdeflocal ARGS((bool local));
void wmenuattach ARGS((WINDOW *win, MENU *mp));
void wmenudetach ARGS((WINDOW *win, MENU *mp));
void wmenuenable ARGS((MENU *mp, int item, int flag));
void wmenucheck ARGS((MENU *mp, int item, int flag));

/* The following is only available in termcap stdwin: */
void wsetshortcut ARGS((int id, int item, char *keys));

#include "stdevent.h"

void wgetevent ARGS((EVENT *ep));
void wungetevent ARGS((EVENT *ep));
void wupdate ARGS((WINDOW *win));
void wbegindrawing ARGS((WINDOW *win));
void wenddrawing ARGS((WINDOW *win));
void wflush NOARGS;

void wdrawline ARGS((int h1, int v1, int h2, int v2));
void wxorline ARGS((int h1, int v1, int h2, int v2));
void wdrawcircle ARGS((int h, int v, int radius));
void wdrawelarc ARGS((int h, int v, int hrad, int vrad, int ang1, int ang2));
void wdrawbox ARGS((int left, int top, int right, int bottom));
void werase ARGS((int left, int top, int right, int bottom));
void wpaint ARGS((int left, int top, int right, int bottom));
void winvert ARGS((int left, int top, int right, int bottom));
void wshade ARGS((int left, int top, int right, int bottom, int percent));

int wdrawtext ARGS((int h, int v, char *str, int len));
int wdrawchar ARGS((int h, int v, int c));
int wlineheight NOARGS;
int wbaseline NOARGS;
int wtextwidth ARGS((char *str, int len));
int wcharwidth ARGS((int c));
int wtextbreak ARGS((char *str, int len, int width));

void wgettextattr ARGS((TEXTATTR *attr));
void wsettextattr ARGS((TEXTATTR *attr));
void wgetwintextattr ARGS((WINDOW *win, TEXTATTR *attr));
void wsetwintextattr ARGS((WINDOW *win, TEXTATTR *attr));

void wsetplain NOARGS;
void wsethilite NOARGS;
void wsetinverse NOARGS;
void wsetitalic NOARGS;
void wsetbold NOARGS;
void wsetbolditalic NOARGS;
void wsetunderline NOARGS;

void wsetfont ARGS((char *fontname));
void wsetsize ARGS((int pointsize));

#include "stdtext.h"

#endif /* GENERIC_STDWIN */
