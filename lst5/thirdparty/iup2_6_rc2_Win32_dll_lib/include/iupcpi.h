/** \file
 * \brief Control Creation Interface.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPCPI_H 
#define __IUPCPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iclass_ Iclass;
typedef void (*Imethod)(Ihandle *, ...);

/* Method names */
#define ICPI_SETNATURALSIZE "SETNATURALSIZE"
#define ICPI_SETCURRENTSIZE "SETCURRENTSIZE"
#define ICPI_GETSIZE        "GETSIZE"
#define ICPI_SETPOSITION    "SETPOSITION"
#define ICPI_CREATE         "CREATE"
#define ICPI_DESTROY        "DESTROY" 
#define ICPI_MAP            "MAP"
#define ICPI_UNMAP          "UNMAP"
#define ICPI_SETATTR        "SETATTR"
#define ICPI_GETATTR        "GETATTR"
#define ICPI_GETDEFAULTATTR "GETDEFAULTATTR"
#define ICPI_POPUP          "POPUP"

int   iupCpiDefaultSetNaturalSize (Ihandle* self);
void  iupCpiDefaultSetCurrentSize (Ihandle* self, int w, int h);
int   iupCpiDefaultGetSize (Ihandle* self, int* w, int* h);
void  iupCpiDefaultSetPosition (Ihandle* self, int w, int h);
Ihandle* iupCpiDefaultCreate (Iclass* self, Ihandle** array);
void  iupCpiDefaultDestroy (Ihandle* self);
void  iupCpiDefaultMap (Ihandle* self, Ihandle* parent);
void  iupCpiDefaultUnmap (Ihandle* self);
void  iupCpiDefaultSetAttr (Ihandle* self, char* attr, char* value);
char* iupCpiDefaultGetAttr (Ihandle* self, char* attr);
char* iupCpiDefaultGetDefaultAttr (Ihandle* self, char* attr);

int   iupCpiSetNaturalSize( Ihandle* self );
void  iupCpiSetCurrentSize( Ihandle* self, int w, int h );
int   iupCpiGetSize( Ihandle* self, int* w, int* h );
void  iupCpiSetPosition ( Ihandle* self, int w, int h );
void  iupCpiDestroy( Ihandle* self );
void  iupCpiMap( Ihandle* self, Ihandle* parent );
void  iupCpiUnmap( Ihandle* self );
void  iupCpiSetAttribute( Ihandle* self, char* attr, char* value );
char* iupCpiGetAttribute( Ihandle* self, char* attr );
char* iupCpiGetDefaultAttr( Ihandle* self, char* attr );
int   iupCpiPopup( Ihandle* self, int x, int y );
Ihandle *iupCpiCreate(Iclass *ic, Ihandle **params);

void iupCpiFinish(void);
void iupCpiInit(void);

char *iupCpiGetClassName(Iclass *ic);
Iclass* iupCpiGetClass(char *name);
char *iupCpiGetClassFormat(Iclass *ic);
Imethod iupCpiGetClassMethod(Iclass *ic, char *method);
int iupCpiSetClassMethod(Iclass *ic, char *method, Imethod func);

Iclass *iupCpiCreateNewClass(char *name, char *format);
void iupCpiFreeClass(Iclass *a);

#ifdef __cplusplus
}
#endif

#endif
