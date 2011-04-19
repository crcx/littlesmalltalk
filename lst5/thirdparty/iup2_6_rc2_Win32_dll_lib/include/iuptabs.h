/** \file
 * \brief iuptabs control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPTABS_H 
#define __IUPTABS_H

#ifdef __cplusplus
extern "C" {
#endif

/* callbacks */

/* Atributos */
#ifndef ICTL_TOP
#define ICTL_TOP "TOP"
#endif

#ifndef ICTL_BOTTOM
#define ICTL_BOTTOM "BOTTOM"
#endif

#ifndef ICTL_LEFT
#define ICTL_LEFT "LEFT"
#endif

#ifndef ICTL_RIGHT  
#define ICTL_RIGHT "RIGHT"
#endif

#ifndef ICTL_TABTYPE
#define ICTL_TABTYPE "TABTYPE"
#endif

#ifndef ICTL_TABTITLE
#define ICTL_TABTITLE "TABTITLE"
#endif

#ifndef ICTL_TABSIZE
#define ICTL_TABSIZE "TABSIZE"
#endif

#ifndef ICTL_TABCHANGE_CB
#define ICTL_TABCHANGE_CB "TABCHANGE_CB"
#endif

#ifndef ICTL_FONT
#define ICTL_FONT "FONT"
#endif

#ifndef ICTL_FONT_ACTIVE
#define ICTL_FONT_ACTIVE "FONT_ACTIVE"
#endif

#ifndef ICTL_FONT_INACTIVE
#define ICTL_FONT_INACTIVE "FONT_INACTIVE"
#endif

#ifndef IUP_FGCOLOR
#define IUP_FGCOLOR "FGCOLOR"
#endif

#ifndef IUP_BGCOLOR
#define IUP_BGCOLOR "BGCOLOR"
#endif


/* iuptabs */

Ihandle *IupTabsv(Ihandle** params);
Ihandle *IupTabs(Ihandle* first, ...);

#ifdef __cplusplus
}
#endif

#endif
