/* vframe.h - VFrame widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains the C-language declarations needed to use the VFrame
widget.  Include this file into any C module that directly uses a VFrame widget.

MODIFICATION HISTORY:

28-Aug-1993 (sjk/nfc) Put through Motif porting tools, cleaned up, and added global function prototypes

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

06-Nov-1991 (sjk) Cleanup.

03-Apr-1991 (sjk) Added VFrameInitializeDRM synonym for consistency.

19-Mar-1990 (sjk) Added VFrameNforeground resource.

28-Dec-1989 (sjk) Version X2.0 update.

01-Aug-1989 (sjk) Initial entry.
*/

#ifndef _VFrame_h
#define _VFrame_h

/* Widget class */

external WidgetClass vframewidgetclass;

/* Resource names */

#define VFrameNforeground	"foreground"
#define VFrameNresizeCallback	"resizeCallback"

/* Callback reasons */

#define VFrameCRResize		1

/* Callback structures */

typedef struct {
    int		reason;
    XEvent	*event;
} VFrameResizeCallbackStruct;

/* Public entrypoints */

void VFrameArrangeWidgets(Widget, Widget, Widget, Widget);
void VFrameInitializeForMRM(void);
Widget VFrameCreate(Widget, char *, Arg *, int);
Widget VFrameGetScrollBarWidget(Widget, int);

/* Historical synonyms */

#define VFrameInitializeDRM	VFrameInitializeForMRM
#define VFrameInitializeForDRM	VFrameInitializeForMRM

#endif /* _VFrame_h */
