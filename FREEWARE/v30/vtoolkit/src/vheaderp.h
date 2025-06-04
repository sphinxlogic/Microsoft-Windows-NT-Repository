/* vheaderp.h - VHeader widget C-language private include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains the private C-language declarations needed to use the VHeader
widget source code. 

MODIFICATION HISTORY:

30-Aug-1993 (sjc/nfc) Initial entry - created from elements taken from the old vheader.h and
	vheader.c after they had been run through Motif porting tools.
*/

#ifndef _VHeaderP_h
#define _VHeaderP_h

#include "vheader.h"

typedef struct {
    int			notused;
} VHeaderClassPart;

typedef struct _VHeaderClassRec {
    CoreClassPart	core_class;
    VHeaderClassPart	vheader_class;
} VHeaderClassRec, *VHeaderClass;

typedef struct {
    VHeaderLabel	vLabel;		/* clone of supplied values */
    int			textStrlen;	/* length in bytes of text */
    int			y;		/* y-position of text */
} LabelInfo;

typedef struct _VHeaderRec {		/* Widget record */
    CorePart		core;		/* Core widget fields */

    /* Resource fields */
    int			bottomMarginHeight;
    int			dashOff;
    int			dashOn;
    struct _XmFontListRec	*fontTableP;
    Pixel		foreground;
    int			labelCnt;
    VHeaderLabel	*labelsP;
    int			spacingHeight;
    int			textMarginWidth;
    int			topMarginHeight;
    int			xAdjust;

    /* Private fields */
    Opaque		beginZeroInit;
    int			charHeight;		/* char height in pixels */
    int			externalXAdjust; 	/* recently acknowledged position */
    XFontStruct		*fontP;			/* font struct pointer */
    LabelInfo		*labelInfosP;		/* array of LabelInfo entries */
    GC			normgc;			/* graphics context */
    GC			dashgc;			/* dashed line GC */
    int			requestedXAdjust; 	/* recently requested position */
    int			scrollIsActive; 	/* XCopyArea is in progress */
    int			yFontAdjust;		/* adjustment for text positioning */
    Opaque		endZeroInit;
} VHeaderWidgetRec, *VHeaderWidget;

#endif /* _VHeaderP_h */
