/** \file
 * \brief Register the AVI Format
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format_avi.h,v 1.5 2006/05/16 13:57:17 scuri Exp $
 */

#ifndef __IM_FORMAT_AVI_H
#define __IM_FORMAT_AVI_H

#if	defined(__cplusplus)
extern "C" {
#endif

/** \defgroup avi AVI - Windows Audio-Video Interleaved RIFF
 * \section Description
 * 
 * \par
 * Windows Copyright Microsoft Corporation.
 * \par
 * Access to the AVI format uses Windows AVIFile library. Available in Windows Only. \n
 * When writing a new file you must use an ".avi" extension, or the Windows API will fail. \n
 * You must link the application with "im_avi.lib" 
 * and you must call the function \ref imFormatRegisterAVI once 
 * to register the format into the IM core library. \n
 * Depends also on the VFW library (vfw32.lib).
 * When using the "im_avi.dll" this extra library is not necessary. \n
 * If using Cygwin or MingW must link with "-lvfw32". 
 * Old versions of Cygwin and MingW use the "-lvfw_ms32" and "-lvfw_avi32". 
 * \par
 * See \ref im_format_avi.h
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: RGB, MAP and Binary (Gray saved as MAP)
    Compressions (installed in Windows XP by default):
      NONE     - no compression [default]
      RLE      - Microsoft RLE (8bpp only)
      CINEPACK - Cinepak Codec by Radius
      MSVC     - Microsoft Video 1 (old)
      M261     - Microsoft H.261 Video Codec
      M263     - Microsoft H.263 Video Codec
      I420     - Intel 4:2:0 Video Codec (same as M263)
      IV32     - Intel Indeo Video Codec 3.2 (old)
      IV41     - Intel Indeo Video Codec 4.5 (old)
      IV50     - Intel Indeo Video 5.1
      IYUV     - Intel IYUV Codec
      MPG4     - Microsoft MPEG-4 Video Codec V1 (not MPEG-4 compliant) (old)
      MP42     - Microsoft MPEG-4 Video Codec V2 (not MPEG-4 compliant)
      CUSTOM   - (show compression dialog)
      DIVX     - DivX 5.0.4 Codec (DivX must be installed)
      (others, must be the 4 charaters of the fourfcc code)
    Can have more than one image. 
    Can have an alpha channel (only for RGB)
    Internally the components are always packed.
    Lines arranged from top down to bottom or bottom up to top. But are saved always as bottom up.
    Handle(0) returns NULL. imBinFile is not supported.
    Handle(1) returns PAVIFILE.
    Handle(2) returns PAVISTREAM.
 
    Attributes:
      FPS IM_FLOAT (1) (should set when writing, default 15)
      AVIQuality IM_INT (1) [1-10000, default -1] (write only)
      KeyFrameRate IM_INT (1) (write only) [key frame frequency, if 0 not using key frames, default 15]
      DataRate IM_INT (1) (write only) [kilobits/second, default 2400]

    Comments:
      Reads only the first video stream. Other streams are ignored.
      All the images have the same size, you must call imFileReadImageInfo/imFileWriteImageInfo 
        at least once.
      For codecs comparsion and download go to:
        http://graphics.lcs.mit.edu/~tbuehler/video/codecs/
        http://www.fourcc.org
\endverbatim
 * \ingroup format */
 
/** Register the AVI Format 
 * \ingroup avi */
void imFormatRegisterAVI(void);

#if defined(__cplusplus)
}
#endif

#endif
