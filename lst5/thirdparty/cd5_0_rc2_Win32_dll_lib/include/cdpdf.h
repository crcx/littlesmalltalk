/** \file
 * \brief PDF driver
 *
 * See Copyright Notice in cd.h
 */

#ifndef __CD_PDF_H
#define __CD_PDF_H

#ifdef __cplusplus
extern "C" {
#endif

cdContext* cdContextPDF(void);

#define CD_PDF cdContextPDF()
        
#ifndef CD_PAPERSIZE
#define CD_PAPERSIZE
enum {                          /* paper sizes */
  CD_A0,
  CD_A1,
  CD_A2,
  CD_A3,
  CD_A4,
  CD_A5,
  CD_LETTER,
  CD_LEGAL
};
#endif


#ifdef __cplusplus
}
#endif

#endif /* ifndef _CD_PDF_ */

