/** \file
 * \brief IM Lua 5 Binding
 *
 * See Copyright Notice in im_lib.h
 * $Id: imlua.h,v 1.5 2005/12/12 15:42:29 scuri Exp $
 */

#ifndef __IMLUA_H
#define __IMLUA_H

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup imlua IM Lua 5 Binding
 * \par
 * Binding for the Lua 5 scripting language. Works with versions 5.0 and 5.1. \n
 * Lua 5.0.2 Copyright (C) 1994-2004 Tecgraf, PUC-Rio                         \n
 * Lua 5.1 Copyright (C) 1994-2005 Lua.org, PUC-Rio                           \n
 * R. Ierusalimschy, L. H. de Figueiredo & W. Celes                           \n
 * http://www.lua.org
 * \par
 * The name of the functions were changed because of the namespace "im" and because of the object orientation. \n
 * As a general rule use:
\verbatim
    imXxx  -> im.Xxx
    IM_XXX -> im.XXX
    imFileXXX(ifile,... -> ifile:XXX(...
    imImageXXX(image,... -> image:XXX(...
\endverbatim
 * All the objects are garbage collected by the Lua garbage collector.
 * \par
 * See \ref imlua.h
 * \ingroup util */

#ifdef LUA_NOOBJECT  /* Lua 3 */
void imlua_open(void);
#endif

#ifdef LUA_TNONE  /* Lua 5 */

/** Initializes the Lua binding of the main IM library.  \n
 * You must link the application with the "imlua5" library.
 * \ingroup imlua */
int imlua_open(lua_State *L);

/** Initializes the Lua binding of the capture library.  \n
 * You must link the application with the "imlua_capture5" library.
 * \ingroup imlua */
int imlua_open_capture(lua_State *L);

/** Initializes the Lua binding of the process library.  \n
 * You must link the application with the "imlua_process5" library.
 * \ingroup imlua */
int imlua_open_process(lua_State *L);

/** Initializes the Lua binding of the fourier transform library.  \n
 * You must link the application with the "imlua_fftw5" library.
 * \ingroup imlua */
int imlua_open_fftw(lua_State *L);

/** Initializes the Lua binding of addicional functions for the CD library.  \n
 * CD library is a 2D graphics library. \n
 * http://www.tecgraf.puc-rio.br/cd \n
 * You must link the application with the "imlua_cd5" library.
 *
 * The following functions are defined:
 * \verbatim bitmap:imImageCreate() -> image: imImage [in Lua 5] 
     Creates an imImage from a cdBitmap. \endverbatim
 * 
 * \verbatim image:cdCreateBitmap() -> bitmap: cdBitmap [in Lua 5]
     Creates a cdBitmap from an imImage. 
     The imImage must be a bitmap image, see imImageIsBitmap. \endverbatim
 * 
 * \verbatim image:cdInitBitmap() -> bitmap: cdBitmap [in Lua 5] 
     Creates a cdBitmap from an imImage, but reuses image data. 
     When the cdBitmap is destroyed the data is preserved. \endverbatim
 * 
 * \verbatim image:cdPutImageRect(x: number, y: number, w: number, h: number, xmin: number, xmax: number, ymin: number, ymax: number) [in Lua 5]
     Draws the imImage into the active cdCanvas.
     The imImage must be a bitmap image, see imImageIsBitmap. \endverbatim
 * 
 * \verbatim image:wdPutImageRect(x: number, y: number, w: number, h: number, xmin: number, xmax: number, ymin: number, ymax: number) [in Lua 5]
     Draws the imImage into the active cdCanvas using world coordinates.
     The imImage must be a bitmap image, see imImageIsBitmap. \endverbatim
 * 
 * \verbatim image:cdGetImage(x: number, y: number) [in Lua 5]
     Retrieve the imImage data from the active cdCanvas.
     The imImage must be a IM_RGB/IM_BYTE image. \endverbatim
 * 
 * \verbatim image:cdCreateCanvas([res: number]) -> canvas: cdCanvas [in Lua 5]
     Creates a cdCanvas using the CD_IMAGERGB driver. Resolution is optional, default is 3.8 pixels per milimiter (96.52 DPI).
     The imImage must be a IM_RGB/IM_BYTE image. \endverbatim
 * 
 * \ingroup imlua */
int imlua_open_cd(lua_State *L);


#endif

#ifdef __cplusplus
}
#endif

#endif
