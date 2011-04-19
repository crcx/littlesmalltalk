/** \file
 * \brief File Format Access
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_format.h,v 1.5 2006/11/22 19:55:32 scuri Exp $
 */

#include "im_file.h"
#include "im_attrib.h"

#ifndef __IM_FORMAT_H
#define __IM_FORMAT_H


/** \brief Image File Format Driver (SDK Use Only) 
 * 
 * \par
 * Virtual Base class for file formats. All file formats inherit from this class.
 * \ingroup filesdk */
class imFormat: public _imFile
{
public:
  const char* format; 
  const char* desc;
  const char* ext;
  const char** comp;
  int comp_count, 
      can_sequence;

  imFormat(const char* _format, const char* _desc, const char* _ext, 
           const char** _comp, int _comp_count, int _can_sequence)
    :format(_format), desc(_desc), ext(_ext), comp(_comp), 
     comp_count(_comp_count), can_sequence(_can_sequence)
    {} 
  virtual ~imFormat() {}

  imAttribTable* AttribTable() {return (imAttribTable*)this->attrib_table;}

  /* Pure Virtual Methods. Every driver must implement all the following methods. */

  virtual int Open(const char* file_name) = 0; // Must initialize compression and image_count
  virtual int New(const char* file_name) = 0;
  virtual void Close() = 0;
  virtual void* Handle(int index) = 0;
  virtual int ReadImageInfo(int index) = 0;    // Should update compression
  virtual int ReadImageData(void* data) = 0;
  virtual int WriteImageInfo() = 0;            // Should update compression
  virtual int WriteImageData(void* data) = 0;  // Must update image_count
  virtual int CanWrite(const char* compression, int color_mode, int data_type) const = 0;
};

extern "C"
{

/* Internal Use only */

/* Opens a file with the respective format driver 
 * Uses the file extension to speed up the search for the format driver.
 * Used by "im_file.cpp" only. */
imFormat* imFormatOpen(const char* file_name, int *error);

/* Opens a file with the given format
 * Used by "im_file.cpp" only. */
imFormat* imFormatOpenAs(const char* file_name, const char* format, int *error);

/* Creates a file using the given format driver.
 * Used by "im_file.cpp" only. */
imFormat* imFormatNew(const char* file_name, const char* format, int *error);


/* File Format SDK */

/** Register a format driver.
 * \ingroup filesdk */
void imFormatRegister(imFormat* iformat);


}

#endif
