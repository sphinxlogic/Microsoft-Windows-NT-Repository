/* $XFree86: mit/lib/Xaw/Viewport.c,v 1.3 1993/03/21 05:10:55 dawes Exp $ */
/* $XConsortium: Viewport.c,v 1.68 91/07/24 18:56:11 converse Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/ViewportP.h>

static void ScrollUpDownProc(), ThumbProc();
static Boolean GetGeometry();

static void ComputeWithForceBars();

#define offset(field) XtOffsetOf(ViewportRec, viewport.field)
static XtResource resources[] = {
    {XtNforceBars, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(forcebars), XtRImmediate, (XtPointer)False},
    {XtNallowHoriz, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(allowhoriz), XtRImmediate, (XtPointer)False},
    {XtNallowVert, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(allowvert), XtRImmediate, (XtPointer)False},
    {XtNuseBottom, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(usebottom), XtRImmediate, (XtPointer)False},
    {XtNuseRight, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(useright), XtRImmediate, (XtPointer)False},
    {XtNreportCallback, XtCReportCallback, XtRCallback, sizeof(XtPointer),
	 offset(report_callbacks), XtRImmediate, (XtPointer) NULL},
};
#undef offset

static void Initialize(), ConstraintInitialize(),
    Realize(), Resize(), ChangeManaged();
static Boolean SetValues(), Layout();
static XtGeometryResult GeometryManager(), PreferredGeometry();

#ifndef SVR3SHLIB
#define superclass	(&formClassRec)
#else
#define superclass	((FormWidgetClass)formWidgetClass)
#endif
ViewportClassRec viewportClassRec = {
  { /* core_class fields */
#ifndef SVR3SHLIB
    /* superclass	  */	(WidgetClass) superclass,
#else
    /* superclass	  */	NULL,
#endif
    /* class_name	  */	"Viewport",
    /* widget_size	  */	sizeof(ViewportRec),
    /* class_initialize	  */	XawInitializeWidgetSet,
    /* class_part_init    */    NULL,
    /* class_inited	  */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	Resize,
    /* expose		  */	XtInheritExpose,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table    	  */	NULL,
    /* query_geometry     */    PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager	  */	GeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },
  { /* constraint_class fields */
    /* subresourses	  */	NULL,
    /* subresource_count  */	0,
    /* constraint_size	  */	sizeof(ViewportConstraintsRec),
    /* initialize	  */	ConstraintInitialize,
    /* destroy		  */	NULL,
    /* set_values	  */	NULL,
    /* extension          */	NULL
  },
  { /* form_class fields */
    /* layout		  */	Layout
  },
  { /* viewport_class fields */
    /* empty		  */	0
  }
};


#ifndef SVR3SHLIB
WidgetClass viewportWidgetClass = (WidgetClass)&viewportClassRec;
#else
extern WidgetClass viewportWidgetClass;
#endif

static Widget CreateScrollbar(w, horizontal)
    ViewportWidget w;
    Boolean horizontal;
{
    Widget clip = w->viewport.clip;
    ViewportConstraints constraints =
	(ViewportConstraints)clip->core.constraints;
    static Arg barArgs[] = {
	{XtNorientation, (XtArgVal)NULL},
	{XtNlength, (XtArgVal)NULL},
	{XtNleft, (XtArgVal)NULL},
	{XtNright, (XtArgVal)NULL},
	{XtNtop, (XtArgVal)NULL},
	{XtNbottom, (XtArgVal)NULL},
	{XtNmappedWhenManaged, (XtArgVal)False},
    };
    Widget bar;

    XtSetArg(barArgs[0], XtNorientation,
	      horizontal ? XtorientHorizontal : XtorientVertical );
    XtSetArg(barArgs[1], XtNlength,
	     horizontal ? clip->core.width : clip->core.height);
    XtSetArg(barArgs[2], XtNleft,
	     (!horizontal && w->viewport.useright) ? XtChainRight : XtChainLeft);
    XtSetArg(barArgs[3], XtNright,
	     (!horizontal && !w->viewport.useright) ? XtChainLeft : XtChainRight);
    XtSetArg(barArgs[4], XtNtop,
	     (horizontal && w->viewport.usebottom) ? XtChainBottom: XtChainTop);
    XtSetArg(barArgs[5], XtNbottom,
	     (horizontal && !w->viewport.usebottom) ? XtChainTop: XtChainBottom);

    bar = XtCreateWidget( (horizontal ? "horizontal" : "vertical"),
			  scrollbarWidgetClass, (Widget)w,
			  barArgs, XtNumber(barArgs) );
    XtAddCallback( bar, XtNscrollProc, ScrollUpDownProc, (XtPointer)w );
    XtAddCallback( bar, XtNjumpProc, ThumbProc, (XtPointer)w );

    if (horizontal) {
	w->viewport.horiz_bar = bar;
	constraints->form.vert_base = bar;
    }
    else {
	w->viewport.vert_bar = bar;
	constraints->form.horiz_base = bar;
    }

    XtManageChild( bar );

    return bar;
}

/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
    ViewportWidget w = (ViewportWidget)new;
    static Arg clip_args[8];
    Cardinal num_args;
    Widget h_bar, v_bar;
    Dimension clip_height, clip_width;

    w->form.default_spacing = 0;  /* Reset the default spacing to 0 pixels. */


/* 
 * Initialize all widget pointers to NULL.
 */

    w->viewport.child = (Widget) NULL;
    w->viewport.horiz_bar = w->viewport.vert_bar = (Widget)NULL;

/* 
 * Create Clip Widget.
 */

    num_args = 0;
    XtSetArg(clip_args[num_args], XtNbackgroundPixmap, None); num_args++;
    XtSetArg(clip_args[num_args], XtNborderWidth, 0); num_args++;
    XtSetArg(clip_args[num_args], XtNleft, XtChainLeft); num_args++;
    XtSetArg(clip_args[num_args], XtNright, XtChainRight); num_args++;
    XtSetArg(clip_args[num_args], XtNtop, XtChainTop); num_args++;
    XtSetArg(clip_args[num_args], XtNbottom, XtChainBottom); num_args++;
    XtSetArg(clip_args[num_args], XtNwidth, w->core.width); num_args++;
    XtSetArg(clip_args[num_args], XtNheight, w->core.height); num_args++;

    w->viewport.clip = XtCreateManagedWidget("clip", widgetClass, new,
					     clip_args, num_args);

    if (!w->viewport.forcebars) 
        return;		 /* If we are not forcing the bars then we are done. */

    if (w->viewport.allowhoriz) 
      (void) CreateScrollbar(w, True);
    if (w->viewport.allowvert) 
      (void) CreateScrollbar(w, False);

    h_bar = w->viewport.horiz_bar;
    v_bar = w->viewport.vert_bar;

/*
 * Set the clip widget to the correct height.
 */

    clip_width = w->core.width;
    clip_height = w->core.height;

    if ( (h_bar != NULL) &&
	 ((int)w->core.width >
	  (int)(h_bar->core.width + h_bar->core.border_width)) )
        clip_width -= h_bar->core.width + h_bar->core.border_width;
    
    if ( (v_bar != NULL) &&
	 ((int)w->core.height >
	  (int)(v_bar->core.height + v_bar->core.border_width)) )
        clip_height -= v_bar->core.height + v_bar->core.border_width;

    num_args = 0;
    XtSetArg(clip_args[num_args], XtNwidth, clip_width); num_args++;
    XtSetArg(clip_args[num_args], XtNheight, clip_height); num_args++;
    XtSetValues(w->viewport.clip, clip_args, num_args);
}

/* ARGSUSED */
static void ConstraintInitialize(request, new)
    Widget request, new;
{
    ((ViewportConstraints)new->core.constraints)->viewport.reparented = False;
}

static void Realize(widget, value_mask, attributes)
    Widget widget;
    XtValueMask *value_mask;
    XSetWindowAttributes *attributes;
{
    ViewportWidget w = (ViewportWidget)widget;
    register Widget child = w->viewport.child;
    register Widget clip = w->viewport.clip;

    *value_mask |= CWBitGravity;
    attributes->bit_gravity = NorthWestGravity;
    (*superclass->core_class.realize)(widget, value_mask, attributes);

    (*w->core.widget_class->core_class.resize)(widget);	/* turn on bars */

    if (child != (Widget)NULL) {
	XtMoveWidget( child, (Position)0, (Position)0 );
	XtRealizeWidget( clip );
	XtRealizeWidget( child );
	XReparentWindow( XtDisplay(w), XtWindow(child), XtWindow(clip),
			 (Position)0, (Position)0 );
	XtMapWidget( child );
    }
}

/* ARGSUSED */
static Boolean SetValues(current, request, new)
    Widget current, request, new;
{
    ViewportWidget w = (ViewportWidget)new;
    ViewportWidget cw = (ViewportWidget)current;

    if ( (w->viewport.forcebars != cw->viewport.forcebars) ||
	 (w->viewport.allowvert != cw->viewport.allowvert) ||
	 (w->viewport.allowhoriz != cw->viewport.allowhoriz) ||
	 (w->viewport.useright != cw->viewport.useright) ||
	 (w->viewport.usebottom != cw->viewport.usebottom) ) 
    {
	(*w->core.widget_class->core_class.resize)(new); /* Recompute layout.*/
    }

    return False;
}


static void ChangeManaged(widget)
    Widget widget;
{
    ViewportWidget w = (ViewportWidget)widget;
    register int num_children = w->composite.num_children;
    register Widget child, *childP;
    register int i;

    child = (Widget)NULL;
    for (childP=w->composite.children, i=0; i < num_children; childP++, i++) {
	if (XtIsManaged(*childP)
	    && *childP != w->viewport.clip
	    && *childP != w->viewport.horiz_bar
	    && *childP != w->viewport.vert_bar)
	{
	    child = *childP;
	    break;
	}
    }

    if (child != w->viewport.child) {
	w->viewport.child = child;
	if (child != (Widget)NULL) {
	    XtResizeWidget( child, child->core.width,
			    child->core.height, (Dimension)0 );
	    if (XtIsRealized(widget)) {
		ViewportConstraints constraints =
		    (ViewportConstraints)child->core.constraints;
		if (!XtIsRealized(child)) {
		    Window window = XtWindow(w);
		    XtMoveWidget( child, (Position)0, (Position)0 );
#ifdef notdef
		    /* this is dirty, but it saves the following code: */
		    XtRealizeWidget( child );
		    XReparentWindow( XtDisplay(w), XtWindow(child),
				     XtWindow(w->viewport.clip),
				     (Position)0, (Position)0 );
		    if (child->core.mapped_when_managed)
			XtMapWidget( child );
#else 
		    w->core.window = XtWindow(w->viewport.clip);
		    XtRealizeWidget( child );
		    w->core.window = window;
#endif /* notdef */
		    constraints->viewport.reparented = True;
		}
		else if (!constraints->viewport.reparented) {
		    XReparentWindow( XtDisplay(w), XtWindow(child),
				     XtWindow(w->viewport.clip),
				     (Position)0, (Position)0 );
		    constraints->viewport.reparented = True;
		    if (child->core.mapped_when_managed)
			XtMapWidget( child );
		}
	    }
	    GetGeometry( widget, child->core.width, child->core.height );
	    (*((ViewportWidgetClass)w->core.widget_class)->form_class.layout)
		( (FormWidget)w, w->core.width, w->core.height );
	    /* %%% do we need to hide this child from Form?  */
	}
    }

#ifdef notdef
    (*superclass->composite_class.change_managed)( widget );
#endif
}


static void SetBar(w, top, length, total)
    Widget w;
    Position top;
    Dimension length, total;
{
    XawScrollbarSetThumb(w, (float)top/(float)total,
			 (float)length/(float)total);
}

static void RedrawThumbs(w)
  ViewportWidget w;
{
    register Widget child = w->viewport.child;
    register Widget clip = w->viewport.clip;

    if (w->viewport.horiz_bar != (Widget)NULL)
	SetBar( w->viewport.horiz_bar, -(child->core.x),
	        clip->core.width, child->core.width );

    if (w->viewport.vert_bar != (Widget)NULL)
	SetBar( w->viewport.vert_bar, -(child->core.y),
	        clip->core.height, child->core.height );
}



static void SendReport (w, changed)
    ViewportWidget w;
    unsigned int changed;
{
    XawPannerReport rep;

    if (w->viewport.report_callbacks) {
	register Widget child = w->viewport.child;
	register Widget clip = w->viewport.clip;

	rep.changed = changed;
	rep.slider_x = -child->core.x;	/* child is canvas */
	rep.slider_y = -child->core.y;	/* clip is slider */
	rep.slider_width = clip->core.width;
	rep.slider_height = clip->core.height;
	rep.canvas_width = child->core.width;
	rep.canvas_height = child->core.height;
	XtCallCallbackList ((Widget) w, w->viewport.report_callbacks,
			    (XtPointer) &rep);
    }
}


static void MoveChild(w, x, y)
    ViewportWidget w;
    Position x, y;
{
    register Widget child = w->viewport.child;
    register Widget clip = w->viewport.clip;

    /* make sure we never move past right/bottom borders */
    if (-x + (int)clip->core.width > (int)child->core.width)
	x = -(child->core.width - clip->core.width);

    if (-y + (int)clip->core.height > (int)child->core.height)
	y = -(child->core.height - clip->core.height);

    /* make sure we never move past left/top borders */
    if (x >= 0) x = 0;
    if (y >= 0) y = 0;

    XtMoveWidget(child, x, y);
    SendReport (w, (XawPRSliderX | XawPRSliderY));

    RedrawThumbs(w);
}


static void ComputeLayout(widget, query, destroy_scrollbars)
    Widget widget;		/* Viewport */
    Boolean query;		/* query child's preferred geom? */
    Boolean destroy_scrollbars;	/* destroy un-needed scrollbars? */
{
    ViewportWidget w = (ViewportWidget)widget;
    register Widget child = w->viewport.child;
    register Widget clip = w->viewport.clip;
    ViewportConstraints constraints
	= (ViewportConstraints)clip->core.constraints;
    Boolean needshoriz, needsvert;
    int clip_width, clip_height;
    XtWidgetGeometry intended;

    if (child == (Widget) NULL) return;

    clip_width = w->core.width;
    clip_height = w->core.height;
    intended.request_mode = CWBorderWidth;
    intended.border_width = 0;

    if (w->viewport.forcebars) {
        needsvert = w->viewport.allowvert;
        needshoriz = w->viewport.allowhoriz;
        ComputeWithForceBars(widget, query, &intended, 
			     &clip_width, &clip_height);
    }
    else {
        Dimension prev_width, prev_height;
	XtGeometryMask prev_mode;
	XtWidgetGeometry preferred;

	needshoriz = needsvert = False;

	/*
	 * intended.{width,height} caches the eventual child dimensions,
	 * but we don't set the mode bits until after we decide that the
	 * child's preferences are not acceptable.
	 */

	if (!w->viewport.allowhoriz) 
	    intended.request_mode |= CWWidth;

	if ((int)child->core.width < clip_width) 
	    intended.width = clip_width;
	else
	    intended.width = child->core.width;

	if ((int)child->core.height < clip_height) 
	    intended.height = clip_height;
	else
	    intended.height = child->core.height;

	if (!w->viewport.allowvert) 
	    intended.request_mode |= CWHeight;

	if (!query) {
	    preferred.width = child->core.width;
	    preferred.height = child->core.height;
	}
	do { /* while intended != prev  */

	    if (query) {
	        (void) XtQueryGeometry( child, &intended, &preferred );
		if ( !(preferred.request_mode & CWWidth) )
		    preferred.width = intended.width;
		if ( !(preferred.request_mode & CWHeight) )
		    preferred.height = intended.height;
	    }
	    prev_width = intended.width;
	    prev_height = intended.height;
	    prev_mode = intended.request_mode;
	    /*
	     * note that having once decided to turn on either bar
	     * we'll not change our mind until we're next resized,
	     * thus avoiding potential oscillations.
	     */
#define CheckHoriz()							  \
	    if ( w->viewport.allowhoriz &&				  \
		 (int)preferred.width > clip_width) { 			  \
	        if (!needshoriz) {					  \
		    Widget bar;					          \
		    needshoriz = True;				          \
		    if ((bar = w->viewport.horiz_bar) == (Widget)NULL)    \
		        bar = CreateScrollbar(w, True);		          \
		    clip_height -= bar->core.height +		          \
			           bar->core.border_width;		  \
		    if (clip_height < 1) clip_height = 1;		  \
		}							  \
	        intended.width = preferred.width;			  \
	    }
/*enddef*/
	    CheckHoriz();
	    if (w->viewport.allowvert && (int)preferred.height > clip_height) {
	        if (!needsvert) {
		    Widget bar;
		    needsvert = True;
		    if ((bar = w->viewport.vert_bar) == (Widget)NULL)
		        bar = CreateScrollbar(w, False);
		    clip_width -= bar->core.width + bar->core.border_width;
		    if (clip_width < 1) clip_width = 1;
		    CheckHoriz();
		}
		intended.height = preferred.height;
	    }
	    if ( !w->viewport.allowhoriz ||
		 (int)preferred.width < clip_width) {
	        intended.width = clip_width;
		intended.request_mode |= CWWidth;
	    }
	    if ( !w->viewport.allowvert ||
		 (int)preferred.height < clip_height) {
	        intended.height = clip_height;
		intended.request_mode |= CWHeight;
	    }
	} while (intended.request_mode != prev_mode
		 || (intended.request_mode & CWWidth
		     && intended.width != prev_width)
		 || (intended.request_mode & CWHeight
		     && intended.height != prev_height));
    }

    if (XtIsRealized(clip))
	XRaiseWindow( XtDisplay(clip), XtWindow(clip) );

    XtMoveWidget( clip,
		  needsvert ? (w->viewport.useright ? 0 :
			       w->viewport.vert_bar->core.width +
			       w->viewport.vert_bar->core.border_width) : 0,
		  needshoriz ? (w->viewport.usebottom ? 0 :
				w->viewport.horiz_bar->core.height +
			        w->viewport.horiz_bar->core.border_width) : 0);
    XtResizeWidget( clip, (Dimension)clip_width,
		    (Dimension)clip_height, (Dimension)0 );
	
    if (w->viewport.horiz_bar != (Widget)NULL) {
	register Widget bar = w->viewport.horiz_bar;
	if (!needshoriz) {
	    constraints->form.vert_base = (Widget)NULL;
	    if (destroy_scrollbars) {
		XtDestroyWidget( bar );
		w->viewport.horiz_bar = (Widget)NULL;
	    }
	}
	else {
	    register int bw = bar->core.border_width;
	    XtResizeWidget( bar, clip_width, bar->core.height, bw );
	    XtMoveWidget( bar,
			  ((needsvert && !w->viewport.useright)
			   ? w->viewport.vert_bar->core.width
			   : -bw),
			  (w->viewport.usebottom
			    ? w->core.height - bar->core.height - bw
			    : -bw) );
	    XtSetMappedWhenManaged( bar, True );
	}
    }

    if (w->viewport.vert_bar != (Widget)NULL) {
	register Widget bar = w->viewport.vert_bar;
	if (!needsvert) {
	    constraints->form.horiz_base = (Widget)NULL;
	    if (destroy_scrollbars) {
		XtDestroyWidget( bar );
		w->viewport.vert_bar = (Widget)NULL;
	    }
	}
	else {
	    register int bw = bar->core.border_width;
	    XtResizeWidget( bar, bar->core.width, clip_height, bw );
	    XtMoveWidget( bar,
			  (w->viewport.useright
			   ? w->core.width - bar->core.width - bw 
			   : -bw),
			  ((needshoriz && !w->viewport.usebottom)
			    ? w->viewport.horiz_bar->core.height
			    : -bw) );
	    XtSetMappedWhenManaged( bar, True );
	}
    }

    if (child != (Widget)NULL) {
	XtResizeWidget( child, (Dimension)intended.width,
		        (Dimension)intended.height, (Dimension)0 );
	MoveChild(w,
		  needshoriz ? child->core.x : 0,
		  needsvert ? child->core.y : 0);
    }

    SendReport (w, XawPRAll);
}

/*      Function Name: ComputeWithForceBars
 *      Description: Computes the layout give forcebars is set.
 *      Arguments: widget - the viewport widget.
 *                 query - whether or not to query the child.
 *                 intended - the cache of the childs height is
 *                            stored here ( USED AND RETURNED ).
 *                 clip_width, clip_height - size of clip window.
 *                                           (USED AND RETURNED ).
 *      Returns: none.
 */

static void
ComputeWithForceBars(widget, query, intended, clip_width, clip_height)
Widget widget;
Boolean query;
XtWidgetGeometry * intended;
int *clip_width, *clip_height;
{
    ViewportWidget w = (ViewportWidget)widget;
    register Widget child = w->viewport.child;
    XtWidgetGeometry preferred;

/*
 * If forcebars then needs = allows = has.
 * Thus if needsvert is set it MUST have a scrollbar.
 */

    if (w->viewport.allowvert) {
	if (w->viewport.vert_bar == NULL) 
	    w->viewport.vert_bar = CreateScrollbar(w, False);

	*clip_width -= w->viewport.vert_bar->core.width +
		       w->viewport.vert_bar->core.border_width;
    }

    if (w->viewport.allowhoriz) {
	if (w->viewport.horiz_bar == NULL) 
	    w->viewport.horiz_bar = CreateScrollbar(w, True);

        *clip_height -= w->viewport.horiz_bar->core.height +
		       w->viewport.horiz_bar->core.border_width;
    }

    AssignMax( *clip_width, 1 );
    AssignMax( *clip_height, 1 );

    if (!w->viewport.allowvert) {
        intended->height = *clip_height;
        intended->request_mode = CWHeight;
    }
    if (!w->viewport.allowhoriz) {
        intended->width = *clip_width;
        intended->request_mode = CWWidth;
    }

    if ( query ) {
        if ( (w->viewport.allowvert || w->viewport.allowhoriz) ) { 
	    XtQueryGeometry( child, intended, &preferred );
	  
	    if ( !(intended->request_mode & CWWidth) )
	        if ( preferred.request_mode & CWWidth )
		    intended->width = preferred.width;
		else
		    intended->width = child->core.width;

	    if ( !(intended->request_mode & CWHeight) )
	        if ( preferred.request_mode & CWHeight )
		    intended->height = preferred.height;
		else
		    intended->height = child->core.height;
	}
    }
    else {
        if (w->viewport.allowvert)
	    intended->height = child->core.height;
	if (w->viewport.allowhoriz)
	    intended->width = child->core.width;
    }

    if (*clip_width > (int)intended->width)
	intended->width = *clip_width;
    if (*clip_height > (int)intended->height)
	intended->height = *clip_height;
}

static void Resize(widget)
    Widget widget;
{
    ComputeLayout( widget, /*query=*/True, /*destroy=*/True );
}


/* ARGSUSED */
static Boolean Layout(w, width, height)
    FormWidget w;
    Dimension width, height;
{
    ComputeLayout( (Widget)w, /*query=*/True, /*destroy=*/True );
    w->form.preferred_width = w->core.width;
    w->form.preferred_height = w->core.height;
    return False;
}


static void ScrollUpDownProc(widget, closure, call_data)
    Widget widget;
    XtPointer closure;
    XtPointer call_data;
{
    ViewportWidget w = (ViewportWidget)closure;
    register Widget child = w->viewport.child;
    int pix = (int)call_data;
    Position x, y;

    if (child == NULL) return;	/* no child to scroll. */

    x = child->core.x - ((widget == w->viewport.horiz_bar) ? pix : 0);
    y = child->core.y - ((widget == w->viewport.vert_bar) ? pix : 0);
    MoveChild(w, x, y);
}


/* ARGSUSED */
static void ThumbProc(widget, closure, percent)
    Widget widget;
    XtPointer closure;
    float *percent;
{
    ViewportWidget w = (ViewportWidget)closure;
    register Widget child = w->viewport.child;
    Position x, y;

    if (child == NULL) return;	/* no child to scroll. */

    if (widget == w->viewport.horiz_bar)
#ifdef macII				/* bug in the macII A/UX 1.0 cc */
	x = (int)(-*percent * child->core.width);
#else /* else not macII */
	x = -(int)(*percent * child->core.width);
#endif /* macII */
    else
	x = child->core.x;

    if (widget == w->viewport.vert_bar)
#ifdef macII				/* bug in the macII A/UX 1.0 cc */
	y = (int)(-*percent * child->core.height);
#else /* else not macII */
	y = -(int)(*percent * child->core.height);
#endif /* macII */
    else
	y = child->core.y;

    MoveChild(w, x, y);
}

static XtGeometryResult
TestSmaller(w, request, reply_return)
     ViewportWidget w; XtWidgetGeometry *request, *reply_return;
{
  if (request->width < w->core.width || request->height < w->core.height)
    return XtMakeGeometryRequest((Widget)w, request, reply_return);
  else
    return XtGeometryYes;  
}

static XtGeometryResult
GeometryRequestPlusScrollbar(w, horizontal, request, reply_return)
     Boolean horizontal;
     ViewportWidget w; 
     XtWidgetGeometry *request, *reply_return;
{
  Widget sb;
  XtWidgetGeometry plusScrollbars;
  plusScrollbars = *request;
  if ((sb = w->viewport.horiz_bar) == (Widget)NULL)
    sb = CreateScrollbar( w, horizontal);
  request->width += sb->core.width;
  request->height += sb->core.height;
  XtDestroyWidget(sb);
  return XtMakeGeometryRequest((Widget) w, &plusScrollbars, reply_return);
 }

#define WidthChange() (request->width != w->core.width)
#define HeightChange() (request->height != w->core.height)

static XtGeometryResult 
QueryGeometry(w, request, reply_return)
     ViewportWidget w; XtWidgetGeometry *request, *reply_return;
{	
  if (w->viewport.allowhoriz && w->viewport.allowvert) 
    return TestSmaller(w, request, reply_return);

  else if (w->viewport.allowhoriz && !w->viewport.allowvert) {
    if (WidthChange() && !HeightChange())
      return TestSmaller(w, request, reply_return);
    else if (!WidthChange() && HeightChange())
      return XtMakeGeometryRequest((Widget) w, request, reply_return);
    else if (WidthChange() && HeightChange()) /* hard part */
      return GeometryRequestPlusScrollbar(w, True, request, reply_return);
    else /* !WidthChange() && !HeightChange() */
      return XtGeometryYes;
  }
  else if (!w->viewport.allowhoriz && w->viewport.allowvert) {
    if (!WidthChange() && HeightChange())
      return TestSmaller(w, request, reply_return);
    else if (WidthChange() && !HeightChange())
      return XtMakeGeometryRequest((Widget)w, request, reply_return);
    else if (WidthChange() && HeightChange()) /* hard part */
      return GeometryRequestPlusScrollbar(w, False, request, reply_return);
    else /* !WidthChange() && !HeightChange() */
      return XtGeometryYes;
  }      
  else /* (!w->viewport.allowhoriz && !w->viewport.allowvert) */
    return XtMakeGeometryRequest((Widget) w, request, reply_return);
}

#undef WidthChange
#undef HeightChange

static XtGeometryResult GeometryManager(child, request, reply)
    Widget child;
    XtWidgetGeometry *request, *reply;
{
    ViewportWidget w = (ViewportWidget)child->core.parent;
    Boolean rWidth = (Boolean)(request->request_mode & CWWidth);
    Boolean rHeight = (Boolean)(request->request_mode & CWHeight);
    XtWidgetGeometry allowed;
    XtGeometryResult result;
    Boolean reconfigured;
    Boolean child_changed_size;
    Dimension height_remaining;

    if (request->request_mode & XtCWQueryOnly)
      return QueryGeometry(w, request, reply);

    if (child != w->viewport.child
        || request->request_mode & ~(CWWidth | CWHeight
				     | CWBorderWidth)
	|| ((request->request_mode & CWBorderWidth)
	    && request->border_width > 0))
	return XtGeometryNo;

    allowed = *request;

    reconfigured = GetGeometry( (Widget)w,
			        (rWidth ? request->width : w->core.width),
			        (rHeight ? request->height : w->core.height)
			      );

    child_changed_size = ((rWidth && child->core.width != request->width) ||
			  (rHeight && child->core.height != request->height));

    height_remaining = w->core.height;
    if (rWidth && w->core.width != request->width) {
	if (w->viewport.allowhoriz && request->width > w->core.width) {
	    /* horizontal scrollbar will be needed so possibly reduce height */
	    Widget bar; 
	    if ((bar = w->viewport.horiz_bar) == (Widget)NULL)
		bar = CreateScrollbar( w, True );
	    height_remaining -= bar->core.height + bar->core.border_width;
	    reconfigured = True;
	}
	else {
	    allowed.width = w->core.width;
	}
    }
    if (rHeight && height_remaining != request->height) {
	if (w->viewport.allowvert && request->height > height_remaining) {
	    /* vertical scrollbar will be needed, so possibly reduce width */
	    if (!w->viewport.allowhoriz || request->width < w->core.width) {
		Widget bar;
		if ((bar = w->viewport.vert_bar) == (Widget)NULL)
		    bar = CreateScrollbar( w, False );
		if (!rWidth) {
		    allowed.width = w->core.width;
		    allowed.request_mode |= CWWidth;
		}
		if ( (int)allowed.width >
		     (int)(bar->core.width + bar->core.border_width) )
		    allowed.width -= bar->core.width + bar->core.border_width;
		else
		    allowed.width = 1;
		reconfigured = True;
	    }
	}
	else {
	    allowed.height = height_remaining;
	}
    }

    if (allowed.width != request->width || allowed.height != request->height) {
	*reply = allowed;
	result = XtGeometryAlmost;
    }
    else {
	if (rWidth)  child->core.width = request->width;
	if (rHeight) child->core.height = request->height;
	result = XtGeometryYes;
    }

    if (reconfigured || child_changed_size)
	ComputeLayout( (Widget)w,
		       /*query=*/ False,
		       /*destroy=*/ (result == XtGeometryYes) ? True : False );

    return result;
  }


static Boolean GetGeometry(w, width, height)
    Widget w;
    Dimension width, height;
{
    XtWidgetGeometry geometry, return_geom;
    XtGeometryResult result;

    if (width == w->core.width && height == w->core.height)
	return False;

    geometry.request_mode = CWWidth | CWHeight;
    geometry.width = width;
    geometry.height = height;

    if (XtIsRealized(w)) {
	if (((ViewportWidget)w)->viewport.allowhoriz && width > w->core.width)
	    geometry.width = w->core.width;
	if (((ViewportWidget)w)->viewport.allowvert && height > w->core.height)
	    geometry.height = w->core.height;
    } else {
	/* This is the Realize call; we'll inherit a w&h iff none currently */
	if (w->core.width != 0) {
	    if (w->core.height != 0) return False;
	    geometry.width = w->core.width;
	}
	if (w->core.height != 0) geometry.height = w->core.height;
    }

    result = XtMakeGeometryRequest(w, &geometry, &return_geom);
    if (result == XtGeometryAlmost)
	result = XtMakeGeometryRequest(w, &return_geom, NULL);

    return (result == XtGeometryYes);
}

static XtGeometryResult PreferredGeometry(w, constraints, reply)
    Widget w;
    XtWidgetGeometry *constraints, *reply;
{
    if (((ViewportWidget)w)->viewport.child != NULL)
	return XtQueryGeometry( ((ViewportWidget)w)->viewport.child,
			       constraints, reply );
    else
	return XtGeometryYes;
}


void
#if NeedFunctionPrototypes
XawViewportSetLocation (Widget gw,
#if NeedWidePrototypes
			double xoff, double yoff)
#else
			float xoff, float yoff)
#endif
#else
XawViewportSetLocation (gw, xoff, yoff)
    Widget gw;
    float  xoff,yoff;
#endif
{
    ViewportWidget w = (ViewportWidget) gw;
    register Widget child = w->viewport.child;
    Position x, y;

    if (xoff > 1.0)			/* scroll to right */
       x = child->core.width;
    else if (xoff < 0.0)		/* if the offset is < 0.0 nothing */ 
       x = child->core.x;
    else
       x = (Position) (((float) child->core.width) * xoff);

    if (yoff > 1.0) 
       y = child->core.height;
    else if (yoff < 0.0)
       y = child->core.y;
    else
       y = (Position) (((float) child->core.height) * yoff);

    MoveChild (w, -x, -y);
}

void
#if NeedFunctionPrototypes
XawViewportSetCoordinates (Widget gw,
#if NeedWidePrototypes
			   int x, int y)
#else
			   Position x, Position y)
#endif
#else
XawViewportSetCoordinates (gw, x, y)
    Widget gw;
    Position x, y;
#endif
{
    ViewportWidget w = (ViewportWidget) gw;
    Widget child = w->viewport.child;

    if (x > (int)child->core.width) 
      x = child->core.width;
    else if (x < 0)
      x = child->core.x;

    if (y > (int)child->core.height)
      y = child->core.height;
    else if (y < 0)
      y = child->core.y;

    MoveChild (w, -x, -y);
}

