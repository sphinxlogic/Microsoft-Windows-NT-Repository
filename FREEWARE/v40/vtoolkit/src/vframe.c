/* vframe.c - VFrame widget implementation module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the VFrame widget.  The VFrame widget is a simple
container for other widgets.  The primary purpose of the VFrame widget is
to provide a resize callback.

MODIFICATION HISTORY:

12-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

14-Sep-1993 (sjk) Reapply bugfix from 27-May-1992 - Maintain positive sizes for all widgets during resize.

28-Aug-1993 (sjk/nfc) Run through Motif porting tools, widget structure definition moved to vframep.h

19-Mar-1992 (sjk) Do separate XtSetValues calls for borderWidth and position (in VFrameArrangeWidgets) to get around
	(apparently new) XUI toolkit bug.  Conditionalize #pragma for VMS only.

07-Nov-1991 (sjk) Cleanup.

03-Apr-1991 (sjk) Add VFrameGetScrollBarWidget and VFrameArrangeWidgets routines.  Reformat source.

19-Mar-1990 (sjk) Add VFrameNforeground resource so that DRM default foreground icon color can be inherited properly.
	Add BitGravity to avoid extra Expose events.

07-Feb-1990 (sjk) More portability tweaking, thanks to Nguyen Nguyen.

31-Jan-1990 (sjk) Add casting as necessary to improve portability.

19-Jan-1990 (sjk) Add MOTIF support.

28-Dec-1989 (sjk) Version X2.0 update.

01-Aug-1989 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vframep.h"
#include <Xm/ScrollBar.h>

static void DoResize(Widget ow)
{
    VFrameWidget w = (VFrameWidget)ow;
    VFrameResizeCallbackStruct cbData;

    cbData.reason = VFrameCRResize;
    cbData.event  = 0;

    XtCallCallbacks ((Widget)w, VFrameNresizeCallback, &cbData);
}

static void DoRealize(Widget ow, XtValueMask *maskP, XSetWindowAttributes *attributesP)
{
    VFrameWidget w = (VFrameWidget)ow;

    if (!w->core.width)  w->core.width  = 1;
    if (!w->core.height) w->core.height = 1;

    /* Give a BitGravity to minimize repaints during resize. */

    *maskP |= CWBitGravity;
    attributesP->bit_gravity = NorthWestGravity;

    XtCreateWindow ((Widget)w, InputOutput, CopyFromParent, *maskP, attributesP);
}

#define XX(n, c, r, s, o, dr, dv)       {n, c, r, sizeof(s), XtOffset (VFrameWidget, o), dr, (char *)dv},

static XtResource resources[] = {
XX (VFrameNforeground,	   XtCForeground, XtRPixel,	Pixel,   	foreground,     XtRString,   XtDefaultForeground)
XX (VFrameNresizeCallback, XtCCallback,   XtRCallback,  XtCallbackList, resizeCallback, XtRCallback, 0)
};

extern XtGeometryResult VtkPermissiveGeometryManager();         /* permit anything */
extern void VtkNullRoutine();					/* do nothing */

externaldef(vframewidgetclassrec) VFrameClassRec vframewidgetclassrec = {
    {/* core class fields	*/
	/* superclass		*/	(WidgetClass)&compositeClassRec,
	/* class_name		*/	"VFrame",
	/* widget_size		*/	sizeof (VFrameWidgetRec),
	/* class_initialize	*/	0,
	/* class_part_initialize */	0,
	/* class_inited		*/	0,
	/* initialize		*/	0,
	/* initialize_hook	*/	0,
	/* realize		*/	DoRealize,
	/* actions		*/    	0,
	/* num_actions	  	*/	0,
	/* resources		*/	resources,
	/* num_resources	*/	XtNumber (resources),
	/* xrm_class		*/	0,
	/* compress_motion	*/	0,
	/* compress_exposure	*/	0,
	/* compress_enterleave	*/	0,
	/* visible_interest	*/	0,
	/* destroy		*/	0,
	/* resize		*/	DoResize,
	/* expose		*/	0,
	/* set_values		*/	0,
	/* set_values_hook	*/	0,
	/* set_values_almost	*/	(XtAlmostProc)_XtInherit,
	/* get_values_hook	*/	0,
	/* accept_focus		*/	0,
	/* version		*/	XtVersionDontCheck,
	/* callback_private	*/	0,
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

externaldef(vframewidgetclass) WidgetClass vframewidgetclass = (WidgetClass)&vframewidgetclassrec;

Widget VFrameCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
{
    return (XtCreateWidget (nameP, vframewidgetclass, pW, argsP, argCnt));
}

void VFrameInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "VFrame", "VFrameCreate", VFrameCreate, vframewidgetclass);
}

Widget VFrameGetScrollBarWidget(Widget w, int orientation)
{
    int numChildren = (int)VtkGetOneValue (w, XmNnumChildren);
    Widget *childrenP = (Widget *)VtkGetOneValue (w, XmNchildren);
    Widget childW;
    int i;

    for (i = 0; i < numChildren; i++) {
	childW = childrenP[i];

	if (XtIsSubclass (childW, xmScrollBarWidgetClass)) {
	    int childOrientation = 0;    /* in case a char or short int is returned */

            {Arg args[1];
            XtSetArg (args[0], XmNorientation, &childOrientation);
            XtGetValues (childW, args, XtNumber (args));}

	    if (childOrientation == orientation) return (childW);
        }
    }

    return ((Widget) 0);
}

void VFrameArrangeWidgets(Widget ow, Widget workW, Widget hScrollW, Widget vScrollW)
{
    VFrameWidget w = (VFrameWidget)ow;
    int workWidth, workHeight;

    if (hScrollW && (!XtIsManaged (hScrollW))) hScrollW = 0;
    if (vScrollW && (!XtIsManaged (vScrollW))) vScrollW = 0;

    workWidth = w->core.width;
    if (vScrollW) workWidth -= (vScrollW->core.width + vScrollW->core.border_width);
    if (workWidth <= 0) workWidth = 1;

    workHeight = w->core.height;
    if (hScrollW) workHeight -= (hScrollW->core.height + hScrollW->core.border_width);
    if (workHeight <= 0) workHeight = 1;

    /* Do separate XtSetValues calls for position and size to avoid an XUI V2 toolkit bug. */

    if (hScrollW) {
        XtVaSetValues (hScrollW,
	    XtNx, -hScrollW->core.border_width,
            XtNy, workHeight,
	    0);

        XtVaSetValues (hScrollW, XtNwidth, workWidth, 0);
    }

    if (vScrollW) {
        XtVaSetValues (vScrollW,
	    XtNx, workWidth,
            XtNy, -vScrollW->core.border_width,
	    0);

        XtVaSetValues (vScrollW, XtNheight, workHeight, 0);
    }

    XtVaSetValues (workW, XtNborderWidth, 0, 0);

    XtVaSetValues (workW,
	XtNx, 		0,
	XtNy, 		0,
	XtNwidth, 	workWidth,
	XtNheight, 	workHeight,
	0);
}
