/* vdrag.h - VDrag widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains utility routines that support dragging windows around within their parents.

MODIFICATION HISTORY:

06-Sep-1993 (sjk/nfc) Initial entry.  Created in order to define 
        vdrag.c global function prototypes.
*/

/* prototypes for global functions */

Opaque VDragEnableGrab(Widget, int, int);
void VDragDisableGrab(Opaque);
void VDragEnable(Widget);
void VDragDisable(Widget);
