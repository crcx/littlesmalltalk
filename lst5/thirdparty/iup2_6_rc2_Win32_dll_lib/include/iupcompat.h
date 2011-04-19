/** \file
 * \brief Internal exported functions. repeted from <iglobal.h>
 * should remove this file!!!!
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPCOMPAT_H 
#define __IUPCOMPAT_H

#include <iupcbs.h>
#include <string.h>
#include <istrutil.h>

#ifdef __cplusplus
extern "C" {
#endif

/* idrv.h */
void  iupdrvGetCharSize (Ihandle* n, int *w, int *h);
void  iupdrvResizeObjects (Ihandle *n);
void  iupdrvStringSize(Ihandle* n, char* text, int *w, int *h);
int   iupdrvGetScrollSize(void);

/* env.c */
int   iupGetSize (Ihandle* n, int* w, int *h);
int   iupGetColor(Ihandle* n, char *a, unsigned int *r, unsigned int *g, unsigned int *b);
int   iupGetRGB (char *color, unsigned int *r, unsigned int *g, unsigned int *b);
void  iupSetEnv(Ihandle* n, char* a, char* v);
void  iupStoreEnv(Ihandle* n, char* a, char* v);
char* iupGetEnv(Ihandle* n, char* a);
int   iupGetEnvInt (Ihandle* n, char* a);
char* iupGetEnvRaw (Ihandle* n, char* a);   /* no inheritance */
int   iupCheck (Ihandle *n, char *a);

/* calcsize.c  */
int iupSetSize(Ihandle* root);

/* iup.c */
void  iupSetPosX(Ihandle* self, int x);
void  iupSetPosY(Ihandle* self, int y);
int   iupGetPosX(Ihandle* self);
int   iupGetPosY(Ihandle* self);
void  iupSetCurrentWidth(Ihandle* self, int w);
void  iupSetCurrentHeight(Ihandle* self, int h);
int   iupGetCurrentWidth(Ihandle* self);
int   iupGetCurrentHeight(Ihandle* self);
void* iupGetNativeHandle(Ihandle* self);
void  iupSetNativeHandle(Ihandle* self, void* handle);
void  iupSetNaturalWidth(Ihandle* self, int w);
void  iupSetNaturalHeight(Ihandle* self, int h);
int   iupGetNaturalWidth(Ihandle* self);
int   iupGetNaturalHeight(Ihandle* self);
void* iupGetImageData(Ihandle* self);

#ifdef __cplusplus
}
#endif

#endif
