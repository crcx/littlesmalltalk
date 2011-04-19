/** \file
 * \brief Contains all function pointer typedefs.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPCBS_H 
#define __IUPCBS_H

typedef int (*IFidle)(void);  /* idle */

typedef int (*IFn)(Ihandle*);  /* default definition */
typedef int (*IFni)(Ihandle*, int);   /* k_any, show_cb, toggle_action, spin_cb, branchopen_cb, branchclose_cb, executeleaf_cb, showrename_cb, rightclick_cb, extended_cb */
typedef int (*IFnii)(Ihandle*, int, int);  /* resize_cb, caret_cb, matrix_mousemove_cb, enteritem_cb, leaveitem_cb, scrolltop_cb, dropcheck_cb, selection_cb, select_cb, switch_cb */
typedef int (*IFniii)(Ihandle*, int, int, int); /* trayclick_cb, edition_cb */
typedef int (*IFniiii)(Ihandle*, int, int, int, int); /* dragdrop_cb */
typedef int (*IFniiiiii)(Ihandle*, int, int, int, int, int, int);  /* draw_cb */
                                                       
typedef int (*IFnff)(Ihandle*, float, float);    /* canvas_action */
typedef int (*IFniff)(Ihandle*,int,float,float);  /* scroll_cb */

typedef int (*IFnnii)(Ihandle*, Ihandle*, int, int); /* drop_cb */
typedef int (*IFnnn)(Ihandle*, Ihandle*, Ihandle*); /* tabchange_cb */
typedef int (*IFnss)(Ihandle*, char *, char *);  /* file_cb */
typedef int (*IFns)(Ihandle*, char *);  /* multiselect_cb */
typedef int (*IFnis)(Ihandle*, int, char *);  /* text_action, multiline_action, edit_cb, renamenode_cb, rename_cb */
typedef int (*IFnsii)(Ihandle*, char*, int, int);  /* list_action */
typedef int (*IFnsiii)(Ihandle*, char*, int, int, int); /* dropfiles_cb */
typedef int (*IFniis)(Ihandle*, int, int, char*);  /* motion_cb, click_cb, value_edit_cb */
typedef int (*IFniiiis)(Ihandle*, int, int, int, int, char*);  /* button_cb, matrix_action */

typedef int (*IFnIi)(Ihandle*, int*, int); /* multiselection_cb, multiselection_cb */
typedef int (*IFnd)(Ihandle*, double);  /* mousemove_cb, button_press_cb, button_release_cb */
typedef int (*IFniiIII)(Ihandle*, int, int, int*, int*, int*); /* fgcolor_cb, bgcolor_cb */
typedef int (*IFniinsii)(Ihandle*, int, int, Ihandle*, char*, int, int); /* dropselect_cb */
typedef int (*IFnccc)(Ihandle*, unsigned char, unsigned char, unsigned char); /* drag_cb, change_cb */

typedef char* (*sIFnii)(Ihandle*, int, int);  /* value_cb */
typedef char* (*sIFni)(Ihandle*, int);  /* cell_cb */

#endif
