/** \file
 * \brief Old API
 *
 * See Copyright Notice in im_lib.h
 * $Id: old_im.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_OLD_H
#define __IM_OLD_H

#if	defined(__cplusplus)
extern "C" {
#endif

enum {IM_BMP, IM_PCX, IM_GIF, IM_TIF, IM_RAS, IM_SGI, IM_JPG, IM_LED, IM_TGA};
enum {IM_NONE = 0x0000, IM_DEFAULT = 0x0100, IM_COMPRESSED = 0x0200};                        

#define IM_ERR_READ IM_ERR_ACCESS
#define IM_ERR_WRITE IM_ERR_ACCESS
#define IM_ERR_TYPE IM_ERR_DATA
#define IM_ERR_COMP IM_ERR_COMPRESS

long imEncodeColor(unsigned char red, unsigned char green, unsigned char blue);
void imDecodeColor(unsigned char* red, unsigned char* green, unsigned char* blue, long palette);
int imFileFormat(char *filename, int* format);
int imImageInfo(char *filename, int *width, int *height, int *type, int *palette_count);
int imLoadRGB(char *filename, unsigned char *red, unsigned char *green, unsigned char *blue);
int imSaveRGB(int width, int height, int format, unsigned char *red, unsigned char *green, unsigned char *blue, char *filename);
int imLoadMap(char *filename, unsigned char *map, long *palette);
int imSaveMap(int width, int height, int format, unsigned char *map, int palette_count, long *palette, char *filename);
void imRGB2Map(int width, int height, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *map, int palette_count, long *palette);
void imMap2RGB(int width, int height, unsigned char *map, int palette_count, long *colors, unsigned char *red, unsigned char *green, unsigned char *blue);
void imRGB2Gray(int width, int height, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *map, long *grays);
void imMap2Gray(int width, int height, unsigned char *map, int palette_count, long *colors, unsigned char *grey_map, long *grays);
void imResize(int src_width, int src_height, unsigned char *src_map, int dst_width, int dst_height, unsigned char *dst_map);
void imStretch(int src_width, int src_height, unsigned char *src_map, int dst_width, int dst_height, unsigned char *dst_map);
typedef int (*imCallback)(char *filename);
int imRegisterCallback(imCallback cb, int cb_id, int format);

#define IM_INTERRUPTED -1
#define IM_ALL -1
#define IM_COUNTER_CB   0
typedef int (*imFileCounterCallback)(char *filename, int percent, int io);

#define IM_RESOLUTION_CB   1
typedef int (*imResolutionCallback)(char *filename, double* xres, double* yres, int* res_unit);

enum {IM_RES_NONE, IM_RES_DPI, IM_RES_DPC};

#define IM_GIF_TRANSPARENT_COLOR_CB 0
typedef int (*imGifTranspIndex)(char *filename, unsigned char *transp_index);

#define IM_TIF_IMAGE_DESCRIPTION_CB 0
typedef int (*imTiffImageDesc)(char *filename, char* img_desc);

#if defined(__cplusplus)
}
#endif

#endif
