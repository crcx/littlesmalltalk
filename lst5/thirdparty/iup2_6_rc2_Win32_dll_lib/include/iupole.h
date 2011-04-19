/** \file
 * \brief iupole control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPOLE_H 
#define __IUPOLE_H

#ifdef __cplusplus
extern "C" {
#endif

Ihandle *IupOleControl(char *);
void IupOleControlOpen(void);

void *IupOleControl_getclass(void);

#ifdef __cplusplus
}
#endif

#endif

