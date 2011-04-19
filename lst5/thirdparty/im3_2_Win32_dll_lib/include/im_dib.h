/** \file
 * \brief Windows DIB (Device Independent Bitmap)
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_dib.h,v 1.2 2006/11/20 13:18:51 scuri Exp $
 */

#ifndef __IM_DIB_H
#define __IM_DIB_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup dib Windows DIB
 *
 * \par
 * Windows DIBs in memory are handled just like a BMP file without the file header. \n
 * These functions will work only in Windows. They are usefull for interchanging data 
 * with the clipboard, with capture drivers, with the AVI and WMF file formats and others.
 * \par
 * Supported DIB aspects:
 * \li bpp must be 1, 4, 8, 16, 24, or 32.
 * \li BITMAPV4HEADER or BITMAPV5HEADER are handled but ignored. \n
 * \li BITMAPCOREHEADER is not handled .
 * \li BI_JPEG and BI_PNG compressions are not handled. 
 * \li biHeight can be negative, compression can be RLE only if created
 *   from imDibCreateReference, imDibPasteClipboard, imDibLoadFile. 
 * \li can not encode/decode Images to/from RLE compressed Dibs. 
 * \li if working with RLE Dibs bits_size is greatter than used. 
 * \li the resolution of a new Dib is taken from the screen. 
 * \li SetDIBitsToDevice(start_scan is 0, scan_lines is dib->bmih->biHeight).  
 * \li StretchDIBits(use always DIB_RGB_COLORS). 
 * \li CreateDIBPatternBrushPt(packed_dib is dib->dib). 
 * \par
 * Must include <windows.h> before using these functions. \n 
 * Check <wingdi.h> for structures and definitions. 
 * \par
 * See \ref im_dib.h
 * \ingroup util */


/** \brief Windows DIB Structure
 *
 * \par
 * Handles a DIB in memory. \n
 * The DIB is stored in only one buffer.
 * The secondary members are pointers to the main buffer.
 * \ingroup dib */
typedef struct _imDib 
{
  HGLOBAL           handle;        /**< The windows memory handle */
  BYTE*             dib;           /**< The DIB as it is defined in memory */
  int               size;          /**< Full size in memory */
                                  
  BITMAPINFO*       bmi;           /**< Bitmap Info = Bitmap Info Header + Palette */
  BITMAPINFOHEADER* bmih;          /**< Bitmap Info Header */
  RGBQUAD*          bmic;          /**< Bitmap Info Colors = Palette */
  BYTE*             bits;	         /**< Bitmap Bits */
                                  
  int               palette_count; /**< number of colors in the palette */
  int               bits_size;     /**< size in bytes of the Bitmap Bits */
  int               line_size;     /**< size in bytes of one line, includes padding */
  int               pad_size;      /**< number of bytes remaining in the line, lines are in a word boundary */
                                  
  int               is_reference;  /**< only a reference, do not free pointer */
} imDib; 

/** Creates a new DIB. \n
 * use bpp=-16/-32 to allocate space for BITFLIEDS.
 * \ingroup dib */
imDib* imDibCreate(int width, int height, int bpp);     

/** Duplicates the DIB contents in a new DIB.
 * \ingroup dib */
imDib* imDibCreateCopy(const imDib* dib);

/** Creates a DIB using an already allocated memory. \n
 * "bmi" must be a pointer to BITMAPINFOHEADER. \n
 * "bits" can be NULL if it is inside "bmi" after the palette.
 * \ingroup dib */
imDib* imDibCreateReference(BYTE* bmi, BYTE* bits);     

/** Creates a DIB section for drawing porposes. \n
 * Returns the image handle also created.
 * \ingroup dib */
imDib* imDibCreateSection(HDC hDC, HBITMAP *image, int width, int height, int bpp);

/** Destroy the DIB
 * \ingroup dib */
void imDibDestroy(imDib* dib);                                 

/** DIB GetPixel function definition. \n 
 * the DWORD is a raw copy of the bits, use (unsigned char*)&pixel
 * \ingroup dib */
typedef unsigned int (*imDibLineGetPixel)(unsigned char* line, int col);

/** Returns a function to read pixels from a DIB line.
 * \ingroup dib */
imDibLineGetPixel imDibLineGetPixelFunc(int bpp);  

/** DIB SetPixel function definition
 * \ingroup dib */
typedef void (*imDibLineSetPixel)(unsigned char* line, int col, unsigned int pixel);

/** Returns a function to write pixels into a DIB line.
 * \ingroup dib */
imDibLineSetPixel imDibLineSetPixelFunc(int bpp);  

/** Creates a DIB from a image handle and a palette handle.
 * \ingroup dib */
imDib* imDibFromHBitmap(const HBITMAP image, const HPALETTE hPalette); 

/** Creates a image handle from a DIB.
 * \ingroup dib */
HBITMAP imDibToHBitmap(const imDib* dib);

/** Returns a Logical palette from the DIB palette. \n
 * DIB bpp must be <=8.
 * \ingroup dib */
HPALETTE imDibLogicalPalette(const imDib* dib);

/** Captures the screen into a DIB.
 * \ingroup dib */
imDib* imDibCaptureScreen(int x, int y, int width, int height);

/** Transfer the DIB to the clipboard. \n 
 * "dib" pointer can not be used after, or use imDibCopyClipboard(imDibCreateCopy(dib)).
 * Warning: Clipboard functions in C++ can fail with Visual C++ /EHsc (Enable C++ Exceptions)
* \ingroup dib */
void imDibCopyClipboard(imDib* dib); 

/** Creates a reference for the DIB in the clipboard if any. Returns NULL otherwise.
 * Warning: Clipboard functions in C++ can fail with Visual C++ /EHsc (Enable C++ Exceptions)
 * \ingroup dib */
imDib* imDibPasteClipboard(void);           

/** Checks if there is a dib at the clipboard.
 * \ingroup dib */
int imDibIsClipboardAvailable(void);

/** Saves the DIB into a file ".bmp".
 * \ingroup dib */
int imDibSaveFile(const imDib* dib, const char* filename);

/** Creates a DIB from a file ".bmp".
 * \ingroup dib */
imDib* imDibLoadFile(const char* filename);

/** Converts a DIB into an RGBA image. alpha is optional. bpp must be >8. \n 
 * alpha is used only when bpp=32.
 * \ingroup dib */
void imDibDecodeToRGBA(const imDib* dib, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha);

/** Converts a DIB into an indexed image. bpp must be <=8. colors must have room for at least 256 colors. 
 * colors is rgb packed (RGBRGBRGB...)
 * \ingroup dib */
void imDibDecodeToMap(const imDib* dib, unsigned char* map, long* palette);

/** Converts an RGBA image into a DIB. alpha is optional. bpp must be >8. \n
 * alpha is used only when bpp=32.
 * \ingroup dib */
void imDibEncodeFromRGBA(imDib* dib, const unsigned char* red, const unsigned char* green, const unsigned char* blue, const unsigned char* alpha);

/** Converts an indexed image into a DIB. bpp must be <=8. \n
 * colors is rgb packed (RGBRGBRGB...)
 * \ingroup dib */
void imDibEncodeFromMap(imDib* dib, const unsigned char* map, const long* palette, int palette_count);

/** Converts a IM_RGB packed image, with or without alpha, into a DIB.
 * \ingroup dib */
void imDibEncodeFromBitmap(imDib* dib, const unsigned char* data);

/** Converts a DIB into IM_RGB packed image, with or without alpha.
 * \ingroup dib */
void imDibDecodeToBitmap(const imDib* dib, unsigned char* data);

#ifdef __IM_IMAGE_H
/* You must include "im_image.h" before this header to enable these declarations. */

/** Creates a imImage from the dib data.
 * \ingroup dib */
imImage* imDibToImage(const imDib* dib);

/** Creates a Dib from the image. It must be a bitmap image.
 * \ingroup dib */
imDib* imDibFromImage(const imImage* image);

#endif

#if defined(__cplusplus)
}
#endif

#endif
