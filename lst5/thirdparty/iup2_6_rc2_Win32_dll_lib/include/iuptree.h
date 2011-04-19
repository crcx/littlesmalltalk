/** \file
 * \brief iuptree control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPTREE_H 
#define __IUPTREE_H

#ifdef __cplusplus
extern "C" {
#endif

Ihandle* IupTree( void );

int IupTreeSetUserId(Ihandle *h, int id, void *userid);
void* IupTreeGetUserId(Ihandle *h, int id);
int IupTreeGetId(Ihandle *h, void *userid);

void  IupTreeSetAttribute  (Ihandle *n, char* a, int id, char* v);
void  IupTreeStoreAttribute(Ihandle *n, char* a, int id, char* v);
char* IupTreeGetAttribute  (Ihandle *n, char* a, int id);
int   IupTreeGetInt        (Ihandle *n, char* a, int id);
float IupTreeGetFloat      (Ihandle *n, char* a, int id);
void  IupTreeSetfAttribute (Ihandle *n, char* a, int id, char* f, ...);

/* Attributes which are only set */
#define IUP_ADDLEAF          "ADDLEAF"
#define IUP_ADDBRANCH        "ADDBRANCH"
#define IUP_DELNODE          "DELNODE"
#define IUP_IMAGELEAF        "IMAGELEAF"
#define IUP_IMAGEBRANCHCOLLAPSED "IMAGEBRANCHCOLLAPSED"
#define IUP_IMAGEBRANCHEXPANDED  "IMAGEBRANCHEXPANDED"
#define IUP_IMAGEEXPANDED     "IMAGEEXPANDED"

/* Attributes which are only retrieved */
#define IUP_KIND             "KIND"
#define IUP_PARENT           "PARENT"
#define IUP_DEPTH            "DEPTH"
#define IUP_MARKED           "MARKED"

/* Attributes that are both set and retrieved */
#define IUP_ADDEXPANDED      "ADDEXPANDED"
#define IUP_CTRL             "CTRL"
#define IUP_SHIFT            "SHIFT"
#define IUP_NAME             "NAME"
#define IUP_STATE            "STATE"
#define IUP_STARTING         "STARTING"

#define IUP_LEAF             "LEAF"
#define IUP_BRANCH           "BRANCH"

#define IUP_SELECTED         "SELECTED"
#define IUP_CHILDREN         "CHILDREN"
#define IUP_MARKED           "MARKED"

#define IUP_ROOT             "ROOT"
#define IUP_LAST             "LAST"
#define IUP_PGUP             "PGUP"
#define IUP_PGDN             "PGDN"
#define IUP_NEXT             "NEXT"
#define IUP_PREVIOUS         "PREVIOUS"
#define IUP_INVERT           "INVERT"
#define IUP_BLOCK            "BLOCK"
#define IUP_CLEARALL         "CLEARALL"
#define IUP_MARKALL          "MARKALL"
#define IUP_INVERTALL        "INVERTALL"

#define IUP_REDRAW           "REDRAW"
#define IUP_COLLAPSED        "COLLAPSED"
#define IUP_EXPANDED         "EXPANDED"

/* User callbacks */
#define IUP_SELECTION_CB "SELECTION_CB"
#define IUP_BRANCHOPEN_CB    "BRANCHOPEN_CB"
#define IUP_BRANCHCLOSE_CB   "BRANCHCLOSE_CB"
#define IUP_RIGHTCLICK_CB    "RIGHTCLICK_CB"
#define IUP_EXECUTELEAF_CB   "EXECUTELEAF_CB"
#define IUP_RENAMENODE_CB    "RENAMENODE_CB"

/* Pre-definied images */
#define IUP_IMGLEAF             "IMGLEAF"
#define IUP_IMGCOLLAPSED        "IMGCOLLAPSED"
#define IUP_IMGEXPANDED         "IMGEXPANDED"
#define IUP_IMGBLANK            "IMGBLANK"
#define IUP_IMGPAPER            "IMGPAPER"

#ifdef __cplusplus
}
#endif

#endif
