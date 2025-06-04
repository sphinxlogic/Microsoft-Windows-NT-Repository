/* vheader.h - VHeader widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains the C-language declarations needed to use the VHeader
widget.  Include this file into any C module that directly uses a VHeader widget.

MODIFICATION HISTORY:

28-Aug-1993 (sjk/nfc) Run through Motif porting tools, cleaned up, added global function prototypes
	and the VHeaderWidget structure definitions.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

06-Nov-1991 (sjk) Cleanup.

19-Jan-1990 (sjk) Add padWithDashes option.

28-Dec-1989 (sjk) Version X2.0 update.

07-Dec-1989 (sjk) Initial entry.
*/

#ifndef _VHeader_h
#define _VHeader_h

/* Widget class */

external WidgetClass vheaderwidgetclass;

/* Resource names */

#define VHeaderNbottomMarginHeight	"bottomMarginHeight"
#define VHeaderNdashOff			"dashOff"
#define VHeaderNdashOn			"dashOn"
#define VHeaderNfontTable		"fontTable"
#define VHeaderNforeground		"foreground"
#define VHeaderNlabelCount		"labelCount"
#define VHeaderNlabels			"labels"
#define VHeaderNspacingHeight		"spacingHeight"
#define VHeaderNtextMarginWidth		"textMarginWidth"
#define VHeaderNtopMarginHeight		"topMarginHeight"
#define VHeaderNxAdjust			"xAdjust"

/* Label structure definition for VHeaderLabels resource */

typedef struct {
    char	*textP;		/* pointer to label's ASCIZ text */
    int		x;		/* x-position of field */
    int		textX;		/* x-position of text */
    int		row;		/* row */
    int		width;		/* width of field */
    int		textWidth;	/* width of text */
    int		pad;		/* pad option */
} VHeaderLabel;

/* Values for VHeaderLabel.pad */

#define VHeaderLabelPadWithBlanks	1
#define VHeaderLabelPadWithDashes	2

/* Public entrypoints */

Widget VHeaderCreate(Widget, char *, Arg *, int);
void VHeaderInitializeForMRM(void);
int VHeaderComputeLogicalHeight(Widget, int);
int VHeaderComputeLabelWidth(Widget, char *);

/* Historical synonyms */

#define VHeaderInitializeForDRM		VHeaderInitializeForMRM

#endif /* _VHeader_h */
