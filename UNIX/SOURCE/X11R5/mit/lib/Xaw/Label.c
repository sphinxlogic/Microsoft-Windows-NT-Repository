/* $XFree86: mit/lib/Xaw/Label.c,v 1.3 1993/03/27 09:10:03 dawes Exp $ */
/* $XConsortium: Label.c,v 1.92 91/07/24 16:56:28 converse Exp $ */

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

/*
 * Label.c - Label widget
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/LabelP.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Drawing.h>
#include <stdio.h>
#include <ctype.h>

#define streq(a,b) (strcmp( (a), (b) ) == 0)

#define MULTI_LINE_LABEL 32767

#ifdef CRAY
#define WORD64
#endif

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffsetOf(LabelRec, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(label.foreground), XtRString, XtDefaultForeground},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(label.font),XtRString, XtDefaultFont},
    {XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(label.label), XtRString, NULL},
    {XtNencoding, XtCEncoding, XtRUnsignedChar, sizeof(unsigned char),
	offset(label.encoding), XtRImmediate, (XtPointer)XawTextEncoding8bit},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	offset(label.justify), XtRImmediate, (XtPointer)XtJustifyCenter},
    {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(label.internal_width), XtRImmediate, (XtPointer)4},
    {XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(label.internal_height), XtRImmediate, (XtPointer)2},
    {XtNleftBitmap, XtCLeftBitmap, XtRBitmap, sizeof(Pixmap),
       offset(label.left_bitmap), XtRImmediate, (XtPointer) None},
    {XtNbitmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
	offset(label.pixmap), XtRImmediate, (XtPointer)None},
    {XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
	offset(label.resize), XtRImmediate, (XtPointer)True},
};
#undef offset

static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void ClassInitialize();
static void Destroy();
static XtGeometryResult QueryGeometry();

LabelClassRec labelClassRec = {
  {
/* core_class fields */	
#ifndef SVR3SHLIB
#define superclass		(&simpleClassRec)
#else
#define superclass		NULL
#endif
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"Label",
    /* widget_size	  	*/	sizeof(LabelRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
/* Simple class fields initialization */
  {
    /* change_sensitive		*/	XtInheritChangeSensitive
  }
};
#ifndef SVR3SHLIB
WidgetClass labelWidgetClass = (WidgetClass)&labelClassRec;
#else
extern WidgetClass labelWidgetClass;
#endif
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize()
{
    XawInitializeWidgetSet();
    XtAddConverter( XtRString, XtRJustify, XmuCvtStringToJustify, NULL, 0 );
}

#ifndef WORD64

#define TXT16 XChar2b

#else

#define TXT16 char

static XChar2b *buf2b;
static int buf2blen = 0;

_XawLabelWidth16(fs, str, n)
    XFontStruct *fs;
    char *str;
    int	n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    return XTextWidth16(fs, buf2b, n);
}

_XawLabelDraw16(dpy, d, gc, x, y, str, n)
    Display *dpy;
    Drawable d;
    GC gc;
    int x, y;
    char *str;
    int n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    XDrawString16(dpy, d, gc, x, y, buf2b, n);
}

#define XTextWidth16 _XawLabelWidth16
#define XDrawString16 _XawLabelDraw16

#endif /* WORD64 */

/*
 * Calculate width and height of displayed text in pixels
 */

static void SetTextWidthAndHeight(lw)
    LabelWidget lw;
{
    register XFontStruct	*fs = lw->label.font;
    char *nl;

    if (lw->label.pixmap != None) {
	Window root;
	int x, y;
	unsigned int width, height, bw, depth;
	if (XGetGeometry(XtDisplay(lw), lw->label.pixmap, &root, &x, &y,
			 &width, &height, &bw, &depth)) {
	    lw->label.label_height = height;
	    lw->label.label_width = width;
	    lw->label.label_len = depth;
	    return;
	}
    }

    lw->label.label_height = fs->max_bounds.ascent + fs->max_bounds.descent;
    if (lw->label.label == NULL) {
	lw->label.label_len = 0;
	lw->label.label_width = 0;
    }
    else if ((nl = index(lw->label.label, '\n')) != NULL) {
	char *label;
	lw->label.label_len = MULTI_LINE_LABEL;
	lw->label.label_width = 0;
	for (label = lw->label.label; nl != NULL; nl = index(label, '\n')) {
	    int width;

	    if (lw->label.encoding)
		width = XTextWidth16(fs, (TXT16*)label, (int)(nl - label)/2);
	    else
		width = XTextWidth(fs, label, (int)(nl - label));
	    if (width > (int)lw->label.label_width)
		lw->label.label_width = width;
	    label = nl + 1;
	    if (*label)
		lw->label.label_height +=
		    fs->max_bounds.ascent + fs->max_bounds.descent;
	}
	if (*label) {
	    int width = XTextWidth(fs, label, strlen(label));

	    if (lw->label.encoding)
		width = XTextWidth16(fs, (TXT16*)label, (int)strlen(label)/2);
	    else
		width = XTextWidth(fs, label, strlen(label));
	    if (width > (int) lw->label.label_width)
		lw->label.label_width = width;
	}
    } else {
	lw->label.label_len = strlen(lw->label.label);
	if (lw->label.encoding)
	    lw->label.label_width =
		XTextWidth16(fs, (TXT16*)lw->label.label,
			     (int) lw->label.label_len/2);
	else
	    lw->label.label_width =
		XTextWidth(fs, lw->label.label, (int) lw->label.label_len);
    }
}

static void GetnormalGC(lw)
    LabelWidget lw;
{
    XGCValues	values;

    values.foreground	= lw->label.foreground;
    values.background	= lw->core.background_pixel;
    values.font		= lw->label.font->fid;
    values.graphics_exposures = False;

    lw->label.normal_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont | GCGraphicsExposures,
	&values);
}

static void GetgrayGC(lw)
    LabelWidget lw;
{
    XGCValues	values;

    values.foreground = lw->label.foreground;
    values.background = lw->core.background_pixel;
    values.font	      = lw->label.font->fid;
    values.fill_style = FillTiled;
    values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)lw),
						lw->label.foreground, 
						lw->core.background_pixel,
						lw->core.depth);
    values.graphics_exposures = False;

    lw->label.stipple = values.tile;
    lw->label.gray_GC = XtGetGC((Widget)lw, 
				(unsigned) GCForeground | GCBackground |
					   GCFont | GCTile | GCFillStyle |
					   GCGraphicsExposures,
				&values);
}

static void compute_bitmap_offsets (lw)
    LabelWidget lw;
{
    /*
     * label will be displayed at (internal_width, internal_height + lbm_y)
     */
    if (lw->label.lbm_height != 0) {
	lw->label.lbm_y = (((int) lw->core.height) -
			   ((int) lw->label.internal_height * 2) -
			   ((int) lw->label.lbm_height)) / 2;
    } else {
	lw->label.lbm_y = 0;
    }
}


static void set_bitmap_info (lw)
    LabelWidget lw;
{
    Window root;
    int x, y;
    unsigned int bw, depth;

    if (!(lw->label.left_bitmap &&
	  XGetGeometry (XtDisplay(lw), lw->label.left_bitmap, &root, &x, &y,
			&lw->label.lbm_width, &lw->label.lbm_height,
			&bw, &depth))) {
	lw->label.lbm_width = lw->label.lbm_height = 0;
    }
    compute_bitmap_offsets (lw);
}



/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    LabelWidget lw = (LabelWidget) new;

    if (lw->label.label == NULL) 
        lw->label.label = XtNewString(lw->core.name);
    else {
        lw->label.label = XtNewString(lw->label.label);
    }

    GetnormalGC(lw);
    GetgrayGC(lw);

    SetTextWidthAndHeight(lw);

    if (lw->core.height == 0)
        lw->core.height = lw->label.label_height + 2*lw->label.internal_height;

    set_bitmap_info (lw);		/* need core.height */

    if (lw->core.width == 0)		/* need label.lbm_width */
        lw->core.width = (lw->label.label_width + 2 * lw->label.internal_width
			  + LEFT_OFFSET(lw));

    lw->label.label_x = lw->label.label_y = 0;
    (*XtClass(new)->core_class.resize) ((Widget)lw);

} /* Initialize */

/*
 * Repaint the widget window
 */

/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
   LabelWidget lw = (LabelWidget) w;
   GC gc;

   if (region != NULL) {
       int x = lw->label.label_x;
       unsigned int width = lw->label.label_width;
       if (lw->label.lbm_width) {
	   if (lw->label.label_x > (x = lw->label.internal_width))
	       width += lw->label.label_x - x;
       }
       if (XRectInRegion(region, x, lw->label.label_y,
			 width, lw->label.label_height) == RectangleOut)
	   return;
   }

   gc = XtIsSensitive((Widget)lw) ? lw->label.normal_GC : lw->label.gray_GC;
#ifdef notdef
   if (region != NULL) XSetRegion(XtDisplay(w), gc, region);
#endif /*notdef*/
   if (lw->label.pixmap == None) {
       int len = lw->label.label_len;
       char *label = lw->label.label;
       Position y = lw->label.label_y + lw->label.font->max_bounds.ascent;

       /* display left bitmap */
       if (lw->label.left_bitmap && lw->label.lbm_width != 0) {
	   XCopyPlane (XtDisplay(w), lw->label.left_bitmap, XtWindow(w), gc,
		       0, 0, lw->label.lbm_width, lw->label.lbm_height,
		       (int) lw->label.internal_width, 
		       (int) lw->label.internal_height + lw->label.lbm_y, 
		       (unsigned long) 1L);
       }

       if (len == MULTI_LINE_LABEL) {
	   char *nl;
	   while ((nl = index(label, '\n')) != NULL) {
	       if (lw->label.encoding)
		   XDrawString16(XtDisplay(w), XtWindow(w), gc,
				 lw->label.label_x, y,
				 (TXT16*)label, (int)(nl - label)/2);
	       else
		   XDrawString(XtDisplay(w), XtWindow(w), gc,
			       lw->label.label_x, y, label, (int)(nl - label));
	       y += lw->label.font->max_bounds.ascent + lw->label.font->max_bounds.descent;
	       label = nl + 1;
	   }
	   len = strlen(label);
       }
       if (len) {
	   if (lw->label.encoding)
	       XDrawString16(XtDisplay(w), XtWindow(w), gc,
			     lw->label.label_x, y, (TXT16*)label, len/2);
	   else
	       XDrawString(XtDisplay(w), XtWindow(w), gc,
			   lw->label.label_x, y, label, len);
       }
   } else if (lw->label.label_len == 1) { /* depth */
       XCopyPlane(XtDisplay(w), lw->label.pixmap, XtWindow(w), gc,
		  0, 0, lw->label.label_width, lw->label.label_height,
		  lw->label.label_x, lw->label.label_y, 1L);
   } else {
       XCopyArea(XtDisplay(w), lw->label.pixmap, XtWindow(w), gc,
		 0, 0, lw->label.label_width, lw->label.label_height,
		 lw->label.label_x, lw->label.label_y);
   }
#ifdef notdef
   if (region != NULL) XSetClipMask(XtDisplay(w), gc, (Pixmap)None);
#endif /* notdef */
}

static void _Reposition(lw, width, height, dx, dy)
    register LabelWidget lw;
    Dimension width, height;
    Position *dx, *dy;
{
    Position newPos;
    Position leftedge = lw->label.internal_width + LEFT_OFFSET(lw);

    switch (lw->label.justify) {

	case XtJustifyLeft   :
	    newPos = leftedge;
	    break;

	case XtJustifyRight  :
	    newPos = width -
		(lw->label.label_width + lw->label.internal_width);
	    break;

	case XtJustifyCenter :
	default:
	    newPos = (int)(width - lw->label.label_width) / 2;
	    break;
    }
    if (newPos < (Position)leftedge)
	newPos = leftedge;
    *dx = newPos - lw->label.label_x;
    lw->label.label_x = newPos;
    *dy = (newPos = (int)(height - lw->label.label_height) / 2)
	  - lw->label.label_y;
    lw->label.label_y = newPos;
    return;
}

static void Resize(w)
    Widget w;
{
    LabelWidget lw = (LabelWidget)w;
    Position dx, dy;
    _Reposition(lw, w->core.width, w->core.height, &dx, &dy);
    compute_bitmap_offsets (lw);
}

/*
 * Set specified arguments into widget
 */

#define PIXMAP 0
#define WIDTH 1
#define HEIGHT 2
#define NUM_CHECKS 3

static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    LabelWidget curlw = (LabelWidget) current;
    LabelWidget reqlw = (LabelWidget) request;
    LabelWidget newlw = (LabelWidget) new;
    int i;
    Boolean was_resized = False, redisplay = False, checks[NUM_CHECKS];

    for (i = 0; i < NUM_CHECKS; i++)
	checks[i] = FALSE;

    for (i = 0; i < *num_args; i++) {
	if (streq(XtNbitmap, args[i].name))
	    checks[PIXMAP] = TRUE;
	if (streq(XtNwidth, args[i].name))
	    checks[WIDTH] = TRUE;
	if (streq(XtNheight, args[i].name))
	    checks[HEIGHT] = TRUE;
    }

    if (newlw->label.label == NULL) {
	newlw->label.label = newlw->core.name;
    }

    /*
     * resize on bitmap change
     */
    if (curlw->label.left_bitmap != newlw->label.left_bitmap) {
	was_resized = True;
    }

    if (curlw->label.encoding != newlw->label.encoding)
	was_resized = True;

    if (curlw->label.label != newlw->label.label) {
        if (curlw->label.label != curlw->core.name)
	    XtFree( (char *)curlw->label.label );

	if (newlw->label.label != newlw->core.name) {
	    newlw->label.label = XtNewString( newlw->label.label );
	}
	was_resized = True;
    }

    if (was_resized || (curlw->label.font != newlw->label.font) ||
	(curlw->label.justify != newlw->label.justify) || checks[PIXMAP]) {

	SetTextWidthAndHeight(newlw);
	was_resized = True;
    }

    /* recalculate the window size if something has changed. */
    if (newlw->label.resize && was_resized) {
	if ((curlw->core.height == reqlw->core.height) && !checks[HEIGHT])
	    newlw->core.height = (newlw->label.label_height + 
				  2 * newlw->label.internal_height);

	set_bitmap_info (newlw);

	if ((curlw->core.width == reqlw->core.width) && !checks[WIDTH])
	    newlw->core.width = (newlw->label.label_width +
				 LEFT_OFFSET(newlw) +
				 2 * newlw->label.internal_width);
    }

    if (curlw->label.foreground != newlw->label.foreground
	|| curlw->core.background_pixel != newlw->core.background_pixel
	|| curlw->label.font->fid != newlw->label.font->fid) {

	XtReleaseGC(new, curlw->label.normal_GC);
	XtReleaseGC(new, curlw->label.gray_GC);
	XmuReleaseStippledPixmap( XtScreen(current), curlw->label.stipple );
	GetnormalGC(newlw);
	GetgrayGC(newlw);
	redisplay = True;
    }

    if ((curlw->label.internal_width != newlw->label.internal_width)
        || (curlw->label.internal_height != newlw->label.internal_height)
	|| was_resized) {
	/* Resize() will be called if geometry changes succeed */
	Position dx, dy;
	_Reposition(newlw, curlw->core.width, curlw->core.height, &dx, &dy);
    }

    return was_resized || redisplay ||
	   XtIsSensitive(current) != XtIsSensitive(new);
}

static void Destroy(w)
    Widget w;
{
    LabelWidget lw = (LabelWidget)w;

    XtFree( lw->label.label );
    XtReleaseGC( w, lw->label.normal_GC );
    XtReleaseGC( w, lw->label.gray_GC);
    XmuReleaseStippledPixmap( XtScreen(w), lw->label.stipple );
}


static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register LabelWidget lw = (LabelWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = (lw->label.label_width + 2 * lw->label.internal_width +
			LEFT_OFFSET(lw));
    preferred->height = lw->label.label_height + 2*lw->label.internal_height;
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}
