/** \file
 * \brief IUP driver
 *
 * See Copyright Notice in cd.h
 */

#ifndef __CD_IUP_H
#define __CD_IUP_H

#ifdef __cplusplus
extern "C" {
#endif

cdContext* cdContextIup(void);

#define CD_IUP cdContextIup()

#ifdef __cplusplus
}
#endif

#endif
