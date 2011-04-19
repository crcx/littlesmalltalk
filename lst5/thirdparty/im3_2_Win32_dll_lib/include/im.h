/** \file
 * \brief Main API
 *
 * See Copyright Notice in im_lib.h
 * $Id: im.h,v 1.10 2006/11/22 19:55:32 scuri Exp $
 */

#ifndef __IM_H
#define __IM_H

#if	defined(__cplusplus)
extern "C" {
#endif
      

/** Image data type descriptors. \n
 * See also \ref datatypeutl.
 * \ingroup imagerep */
enum imDataType
{
  IM_BYTE,   /**< "unsigned char". 1 byte from 0 to 255.                  */
  IM_USHORT, /**< "unsigned short". 2 bytes from 0 to 65,535.             */
  IM_INT,    /**< "int". 4 bytes from -2,147,483,648 to 2,147,483,647.    */
  IM_FLOAT,  /**< "float". 4 bytes single precision IEEE floating point.  */
  IM_CFLOAT  /**< complex "float". 2 float values in sequence, real and imaginary parts.   */
};

/** Image color mode color space descriptors (first byte). \n
 * See also \ref colormodeutl.
 * \ingroup imagerep */
enum imColorSpace
{
  IM_RGB,    /**< Red, Green and Blue (nonlinear).              */
  IM_MAP,    /**< Indexed by RGB color map (data_type=IM_BYTE). */
  IM_GRAY,   /**< Shades of gray, luma (nonlinear Luminance), or an intensity value that is not related to color. */
  IM_BINARY, /**< Indexed by 2 colors: black (0) and white (1) (data_type=IM_BYTE).     */
  IM_CMYK,   /**< Cian, Magenta, Yellow and Black (nonlinear).                          */
  IM_YCBCR,  /**< ITU-R 601 Y'CbCr. Y' is luma (nonlinear Luminance).                   */
  IM_LAB,    /**< CIE L*a*b*. L* is Lightness (nonlinear Luminance, nearly perceptually uniform). */
  IM_LUV,    /**< CIE L*u*v*. L* is Lightness (nonlinear Luminance, nearly perceptually uniform). */
  IM_XYZ     /**< CIE XYZ. Linear Light Tristimulus, Y is linear Luminance.             */
};

/** Image color mode configuration/extra descriptors (1 bit each in the second byte). \n
 * See also \ref colormodeutl.
 * \ingroup imagerep */
enum imColorModeConfig
{
  IM_ALPHA    = 0x100,  /**< adds an Alpha channel */
  IM_PACKED   = 0x200,  /**< packed components (rgbrgbrgb...) */
  IM_TOPDOWN  = 0x400   /**< orientation from top down to bottom */
};



/** File Access Error Codes
 * \ingroup file */
enum imErrorCodes	
{
  IM_ERR_NONE,     /**< No error. */
  IM_ERR_OPEN,     /**< Error while opening the file (read or write). */
  IM_ERR_ACCESS,   /**< Error while accessing the file (read or write). */
  IM_ERR_FORMAT,   /**< Invalid or unrecognized file format. */
  IM_ERR_DATA,     /**< Invalid or unsupported data. */
  IM_ERR_COMPRESS, /**< Invalid or unsupported compression. */
  IM_ERR_MEM,      /**< Insuficient memory */
  IM_ERR_COUNTER   /**< Interrupted by the counter */
};

/* Internal Image File Structure. */
typedef struct _imFile imFile;

/** Opens the file for reading. It must exists. Also reads file header.
 * It will try to identify the file format.
 * See also \ref imErrorCodes.
 *
 * \verbatim im.FileOpen(file_name: string) -> ifile: imFile, error: number [in Lua 5] \endverbatim
 * \ingroup file */
imFile* imFileOpen(const char* file_name, int *error);

/** Opens the file for reading using a specific format. It must exists. Also reads file header.
 * See also \ref imErrorCodes and \ref format.
 *
 * \verbatim im.FileOpenAs(file_name, format: string) -> ifile: imFile, error: number [in Lua 5] \endverbatim
 * \ingroup file */
imFile* imFileOpenAs(const char* file_name, const char* format, int *error);

/** Creates a new file for writing using a specific format. If the file exists will be replaced. \n
 * It will only initialize the format driver and create the file, no data is actually written.
 * See also \ref imErrorCodes and \ref format.
 *
 * \verbatim im.FileNew(file_name: string, format: string) -> ifile: imFile, error: number [in Lua 5] \endverbatim
 * \ingroup file */
imFile* imFileNew(const char* file_name, const char* format, int *error);

/** Closes the file
 *
 * \verbatim im.FileClose(ifile: imFile) [in Lua 5] \endverbatim
 * \verbatim ifile:Close() [in Lua 5] \endverbatim
 * \ingroup file */
void imFileClose(imFile* ifile);  

/** Returns an internal handle.
 * index=0 returns always an imBinFile* handle, 
 * but for some formats returns NULL because they do not use imBinFile (like AVI and WMV).
 * index=1 return an internal structure used by the format, usually is a handle 
 * to a third party library structure. This is file format dependent.
 *
 * \verbatim ifile:Handle() -> handle: userdata [in Lua 5] \endverbatim
 * \ingroup file */
void* imFileHandle(imFile* ifile, int index);  

/** Returns file information. 
 *  image_count is the number of images in a stack or 
 *  the number of frames in a video/animation or the depth of a volume data. \n
 *  compression and image_count can be NULL.
 * See also \ref format.
 *
 * \verbatim ifile:GetInfo() -> format: string, compression: string, image_count: number [in Lua 5] \endverbatim
 * \ingroup file */
void imFileGetInfo(imFile* ifile, char* format, char* compression, int *image_count);

/** Changes the write compression method. \n
 * If the compression is not supported will return an error code when writting. \n
 * Use NULL to set the default compression. You can use the imFileGetInfo to retrieve the actual compression
 * but only after \ref imFileWriteImageInfo. Only a few formats allow you to change the compression between frames.
 *
 * \verbatim ifile:SetInfo(compression: string) [in Lua 5] \endverbatim
 * \ingroup file */
void imFileSetInfo(imFile* ifile, const char* compression);

/** Changes an extended attribute. \n
 * The data will be internally duplicated. \n
 * If data is NULL the attribute is removed.
 * See also \ref imDataType.
 *
 * \verbatim ifile:SetAttribute(attrib: string, data_type: number, data: table of numbers or string) [in Lua 5] \endverbatim
 * If data_type is IM_BYTE, as_string can be used as data.
 * \ingroup file */
void imFileSetAttribute(imFile* ifile, const char* attrib, int data_type, int count, const void* data);

/** Returns an extended attribute. \n
 * Returns NULL if not found. data_type and count can be NULL.
 * See also \ref imDataType.
 *
 * \verbatim ifile:GetAttribute(attrib: string, [as_string: boolean]) -> data: table of numbers or string, data_type: number [in Lua 5] \endverbatim
 * If data_type is IM_BYTE, as_string can be used to return a string instead of a table.
 * \ingroup file */
const void* imFileGetAttribute(imFile* ifile, const char* attrib, int *data_type, int *count);

/** Returns a list of the attribute names. \n
 * "attrib" must contain room enough for "attrib_count" names. Use "attrib=NULL" to return only the count.
 *
 * \verbatim ifile:GetAttributeList() -> data: table of strings [in Lua 5] \endverbatim
 * \ingroup file */
void imFileGetAttributeList(imFile* ifile, char** attrib, int *attrib_count);

/** Returns the pallete if any. \n
 * "palette" must be a 256 colors alocated array. \n
 * Returns zero in "palette_count" if there is no palette. "palette_count" is >0 and <=256.
 *
 * \verbatim ifile:GetPalette() -> palette: imPalette [in Lua 5] \endverbatim
 * \ingroup file */
void imFileGetPalette(imFile* ifile, long* palette, int *palette_count);

/** Changes the pallete. \n
 *  "palette_count" is >0 and <=256.
 *
 * \verbatim ifile:SetPalette(palette: imPalette) [in Lua 5] \endverbatim
 * \ingroup file */
void imFileSetPalette(imFile* ifile, long* palette, int palette_count);

/** Reads the image header if any and returns image information. \n
 * Reads also the extended image attributes, so other image attributes will be available only after calling this function. \n
 * Returns an error code.
 * index specifies the image number between 0 and image_count-1. \n
 * Some drivers reads only in sequence, so "index" can be ignored by the format driver. \n
 * Any parameters can be NULL. This function must be called at least once, check each format documentation.
 * See also \ref imErrorCodes, \ref imDataType, \ref imColorSpace and \ref imColorModeConfig.
 *
 * \verbatim ifile:ReadImageInfo([index: number]) -> error: number, width: number, height: number, file_color_mode: number, file_data_type: number [in Lua 5] \endverbatim
 * Default index is 0.
 * \ingroup file */
int imFileReadImageInfo(imFile* ifile, int index, int *width, int *height, int *file_color_mode, int *file_data_type);

/** Writes the image header. Writes the file header at the first time it is called.
 * Writes also the extended image attributes. \n
 * Must call imFileSetPalette and set other attributes before calling this function. \n
 * In some formats the color space will be converted to match file format specification. \n
 * Returns an error code. This function must be called at least once, check each format documentation.
 * See also \ref imErrorCodes, \ref imDataType, \ref imColorSpace and \ref imColorModeConfig.
 *
 * \verbatim ifile:WriteImageInfo(width: number, height: number, user_color_mode: number, user_data_type: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup file */
int imFileWriteImageInfo(imFile* ifile, int width, int height, int user_color_mode, int user_data_type);

/** Reads the image data with or without conversion. \n
 * The data can be converted to bitmap when reading.
 * Data type conversion to byte will always scan for min-max then scale to 0-255, 
 * except integer values that min-max are already between 0-255. Complex to real conversions will use the magnitude. \n
 * Color mode flags contains packed, alpha and top-botttom information.
 * If flag is 0 means unpacked, no alpha and bottom up. If flag is -1 the file original flags are used. \n
 * Returns an error code.
 * See also \ref imErrorCodes, \ref imDataType, \ref imColorSpace and \ref imColorModeConfig.
 *
 * \verbatim ifile:ReadImageData(data: userdata, convert2bitmap: bool, color_mode_flags: number) -> error: number [in Lua 5] \endverbatim
 * \ingroup file */
int imFileReadImageData(imFile* ifile, void* data, int convert2bitmap, int color_mode_flags);
                           
/** Writes the image data. \n
 * Returns an error code.
 *
 * \verbatim ifile:WriteImageData(data: userdata) -> error: number [in Lua 5] \endverbatim
 * \ingroup file */
int imFileWriteImageData(imFile* ifile, void* data);




/** Registers all the internal formats. \n
 * It is automatically called internally when a format is accessed, 
 * but can be called to force the internal formats to be registered before other formats.
 * Notice that additional formats when registered will be registered before the internal formats 
 * if imFormatRegisterInternal is not called yet. \n
 * To control the register order is usefull when two format drivers handle the same format. 
 * The first registered format will always be used first.
 * \ingroup format */
void imFormatRegisterInternal(void);

/** Remove all registered formats.
 * \ingroup format */
void imFormatRemoveAll(void);

/** Returns a list of the registered formats. \n
 * format_list is an array of format identifiers. 
 * Each format identifier is 10 chars max, maximum of 50 formats. 
 * You can use "char* format_list[50]". 
 *
 * \verbatim im.FormatList() -> format_list: table of strings [in Lua 5] \endverbatim
 * \ingroup format */
void imFormatList(char** format_list, int *format_count);

/** Returns the format description. \n
 * Format description is 50 chars max. \n
 * Extensions are separated like "*.tif;*.tiff;", 50 chars max. \n
 * Returns an error code. The parameters can be NULL, except format.
 * See also \ref format.
 *
 * \verbatim im.FormatInfo(format: string) -> error: number, desc: string, ext: string, can_sequence: boolean [in Lua 5] \endverbatim
 * \ingroup format */
int imFormatInfo(const char* format, char* desc, char* ext, int *can_sequence);

/** Returns the format compressions. \n
 * Compressions are 20 chars max each, maximum of 50 compressions. You can use "char* comp[50]". \n
 * color_mode and data_type are optional, use -1 to ignore them. \n
 * If you use them they will select only the allowed compressions checked like in \ref imFormatCanWriteImage. \n
 * Returns an error code.
 * See also \ref format, \ref imErrorCodes, \ref imDataType, \ref imColorSpace and \ref imColorModeConfig.
 *
 * \verbatim im.FormatCompressions(format: string, [color_mode: number], [data_type: number]) -> error: number, comp: table of strings [in Lua 5] \endverbatim
 * \ingroup format */
int imFormatCompressions(const char* format, char** comp, int *comp_count, int color_mode, int data_type);

/** Checks if the format suport the given image class at the given compression. \n
 * Returns an error code.
 * See also \ref format, \ref imErrorCodes, \ref imDataType, \ref imColorSpace and \ref imColorModeConfig.
 *
 * \verbatim im.FormatCanWriteImage(format: string, compression: string, color_mode: number, data_type: number) -> can_write: boolean [in Lua 5] \endverbatim
 * \ingroup format */
int imFormatCanWriteImage(const char* format, const char* compression, int color_mode, int data_type);


#if defined(__cplusplus)
}
#endif

#include "old_im.h"

#endif
