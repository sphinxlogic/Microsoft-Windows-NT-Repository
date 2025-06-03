#ifndef _PaintP_h
#define _PaintP_h

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

#include "Paint.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRPaintResource "PaintResource"

typedef struct {
    int		empty;
} PaintClassPart;

typedef struct _PaintClassRec {
    CoreClassPart	core_class;
    PaintClassPart	paint_class;
} PaintClassRec;

extern PaintClassRec paintClassRec;

typedef struct s_undoStack {
	XRectangle		box;
	Pixmap			pixmap;
	struct s_undoStack	*next, *prev;
} UndoStack;

typedef struct {
	int		drawWidth, drawHeight;
	int		fillRule;
	GC		gc, igc, fgc;
	Pixmap		sourcePixmap;
	Pixmap		pattern;
	int		lineWidth;
	XFontStruct	*fontInfo;
	Pixel		foreground, background;
	int		undoSize;

	/*
	**  The undo system
	*/
	Pixmap		base;
	UndoStack	*undo, *top;
	Boolean		dirty;

	Boolean		gridOn;
	int		grid;
	void		*eventList;
	XtCallbackList	fatcalls;
	int		zoomX, zoomY;
	int		zoom;
	GC		tgc;	/* scratch GC, for stuffing clip masks into */
	GC		mgc;	/* region mask GC, 1 bit deep */
	Widget		paint;
	Pixel		linePixel;
	Boolean		compress;

	/*
	**	Selection regions
	*/
	Boolean		regionDrawn;
	Pixmap		regionPix, regionMask;
	XRectangle	regionRect;
} PaintPart;

#define PwPixmap(w)	((w)->paint.active->pixmap)

typedef struct _PaintRec {
    CorePart		core;
    PaintPart		paint;
} PaintRec;

#endif /* _PaintP_h */
