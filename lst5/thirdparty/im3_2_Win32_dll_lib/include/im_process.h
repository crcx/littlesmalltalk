/** \file
 * \brief Image Processing
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_PROCESS_H
#define __IM_PROCESS_H

#include "im_process_pon.h"
#include "im_process_loc.h"
#include "im_process_glo.h"
#include "im_process_ana.h"

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup process Image Processing
 * \par
 * Several image processing functions based on the \ref imImage structure.
 * \par
 * You must link the application with "im_process.lib/.a/.so". \n
 * Some complex operations use the \ref counter.\n
 * There is no check on the input/output image properties, 
 * check each function documentation before using it.
 */


#if defined(__cplusplus)
}
#endif

#endif
