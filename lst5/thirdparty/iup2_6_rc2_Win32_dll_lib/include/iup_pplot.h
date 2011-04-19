/** \file
 * \brief PPlot component for Iup.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUP_PPLOT_H 
#define __IUP_PPLOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize PPlot widget class */
void IupPPlotOpen(void);

/* Create an PPlot widget instance */
Ihandle* IupPPlot(void);

/* Add dataset to plot */
void IupPPlotBegin(Ihandle *self, int strXdata);
void IupPPlotAdd(Ihandle *self, float x, float y);
void IupPPlotAddStr(Ihandle *self, const char* x, float y);
int IupPPlotEnd(Ihandle *self);

void IupPPlotInsertStr(Ihandle *self, int index, int sample_index, const char* x, float y);
void IupPPlotInsert(Ihandle *self, int index, int sample_index, float x, float y);

/* convert from plot coordinates to pixels */
void IupPPlotTransform(Ihandle* self, float x, float y, int *ix, int *iy);

/* Plot on the given device. Uses a "cdCanvas*". */
void IupPPlotPaintTo(Ihandle *self, void *cnv);


#ifdef __cplusplus
}
#endif

#endif
