/** \file
 * \brief Utilities
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_util.h,v 1.5 2005/12/12 00:18:29 scuri Exp $
 */

#ifndef __IM_UTIL_H
#define __IM_UTIL_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup util Utilities
 * \par
 * See \ref im_util.h
 * @{
 */

#define IM_MIN(_a, _b) (_a < _b? _a: _b)
#define IM_MAX(_a, _b) (_a > _b? _a: _b)

/** @} */


/** \defgroup str String Utilities
 * \par
 * See \ref im_util.h
 * \ingroup util */

/** Check if the two strings are equal.
 * \ingroup str */
int imStrEqual(const char* str1, const char* str2);

/** Calculate the size of the string but limited to max_len.
 * \ingroup str */
int imStrNLen(const char* str, int max_len);

/** Check if the data is a string.
 * \ingroup str */
int imStrCheck(const void* data, int count);



/** \defgroup imageutil Raw Data Utilities
 * \par
 * See \ref im_util.h
 * \ingroup imagerep */

/** Returns the size of the data buffer.
 *
 * \verbatim im.ImageDataSize(width: number, height: number, color_mode: number, data_type: number) -> datasize: number [in Lua 5] \endverbatim
 * \ingroup imageutil */
int imImageDataSize(int width, int height, int color_mode, int data_type);

/** Returns the size of one line of the data buffer. \n
 * This depends if the components are packed. If packed includes all components, if not includes only one.
 *
 * \verbatim im.ImageLineSize(width: number, color_mode: number, data_type: number) -> linesize: number [in Lua 5] \endverbatim
 * \ingroup imageutil */
int imImageLineSize(int width, int color_mode, int data_type);

/** Returns the number of elements of one line of the data buffer. \n
 * This depends if the components are packed. If packed includes all components, if not includes only one.
 *
 * \verbatim im.ImageLineCount(width: number, color_mode: number) -> linecount: number [in Lua 5] \endverbatim
 * \ingroup imageutil */
int imImageLineCount(int width, int color_mode);

/** Check if the combination color_mode+data_type is valid.
 *
 * \verbatim im.ImageCheckFormat(color_mode: number, data_type: number) -> check: boolean [in Lua 5] \endverbatim
 * \ingroup imageutil */
int imImageCheckFormat(int color_mode, int data_type);



/** \defgroup colorutl Color Utilities
 * \par
 * See \ref im_util.h
 * \ingroup util */

/** Encode RGB components in a long for palete usage. \n
 * "long" definition is compatible with the CD library definition.
 *
 * \verbatim im.ColorEncode(red: number, green: number, blue: number) -> color: lightuserdata [in Lua 5] \endverbatim
 * \ingroup colorutl */
long imColorEncode(unsigned char red, unsigned char green, unsigned char blue);

/** Decode RGB components from a long for palete usage. \n
 * "long" definition is compatible with the CD library definition.
 *
 * \verbatim im.ColorDecode(color: lightuserdata) -> red: number, green: number, blue: number [in Lua 5] \endverbatim
 * \ingroup colorutl */
void imColorDecode(unsigned char *red, unsigned char *green, unsigned char *blue, long color);



/** \defgroup colormodeutl Color Mode Utilities
 * \par
 * See \ref im_util.h
 * \ingroup imagerep */

/** Returns the color mode name.
 *
 * \verbatim im.ColorModeSpaceName(color_mode: number) -> name: string [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
const char* imColorModeSpaceName(int color_mode);

/** Returns the number of components of the color space including alpha.
 *
 * \verbatim im.ColorModeDepth(color_mode: number) -> depth: number [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
int imColorModeDepth(int color_mode);

/** Returns the color space of the color mode.
 *
 * \verbatim im.ColorModeSpace(color_mode: number) -> color_space: number [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
#define imColorModeSpace(_cm) (_cm & 0xFF)

/** Check if the two color modes match. Only the color space is compared.
 *
 * \verbatim im.ColorModeMatch(color_mode1: number, color_mode2: number) -> match: boolean [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
#define imColorModeMatch(_cm1, _cm2) (imColorModeSpace(_cm1) == imColorModeSpace(_cm2))

/** Check if the color mode has an alpha channel.
 *
 * \verbatim im.ColorModeHasAlpha(color_mode: number) -> has_alpha: boolean [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
#define imColorModeHasAlpha(_cm) (_cm & IM_ALPHA)

/** Check if the color mode components are packed in one plane.
 *
 * \verbatim im.ColorModeIsPacked(color_mode: number) -> is_packed: boolean [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
#define imColorModeIsPacked(_cm) (_cm & IM_PACKED)

/** Check if the color mode orients the image from top down to bottom.
 *
 * \verbatim im.ColorModeIsTopDown(color_mode: number) -> is_top_down: boolean [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
#define imColorModeIsTopDown(_cm) (_cm & IM_TOPDOWN)

/** Returns the color space of the equivalent display bitmap image. \n
 * Original packing and alpha are ignored. Returns IM_RGB, IM_GRAY, IM_MAP or IM_BINARY.
 *
 * \verbatim im.ColorModeToBitmap(color_mode: number) -> color_space: number [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
int imColorModeToBitmap(int color_mode);

/** Check if the color mode and data_type defines a display bitmap image.
 *
 * \verbatim im.ColorModeIsBitmap(color_mode: number, data_type: number) -> is_bitmap: boolean [in Lua 5] \endverbatim
 * \ingroup colormodeutl */
int imColorModeIsBitmap(int color_mode, int data_type);



/** \defgroup datatypeutl Data Type Utilities
 * \par
 * See \ref im_util.h
 * \ingroup util
 * @{
 */

typedef unsigned char imbyte;
typedef unsigned short imushort;

#define IM_BYTECROP(_v) (_v < 0? 0: _v > 255? 255: _v)
#define IM_CROPMAX(_v, _max) (_v < 0? 0: _v > _max? _max: _v)

/** @} */

/** Returns the size in bytes of a specified numeric data type.
 *
 * \verbatim im.DataTypeSize(data_type: number) -> size: number [in Lua 5] \endverbatim
 * \ingroup datatypeutl */
int imDataTypeSize(int data_type);

/** Returns the numeric data type name given its identifier.
 *
 * \verbatim im.DataTypeName(data_type: number) -> name: string [in Lua 5] \endverbatim
 * \ingroup datatypeutl */
const char* imDataTypeName(int data_type);

/** Returns the maximum value of an integer data type. For floating point returns 0.
 *
 * \verbatim im.DataTypeIntMax(data_type: number) -> int_max: number [in Lua 5] \endverbatim
 * \ingroup datatypeutl */
unsigned long imDataTypeIntMax(int data_type);

/** Returns the minimum value of an integer data type. For floating point returns 0. 
 *
 * \verbatim im.DataTypeIntMin(data_type: number) -> int_min: number [in Lua 5] \endverbatim
 * \ingroup datatypeutl */
long imDataTypeIntMin(int data_type);
           


/** \defgroup bin Binary Data Utilities
 * \par
 * See \ref im_util.h
 * \ingroup util */

/** CPU Byte Orders. 
  * \ingroup bin */
enum imByteOrder
{
  IM_LITTLEENDIAN, /**< Little Endian - The most significant byte is on the right end of a word. Used by Intel processors. */
  IM_BIGENDIAN     /**< Big Endian - The most significant byte is on the left end of a word. Used by Motorola processors, also is the network standard byte order. */
};

/** Returns the current CPU byte order.
 * \ingroup bin */
int imBinCPUByteOrder(void);

/** Changes the byte order of an array of 2, 4 or 8 byte values.
 * \ingroup bin */
void imBinSwapBytes(void *data, int count, int size);

/** Changes the byte order of an array of 2 byte values.
 * \ingroup bin */
void imBinSwapBytes2(void *data, int count);

/** Inverts the byte order of the 4 byte values 
 * \ingroup bin */
void imBinSwapBytes4(void *data, int count);
            
/** Inverts the byte order of the 8 byte values 
 * \ingroup bin */
void imBinSwapBytes8(void *data, int count);



/** \defgroup compress Data Compression Utilities
 * \par
 * Deflate compression support uses zlib version 1.2.3.     \n
 * http://www.zlib.org/                                      \n
 * Copyright (C) 1995-2004 Jean-loup Gailly and Mark Adler
 * \par
 * LZF compression support uses libLZF version 1.51.     \n
 * http://liblzf.plan9.de/                                \n
 * Copyright (C) 2000-2005 Marc Alexander Lehmann
 * See \ref im_util.h
 * \ingroup util */

/** Compresses the data using the ZLIB Deflate compression. \n
 * The destination buffer must be at least 0.1% larger than source_size plus 12 bytes. \n
 * It compresses raw byte data. zip_quality can be 1 to 9. \n
 * Returns the size of the compressed buffer or zero if failed.
 * \ingroup compress */
int imCompressDataZ(const void* src_data, int src_size, void* dst_data, int dst_size, int zip_quality);

/** Uncompresses the data compressed with the ZLIB Deflate compression. \n
 * Returns zero if failed.
 * \ingroup compress */
int imCompressDataUnZ(const void* src_data, int src_size, void* dst_data, int dst_size);

/** Compresses the data using the libLZF compression. \n
 * Returns the size of the compressed buffer or zero if failed.
 * \ingroup compress */
int imCompressDataLZF(const void* src_data, int src_size, void* dst_data, int dst_size, int zip_quality);

/** Uncompresses the data compressed with the libLZF compression.
 * Returns zero if failed.
 * \ingroup compress */
int imCompressDataUnLZF(const void* src_data, int src_size, void* dst_data, int dst_size);


#if defined(__cplusplus)
}
#endif

#endif 
