/* vfake.c - VFake widget implementation

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the VFake widget.

MODIFICATION HISTORY:

12-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

06-Sep-1993 (sjk/nfc) Run through the MOTIF porting tools and functions changed
	to prototypes definitions.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

11-Nov-1991 (sjk) Add VFakeNwindowType resource and support for VFakeWindowTypeRoot.

06-Nov-1991 (sjk) Cleanup.

05-Apr-1990 (sjk) Add MOTIF support.

21-Feb-1990 (sjk) Initial public release.
*/

#include "vtoolkit.h"
#include "vfake.h"

typedef struct {			/* widget class contribution */
    int			notused;
} VFakeClassPart;

typedef struct {			/* widget class record */
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    VFakeClassPart	vfake_class;
} VFakeClassRec, *VFakeClass;

typedef struct {			/* Widget record */
    CorePart		core;		/* Core widget fields */
    CompositePart	composite;	/* composite widget fields */

    /* Resource fields */
    Window		window;
    int			windowType;
} VFakeWidgetRec, *VFakeWidget;

static void DoRealize(Widget ow, XtValueMask *unused_maskP, XSetWindowAttributes *unused_attributesP)
{
    VFakeWidget w = (VFakeWidget)ow;

    switch (w->windowType) {
      case VFakeWindowTypeAny	: w->core.window = w->window;				break;
      case VFakeWindowTypeRoot	: w->core.window = RootWindowOfScreen (XtScreen (w));	break;
    }
}

#define Offset(x) XtOffset(VFakeWidget, x)

static XtResource resources[] = {
 {VFakeNwindow,     XtCValue, XtRWindow, sizeof(Window), Offset(window),     XtRImmediate, 0},
 {VFakeNwindowType, XtCValue, XtRInt,    sizeof(int),    Offset(windowType), XtRImmediate, (char *)VFakeWindowTypeAny},
};

externaldef(vfakewidgetclassrec) VFakeClassRec vfakewidgetclassrec = {
    {/* core_class fields	*/
	/* superclass		*/	(WidgetClass)&compositeClassRec,
	/* class_name	  	*/	"VFake",
	/* widget_size	  	*/	sizeof(VFakeWidgetRec),
	/* class_initialize   	*/    	0,
	/* class_part_initialize */	0,
	/* class_inited       	*/	0,
	/* initialize	  	*/	0,
	/* initialize_hook	*/	0,
	/* realize		*/	DoRealize,
	/* actions		*/    	0,
	/* num_actions	  	*/	0,
	/* resources	  	*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class	  	*/	0,
	/* compress_motion	*/	0,
	/* compress_exposure  	*/	0,
	/* compress_enterleave	*/	0,
	/* visible_interest	*/	0,
	/* destroy		*/	0,
	/* resize		*/	0,
	/* expose		*/	0,
	/* set_values	  	*/	0,
	/* set_values_hook	*/	0,
	/* set_values_almost  	*/	(XtAlmostProc)_XtInherit,
	/* get_values_hook    	*/	0,
	/* accept_focus	  	*/	0,
	/* version		*/	XtVersionDontCheck,
	/* callback_private   	*/	0,
	/* tm_table		*/	0,
	/* query_geometry	*/	0,
	/* display_accelerator	*/	0,
	/* extension		*/	0
    },
    {/* composite class fields	*/
	/* geometry_manager	*/	VtkPermissiveGeometryManager,
	/* change_managed	*/	(XtWidgetProc)VtkNullRoutine,
	/* insert_child		*/	(XtWidgetProc)_XtInherit,
	/* delete_child		*/	(XtWidgetProc)_XtInherit,
	/* extension		*/	0
    }
};

externaldef(vfakewidgetclass) WidgetClass vfakewidgetclass = (WidgetClass)&vfakewidgetclassrec;

Widget VFakeCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
/*    Widget pW;     parent (ignored)
**    char *nameP;   widget name
**    Arg *argsP;    pointer to argument vector
**    int argCnt;    number of arguments in vector 
*/
{
    Widget w = XtCreatePopupShell (nameP, vfakewidgetclass, pW, argsP, argCnt);

    return (w);
}

void VFakeInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "VFake", "VFakeCreate", VFakeCreate, vfakewidgetclass);
}
