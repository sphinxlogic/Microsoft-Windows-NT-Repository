/* vheader.c - VHeader widget implementation module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the VHeader widget.

MODIFICATION HISTORY:

03-Dec-1993 (sjk) Even more portability work.  Thanks Roy!

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

17-Sep-1993 (sjk) Apply bugfix from VToolkit note 167 - Limited support for changing font after creation.  There is
	still a restriction that the layout of the VList widget will not be recomputed automatically when the VHeader
	font is changed, so it should be done within a VList batch update to force the VList to recompute layout.

16-Sep-1993 (sjk) Bugfix typedef for ALPHA/OSF.  Thanks Jean-Paul!

28-Aug-1993 (nfc) Run through Motif porting tools and the VHeaderWidget structure
	definitions moved to vheader.c.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

07-Nov-1991 (sjk) Cleanup and fix for FIXED font.

10-May-1990 (sjk) Use portable font routines, part of MOTIF BL1.1 port.

20-Apr-1990 (sjk) More portability work.

19-Mar-1990 (sjk) Use BitGravity to avoid extra Expose events.

16-Feb-1990 (sjk) Fix bug in the width calculation of 8-bit characters.

07-Feb-1990 (sjk) More portability tweaking, thanks to Nguyen Nguyen and Karl Freburger.

31-Jan-1990 (sjk) Add casting as necessary to improve portabilitiy.

22-Jan-1990 (sjk) Add label padding options.

19-Jan-1990 (sjk) Add MOTIF support.

28-Dec-1989 (sjk) Version x2.0 update.

07-Dec-1989 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vheaderp.h"

static void FreeLabels(VHeaderWidget w)
{
    LabelInfo *lInfoP;
    int label;

    for (label=0, lInfoP=w->labelInfosP; label<w->labelCnt; label++, lInfoP++) XtFree (lInfoP->vLabel.textP);
}

static void CopyLabels(VHeaderWidget w)
{
    VHeaderLabel *vLabelP;
    LabelInfo *lInfoP;
    int label;

    if (!w->labelCnt) return;

    w->labelInfosP = (LabelInfo *)XtRealloc ((char *)w->labelInfosP, w->labelCnt * sizeof (LabelInfo));

    for (label=0, vLabelP=w->labelsP, lInfoP=w->labelInfosP; label<w->labelCnt; label++, vLabelP++, lInfoP++) {
	lInfoP->vLabel = (*vLabelP);
	lInfoP->vLabel.textP = XtNewString (lInfoP->vLabel.textP);
    }

    w->labelsP = (VHeaderLabel *)(-1);	/* and don't look at it again */
}

static void ComputeLabelInfo(VHeaderWidget w)
{
    int label;
    LabelInfo *lInfoP;

    if (!w->labelCnt) return;

    for (label=0, lInfoP=w->labelInfosP; label<w->labelCnt; label++, lInfoP++) {
        lInfoP->textStrlen = strlen (lInfoP->vLabel.textP);
	lInfoP->y          = w->topMarginHeight + lInfoP->vLabel.row * (w->charHeight + w->spacingHeight);
    }
}

static void Draw(VHeaderWidget w, LabelInfo *lInfoP)
{
    int x0, x1, y;

    XDrawString (XtDisplay (w), XtWindow (w), w->normgc, lInfoP->vLabel.textX - w->requestedXAdjust, lInfoP->y + w->yFontAdjust,
	lInfoP->vLabel.textP, lInfoP->textStrlen);

    if (lInfoP->vLabel.pad == VHeaderLabelPadWithDashes) {
	y = lInfoP->y + w->charHeight / 2;

	x0 = lInfoP->vLabel.x - w->requestedXAdjust;
	x1 = lInfoP->vLabel.textX - 1 - w->textMarginWidth - w->requestedXAdjust;
	if (x0 < x1) XDrawLine (XtDisplay (w), XtWindow (w), w->dashgc, x0, y, x1, y);

	x0 = lInfoP->vLabel.textX + lInfoP->vLabel.textWidth + w->textMarginWidth - w->requestedXAdjust;
	x1 = lInfoP->vLabel.x + lInfoP->vLabel.width - 1 - w->requestedXAdjust;
	if (x0 < x1) XDrawLine (XtDisplay (w), XtWindow (w), w->dashgc, x0, y, x1, y);
    }
}

static void DrawExposedRegion(VHeaderWidget w, int vX, int y, int width, int height)
{
    int label;
    LabelInfo *lInfoP;

    for (label=0, lInfoP=w->labelInfosP; label<w->labelCnt; label++, lInfoP++)
	if (((lInfoP->vLabel.x + lInfoP->vLabel.width) > vX)
	  && ((vX + width) > lInfoP->vLabel.x)
	  && ((lInfoP->y + w->charHeight) > y)
	  && ((y + height) > lInfoP->y)) Draw (w, lInfoP);
}

static void SetPosition(VHeaderWidget w)
{
    w->externalXAdjust = w->requestedXAdjust = w->xAdjust;
}

static void AdjustPosition(VHeaderWidget w)
{
    if (w->xAdjust == w->requestedXAdjust) return;	/* nothing to do */
    if (w->scrollIsActive) return;			/* already busy */

    XCopyArea (XtDisplay (w), XtWindow (w), XtWindow (w), w->normgc, w->xAdjust - w->externalXAdjust, 0,
	w->core.width, w->core.height, 0, 0);

    w->requestedXAdjust = w->xAdjust;
    w->scrollIsActive = 1;
}

static void NonmaskableEventHandler(Widget ow, XtPointer unused_closure, XEvent *eventP, Boolean *unused_continueP)
{
    VHeaderWidget w = (VHeaderWidget)ow;

    if ((eventP->xany.type != GraphicsExpose) && (eventP->xany.type != NoExpose)) return;

    if (eventP->xany.type == GraphicsExpose) {
	DrawExposedRegion (w, eventP->xgraphicsexpose.x + w->requestedXAdjust,
	    eventP->xgraphicsexpose.y, eventP->xgraphicsexpose.width, eventP->xgraphicsexpose.height);
    
	if (eventP->xgraphicsexpose.count) return;
    }

    w->externalXAdjust = w->requestedXAdjust;
    w->scrollIsActive = 0;

    AdjustPosition (w);
}

static void CopyFont(VHeaderWidget w)
{
    /* Use first font in list. */

    if (!w->fontTableP) w->fontTableP = (XmFontList)VtkGetDefaultFontList ((Widget)w);

    w->fontP = (XFontStruct *)VtkGetFirstFont (w->fontTableP);
    w->charHeight  = w->fontP->max_bounds.descent + w->fontP->max_bounds.ascent;
    w->yFontAdjust = w->fontP->max_bounds.ascent;
}

static void DoInitialize(Widget unused_requestOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VHeaderWidget w = (VHeaderWidget)ow;

    VtkSetFieldsToZero ((char *)&w->beginZeroInit, (char *)&w->endZeroInit);

    CopyFont (w);
    CopyLabels (w);
}

static void DoRealize(Widget ow, XtValueMask *maskP, XSetWindowAttributes *attributesP)
{
    VHeaderWidget w = (VHeaderWidget)ow;

    ComputeLabelInfo (w);

    if (!w->core.width)  w->core.width  = 1;
    if (!w->core.height) w->core.height = 1;

    XtAddEventHandler ((Widget)w, 0, 1, NonmaskableEventHandler, w);

    /* Give a BitGravity to minimize repaints during resize. */

    *maskP |= CWBitGravity;
    attributesP->bit_gravity = NorthWestGravity;

    XtCreateWindow ((Widget)w, InputOutput, CopyFromParent, *maskP, attributesP);

    {XGCValues values;
    values.font  	= w->fontP->fid;
    values.foreground 	= w->foreground;
    values.background 	= w->core.background_pixel;
    values.function 	= GXcopy;
    w->normgc = XCreateGC (XtDisplay (w), XtWindow (w), GCFont | GCForeground | GCBackground | GCFunction, &values);}

    {XGCValues values;
    values.function 	= GXcopy;
    values.foreground 	= w->foreground;
    values.background 	= w->core.background_pixel;
    values.line_style 	= LineOnOffDash;
    w->dashgc = XCreateGC (XtDisplay (w), XtWindow (w), GCForeground | GCBackground | GCFunction | GCLineStyle, &values);}

    {char dashList[2];

    dashList[0] = w->dashOn;
    dashList[1] = w->dashOff;
    XSetDashes (XtDisplay (w), w->dashgc, 0, dashList, XtNumber (dashList));}
}

static void DoExpose(Widget ow, XEvent *eventOP, Region unused_region)
{
    VHeaderWidget w = (VHeaderWidget)ow;
    XExposeEvent *eventP = (XExposeEvent *)eventOP;

    DrawExposedRegion (w, eventP->x + w->externalXAdjust, eventP->y, eventP->width, eventP->height);
}

#define Changed(f) (w->f != oldW->f)

static Boolean DoSetValues(Widget oldOW, Widget unused_refOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VHeaderWidget oldW = (VHeaderWidget)oldOW;
    VHeaderWidget w = (VHeaderWidget)ow;
    int redraw = 0;

    if (Changed (labelsP) || Changed (labelCnt)) {
	FreeLabels (oldW);
	CopyLabels (w);
	if (VtkIsRealized (w)) ComputeLabelInfo (w);
	redraw = 1;
    }

    if (Changed (xAdjust))
	if (redraw || (!VtkIsRealized (w))) SetPosition    (w);
	else 				    AdjustPosition (w);

    if (Changed (fontTableP)) {
	CopyFont (w);
	if (VtkIsRealized (w)) XSetFont (XtDisplay (w), w->normgc, w->fontP->fid);
	redraw = 1;
    }

    return (redraw);
}

#define XY(n, c, r, t, f, dr, d)        {n, c, r, sizeof(t), XtOffset(VHeaderWidget, f), dr, (char *)d},
#define XX(n, c, r, t, f, d)            XY (n, c, r, t, f, XtRImmediate, d)
#define XI(n, f, d)                     XX (n, XtCValue, XtRInt, int, f, d)

static XtResource resources[] = {
XI (VHeaderNbottomMarginHeight,	bottomMarginHeight, 	1)
XI (VHeaderNdashOff, 		dashOff, 		3)
XI (VHeaderNdashOn, 		dashOn, 		1)
XI (VHeaderNlabelCount, 	labelCnt, 		0)
XI (VHeaderNspacingHeight, 	spacingHeight, 		1)
XI (VHeaderNtextMarginWidth, 	textMarginWidth, 	4)
XI (VHeaderNtopMarginHeight, 	topMarginHeight, 	1)
XI (VHeaderNxAdjust, 		xAdjust, 		0)

XX (VHeaderNlabels, 	XtCValue, 	XtRPointer, 	VHeaderLabel *, labelsP,        (VHeaderLabel *)0)

XY (VHeaderNfontTable,  XmCFontList,  	XmRFontList, 	struct _XmFontListRec *, 	fontTableP, XtRString, 0)
XY (VHeaderNforeground, XtCForeground, 	XtRPixel,     	Pixel,        	foreground, XtRString, XtDefaultForeground)
};

externaldef(vheaderwidgetclassrec) VHeaderClassRec vheaderwidgetclassrec = {
    {/* core_class fields	*/
	/* superclass		*/	(WidgetClass)&widgetClassRec,
	/* class_name	  	*/	"VHeader",
	/* widget_size	  	*/	sizeof (VHeaderWidgetRec),
	/* class_initialize   	*/    	0,
	/* class_part_initialize */	0,
	/* class_inited       	*/	0,
	/* initialize	  	*/	DoInitialize,
	/* initialize_hook	*/	0,
	/* realize		*/	DoRealize,
	/* actions		*/    	0,
	/* num_actions	  	*/	0,
	/* resources	  	*/	resources,
	/* num_resources	*/	XtNumber (resources),
	/* xrm_class	  	*/	0,
	/* compress_motion	*/	0,
	/* compress_exposure  	*/	0,
	/* compress_enterleave	*/	0,
	/* visible_interest	*/	0,
	/* destroy		*/	0,
	/* resize		*/	0,
	/* expose		*/	DoExpose,
	/* set_values	  	*/	DoSetValues,
	/* set_values_hook	*/	0,
	/* set_values_almost  	*/	XtInheritSetValuesAlmost,
	/* get_values_hook    	*/	0,
	/* accept_focus	  	*/	0,
	/* version		*/	XtVersionDontCheck,
	/* callback_private   	*/	0,
	/* tm_table		*/	0,
	/* query_geometry	*/	0,
	/* display_accelerator	*/	0,
	/* extension		*/	0
  }
};

externaldef(vheaderwidgetclass) WidgetClass vheaderwidgetclass =  (WidgetClass)&vheaderwidgetclassrec;

Widget VHeaderCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
{
    return (XtCreateWidget (nameP, vheaderwidgetclass, pW, argsP, argCnt));
}

void VHeaderInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "VHeader", "VHeaderCreate", VHeaderCreate, vheaderwidgetclass);
}

int VHeaderComputeLabelWidth(Widget ow, char *textP)
{
    VHeaderWidget w        = (VHeaderWidget)ow;
    int width 	           = 0;
    unsigned char *chP 	   = (unsigned char *)textP;
    XCharStruct *charP     = w->fontP->per_char;
    unsigned int firstCol  = w->fontP->min_char_or_byte2;
    unsigned int numCols   = w->fontP->max_char_or_byte2 - firstCol + 1;
    unsigned int c;

    while (*chP) {
	c = *(chP++) - firstCol;
	if (c < numCols) 
	    if (charP) width += charP[c].width;
	    else       width += w->fontP->max_bounds.width;
    }

    return (width);
}

int VHeaderComputeLogicalHeight(Widget ow, int rows)
{
    VHeaderWidget w = (VHeaderWidget)ow;

    return (rows * w->charHeight + (rows - 1) * w->spacingHeight + w->topMarginHeight + w->bottomMarginHeight);
}
