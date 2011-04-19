/** \file
 * \brief Kernel Generators
 * Creates several known kernels
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_kernel.h,v 1.1 2006/04/23 20:55:55 scuri Exp $
 */

#ifndef __IM_KERNEL_H
#define __IM_KERNEL_H

#if	defined(__cplusplus)
extern "C" {
#endif


/** \defgroup kernel Kernel Generators
 * \par
 * Creates several known kernels
 * \par
 * See \ref im_kernel.h
 * \ingroup convolve */


/** Creates a kernel with the following values:
 *
\verbatim
   1  2  1
   0  0  0
  -1 -2 -1
\endverbatim
 *
 * \verbatim im.KernelSobel() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelSobel(void);

/** Creates a kernel with the following values:
 *
\verbatim
   1  1  1
   0  0  0
  -1 -1 -1
\endverbatim
 *
 * \verbatim im.KernelPrewitt() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelPrewitt(void);

/** Creates a kernel with the following values:
 *
\verbatim
   5  5  5
  -3  0 -3
  -3 -3 -3
\endverbatim
 *
 * \verbatim im.KernelKirsh() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelKirsh(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0 -1  0 
  -1  4 -1 
   0 -1  0 
\endverbatim
 *
 * \verbatim im.KernelLaplacian4() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelLaplacian4(void);

/** Creates a kernel with the following values:
 *
\verbatim
  -1 -1 -1 
  -1  8 -1 
  -1 -1 -1 
\endverbatim
 *
 * \verbatim im.KernelLaplacian8() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelLaplacian8(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0 -1 -1 -1  0
  -1  0  1  0 -1
  -1  1  8  1 -1
  -1  0  1  0 -1
   0 -1 -1 -1  0
\endverbatim
 *
 * \verbatim im.KernelLaplacian5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelLaplacian5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
  -1 -1 -1 -1 -1 -1 -1
  -1 -1 -1 -1 -1 -1 -1
  -1 -1 -1 -1 -1 -1 -1
  -1 -1 -1 48 -1 -1 -1
  -1 -1 -1 -1 -1 -1 -1
  -1 -1 -1 -1 -1 -1 -1
  -1 -1 -1 -1 -1 -1 -1
\endverbatim
 *
 * \verbatim im.KernelLaplacian7x7() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelLaplacian7x7(void);

/** Creates a kernel with the following values:
 *
\verbatim
  0 -1 0  
  0  1 0  
  0  0 0  
\endverbatim
 *
 * \verbatim im.KernelGradian3x3() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelGradian3x3(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0 -1 -1  0  1  1  0
  -1 -2 -2  0  2  2  1
  -1 -2 -3  0  3  2  1
  -1 -2 -3  0  3  2  1
  -1 -2 -3  0  3  2  1
  -1 -2 -2  0  2  2  1
   0 -1 -1  0  1  1  0
\endverbatim
 *
 * \verbatim im.KernelGradian7x7() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelGradian7x7(void);

/** Creates a kernel with the following values:
 *
\verbatim
  -1 0 0 
   0 0 0 
   0 0 1 
\endverbatim
 *
 * \verbatim im.KernelSculpt() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelSculpt(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1 1 1 
  1 1 1 
  1 1 1 
\endverbatim
 *
 * \verbatim im.KernelMean3x3() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelMean3x3(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1 1 1 1 1 
  1 1 1 1 1 
  1 1 1 1 1 
  1 1 1 1 1 
  1 1 1 1 1 
\endverbatim
 *
 * \verbatim im.KernelMean5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelMean5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
  0 1 1 1 0 
  1 1 1 1 1 
  1 1 1 1 1 
  1 1 1 1 1 
  0 1 1 1 0 
\endverbatim
 *
 * \verbatim im.KernelMean5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelCircularMean5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
\endverbatim
 *
 * \verbatim im.KernelMean7x7() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelMean7x7(void);

/** Creates a kernel with the following values:
 *
\verbatim
  0 0 1 1 1 0 0 
  0 1 1 1 1 1 0 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  1 1 1 1 1 1 1 
  0 1 1 1 1 1 0 
  0 0 1 1 1 0 0 
\endverbatim
 *
 * \verbatim im.KernelCircularMean7x7() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelCircularMean7x7(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1 2 1 
  2 4 2 
  1 2 1 
\endverbatim
 *
 * \verbatim im.KernelGaussian3x3() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelGaussian3x3(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1  4  6  4 1 
  4 16 24 16 4 
  6 24 36 24 6 
  4 16 24 16 4 
  1  4  6  4 1 
\endverbatim
 *
 * \verbatim im.KernelGaussian5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelGaussian5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
  1 2 3 2 1 
  2 4 6 4 2 
  3 6 9 6 3 
  2 4 6 4 2 
  1 2 3 2 1 
\endverbatim
 *
 * \verbatim im.KernelBarlett5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelBarlett5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0 -1 -1 -1  0 
  -1 -1  3 -1 -1 
  -1  3  4  3 -1 
  -1 -1  3 -1 -1 
   0 -1 -1 -1  0 
\endverbatim
 *
 * \verbatim im.KernelTopHat5x5() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelTopHat5x5(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0  0 -1 -1 -1  0  0
   0 -1 -1 -1 -1 -1  0 
  -1 -1  3  3  3 -1 -1 
  -1 -1  3  4  3 -1 -1 
  -1 -1  3  3  3 -1 -1 
   0 -1 -1 -1 -1 -1  0 
   0  0 -1 -1 -1  0  0 
\endverbatim
 *
 * \verbatim im.KernelTopHat7x7() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelTopHat7x7(void);

/** Creates a kernel with the following values:
 *
\verbatim
   0 -1 -2 -1  0 
  -1 -4  0 -4 -1 
  -2  0 40  0 -2 
  -1 -4  0 -4 -1 
   0 -1 -2 -1  0 
\endverbatim
 *
 * \verbatim im.KernelEnhance() -> kernel: imImage [in Lua 5] \endverbatim
 * \ingroup kernel */
imImage* imKernelEnhance(void);

#if defined(__cplusplus)
}
#endif

#endif

