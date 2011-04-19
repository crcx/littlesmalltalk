/** \file
 * \brief iupgauge control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPGAUGE_H 
#define __IUPGAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* callbacks */
#ifndef ICTL_MOUSEMOVE_CB
#define ICTL_MOUSEMOVE_CB "MOUSEMOVE_CB"
#endif

#ifndef ICTL_BUTTON_PRESS_CB 
#define ICTL_BUTTON_PRESS_CB "BUTTON_PRESS_CB"
#endif

#ifndef ICTL_BUTTON_RELEASE_CB
#define ICTL_BUTTON_RELEASE_CB "BUTTON_RELEASE_CB"
#endif

#ifndef ICTL_SHOW_TEXT
#define ICTL_SHOW_TEXT "SHOW_TEXT"
#endif

#ifndef ICTL_DASHED
#define ICTL_DASHED "DASHED"
#endif

#ifndef ICTL_MARGIN
#define ICTL_MARGIN "MARGIN"
#endif

#ifndef ICTL_TEXT
#define ICTL_TEXT "TEXT"
#endif

#ifndef IUP_FGCOLOR
#define IUP_FGCOLOR "FGCOLOR"
#endif

#ifndef IUP_BGCOLOR
#define IUP_BGCOLOR "BGCOLOR"
#endif

Ihandle *IupGauge(void);

#ifdef __cplusplus
}
#endif

#endif
