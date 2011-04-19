/** \file
 * \brief Private CD declarations
 *
 * See Copyright Notice in cd.h
 */

#ifndef __CD_PRIVATE_H
#define __CD_PRIVATE_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* All context canvas must have at least the base canvas pointer. */
typedef struct _cdCtxCanvasBase 
{
  cdCanvas* canvas;
} cdCtxCanvasBase;

typedef struct _cdCtxCanvas cdCtxCanvas;
typedef struct _cdCtxImage cdCtxImage;

typedef struct _cdVectorFont cdVectorFont;
typedef struct _cdSimulation cdSimulation;

typedef struct _cdPoint 
{
  int x, y; 
} cdPoint; 

typedef struct _cdfPoint 
{
  double x, y; 
} cdfPoint; 

typedef struct _cdRect 
{
  int xmin, xmax, ymin, ymax; 
} cdRect; 

typedef struct _cdfRect 
{
  double xmin, xmax, ymin, ymax; 
} cdfRect; 

typedef struct _cdAttribute
{
  const char *name;

  /* can be NULL one of them */
  void (*set)(cdCtxCanvas* ctxcanvas, char* data);
  char* (*get)(cdCtxCanvas* ctxcanvas);
} cdAttribute; 

struct _cdImage
{
  int w, h;
  cdCtxImage* ctximage;

  /* can NOT be NULL */
  void   (*cxGetImage)(cdCtxCanvas* ctxcanvas, cdCtxImage* ctximage, int x, int y);
  void   (*cxPutImageRect)(cdCtxCanvas* ctxcanvas, cdCtxImage* ctximage, int x, int y, int xmin, int xmax, int ymin, int ymax);
  void   (*cxKillImage)(cdCtxImage* ctximage);
};

struct _cdContext
{
  unsigned long caps;

  /* can NOT be NULL */
  void  (*cxCreateCanvas)(cdCanvas* canvas, void *data);
  void  (*cxInitTable)(cdCanvas* canvas);

  /* can be NULL */
  int   (*cxPlay)(cdCanvas* canvas, int xmin, int xmax, int ymin, int ymax, void *data); 
  int   (*cxRegisterCallback)(int cb, cdCallback func);
};

struct _cdCanvas
{
  char signature[2];  /* must be "CD" */

  /* can NOT be NULL */
  void   (*cxPixel)(cdCtxCanvas* ctxcanvas, int x, int y, long color);
  void   (*cxLine)(cdCtxCanvas* ctxcanvas, int x1, int y1, int x2, int y2);
  void   (*cxPoly)(cdCtxCanvas* ctxcanvas, int mode, cdPoint* points, int n);
  void   (*cxRect)(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax);
  void   (*cxBox)(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax);
  void   (*cxArc)(cdCtxCanvas* ctxcanvas, int xc, int yc, int w, int h, double angle1, double angle2);
  void   (*cxSector)(cdCtxCanvas* ctxcanvas, int xc, int yc, int w, int h, double angle1, double angle2);
  void   (*cxChord)(cdCtxCanvas* ctxcanvas, int xc, int yc, int w, int h, double angle1, double angle2);
  void   (*cxText)(cdCtxCanvas* ctxcanvas, int x, int y, const char *s);
  void   (*cxKillCanvas)(cdCtxCanvas* ctxcanvas);
  int    (*cxFont)(cdCtxCanvas* ctxcanvas, const char *type_face, int style, int size);
  void   (*cxPutImageRectMap)(cdCtxCanvas* ctxcanvas, int iw, int ih, const unsigned char *index, const long *colors, int x, int y, int w, int h, int xmin, int xmax, int ymin, int ymax);
  void   (*cxPutImageRectRGB)(cdCtxCanvas* ctxcanvas, int iw, int ih, const unsigned char *r, const unsigned char *g, const unsigned char *b, int x, int y, int w, int h, int xmin, int xmax, int ymin, int ymax);

  /* default implementation uses the simulation driver */
  void   (*cxGetFontDim)(cdCtxCanvas* ctxcanvas, int *max_width, int *height, int *ascent, int *descent);
  void   (*cxGetTextSize)(cdCtxCanvas* ctxcanvas, const char *s, int *width, int *height);

  /* all the following function pointers can be NULL */

  void   (*cxFlush)(cdCtxCanvas* ctxcanvas);
  void   (*cxClear)(cdCtxCanvas* ctxcanvas);

  void   (*cxFLine)(cdCtxCanvas* ctxcanvas, double x1, double y1, double x2, double y2);
  void   (*cxFPoly)(cdCtxCanvas* ctxcanvas, int mode, cdfPoint* points, int n);
  void   (*cxFRect)(cdCtxCanvas* ctxcanvas, double xmin, double xmax, double ymin, double ymax);
  void   (*cxFBox)(cdCtxCanvas* ctxcanvas, double xmin, double xmax, double ymin, double ymax);
  void   (*cxFArc)(cdCtxCanvas* ctxcanvas, double xc, double yc, double w, double h, double angle1, double angle2);
  void   (*cxFSector)(cdCtxCanvas* ctxcanvas, double xc, double yc, double w, double h, double angle1, double angle2);
  void   (*cxFChord)(cdCtxCanvas* ctxcanvas, double xc, double yc, double w, double h, double angle1, double angle2);
  void   (*cxFText)(cdCtxCanvas* ctxcanvas, double x, double y, const char *s);

  int    (*cxClip)(cdCtxCanvas* ctxcanvas, int mode);
  void   (*cxClipArea)(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax);
  void   (*cxFClipArea)(cdCtxCanvas* ctxcanvas, double xmin, double xmax, double ymin, double ymax);
  int    (*cxBackOpacity)(cdCtxCanvas* ctxcanvas, int opacity);
  int    (*cxWriteMode)(cdCtxCanvas* ctxcanvas, int mode);
  int    (*cxLineStyle)(cdCtxCanvas* ctxcanvas, int style);
  int    (*cxLineWidth)(cdCtxCanvas* ctxcanvas, int width);
  int    (*cxLineJoin)(cdCtxCanvas* ctxcanvas, int join);
  int    (*cxLineCap)(cdCtxCanvas* ctxcanvas, int cap);
  int    (*cxInteriorStyle)(cdCtxCanvas* ctxcanvas, int style);
  int    (*cxHatch)(cdCtxCanvas* ctxcanvas, int style);
  void   (*cxStipple)(cdCtxCanvas* ctxcanvas, int w, int h, const unsigned char *stipple);
  void   (*cxPattern)(cdCtxCanvas* ctxcanvas, int w, int h, const long *pattern);
  int    (*cxNativeFont)(cdCtxCanvas* ctxcanvas, const char* font);
  int    (*cxTextAlignment)(cdCtxCanvas* ctxcanvas, int alignment);
  double (*cxTextOrientation)(cdCtxCanvas* ctxcanvas, double angle);
  void   (*cxPalette)(cdCtxCanvas* ctxcanvas, int n, const long *palette, int mode);
  long   (*cxBackground)(cdCtxCanvas* ctxcanvas, long color);
  long   (*cxForeground)(cdCtxCanvas* ctxcanvas, long color);
  void   (*cxTransform)(cdCtxCanvas* ctxcanvas, const double* matrix);

  void   (*cxPutImageRectRGBA)(cdCtxCanvas* ctxcanvas, int iw, int ih, const unsigned char *r, const unsigned char *g, const unsigned char *b, const unsigned char *a, int x, int y, int w, int h, int xmin, int xmax, int ymin, int ymax);
  void   (*cxGetImageRGB)(cdCtxCanvas* ctxcanvas, unsigned char *r, unsigned char *g, unsigned char *b, int x, int y, int w, int h);
  void   (*cxScrollArea)(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax, int dx, int dy);

  cdCtxImage* (*cxCreateImage)(cdCtxCanvas* ctxcanvas, int w, int h);
  void   (*cxKillImage)(cdCtxImage* ctximage);
  void   (*cxGetImage)(cdCtxCanvas* ctxcanvas, cdCtxImage* ctximage, int x, int y);
  void   (*cxPutImageRect)(cdCtxCanvas* ctxcanvas, cdCtxImage* ctximage, int x, int y, int xmin, int xmax, int ymin, int ymax);

  void   (*cxNewRegion)(cdCtxCanvas* ctxcanvas);
  int    (*cxIsPointInRegion)(cdCtxCanvas* ctxcanvas, int x, int y);
  void   (*cxOffsetRegion)(cdCtxCanvas* ctxcanvas, int x, int y);
  void   (*cxGetRegionBox)(cdCtxCanvas* ctxcanvas, int *xmin, int *xmax, int *ymin, int *ymax);

  int    (*cxActivate)(cdCtxCanvas* ctxcanvas);
  void   (*cxDeactivate)(cdCtxCanvas* ctxcanvas);

  /* the driver must update these, when the canvas is created and
     whenever the canvas change its size or bpp. */
  int w,h;            /* size in pixels */              /****  pixel =   mm   * res  ****/
  double w_mm, h_mm;  /* size in mm */                  /****   mm   =  pixel / res  ****/
  double xres, yres;  /* resolution in pixels/mm */     /****   res  =  pixel / mm   ****/
  int bpp;            /* number of bits per pixel */
  int invert_yaxis;   /* the driver has the y axis from top to bottom */
  double matrix[6];
  int use_matrix;

  /* clipping attributes */
  int clip_mode;
  cdRect clip_rect;
  cdfRect clip_frect;
  int clip_poly_n;
  cdPoint* clip_poly;    /* only defined if integer poligon created, if exist clip_fpoly is NULL, and ->Poly exists */
  cdfPoint* clip_fpoly;  /* only defined if real poligon created, if exist clip_poly is NULL, and ->fPoly exists  */

  /* clipping region attributes */
  int new_region;
  int combine_mode;
  
  /* color attributes */
  long foreground, background;
  int back_opacity, write_mode;

  /* primitive attributes */
  int mark_type, mark_size;

  int line_style, line_width;
  int line_cap, line_join;
  int* line_dashes;
  int line_dashes_count;

  int interior_style, hatch_style;
  int fill_mode;

  char font_type_face[1024];
  int font_style, font_size;
  int text_alignment;
  double text_orientation;
  char native_font[1024];

  int pattern_w, pattern_h, pattern_size;
  long* pattern;
  int stipple_w, stipple_h, stipple_size;
  unsigned char* stipple;

  /* origin */
  int use_origin;
  cdPoint origin;            /* both points contains the same coordinate always */
  cdfPoint forigin;

  /* last polygon */
  int poly_mode, 
      poly_n,                /* current number of vertices */
      poly_size, fpoly_size; /* allocated number of vertices, only increases */
  cdPoint* poly;             /* used during an integer poligon creation, only if ->Poly exists */
  cdfPoint* fpoly;           /* used during an real poligon creation, only if ->fPoly exists */
  int use_fpoly;

  /* simulation flags */
  int sim_mode;
  int sim_poly;

  /* WC */
  double s, sx, tx, sy, ty;   /* Transformacao Window -> Viewport (scale+translation)*/
  cdfRect window;             /* Window in WC */
  cdRect viewport;            /* Viewport in pixels */

  cdAttribute* attrib_list[50];
  int attrib_n;

  cdVectorFont* vector_font;
  cdSimulation* simulation;
  cdCtxCanvas* ctxcanvas;
  cdContext* context;
};


/***************/
/* attributes  */
/***************/
void cdRegisterAttribute(cdCanvas* canvas, cdAttribute* attrib);

/***************/
/* vector font */
/***************/
cdVectorFont* cdCreateVectorFont(cdCanvas* canvas);
void cdKillVectorFont(cdVectorFont* vector_font_data);

/**********/
/*   WC   */
/**********/
void wdSetDefaults(cdCanvas* canvas);

/*************/
/* utilities */
/*************/
int* cdGetZoomTable(int w, int rw, int xmin);
int cdCalcZoom(int canvas_size, int cnv_rect_pos, int cnv_rect_size, 
               int *new_cnv_rect_pos, int *new_cnv_rect_size, 
               int img_rect_pos, int img_rect_size, 
               int *new_img_rect_pos, int *new_img_rect_size, int is_horizontal);
int cdRound(double x);

void cdCanvasGetEllipseBox(int xc, int yc, int w, int h, double a1, double a2, int *xmin, int *xmax, int *ymin, int *ymax);

int cdGetFileName(const char* strdata, char* filename);

void cdMatrixTransformPoint(double* matrix, int x, int y, int *rx, int *ry);
void cdfMatrixTransformPoint(double* matrix, double x, double y, double *rx, double *ry);
void cdMatrixMultiply(const double* matrix, double* mul_matrix);
void cdMatrixInverse(const double* matrix, double* inv_matrix);
void cdRotatePoint(cdCanvas* canvas, int x, int y, int cx, int cy, int *rx, int *ry, double sin_teta, double cos_teta);
void cdRotatePointY(cdCanvas* canvas, int x, int y, int cx, int cy, int *ry, double sin_theta, double cos_theta);
void cdTextTranslatePoint(cdCanvas* canvas, int x, int y, int w, int h, int baseline, int *rx, int *ry);

int cdCheckBoxSize(int *xmin, int *xmax, int *ymin, int *ymax);
int cdfCheckBoxSize(double *xmin, double *xmax, double *ymin, double *ymax);
void cdNormalizeLimits(int w, int h, int *xmin, int *xmax, int *ymin, int *ymax);

int cdParsePangoFont(const char *nativefont, char *type_face, int *style, int *size);
int cdParseIupWinFont(const char *nativefont, char *type_face, int *style, int *size);
int cdParseXWinFont(const char *nativefont, char *type_face, int *style, int *size);
int cdGetFontSizePixels(cdCanvas* canvas, int size);
int cdGetFontSizePoints(cdCanvas* canvas, int size);

unsigned char cdZeroOrderInterpolation(int width, int height, const unsigned char *map, float xl, float yl);
unsigned char cdBilinearInterpolation(int width, int height, const unsigned char *map, float xl, float yl);
void cdImageRGBInitInverseTransform(int w, int h, int xmin, int xmax, int ymin, int ymax, float *xfactor, float *yfactor, const double* matrix, double* inv_matrix);
void cdImageRGBInverseTransform(int t_x, int t_y, float *i_x, float *i_y, float xfactor, float yfactor, int xmin, int ymin, int x, int y, double *inv_matrix);
void cdImageRGBCalcDstLimits(cdCanvas* canvas, int x, int y, int w, int h, int *xmin, int *xmax, int *ymin, int *ymax, int* rect);


#define CD_ALPHA_BLEND(_src,_dst,_alpha) (unsigned char)(((_src) * (_alpha) + (_dst) * (255 - (_alpha))) / 255)

#define _cdCheckCanvas(_canvas) (_canvas!=NULL && ((unsigned char*)_canvas)[0] == 'C' && ((unsigned char*)_canvas)[1] == 'D')
#define _cdInvertYAxis(_canvas, _y) (_canvas->h - (_y) - 1)
#define _cdSwapInt(_a,_b) {int _c=_a;_a=_b;_b=_c;}
#define _cdSwapDouble(_a,_b) {double _c=_a;_a=_b;_b=_c;}
#define _cdRound(_x) ((int)(_x < 0? (_x-0.5): (_x+0.5)))
#define _cdRotateHatch(_x)  ((_x) = ((_x)<< 1) | ((_x)>>7))

/**************/
/* simulation */
/**************/
cdSimulation* cdCreateSimulation(cdCanvas* canvas);
void cdKillSimulation(cdSimulation* simulation);
void cdSimInitText(cdSimulation* simulation);

/* Replacements for cdCanvas function pointers */
void cdrectSIM(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax);
void cdboxSIM(cdCtxCanvas* ctxcanvas, int xmin, int xmax, int ymin, int ymax);
void cdlineSIM(cdCtxCanvas* ctxcanvas, int x1, int y1, int x2, int y2);
void cdarcSIM(cdCtxCanvas* ctxcanvas, int xc, int yc, int width, int height, double angle1, double angle2);
void cdsectorSIM(cdCtxCanvas* ctxcanvas, int xc, int yc, int width, int height, double angle1, double angle2);
void cdchordSIM(cdCtxCanvas* ctxcanvas, int xc, int yc, int width, int height, double angle1, double angle2);
void cdpolySIM(cdCtxCanvas* ctxcanvas, int mode, cdPoint* points, int n);

/* Replacements for Text and Font using FreeType library */
void cdtextSIM(cdCtxCanvas* ctxcanvas, int x, int y, const char *s);
int cdfontSIM(cdCtxCanvas* ctxcanvas, const char *type_face, int style, int size);
void cdgetfontdimSIM(cdCtxCanvas* ctxcanvas, int *max_width, int *height, int *ascent, int *descent);
void cdgettextsizeSIM(cdCtxCanvas* ctxcanvas, const char *s, int *width, int *height);

/* Simulation functions that are independent of the simulation base driver */
void cdSimMark(cdCanvas* canvas, int x, int y);
void cdSimPolyBezier(cdCanvas* canvas, const cdPoint* points, int n);
void cdSimPutImageRectRGBA(cdCanvas* canvas, int iw, int ih, const unsigned char *r, const unsigned char *g, const unsigned char *b, const unsigned char *a, int x, int y, int w, int h, int xmin, int xmax, int ymin, int ymax);

/* Replacements for Font using estimation */
void cdgetfontdimEX(cdCtxCanvas* ctxcanvas, int *max_width, int *height, int *ascent, int *descent);
void cdgettextsizeEX(cdCtxCanvas* ctxcanvas, const char *s, int *width, int *height);


#ifdef __cplusplus
}
#endif

#endif
