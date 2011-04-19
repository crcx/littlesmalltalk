/** \file
 * \brief Math Utilities
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_math.h,v 1.2 2005/05/19 19:34:35 uid20021 Exp $
 */

#ifndef __IM_MATH_H
#define __IM_MATH_H

#include <math.h>
#include "im_util.h"

#ifdef IM_DEFMATHFLOAT
inline float acosf(float _X) {return ((float)acos((double)_X)); }
inline float asinf(float _X) {return ((float)asin((double)_X)); }
inline float atanf(float _X) {return ((float)atan((double)_X)); }
inline float atan2f(float _X, float _Y) {return ((float)atan2((double)_X, (double)_Y)); }
inline float ceilf(float _X) {return ((float)ceil((double)_X)); }
inline float cosf(float _X)  {return ((float)cos((double)_X)); }
inline float coshf(float _X) {return ((float)cosh((double)_X)); }
inline float expf(float _X) {return ((float)exp((double)_X)); }
inline float fabsf(float _X) {return ((float)fabs((double)_X)); }
inline float floorf(float _X) {return ((float)floor((double)_X)); }
inline float fmodf(float _X, float _Y) {return ((float)fmod((double)_X, (double)_Y)); }
inline float logf(float _X) {return ((float)log((double)_X)); }
inline float log10f(float _X) {return ((float)log10((double)_X)); }
inline float powf(float _X, float _Y) {return ((float)pow((double)_X, (double)_Y)); }
inline float sinf(float _X) {return ((float)sin((double)_X)); }
inline float sinhf(float _X) {return ((float)sinh((double)_X)); }
inline float sqrtf(float _X) {return ((float)sqrt((double)_X)); }
inline float tanf(float _X) {return ((float)tan((double)_X)); }
inline float tanhf(float _X) {return ((float)tanh((double)_X)); }
#endif

/** \defgroup math Math Utilities
 * \par
 * See \ref im_color.h
 * \ingroup util */


/** Does Zero Order Decimation (Mean).
 * \ingroup math */
template <class T, class TU>
inline T imZeroOrderDecimation(int width, int height, T *map, float xl, float yl, float box_width, float box_height, TU Dummy)
{
  int x0,x1,y0,y1;
  (void)Dummy;

  x0 = (int)floor(xl - box_width/2.0 - 0.5) + 1;
  y0 = (int)floor(yl - box_height/2.0 - 0.5) + 1;
  x1 = (int)floor(xl + box_width/2.0 - 0.5);
  y1 = (int)floor(yl + box_height/2.0 - 0.5);

  if (x0 == x1) x1++;
  if (y0 == y1) y1++;

  x0 = x0<0? 0: x0>width-1? width-1: x0;
  y0 = y0<0? 0: y0>height-1? height-1: y0;
  x1 = x1<0? 0: x1>width-1? width-1: x1;
  y1 = y1<0? 0: y1>height-1? height-1: y1;

  TU Value;
  int Count = 0;

  Value = 0;

  for (int y = y0; y <= y1; y++)
  {
    for (int x = x0; x <= x1; x++)
    {
      Value += map[y*width+x];
      Count++;
    }
  }

  if (Count == 0)
  {
    Value = 0;
    return (T)Value;
  }

  return (T)(Value/(float)Count);
}

/** Does Bilinear Decimation.
 * \ingroup math */
template <class T, class TU>
inline T imBilinearDecimation(int width, int height, T *map, float xl, float yl, float box_width, float box_height, TU Dummy)
{
  int x0,x1,y0,y1;
  (void)Dummy;

  x0 = (int)floor(xl - box_width/2.0 - 0.5) + 1;
  y0 = (int)floor(yl - box_height/2.0 - 0.5) + 1;
  x1 = (int)floor(xl + box_width/2.0 - 0.5);
  y1 = (int)floor(yl + box_height/2.0 - 0.5);

  if (x0 == x1) x1++;
  if (y0 == y1) y1++;

  x0 = x0<0? 0: x0>width-1? width-1: x0;
  y0 = y0<0? 0: y0>height-1? height-1: y0;
  x1 = x1<0? 0: x1>width-1? width-1: x1;
  y1 = y1<0? 0: y1>height-1? height-1: y1;

  TU Value, LineValue;
  float LineNorm, Norm, dxr, dyr;

  Value = 0;
  Norm = 0;

  for (int y = y0; y <= y1; y++)
  {
    dyr = yl - (y+0.5f);
    if (dyr < 0) dyr *= -1;

    LineValue = 0;
    LineNorm = 0;

    for (int x = x0; x <= x1; x++)
    {
      dxr = xl - (x+0.5f);
      if (dxr < 0) dxr *= -1;

      LineValue += map[y*width+x] * dxr;
      LineNorm += dxr;
    }

    Value += LineValue * dyr;
    Norm += dyr * LineNorm;
  }

  if (Norm == 0)
  {
    Value = 0;
    return (T)Value;
  }

  return (T)(Value/Norm);
}

/** Does Zero Order Interpolation (Nearest Neighborhood).
 * \ingroup math */
template <class T>
inline T imZeroOrderInterpolation(int width, int height, T *map, float xl, float yl)
{
  int x0 = (int)(xl-0.5f);
  int y0 = (int)(yl-0.5f);
  x0 = x0<0? 0: x0>width-1? width-1: x0;
  y0 = y0<0? 0: y0>height-1? height-1: y0;
  return map[y0*width + x0];
}

/** Does Bilinear Interpolation.
 * \ingroup math */
template <class T>
inline T imBilinearInterpolation(int width, int height, T *map, float xl, float yl)
{
  int x0 = (int)(xl-0.5f);
  int y0 = (int)(yl-0.5f);
  int x1 = x0+1;
  int y1 = y0+1;

  float t = xl - (x0+0.5f);
  float u = yl - (y0+0.5f);

  x0 = x0<0? 0: x0>width-1? width-1: x0;
  y0 = y0<0? 0: y0>height-1? height-1: y0;
  x1 = x1<0? 0: x1>width-1? width-1: x1;
  y1 = y1<0? 0: y1>height-1? height-1: y1;

  T fll = map[y0*width + x0];
  T fhl = map[y0*width + x1];
  T flh = map[y1*width + x0];
  T fhh = map[y1*width + x1];

  return (T)((fhh - flh - fhl + fll) * u * t +
                         (fhl - fll) * t +
                         (flh - fll) * u +
                                fll);
}

/** Does Bicubic Interpolation.
 * \ingroup math */
template <class T, class TU>
inline T imBicubicInterpolation(int width, int height, T *map, float xl, float yl, TU Dummy)
{
  (void)Dummy;

  int x0 = (int)(xl-0.5f);
  int y0 = (int)(yl-0.5f);
  int x1 = x0-1; 
  int x2 = x0+2;
  int y1 = y0-1; 
  int y2 = y0+2;

  float t = xl - (x0+0.5f);
  float u = yl - (y0+0.5f);

  x1 = x1<0? 0: x1>width-1? width-1: x1;
  y1 = y1<0? 0: y1>height-1? height-1: y1;
  x2 = x2<0? 0: x2>width-1? width-1: x2;
  y2 = y2<0? 0: y2>height-1? height-1: y2;

  float CX[4], CY[4];

  // Optimize calculations
  {
    float x, x2, x3;

#define C0 (-x3 + 2.0f*x2 - x)
#define C1 ( x3 - 2.0f*x2 + 1.0f)
#define C2 (-x3 + x2 + x)
#define C3 ( x3 - x2)

    x = t;
    x2 = x*x; x3 = x2*x;
    CX[0] = C0; CX[1] = C1; CX[2] = C2; CX[3] = C3;

    x = u;
    x2 = x*x; x3 = x2*x;
    CY[0] = C0; CY[1] = C1; CY[2] = C2; CY[3] = C3;
  }

#undef C0
#undef C1
#undef C2
#undef C3

  TU LineValue, Value;
  float LineNorm, Norm;

  Value = 0;
  Norm = 0;

  for (int y = y1; y <= y2; y++)
  {
    LineValue = 0;
    LineNorm = 0;

    for (int x = x1; x <= x2; x++)
    {
      LineValue += map[y*width+x] * CX[x-x1];
      LineNorm += CX[x-x1];
    }

    Value += LineValue * CY[y-y1];
    Norm += CY[y-y1] * LineNorm;
  }

  if (Norm == 0)
  {
    Value = 0;
    return (T)Value;
  }

  Value = (Value/Norm);

  int size = sizeof(T); 
  if (size == 1)
    return (T)(Value<=(TU)0? (TU)0: Value<=(TU)255? Value: (TU)255);
  else
    return (T)(Value);
}

/** Calculates minimum and maximum values.
 * \ingroup math */
template <class T> 
inline void imMinMax(const T *map, int count, T& min, T& max)
{
  min = *map++;
  max = min;
  for (int i = 1; i < count; i++)
  {
    T value = *map++;

    if (value > max)
      max = value;
    else if (value < min)
      min = value;
  }
}

#endif
