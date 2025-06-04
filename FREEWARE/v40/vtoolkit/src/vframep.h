/* vframep.h - VFrame widget private C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains the private C-language declarations needed by the VFrame
widget source code.  It also includes vframe.h.

MODIFICATION HISTORY:

28-Aug-1993 (sjc/nfc) Initial entry - extracted from vframe.c.
*/

#ifndef _VFrameP_h
#define _VFrameP_h

#include "vframe.h"

typedef struct {
    int			notused;
} VFrameClassPart;

typedef struct _VFrameClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    VFrameClassPart	vframe_class;
} VFrameClassRec, *VFrameClass;

typedef struct _VFrameRec {
    CorePart		core;		/* core widget fields */
    CompositePart	composite;	/* composite widget fields */

    /* resources */
    Pixel		foreground;
    XtCallbackList	resizeCallback;

    /* private context */
} VFrameWidgetRec, *VFrameWidget;

#endif /* _VFrameP_h */
