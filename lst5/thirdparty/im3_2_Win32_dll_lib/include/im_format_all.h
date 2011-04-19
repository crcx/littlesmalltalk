/** \file
 * \brief All the Internal File Formats.
 * They are all automatically registered by the library.
 * The signatures are in C, but the functions are C++.
 * Header for internal use only.
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format_all.h,v 1.9 2006/05/16 13:57:17 scuri Exp $
 */

#ifndef __IM_FORMAT_ALL_H
#define __IM_FORMAT_ALL_H

#if	defined(__cplusplus)
extern "C" {
#endif

/** \defgroup tiff TIFF - Tagged Image File Format
 * \section Description
 * 
 * \par
 * Copyright (c) 1986-1988, 1992 by Adobe Systems Incorporated. \n
 * Originally created by a group of companies, 
 * the Aldus Corporation keeped the copyright until Aldus was aquired by Adobe. \n
 * TIFF Revision 6.0 Final — June 3, 1992 \n
 * http://www.adobe.com/Support/TechNotes.html
 * \par
 * Access to the TIFF file format uses libTIFF version 3.7.4 \n
 * http://www.remotesensing.org/libtiff/                     \n
 * Copyright (c) 1988-1997 Sam Leffler                      \n
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.           \n
 *
 * \section Features
 *
\verbatim
    Data Types: <all>
    Color Spaces: Gray, RGB, CMYK, YCbCr, Lab, XYZ, Map and Binary.
    Compressions: 
      NONE - no compression  [default for IEEE Floating Point Data]
      CCITTRLE - CCITT modified Huffman RLE (binary only) [default for Binary]
      CCITTFAX3 - CCITT Group 3 fax         (binary only)
      CCITTFAX4 - CCITT Group 4 fax         (binary only)
      LZW - Lempel-Ziv & Welch  [default]
      JPEG - ISO JPEG    [default for YCBCR] 
      NEXT - NeXT 2-bit RLE (2 bpp only)
      CCITTRLEW - CCITT modified Huffman RLE with word alignment (binary only)
      RLE - Packbits (Macintosh RLE) [default for MAP]
      THUNDERSCAN - ThunderScan 4-bit RLE (only for 2 or 4 bpp)
      PIXARLOG - Pixar companded 11-bit ZIP (only byte, ushort and float)
      DEFLATE - LZ77 variation (ZIP) 
      ADOBE_DEFLATE - Adobe LZ77 variation
      SGILOG - SGI Log Luminance RLE for L and Luv (only byte, ushort and float) [default for XYZ]
      SGILOG24 - SGI Log 24-bit packed for Luv (only byte, ushort and float)
    Can have more than one image.
    Can have an alpha channel.
    Components can be packed or not.
    Lines arranged from top down to bottom or bottom up to top.
    Handle(1) returns a TIFF* libTIFF structure.
 
    Attributes:
      Photometric IM_USHORT (1) (when writing this will complement the color_mode information, for Mask, MinIsWhite, ITULab and ICCLab)
      ExtraSampleInfo IM_USHORT (1) (description of alpha channel: 0- uknown, 1- pre-multiplied, 2-normal)
      JPEGQuality IM_INT (1) [0-100, default 75] (write only)
      ZIPQuality IM_INT (1) [1-9, default 6] (write only)
      ResolutionUnit (string) ["DPC", "DPI"]
      XResolution, YResolution IM_FLOAT (1)
      Description, Author, Copyright, DateTime, DocumentName, 
      PageName, TargetPrinter, Make, Model, Software, HostComputer (string)
      InkNames (strings separated by '0's)
      InkSet IM_USHORT (1)
      NumberOfInks IM_USHORT (1)
      DotRange IM_USHORT (2)
      TransferFunction0, TransferFunction1, TransferFunction3 IM_USHORT [gray=0, rgb=012]
      ReferenceBlackWhite IMFLOAT (6)
      WhitePoint IMFLOAT (2)
      PrimaryChromaticities  IMFLOAT (6)
      YCbCrCoefficients IM_FLOAT (3)
      YCbCrSubSampling IM_USHORT (2)
      YCbCrPositioning IM_USHORT (1)
      PageNumber IM_USHORT (2)
      StoNits IM_FLOAT (1) 
      XPosition, YPosition IM_FLOAT (1)
      SMinSampleValue, SMaxSampleValue IM_FLOAT (1)
      HalftoneHints IM_USHORT (2)
      SubfileType IM_INT (1)
      ICCProfile IM_BYTE (N)
      GeoTiePoints, GeoTransMatrix, IntergraphMatrix, GeoPixelScale, GeoDoubleParams IM_FLOAT (N)
      GeoASCIIParams (string)
      (other attributes can be obtained by using libTIFF directly using the Handle(1) function)

    Comments:
      LogLuv is in fact Y'+CIE(u,v), so we choose to convert to XYZ.
      SubIFD is not handled.
      Since LZW patent expired, LZW compression is enabled. LZW Copyright Unisys.
      libGeoTIFF can be used without XTIFF initialization. Use Handle(1) to obtain a TIFF*.
      
    Changes:
      "tiff_jpeg.c" - commented "downsampled_output = TRUE" in 2 places.
      "tiff_ojpeg.c" for boolean type compilation. 
      New file "tif_config.h" to match our needs.
      New file "tiff_binfile.c" that implement I/O rotines using imBinFile.
\endverbatim
 * \ingroup format */
void imFormatRegisterTIFF(void);

/** \defgroup jpeg JPEG - JPEG File Interchange Format
 * \section Description
 * 
 * \par
 * ISO/IEC 10918 (1994, 1995, 1997, 1999)\n
 * http://www.jpeg.org/
 * \par
 * Access to the JPEG file format uses libJPEG version 6b. \n
 * http://www.ijg.org                                      \n
 * Copyright (C) 1991-1998, Thomas G. Lane                 \n
 *   from the Independent JPEG Group.
 * \par
 * Access to the EXIF attributes uses libEXIF version 0.6.12. \n
 * http://sourceforge.net/projects/libexif                    \n
 * Copyright (C) 2001-2003, Lutz Müller
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: Gray, RGB, CMYK and YCbCr (Binary Saved as Gray)
    Compressions: 
      JPEG - ISO JPEG  [default]
    Only one image.
    No alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from top down to bottom.
    Handle(1) returns jpeg_decompress_struct* when reading, and 
                      jpeg_compress_struct* when writing (libJPEG structures).
 
    Attributes:
      AutoYCbCr IM_INT (1) (controls YCbCr auto conversion) default 1
      JPEGQuality IM_INT (1) [0-100, default 75] (write only)
      ResolutionUnit (string) ["DPC", "DPI"]
      XResolution, YResolution IM_FLOAT (1)
      Interlaced (same as Progressive) IM_INT (1 | 0) default 0  
      Description (string)
      (lots of Exif tags)
 
    Changes to libJPEG:
      jdatadst.c - fflush and ferror replaced by macros JFFLUSH and JFERROR.
      jinclude.h - standard JFFLUSH and JFERROR definitions, and new macro HAVE_JFIO.
      jmorecfg.h - changed definition of INT32 to JINT32 for better compatibility.
      jdhuf.c - added support for OJPEG_SUPPORT in libTIFF.
      new file created: jconfig.h
 
    Changes to libEXIF:
      new file config.h
      changed "exif-tag.c" to add new function
      changed "exif-entry.c" to improve exif_entry_initialize
      fixed small bug in "mnote-pentax-tag.h".

    Comments:
      Other APPx markers are ignored.
      No thumbnail support.
      RGB images are automatically converted to YCbCr when saved. 
      Also YcbCr are converted to RGB when loaded. Use AutoYCbCr=0 to disable this behavior. 
\endverbatim
 * \ingroup format */
void imFormatRegisterJPEG(void);

/** \defgroup png PNG - Portable Network Graphic Format
 * \section Description
 * 
 * \par
 * Access to the PNG file format uses libPNG version 1.2.8. \n
 * http://www.libpng.org                                    \n
 * Copyright (C) 1998-2004 Glenn Randers-Pehrson
 *
 * \section Features
 *
\verbatim
    Data Types: Byte and UShort
    Color Spaces: Gray, RGB, MAP and Binary
    Compressions: 
      DEFLATE - LZ77 variation (ZIP) [default]
    Only one image.
    Can have an alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from top down to bottom.
    Handle(1) returns png_structp libPNG structure.
 
    Attributes:
      ZIPQuality IM_INT (1) [1-9, default 6] (write only)
      ResolutionUnit (string) ["DPC", "DPI"]
      XResolution, YResolution IM_FLOAT (1)
      Interlaced (same as Progressive) IM_INT (1 | 0) default 0 
      Gamma IM_FLOAT (1)
      WhitePoint IMFLOAT (2)
      PrimaryChromaticities  IMFLOAT (6)
      XPosition, YPosition IM_FLOAT (1)
      sRGBIntent IM_INT (1) [0: Perceptual, 1: Relative colorimetric, 2: Saturation, 3: Absolute colorimetric]
      TransparencyIndex IM_BYTE (1 or N)
      TransparentColor IM_BYTE (3)
      CalibrationName, CalibrationUnits (string)
      CalibrationLimits IM_INT (2) 
      CalibrationEquation IM_BYTE (1) [0-Linear,1-Exponential,2-Arbitrary,3-HyperbolicSine)]
      CalibrationParam (string) [params separated by '\\n']
      Title, Author, Description, Copyright, DateTime (string)
      Software, Disclaimer, Warning, Source, Comment, ...       (string)
      DateTimeModified (string) [when writing uses the current system time]
      ICCProfile IM_BYTE (N)
      ScaleUnit (string) ["meters", "radians"]
      XScale, YScale IM_FLOAT (1)

    Comments:
      Attributes after the image are ignored.
      Define PNG_NO_CONSOLE_IO to avoid printfs.
      We define PNG_TIME_RFC1123_SUPPORTED.
      Add the following files to the makefile to optimize the library:
      pngvcrd.c  - PNG_USE_PNGVCRD
                   For Intel x86 CPU and Microsoft Visual C++ compiler
      pnggccrd.c - PNG_USE_PNGGCCRD
                   For Intel x86 CPU (Pentium-MMX or later) and GNU C compiler.
      Changed pngconf.h to use int instead of long in png_uint_32 and png_int_32.
\endverbatim
 * \ingroup format */
void imFormatRegisterPNG(void);

/** \defgroup gif GIF - Graphics Interchange Format
 * \section Description
 * 
 * \par
 * Copyright (c) 1987,1988,1989,1990 CompuServe Incorporated. \n
 * GIF is a Service Mark property of CompuServe Incorporated. \n
 * Graphics Interchange Format Programming Reference, 1990. \n
 * LZW Copyright Unisys.
 * \par
 * Patial Internal Implementation. \n
 * Decoding and encoding code were extracted from GIFLib 1.0. \n
 * Copyright (c) 1989 Gershon Elber.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: MAP only, (Gray and Binary saved as MAP)
    Compressions: 
      LZW - Lempel-Ziv & Welch      [default]
    Can have more than one image.
    No alpha channel.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      ScreenHeight, ScreenWidth IM_USHORT (1) screen size [default to the first image size]
      Interlaced IM_INT (1 | 0) default 0 
      Description (string)
      TransparencyIndex IM_BYTE (1)
      XScreen, YScreen IM_USHORT (1) screen position
      UserInput IM_BYTE (1) [1, 0]
      Disposal (string) [UNDEF, LEAVE, RBACK, RPREV]
      Delay IM_USHORT (1)
      Iterations IM_USHORT (1) (NETSCAPE2.0 Application Extension)

    Comments:
      Attributes after the last image are ignored.
      Reads GIF87 and GIF89, but writes GIF89 always.
      Ignored attributes: Background Color Index, Pixel Aspect Ratio, 
                          Plain Text Extensions, Application Extensions...
\endverbatim
 * \ingroup format */
void imFormatRegisterGIF(void);

/** \defgroup bmp BMP - Windows Device Independent Bitmap
 * \section Description
 * 
 * \par
 * Windows Copyright Microsoft Corporation.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: RGB, MAP and Binary (Gray saved as MAP)
    Compressions: 
      NONE - no compression [default]
      RLE  - Run Lenght Encoding (only for MAP and Gray)
    Only one image.
    Can have an alpha channel (only for RGB)
    Internally the components are always packed.
    Lines arranged from top down to bottom or bottom up to top. But are saved always as bottom up.
 
    Attributes:
      ResolutionUnit (string) ["DPC", "DPI"]
      XResolution, YResolution IM_FLOAT (1)

    Comments:
      Reads OS2 1.x and Windows 3, but writes Windows 3 always.
      Version 4 and 5 BMPs are not supported.
\endverbatim
 * \ingroup format */
void imFormatRegisterBMP(void);

/** \defgroup ras RAS - Sun Raster File
 * \section Description
 * 
 * \par
 * Copyright Sun Corporation.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: Gray, RGB, MAP and Binary
    Compressions: 
      NONE - no compression   [default]
      RLE  - Run Lenght Encoding
    Only one image.
    Can have an alpha channel (only for IM_RGB)
    Internally the components are always packed.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      none
\endverbatim
 * \ingroup format */
void imFormatRegisterRAS(void);

/** \defgroup led LED - IUP image in LED
 * \section Description
 * 
 * \par
 * Copyright Tecgraf/PUC-Rio and PETROBRAS/CENPES.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: MAP only (Gray and Binary saved as MAP)
    Compressions: 
      NONE - no compression  [default]
    Only one image. 
    No alpha channel.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      none

    Comments:
      LED file must start with "LEDImage = IMAGE[".
\endverbatim
 * \ingroup format */
void imFormatRegisterLED(void);

/** \defgroup sgi SGI - Silicon Graphics Image File Format
 * \section Description
 * 
 * \par
 * SGI is a trademark of Silicon Graphics, Inc.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte and UShort
    Color Spaces: Gray and RGB (Binary saved as Gray, MAP with fixed palette when reading only)
    Compressions: 
      NONE - no compression  [default]
      RLE  - Run Lenght Encoding
    Only one image.
    Can have an alpha channel (only for IM_RGB)
    Internally the components are always packed.
    Internally the lines are arranged from bottom up to top.
 
    Attributes:
      Description (string)
\endverbatim
 * \ingroup format */
void imFormatRegisterSGI(void);

/** \defgroup pcx PCX - ZSoft Picture
 * \section Description
 * 
 * \par
 * Copyright ZSoft Corporation. \n
 * ZSoft (1988) PCX Technical Reference Manual.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: RGB, MAP and Binary (Gray saved as MAP)
    Compressions: 
      NONE - no compression 
      RLE  - Run Lenght Encoding [default - since uncompressed PCX is not well supported]
    Only one image.
    No alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      ResolutionUnit (string) ["DPC", "DPI"]
      XResolution, YResolution IM_FLOAT (1)
      XScreen, YScreen IM_USHORT (1) screen position

    Comments:
      Reads Versions 0-5, but writes Version 5 always.
\endverbatim
 * \ingroup format */
void imFormatRegisterPCX(void);

/** \defgroup tga TGA - Truevision Graphics Adapter File
 * \section Description
 * 
 * \par
 * Truevision TGA File Format Specification Version 2.0 \n
 * Technical Manual Version 2.2 January, 1991           \n
 * Copyright 1989, 1990, 1991 Truevision, Inc.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Supports 8 bits per component only. Data type is always Byte.
    Color Spaces: Gray, RGB and MAP (Binary saved as Gray)
    Compressions: 
      NONE - no compression [default]
      RLE  - Run Lenght Encoding
    Only one image.
    No alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from bottom up to top or from top down to bottom.
 
    Attributes:
      XScreen, YScreen IM_USHORT (1) screen position
      Title, Author, Description, JobName, Software (string)
      SoftwareVersion (read only) (string)
      DateTimeModified (string) [when writing uses the current system time]
      Gamma IM_FLOAT (1)
\endverbatim
 * \ingroup format */
void imFormatRegisterTGA(void);

/** \defgroup pnm PNM - Netpbm Portable Image Map
 * \section Description
 * 
 * \par
 * PNM formats Copyright Jef Poskanzer
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte and UShort
    Color Spaces: Gray, RGB and Binary
    Compressions: 
      NONE - no compression [default]
      ASCII (textual data)
    Can have more than one image, but sequencial access only.
    No alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      Description (string)

    Comments:
      In fact ASCII is an expansion...
\endverbatim
 * \ingroup format */
void imFormatRegisterPNM(void);

/** \defgroup ico ICO - Windows Icon
 * \section Description
 * 
 * \par
 * Windows Copyright Microsoft Corporation.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte
    Color Spaces: RGB, MAP and Binary (Gray saved as MAP)
    Compressions: 
      NONE - no compression [default]
    Can have more than one image. But writing is limited to 5 images,
      and all images must have different sizes and bpp.
    No alpha channel.
    Internally the components are always packed.
    Internally the lines are arranged from bottom up to top.
 
    Attributes:
      TransparencyIndex IM_BYTE (1 or N)

    Comments:
      If the user specifies an alpha channel, the AND mask is loaded as alpha,
        but the file color mode will not contain the IM_ALPHA flag.
      For IM_MAP imagens, if the user does not specifies an alpha channel 
        the TransparencyIndex is used to initialize the AND mask when writing,
        and the most repeated index with transparency will be the transparent index.
      Although any size and bpp can be used is recomended to use the typical configurations:
        16x16, 32x32, 48x48, 64x64 or 96x96
        2 colors, 16 colors or 256 colors
\endverbatim
 * \ingroup format */
void imFormatRegisterICO(void);

/** \defgroup krn KRN - IM Kernel File Format
 * \section Description
 * 
 * \par
 * Textual format to provied a simple way to create kernel convolution images.
 * \par
 * Internal Implementation.
 *
 * \section Features
 *
\verbatim
    Data Types: Byte, Int
    Color Spaces: Gray
    Compressions: 
      NONE - no compression [default]
    Only one image.
    No alpha channel.
    Internally the lines are arranged from top down to bottom.
 
    Attributes:
      Description (string)

    Comments:
      The format is very simple, inspired by PNM.
      It was developed because PNM does not have support for INT and FLOAT.
      Remeber that usually convolution operations use kernel size an odd number.

    Format Model:
      IMKERNEL
      Description up to 512 characters
      width height
      type (0 - IM_INT, 1 - IM_FLOAT)
      data...

    Example:
      IMKERNEL
      Gradian
      3 3
      0
      0 -1 0     
      0  1 0  
      0  0 0  
\endverbatim
 * \ingroup format */
void imFormatRegisterKRN(void);


#if defined(__cplusplus)
}
#endif

#endif
