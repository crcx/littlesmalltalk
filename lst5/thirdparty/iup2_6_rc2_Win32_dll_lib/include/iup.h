/** \file
 * \brief User API
 * IUP - A Portable User Interface Toolkit
 * Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil
 * http://www.tecgraf.puc-rio.br/iup	mailto:iup@tecgraf.puc-rio.br
 *
 * See Copyright Notice at the end of this file
 */
 
#ifndef __IUP_H 
#define __IUP_H

#include <iupkey.h>
#include <iupdef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IUP_COPYRIGHT	"Copyright (C) 1994-2007 Tecgraf/PUC-Rio and PETROBRAS S/A"
#define IUP_VERSION "2.6.0"
#define IUP_VERSION_DATE "2007/05/10"
#define IUP_VERSION_NUMBER 206000

typedef struct Ihandle_ Ihandle;
typedef int (*Icallback)(Ihandle*);

/************************************************************************/
/*                      pre-definided dialogs                           */
/************************************************************************/
int  IupGetFile    (char *arq);
void IupMessage    (char *title, char *msg);
void IupMessagef   (char *title, char *format, ...);
int  IupAlarm      (char *title, char *msg, char *b1, char *b2, char *b3);
int  IupScanf      (const char *format, ...);
int  IupListDialog (int type, char *title, int size, char *lst[],
                    int op, int col, int line, int marks[]);
int  IupGetText    (char* title, char* text);

/************************************************************************/
/*                      Functions                                       */
/************************************************************************/
char*     IupMapFont       (char *iupfont);
char*     IupUnMapFont     (char *driverfont);
int       IupMainLoop      (void);
int       IupLoopStep      (void);
void      IupExitLoop      (void);
int       IupOpen          (void);
void      IupClose         (void);
void      IupFlush         (void);
int       IupHelp          (char* url);
char*     IupVersion       (void);
char*     IupVersionDate   (void);
int       IupVersionNumber (void);

void      IupDetach        (Ihandle *ih);
Ihandle*  IupAppend        (Ihandle *ih, Ihandle *child);
Ihandle*  IupGetNextChild  (Ihandle *ih, Ihandle *next);
Ihandle*  IupGetBrother    (Ihandle *ih);
Ihandle*  IupGetParent     (Ihandle *ih);

void      IupDestroy       (Ihandle *ih);
int       IupPopup         (Ihandle *ih, int x, int y);
int       IupShow          (Ihandle *ih);
int       IupShowXY        (Ihandle *ih, int x, int y);
int       IupHide          (Ihandle *ih);
int       IupMap           (Ihandle *ih);
Ihandle*  IupSetFocus      (Ihandle *ih);
Ihandle*  IupGetFocus      (void);
void      IupSetLanguage   (char *lng);
char*     IupGetLanguage   (void);
void      IupRefresh       (Ihandle *self);
void      IupUpdate        (Ihandle* ih);

char *    IupLoad          (char *filename);

void      IupSetAttribute  (Ihandle *ih, char* name, char* value);
void      IupStoreAttribute(Ihandle *ih, char* name, char* value);
Ihandle*  IupSetAttributes (Ihandle *ih, char *str);
char*     IupGetAttribute  (Ihandle *ih, char* name);
char*     IupGetAttributes (Ihandle *ih);
int       IupGetInt        (Ihandle *ih, char* name);
int       IupGetInt2       (Ihandle *ih, char* name);
float     IupGetFloat      (Ihandle *ih, char* name);
void      IupSetfAttribute (Ihandle *ih, char* name, char* format, ...);

void      IupSetGlobal     (const char* name, char* value);
void      IupStoreGlobal   (const char* name, char* value);
char*     IupGetGlobal     (const char* name);

Ihandle*  IupPreviousField  (Ihandle *ih);  
Ihandle*  IupNextField      (Ihandle *ih);

Icallback IupGetCallback(Ihandle *ih, const char *name);
Icallback IupSetCallback(Ihandle *ih, const char *name, Icallback func);
Ihandle*  IupSetCallbacks(Ihandle* ih, ...);

Icallback   IupGetFunction   (const char *name);
Icallback   IupSetFunction   (const char *name, Icallback func);
const char* IupGetActionName (void);

Ihandle*  IupGetHandle     (const char *name);
Ihandle*  IupSetHandle     (const char *name, Ihandle *ih);
int       IupGetAllNames   (char *names[], int n);
int       IupGetAllDialogs (char *names[], int n);
char*     IupGetName       (Ihandle* ih);

void      IupSetAttributeHandle (Ihandle *ih, char* name, Ihandle *ih_named);
Ihandle*  IupGetAttributeHandle(Ihandle *ih, char* name);

Ihandle*  IupGetDialog     (Ihandle *ih);
char*     IupGetType       (Ihandle *ih);

Ihandle*  IupCreate        (char *name);
Ihandle*  IupCreatev       (char *name, Ihandle **children);
Ihandle*  IupCreatep       (char *name, Ihandle* first, ...);
Ihandle*  IupVbox          (Ihandle *first, ...);
Ihandle*  IupVboxv         (Ihandle **params);
Ihandle*  IupZbox          (Ihandle *first, ...);
Ihandle*  IupZboxv         (Ihandle **params);
Ihandle*  IupHbox          (Ihandle *first,...);
Ihandle*  IupHboxv         (Ihandle **params);
Ihandle*  IupFill          (void);

Ihandle*  IupRadio         (Ihandle *child);
Ihandle*  IupFrame         (Ihandle* child);

Ihandle*  IupColor         (unsigned int r, unsigned int g, unsigned int b);
Ihandle*  IupImage         (unsigned width, unsigned height, char *pixmap);
Ihandle*  IupButton        (char* title, char* action);
Ihandle*  IupCanvas        (char* repaint);
Ihandle*  IupDialog        (Ihandle* child);
Ihandle*  IupUser          (void);
Ihandle*  IupItem          (char* title, char* action);
Ihandle*  IupSubmenu       (char* title, Ihandle* child);
Ihandle*  IupSeparator     (void);
Ihandle*  IupLabel         (char* title);
Ihandle*  IupList          (char* action);
Ihandle*  IupMenu          (Ihandle *first,...);
Ihandle*  IupMenuv         (Ihandle **params);
Ihandle*  IupText          (char* action);
Ihandle*  IupMultiLine     (char* action);
Ihandle*  IupToggle        (char* title, char* action);
Ihandle*  IupFileDlg       (void);
Ihandle*  IupTimer         (void);

#ifdef __cplusplus
}
#endif

/************************************************************************/
/*                   Common Return Values                               */
/************************************************************************/
#define IUP_ERROR     1
#define IUP_NOERROR   0
#define IUP_OPENED    -1
#define IUP_INVALID   -1

/************************************************************************/
/*                   Callback Return Values                             */
/************************************************************************/
#define IUP_IGNORE    -1
#define IUP_DEFAULT   -2
#define IUP_CLOSE     -3
#define IUP_CONTINUE  -4

/************************************************************************/
/*           IupPopup and IupShowXY Parameter Values                    */
/************************************************************************/
#define IUP_CENTER    0xFFFF  /* 65535 */
#define IUP_LEFT      0xFFFE  /* 65534 */
#define IUP_RIGHT     0xFFFD  /* 65533 */
#define IUP_MOUSEPOS  0xFFFC  /* 65532 */
#define IUP_CURRENT   0xFFFB  /* 65531 */
#define IUP_TOP       IUP_LEFT
#define IUP_BOTTOM    IUP_RIGHT
#define IUP_ANYWHERE  IUP_CURRENT

#define IUP_SBUP       1
#define IUP_SBDN       2
#define IUP_SBPGUP     3
#define IUP_SBPGDN     4
#define IUP_SBPOSV     5
#define IUP_SBPGLEFT   6
#define IUP_SBPGRIGHT  7
#define IUP_SBLEFT     8
#define IUP_SBRIGHT    9
#define IUP_SBPOSH    10
#define IUP_SBDRAGH   11
#define IUP_SBDRAGV   12

/************************************************************************/
/*               IUP_SHOW_CB Callback Values                            */
/************************************************************************/
#define IUP_SHOW       0
#define IUP_RESTORE    1
#define IUP_MINIMIZE   2
#define IUP_MAXIMIZE   3
#define IUP_HIDE       4

/************************************************************************/
/*               Mouse Button Values and Macros                         */
/************************************************************************/
#define IUP_BUTTON1   '1'
#define IUP_BUTTON2   '2'
#define IUP_BUTTON3   '3'

#define isshift(_s)    (_s[0]=='S')
#define iscontrol(_s)  (_s[1]=='C')
#define isbutton1(_s)  (_s[2]=='1')
#define isbutton2(_s)  (_s[3]=='2')
#define isbutton3(_s)  (_s[4]=='3')
#define isdouble(_s)   (_s[5]=='D')
#define isalt(_s)      (_s[6]=='A')

/************************************************************************/
/*              Replacement for the WinMain in Windows,                 */
/*        this allows the application to start from "main".             */
/************************************************************************/
#if defined (__WATCOMC__) || defined (__MWERKS__)
#ifdef __cplusplus
extern "C" {
int IupMain (int argc, char** argv); /* In C++ we have to declare the prototype */
}
#endif
#define main IupMain /* this is the trick for Watcom and MetroWerks */
#endif

/******************************************************************************
* Copyright (C) 1994-2007 Tecgraf/PUC-Rio and PETROBRAS S/A.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#endif
