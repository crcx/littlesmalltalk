/** \file
 * \brief RAW File Format
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_raw.h,v 1.3 2006/11/21 11:56:16 scuri Exp $
 */

#ifndef __IM_RAW_H
#define __IM_RAW_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** Opens a RAW image file.
 * See also \ref imErrorCodes.
 *
 * \verbatim im.FileOpenRaw(file_name: string) -> ifile: imFile, error: number [in Lua 5] \endverbatim
 * \ingroup raw */
imFile* imFileOpenRaw(const char* file_name, int *error);
                   
/** Creates a RAW image file.
 * See also \ref imErrorCodes.
 *
 * \verbatim im.FileNewRaw(file_name: string) -> ifile: imFile, error: number [in Lua 5] \endverbatim
 * \ingroup raw */
imFile* imFileNewRaw(const char* file_name, int *error);
                   

#if defined(__cplusplus)
}
#endif

#endif
