/** \file
 * \brief Utilities using IM
 *
 * See Copyright Notice in iup.h
 */

#ifndef __IUPIM_H
#define __IUPIM_H

#if	defined(__cplusplus)
extern "C" {
#endif

Ihandle* IupLoadImage(const char* file_name);
int IupSaveImage(Ihandle* image, const char* file_name, const char* format);

#if defined(__cplusplus)
}
#endif

#endif
