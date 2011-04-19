/** \file
 * \brief Initialize the RAW Format Driver
 * Header for internal use only.
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format_raw.h,v 1.2 2006/05/16 13:57:17 scuri Exp $
 */

#ifndef __IM_FORMAT_RAW_H
#define __IM_FORMAT_RAW_H

#if	defined(__cplusplus)
extern "C" {
#endif

/** \defgroup raw RAW - RAW File
 * 
 * \par
 * The file must be open/created with the functions \ref imFileOpenRaw and \ref imFileNewRaw.
 * 
 * \section Description
 * 
 * \par
 * Internal Implementation.
 * \par
 * Supports RAW binary images. You must know image parameters a priori. \n
 * You must set the IM_INT attributes "Width", "Height", "ColorMode", "DataType" before the imFileReadImageInfo/imFileWriteImageInfo functions.
 * \par
 * The data must be in binary form, but can start in an arbitrary offset from the begining of the file, use attribute "StartOffset".
 * The default is at 0 offset. 
 * \par
 * Integer sign and double precision can be converted using attribute "SwitchType". \n
 * The conversions will be BYTE<->CHAR, USHORT<->SHORT, INT<->UINT, FLOAT<->DOUBLE.
 * \par
 * Byte Order can be Little Endian (Intel=1) or Big Endian (Motorola=0), use the attribute "ByteOrder", the default is the current CPU.
 * \par
 * The lines can be aligned to a BYTE (1), WORD (2) or DWORD (4) boundaries, ue attribute "Padding" with the respective value.
 * \par
 * See \ref im_raw.h
 * 
 * \section Features
 *
\verbatim
    Data Types: <all>
    Color Spaces: all, except MAP.
    Compressions: 
      NONE - no compression 
    Can have more than one image, depends on "StartOffset" attribute.
    Can have an alpha channel.
    Components can be packed or not.
    Lines arranged from top down to bottom or bottom up to top.

    Attributes:
      Width, Height, ColorMode, DataType IM_INT (1)
      StartOffset, SwitchType, ByteOrder, Padding  IM_INT (1)
\endverbatim
 * \ingroup format */
imFormat* imFormatInitRAW(void);


#if defined(__cplusplus)
}
#endif

#endif
