/** \file
 * \brief Binary File Access.
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_binfile.h,v 1.4 2006/11/21 11:56:16 scuri Exp $
 */

#include "im_util.h"

#ifndef __IM_BINFILE_H
#define __IM_BINFILE_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup binfile Binary File Access 
 *
 * \par
 * These functions are very usefull for reading/writing binary files 
 * that have headers or data that have to be converted depending on 
 * the current CPU byte order. It can invert 2, 4 or 8 bytes numbers to/from little/big-endian orders.
 * \par
 * It will process the data only if the file format is diferent from the current CPU.
 * \par
 * Can read from disk or memory. In case of a memory buffer, the file name must be the \ref imBinMemoryFileName structure.
 * \par
 * See \ref im_binfile.h
 * \ingroup util */

typedef struct _imBinFile imBinFile;

/** Opens an existant binary file for reading.
 * The default file byte order is the CPU byte order.
 * Returns NULL if failed.
 * \ingroup binfile */
imBinFile* imBinFileOpen(const char* pFileName);

/** Creates a new binary file for writing.
 * The default file byte order is the CPU byte order.
 * Returns NULL if failed.
 * \ingroup binfile */
imBinFile* imBinFileNew(const char* pFileName);

/** Closes the file.
 * \ingroup binfile */
void imBinFileClose(imBinFile* bfile);

/** Indicates that was an error on the last operation.
 * \ingroup binfile */
int imBinFileError(imBinFile* bfile);

/** Returns the file size in bytes.
 * \ingroup binfile */
unsigned long imBinFileSize(imBinFile* bfile);

/** Changes the file byte order. Returns the old one.
 * \ingroup binfile */
int imBinFileByteOrder(imBinFile* bfile, int pByteOrder);

/** Reads an array of count values with byte sizes: 1, 2, 4, or 8. And invert the byte order if necessary after read.
 * \ingroup binfile */
unsigned long imBinFileRead(imBinFile* bfile, void* pValues, unsigned long pCount, int pSizeOf);

/** Writes an array of values with sizes: 1, 2, 4, or 8. And invert the byte order if necessary before write.\n
 * <b>ATENTION</b>: The function will not make a temporary copy of the values to invert the byte order.\n
 * So after the call the values will be invalid, if the file byte order is diferent from the CPU byte order. 
 * \ingroup binfile */
unsigned long imBinFileWrite(imBinFile* bfile, void* pValues, unsigned long pCount, int pSizeOf);

/** Writes a string without the NULL terminator. The function uses sprintf to compose the string. \n
 * The internal buffer is fixed at 4096 bytes.
 * \ingroup binfile */
unsigned long imBinFilePrintf(imBinFile* bfile, char *format, ...);

/** Moves the file pointer from the begining of the file.\n
 * When writing to a file seeking can go beyond the end of the file.
 * \ingroup binfile */
void imBinFileSeekTo(imBinFile* bfile, unsigned long pOffset);

/** Moves the file pointer from current position.\n
 * If the offset is a negative value the pointer moves backwards.
 * \ingroup binfile */
void imBinFileSeekOffset(imBinFile* bfile, long pOffset);

/** Moves the file pointer from the end of the file.\n
 * The offset is usually a negative value.
 * \ingroup binfile */
void imBinFileSeekFrom(imBinFile* bfile, long pOffset);

/** Returns the current offset position.
 * \ingroup binfile */
unsigned long imBinFileTell(imBinFile* bfile);

/** Indicates that the file pointer is at the end of the file.
 * \ingroup binfile */
int imBinFileEndOfFile(imBinFile* bfile);

/** Predefined I/O Modules.
 * \ingroup binfile */
enum imBinFileModule 	
{               
	IM_RAWFILE,   /**< System dependent file I/O Rotines. */
	IM_STREAM,    /**< Standard Ansi C Stream I/O Rotines. */
	IM_MEMFILE,   /**< Uses a memory buffer. */
	IM_SUBFILE,   /**< It is a sub file. FileName is a imBinFile* pointer from any other module. */
  IM_FILEHANDLE,/**< System dependent file I/O Rotines, but FileName is a system file handle ("int" in UNIX and "HANDLE" in Windows). */
	IM_IOCUSTOM0  /**< Other registered modules starts from here. */
};

/** Sets the current I/O module.
 * \returns the previous function set, or -1 if failed.
 * See also \ref imBinFileModule.
 * \ingroup binfile */
int imBinFileSetCurrentModule(int pModule);

/** \brief Memory File I/O Filename
 *
 * \par
 *  Fake file name for the memory I/O module.
 * \ingroup binfile */
typedef struct _imBinMemoryFileName
{
  unsigned char *buffer; /**< The memory buffer. If you are reading the buffer must exists. 
                          *   If you are writing the buffer can be internally allocated to the given size. The buffer is never free.
                          *   The buffer is allocated using "malloc", and reallocated using "realloc". Use "free" to release it. 
                          *   To avoid RTL conflicts use the function imBinMemoryRelease. */
  int size;              /**< Size of the buffer. */ 
  float reallocate;      /**< Reallocate factor for the memory buffer when writing (size += reallocate*size). 
                          *   Set reallocate to 0 to disable reallozation, in this case buffer must not be NULL. */
}imBinMemoryFileName;
                                             
/** Release the internal memory allocated when writing a Memory File.
 * \ingroup binfile */
void imBinMemoryRelease(unsigned char *buffer);


#if	defined(__cplusplus)
}
#endif


#if	defined(__cplusplus)

/** Base class to help the creation of new modules.\n
 * It handles the read/write operations with byte order correction if necessary.
 * \ingroup binfile */
class imBinFileBase
{
  friend class imBinSubFile;

protected:
  int IsNew,
      FileByteOrder,
      DoByteOrder;   // to speed up byte order checking

  // These will actually read/write the data
  virtual unsigned long ReadBuf(void* pValues, unsigned long pSize) = 0;
  virtual unsigned long WriteBuf(void* pValues, unsigned long pSize) = 0;

public:

  int InitByteOrder(int ByteOrder)
  {
    int old_byte_order = this->FileByteOrder;
    this->FileByteOrder = ByteOrder;
    
	  if (ByteOrder != imBinCPUByteOrder())
	    this->DoByteOrder = 1;
	  else
      this->DoByteOrder = 0;
    return old_byte_order;
  }

  // These will take care of byte swap if needed.

  unsigned long Read(void* pValues, unsigned long pCount, int pSizeOf)
  {
    unsigned long rSize = ReadBuf(pValues, pCount * pSizeOf);
    if (pSizeOf != 1 && DoByteOrder) imBinSwapBytes(pValues, pCount, pSizeOf);
    return rSize/pSizeOf;
  }

  unsigned long Write(void* pValues, unsigned long pCount, int pSizeOf)
  {
    if (pSizeOf != 1 && DoByteOrder) imBinSwapBytes(pValues, pCount, pSizeOf);
    return WriteBuf(pValues, pCount * pSizeOf)/pSizeOf;
  }

  virtual void Open(const char* pFileName) = 0;
  virtual void New(const char* pFileName) = 0;
  virtual void Close() = 0;
  virtual unsigned long FileSize() = 0;
  virtual int HasError() const = 0;
  virtual void SeekTo(unsigned long pOffset) = 0;
  virtual void SeekOffset(long pOffset) = 0;
  virtual void SeekFrom(long pOffset) = 0;
  virtual unsigned long Tell() const = 0;
  virtual int EndOfFile() const = 0;
};

/** File I/O module creation callback.
 * \ingroup binfile */
typedef imBinFileBase* (*imBinFileNewFunc)();

/** Register a user I/O module.\n
 * Returns the new function set id.\n
 * Accepts up to 10 modules.
 * \ingroup binfile */
int imBinFileRegisterModule(imBinFileNewFunc pNewFunc);

#endif

#endif
