/* vlistp.h - Private VList widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains the private C-language declarations needed by VList widget
modules.  It includes the public header VList.h as well.

MODIFICATION HISTORY

29-Oct-1993 (sjk) Add VListNselectionIsLocked resource.

22-Oct-1993 (sjk) Add VListNgridStyle and VListNgridPixmap resources.

30-Aug-1993 (sjc/nfc) Initial entry - extracted from vlist.c.
*/

#ifndef _VListP_h
#define _VListP_h

#include "vlist.h"

typedef struct {					/* widget class contribution */
    int			notused;
} VListClassPart;

typedef struct {					/* widget class record */
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    VListClassPart	vlist_class;
} VListClassRec, *VListClass;

typedef struct {
    int			x;				/* position within virtual window */
    int			width;				/* actual width */
    int			widthCnt;			/* how many have that width? */
} ColumnInfo;

typedef struct {
    int			textWidth;			/* width of label in pixels */
    int			widthPerColumn;			/* width contribution */
} LabelInfo;

typedef struct {
    VListSegment	vSegment;			/* copy of user-supplied entry */
    int			bodyHeight;			/* segment body height in pixels */
    int			bodyWidth;			/* segment body width in pixels */
    int			maxAscent;			/* overall segment ascent in pixels */
} SegmentInfo;

typedef struct {
    VListLine		vLine;				/* copy of user-supplied entry */
    int			bodyWidth;			/* significant width in pixels */
    int			bodyHeight;			/* significant height */
    char		expose;				/* line marked for expose (1/0) */
    char		undoSelect;			/* selected at ButtonPress (1/0) */
    char		undragSelect;			/* selected at start of drag (1/0) */
    char		notUsed;			/* filer for alignment */
    int			textStrlen;			/* length in bytes of text */
    SegmentInfo		*segmentInfosP;			/* list of segments */
    int			maxAscent;			/* ascent in pixels for text */
} LineInfo;

typedef struct {					/* Widget record */
    CorePart		core;				/* Core widget fields */
    CompositePart	composite;			/* composite widget fields */

    /* Resource fields */

    int			alignment;
    int			allowDeadspace;
    int			autoScrollRate;
    Pixmap		colorList;
    int			columnStyle;
    int			doubleClickDelay;
    XtCallbackList	dragArmCallback;
    int			dragButton;
    XtCallbackList	dragDropCallback;
    Widget		dragW;
    int			fieldCnt;
    int			fieldResizePolicy;
    int			fieldSpacingWidth;
    VListField		*fieldsP;
    struct _XmFontListRec *fontTableP;
    Pixel		foreground;
    int			gridDashOff;
    int			gridDashOn;
    Pixmap		gridPixmap;
    int			gridStyle;
    int			histHeight;
    int			histMarginHeight;
    int			histMarginWidth;
    int			histWidth;
    int			hScrollPolicy;
    Widget		hScrollW;
    int			iconMarginHeight;
    int			iconMarginWidth;
    int			labelCnt;
    int			labelPadOption;
    VListLabel		*labelsP;
    int			marginHeight;
    int			marginWidth;
    int			rowHeight;
    int			rowSpacingHeight;
    int			selectButton;
    XtCallbackList	selectCallback;
    XtCallbackList	selectConfirmCallback;
    int			selectStyle;
    int			selectUnit;
    int			selectionIsLocked;
    int			spaceGroupsEqually;
    int			spaceTiersEqually;
    int			textMarginHeight;
    int			textMarginWidth;
    Opaque		userData;
    Widget		vHeaderW;
    int			vScrollPolicy;
    int			vScrollPosition;
    Widget		vScrollW;

    /* Private fields */

    Opaque		beginZeroInit;
    int			activeGropCnt;				/* number of active graphics operations */
    int			anyAreExposed;				/* some exposures may be pending */
    int			batchUpdate;				/* batch update depth */
    int			cellHeight;				/* cell height in pixels */
    int			cellHeightCnt; 				/* number of cells with that height */
    int			charWidth;				/* char '0' width in pixels */
    int			colorCnt;				/* number of colors in colorTable */
    Pixel		*colorTableP;				/* array of color values */
    int			columnCnt;				/* number of physical columns */
    ColumnInfo		*columnInfosP;				/* address of ColumnInfo array */
    Pixel		currentBackground; 			/* background currently set */
    Pixel		currentForeground;			/* foreground currently set */
    int			currentDashOff; 			/* dash pattern currently set */
    int			currentDashOn;				/* dash pattern currently set */
    int			currentDashOffset; 			/* dash offset currently set */
    int			currentFontidx;				/* font currently set (index) */
    XFontStruct 	*currentFontP;				/* font currently set (font struct) */
    int			currentStippled; 			/* stipple mask is enabled (1/0) */
    int			currentStippleX; 			/* stipple origin */
    int			currentStippleY; 			/* stipple origin */
    int			deferMakeLineVisible;			/* defer VListMakeLineVisible calls while scrollbar is grabbed */
    int			doubleClickIsPending; 			/* doubleClick in progress */
    int			downColumn;				/* downclick column */
    int			downTier;				/* downclick tier */
    VListDragDropCallbackStruct dragDropCallbackStruct;		/* callback structure to pass to DragDrop callback */
    int			dragAdjustX, dragAdjustY;		/* drag widget positioning adjustments */
    int			dragIsActive;				/* drag is in progress */
    int			dragX, dragY;				/* current drag widget position */
    int			externalXAdjust;			/* acknowleged scrolling position */
    int			externalYAdjust; 			/* acknowleged scrolling position */
    GC			flipgc;					/* reverse video graphics context */
    XFontStruct		**fontPP;				/* address of font pointer array */
    int			fontCnt;				/* number of fonts in fontTable */
    int			groupCnt;				/* number of field groups */
    int			headerHeight;				/* height desired by vHeader widget */
    int			ignoreScrollValueChanged; 		/* ignore callback */
    int			internalXAdjust; 			/* desired scrolling position */
    int			internalYAdjust; 			/* desired scrolling position */
    GC			invgc;					/* highlight graphics context */
    LabelInfo		*labelInfosP;				/* address of LabelInfo array */
    int			lastUsedClosure; 			/* widget-supplied closure counter */
    Pixmap		latestPixmapId;				/* most recent pixmap whose size was computed */
    int			latestPixmapWidth; 			/* ... and its width ... */
    int			latestPixmapHeight; 			/* ... and height */
    int			layoutIsValid;				/* layout is ok */
    int			lineCnt;				/* number of logical line(item)s */
    LineInfo		*lineInfosP;				/* address of LineInfo array */
    int			logicalHeight;				/* virtual window's height */
    int			logicalWidth;				/* virtual window's width */
    int			maxLineCnt;				/* lines allocated in LineInfo array */
    int			mouseColumn;				/* column where mouse was last seen */
    int			mouseTier;				/* tier where mouse was last seen */
    GC			normgc;   				/* normal graphics context */
    unsigned long	redrawSerial; 				/* pending redraw expose event */
    int			requestedXAdjust; 			/* requested scrolling position */
    int			requestedYAdjust; 			/* requested scrolling position */
    int			rowCnt;					/* number of logical rows */
    int			scrollIsActive;				/* XCopyArea in progress */
    unsigned long	scrollSerial; 				/* pending GraphicsExpose/NoExpose event */
    VListSelectChange	*selectChangesP; 			/* array to be passed to callbacks */
    int			selectIsActive;				/* downclick in list area */
    int			selectParity;				/* flavor of drag select (1/0) */
    int			selectionCnt;				/* number of selected lines */
    int			shownHeaderXAdjust; 			/* current header setting */
    int			shownScrollXAdjust; 			/* current scroll bar setting */
    int			shownScrollYAdjust; 			/* current scroll bar setting */
    Pixmap		stippleBitmap;				/* stipple bitmap */
    int			tierCnt;				/* number of physical tiers */
    int			tierHeight;				/* height of one physical tier */
    int			topMarginHeight; 			/* (pretty) top margin */
    int			undoXAdjust;				/* internalXAdjust at downclick */
    int			undoYAdjust;				/* internalYAdjust at downclick */
    int			upColumn;				/* upclick column */
    int			upTier;					/* upclick tier */
    int			upTime;					/* start time for doubleClick */
    int			visible;				/* at least one ExposeEvent received */
    Opaque		visibleClosure;				/* line to be made visible */
    int			visiblePosition; 			/* and where */
    int			xScrollStrength; 			/* autoscroll speed factor */
    int			yScrollStrength; 			/* autoscroll speed factor */
    Opaque		endZeroInit;
} VListWidgetRec, *VListWidget;

#endif /* _VListP_h */
