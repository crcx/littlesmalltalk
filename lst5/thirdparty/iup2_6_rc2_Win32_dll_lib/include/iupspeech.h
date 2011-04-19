/** \file
 * \brief iupspeech control.
 *
 * See Copyright Notice in iup.h
 */

#ifndef __IUPSPEECH_H
#define __IUPSPEECH_H

#ifdef __cplusplus
extern "C" {
#endif

int IupSpeechOpen(void);
void IupSpeechClose(void);
Ihandle *IupSpeech(void);

#ifdef __cplusplus
}
#endif

#endif

