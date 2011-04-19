/** \file
 * \brief Register the JP2 Format
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format_jp2.h,v 1.3 2006/05/16 13:57:17 scuri Exp $
 */

#ifndef __IM_FORMAT_JP2_H
#define __IM_FORMAT_JP2_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup jp2 JP2 - JPEG-2000 JP2 File Format
 * \section Description
 * 
 * \par
 * ISO/IEC 15444 (2000, 2003)\n
 * http://www.jpeg.org/
 * \par
 * You must link the application with "im_jp2.lib" 
 * and you must call the function \ref imFormatRegisterJP2 once 
 * to register the format into the IM core library. \n
 * \par
 * Access to the JPEG2000 file format uses libJasper version 1.701.0. \n
 * http://www.ece.uvic.ca/~mdadams/jasper                             \n
 * Copyright (c) 2001-2003 Michael David Adams.
 * \par
 * See \ref im_format_jp2.h
 *
 * \section Features
 *
\verbatim
    Data Types: Byte and UShort
    Color Spaces: Binary, Gray, RGB, YCbCr, Lab and XYZ
    Compressions: 
      JPEG-2000 - ISO JPEG 2000  [default]
    Only one image.
    Can have an alpha channel.
    Internally the components are always unpacked.
    Internally the lines are arranged from top down to bottom.
    Handle(1) returns jas_image_t*
    Handle(2) returns jas_stream_t*
 
    Attributes:
      CompressionRatio IM_FLOAT (1) [write only, example: Ratio=7 just like 7:1]

    Comments:
      We read code stream syntax and JP2, but we write always as JP2.
      Used definitions EXCLUDE_JPG_SUPPORT,EXCLUDE_MIF_SUPPORT,
                       EXCLUDE_PNM_SUPPORT,EXCLUDE_RAS_SUPPORT,
                       EXCLUDE_BMP_SUPPORT,EXCLUDE_PGX_SUPPORT
      Changed jas_config.h to match our needs.
      New file jas_binfile.c
      Changed jas_stream.c to export jas_stream_create and jas_stream_initbuf.
      Changed jp2_dec.c and jpc_cs.c to remove "uint" and "ulong" usage.
\endverbatim
 * \ingroup format */
 
/** Register the JP2 Format 
 * \ingroup jp2 */
void imFormatRegisterJP2(void);


#if defined(__cplusplus)
}
#endif

#endif
