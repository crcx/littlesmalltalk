/** \file
 * \brief iupcells control.
 *
 * See Copyright Notice in iup.h
 */

#ifndef __IUPCELLS_H
#define __IUPCELLS_H

#define IUP_OUT  -999

#ifndef IUP_ALL 
#define IUP_ALL  "ALL"
#endif

#ifndef IUP_BOXED 
#define IUP_BOXED  "BOXED"
#endif

#ifndef IUP_CLIPPED 
#define IUP_CLIPPED  "CLIPPED"
#endif

#ifndef IUP_TRANSPARENT 
#define IUP_TRANSPARENT  "TRANSPARENT"
#endif

#ifndef IUP_NON_SCROLLABLE_LINES 
#define IUP_NON_SCROLLABLE_LINES  "NON_SCROLLABLE_LINES"
#endif

#ifndef IUP_NON_SCROLLABLE_COLS 
#define IUP_NON_SCROLLABLE_COLS  "NON_SCROLLABLE_COLS"
#endif

#ifndef IUP_ORIGIN 
#define IUP_ORIGIN  "ORIGIN"
#endif

#ifndef IUP_NO_COLOR 
#define IUP_NO_COLOR  "NO_COLOR"
#endif

#ifndef IUP_FIRST_LINE 
#define IUP_FIRST_LINE  "FIRST_LINE"
#endif

#ifndef IUP_FIRST_COL 
#define IUP_FIRST_COL  "FIRST_COL"
#endif

#ifndef IUP_DOUBLE_BUFFER 
#define IUP_DOUBLE_BUFFER  "DOUBLE_BUFFER"
#endif

#ifndef IUP_BUFFERIZE 
#define IUP_BUFFERIZE  "BUFFERIZE"
#endif

#ifndef IUP_REPAINT 
#define IUP_REPAINT  "REPAINT"
#endif

#ifndef IUP_LIMITS 
#define IUP_LIMITS  "LIMITS"
#endif

#ifndef IUP_CANVAS 
#define IUP_CANVAS  "CANVAS"
#endif

#ifndef IUP_IMAGE_CANVAS 
#define IUP_IMAGE_CANVAS  "IMAGE_CANVAS"
#endif

#ifndef IUP_FULL_VISIBLE 
#define IUP_FULL_VISIBLE  "FULL_VISIBLE"
#endif

#ifndef IUP_MOUSECLICK_CB
#define IUP_MOUSECLICK_CB "MOUSECLICK_CB"
#endif

#ifndef IUP_MOUSEMOTION_CB
#define IUP_MOUSEMOTION_CB "MOUSEMOTION_CB"
#endif

#ifndef IUP_DRAW_CB
#define IUP_DRAW_CB "DRAW_CB"
#endif

#ifndef IUP_WIDTH_CB
#define IUP_WIDTH_CB "WIDTH_CB"
#endif

#ifndef IUP_HEIGHT_CB
#define IUP_HEIGHT_CB "HEIGHT_CB"
#endif

#ifndef IUP_NLINES_CB
#define IUP_NLINES_CB "NLINES_CB"
#endif

#ifndef IUP_NCOLS_CB
#define IUP_NCOLS_CB "NCOLS_CB"
#endif

#ifndef IUP_HSPAN_CB
#define IUP_HSPAN_CB "HSPAN_CB"
#endif

#ifndef IUP_VSPAN_CB
#define IUP_VSPAN_CB "VSPAN_CB"
#endif

#ifndef IUP_SCROLLING_CB
#define IUP_SCROLLING_CB "SCROLLING_CB"
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */              

Ihandle* IupCells(void);

#ifdef __cplusplus
} 
#endif  /* __cplusplus */              

#endif /* _iupcells_h */
