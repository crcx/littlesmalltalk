/** \file
 * \brief HSI Color Manipulation
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_colorhsi.h,v 1.2 2005/07/15 19:59:21 scuri Exp $
 */

#ifndef __IM_COLORHSI_H
#define __IM_COLORHSI_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup hsi HSI Color Coordinate System Conversions
 *
 * \par
 * HSI is just the RGB color space written in a different coordinate system.
 * \par
 * "I" is defined along the cube diagonal. It ranges from 0 (black) to 1 (white). \n
 * HS are the polar coordinates of a plane normal to "I". \n
 * "S" is the normal distance from the diagonal of the RGB cube. It ranges from 0 to Smax. \n
 * "H" is the angle starting from the red vector, given in degrees.
 * \par
 * This is not a new color space, this is exactly the same gammut as RGB. \n
 * Since it is still a cube, Smax depends on H. 
 * \par
 * See \ref im_colorhsi.h
 * \ingroup color */


/** Returns the maximum S for H (here in radians) and I.
 * \ingroup hsi */
float imColorHSI_Smax(float h, double cosh, double sinh, float i);

/** Returns I where S is maximum given H (here in radians).
 * \ingroup hsi */
float imColorHSI_ImaxS(float h, double cosh, double sinh);

/** Converts from RGB to HSI.
 * \ingroup hsi */
void imColorRGB2HSI(float r, float g, float b, float *h, float *s, float *i);

/** Converts from RGB (byte) to HSI.
 * \ingroup hsi */
void imColorRGB2HSIbyte(unsigned char r, unsigned char g, unsigned char b, float *h, float *s, float *i);

/** Converts from HSI to RGB.
 * \ingroup hsi */
void imColorHSI2RGB(float h, float s, float i, float *r, float *g, float *b);

/** Converts from HSI to RGB (byte).
 * \ingroup hsi */
void imColorHSI2RGBbyte(float h, float s, float i, unsigned char *r, unsigned char *g, unsigned char *b);


#if defined(__cplusplus)
}
#endif

#endif
