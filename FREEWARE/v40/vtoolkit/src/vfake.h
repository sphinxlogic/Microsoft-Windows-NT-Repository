/* vfake.h - VFake widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains C-language declarations needed to use the VFake widget.
Include this file into any C module that directly uses a VFake widget.

MODIFICATION HISTORY:

06-Sep-1993 (sjk/nfc) Run through the MOTIF porting tools, prototypes added
	for global functions.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

11-Nov-1991 (sjk) Add VFakeNwindowType resource.

06-Nov-1991 (sjk) Cleanup.

21-Feb-1990 (sjk) Initial release.
*/

#ifndef _VFake_h
#define _VFake_h

/* Widget class */

external WidgetClass vfakewidgetclass;

/* Resource names */

#define VFakeNwindow			"window"
#define VFakeNwindowType		"windowType"

/* Values for VFakeNwindowType resource */

#define VFakeWindowTypeAny		1
#define VFakeWindowTypeRoot		2

/* global function prototypes */

Widget VFakeCreate(Widget, char *, Arg *, int);
void VFakeInitializeForMRM(void);

/* Historical synonyms */

#define VFakeInitializeForDRM	VFakeInitializeForMRM

#endif /* _VFake_h */
