/** \file
 * \brief Library Management and Main Documentation
 *
 * See Copyright Notice in this file.
 * $Id: im_lib.h,v 1.9 2006/11/24 13:18:06 scuri Exp $
 */

#ifndef __IM_LIB_H
#define __IM_LIB_H

#if	defined(__cplusplus)
extern "C" {
#endif

      
/** \defgroup lib Library Management
 * \ingroup util 
 * \par
 * Usefull definitions for about dialogs and 
 * for checking the compiled version with the linked version for dynamic libraries.
 * \par
 * See \ref im_lib.h
 * @{
 */
#define IM_AUTHOR "Antonio Scuri"
#define IM_COPYRIGHT "Copyright (C) 1994-2006 Tecgraf/PUC-Rio and PETROBRAS S/A"
#define IM_VERSION "3.2.0"
#define IM_VERSION_DATE "2006/11/24"
/** @} */

/** Library release number used in the compilation time. \n
 * You can compare this with the value returned by \ref imVersionNumber.
 * \ingroup lib */
#define IM_VERSION_NUMBER 302000

/** Returns the library current version.
 *
 * \verbatim im.Version() -> version: string [in Lua 5] \endverbatim
 * \ingroup lib */
const char* imVersion(void);

/** Returns the library current version release date.
 *
 * \verbatim im.VersionDate() -> date: string [in Lua 5] \endverbatim
 * \ingroup lib */
const char* imVersionDate(void);

/** Returns the library current version number.
 *
 * \verbatim im.VersionNumber() -> version: number [in Lua 5] \endverbatim
 * \ingroup lib */
int imVersionNumber(void);


#if defined(__cplusplus)
}
#endif


/*! \mainpage IM
 * <CENTER>
 * <H3> Image Representation, Storage, Capture and Processing </H3>
 * Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil \n
 * http://www.tecgraf.puc-rio.br/im \n
 * mailto:im@tecgraf.puc-rio.br
 * </CENTER>
 *
 * \section over Overview
 * \par
 * IM is a toolkit for Digital Imaging. 
 * \par
 * It provides support for image capture, several image file formats and many image processing operations. 
 * \par
 * Image representation includes scientific data types (like IEEE floating point data) 
 * and attributes (or metadata like GeoTIFF and Exif tags).
 * Animation, video and volumes are supported as image sequences, 
 * but there is no digital audio support.
 * \par
 * The main goal of the library is to provide a simple API and abstraction
 * of images for scientific applications.
 * \par
 * The toolkit API is written in C. 
 * The core library source code is implemented in C++ and it is very portable, 
 * it can be compiled in Windows and UNIX with no modifications. 
 * New image processing operations can be implemented in C or in C++.
 * \par
 * IM is free software, can be used for public and commercial applications.
 * \par
 * This work was developed at Tecgraf/PUC-Rio 
 * by means of the partnership with PETROBRAS/CENPES.
 *
 * \section author Author
 * \par
 * Basic Software Group @ Tecgraf/PUC-Rio
 * - Antonio Scuri scuri@tecgraf.puc-rio.br
 *
 * \section copyright Copyright Notice
\verbatim

****************************************************************************
* Copyright (C) 1994-2006 Tecgraf/PUC-Rio and PETROBRAS S/A.               *
* All rights reserved.                                                     *
*                                                                          *
* Permission is hereby granted, free of charge, to any person obtaining    *
* a copy of this software and associated documentation files (the          *
* "Software"), to deal in the Software without restriction, including      *
* without limitation the rights to use, copy, modify, merge, publish,      *
* distribute, sublicense, and/or sell copies of the Software, and to       *
* permit persons to whom the Software is furnished to do so, subject to    *
* the following conditions:                                                *
*                                                                          *
* The above copyright notice and this permission notice shall be           *
* included in all copies or substantial portions of the Software.          *
*                                                                          *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          *
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF       *
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
****************************************************************************
\endverbatim
 */


/** \defgroup imagerep Image Representation
 * \par
 * See \ref im.h
 */


/** \defgroup file Image Storage
 * \par
 * See \ref im.h
 */


/** \defgroup format File Formats
 * \par
 * See \ref im.h
 *
 * Internal Predefined File Formats:
 * \li "BMP" - Windows Device Independent Bitmap
 * \li "PCX" - ZSoft Picture
 * \li "GIF" - Graphics Interchange Format
 * \li "TIFF" - Tagged Image File Format
 * \li "RAS" - Sun Raster File
 * \li "SGI" - Silicon Graphics Image File Format
 * \li "JPEG" - JPEG File Interchange Format
 * \li "LED" - IUP image in LED
 * \li "TGA" - Truevision Targa
 * \li "RAW" - RAW File
 * \li "PNM" - Netpbm Portable Image Map 
 * \li "ICO" - Windows Icon 
 * \li "PNG" - Portable Network Graphic Format 
 *
 * Other Supported File Formats:
 * \li "JP2" - JPEG-2000 JP2 File Format 
 * \li "AVI" - Windows Audio-Video Interleaved RIFF
 * \li "WMV" -  Windows Media Video Format
 *          
 * Some Known Compressions:
 * \li "NONE" - No Compression.
 * \li "RLE"  - Run Lenght Encoding.
 * \li "LZW"  - Lempel, Ziff and Welsh.
 * \li "JPEG" - Join Photographics Experts Group.
 * \li "DEFLATE" - LZ77 variation (ZIP)
 *          
 * \ingroup file */

 
/* Library Names Convention
 *
 *   Global Functions and Types - "im[Object][Action]"  using first capitals (imFileOpen)
 *   Local Functions and Types  -  "i[Object][Action]"  using first capitals (iTIFFGetCompIndex)
 *   Local Static Variables - same as local functions and types (iFormatCount)
 *   Local Static Tables - same as local functions and types with "Table" suffix (iTIFFCompTable)
 *   Variables and Members - no prefix, all lower case (width)
 *   Defines and Enumerations - all capitals (IM_ERR_NONE)
 *
 */


#endif
