/** \file
 * \brief Complex Data Type.
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_complex.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_COMPLEX_H
#define __IM_COMPLEX_H

#include "im_math.h"

/** \defgroup cpx Complex Numbers
 * \par
 * See \ref im_complex.h
 * \ingroup util
 */

/** \brief Complex Float Data Type  
 *         
 * \par
 * Complex class using two floats, one for real part, one for the imaginary part.
 * \par
 * It is not a complete complex class, we just implement constructors inside the class.
 * All the other operators and functions are external to the class.
 * \ingroup cpx */
class imcfloat
{
public:
	float real;  ///< Real part.
	float imag;  ///< Imaginary part.

  ///	Default Constructor (0,0).
	imcfloat():real(0), imag(0) {}

  ///	Constructor from (real, imag)
	imcfloat(const float& r, const float& i):real(r),imag(i) {}

  ///	Constructor from (real)
	imcfloat(const float& r):real(r),imag(0) {}
};

/** \addtogroup cpx
 * Complex numbers operators.
 * @{ 
 */

inline int operator <= (const imcfloat& C1, const imcfloat& C2)
{
  return ((C1.real <= C2.real) && (C1.imag <= C2.imag));
}

inline int operator <= (const imcfloat& C, const float& F)
{
  return ((F <= C.real) && (0 <= C.imag));
}

inline imcfloat operator + (const imcfloat& C1, const imcfloat& C2)
{
  return imcfloat(C1.real + C2.real, C1.imag + C2.imag);
}

inline imcfloat operator += (const imcfloat& C1, const imcfloat& C2)
{
  return imcfloat(C1.real + C2.real, C1.imag + C2.imag);
}

inline imcfloat operator - (const imcfloat& C1, const imcfloat& C2)
{
  return imcfloat(C1.real - C2.real, C1.imag - C2.imag);
}

inline imcfloat operator * (const imcfloat& C1, const imcfloat& C2)
{
  return imcfloat(C1.real * C2.real - C1.imag * C2.imag, 
                        C1.imag * C2.real + C1.real * C2.imag);
}

inline imcfloat operator / (const imcfloat& C1, const imcfloat& C2)
{
  float den = C2.real * C2.real - C2.imag * C2.imag;
  return imcfloat((C1.real * C2.real + C1.imag * C2.imag) / den, 
                        (C1.imag * C2.real - C1.real * C2.imag) / den);
}

inline imcfloat operator / (const imcfloat& C, const float& R)
{
  return imcfloat(C.real / R, C.imag / R);
}

inline imcfloat operator /= (const imcfloat& C, const float& R)
{
  return imcfloat(C.real / R, C.imag / R);
}

inline imcfloat operator * (const imcfloat& C, const float& R)
{
  return imcfloat(C.real * R, C.imag * R);
}

inline int operator == (const imcfloat& C1, const imcfloat& C2)
{
  return ((C1.real == C2.real) && (C1.imag == C2.imag));
}

inline float cpxreal(const imcfloat& C)
{
  return C.real;
}

inline float cpximag(const imcfloat& C)
{
  return C.imag;
}

inline float cpxmag(const imcfloat& C)
{
  return sqrtf(C.real*C.real + C.imag*C.imag);
}

inline float cpxphase(const imcfloat& C)
{
  return atan2f(C.real, C.imag);
}

inline imcfloat cpxconj(const imcfloat& C)
{
  return imcfloat(C.real, -C.imag);
}

inline imcfloat log(const imcfloat& C)
{
  return imcfloat(logf(cpxmag(C)), atan2f(C.real, C.imag));
}

inline imcfloat exp(const imcfloat& C)
{
  float mag = expf(C.real);
  return imcfloat(mag * cosf(C.imag), mag * sinf(C.imag));
}

inline imcfloat pow(const imcfloat& C1, const imcfloat& C2)
{
  return exp(C1 * log(C2));
}

inline imcfloat sqrt(const imcfloat& C)
{
  float mag = sqrtf(sqrtf(C.real*C.real + C.imag*C.imag));
  float phase = atan2f(C.real, C.imag) / 2;
  return imcfloat(mag * cosf(phase), mag * sinf(phase));
}

inline imcfloat cpxpolar(const float& mag, const float& phase)
{
  return imcfloat(mag * cosf(phase), mag * sinf(phase));
}

/** @} */

#endif
