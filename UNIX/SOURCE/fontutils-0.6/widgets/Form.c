/* Form.c: implementation of the Athena Form widget.  The geometry
   manager has been modified from the distributed version.

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
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Xaw/XawInit.h>

#include <X11/Xaw/FormP.h>

/* Private Definitions */

static int default_value = -99999;

#define Offset(field) XtOffset(FormWidget, form.field)
static XtResource resources[] = {
    {XtNdefaultDistance, XtCThickness, XtRInt, sizeof(int),
	Offset(default_spacing), XtRImmediate, (caddr_t)4}
};
#undef Offset

static XtEdgeType defEdge = XtRubber;

#define Offset(field) XtOffset(FormConstraints, form.field)
static XtResource formConstraintResources[] = {
    {XtNtop, XtCEdge, XtREdgeType, sizeof(XtEdgeType),
	Offset(top), XtREdgeType, (XtPointer)&defEdge},
    {XtNbottom, XtCEdge, XtREdgeType, sizeof(XtEdgeType),
	Offset(bottom), XtREdgeType, (XtPointer)&defEdge},
    {XtNleft, XtCEdge, XtREdgeType, sizeof(XtEdgeType),
	Offset(left), XtREdgeType, (XtPointer)&defEdge},
    {XtNright, XtCEdge, XtREdgeType, sizeof(XtEdgeType),
	Offset(right), XtREdgeType, (XtPointer)&defEdge},
    {XtNhorizDistance, XtCThickness, XtRInt, sizeof(int),
	Offset(dx), XtRInt, (XtPointer) &default_value},
    {XtNfromHoriz, XtCWidget, XtRWidget, sizeof(Widget),
	Offset(horiz_base), XtRWidget, (XtPointer)NULL},
    {XtNvertDistance, XtCThickness, XtRInt, sizeof(int),
	Offset(dy), XtRInt, (XtPointer) &default_value},
    {XtNfromVert, XtCWidget, XtRWidget, sizeof(Widget),
	Offset(vert_base), XtRWidget, (XtPointer)NULL},
    {XtNresizable, XtCBoolean, XtRBoolean, sizeof(Boolean),
	Offset(allow_resize), XtRImmediate, (XtPointer) FALSE},
};
#undef Offset

static void ClassInitialize(), ClassPartInitialize(), Initialize(), Resize();
static void ConstraintInitialize();
static Boolean SetValues(), ConstraintSetValues();
static XtGeometryResult GeometryManager(), PreferredGeometry();
static void ChangeManaged();
static Boolean Layout();

FormClassRec formClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "Form",
    /* widget_size        */    sizeof(FormRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresourses       */   formConstraintResources,
    /* subresource_count  */   XtNumber(formConstraintResources),
    /* constraint_size    */   sizeof(FormConstraintsRec),
    /* initialize         */   ConstraintInitialize,
    /* destroy            */   NULL,
    /* set_values         */   ConstraintSetValues,
    /* extension          */   NULL
  },
  { /* form_class fields */
    /* layout             */   Layout
  }
};

WidgetClass formWidgetClass = (WidgetClass)&formClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/


static XrmQuark	XtQChainLeft, XtQChainRight, XtQChainTop,
		XtQChainBottom, XtQRubber;

#define	done(address, type) \
	{ toVal->size = sizeof(type); \
	  toVal->addr = (caddr_t) address; \
	  return; \
	}

/* ARGSUSED */
static void _CvtStringToEdgeType(args, num_args, fromVal, toVal)
    XrmValuePtr args;		/* unused */
    Cardinal    *num_args;      /* unused */
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XtEdgeType edgeType;
    XrmQuark q;
    char lowerName[1000];

    XmuCopyISOLatin1Lowered (lowerName, (char*)fromVal->addr);
    q = XrmStringToQuark(lowerName);
    if (q == XtQChainLeft) {
	edgeType = XtChainLeft;
	done(&edgeType, XtEdgeType);
    }
    if (q == XtQChainRight) {
	edgeType = XtChainRight;
	done(&edgeType, XtEdgeType);
    }
    if (q == XtQChainTop) {
	edgeType = XtChainTop;
	done(&edgeType, XtEdgeType);
    }
    if (q == XtQChainBottom) {
	edgeType = XtChainBottom;
	done(&edgeType, XtEdgeType);
    }
    if (q == XtQRubber) {
	edgeType = XtRubber;
	done(&edgeType, XtEdgeType);
    }

    XtStringConversionWarning(fromVal->addr, "edgeType");
    toVal->addr = NULL;
    toVal->size = 0;
}

static void ClassInitialize()
{
    static XtConvertArgRec parentCvtArgs[] = {
	{XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent), sizeof(Widget)}
    };
    XawInitializeWidgetSet();
    XtQChainLeft   = XrmStringToQuark("chainleft");
    XtQChainRight  = XrmStringToQuark("chainright");
    XtQChainTop    = XrmStringToQuark("chaintop");
    XtQChainBottom = XrmStringToQuark("chainbottom");
    XtQRubber      = XrmStringToQuark("rubber");

    XtAddConverter( XtRString, XtREdgeType, _CvtStringToEdgeType, NULL, 0 );
    XtAddConverter( XtRString, XtRWidget, XmuCvtStringToWidget,
		    parentCvtArgs, XtNumber(parentCvtArgs) );
}

static void ClassPartInitialize(class)
    WidgetClass class;
{
    register FormWidgetClass c = (FormWidgetClass)class;

    if (c->form_class.layout == XtInheritLayout)
	c->form_class.layout = Layout;
}


/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
    FormWidget fw = (FormWidget)new;

    fw->form.old_width = fw->core.width;
    fw->form.old_height = fw->core.height;
    fw->form.no_refigure = False;
    fw->form.needs_relayout = False;
    fw->form.resize_in_layout = True;
}


static void RefigureLocations(w)
    FormWidget w;
{
    if (w->form.no_refigure) {
	w->form.needs_relayout = True;
    }
    else {
	(*((FormWidgetClass)w->core.widget_class)->form_class.layout)
	    ( w, w->core.width, w->core.height );
	w->form.needs_relayout = False;
    }
}

/* ARGSUSED */
static Boolean Layout(fw, width, height)
    FormWidget fw;
    Dimension width, height;
{
    int num_children = fw->composite.num_children;
    WidgetList children = fw->composite.children;
    Widget *childP;
    Position maxx, maxy;
    static void LayoutChild();
    Boolean ret_val;

    for (childP = children; childP - children < num_children; childP++) {
	FormConstraints form = (FormConstraints)(*childP)->core.constraints;
	form->form.layout_state = LayoutPending;
    }

    maxx = maxy = 1;

#define CHILD_BORDER_WIDTH(childP) ((*childP)->core.border_width << 1)
#define CHILD_RIGHT_EDGE(childP) ((*childP)->core.x \
    + (*childP)->core.width + CHILD_BORDER_WIDTH (childP))
#define CHILD_BOTTOM_EDGE(childp) ((*childP)->core.y \
    + (*childP)->core.height + CHILD_BORDER_WIDTH (childP))

    for (childP = children; childP - children < num_children; childP++) {
	if (XtIsManaged(*childP)) {
	    Position x, y;
	    LayoutChild(*childP);
	    x = CHILD_RIGHT_EDGE (childP);
	    y = CHILD_BOTTOM_EDGE (childP);
	    if (maxx < x) maxx = x;
	    if (maxy < y) maxy = y;
	}
    }

    fw->form.preferred_width = (maxx += fw->form.default_spacing);
    fw->form.preferred_height = (maxy += fw->form.default_spacing);

    if (fw->form.resize_in_layout
	&& ((Dimension) maxx != fw->core.width
            || (Dimension) maxy != fw->core.height)) {
	XtGeometryResult result;
	result = XtMakeResizeRequest( (Widget) fw,
                                      (Dimension)maxx, (Dimension)maxy,
				      (Dimension*)&maxx, (Dimension*)&maxy );
	if (result == XtGeometryAlmost)
	    result = XtMakeResizeRequest( (Widget) fw,
                                          (Dimension)maxx, (Dimension)maxy,
					  NULL, NULL );
        else if (result == XtGeometryNo) {
            /* Resize the children, since at least one of them doesn't
               fit within our current borders.  */
            for (childP = children; childP - children < num_children;
                 childP++) {
                if (XtIsManaged(*childP)) {
                    Dimension w
                      = CHILD_RIGHT_EDGE (childP) > fw->core.width
                        ? fw->core.width - (*childP)->core.x
                          - CHILD_BORDER_WIDTH (childP)
                        : (*childP)->core.width;
                    Dimension h
                      = CHILD_BOTTOM_EDGE (childP) > fw->core.height
                        ? fw->core.height - (*childP)->core.y
                          - CHILD_BORDER_WIDTH (childP)
                        : (*childP)->core.height;
                    XtResizeWidget(*childP, w, h, (*childP)->core.border_width);
                }
            }
        }
        
	fw->form.old_width  = fw->core.width;
	fw->form.old_height = fw->core.height;
	ret_val = (result == XtGeometryYes);
    } else ret_val = False;

    return ret_val;
}


static void LayoutChild(w)
    Widget w;
{
    FormConstraints form = (FormConstraints)w->core.constraints;
    Position x, y;
    Widget ref;

    switch (form->form.layout_state) {

      case LayoutPending:
	form->form.layout_state = LayoutInProgress;
	break;

      case LayoutDone:
	return;

      case LayoutInProgress:
	{
	String subs[2];
	Cardinal num_subs = 2;
	subs[0] = w->core.name;
	subs[1] = w->core.parent->core.name;
	XtAppWarningMsg(XtWidgetToApplicationContext(w),
			"constraintLoop","xawFormLayout","XawToolkitError",
   "constraint loop detected while laying out child '%s' in FormWidget '%s'",
			subs, &num_subs);
	return;
	}
    }
    x = form->form.dx;
    y = form->form.dy;
    if ((ref = form->form.horiz_base) != (Widget)NULL) {
	LayoutChild(ref);
	x += ref->core.x + ref->core.width + (ref->core.border_width << 1);
    }
    if ((ref = form->form.vert_base) != (Widget)NULL) {
	LayoutChild(ref);
	y += ref->core.y + ref->core.height + (ref->core.border_width << 1);
    }
    XtMoveWidget( w, x, y );
    form->form.layout_state = LayoutDone;
}


static Position TransformCoord(loc, old, new, type)
    register Position loc;
    Dimension old, new;
    XtEdgeType type;
{
    if (type == XtRubber) {
        if ( ((int) old) > 0)
	    loc = (loc * new) / old;
    }
    else if (type == XtChainBottom || type == XtChainRight)
      loc += (Position)new - (Position)old;

    /* I don't see any problem with returning values less than zero. */

    return (loc);
}


static void Resize(w)
    Widget w;
{
    FormWidget fw = (FormWidget)w;
    WidgetList children = fw->composite.children;
    int num_children = fw->composite.num_children;
    Widget *childP;
    Position x, y;
    Dimension width, height;

    for (childP = children; childP - children < num_children; childP++) {
	FormConstraints form = (FormConstraints)(*childP)->core.constraints;
	if (!XtIsManaged(*childP)) continue;
	x = TransformCoord( (*childP)->core.x, fw->form.old_width,
			    fw->core.width, form->form.left );
	y = TransformCoord( (*childP)->core.y, fw->form.old_height,
			    fw->core.height, form->form.top );

	form->form.virtual_width =
	  TransformCoord((Position)((*childP)->core.x
				    + form->form.virtual_width
				    + 2 * (*childP)->core.border_width),
			 fw->form.old_width, fw->core.width,
			 form->form.right )
	    - (x + 2 * (*childP)->core.border_width);

	form->form.virtual_height =
	  TransformCoord((Position)((*childP)->core.y
				    + form->form.virtual_height
				    + 2 * (*childP)->core.border_width),
			 fw->form.old_height, fw->core.height,
			 form->form.bottom ) 
	    - ( y + 2 * (*childP)->core.border_width);
	
	width = (Dimension) 
	       (form->form.virtual_width < 1) ? 1 : form->form.virtual_width;
	height = (Dimension)
	       (form->form.virtual_height < 1) ? 1 : form->form.virtual_height;

	XtConfigureWidget( *childP, x, y, (Dimension)width, (Dimension)height,
			  (*childP)->core.border_width );
    }

    fw->form.old_width = fw->core.width;
    fw->form.old_height = fw->core.height;
}


/* ARGSUSED */
static XtGeometryResult GeometryManager(w, request, reply)
    Widget w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;	/* RETURN */
{
    XtWidgetGeometry allowed, parent_request;
    Boolean parent_no = False, parent_almost = False;
    Boolean querying = request->request_mode & XtCWQueryOnly;
    Boolean requesting_width = request->request_mode & CWWidth;
    Boolean requesting_height = request->request_mode & CWHeight;
    FormWidget fw = (FormWidget) w->core.parent;
    FormConstraints fc = (FormConstraints)w->core.constraints;

    /* Only allow width and height changes (unless just querying).  */
    if ((request->request_mode & ~(XtCWQueryOnly | CWWidth | CWHeight))
        || !fc->form.allow_resize)
	return XtGeometryNo;

    /* If the size of the child is the same as it was before, we don't
       need to do anything.  */
    if ((!requesting_width || request->width == w->core.width)
        && (!requesting_height || request->height == w->core.height))
	return XtGeometryYes;

#define REQUESTED_RIGHT_EDGE (w->core.x + request->width \
    + (w->core.border_width << 1))
#define REQUESTED_BOTTOM_EDGE (w->core.y + request->height \
    + (w->core.border_width << 1))

    /* Check if the child wants to go outside our current borders.  */
    parent_request.request_mode = 0;

    if (requesting_width) {
       if (REQUESTED_RIGHT_EDGE > fw->core.width) {
           parent_request.request_mode |= CWWidth;
           parent_request.width = request->width;
       } else
           /* Else the new size fits already, or is smaller.  */
           allowed.width = request->width;
    } else
       allowed.width = w->core.width; /* Don't change it.  */

    if (requesting_height) {
        if (REQUESTED_BOTTOM_EDGE > fw->core.height) {
            parent_request.request_mode |= CWHeight;
            parent_request.height = request->height;
        } else
            allowed.height = request->height;
    } else
        allowed.height = w->core.height;

    /* If necessary, ask our parent if we can grow.  */
    if (parent_request.request_mode != 0) {
        XtGeometryResult result;
        XtWidgetGeometry parent_return;
        
        if (querying)
          parent_request.request_mode |= XtCWQueryOnly;
        result = XtMakeGeometryRequest ((Widget) fw, &parent_request,
                                        &parent_return);
        if (result == XtGeometryNo)
            parent_no = True;
        else if (result == XtGeometryAlmost) {
            XtMakeGeometryRequest ((Widget) fw, &parent_return, NULL);
            parent_almost = True;
        }
        if (parent_request.request_mode & CWWidth)
            allowed.width = fw->core.width - w->core.x
                            - (w->core.border_width << 1);
        if (parent_request.request_mode & CWHeight)
            allowed.height = fw->core.height - w->core.y
                            - (w->core.border_width << 1);
    }

    /* If we needed to grow, and the parent didn't let us, we couldn't
       satisfy the request.  Say that we'll let the widget go right up to
       our edge, and return XtGeometryAlmost.  */
    if (parent_no || parent_almost) {
        if (reply != NULL) {
            reply->request_mode = CWWidth | CWHeight;
            reply->width = allowed.width;
            reply->height = allowed.height;
        }
        return XtGeometryAlmost;
    } else if (!querying) {
        /* We were able to satisfy everything, so reset the virtual width
           and height.  */
      	fc->form.virtual_width = w->core.width = allowed.width;
	fc->form.virtual_height = w->core.height = allowed.height;
	RefigureLocations(fw);
    }
    
    /* This return only applies to the last else clause, but putting it
       here keeps the warnings away.  */
    return XtGeometryYes;
}



/* ARGSUSED */
static Boolean SetValues(current, request, new)
    Widget current, request, new;
{
    return( FALSE );
}


/* ARGSUSED */
static void ConstraintInitialize(request, new)
    Widget request, new;
{
    FormConstraints form = (FormConstraints)new->core.constraints;
    FormWidget fw = (FormWidget)new->core.parent;

    form->form.virtual_width = (int) new->core.width;
    form->form.virtual_height = (int) new->core.height;

    if (form->form.dx == default_value)
        form->form.dx = fw->form.default_spacing;

    if (form->form.dy == default_value)
        form->form.dy = fw->form.default_spacing;
}

/* ARGSUSED */
static Boolean ConstraintSetValues(current, request, new)
    Widget current, request, new;
{
    return( FALSE );
}

static void ChangeManaged(w)
    Widget w;
{
  FormWidget fw = (FormWidget)w;
  FormConstraints form;
  WidgetList children, childP;
  int num_children = fw->composite.num_children;
  Widget child;
  
  /*
   * Reset virtual width and height for all children.
   */
  
  for (children = childP = fw->composite.children ;
       childP - children < num_children; childP++) {
    child = *childP;
    if (XtIsManaged(child)) {
      form = (FormConstraints)child->core.constraints;

/*
 * If the size is one (1) then we must not change the virtual sizes, as
 * they contain useful information.  If someone actually wants a widget of
 * width or height one (1) in a form widget he will lose, can't win them all.
 *
 * Chris D. Peterson 2/9/89.
 */
	 

      if ( child->core.width != 1)
	form->form.virtual_width = (int) child->core.width;
      if ( child->core.height != 1)
	form->form.virtual_height = (int) child->core.height;
    }
  }
  RefigureLocations( (FormWidget)w );
}


static XtGeometryResult PreferredGeometry( widget, request, reply  )
    Widget widget;
    XtWidgetGeometry *request, *reply;
{
    FormWidget w = (FormWidget)widget;
    
    reply->width = w->form.preferred_width;
    reply->height = w->form.preferred_height;
    reply->request_mode = CWWidth | CWHeight;
    if (  request->request_mode & (CWWidth | CWHeight) ==
	    reply->request_mode & (CWWidth | CWHeight)
	  && request->width == reply->width
	  && request->height == reply->height)
	return XtGeometryYes;
    else if (reply->width == w->core.width && reply->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}


/**********************************************************************
 *
 * Public routines
 *
 **********************************************************************/

/* 
 * Set or reset figuring (ignored if not realized)
 */

void XawFormDoLayout(w, doit)
Widget w;
Boolean doit;
{
    register FormWidget fw = (FormWidget)w;

    fw->form.no_refigure = !doit;

    if ( XtIsRealized(w) && fw->form.needs_relayout )
        RefigureLocations( fw );
}
