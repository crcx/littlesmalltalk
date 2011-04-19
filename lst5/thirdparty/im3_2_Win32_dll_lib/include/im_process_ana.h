/** \file
 * \brief Image Statistics and Analysis
 *
 * See Copyright Notice in im_lib.h
 * $Id: im_process_ana.h,v 1.7 2006/11/20 13:18:51 scuri Exp $
 */

#ifndef __IM_PROC_ANA_H
#define __IM_PROC_ANA_H

#include "im_image.h"

#if	defined(__cplusplus)
extern "C" {
#endif



/** \defgroup stats Image Statistics Calculations
 * \par
 * Operations to calculate some statistics over images.
 * \par
 * See \ref im_process_ana.h
 * \ingroup process */

/** Calculates the RMS error between two images (Root Mean Square Error).
 *
 * \verbatim im.CalcRMSError(image1: imImage, image2: imImage) -> rms: number [in Lua 5] \endverbatim
 * \ingroup stats */
float imCalcRMSError(const imImage* image1, const imImage* image2);

/** Calculates the SNR of an image and its noise (Signal Noise Ratio).
 *
 * \verbatim im.CalcSNR(src_image: imImage, noise_image: imImage) -> snr: number [in Lua 5] \endverbatim
 * \ingroup stats */
float imCalcSNR(const imImage* src_image, const imImage* noise_image);

/** Count the number of different colors in an image. \n
 * Image must be IM_BYTE, but all color spaces except IM_CMYK.
 *
 * \verbatim im.CalcCountColors(image: imImage) -> count: number [in Lua 5] \endverbatim
 * \ingroup stats */
unsigned long imCalcCountColors(const imImage* image);

/** Calculates the histogram of a IM_BYTE data. \n
 * Histogram is always 256 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram.
 *
 * \verbatim im.CalcHistogram(image: imImage, plane: number, cumulative: number) -> histo: table of numbers [in Lua 5] \endverbatim
 * Where plane is the depth plane to calculate the histogram. \n
 * The returned table is zero indexed. image can be IM_USHORT or IM_BYTE.
 * \ingroup stats */
void imCalcHistogram(const unsigned char* data, int count, unsigned long* histo, int cumulative);

/** Calculates the histogram of a IM_USHORT data. \n
 * Histogram is always 65535 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram. \n
 * Use \ref imCalcHistogram in Lua.
 * \ingroup stats */
void imCalcUShortHistogram(const unsigned short* data, int count, unsigned long* histo, int cumulative);

/** Calculates the gray histogram of an image. \n
 * Image must be IM_BYTE/(IM_RGB, IM_GRAY, IM_BINARY or IM_MAP). \n
 * If the image is IM_RGB then the histogram of the luma component is calculated. \n
 * Histogram is always 256 positions long. \n
 * When cumulative is different from zero it calculates the cumulative histogram.
 *
 * \verbatim im.CalcGrayHistogram(image: imImage, cumulative: number) -> histo: table of numbers [in Lua 5] \endverbatim
 * \ingroup stats */
void imCalcGrayHistogram(const imImage* image, unsigned long* histo, int cumulative);

/** Numerical Statistics Structure
 * \ingroup stats */
typedef struct _imStats
{
  float max;                /**< Maximum value              */
  float min;                /**< Minimum value              */
  unsigned long positive;   /**< Number of Positive Values  */
  unsigned long negative;   /**< Number of Negative Values  */
  unsigned long zeros;      /**< Number of Zeros            */
  float mean;               /**< Mean                       */
  float stddev;             /**< Standard Deviation         */
} imStats;

/** Calculates the statistics about the image data. \n
 * There is one stats for each depth plane. For ex: stats[0]=red stats, stats[0]=green stats, ... \n
 * Supports all data types except IM_CFLOAT. \n
 *
 * \verbatim im.CalcImageStatistics(image: imImage) -> stats: table [in Lua 5] \endverbatim
 * Table contains the following fields: max, min, positive, negative, zeros, mean, stddev. 
 * The same as the \ref imStats structure.
 * \ingroup stats */
void imCalcImageStatistics(const imImage* image, imStats* stats);

/** Calculates the statistics about the image histogram data.\n
 * There is one stats for each depth plane. For ex: stats[0]=red stats, stats[0]=green stats, ... \n
 * Only IM_BYTE images are supported.
 *
 * \verbatim im.CalcHistogramStatistics(image: imImage) -> stats: table [in Lua 5] \endverbatim
 * \ingroup stats */
void imCalcHistogramStatistics(const imImage* image, imStats* stats);

/** Calculates some extra statistics about the image histogram data.\n
 * There is one stats for each depth plane. \n
 * Only IM_BYTE images are supported. \n
 * mode will be -1 if more than one max is found.
 *
 * \verbatim im.CalcHistoImageStatistics(image: imImage) -> median: number, mode: number [in Lua 5] \endverbatim
 * \ingroup stats */
void imCalcHistoImageStatistics(const imImage* image, int* median, int* mode);



/** \defgroup analyze Image Analysis
 * \par
 * See \ref im_process_ana.h
 * \ingroup process */

/** Find white regions in binary image. \n
 * Result is IM_GRAY/IM_USHORT type. Regions can be 4 connected or 8 connected. \n
 * Returns the number of regions found. Background is marked as 0. \n
 * Regions touching the border are considered only if touch_border=1.
 *
 * \verbatim im.AnalyzeFindRegions(src_image: imImage, dst_image: imImage, connect: number, touch_border: number) -> count: number [in Lua 5] \endverbatim
 * \verbatim im.AnalyzeFindRegionsNew(image: imImage, connect: number, touch_border: number) -> count: number, new_image: imImage [in Lua 5] \endverbatim
 * \ingroup analyze */
int imAnalyzeFindRegions(const imImage* src_image, imImage* dst_image, int connect, int touch_border);

/** Measure the actual area of all regions. Holes are not included. \n
 * This is the number of pixels of each region. \n
 * Source image is IM_GRAY/IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * area has size the number of regions.
 *
 * \verbatim im.AnalyzeMeasureArea(image: imImage, [region_count: number]) -> area: table of numbers [in Lua 5] \endverbatim
 * The returned table is zero indexed. 
 * \ingroup analyze */
void imAnalyzeMeasureArea(const imImage* image, int* area, int region_count);

/** Measure the polygonal area limited by the perimeter line of all regions. Holes are not included. \n
 * Notice that some regions may have polygonal area zero. \n
 * Source image is IM_GRAY/IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * perimarea has size the number of regions.
 *
 * \verbatim im.AnalyzeMeasurePerimArea(image: imImage, [region_count: number]) -> perimarea: table of numbers [in Lua 5] \endverbatim
 * The returned table is zero indexed. 
 * \ingroup analyze */
void imAnalyzeMeasurePerimArea(const imImage* image, float* perimarea);

/** Calculate the centroid position of all regions. Holes are not included. \n
 * Source image is IM_GRAY/IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * area, cx and cy have size the number of regions. If area is NULL will be internally calculated.
 *
 * \verbatim im.AnalyzeMeasureCentroid(image: imImage, [area: table of numbers], [region_count: number]) -> cx: table of numbers, cy: table of numbers [in Lua 5] \endverbatim
 * The returned tables are zero indexed. 
 * \ingroup analyze */
void imAnalyzeMeasureCentroid(const imImage* image, const int* area, int region_count, float* cx, float* cy);

/** Calculate the principal major axis slope of all regions. \n
 * Source image is IM_GRAY/IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * data has size the number of regions. If area or centroid are NULL will be internally calculated. \n
 * Principal (major and minor) axes are defined to be those axes that pass through the
 * centroid, about which the moment of inertia of the region is, respectively maximal or minimal.
 *
 * \verbatim im.AnalyzeMeasurePrincipalAxis(image: imImage, [area: table of numbers], [cx: table of numbers], [cy: table of numbers], [region_count: number]) 
                              -> major_slope: table of numbers, major_length: table of numbers, minor_slope: table of numbers, minor_length: table of numbers [in Lua 5] \endverbatim
 * The returned tables are zero indexed. 
 * \ingroup analyze */
void imAnalyzeMeasurePrincipalAxis(const imImage* image, const int* area, const float* cx, const float* cy, 
                                   const int region_count, float* major_slope, float* major_length, 
                                                           float* minor_slope, float* minor_length);

/** Measure the number and area of holes of all regions. \n
 * Source image is IM_USHORT type (the result of \ref imAnalyzeFindRegions). \n
 * area and perim has size the number of regions, if some is NULL it will be not calculated.
 *
 * \verbatim im.AnalyzeMeasureHoles(image: imImage, connect: number, [region_count: number]) -> holes_count: number, area: table of numbers, perim: table of numbers [in Lua 5] \endverbatim
 * The returned tables are zero indexed. 
 * \ingroup analyze */
void imAnalyzeMeasureHoles(const imImage* image, int connect, int *holes_count, int* area, float* perim);

/** Measure the total perimeter of all regions (external and internal). \n
 * Source image is IM_GRAY/IM_USHORT type (the result of imAnalyzeFindRegions). \n
 * It uses a half-pixel inter distance for 8 neighboors in a perimeter of a 4 connected region. \n
 * This function can also be used to measure line lenght. \n
 * perim has size the number of regions.
 *
 * \verbatim im.AnalyzeMeasurePerimeter(image: imImage) -> perim: table of numbers [in Lua 5] \endverbatim
 * \ingroup analyze */
void imAnalyzeMeasurePerimeter(const imImage* image, float* perim, int region_count);

/** Isolates the perimeter line of gray integer images. Background is defined as being black (0). \n
 * It just checks if at least one of the 4 connected neighboors is non zero. Image borders are extended with zeros.
 *
 * \verbatim im.ProcessPerimeterLine(src_image: imImage, dst_image: imImage) [in Lua 5] \endverbatim
 * \verbatim im.ProcessPerimeterLineNew(image: imImage) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup analyze */
void imProcessPerimeterLine(const imImage* src_image, imImage* dst_image);

/** Eliminates regions that have size outside the given interval. \n
 * Source and destiny are a binary images. Regions can be 4 connected or 8 connected. \n
 * Can be done in-place. end_size can be zero to ignore big objects.
 *
 * \verbatim im.ProcessPrune(src_image: imImage, dst_image: imImage, connect: number, start_size: number, end_size: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessPruneNew(image: imImage, connect: number, start_size: number, end_size: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup analyze */
void imProcessPrune(const imImage* src_image, imImage* dst_image, int connect, int start_size, int end_size);

/** Fill holes inside white regions. \n
 * Source and destiny are a binary images. Regions can be 4 connected or 8 connected. \n
 * Can be done in-place. 
 *
 * \verbatim im.ProcessFillHoles(src_image: imImage, dst_image: imImage, connect: number) [in Lua 5] \endverbatim
 * \verbatim im.ProcessFillHolesNew(image: imImage, connect: number) -> new_image: imImage [in Lua 5] \endverbatim
 * \ingroup analyze */
void imProcessFillHoles(const imImage* src_image, imImage* dst_image, int connect);


#if defined(__cplusplus)
}
#endif

#endif
