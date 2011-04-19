/** \file
 * \brief Register the WMF Format
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format_wmv.h,v 1.3 2006/05/16 13:57:17 scuri Exp $
 */

#ifndef __IM_FORMAT_WMV_H
#define __IM_FORMAT_WMV_H

#if	defined(__cplusplus)
extern "C" {
#endif

/** \defgroup wmv WMV - Windows Media Video Format
 * \section Description
 * 
 * \par
 * Advanced Systems Format (ASF) \n
 * Windows Copyright Microsoft Corporation.
 * \par
 * Access to the WMV format uses Windows Media SDK. Available in Windows Only. \n
 * You must link the application with "im_wmv.lib" 
 * and you must call the function \ref imFormatRegisterWMV once 
 * to register the format into the IM core library. \n
 * Depends also on the WMF SDK (wmvcore.lib).
 * When using the "im_wmv.dll" this extra library is not necessary.
 * \par
 * The application users should have the WMV codec 9 installed:
 * http://www.microsoft.com/windows/windowsmedia/format/codecdownload.aspx
 * \par
 * You must agree with the WMF SDK EULA to use the SDK. \n
 * http://wmlicense.smdisp.net/v9sdk/
 * \par
 * For more information: \n
 * http://www.microsoft.com/windows/windowsmedia/9series/sdk.aspx \n
 * http://msdn.microsoft.com/library/en-us/wmform/htm/introducingwindowsmediaformat.asp 
 * \par
 * See \ref im_format_wmv.h
 * 
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: RGB and MAP (Gray and Binary saved as MAP)
    Compressions (installed in Windows XP by default):
      NONE       - no compression
      MPEG-4v3   - Windows Media MPEG-4 Video V3
      MPEG-4v1   - ISO MPEG-4 Video V1
      WMV7       - Windows Media Video  V7
      WMV7Screen - Windows Media Screen V7
      WMV8       - Windows Media Video  V8
      WMV9Screen - Windows Media Video 9 Screen
      WMV9       - Windows Media Video 9 [default]
      Unknown    - Others
    Can have more than one image. 
    Can have an alpha channel (only for RGB) ?
    Internally the components are always packed.
    Lines arranged from top down to bottom or bottom up to top.
    Handle(0) return NULL. imBinFile is not supported.
    Handle(1) returns IWMSyncReader* when reading, IWMWriter* when writing.
 
    Attributes:
      FPS IM_FLOAT (1) (should set when writing, default 15)
      WMFQuality IM_INT (1) [0-100, default 50] (write only)
      MaxKeyFrameTime IM_INT (1) (write only) [maximum key frame interval in miliseconds, default 5 seconds]
      DataRate IM_INT (1) (write only) [kilobits/second, default 2400]
      VBR IM_INT (1) [0, 1] (write only) [0 - Constant Bit Rate (default), 1 - Variable Bit Rate (Quality-Based)]
      (and several others from the file-level attributes) For ex:
        Title, Author, Copyright, Description (string)
        Duration IM_INT [100-nanosecond units]
        Seekable, HasAudio, HasVideo, Is_Protected, Is_Trusted, IsVBR IM_INT (1) [0, 1]
        NumberOfFrames IM_INT (1)

    Comments:
      IMPORTANT - The "image_count" and the "FPS" attribute may not be available from the file,
        we try to estimate from the duration and from the average time between frames, or using the default value.
      We do not handle DRM protected files (Digital Rights Management).
      Reads only the first video stream. Other streams are ignored.
      All the images have the same size, you must call imFileReadImageInfo/imFileWriteImageInfo 
        at least once.
      For optimal random reading, the file should be indexed previously.
      If not indexed by frame, random positioning may not be precise.
      Sequencial reading will always be precise.
      When writing we use a custom profile and time indexing only.
      We do not support multipass encoding.  
      Since the driver uses COM, CoInitialize(NULL) and CoUninitialize() are called every Open/Close.
\endverbatim
 * \ingroup format */
 
/** Register the WMF Format 
 * \ingroup wmv */
void imFormatRegisterWMV(void);
               
               
#if defined(__cplusplus)
}
#endif

#endif
