#ifndef _Paint_h
#define _Paint_h

/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

/****************************************************************
 *
 * Paint widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNpaintResource "paintResource"
#define XtNpattern	 "pattern"
#define XtNgrid	 	 "grid"
#define XtNgridOn 	 "gridOn"
#define XtNlineWidth	 "lineWidth"
#define XtNfatBack	 "fatBack"
#define XtNundoSize	 "undoSize"
#define XtNzoom	 	 "zoom"
#define XtNpaint	 "paint"
#define XtNzoomX	 "zoomX"
#define XtNzoomY	 "zoomY"
#define XtNdrawWidth	 "drawWidth"
#define XtNdrawHeight	 "drawHeight"
#define XtNcompress	 "compress"
#define XtNdirty	 "dirty"
#define XtNfillRule	 "fillRule"

#define XtCPaintResource "PaintResource"
#define XtCPattern	 "Pattern"
#define XtCGrid		 "Grid"
#define XtCGridOn	 "GridOn"
#define XtCLineWidth	 "LineWidth"
#define XtCFatBack	 "FatBack"
#define XtCUndoSize	 "UndoSize"
#define XtCZoom	  	 "Zoom"
#define XtCPaint	 "Paint"
#define XtCZoomX	 "ZoomX"
#define XtCZoomY	 "ZoomY"
#define XtCDrawWidth	 "DrawWidth"
#define XtCDrawHeight	 "DrawHeight"
#define XtCCompress	 "Compress"
#define XtCDirty	 "Dirty"
#define XtCFillRule	 "FillRule"

/* declare specific PaintWidget class and instance datatypes */

typedef struct _PaintClassRec*		PaintClass;
typedef struct _PaintRec*		PaintWidget;

/* declare the class constant */

extern WidgetClass paintWidgetClass;

/*
**  Operation callback information
*/
typedef enum	{ opPixmap = 0x01, opWindow = 0x02 } OpSurface;

typedef struct {
	int		refCount;
	OpSurface	surface;
	Drawable	drawable;
	GC		filled_gc, base_gc, solid_gc;
	void		*data;
	int		isFat;
	int		x, y;
	int		realX, realY;
	int		zoom;
} OpInfo;

typedef void (*OpEventProc)(Widget, void *, XEvent *, OpInfo *);
void OpRemoveEventHandler(Widget, int, int, Boolean, OpEventProc, void *);
void OpAddEventHandler(Widget, int, int, Boolean, OpEventProc, void *);
void UndoStart(Widget, OpInfo *);
void UndoStartRectangle(Widget, OpInfo *, XRectangle *);
void UndoStartPoint(Widget, OpInfo *, int, int);
void UndoSetRectangle(Widget, XRectangle *);
void UndoGrow(Widget, int, int);

Pixmap PwUndoStart(Widget);
void UndoSwap(Widget);

/*
**  Public functions
*/

void PwUpdateFromLast(Widget, XRectangle *);
void PwUpdate(Widget, XRectangle *, Boolean);
void PwUpdateDrawable(Widget, Drawable, XRectangle *);
void PwSetDrawn(Widget, Boolean);
void PwGetPixmap(Widget, Pixmap *, int *, int *);
void PwMoveRegion(Widget, int, int);
void PwWriteRegion(Widget);
Boolean PwClearRegion(Widget, Boolean);
void PwPutRegion(Widget, Pixmap, Pixmap);
void PwLoadRegion(Widget, XRectangle *, Pixmap);
Boolean PwGetRegion(Widget, Pixmap *, Pixmap *);

#endif /* _Paint_h */
