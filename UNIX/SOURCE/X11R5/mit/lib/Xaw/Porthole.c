/*
 * $XFree86: mit/lib/Xaw/Porthole.c,v 1.3 1993/03/27 09:10:21 dawes Exp $
 * $XConsortium: Porthole.c,v 1.14 91/03/14 16:48:01 converse Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 * 
 * This widget is a trivial clipping widget.  It is typically used with a
 * panner or scrollbar to navigate.
 */

#include <X11/IntrinsicP.h>		/* get basic toolkit stuff */
#include <X11/StringDefs.h>		/* get XtN and XtC defines */
#include <X11/Xaw/XawInit.h>		/* get Xaw initialize stuff */
#include <X11/Xaw/PortholeP.h>		/* get porthole structs */
#include <X11/Xmu/Misc.h>		/* for MAX */


/*
 * resources for the porthole
 */
static XtResource resources[] = {
#define poff(field) XtOffsetOf(PortholeRec, porthole.field)
    { XtNreportCallback, XtCReportCallback, XtRCallback, sizeof(XtPointer),
	poff(report_callbacks), XtRCallback, (XtPointer) NULL },
#undef poff
};


/*
 * widget class methods used below
 */
static void Realize();			/* set gravity and upcall */
static void Resize();			/* report new size */
static XtGeometryResult GeometryManager();  /* deal with child requests */
static void ChangeManaged();		/* somebody added a new widget */
static XtGeometryResult QueryGeometry();  /* say how big would like to be */

PortholeClassRec portholeClassRec = {
  { /* core fields */
#ifndef SVR3SHLIB
    /* superclass		*/	(WidgetClass) &compositeClassRec,
#else
    /* superclass		*/	NULL,
#endif
    /* class_name		*/	"Porthole",
    /* widget_size		*/	sizeof(PortholeRec),
    /* class_initialize		*/	XawInitializeWidgetSet,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	NULL,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* composite fields */
    /* geometry_manager		*/	GeometryManager,
    /* change_managed		*/	ChangeManaged,
    /* insert_child		*/	XtInheritInsertChild,
    /* delete_child		*/	XtInheritDeleteChild,
    /* extension		*/	NULL
  },
  { /* porthole fields */
    /* ignore                   */	0
  }
};

#ifndef SVR3SHLIB
WidgetClass portholeWidgetClass = (WidgetClass) &portholeClassRec;
#else
extern WidgetClass portholeWidgetClass;
#endif


/*****************************************************************************
 *                                                                           *
 *			       utility routines                              *
 *                                                                           *
 *****************************************************************************/

static Widget find_child (pw)
    register PortholeWidget pw;
{
    register Widget *children;
    register int i;

    /*
     * Find the managed child on which we should operate.  Ignore multiple
     * managed children.
     */
    for (i = 0, children = pw->composite.children;
	 i < pw->composite.num_children; i++, children++) {
	if (XtIsManaged(*children)) return *children;
    }

    return (Widget) NULL;
}

static void SendReport (pw, changed)
    PortholeWidget pw;
    unsigned int changed;
{
    Widget child = find_child (pw);

    if (pw->porthole.report_callbacks && child) {
	XawPannerReport prep;

	prep.changed = changed;
	prep.slider_x = -child->core.x;	/* porthole is "inner" */
	prep.slider_y = -child->core.y;	/* child is outer since it is larger */
	prep.slider_width = pw->core.width;
	prep.slider_height = pw->core.height;
	prep.canvas_width = child->core.width;
	prep.canvas_height = child->core.height;
	XtCallCallbackList ((Widget)pw, pw->porthole.report_callbacks,
			    (XtPointer) &prep);
    }
}


static void layout_child (pw, child, geomp, xp, yp, widthp, heightp)
    PortholeWidget pw;
    Widget child;
    XtWidgetGeometry *geomp;
    Position *xp, *yp;
    Dimension *widthp, *heightp;
{
    Position minx, miny;

    *xp = child->core.x;		/* default to current values */
    *yp = child->core.y;
    *widthp = child->core.width;
    *heightp = child->core.height;
    if (geomp) {			/* mix in any requested changes */
	if (geomp->request_mode & CWX) *xp = geomp->x;
	if (geomp->request_mode & CWY) *yp = geomp->y;
	if (geomp->request_mode & CWWidth) *widthp = geomp->width;
	if (geomp->request_mode & CWHeight) *heightp = geomp->height;
    }

    /*
     * Make sure that the child is at least as large as the porthole; there
     * is no maximum size.
     */
    if (*widthp < pw->core.width) *widthp = pw->core.width;
    if (*heightp < pw->core.height) *heightp = pw->core.height;

    /*
     * Make sure that the child is still on the screen.  Note that this must
     * be done *after* the size computation so that we know where to put it.
     */
    minx = ((Position) pw->core.width) - ((Position) *widthp);
    miny = ((Position) pw->core.height) - ((Position) *heightp);

    if (*xp < minx) *xp = minx;		/* keep at lower right corner */
    if (*yp < miny) *yp = miny;

    if (*xp > 0) *xp = 0;		/* keep at upper left corner */
    if (*yp > 0) *yp = 0;
}



/*****************************************************************************
 *                                                                           *
 *			 Porthole Widget Class Methods                       *
 *                                                                           *
 *****************************************************************************/


static void Realize (gw, valueMask, attributes)
    register Widget gw;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    attributes->bit_gravity = NorthWestGravity;
    *valueMask |= CWBitGravity;

    if (gw->core.width < 1) gw->core.width = 1;
    if (gw->core.height < 1) gw->core.height = 1;
    (*portholeWidgetClass->core_class.superclass->core_class.realize)
	(gw, valueMask, attributes);
}


static void Resize (gw)
    Widget gw;
{
    PortholeWidget pw = (PortholeWidget) gw;
    Widget child = find_child (pw);

    /*
     * If we have a child, we need to make sure that it is at least as big
     * as we are and in the right place.
     */
    if (child) {
	Position x, y;
	Dimension width, height;

	layout_child (pw, child, NULL, &x, &y, &width, &height);
	XtConfigureWidget (child, x, y, width, height, (Dimension) 0);
    }

    SendReport (pw, (unsigned int) (XawPRCanvasWidth | XawPRCanvasHeight));
}


static XtGeometryResult QueryGeometry (gw, intended, preferred)
    Widget gw;
    XtWidgetGeometry *intended, *preferred;
{
    register PortholeWidget pw = (PortholeWidget) gw;
    Widget child = find_child (pw);

    if (child) {
#define SIZEONLY (CWWidth | CWHeight)
	preferred->request_mode = SIZEONLY;
	preferred->width = child->core.width;
	preferred->height = child->core.height;

	if (((intended->request_mode & SIZEONLY) == SIZEONLY) &&
	    intended->width == preferred->width &&
	    intended->height == preferred->height)
	  return XtGeometryYes;
	else if (preferred->width == pw->core.width &&
		 preferred->height == pw->core.height)
	  return XtGeometryNo;
	else
	  return XtGeometryAlmost;
#undef SIZEONLY
    } 
    return XtGeometryNo;
}


static XtGeometryResult GeometryManager (w, req, reply)
    Widget w;
    XtWidgetGeometry *req, *reply;
{
    PortholeWidget pw = (PortholeWidget) w->core.parent;
    Widget child = find_child (pw);
    Boolean okay = TRUE;

    if (child != w) return XtGeometryNo;  /* unknown child */

    *reply = *req;			/* assume we'll grant everything */

    if ((req->request_mode & CWBorderWidth) && req->border_width != 0) {
	reply->border_width = 0;	/* require border width of 0 */
	okay = FALSE;
    }

    layout_child (pw, child, req, &reply->x, &reply->y,
		  &reply->width, &reply->height);

    if ((req->request_mode & CWX) && req->x != reply->x) okay = FALSE;
    if ((req->request_mode & CWY) && req->x != reply->x) okay = FALSE;
    if ((req->request_mode & CWWidth) && req->width != reply->width)
      okay = FALSE;
    if ((req->request_mode & CWHeight) && req->height != reply->height)
      okay = FALSE;


    /*
     * If we failed on anything, simply return without touching widget
     */
    if (!okay) return XtGeometryAlmost;

    /*
     * If not just doing a query, update widget and send report.  Note that
     * we will often set fields that weren't requested because we want to keep
     * the child visible.
     */
    if (!(req->request_mode & XtCWQueryOnly)) {
	unsigned int changed = 0;

	if (child->core.x != reply->x) {
	    changed |= XawPRSliderX;
	    child->core.x = reply->x;
	}
	if (child->core.y != reply->y) {
	    changed |= XawPRSliderY;
	    child->core.y = reply->y;
	}
	if (child->core.width != reply->width) {
	    changed |= XawPRSliderWidth;
	    child->core.width = reply->width;
	}
	if (child->core.height != reply->height) {
	    changed |= XawPRSliderHeight;
	    child->core.height = reply->height;
	}
	if (changed) SendReport (pw, changed);
    }

    return XtGeometryYes;		/* success! */
}


static void ChangeManaged (gw)
    Widget gw;
{
    PortholeWidget pw = (PortholeWidget) gw;
    Widget child = find_child (pw);	/* ignore extra children */

    if (child) {
	if (!XtIsRealized (gw)) {
	    XtWidgetGeometry geom, retgeom;

	    geom.request_mode = 0;
	    if (pw->core.width == 0) {
		geom.width = child->core.width;
		geom.request_mode |= CWWidth;
	    }
	    if (pw->core.height == 0) {
		geom.height = child->core.height;
		geom.request_mode |= CWHeight;
	    }
	    if (geom.request_mode &&
		XtMakeGeometryRequest (gw, &geom, &retgeom) ==
		XtGeometryAlmost) {
	        (void) XtMakeGeometryRequest (gw, &retgeom, NULL);
	    }
	}
	
	XtResizeWidget (child, Max (child->core.width, pw->core.width),
			Max (child->core.height, pw->core.height), 0);

	SendReport (pw, (unsigned int) XawPRAll);
    }
}
