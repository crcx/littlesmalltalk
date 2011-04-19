/** \file
 * \brief Color Manipulation
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_color.h,v 1.1 2005/04/02 22:07:00 scuri Exp $
 */

#ifndef __IM_COLOR_H
#define __IM_COLOR_H

#include "im_math.h"

/** \defgroup color Color Manipulation
 *
 * \par
 * Functions to convert from one color space to another, 
 * and color gammut utilities.
 * \par
 * See \ref im_color.h
 *
 * \section s1 Some Color Science
 * \par
 * Y is luminance, a linear-light quantity. 
 * It is directly proportional to physical intensity
 * weighted by the spectral sensitivity of human vision.
 * \par
 * L* is lightness, a nonlinear luminance
 * that aproximates the perception of brightness. 
 * It is nearly perceptual uniform.
 * It has a range of 0 to 100.
 * \par
 * Y' is luma, a nonlinear luminance that aproximates lightness.
 * \par
 * Brightness is a visual sensation according to which an area
 * apears to exhibit more or less light. 
 * It is a subjective quantity and can not be measured.
 * \par
 * One unit of euclidian distante in CIE L*u*v* or CIE L*a*b* corresponds
 * roughly to a just-noticeable difference (JND) of color.
 * \par
\verbatim
 ChromaUV = sqrt(u*u + v*v)       
 HueUV = atan2(v, u)
 SaturationUV = ChromaUV / L      (called psychometric saturation) 
 (the same can be calculated for Lab)
\endverbatim
 * \par
 * IEC 61966-2.1 Default RGB colour space - sRGB
 * \li ITU-R Recommendation BT.709 (D65 white point).
 * \li D65 White Point (X,Y,Z) = (0.9505 1.0000 1.0890)
 * \par
 * Documentation extracted from  Charles Poynton - Digital Video and HDTV - Morgan Kaufmann - 2003.
 *
 * \section Links
 * \li www.color.org - ICC
 * \li www.srgb.com - sRGB
 * \li www.poynton.com - Charles Poynton
 * \li www.littlecms.com - A free Color Management System (use this if you need precise color conversions)
 *
 * \section cci Color Component Intervals
 * \par
 * All the color components are stored in the 0-max interval, even the signed ones. \n
 * Here are the pre-defined intervals for each data type. These values are used for standard color conversion.
 * You should normalize data before converting betwwen color spaces.
 * \par
\verbatim
 byte   [0,255]      or [-128,+127]          (1 byte)
 ushort [0,65535]    or [-32768,+32767]      (2 bytes)
 int    [0,16777215] or [-8388608,+8388607]  (3 bytes)
 float  [0,1]        or [-0.5,+0.5]          (4 bytes)
\endverbatim
 * \ingroup util */

/** Returns the zero value for color conversion porpouses. \n
 * This is a value to be compensated when the data_type is unsigned and component is signed. \n
 * \ingroup color */
inline float imColorZero(int data_type)
{
  float zero[] = {128.0f, 32768.0f, 8388608.0f, 0.5f};
  return zero[data_type];
}

/** Returns the maximum value for color conversion porpouses. \n
 * \ingroup color */
inline int imColorMax(int data_type)
{
  int max[] = {255, 65535, 16777215, 1};
  return max[data_type];
}

/** Quantize 0-1 values into 0-max. \n
 * q = r * (max + 1) \n
 * Divide by the size of each interval 1/(max+1),
 * then the value is rounded down in the typecast. \n
 * But 0 is mapped to 0, and 1 is mapped to max.
 * \ingroup color */
template <class T> 
inline T imColorQuantize(const float& value, const T& max)
{
  if (max == 1) return (T)value; // to allow a dummy quantize
  if (value >= 1) return max;
  if (value <= 0) return 0;
  return (T)(value*(max + 1));  
}                               

/** Reconstruct 0-max values into 0-1. \n
 * r = (q + 0.5)/(max + 1)  \n
 * Add 0.5 to set the same origin, then multiply by the size of each interval 1/(max+1). \n
 * But 0 is mapped to 0, and max is mapped to 1.
 * \ingroup color */
template <class T> 
inline float imColorReconstruct(const T& value, const T& max)
{
  if (max == 1) return (float)value;  // to allow a dummy reconstruct
  if (value <= 0) return 0;
  if (value >= max) return 1;
  return (((float)value + 0.5f)/((float)max + 1.0f));
}

/** Converts Y'CbCr to R'G'B' (all nonlinear). \n
 * ITU-R Recommendation 601-1 with no headroom/footroom.
\verbatim
 0 <= Y <= 1 ; -0.5 <= CbCr <= 0.5 ; 0 <= RGB <= 1 

 R'= Y' + 0.000 *Cb + 1.402 *Cr
 G'= Y' - 0.344 *Cb - 0.714 *Cr
 B'= Y' + 1.772 *Cb + 0.000 *Cr
\endverbatim
 * \ingroup color */
template <class T> 
inline void imColorYCbCr2RGB(const T Y, const T Cb, const T Cr, 
                             T& R, T& G, T& B,
                             const T& zero, const T& max)
{
  float r = float(Y                        + 1.402f * (Cr - zero));
  float g = float(Y - 0.344f * (Cb - zero) - 0.714f * (Cr - zero));
  float b = float(Y + 1.772f * (Cb - zero));

  // now we should enforce 0<= rgb <= max

  R = (T)IM_CROPMAX(r, max);
  G = (T)IM_CROPMAX(g, max);
  B = (T)IM_CROPMAX(b, max);
}

/** Converts R'G'B' to Y'CbCr (all nonlinear). \n
 * ITU-R Recommendation 601-1 with no headroom/footroom.
\verbatim
 0 <= Y <= 1 ; -0.5 <= CbCr <= 0.5 ; 0 <= RGB <= 1 

 Y' =  0.299 *R' + 0.587 *G' + 0.114 *B'
 Cb = -0.169 *R' - 0.331 *G' + 0.500 *B'
 Cr =  0.500 *R' - 0.419 *G' - 0.081 *B'
\endverbatim
 * \ingroup color */
template <class T> 
inline void imColorRGB2YCbCr(const T R, const T G, const T B, 
                             T& Y, T& Cb, T& Cr,
                             const T& zero)
{
  Y  = (T)( 0.299f *R + 0.587f *G + 0.114f *B);
  Cb = (T)(-0.169f *R - 0.331f *G + 0.500f *B + (float)zero);
  Cr = (T)( 0.500f *R - 0.419f *G - 0.081f *B + (float)zero);

  // there is no need for cropping here, YCrCr is already at the limits
}

/** Converts C'M'Y'K' to R'G'B' (all nonlinear). \n
 * This is a poor conversion that works for a simple visualization.
\verbatim
  0 <= CMYK <= 1 ; 0 <= RGB <= 1 

  R = (1 - K) * (1 - C)
  G = (1 - K) * (1 - M)
  B = (1 - K) * (1 - Y)
\endverbatim
 * \ingroup color */
template <class T>
inline void imColorCMYK2RGB(const T C, const T M, const T Y, const T K, 
                            T& R, T& G, T& B, const T& max)
{
  T W = max - K;
  R = (T)((W * (max - C)) / max);
  G = (T)((W * (max - M)) / max);
  B = (T)((W * (max - Y)) / max);

  // there is no need for cropping here, RGB is already at the limits
}

/** Converts CIE XYZ to Rec 709 RGB (all linear). \n
 * ITU-R Recommendation BT.709 (D65 white point). \n
\verbatim
  0 <= XYZ <= 1 ; 0 <= RGB <= 1    

  R =  3.2406 *X - 1.5372 *Y - 0.4986 *Z
  G = -0.9689 *X + 1.8758 *Y + 0.0415 *Z
  B =  0.0557 *X - 0.2040 *Y + 1.0570 *Z
\endverbatim
 * \ingroup color */
template <class T>
inline void imColorXYZ2RGB(const T X, const T Y, const T Z, 
                           T& R, T& G, T& B, const T& max)
{
  float r =  3.2406f *X - 1.5372f *Y - 0.4986f *Z;
  float g = -0.9689f *X + 1.8758f *Y + 0.0415f *Z;
  float b =  0.0557f *X - 0.2040f *Y + 1.0570f *Z;

  // we need to crop because not all XYZ colors are visible

  R = (T)IM_CROPMAX(r, max);
  G = (T)IM_CROPMAX(g, max);
  B = (T)IM_CROPMAX(b, max);
}

/** Converts Rec 709 RGB to CIE XYZ (all linear). \n
 * ITU-R Recommendation BT.709 (D65 white point). \n
\verbatim
  0 <= XYZ <= 1 ; 0 <= RGB <= 1    

  X = 0.4124 *R + 0.3576 *G + 0.1805 *B
  Y = 0.2126 *R + 0.7152 *G + 0.0722 *B
  Z = 0.0193 *R + 0.1192 *G + 0.9505 *B
\endverbatim
 * \ingroup color */
template <class T>
inline void imColorRGB2XYZ(const T R, const T G, const T B, 
                           T& X, T& Y, T& Z)
{
  X = (T)(0.4124f *R + 0.3576f *G + 0.1805f *B);
  Y = (T)(0.2126f *R + 0.7152f *G + 0.0722f *B);
  Z = (T)(0.0193f *R + 0.1192f *G + 0.9505f *B);

  // there is no need for cropping here, XYZ is already at the limits
}

#define IM_FWLAB(_w) (_w > 0.008856f?               \
                        powf(_w, 1.0f/3.0f):        \
                        7.787f * _w + 0.16f/1.16f)

/** Converts CIE XYZ (linear) to CIE L*a*b* (nonlinear). \n
 * The white point is D65. \n
\verbatim
  0 <= L <= 1 ; -0.5 <= ab <= +0.5 ; 0 <= XYZ <= 1 

  if (t > 0.008856)
    f(t) = pow(t, 1/3)
  else
    f(t) = 7.787*t + 16/116

  fX = f(X / Xn)      fY = f(Y / Yn)      fZ = f(Z / Zn)

  L = 1.16 * fY - 0.16
  a = 2.5 * (fX - fY)
  b = (fY - fZ)

\endverbatim
 * \ingroup color */
inline void imColorXYZ2Lab(const float X, const float Y, const float Z, 
                           float& L, float& a, float& b)
{
  float fX = X / 0.9505f;  // white point D65
  float fY = Y / 1.0f;
  float fZ = Z / 1.0890f;

  fX = IM_FWLAB(fX);
  fY = IM_FWLAB(fY);
  fZ = IM_FWLAB(fZ);

  L = 1.16f * fY - 0.16f;
  a = 2.5f * (fX - fY);
  b = (fY - fZ);
}

#define IM_GWLAB(_w)  (_w > 0.20689f?                     \
                         powf(_w, 3.0f):                  \
                         0.1284f * (_w - 0.16f/1.16f))

/** Converts CIE L*a*b* (nonlinear) to CIE XYZ (linear). \n
 * The white point is D65. \n
 * 0 <= L <= 1 ; -0.5 <= ab <= +0.5 ; 0 <= XYZ <= 1 
 * \ingroup color */
inline void imColorLab2XYZ(const float L, const float a, const float b, 
                           float& X, float& Y, float& Z)

{
  float fY = (L + 0.16f) / 1.16f;
  float gY = IM_GWLAB(fY);

  float fgY = IM_FWLAB(gY);
  float gX = fgY + a / 2.5f;
  float gZ = fgY - b;
  gX = IM_GWLAB(gX);
  gZ = IM_GWLAB(gZ);

  X = gX * 0.9505f;     // white point D65
  Y = gY * 1.0f;
  Z = gZ * 1.0890f;
}

/** Converts CIE XYZ (linear) to CIE L*u*v* (nonlinear). \n
 * The white point is D65. \n
\verbatim
  0 <= L <= 1 ; -1 <= uv <= +1 ; 0 <= XYZ <= 1

  Y = Y / 1.0      (for D65)
  if (Y > 0.008856)
    fY = pow(Y, 1/3)
  else
    fY = 7.787 * Y + 0.16/1.16
  L = 1.16 * fY - 0.16

  U(x, y, z) = (4 * x)/(x + 15 * y + 3 * z)
  V(x, y, z) = (9 * x)/(x + 15 * y + 3 * z)
  un = U(Xn, Yn, Zn) = 0.1978      (for D65)
  vn = V(Xn, Yn, Zn) = 0.4683      (for D65)
  fu = U(X, Y, Z) 
  fv = V(X, Y, Z) 

  u = 13 * L * (fu - un)
  v = 13 * L * (fv - vn)
\endverbatim
 * \ingroup color */
inline void imColorXYZ2Luv(const float X, const float Y, const float Z, 
                           float& L, float& u, float& v)
{
  float XYZ = (float)(X + 15 * Y + 3 * Z);
  float fY = Y / 1.0f;

  if (XYZ != 0)
  {
    L = 1.16f * IM_FWLAB(fY) - 0.16f;
    u = 6.5f * L * ((4 * X)/XYZ - 0.1978f);
    v = 6.5f * L * ((9 * Y)/XYZ - 0.4683f);
  }
  else
  {
    L = u = v = 0;
  }
}

/** Converts CIE L*u*v* (nonlinear) to CIE XYZ (linear). \n
 * The white point is D65.
 * 0 <= L <= 1 ; -0.5 <= uv <= +0.5 ; 0 <= XYZ <= 1 \n
 * \ingroup color */
inline void imColorLuv2XYZ(const float L, const float u, const float v, 
                           float& X, float& Y, float& Z)

{
  float fY = (L + 0.16f) / 1.16f;
  Y = IM_GWLAB(fY) * 1.0f;

  float ul = 0.1978f, vl = 0.4683f;
  if (L != 0)
  {
    ul = u / (6.5f * L) + 0.1978f;
    vl = v / (6.5f * L) + 0.4683f;
  }

  X = ((9 * ul) / (4 * vl)) * Y;
  Z = ((12 - 3 * ul - 20 * vl) / (4 * vl)) * Y;
}

/** Converts nonlinear values to linear values. \n
 * We use the sRGB transfer function. sRGB uses ITU-R 709 primaries and D65 white point. \n
\verbatim
  0 <= l <= 1 ; 0 <= v <= 1 

  if (v < 0.03928)
    l = v / 12.92
  else
    l = pow((v + 0.055) / 1.055, 2.4)
\endverbatim
 * \ingroup color */                           
inline float imColorTransfer2Linear(const float& nonlinear_value)
{
  if (nonlinear_value < 0.03928f)
    return nonlinear_value / 12.92f;
  else
    return powf((nonlinear_value + 0.055f) / 1.055f, 2.4f);
}

/** Converts linear values to nonlinear values. \n
 * We use the sRGB transfer function. sRGB uses ITU-R 709 primaries and D65 white point. \n
\verbatim
  0 <= l <= 1 ; 0 <= v <= 1 

  if (l < 0.0031308)
    v = 12.92 * l
  else
    v = 1.055 * pow(l, 1/2.4) - 0.055
\endverbatim
 * \ingroup color */                           
inline float imColorTransfer2Nonlinear(const float& value)
{
  if (value < 0.0031308f)
    return 12.92f * value;
  else
    return 1.055f * powf(value, 1.0f/2.4f) - 0.055f;
}

/** Converts RGB (linear) to R'G'B' (nonlinear).
 * \ingroup color */
inline void imColorRGB2RGBNonlinear(const float RL, const float GL, const float BL,
                                    float& R, float& G, float& B)
{
  R = imColorTransfer2Nonlinear(RL);
  G = imColorTransfer2Nonlinear(GL);
  B = imColorTransfer2Nonlinear(BL);
}

/** Converts R'G'B' to Y' (all nonlinear). \n
\verbatim
 Y'  =  0.299 *R' + 0.587 *G' + 0.114 *B'
\endverbatim
 * \ingroup color */
template <class T> 
inline T imColorRGB2Luma(const T R, const T G, const T B)
{
  return (T)((299 * R + 587 * G + 114 * B) / 1000);
}

/** Converts Luminance (CIE Y) to Lightness (CIE L*) (all linear). \n
 * The white point is D65.
\verbatim
  0 <= Y <= 1 ; 0 <= L* <= 1

  Y = Y / 1.0      (for D65)
  if (Y > 0.008856)
    fY = pow(Y, 1/3)
  else
    fY = 7.787 * Y + 0.16/1.16
  L = 1.16 * fY - 0.16
\endverbatim
 * \ingroup color */
inline float imColorLuminance2Lightness(const float& Y)
{
  return 1.16f * IM_FWLAB(Y) - 0.16f;
}

/** Converts Lightness (CIE L*) to Luminance (CIE Y) (all linear). \n
 * The white point is D65.
\verbatim
  0 <= Y <= 1 ; 0 <= L* <= 1

  fY = (L + 0.16)/1.16
  if (fY > 0.20689)
    Y = pow(fY, 3)
  else
    Y = 0.1284 * (fY - 0.16/1.16)
  Y = Y * 1.0      (for D65)
\endverbatim
 * \ingroup color */
inline float imColorLightness2Luminance(const float& L)
{
  float fY = (L + 0.16f) / 1.16f;
  return IM_GWLAB(fY);
}

#undef IM_FWLAB
#undef IM_GWLAB
#undef IM_CROPL
#undef IM_CROPC

#endif
