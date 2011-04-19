/** \file
 * \brief Image Processing - Local Operations
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process_loc.h,v 1.12 2006/11/22 19:55:32 scuri Exp $
 */

#ifndef __IM_PROCESS_LOC_H
#define __IM_PROCESS_LOC_H

#include "im_image.h"

#if	defined(__cplusplus)
extern "C" {
#endif



/** \defgroup resize Image Resize
 * \par
 * Operations to change the image size.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Only reduze the image size using the given decimation order. \n
 * Supported decimation orders:
 * \li 0 - zero order (mean) 
 * \li 1 - first order (bilinear decimation)
 * Images must be of the same type. If image type is IM_MAP or IM_BINARY, must use order=0. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessReduce(src_image: imImage, dst_image: imImage, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessReduceNew(image: imImage, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
int imProcessReduce(const imImage* src_image, imImage* dst_image, int order);

/** Change the image size using the given interpolation order. \n
 * Supported interpolation orders:
 * \li 0 - zero order (near neighborhood) 
 * \li 1 - first order (bilinear interpolation) 
 * \li 3 - third order (bicubic interpolation)
 * Images must be of the same type. If image type is IM_MAP or IM_BINARY, must use order=0. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessResize(src_image: imImage, dst_image: imImage, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessResizeNew(image: imImage, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
int imProcessResize(const imImage* src_image, imImage* dst_image, int order);

/** Reduze the image area by 4 (w/2,h/2). \n
 * Images must be of the same type. Destiny image size must be source image width/2, height/2.
 * Can not operate on IM_MAP nor IM_BINARY images.
 *
 * \verbatim im.ProcessReduceBy4(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessReduceBy4New(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
void imProcessReduceBy4(const imImage* src_image, imImage* dst_image);

/** Extract a rectangular region from an image. \n
 * Images must be of the same type. Destiny image size must be smaller than source image width-xmin, height-ymin. \n
 * ymin and xmin must be >0 and <size.
 *
 * \verbatim im.ProcessCrop(src_image: imImage, dst_image: imImage, xmin: number, ymin: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessCropNew(image: imImage, xmin: number, xmax: number, ymin: number, ymax: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
void imProcessCrop(const imImage* src_image, imImage* dst_image, int xmin, int ymin);

/** Insert a rectangular region in an image. \n
 * Images must be of the same type. Region image size can be larger than source image. \n
 * ymin and xmin must be >0 and <size. \n
 * Source and destiny must be of the same size. Can be done in place.
 *
 * \verbatim im.ProcessInsert(src_image: imImage, region_image: imImage, dst_image: imImage, xmin: number, ymin: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessInsertNew(image: imImage, region_image: imImage, xmin: number, ymin: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
void imProcessInsert(const imImage* src_image, const imImage* region_image, imImage* dst_image, int xmin, int ymin);

/** Increase the image size by adding pixels with zero value. \n
 * Images must be of the same type. Destiny image size must be greatter than source image width+xmin, height+ymin.
 *
 * \verbatim im.ProcessAddMargins(src_image: imImage, dst_image: imImage, xmin: number, ymin: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessAddMarginsNew(image: imImage, xmin: number, xmax: number, ymin: number, ymax: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup resize */
void imProcessAddMargins(const imImage* src_image, imImage* dst_image, int xmin, int ymin);



/** \defgroup geom Geometric Operations
 * \par
 * Operations to change the shape of the image.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Calculates the size of the new image after rotation.
 *
 * \verbatim im.ProcessCalcRotateSize(width: number, height: number, cos0: number, sin0: number) [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessCalcRotateSize(int width, int height, int *new_width, int *new_height, double cos0, double sin0);

/** Rotates the image using the given interpolation order (see \ref imProcessResize). \n
 * Images must be of the same type. The destiny size can be calculated using \ref imProcessCalcRotateSize to fit the new image size, 
 * or can be any size, including the original size. The rotation is relative to the center of the image. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRotate(src_image: imImage, dst_image: imImage, cos0: number, sin0: number, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRotateNew(image: imImage, cos0: number, sin0: number, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
int imProcessRotate(const imImage* src_image, imImage* dst_image, double cos0, double sin0, int order);

/** Rotates the image using the given interpolation order (see \ref imProcessResize). \n
 * Images must be of the same type. Destiny can have any size, including the original size. \n
 * The rotation is relative to the reference point. But the result can be shifted to the origin. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRotateRef(src_image: imImage, dst_image: imImage, cos0: number, sin0: number, x: number, y: number, to_origin: boolean, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRotateRefNew(image: imImage, cos0: number, sin0: number, x: number, y: number, to_origin: boolean, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
int imProcessRotateRef(const imImage* src_image, imImage* dst_image, double cos0, double sin0, int x, int y, int to_origin, int order);

/** Rotates the image in 90 degrees counterclockwise or clockwise. Swap columns by lines. \n
 * Images must be of the same type. Destiny width and height must be source height and width. \n
 * Direction can be clockwise (1) or counter clockwise (-1).
 *
 * \verbatim im.ProcessRotate90(src_image: imImage, dst_image: imImage, dir_clockwise: boolean) [in Lua 5] \endverbatim
 * \verbatim im.ProcessRotate90New(image: imImage, dir_clockwise: boolean) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessRotate90(const imImage* src_image, imImage* dst_image, int dir_clockwise);

/** Rotates the image in 180 degrees. Swap columns and swap lines. \n
 * Images must be of the same type and size.
 *
 * \verbatim im.ProcessRotate180(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessRotate180New(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessRotate180(const imImage* src_image, imImage* dst_image);

/** Mirror the image in a horizontal flip. Swap columns. \n
 * Images must be of the same type and size.
 * Can be done in-place.
 *
 * \verbatim im.ProcessMirror(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMirrorNew(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessMirror(const imImage* src_image, imImage* dst_image);

/** Apply a vertical flip. Swap lines. \n
 * Images must be of the same type and size.
 * Can be done in-place.
 *
 * \verbatim im.ProcessFlip(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessFlipNew(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessFlip(const imImage* src_image, imImage* dst_image);

/** Apply a radial distortion using the given interpolation order (see imProcessResize). \n
 * Images must be of the same type and size. Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRadial(src_image: imImage, dst_image: imImage, k1: number, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRadialNew(image: imImage, k1: number, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
int imProcessRadial(const imImage* src_image, imImage* dst_image, float k1, int order);

/** Apply a swirl distortion using the given interpolation order (see imProcessResize). \n
 * Images must be of the same type and size. Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessSwirl(src_image: imImage, dst_image: imImage, k: number, order: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessSwirlNew(image: imImage, k: number, order: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
int imProcessSwirl(const imImage* src_image, imImage* dst_image, float k1, int order);

/** Split the image in two images, one containing the odd lines and other containing the even lines. \n
 * Images must be of the same type. Height of the output images must be half the height of the input image.
 * If the height of the input image is odd then the first image must have height equals to half+1.
 *
 * \verbatim im.ProcessInterlaceSplit(src_image: imImage, dst_image1: imImage, dst_image2: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessInterlaceSplitNew(image: imImage) -> new_image1: imImage, new_image2: imImage [in Lua 5] \endverbatim
 * \ingroup geom */
void imProcessInterlaceSplit(const imImage* src_image, imImage* dst_image1, imImage* dst_image2);



/** \defgroup morphgray Morphology Operations for Gray Images
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base gray morphology convolution. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images. \n
 * Kernel is always IM_INT. Use kernel size odd for better results. \n
 * Use -1 for don't care positions in kernel. Kernel values are added to image values, then \n
 * you can use the maximum or the minimum within the kernel area. \n
 * No border extensions are used. 
 * All the gray morphology operations use this function. \n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessGrayMorphConvolve(src_image: imImage, dst_image: imImage, kernel: imImage, ismax: boolean) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphConvolveNew(image: imImage, kernel: imImage, ismax: boolean) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel, int ismax);

/** Gray morphology convolution with a kernel full of "0"s and use minimum value.
 *
 * \verbatim im.ProcessGrayMorphErode(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphErodeNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphErode(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Gray morphology convolution with a kernel full of "0"s and use maximum value.
 *
 * \verbatim im.ProcessGrayMorphDilate(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphDilateNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphDilate(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Erode+Dilate.
 *
 * \verbatim im.ProcessGrayMorphOpen(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphOpenNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphOpen(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Dilate+Erode.
 *
 * \verbatim im.ProcessGrayMorphClose(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphCloseNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphClose(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Open+Difference.
 *
 * \verbatim im.ProcessGrayMorphTopHat(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphTopHatNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphTopHat(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Close+Difference.
 *
 * \verbatim im.ProcessGrayMorphWell(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphWellNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphWell(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Difference(Erode, Dilate).
 *
 * \verbatim im.ProcessGrayMorphGradient(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGrayMorphGradientNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphgray */
int imProcessGrayMorphGradient(const imImage* src_image, imImage* dst_image, int kernel_size);



/** \defgroup morphbin Morphology Operations for Binary Images
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base binary morphology convolution. \n
 * Images are all IM_BINARY. Kernel is IM_INT, but values can be only 1, 0 or -1. Use kernel size odd for better results. \n
 * Hit white means hit=1 and miss=0, or else hit=0 and miss=1. \n
 * Use -1 for don't care positions in kernel. Kernel values are simply compared with image values. \n
 * The operation can be repeated by a number of iterations. 
 * The border is zero extended. \n
 * Almost all the binary morphology operations use this function.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessBinMorphConvolve(src_image: imImage, dst_image: imImage, kernel: imImage, hit_white: boolean, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphConvolveNew(image: imImage, kernel: imImage, hit_white: boolean, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel, int hit_white, int iter);

/** Binary morphology convolution with a kernel full of "1"s and hit white.
 *
 * \verbatim im.ProcessBinMorphErode(src_image: imImage, dst_image: imImage, kernel_size: number, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphErodeNew(image: imImage, kernel_size: number, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphErode(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Binary morphology convolution with a kernel full of "0"s and hit black.
 *
 * \verbatim im.ProcessBinMorphDilate(src_image: imImage, dst_image: imImage, kernel_size: number, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphDilateNew(image: imImage, kernel_size: number, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphDilate(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Erode+Dilate.
 * When iteration is more than one it means Erode+Erode+Erode+...+Dilate+Dilate+Dilate+...
 *
 * \verbatim im.ProcessBinMorphOpen(src_image: imImage, dst_image: imImage, kernel_size: number, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphOpenNew(image: imImage, kernel_size: number, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphOpen(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Dilate+Erode.
 *
 * \verbatim im.ProcessBinMorphClose(src_image: imImage, dst_image: imImage, kernel_size: number, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphCloseNew(image: imImage, kernel_size: number, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphClose(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Erode+Difference. \n
 * The difference from the source image is applied only once.
 *
 * \verbatim im.ProcessBinMorphOutline(src_image: imImage, dst_image: imImage, kernel_size: number, iter: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphOutlineNew(image: imImage, kernel_size: number, iter: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
int imProcessBinMorphOutline(const imImage* src_image, imImage* dst_image, int kernel_size, int iter);

/** Thins the supplied binary image using Rosenfeld's parallel thinning algorithm. \n
 * Reference: \n
 * "Efficient Binary Image Thinning using Neighborhood Maps" \n
 * by Joseph M. Cychosz, 3ksnn64@ecn.purdue.edu              \n
 * in "Graphics Gems IV", Academic Press, 1994
 *
 * \verbatim im.ProcessBinMorphThin(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBinMorphThinNew(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup morphbin */
void imProcessBinMorphThin(const imImage* src_image, imImage* dst_image);



/** \defgroup rank Rank Convolution Operations
 * \par
 * All the rank convolution use the same base function. Near the border the base function 
 * includes only the real image pixels in the rank. No border extensions are used.
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Rank convolution using the median value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images.
 *
 * \verbatim im.ProcessMedianConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessMedianConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup rank */
int imProcessMedianConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using (maximum-minimum) value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images.
 *
 * \verbatim im.ProcessRangeConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRangeConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup rank */
int imProcessRangeConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the closest maximum or minimum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images.
 *
 * \verbatim im.ProcessRankClosestConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRankClosestConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup rank */
int imProcessRankClosestConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the maximum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images.
 *
 * \verbatim im.ProcessRankMaxConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRankMaxConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup rank */
int imProcessRankMaxConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Rank convolution using the minimum value. \n
 * Returns zero if the counter aborted. \n
 * Supports all data types except IM_CFLOAT. Can be applied on color images.
 *
 * \verbatim im.ProcessRankMinConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRankMinConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup rank */
int imProcessRankMinConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Threshold using a rank convolution with a range contrast function. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny. \n
 * Local variable threshold by the method of Bernsen. \n
 * Extracted from XITE, Copyright 1991, Blab, UiO \n
 * http://www.ifi.uio.no/~blab/Software/Xite/
\verbatim
  Reference:	
    Bernsen, J: "Dynamic thresholding of grey-level images"
		Proc. of the 8th ICPR, Paris, Oct 1986, 1251-1255.
  Author:     Oivind Due Trier
\endverbatim
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRangeContrastThreshold(src_image: imImage, dst_image: imImage, kernel_size: number, min_range: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRangeContrastThresholdNew(image: imImage, kernel_size: number, min_range: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessRangeContrastThreshold(const imImage* src_image, imImage* dst_image, int kernel_size, int min_range);

/** Threshold using a rank convolution with a local max function.  \n
 * Returns zero if the counter aborted. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny.
 *
 * \verbatim im.ProcessLocalMaxThreshold(src_image: imImage, dst_image: imImage, kernel_size: number, min_level: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessLocalMaxThresholdNew(image: imImage, kernel_size: number, min_level: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessLocalMaxThreshold(const imImage* src_image, imImage* dst_image, int kernel_size, int min_level);



/** \defgroup convolve Convolution Operations
 * \par
 * See \ref im_process_loc.h
 * \ingroup process */

/** Base Convolution with a kernel. \n
 * Kernel can be IM_INT or IM_FLOAT, but always IM_GRAY. Use kernel size odd for better results. \n
 * Supports all data types. The border is mirrored. \n
 * Returns zero if the counter aborted. Most of the convolutions use this function.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessConvolve(src_image: imImage, dst_image: imImage, kernel: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessConvolveNew(image: imImage, kernel: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessConvolve(const imImage* src_image, imImage* dst_image, const imImage* kernel);

/** Base convolution when the kernel is separable. Only the first line and the first column will be used. \n
 * Returns zero if the counter aborted.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessConvolveSep(src_image: imImage, dst_image: imImage, kernel: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessConvolveSepNew(image: imImage, kernel: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessConvolveSep(const imImage* src_image, imImage* dst_image, const imImage* kernel);

/** Base Convolution with two kernels. The result is the magnitude of the result of each convolution. \n
 * Kernel can be IM_INT or IM_FLOAT, but always IM_GRAY. Use kernel size odd for better results. \n
 * Supports all data types. The border is mirrored. \n
 * Returns zero if the counter aborted. Most of the convolutions use this function.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessConvolveDual(src_image: imImage, dst_image: imImage, kernel1, kernel2: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessConvolveDualNew(image: imImage, kernel1, kernel2: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessConvolveDual(const imImage* src_image, imImage* dst_image, const imImage *kernel1, const imImage *kernel2);

/** Repeats the convolution a number of times. \n
 * Returns zero if the counter aborted.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessConvolveRep(src_image: imImage, dst_image: imImage, kernel: imImage, count: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessConvolveRepNew(image: imImage, kernel: imImage, count: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessConvolveRep(const imImage* src_image, imImage* dst_image, const imImage* kernel, int count);

/** Convolve with a kernel rotating it 8 times and getting the absolute maximum value. \n
 * Kernel must be square. \n
 * The rotation is implemented only for kernel sizes 3x3, 5x5 and 7x7. \n
 * Supports all data types except IM_CFLOAT.
 * Returns zero if the counter aborted.\n
 * If the kernel image attribute "Description" exists it is used by the counter.
 *
 * \verbatim im.ProcessCompassConvolve(src_image: imImage, dst_image: imImage, kernel: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessCompassConvolveNew(image: imImage, kernel: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessCompassConvolve(const imImage* src_image, imImage* dst_image, imImage* kernel);

/** Utility function to rotate a kernel one time.
 *
 * \verbatim im.ProcessRotateKernel(kernel: imImage) [in Lua 5] \endverbatim
 * \ingroup convolve */
void imProcessRotateKernel(imImage* kernel);

/** Difference(Gaussian1, Gaussian2). \n
 * Supports all data types, 
 * but if source is IM_BYTE or IM_USHORT destiny image must be of type IM_INT.
 *
 * \verbatim im.ProcessDiffOfGaussianConvolve(src_image: imImage, dst_image: imImage, stddev1: number, stddev2: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessDiffOfGaussianConvolveNew(image: imImage, stddev1: number, stddev2: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessDiffOfGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev1, float stddev2);

/** Convolution with a laplacian of a gaussian kernel. \n
 * Supports all data types, 
 * but if source is IM_BYTE or IM_USHORT destiny image must be of type IM_INT.
 *
 * \verbatim im.ProcessLapOfGaussianConvolve(src_image: imImage, dst_image: imImage, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessLapOfGaussianConvolveNew(image: imImage, stddev: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessLapOfGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev);

/** Convolution with a kernel full of "1"s inside a circle. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessMeanConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessMeanConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessMeanConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Convolution with a float gaussian kernel. \n
 * If sdtdev is negative its magnitude will be used as the kernel size. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessGaussianConvolve(src_image: imImage, dst_image: imImage, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessGaussianConvolveNew(image: imImage, stddev: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessGaussianConvolve(const imImage* src_image, imImage* dst_image, float stddev);

/** Convolution with a barlett kernel. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessBarlettConvolve(src_image: imImage, dst_image: imImage, kernel_size: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessBarlettConvolveNew(image: imImage, kernel_size: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessBarlettConvolve(const imImage* src_image, imImage* dst_image, int kernel_size);

/** Magnitude of the sobel convolution. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessSobelConvolve(src_image: imImage, dst_image: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessSobelConvolveNew(image: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessSobelConvolve(const imImage* src_image, imImage* dst_image);

/** Magnitude of the prewitt convolution. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessPrewittConvolve(src_image: imImage, dst_image: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessPrewittConvolveNew(image: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessPrewittConvolve(const imImage* src_image, imImage* dst_image);

/** Spline edge dectection. \n
 * Supports all data types.
 *
 * \verbatim im.ProcessSplineEdgeConvolve(src_image: imImage, dst_image: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessSplineEdgeConvolveNew(image: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
int imProcessSplineEdgeConvolve(const imImage* src_image, imImage* dst_image);

/** Finds the zero crossings of IM_INT and IM_FLOAT images. Crossings are marked with non zero values
 * indicating the intensity of the edge. It is usually used after a second derivative, laplace. \n
 * Extracted from XITE, Copyright 1991, Blab, UiO \n
 * http://www.ifi.uio.no/~blab/Software/Xite/
 *
 * \verbatim im.ProcessZeroCrossing(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessZeroCrossingNew(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
void imProcessZeroCrossing(const imImage* src_image, imImage* dst_image);

/** First part of the Canny edge detector. Includes the gaussian filtering and the nonmax suppression. \n
 * After using this you could apply a Hysteresis Threshold, see \ref imProcessHysteresisThreshold. \n
 * Image must be IM_BYTE/IM_GRAY. \n
 * Implementation from the book:
 \verbatim
    J. R. Parker
    "Algoritms for Image Processing and Computer Vision"
    WILEY
 \endverbatim
 *
 * \verbatim im.ProcessCanny(src_image: imImage, dst_image: imImage, stddev: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessCannyNew(image: imImage, stddev: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup convolve */
void imProcessCanny(const imImage* src_image, imImage* dst_image, float stddev);

/** Calculates the kernel size given the standard deviation. \n
 * If sdtdev is negative its magnitude will be used as the kernel size.
 *
 * \verbatim im.GaussianStdDev2KernelSize(stddev: number) -> kernel_size: number [in Lua 5] \endverbatim
 * \ingroup convolve */
int imGaussianStdDev2KernelSize(float stddev);

/** Calculates the standard deviation given the kernel size.
 *
 * \verbatim im.GaussianKernelSize2StdDev(kernel_size: number) -> stddev: number [in Lua 5] \endverbatim
 * \ingroup convolve */
float imGaussianKernelSize2StdDev(int kernel_size);


#if defined(__cplusplus)
}
#endif

#endif
