/* STDWIN -- TEXTEDIT PACKAGE DEFINITIONS */

#define TEXTEDIT struct _textedit

TEXTEDIT *tealloc ARGS((WINDOW *win, int left, int top, int width));
TEXTEDIT *tecreate ARGS((WINDOW *win,
	int left, int top, int right, int bottom));
void tefree ARGS((TEXTEDIT *tp));
void tedestroy ARGS((TEXTEDIT *tp));

void tedraw ARGS((TEXTEDIT *tp));
void tedrawnew ARGS((TEXTEDIT *tp,
	int left, int top, int right, int bottom));
void temove ARGS((TEXTEDIT *tp, int left, int top, int width));
void temovenew ARGS((TEXTEDIT *tp,
	int left, int top, int right, int bottom));

void tesetfocus ARGS((TEXTEDIT *tp, int foc1, int foc2));
void tereplace ARGS((TEXTEDIT *tp, char *str));
void tesetbuf ARGS((TEXTEDIT *tp, char *buf, int buflen));

void tearrow ARGS((TEXTEDIT *tp, int code));
void tebackspace ARGS((TEXTEDIT *tp));
bool teclicknew ARGS((TEXTEDIT *tp, int h, int v, bool extend));
bool tedoubleclick ARGS((TEXTEDIT *tp, int h, int v));
bool teevent ARGS((TEXTEDIT *tp, EVENT *ep));

#define teclick(tp, h, v) teclicknew(tp, h, v, FALSE)
#define teclickextend(tp, h, v) teclicknew(tp, h, v, TRUE)

char *tegettext ARGS((TEXTEDIT *tp));
int tegetlen ARGS((TEXTEDIT *tp));
int tegetnlines ARGS((TEXTEDIT *tp));
int tegetfoc1 ARGS((TEXTEDIT *tp));
int tegetfoc2 ARGS((TEXTEDIT *tp));
int tegetleft ARGS((TEXTEDIT *tp));
int tegettop ARGS((TEXTEDIT *tp));
int tegetright ARGS((TEXTEDIT *tp));
int tegetbottom ARGS((TEXTEDIT *tp));

/* Text paragraph drawing functions: */

int wdrawpar ARGS((int h, int v, char *text, int width));
	/* Returns new v coord. */
int wparheight ARGS((char *text, int width));
	/* Returns height */

/* Clipboard functions; */

void wsetclip ARGS((char *p, int len));
char *wgetclip NOARGS;
