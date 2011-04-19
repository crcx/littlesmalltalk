/** \file
 * \brief File Access
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_file.h,v 1.2 2005/08/10 02:12:16 scuri Exp $
 */

#ifndef __IM_FILE_H
#define __IM_FILE_H

#include "im.h"

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup filesdk File Format SDK
 * \par
 * All the file formats are based on theses structures. Use them to create new file formats. \n
 * The LineBuffer functions will help transfer image from format buffer to application buffer and vice-versa.
 * \par
 * See \ref im_file.h
 * \ingroup file */


/** \brief Image File Format Base (SDK Use Only)
 *
 * \par
 * Base container to hold format independent state variables.
 * \ingroup filesdk */
struct _imFile
{
  int is_new;
  void* attrib_table;    /**< in fact is a imAttribTable, but we hide this here */

  void* line_buffer;     /**< used for line convertion, contains all components if packed, or only one if not */
  int line_buffer_size;
  int line_buffer_extra; /**< extra bytes to be allocated */
  int line_buffer_alloc; /**< total allocated so far */
  int counter;

  int convert_bpp;       /**< number of bpp to unpack/pack to/from 1 byte. 
                              When reading converts n packed bits to 1 byte (unpack). If n>1 will also expand to 0-255. 
                              When writing converts 1 byte to 1 bit (pack). 
                              If negative will only expand to 0-255 (no unpack or pack). */
  int switch_type;       /**< flag to switch the original data type: char-byte, short-ushort, uint-int, double-float */

  long palette[256];
  int palette_count;

  int user_color_mode,
      user_data_type,
      file_color_mode,   /* these two must be filled by te driver always. */
      file_data_type;

  /* these must be filled by the driver when reading,
     and given by the user when writing. */

  char compression[10];
  int image_count,
      image_index,
      width,           
      height;
};


/* Internal Use only */

/* Initializes the imFile structure.
 * Used by the special format RAW. */
void imFileClear(imFile* ifile);

/* Initializes the line buffer.
 * Used by "im_file.cpp" only. */
void imFileLineBufferInit(imFile* ifile);

/* Check if the conversion is valid.
 * Used by "im_file.cpp" only. */
int imFileCheckConversion(imFile* ifile);



/* File Format SDK */

/** Number of lines to be accessed.
 * \ingroup filesdk */
int imFileLineBufferCount(imFile* ifile);

/** Increments the row and plane counters.
 * \ingroup filesdk */
void imFileLineBufferInc(imFile* ifile, int *row, int *plane);

/** Converts from FILE color mode to USER color mode.
 * \ingroup filesdk */
void imFileLineBufferRead(imFile* ifile, void* data, int line, int plane);

/** Converts from USER color mode to FILE color mode.
 * \ingroup filesdk */
void imFileLineBufferWrite(imFile* ifile, const void* data, int line, int plane);

/** Utility to calculate the line size in byte with a specified alignment. \n
 * "align" can be 1, 2 or 4.
 * \ingroup filesdk */
int imFileLineSizeAligned(int width, int bpp, int align);


#if defined(__cplusplus)
}
#endif

#endif
