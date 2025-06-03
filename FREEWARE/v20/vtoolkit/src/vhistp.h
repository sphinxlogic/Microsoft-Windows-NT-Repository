/* vhistp.h - VHist widget C-language private include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains C-language declarations needed to use the VHist widget.
Include this file into any C module that directly uses a VHist widget.

MODIFICATION HISTORY:

03-Sep-1993 (sjk/nfc) Initial entry.
*/

#ifndef _VHistP_h
#define _VHistP_h

#include "vhist.h"

/* VHist widget record definitions */

typedef struct {
    int			notused;
} VHistClassPart;

typedef struct _VHistClassRec {
    CoreClassPart	core_class;
    VHistClassPart	vhist_class;
} VHistClassRec, *VHistClass;

typedef struct {
    int			x;
    int			y;
    int			width;
    char		*labelP;
    int			redraw;
} LabelInfo;

typedef struct {
    Opaque		beginZeroInit;
    VHistBar		vBar;		/* copy of user-supplied entry */
    int			x;		/* OrientationVertical */
    int			topY;		/* OrientationVertical */
    int			bottomY;	/* OrientationVertical */
    int			y;		/* OrientationHorizontal */
    int			leftX;		/* OrientationHorizontal */
    int			rightX;		/* OrientationHorizontal */
    LabelInfo		lowLabelInfo;
    LabelInfo		highLabelInfo;
    Opaque		endZeroInit;
} BarInfo;

typedef struct {
    Opaque		beginZeroInit;
    VHistGrid		vGrid;		/* copy of user-supplied entry */
    int			y;		/* OrientationVertical */
    int			x;		/* OrientationHorizontal */
    LabelInfo		lowLabelInfo;
    LabelInfo		highLabelInfo;
    Opaque		endZeroInit;
} GridInfo;

/* Widget record */

typedef struct _VHistRec {
    CorePart		core;		/* Core widget fields */

    /* Resource fields */
    int			barCnt;
    Pixel		barColor;
    VHistBar		*barsP;
    int			bottomAxisMarginHeightU;
    int			bottomAxisThickness;
    int			bottomLabelHeightU;
    int			bottomMarginHeightU;
    Pixmap              colorList;
    int			doubleClickDelay;
    struct _XmFontListRec		*fontTableP;
    Pixel		foreground;
    int			gridCnt;
    VHistGrid		*gridsP;
    int			leftAxisMarginWidthU;
    int			leftAxisThickness;
    int			leftLabelAlignment;
    int			leftLabelWidthU;
    int			leftMarginWidthU;
    int			maxValue;
    int			minValue;
    int			orientation;
    int			rightAxisMarginWidthU;
    int			rightAxisThickness;
    int			rightLabelAlignment;
    int			rightLabelWidthU;
    int			rightMarginWidthU;
    XtCallbackList	selectCallback;
    XtCallbackList	selectConfirmCallback;
    int			spacingDenominator;
    int			spacingNumerator;
    int			topAxisMarginHeightU;
    int			topAxisThickness;
    int			topLabelHeightU;
    int			topMarginHeightU;
    int			unitType;
    Opaque		userData;

    /* Private fields */
    Opaque		beginZeroInit;
    int			actualOrientation;
    GC			barGC;
    Pixel		barGCColor;
    int			barInfoCnt;
    BarInfo		*barInfosP;
    int			barWidth;
    int			bottomAxisMarginHeight;
    int			bottomLabelHeight;
    int			bottomLabelY;
    int			bottomMarginHeight;
    int			buttonIsDown;
    int			charHeight;
    int                 colorCnt;
    Pixel               *colorTableP;
    int			doubleClickIsPending;
    int			downBar;
    XFontStruct		*fontP;
    GC			gridGC;
    Pixel		gridGCColor;
    int			gridGCDashOff;
    int			gridGCDashOffset;
    int			gridGCDashOn;
    int			gridInfoCnt;
    GridInfo		*gridInfosP;
    int			horizontalAxisLength;
    int			insideBottomY;
    int			insideLeftX;
    int			insideRightX;
    int			insideTopY;
    int			lastUsedClosure;
    int			leftAxisMarginWidth;
    int			leftLabelWidth;
    int			leftLabelX;
    int			leftMarginWidth;
    GC			normGC;
    int			rightAxisMarginWidth;
    int			rightLabelWidth;
    int			rightLabelX;
    int			rightMarginWidth;
    int			topAxisMarginHeight;
    int			topLabelHeight;
    int			topLabelY;
    int			topMarginHeight;
    int			totalBottomMarginHeight;
    int			totalLeftMarginWidth;
    int			totalRightMarginWidth;
    int			totalTopMarginHeight;
    int			upBar;
    int			upTime;
    int			verticalAxisLength;
    int			yFontAdjust;
    Opaque		endZeroInit;
} VHistWidgetRec, *VHistWidget;

#endif /* _VHistP_h */
