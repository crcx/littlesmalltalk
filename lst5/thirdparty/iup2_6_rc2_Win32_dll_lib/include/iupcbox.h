/** \file
 * \brief iupcbox control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPCBOX_H 
#define __IUPCBOX_H

#ifdef __cplusplus
extern "C" {
#endif

Ihandle *IupCbox(Ihandle* first, ...);
Ihandle *IupCboxv(Ihandle** params);
Ihandle *IupCboxBuilder(Ihandle * first,...);
Ihandle *IupCboxBuilderv(Ihandle** params);

#ifdef __cplusplus
}
#endif

#endif
