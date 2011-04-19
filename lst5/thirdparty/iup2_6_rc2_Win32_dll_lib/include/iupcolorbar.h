/** \file
 * \brief iupcolorbar control.
 *
 * See Copyright Notice in iup.h
 */
 
#ifndef __IUPCOLORBAR_H 
#define __IUPCOLORBAR_H

#ifdef __cplusplus
extern "C" {
#endif

/** Attribute name for the number of sections */
#ifndef IUP_NUM_PARTS 
#define IUP_NUM_PARTS  "NUM_PARTS"
#endif

/** Attribute name for the number of cells */
#ifndef IUP_NUM_CELLS
#define IUP_NUM_CELLS "NUM_CELLS"
#endif

/** Attribute name for a color cell */
#ifndef IUP_CELL
#define IUP_CELL "CELL"
#endif

/** Attribute name for the preview area size */
#ifndef IUP_PREVIEW_SIZE
#define IUP_PREVIEW_SIZE "PREVIEW_SIZE"
#endif

/** Attribute name for showing the preview area */
#ifndef IUP_SHOW_PREVIEW
#define IUP_SHOW_PREVIEW "SHOW_PREVIEW"
#endif

/** Attribute name for showing the secondary color option */
#ifndef IUP_SHOW_SECONDARY
#define IUP_SHOW_SECONDARY "SHOW_SECONDARY"
#endif

/** Attribute name for setting the primary color */
#ifndef IUP_PRIMARY_CELL
#define IUP_PRIMARY_CELL "PRIMARY_CELL"
#endif

/** Attribute name for setting the primary color */
#ifndef IUP_SECONDARY_CELL
#define IUP_SECONDARY_CELL "SECONDARY_CELL"
#endif

/** Attribute name for setting the widget orientation */
#ifndef IUP_ORIENTATION 
#define IUP_ORIENTATION  "ORIENTATION"
#endif

/** Attribute name for setting the widget squared cell mode */
#ifndef IUP_SQUARED 
#define IUP_SQUARED  "SQUARED"
#endif

/** Attribute name for setting the widget 3D shadowed cell mode */
#ifndef IUP_SHADOWED 
#define IUP_SHADOWED  "SHADOWED"
#endif

/** Attribute name for setting the bufferization flag */
#ifndef IUP_BUFFERIZE 
#define IUP_BUFFERIZE  "BUFFERIZE"
#endif

/** Attribute name for setting the transparency color */
#ifndef IUP_TRANSPARENCY 
#define IUP_TRANSPARENCY  "TRANSPARENCY"
#endif

/** Attribute name for setting the color browser callback */
#ifndef IUP_CELL_CB
#define IUP_CELL_CB "CELL_CB"
#endif

/** Attribute name for setting the extended callback */
#ifndef IUP_EXTENDED_CB
#define IUP_EXTENDED_CB "EXTENDED_CB"
#endif

/** Attribute name for setting the color selection callback */
#ifndef IUP_SELECT_CB
#define IUP_SELECT_CB "SELECT_CB"
#endif

/** Attribute name for setting the switch primary-secondary callback */
#ifndef IUP_SWITCH_CB
#define IUP_SWITCH_CB "SWITCH_CB"
#endif


/** Value for the color selection type (primary color).  @see IUP_SELECT_CB */
#ifndef IUP_PRIMARY   
#define IUP_PRIMARY -1
#endif

/** Value for the color selection type (secondary color). @see IUP_SELECT_CB */
#ifndef IUP_SECONDARY   
#define IUP_SECONDARY -2
#endif

/** Value for the colorbar orientation (vertical). */
#ifndef IUP_VERTICAL   
#define IUP_VERTICAL    "VERTICAL"
#endif

#ifndef IUP_HORIZONTAL 
/** Value for the colorbar orientation (horizontal) */
#define IUP_HORIZONTAL  "HORIZONTAL"
#endif

Ihandle* IupColorbar(void);

#ifdef __cplusplus
}
#endif

#endif
