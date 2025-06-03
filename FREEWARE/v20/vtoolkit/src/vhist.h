/* vhist.h - VHist widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains C-language declarations needed to use the VHist widget.
Include this file into any C module that directly uses a VHist widget.

MODIFICATION HISTORY:

01-Sep-1993 (sjk/nfc) Run through the Motif poorting tools.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

06-Nov-1991 (sjk) Cleanup.  Add VHistNunitType resource.

15-Oct-1991 (tmr) Added VHistInitializeMRM and conditionalize include file.

01-Jun-1990 (sjk) Add VHistNuserData resource.  Add the following fields:
    VHistBar.mask, VHistBar.coloridx, VHistBar.stippled, VHistBar.closure
    VHistGrid.mask, VHistGrid.coloridx, VHistGrid.closure, and
    VHistSelectCallbackStruct.vBar.  Add VHistBar*Mask and VHistGrid*Mask
    definitions.

05-Apr-1990 (sjk) Add VHistOrientationBestFit option.

02-Jan-1990 (sjk) Select callbacks.

12-Aug-1989 (sjk) Initial entry.
*/

#ifndef _VHist_h
#define _VHist_h

/* Widget class declaration */

external WidgetClass 		vhistwidgetclass;

/* Resource names */

#define VHistNbarColor			"barColor"
#define VHistNbarCount			"barCount"
#define VHistNbars			"bars"
#define VHistNbottomAxisMarginHeight	"bottomAxisMarginHeight"
#define VHistNbottomAxisThickness	"bottomAxisThickness"
#define VHistNbottomLabelHeight		"bottomLabelHeight"
#define VHistNbottomMarginHeight	"bottomMarginHeight"
#define VHistNcolorList			"colorList"
#define VHistNdoubleClickDelay		"doubleClickDelay"
#define VHistNfont			"font"
#define VHistNforeground		"foreground"
#define VHistNgridCount			"gridCount"
#define VHistNgrids			"grids"
#define VHistNleftAxisMarginWidth	"leftAxisMarginWidth"
#define VHistNleftAxisThickness		"leftAxisThickness"
#define VHistNleftLabelAlignment	"leftLabelAlignment"
#define VHistNleftLabelWidth		"leftLabelWidth"
#define VHistNleftMarginWidth		"leftMarginWidth"
#define VHistNmaxValue			"maxValue"
#define VHistNminValue			"minValue"
#define VHistNorientation		"orientation"
#define VHistNrightAxisMarginWidth	"rightAxisMarginWidth"
#define VHistNrightAxisThickness	"rightAxisThickness"
#define VHistNrightLabelAlignment	"rightLabelAlignment"
#define VHistNrightLabelWidth		"rightLabelWidth"
#define VHistNrightMarginWidth		"rightMarginWidth"
#define VHistNselectCallback		"selectCallback"
#define VHistNselectConfirmCallback	"selectConfirmCallback"
#define VHistNspacingDenominator	"spacingDenominator"
#define VHistNspacingNumerator		"spacingNumerator"
#define VHistNtopAxisMarginHeight	"topAxisMarginHeight"
#define VHistNtopAxisThickness		"topAxisThickness"
#define VHistNtopLabelHeight		"topLabelHeight"
#define VHistNtopMarginHeight		"topMarginHeight"
#define VHistNunitType			"unitType"
#define VHistNuserData			"userData"

/* Values for the VHistNorientation resource */

#define VHistOrientationVertical	1
#define VHistOrientationHorizontal	2
#define VHistOrientationBestFit		3

/* Values for the VHistN*LabelAlignment resources */

#define VHistAlignmentCenter		1
#define VHistAlignmentLeft		2
#define VHistAlignmentRight		3

/* Bar definition */

typedef struct {
    int		mask;		/* mask flags */
    int		lowValue;	/* value for low end of bar */
    int		highValue;	/* value for high end of bar */
    int		coloridx;	/* bar color index or pixel value */
    int		stippled;	/* bar is stippled (1/0) */
    Opaque	closure;	/* application-private closure value */
    char	*lowLabelP;	/* ASCIZ label at low end of bar */
    char	*highLabelP;	/* ASCIZ label at high end of bar */
} VHistBar;

/* Mask flag values for VHistBar.mask */

#define VHistBarLowValueMask		(1<<0)
#define VHistBarHighValueMask		(1<<1)
#define VHistBarColoridxMask		(1<<2)
#define VHistBarStippledMask		(1<<3)
#define VHistBarClosureMask		(1<<4)
#define VHistBarLowLabelMask		(1<<5)
#define VHistBarHighLabelMask		(1<<6)

/* Grid definition */

typedef struct {
    int		mask;		/* mask flags */
    int		value;		/* value */
    int		dashOn;		/* number of pixels on (or zero) */
    int		dashOff;	/* number of pixels off */
    int		coloridx;	/* bar color index or pixel value */
    Opaque	closure;	/* application-private closure value */
    char	*lowLabelP;	/* ASCIZ label at low end of grid */
    char	*highLabelP;	/* ASCIZ label at high end of grid */
} VHistGrid;

/* Mask flag values for VHistGrid.mask */

#define VHistGridValueMask		(1<<0)
#define VHistGridDashMask		(1<<1)
#define VHistGridColoridxMask		(1<<2)
#define VHistGridClosureMask		(1<<3)
#define VHistGridLowLabelMask		(1<<4)
#define VHistGridHighLabelMask		(1<<5)

/* Callback reasons */

#define VHistCRSelect			1
#define VHistCRSelectConfirm		2

/* Callback structure */

typedef struct {
    int		reason;		/* VHistCRSelect or VHistCRSelectConfirm */
    XEvent	*event;		/* event which caused callback */
    int		bar;		/* which bar was selected (or -1) */
    VHistBar	vBar;		/* bar description (closure may be zero) */
} VHistSelectCallbackStruct;

/* Public entrypoints */

int VHistXYToBar(Widget, int, int);
Widget VHistCreate(Widget, char *, Arg *, int);
void VHistInitializeForMRM(void);

/* Define historical synonyms for the VHistInitializeForMRM entrypoint. */

#define VHistInitializeMRM	VHistInitializeForMRM
#define VHistInitializeDRM	VHistInitializeForMRM
#define VHistInitializeForDRM	VHistInitializeForMRM

#endif /* _VHist_h */
