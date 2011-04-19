/** \file
 * \brief iupmatrix control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPMATRIX_H 
#define __IUPMATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*
* Prototipos das funcao de incializacao e instaciancao e
* atributos especificos da matrix.
*/

Ihandle* IupMatrix(char *action);

void  IupMatSetAttribute  (Ihandle *n, char* a, int l, int c, char* v);
void  IupMatStoreAttribute(Ihandle *n, char* a, int l, int c, char* v);
char* IupMatGetAttribute  (Ihandle *n, char* a, int l, int c);
int   IupMatGetInt        (Ihandle *n, char* a, int l, int c);
float IupMatGetFloat      (Ihandle *n, char* a, int l, int c);
void  IupMatSetfAttribute (Ihandle *n, char* a, int l, int c, char* f, ...);


#define IUP_ENTERITEM_CB   "ENTERITEM_CB"
#define IUP_LEAVEITEM_CB   "LEAVEITEM_CB"
#define IUP_EDITION_CB     "EDITION_CB"
#define IUP_CLICK_CB       "CLICK_CB"
#define IUP_DROP_CB        "DROP_CB"
#define IUP_DROPSELECT_CB  "DROPSELECT_CB"
#define IUP_DROPCHECK_CB   "DROPCHECK_CB"
#define IUP_SCROLL_CB      "SCROLL_CB"
#define IUP_VALUE_CB       "VALUE_CB"
#define IUP_VALUE_EDIT_CB  "VALUE_EDIT_CB"
#define IUP_FIELD_CB       "FIELD_CB"
#define IUP_RESIZEMATRIX   "RESIZEMATRIX"
#define IUP_ADDLIN         "ADDLIN"
#define IUP_ADDCOL         "ADDCOL"
#define IUP_DELLIN         "DELLIN"
#define IUP_DELCOL         "DELCOL"
#define IUP_NUMLIN         "NUMLIN"
#define IUP_NUMCOL         "NUMCOL"
#define IUP_NUMLIN_VISIBLE "NUMLIN_VISIBLE"
#define IUP_NUMCOL_VISIBLE "NUMCOL_VISIBLE"
#define IUP_MARKED         "MARKED"
#define IUP_WIDTHDEF       "WIDTHDEF"
#define IUP_HEIGHTDEF      "HEIGHTDEF"
#define IUP_AREA           "AREA"
#define IUP_MARK_MODE      "MARK_MODE"
#define IUP_LIN            "LIN"
#define IUP_COL            "COL"
#define IUP_LINCOL         "LINCOL"
#define IUP_CELL           "CELL"
#define IUP_EDIT_MODE      "EDIT_MODE"
#define IUP_FOCUS_CELL     "FOCUS_CELL"
#define IUP_ORIGIN         "ORIGIN"
#define IUP_REDRAW         "REDRAW"
#define IUP_PREVIOUSVALUE  "PREVIOUSVALUE"
#define IUP_MOUSEMOVE_CB   "MOUSEMOVE_CB"

#ifdef __cplusplus
}
#endif

#endif
