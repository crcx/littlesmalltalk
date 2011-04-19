/** \file
 * \brief Image Processing - Pontual Operations
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process_pon.h,v 1.9 2006/11/24 13:18:06 scuri Exp $
 */

#ifndef __IM_PROCESS_PON_H
#define __IM_PROCESS_PON_H

#include "im_image.h"

#if	defined(__cplusplus)
extern "C" {
#endif



/** \defgroup arithm Arithmetic Operations 
 * \par
 * Simple math operations for images.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Unary Arithmetic Operations.
 * Inverse and log may lead to math exceptions.
 * \ingroup arithm */
enum imUnaryOp {
  IM_UN_EQL,    /**< equal             =     a        */
  IM_UN_ABS,    /**< abssolute         =    |a|       */
  IM_UN_LESS,   /**< less              =    -a        */
  IM_UN_INC,    /**< increment        +=     a        */
  IM_UN_INV,    /**< invert            =   1/a       (#) */
  IM_UN_SQR,    /**< square            =     a*a      */
  IM_UN_SQRT,   /**< square root       =     a^(1/2)  */
  IM_UN_LOG,    /**< natural logarithm =  ln(a)      (#) */
  IM_UN_EXP,    /**< exponential       = exp(a)       */
  IM_UN_SIN,    /**< sine              = sin(a)       */
  IM_UN_COS,    /**< cosine            = cos(a)       */
  IM_UN_CONJ,   /**< complex conjugate =     ar - ai*i                   */
  IM_UN_CPXNORM /**< complex normalization by magnitude = a / cpxmag(a)  */
};

/** Apply an arithmetic unary operation. \n
 * Can be done in place, images must match size, does not need to match type.
 *
 * \verbatim im.ProcessUnArithmeticOp(src_image: imImage, dst_image: imImage, op: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessUnArithmeticOpNew(image: imImage, op: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessUnArithmeticOp(const imImage* src_image, imImage* dst_image, int op);

/** Binary Arithmetic Operations.
 * Inverse and log may lead to math exceptions.
 * \ingroup arithm */
enum imBinaryOp {
  IM_BIN_ADD,    /**< add         =    a+b            */
  IM_BIN_SUB,    /**< subtract    =    a-b            */
  IM_BIN_MUL,    /**< multiply    =    a*b            */
  IM_BIN_DIV,    /**< divide      =    a/b            (#) */
  IM_BIN_DIFF,   /**< difference  =    |a-b|          */
  IM_BIN_POW,    /**< power       =    a^b            */
  IM_BIN_MIN,    /**< minimum     =    (a < b)? a: b  */
  IM_BIN_MAX     /**< maximum     =    (a > b)? a: b  */
};

/** Apply a binary arithmetic operation. \n
 * Can be done in place, images must match size. \n
 * Source images must match type, destiny image can be several types depending on source: \n
 * \li byte -> byte, ushort, int, float
 * \li ushort -> ushort, int, float
 * \li int -> int, float
 * \li float -> float
 * \li complex -> complex
 * One exception is that you can combine complex with float resulting complex.
 *
 * \verbatim im.ProcessArithmeticOp(src_image1: imImage, src_image2: imImage, dst_image: imImage, op: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessArithmeticOpNew(image1: imImage, image2: imImage, op: number) -> new_image: imImage [in Lua 5] \endverbatim
 * The New function will create a new image of the same type of the source images.
 * \ingroup arithm */
void imProcessArithmeticOp(const imImage* src_image1, const imImage* src_image2, imImage* dst_image, int op);

/** Apply a binary arithmetic operation with a constant value. \n
 * Can be done in place, images must match size. \n
 * Destiny image can be several types depending on source: \n
 * \li byte -> byte, ushort, int, float
 * \li ushort -> byte, ushort, int, float
 * \li int -> byte, ushort, int, float
 * \li float -> float
 * \li complex -> complex
 * The constant value is type casted to an apropriate type before the operation.
 *
 * \verbatim im.ProcessArithmeticConstOp(src_image: imImage, src_const: number, dst_image: imImage, op: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessArithmeticConstOpNew(image: imImage, src_const: number, op: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessArithmeticConstOp(const imImage* src_image, float src_const, imImage* dst_image, int op);

/** Blend two images using an alpha value = [a * alpha + b * (1 - alpha)]. \n
 * Can be done in place, images must match size and type. \n
 * alpha value must be in the interval [0.0 - 1.0].
 *
 * \verbatim im.ProcessBlend(src_image1: imImage, src_image2: imImage, dst_image: imImage, alpha: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBlendNew(image1: imImage, image2: imImage, alpha: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessBlendConst(const imImage* src_image1, const imImage* src_image2, imImage* dst_image, float alpha);

/** Blend two images using an alpha channel = [a * alpha + b * (1 - alpha)]. \n
 * Can be done in place, images must match size and type. \n
 * alpha_image must have the same data type except for complex images that must be float, and color_space must be IM_GRAY.
 * integer alpha values must be:
\verbatim 
0 - 255        IM_BYTE  
0 - 65535      IM_USHORT
0 - 2147483647 IM_INT
\endverbatim
 * that will be normalized to 0 - 1.
 * \verbatim im.ProcessBlend(src_image1: imImage, src_image2: imImage, alpha_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBlendNew(image1: imImage, image2: imImage, alpha_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessBlend(const imImage* src_image1, const imImage* src_image2, const imImage* alpha_image, imImage* dst_image);

/** Split a complex image into two images with real and imaginary parts \n
 * or magnitude and phase parts (polar). \n
 * Source image must be IM_CFLOAT, destiny images must be IM_FLOAT.
 *
 * \verbatim im.ProcessSplitComplex(src_image: imImage, dst_image1: imImage, dst_image2: imImage, do_polar: boolean) [in Lua 5] \endverbatim
 * \verbatim im.ProcessSplitComplexNew(image: imImage, do_polar: boolean) -> dst_image1: imImage, dst_image2: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessSplitComplex(const imImage* src_image, imImage* dst_image1, imImage* dst_image2, int do_polar);

/** Merges two images as the real and imaginary parts of a complex image, \n
 * or as magnitude and phase parts (polar = 1). \n
 * Source images must be IM_FLOAT, destiny image must be IM_CFLOAT.
 *
 * \verbatim im.ProcessMergeComplex(src_image1: imImage, src_image2: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMergeComplexNew(image1: imImage, image2: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessMergeComplex(const imImage* src_image1, const imImage* src_image2, imImage* dst_image, int polar);

/** Calculates the mean of multiple images. \n
 * Images must match size and type.
 *
 * \verbatim im.ProcessMultipleMean(src_image_list: table of imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMultipleMeanNew(src_image_list: table of imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessMultipleMean(const imImage** src_image_list, int src_image_count, imImage* dst_image);

/** Calculates the standard deviation of multiple images. \n
 * Images must match size and type. Use \ref imProcessMultipleMean to calculate the mean_image.
 *
 * \verbatim im.ProcessMultipleStdDev(src_image_list: table of imImage, mean_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMultipleStdDevNew(src_image_list: table of imImage, mean_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessMultipleStdDev(const imImage** src_image_list, int src_image_count, const imImage *mean_image, imImage* dst_image);

/** Calculates the auto-covariance of an image with the mean of a set of images. \n
 * Images must match size and type. Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessAutoCovariance(src_image: imImage, mean_image: imImage, dst_image: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessAutoCovarianceNew(src_image: imImage, mean_image: imImage) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
int imProcessAutoCovariance(const imImage* src_image, const imImage* mean_image, imImage* dst_image);

/** Multiplies the conjugate of one complex image with another complex image. \n
 * Images must match size. Conj(img1) * img2 \n
 * Can be done in-place.
 *
 * \verbatim im.ProcessMultiplyConj(src_image1: imImage, src_image2: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMultiplyConjNew(src_image1: imImage, src_image2: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup arithm */
void imProcessMultiplyConj(const imImage* src_image1, const imImage* src_image2, imImage* dst_image);



/** \defgroup quantize Additional Image Quantization Operations
 * \par
 * Additionally operations to the \ref imConvertColorSpace function.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Converts a RGB image to a MAP image using uniform quantization 
 * with an optional 8x8 ordered dither. The RGB image must have data type IM_BYTE.
 *
 * \verbatim im.ProcessQuantizeRGBUniform(src_image: imImage, dst_image: imImage, do_dither: boolean) [in Lua 5] \endverbatim
 * \verbatim im.ProcessQuantizeRGBUniformNew(src_image: imImage, do_dither: boolean) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup quantize */
void imProcessQuantizeRGBUniform(const imImage* src_image, imImage* dst_image, int do_dither);

/** Quantizes a gray scale image in less that 256 grays using uniform quantization. \n
 * Both images must be IM_BYTE/IM_GRAY. Can be done in place. 
 *
 * \verbatim im.ProcessQuantizeGrayUniform(src_image: imImage, dst_image: imImage, grays: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessQuantizeGrayUniformNew(src_image: imImage, grays: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup quantize */
void imProcessQuantizeGrayUniform(const imImage* src_image, imImage* dst_image, int grays);



/** \defgroup histo Histogram Based Operations
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Performs an histogram expansion based on a percentage of the number of pixels. \n
 * Percentage defines an amount of pixels to include at the lowest level and at the highest level.
 * If its is zero only empty counts of the histogram will be considered. \n
 * Images must be IM_BYTE/(IM_RGB or IM_GRAY). Can be done in place. \n
 * To expand the gammut without using the histogram, by just specifing the lowest and highest levels
 * use the \ref IM_GAMUT_EXPAND tone gammut operation (\ref imProcessToneGamut).
 *
 * \verbatim im.ProcessExpandHistogram(src_image: imImage, dst_image: imImage, percent: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessExpandHistogramNew(src_image: imImage, percent: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup histo */
void imProcessExpandHistogram(const imImage* src_image, imImage* dst_image, float percent);

/** Performs an histogram equalization. \n
 * Images must be IM_BYTE/(IM_RGB or IM_GRAY). Can be done in place. 
 *
 * \verbatim im.ProcessEqualizeHistogram(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessEqualizeHistogramNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup histo */
void imProcessEqualizeHistogram(const imImage* src_image, imImage* dst_image);



/** \defgroup colorproc Color Processing Operations
 * \par
 * Operations to change the color components configuration.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Split a RGB image into luma and chroma. \n
 * Chroma is calculated as R-Y,G-Y,B-Y. Source image must be IM_RGB/IM_BYTE. \n
 * luma image is IM_GRAY/IM_BYTE and chroma is IM_RGB/IM_BYTE. \n
 * Source and destiny must have the same size. 
 *
 * \verbatim im.ProcessSplitYChroma(src_image: imImage, y_image: imImage, chroma_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessSplitYChromaNew(src_image: imImage) -> y_image: imImage, chroma_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessSplitYChroma(const imImage* src_image, imImage* y_image, imImage* chroma_image);

/** Split a RGB image into HSI planes. \n
 * Source image must be IM_RGB/IM_BYTE,IM_FLOAT. Destiny images are all IM_GRAY/IM_FLOAT. \n
 * Source images must normalized to 0-1 if type is IM_FLOAT (\ref imProcessToneGamut can be used). See \ref hsi for a definition of the color conversion.\n
 * Source and destiny must have the same size. 
 *
 * \verbatim im.ProcessSplitHSI(src_image: imImage, h_image: imImage, s_image: imImage, i_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessSplitHSINew(src_image: imImage) -> h_image: imImage, s_image: imImage, i_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessSplitHSI(const imImage* src_image, imImage* h_image, imImage* s_image, imImage* i_image);

/** Merge HSI planes into a RGB image. \n
 * Source images must be IM_GRAY/IM_FLOAT. Destiny image can be IM_RGB/IM_BYTE,IM_FLOAT. \n
 * Source and destiny must have the same size. See \ref hsi for a definition of the color conversion.
 *
 * \verbatim im.ProcessMergeHSI(h_image: imImage, s_image: imImage, i_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMergeHSINew(h_image: imImage, s_image: imImage, i_image: imImage) -> dst_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessMergeHSI(const imImage* h_image, const imImage* s_image, const imImage* i_image, imImage* dst_image);

/** Split a multicomponent image into separate components.\n
 * Destiny images must be IM_GRAY. Size and data types must be all the same.\n
 * The number of destiny images must match the depth of the source image.
 *
 * \verbatim im.ProcessSplitComponents(src_image: imImage, dst_image_list: table of imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessSplitComponentsNew(src_image: imImage) -> dst_image_list: table of imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessSplitComponents(const imImage* src_image, imImage** dst_image_list);

/** Merges separate components into a multicomponent image.\n
 * Source images must be IM_GRAY. Size and data types must be all the same.\n
 * The number of source images must match the depth of the destiny image.
 *
 * \verbatim im.ProcessMergeComponents(src_image_list: table of imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessMergeComponentsNew(src_image_list: table of imImage) -> dst_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessMergeComponents(const imImage** src_image_list, imImage* dst_image);

/** Normalize the color components by their sum. Example: c1 = c1/(c1+c2+c3). \n
 * Destiny image must be IM_FLOAT. 
 *
 * \verbatim im.ProcessNormalizeComponents(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessNormalizeComponentsNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessNormalizeComponents(const imImage* src_image, imImage* dst_image);

/** Replaces the source color by the destiny color. \n
 * The color will be type casted to the image data type. \n
 * The colors must have the same number of components of the images. \n
 * Supports all color spaces and all data types except IM_CFLOAT.
 *
 * \verbatim im.ProcessReplaceColor(src_image: imImage, dst_image: imImage, src_color: table of numbers, dst_color: table of numbers) [in Lua 5] \endverbatim
 * \verbatim im.ProcessReplaceColorNew(src_image: imImage, src_color: table of numbers, dst_color: table of numbers) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup colorproc */
void imProcessReplaceColor(const imImage* src_image, imImage* dst_image, float* src_color, float* dst_color);



/** \defgroup logic Logical Arithmetic Operations 
 * \par
 * Logical binary math operations for images.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Logical Operations.
 * \ingroup logic */
enum imLogicOp {
  IM_BIT_AND,   /**< and  =   a & b   */
  IM_BIT_OR,    /**< or   =   a | b   */
  IM_BIT_XOR    /**< xor  = ~(a | b)  */
};

/** Apply a logical operation.\n
 * Images must have data type IM_BYTE, IM_USHORT or IM_INT. Can be done in place. 
 *
 * \verbatim im.ProcessBitwiseOp(src_image1: imImage, src_image2: imImage, dst_image: imImage, op: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBitwiseOpNew(src_image1: imImage, src_image2: imImage, op: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup logic */
void imProcessBitwiseOp(const imImage* src_image1, const imImage* src_image2, imImage* dst_image, int op);

/** Apply a logical NOT operation.\n
 * Images must have data type IM_BYTE, IM_USHORT or IM_INT. Can be done in place. 
 *
 * \verbatim im.ProcessBitwiseNot(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBitwiseNotNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup logic */
void imProcessBitwiseNot(const imImage* src_image, imImage* dst_image);

/** Apply a bit mask. \n
 * The same as imProcessBitwiseOp but the second image is replaced by a fixed mask. \n
 * Images must have data type IM_BYTE. It is valid only for AND, OR and XOR. Can be done in place.
 *
 * \verbatim im.ProcessBitMask(src_image: imImage, dst_image: imImage, mask: string, op: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBitMaskNew(src_image: imImage, mask: string, op: number) -> new_image: imImage [in Lua 5] \endverbatim
 * In Lua, mask is a string with 0s and 1s, for example: "11001111".
 * \ingroup logic */
void imProcessBitMask(const imImage* src_image, imImage* dst_image, unsigned char mask, int op);

/** Extract or Reset a bit plane. For ex: 000X0000 or XXX0XXXX (plane=3).\n
 * Images must have data type IM_BYTE. Can be done in place. 
 *
 * \verbatim im.ProcessBitPlane(src_image: imImage, dst_image: imImage, plane: number, do_reset: boolean) [in Lua 5] \endverbatim
 * \verbatim im.ProcessBitPlaneNew(src_image: imImage, plane: number, do_reset: boolean) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup logic */
void imProcessBitPlane(const imImage* src_image, imImage* dst_image, int plane, int do_reset);



/** \defgroup render Synthetic Image Render
 * \par
 * Renders some 2D mathematical functions as images. All the functions operates in place 
 * and supports all data types except IM_CFLOAT.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Render Funtion.
 * \verbatim render_func(x: number, y: number, d: number, param: table of number) -> value: number [in Lua 5] \endverbatim
 * \ingroup render */
typedef float (*imRenderFunc)(int x, int y, int d, float* param);

/** Render Conditional Funtion.
 * \verbatim render_cond_func(x: number, y: number, d: number, param: table of number) -> value: number, cond: boolean [in Lua 5] \endverbatim
 * \ingroup render */
typedef float (*imRenderCondFunc)(int x, int y, int d, int *cond, float* param);

/** Render a synthetic image using a render function. \n
 * plus will make the render be added to the current image data, 
 * or else all data will be replaced. All the render functions use this or the conditional function. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRenderOp(image: imImage, render_func: function, render_name: string, param: table of number, plus: boolean) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderOp(imImage* image, imRenderFunc render_func, char* render_name, float* param, int plus);

/** Render a synthetic image using a conditional render function. \n
 * Data will be rendered only if the condional param is true. \n
 * Returns zero if the counter aborted.
 *
 * \verbatim im.ProcessRenderCondOp(image: imImage, render_cond_func: function, render_name: string, param: table of number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderCondOp(imImage* image, imRenderCondFunc render_cond_func, char* render_name, float* param);

/** Render speckle noise on existing data. Can be done in place.
 *
 * \verbatim im.ProcessRenderAddSpeckleNoise(src_image: imImage, dst_image: imImage, percent: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRenderAddSpeckleNoiseNew(src_image: imImage, percent: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderAddSpeckleNoise(const imImage* src_image, imImage* dst_image, float percent);

/** Render gaussian noise on existing data. Can be done in place.
 *
 * \verbatim im.ProcessRenderAddGaussianNoise(src_image: imImage, dst_image: imImage, mean: number, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRenderAddGaussianNoiseNew(src_image: imImage, mean: number, stddev: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderAddGaussianNoise(const imImage* src_image, imImage* dst_image, float mean, float stddev);

/** Render uniform noise on existing data. Can be done in place.
 *
 * \verbatim im.ProcessRenderAddUniformNoise(src_image: imImage, dst_image: imImage, mean: number, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \verbatim im.ProcessRenderAddUniformNoiseNew(src_image: imImage, mean: number, stddev: number) -> counter: boolean, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderAddUniformNoise(const imImage* src_image, imImage* dst_image, float mean, float stddev);

/** Render random noise.
 *
 * \verbatim im.ProcessRenderRandomNoise(image: imImage) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderRandomNoise(imImage* image);

/** Render a constant. The number of values must match the depth of the image.
 *
 * \verbatim im.ProcessRenderConstant(image: imImage, value: table of number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderConstant(imImage* image, float* value);

/** Render a centered wheel.
 *
 * \verbatim im.ProcessRenderWheel(image: imImage, internal_radius: number, external_radius: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderWheel(imImage* image, int internal_radius, int external_radius);

/** Render a centered cone.
 *
 * \verbatim im.ProcessRenderCone(image: imImage, radius: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderCone(imImage* image, int radius);

/** Render a centered tent.
 *
 * \verbatim im.ProcessRenderTent(image: imImage, tent_width: number, tent_height: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderTent(imImage* image, int tent_width, int tent_height);

/** Render a ramp. Direction can be vertical (1) or horizontal (0).
 *
 * \verbatim im.ProcessRenderRamp(image: imImage, start: number, end: number, vert_dir: boolean) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderRamp(imImage* image, int start, int end, int vert_dir);

/** Render a centered box.
 *
 * \verbatim im.ProcessRenderBox(image: imImage, box_width: number, box_height: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderBox(imImage* image, int box_width, int box_height);

/** Render a centered sinc.
 *
 * \verbatim im.ProcessRenderSinc(image: imImage, x_period: number, y_period: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderSinc(imImage* image, float x_period, float y_period);

/** Render a centered gaussian.
 *
 * \verbatim im.ProcessRenderGaussian(image: imImage, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderGaussian(imImage* image, float stddev);

/** Render the laplacian of a centered gaussian.
 *
 * \verbatim im.ProcessRenderLapOfGaussian(image: imImage, stddev: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderLapOfGaussian(imImage* image, float stddev);

/** Render a centered cosine.
 *
 * \verbatim im.ProcessRenderCosine(image: imImage, x_period: number, y_period: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderCosine(imImage* image, float x_period, float y_period);

/** Render a centered grid.
 *
 * \verbatim im.ProcessRenderGrid(image: imImage, x_space: number, y_space: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderGrid(imImage* image, int x_space, int y_space);

/** Render a centered chessboard.
 *
 * \verbatim im.ProcessRenderChessboard(image: imImage, x_space: number, y_space: number) -> counter: boolean [in Lua 5] \endverbatim
 * \ingroup render */
int imProcessRenderChessboard(imImage* image, int x_space, int y_space);



/** \defgroup tonegamut Tone Gamut Operations
 * \par
 * Operations that try to preserve the min-max interval in the output (the dynamic range).
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */


/** Tone Gamut Operations.
 * \ingroup tonegamut */
enum imToneGamut {
  IM_GAMUT_NORMALIZE, /**< normalize = (a-min) / (max-min)     (destiny image must be IM_FLOAT)   */
  IM_GAMUT_POW,       /**< pow       = ((a-min) / (max-min))^gamma * (max-min) + min                  \n
                                       param[0]=gamma                                             */
  IM_GAMUT_LOG,       /**< log       = log(K * (a-min) / (max-min) + 1))*(max-min)/log(K+1) + min     \n
                                       param[0]=K     (K>0)                                       */
  IM_GAMUT_EXP,       /**< exp       = (exp(K * (a-min) / (max-min)) - 1))*(max-min)/(exp(K)-1) + min \n
                                       param[0]=K                                                 */
  IM_GAMUT_INVERT,    /**< invert    = max - (a-min)                                              */
  IM_GAMUT_ZEROSTART, /**< zerostart = a - min                                                    */
  IM_GAMUT_SOLARIZE,  /**< solarize  = a < level ?  a:  (level * (max-min) - a * (level-min)) / (max-level) \n
                                       param[0]=level percentage (0-100) relative to min-max      \n
                                       photography solarization effect. */
  IM_GAMUT_SLICE,     /**< slice     = start < a || a > end ?  min:  binarize?  max: a                     \n
                                       param[0]=start,  param[1]=end,  param[2]=binarize          */
  IM_GAMUT_EXPAND,    /**< expand    = a < start ?  min: a > end ? max :  (a-start)*(max-min)/(end-start) + min  \n
                                       param[0]=start,  param[1]=end                              */
  IM_GAMUT_CROP,      /**< crop      = a < start ?  start: a > end ? end : a                                        \n
                                       param[0]=start,  param[1]=end                              */
  IM_GAMUT_BRIGHTCONT /**< brightcont = a < min ?  min:  a > max ?  max:  a * tan(c_a) + b_s + (max-min)*(1 - tan(c_a))/2  \n
                                        param[0]=bright_shift (-100%..+100%),  param[1]=contrast_factor (-100%..+100%)     \n
                                        change brightness and contrast simultaneously. */
};

/** Apply a gamut operation with arguments. \n
 * Supports all data types except IM_CFLOAT. \n
 * The linear operation do a special convertion when min > 0 and max < 1, it forces min=0 and max=1. \n
 * IM_BYTE images have min=0 and max=255 always. \n
 * Can be done in place. When there is no extra params use NULL.
 *
 * \verbatim im.ProcessToneGamut(src_image: imImage, dst_image: imImage, op: number, param: table of number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessToneGamutNew(src_image: imImage, op: number, param: table of number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup tonegamut */
void imProcessToneGamut(const imImage* src_image, imImage* dst_image, int op, float* param);

/** Converts from (0-1) to (0-255), crop out of bounds values. \n
 * Source image must be IM_FLOAT, and destiny image must be IM_BYTE.
 *
 * \verbatim im.ProcessUnNormalize(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessUnNormalizeNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup tonegamut */
void imProcessUnNormalize(const imImage* src_image, imImage* dst_image);

/** Directly converts IM_USHORT, IM_INT and IM_FLOAT into IM_BYTE images. \n
 * This can also be done using \ref imConvertDataType with IM_CAST_DIRECT.
 *
 * \verbatim im.ProcessDirectConv(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessDirectConvNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup tonegamut */
void imProcessDirectConv(const imImage* src_image, imImage* dst_image);

/** A negative effect. Uses \ref imProcessToneGamut with IM_GAMUT_INVERT for non MAP images. \n
 * Supports all color spaces and all data types except IM_CFLOAT. \n
 * Can be done in place. 
 *
 * \verbatim im.ProcessNegative(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessNegativeNew(src_image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup tonegamut */
void imProcessNegative(const imImage* src_image, imImage* dst_image);



/** \defgroup threshold Threshold Operations
 * \par
 * Operations that converts a usually IM_GRAY/IM_BYTE image into a IM_BINARY image using several threshold techniques.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */

/** Apply a manual threshold. \n
 * threshold = a <= level ? 0: value \n
 * Normal value is 1 but another common value is 255. Can be done in place for IM_BYTE source. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny.
 *
 * \verbatim im.ProcessThreshold(src_image: imImage, dst_image: imImage, level: number, value: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessThresholdNew(src_image: imImage, level: number, value: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessThreshold(const imImage* src_image, imImage* dst_image, int level, int value);

/** Apply a threshold by the difference of two images. \n
 * threshold = a1 <= a2 ? 0: 1   \n
 * Can be done in place. 
 *
 * \verbatim im.ProcessThresholdByDiff(src_image1: imImage, src_image2: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessThresholdByDiffNew(src_image1: imImage, src_image2: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessThresholdByDiff(const imImage* src_image1, const imImage* src_image2, imImage* dst_image);

/** Apply a threshold by the Hysteresis method. \n
 * Hysteresis thersholding of edge pixels. Starting at pixels with a
 * value greater than the HIGH threshold, trace a connected sequence
 * of pixels that have a value greater than the LOW threhsold. \n
 * Note: could not find the original source code author name.
 *
 * \verbatim im.ProcessHysteresisThreshold(src_image: imImage, dst_image: imImage, low_thres: number, high_thres: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessHysteresisThresholdNew(src_image: imImage, low_thres: number, high_thres: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessHysteresisThreshold(const imImage* src_image, imImage* dst_image, int low_thres, int high_thres);

/** Estimates hysteresis low and high threshold levels. \n
 * Usefull for \ref imProcessHysteresisThreshold.
 *
 * \verbatim im.ProcessHysteresisThresEstimate(image: imImage) -> low_level: number, high_level: number [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessHysteresisThresEstimate(const imImage* image, int *low_level, int *high_level);

/** Calculates the threshold level for manual threshold using an uniform error approach. \n
 * Extracted from XITE, Copyright 1991, Blab, UiO \n
 * http://www.ifi.uio.no/~blab/Software/Xite/
\verbatim
  Reference:
    S. M. Dunn & D. Harwood & L. S. Davis:
    "Local Estimation of the Uniform Error Threshold"
    IEEE Trans. on PAMI, Vol PAMI-6, No 6, Nov 1984.
  Comments: It only works well on images whith large objects.
  Author: Olav Borgli, BLAB, ifi, UiO
  Image processing lab, Department of Informatics, University of Oslo
\endverbatim
 * Returns the used level.
 *
 * \verbatim im.ProcessUniformErrThreshold(src_image: imImage, dst_image: imImage) -> level: number [in Lua 5] \endverbatim
 * \verbatim im.ProcessUniformErrThresholdNew(src_image: imImage)  -> level: number, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessUniformErrThreshold(const imImage* src_image, imImage* dst_image);

/** Apply a dithering on each image channel by using a difusion error method. \n
 * It can be applied on any IM_BYTE images. It will "threshold" each channel indivudually, so
 * source and destiny must be of the same depth.
 *
 * \verbatim im.ProcessDifusionErrThreshold(src_image: imImage, dst_image: imImage, level: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessDifusionErrThresholdNew(src_image: imImage, level: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessDifusionErrThreshold(const imImage* src_image, imImage* dst_image, int level);

/** Calculates the threshold level for manual threshold using a percentage of pixels
 * that should stay bellow the threshold. \n
 * Returns the used level.
 *
 * \verbatim im.ProcessPercentThreshold(src_image: imImage, dst_image: imImage, percent: number) -> level: number [in Lua 5] \endverbatim
 * \verbatim im.ProcessPercentThresholdNew(src_image: imImage, percent: number) -> level: number, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessPercentThreshold(const imImage* src_image, imImage* dst_image, float percent);

/** Calculates the threshold level for manual threshold using the Otsu approach. \n
 * Returns the used level. \n
 * Original implementation by Flavio Szenberg.
 *
 * \verbatim im.ProcessOtsuThreshold(src_image: imImage, dst_image: imImage) -> level: number [in Lua 5] \endverbatim
 * \verbatim im.ProcessOtsuThresholdNew(src_image: imImage) -> level: number, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessOtsuThreshold(const imImage* src_image, imImage* dst_image);

/** Calculates the threshold level for manual threshold using (max-min)/2. \n
 * Returns the used level. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny.
 *
 * \verbatim im.ProcessMinMaxThreshold(src_image: imImage, dst_image: imImage) -> level: number [in Lua 5] \endverbatim
 * \verbatim im.ProcessMinMaxThresholdNew(src_image: imImage) -> level: number, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
int imProcessMinMaxThreshold(const imImage* src_image, imImage* dst_image);

/** Estimates Local Max threshold level for IM_BYTE images.
 *
 * \verbatim im.ProcessLocalMaxThresEstimate(image: imImage) -> level: number [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessLocalMaxThresEstimate(const imImage* image, int *level);

/** Apply a manual threshold using an interval. \n
 * threshold = start_level <= a <= end_level ? 1: 0 \n
 * Normal value is 1 but another common value is 255. Can be done in place for IM_BYTE source. \n
 * Supports all integer IM_GRAY images as source, and IM_BINARY as destiny.
 *
 * \verbatim im.ProcessSliceThreshold(src_image: imImage, dst_image: imImage, start_level: number, end_level: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessSliceThresholdNew(src_image: imImage, start_level: number, end_level: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup threshold */
void imProcessSliceThreshold(const imImage* src_image, imImage* dst_image, int start_level, int end_level);


/** \defgroup effects Special Effects
 * \par
 * Operations to change image appearance.
 * \par
 * See \ref im_process_pon.h
 * \ingroup process */


/** Generates a zoom in effect averaging colors inside a square region. \n
 * Operates only on IM_BYTE images.
 *
 * \verbatim im.ProcessPixelate(src_image: imImage, dst_image: imImage, box_size: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessPixelateNew(src_image: imImage, box_size: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup effects */
void imProcessPixelate(const imImage* src_image, imImage* dst_image, int box_size);

/** A simple Posterize effect. It reduces the number of colors in the image eliminating 
 * less significant bit planes. Can have 1 to 7 levels. See \ref imProcessBitMask. \n
 * Image data type must be integer.
 *
 * \verbatim im.ProcessPosterize(src_image: imImage, dst_image: imImage, level: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessPosterizeNew(src_image: imImage, level: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup effects */
void imProcessPosterize(const imImage* src_image, imImage* dst_image, int level);



#if defined(__cplusplus)
}
#endif

#endif
