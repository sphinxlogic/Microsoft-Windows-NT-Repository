/*
   This is a modified version of the form widget.
   It draws some shadows in the interior of the form widget
   on expense of its internal size.

   Please note that this widget is not intended to be used
   anywhere else than in GhostView-VMS.
   Only a minimum amount of work has been spent on this widget.
   It may contain many more than the usual amount of errors
   and has survived the last revision of GhostView-VMS only
   because no suitable frame widget has been found yet.
   Either this widget has to be recoded (there are surely
   easier ways to implement the wanted effect) or it has
   to be replaced by a more powerful framing device.
   In this context I'd be glad if someone knows about a frame
   widget and could point out to me the location of its sources.

   I'm sorry that this 'thing' has to be handed out in its current
   state.

   Rights, permissions, and disclaimer of warranty are as in the
   DEC and MIT notice below. 

   Johannes Plass, plass@dipmza.physik.uni-mainz.de
   ###jp###, 11/94
*/

/* $XConsortium: Form.c,v 1.49 91/10/16 21:34:30 eswu Exp $ */

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

#ifdef VMS
#   include <X11_DIRECTORY/Xlib.h>
#   include <X11_DIRECTORY/IntrinsicP.h>
#   include <X11_DIRECTORY/StringDefs.h>
#   include <XMU_DIRECTORY/Converters.h>
#   include <XMU_DIRECTORY/CharSet.h>
#   include <XAW_DIRECTORY/XawInit.h>
#   include "Form3dP.h"
#   include <X11_DIRECTORY/Xosdefs.h>
#else
#   include <X11/Xlib.h>
#   include <X11/IntrinsicP.h>
#   include <X11/StringDefs.h>
#   include <X11/Xmu/Converters.h>
#   include <X11/Xmu/CharSet.h>
#   include <X11/Xaw3d/XawInit.h>
#   include "Form3dP.h"
#   include <X11/Xosdefs.h>
#endif

#include "message.h"

/* Private Definitions */

static int default_value = -99999;

#define XtNtopShadowPixmap "topShadowPixmap"
#define XtCTopShadowPixmap "TopShadowPixmap"
#define XtNbottomShadowPixmap "bottomShadowPixmap"
#define XtCBottomShadowPixmap "BottomShadowPixmap"

#define Offset(field) XtOffsetOf(Form3dRec, form.field)
#define offset(field) XtOffsetOf(Form3dRec, threeD.field)

static XtResource resources[] = {
    {XtNdefaultDistance, XtCThickness, XtRInt, sizeof(int),
	Offset(default_spacing), XtRImmediate, (XtPointer)2},
    {XtNshadowWidth, XtCShadowWidth, XtRDimension, sizeof(Dimension),
	offset(shadow_width), XtRImmediate, (XtPointer) 2},
    {XtNtopShadowPixel, XtCTopShadowPixel, XtRPixel, sizeof(Pixel),
	offset(top_shadow_pixel), XtRString, XtDefaultForeground},
    {XtNbottomShadowPixel, XtCBottomShadowPixel, XtRPixel, sizeof(Pixel),
	offset(bot_shadow_pixel), XtRString, XtDefaultForeground},
    {XtNtopShadowPixmap, XtCTopShadowPixmap, XtRPixmap, sizeof(Pixmap),
	offset(top_shadow_pxmap), XtRImmediate, (XtPointer) NULL},
    {XtNbottomShadowPixmap, XtCBottomShadowPixmap, XtRPixmap, sizeof(Pixmap),
	offset(bot_shadow_pxmap), XtRImmediate, (XtPointer) NULL},
    {XtNtopShadowContrast, XtCTopShadowContrast, XtRInt, sizeof(int),
	offset(top_shadow_contrast), XtRImmediate, (XtPointer) 20},
    {XtNbottomShadowContrast, XtCBottomShadowContrast, XtRInt, sizeof(int),
	offset(bot_shadow_contrast), XtRImmediate, (XtPointer) 40},
    {XtNuserData, XtCUserData, XtRPointer, sizeof(XtPointer),
	offset(user_data), XtRPointer, (XtPointer) NULL},
    {XtNbeNiceToColormap, XtCBeNiceToColormap, XtRBoolean, sizeof(Boolean),
	offset(be_nice_to_cmap), XtRImmediate, (XtPointer) True}
};
#undef Offset
#undef offset

static XtEdgeType defEdge = XtRubber;

#define Offset(field) XtOffsetOf(Form3dConstraintsRec, form.field)
static XtResource form3dConstraintResources[] = {
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
static void Destroy();
static void Redisplay();
static void Realize();
static void  _Form3dDrawShadows();

static void LayoutChild(), ResizeChildren();

Form3dClassRec form3dClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "Form3d",
    /* widget_size        */    sizeof(Form3dRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    Realize,
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
    /* expose             */    Redisplay,
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
    /* subresourses       */   form3dConstraintResources,
    /* subresource_count  */   XtNumber(form3dConstraintResources),
    /* constraint_size    */   sizeof(Form3dConstraintsRec),
    /* initialize         */   ConstraintInitialize,
    /* destroy            */   Destroy,
    /* set_values         */   ConstraintSetValues,
    /* extension          */   NULL
  },
  { /* form3d_class fields */
    /* layout             */   Layout
  },
  { /* threeD fields */
    /* shadow draw              */      _Form3dDrawShadows
  }


};

WidgetClass form3dWidgetClass = (WidgetClass)&form3dClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

#define shadowpm_width 8
#define shadowpm_height 8
static unsigned char shadowpm_bits[] = {
    0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55};

static unsigned char mtshadowpm_bits[] = {
    0x92, 0x24, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24};

static unsigned char mbshadowpm_bits[] = {
    0x6d, 0xdb, 0xb6, 0x6d, 0xdb, 0xb6, 0x6d, 0xdb};

/* ARGSUSED */
static void AllocTopShadowGC (w)
    Widget w;
{
    Form3dWidget	tdw = (Form3dWidget) w;
    Screen		*scn = XtScreen (w);
    XtGCMask		valuemask;
    XGCValues		myXGCV;

    if (tdw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scn) == 1) {
	valuemask = GCTile | GCFillStyle;
	myXGCV.tile = tdw->threeD.top_shadow_pxmap;
	myXGCV.fill_style = FillTiled;
    } else {
	valuemask = GCForeground;
	myXGCV.foreground = tdw->threeD.top_shadow_pixel;
    }
    tdw->threeD.top_shadow_GC = XtGetGC(w, valuemask, &myXGCV);
}

/* ARGSUSED */
static void AllocBotShadowGC (w)
    Widget w;
{
    Form3dWidget	tdw = (Form3dWidget) w;
    Screen		*scn = XtScreen (w);
    XtGCMask		valuemask;
    XGCValues		myXGCV;

    if (tdw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scn) == 1) {
	valuemask = GCTile | GCFillStyle;
	myXGCV.tile = tdw->threeD.bot_shadow_pxmap;
	myXGCV.fill_style = FillTiled;
    } else {
	valuemask = GCForeground;
	myXGCV.foreground = tdw->threeD.bot_shadow_pixel;
    }
    tdw->threeD.bot_shadow_GC = XtGetGC(w, valuemask, &myXGCV);
}

/* ARGSUSED */
static void AllocTopShadowPixmap (new)
    Widget new;
{
    Form3dWidget	tdw = (Form3dWidget) new;
    Display		*dpy = XtDisplay (new);
    Screen		*scn = XtScreen (new);
    unsigned long	top_fg_pixel, top_bg_pixel;
    unsigned char	*pm_data;
    Boolean		create_pixmap = FALSE;

    /*
     * I know, we're going to create two pixmaps for each and every
     * shadow'd widget.  Yeuck.  I'm semi-relying on server side
     * pixmap cacheing.
     */

    if (DefaultDepthOfScreen (scn) == 1) {
	top_fg_pixel = BlackPixelOfScreen (scn);
	top_bg_pixel = WhitePixelOfScreen (scn);
	pm_data = mtshadowpm_bits;
	create_pixmap = TRUE;
    } else if (tdw->threeD.be_nice_to_cmap) {
	if (tdw->core.background_pixel == WhitePixelOfScreen (scn)) {
	    top_fg_pixel = WhitePixelOfScreen (scn);
	    top_bg_pixel = BlackPixelOfScreen (scn);
	} else if (tdw->core.background_pixel == BlackPixelOfScreen (scn)) {
	    top_fg_pixel = BlackPixelOfScreen (scn);
	    top_bg_pixel = WhitePixelOfScreen (scn);
	} else {
	    top_fg_pixel = tdw->core.background_pixel;
	    top_bg_pixel = WhitePixelOfScreen (scn);
	}
	if (tdw->core.background_pixel == WhitePixelOfScreen (scn) ||
	    tdw->core.background_pixel == BlackPixelOfScreen (scn))
	    pm_data = mtshadowpm_bits;
	else
	    pm_data = shadowpm_bits;
	create_pixmap = TRUE;
    }
    if (create_pixmap)
	tdw->threeD.top_shadow_pxmap = XCreatePixmapFromBitmapData (dpy,
			RootWindowOfScreen (scn),
			(char *) pm_data,
			shadowpm_width,
			shadowpm_height,
			top_fg_pixel,
			top_bg_pixel,
			DefaultDepthOfScreen (scn));
}

/* ARGSUSED */
static void AllocBotShadowPixmap (new)
    Widget new;
{
    Form3dWidget	tdw = (Form3dWidget) new;
    Display		*dpy = XtDisplay (new);
    Screen		*scn = XtScreen (new);
    unsigned long	bot_fg_pixel, bot_bg_pixel;
    unsigned char	*pm_data;
    Boolean		create_pixmap = FALSE;

    if (DefaultDepthOfScreen (scn) == 1) {
	bot_fg_pixel = BlackPixelOfScreen (scn);
	bot_bg_pixel = WhitePixelOfScreen (scn);
	pm_data = mbshadowpm_bits;
	create_pixmap = TRUE;
    } else if (tdw->threeD.be_nice_to_cmap) {
	if (tdw->core.background_pixel == WhitePixelOfScreen (scn)) {
	    bot_fg_pixel = WhitePixelOfScreen (scn);
	    bot_bg_pixel = BlackPixelOfScreen (scn);
	} else if (tdw->core.background_pixel == BlackPixelOfScreen (scn)) {
	    bot_fg_pixel = BlackPixelOfScreen (scn);
	    bot_bg_pixel = WhitePixelOfScreen (scn);
	} else {
	    bot_fg_pixel = tdw->core.background_pixel;
	    bot_bg_pixel = BlackPixelOfScreen (scn);
	}
	if (tdw->core.background_pixel == WhitePixelOfScreen (scn) ||
	    tdw->core.background_pixel == BlackPixelOfScreen (scn))
	    pm_data = mbshadowpm_bits;
	else
	    pm_data = shadowpm_bits;
	create_pixmap = TRUE;
    }
    if (create_pixmap)
	tdw->threeD.bot_shadow_pxmap = XCreatePixmapFromBitmapData (dpy,
			RootWindowOfScreen (scn),
			(char *) pm_data,
			shadowpm_width,
			shadowpm_height,
			bot_fg_pixel,
			bot_bg_pixel,
			DefaultDepthOfScreen (scn));
}

/* ARGSUSED */
void _Form3dComputeTopShadowRGB (scn, pixel, icontrast, xcol_out)
    Screen *scn;
    unsigned long pixel;
    int icontrast;
    XColor *xcol_out;
{
    XColor get_c;
    double contrast;
    Display *dpy = scn->display;
    Colormap cmap = DefaultColormapOfScreen (scn);

    get_c.pixel = pixel;
    if (get_c.pixel == WhitePixelOfScreen (scn) ||
	get_c.pixel == BlackPixelOfScreen (scn)) {
	contrast = (100 - icontrast) / 100.0;
	xcol_out->red = xcol_out->green = xcol_out->blue = contrast * 65535.0;
    } else {
	contrast = 1.0 + icontrast / 100.0;
	XQueryColor (dpy, cmap, &get_c);
#       ifdef MIN
#          undef MIN
#       endif
#       define MIN(x,y) (unsigned short) (x < y) ? x : y
	xcol_out->red   = MIN (65535, (int) (contrast * (double) get_c.red));
	xcol_out->green = MIN (65535, (int) (contrast * (double) get_c.green));
	xcol_out->blue  = MIN (65535, (int) (contrast * (double) get_c.blue));
#       undef MIN
    }
}

/* ARGSUSED */
static void AllocTopShadowPixel (new)
    Widget new;
{
    XColor set_c;
    Form3dWidget tdw = (Form3dWidget) new;
    Display *dpy = XtDisplay (new);
    Screen *scn = XtScreen (new);
    Colormap cmap = DefaultColormapOfScreen (scn);

    _Form3dComputeTopShadowRGB (XtScreen(new),
			       tdw->core.background_pixel, 
			       tdw->threeD.top_shadow_contrast,
			       &set_c);
    (void) XAllocColor (dpy, cmap, &set_c);
    tdw->threeD.top_shadow_pixel = set_c.pixel;
}

/* ARGSUSED */
void _Form3dComputeBotShadowRGB (scn, pixel, icontrast, xcol_out)
    Screen *scn;
    unsigned long pixel;
    int icontrast;
    XColor *xcol_out;
{
    XColor get_c;
    double contrast;
    Display *dpy = scn->display;
    Colormap cmap = DefaultColormapOfScreen (scn);

    get_c.pixel = pixel;
    if (get_c.pixel == WhitePixelOfScreen (scn) ||
	get_c.pixel == BlackPixelOfScreen (scn)) {
	contrast = icontrast / 100.0;
	xcol_out->red = xcol_out->green = xcol_out->blue = contrast * 65535.0;
    } else {
	XQueryColor (dpy, cmap, &get_c);
	contrast = (100 - icontrast) / 100.0;
	xcol_out->red   = contrast * get_c.red;
	xcol_out->green = contrast * get_c.green;
	xcol_out->blue  = contrast * get_c.blue;
    }
}

/* ARGSUSED */
static void AllocBotShadowPixel (new)
    Widget new;
{
    XColor set_c;
    Form3dWidget tdw = (Form3dWidget) new;
    Display *dpy = XtDisplay (new);
    Screen *scn = XtScreen (new);
    Colormap cmap = DefaultColormapOfScreen (scn);

    _Form3dComputeBotShadowRGB (XtScreen(new), 
			       tdw->core.background_pixel, 
			       tdw->threeD.bot_shadow_contrast,
			       &set_c);
    (void) XAllocColor (dpy, cmap, &set_c);
    tdw->threeD.bot_shadow_pixel = set_c.pixel;
}


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
	{XtBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.parent),
	     sizeof(Widget)}
    };
    BEGINMESSAGE(ClassInitialize)
    XawInitializeWidgetSet();
    XtQChainLeft   = XrmPermStringToQuark("chainleft");
    XtQChainRight  = XrmPermStringToQuark("chainright");
    XtQChainTop    = XrmPermStringToQuark("chaintop");
    XtQChainBottom = XrmPermStringToQuark("chainbottom");
    XtQRubber      = XrmPermStringToQuark("rubber");

    XtAddConverter( XtRString, XtREdgeType, _CvtStringToEdgeType, 
		    (XtConvertArgList)NULL, 0 );
    XtSetTypeConverter (XtRString, XtRWidget, XmuNewCvtStringToWidget,
			parentCvtArgs, XtNumber(parentCvtArgs), XtCacheNone,
			(XtDestructor)NULL);
    ENDMESSAGE(ClassInitialize)
}

static void ClassPartInitialize(class)
    WidgetClass class;
{
    register Form3dWidgetClass c = (Form3dWidgetClass)class;
    register Form3dWidgetClass super = (Form3dWidgetClass) 
	c->core_class.superclass;

    BEGINMESSAGE(ClassPartInitialize)
    if (c->form_class.layout == XtInheritLayout)
	c->form_class.layout = super->form_class.layout;
    if (c->threeD_class.shadowdraw == XtInheritForm3dShadowDraw)
	c->threeD_class.shadowdraw = super->threeD_class.shadowdraw;
    ENDMESSAGE(ClassPartInitialize)
}

/* ARGSUSED */
static void Initialize(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    Form3dWidget fw = (Form3dWidget)new;
    Screen		*scr = XtScreen (new);

    BEGINMESSAGE(Initialize)
    fw->form.old_width = fw->core.width;
    fw->form.old_height = fw->core.height;
    fw->form.no_refigure = False;
    fw->form.needs_relayout = False;
    fw->form.resize_in_layout = True;
    fw->form.resize_is_no_op = False;

    if (fw->threeD.be_nice_to_cmap || DefaultDepthOfScreen (scr) == 1) {
	AllocTopShadowPixmap (new);
	AllocBotShadowPixmap (new);
    } else {
	if (fw->threeD.top_shadow_pixel == fw->threeD.bot_shadow_pixel) {
	    /* 
		Eeek.  We're probably going to XQueryColor() twice 
		for each widget.  Necessary because you can set the
		top and bottom shadows independent of each other in
		SetValues.  Some cacheing would certainly help...
	    */
	    AllocTopShadowPixel (new);
	    AllocBotShadowPixel (new);
	}
	fw->threeD.top_shadow_pxmap = fw->threeD.bot_shadow_pxmap = (Pixmap) 0;
    }
    AllocTopShadowGC (new);
    AllocBotShadowGC (new);
    ENDMESSAGE(Initialize)

}

/*	Function Name: ChangeForm3dGeometry
 *	Description: Ask the parent to change the form widget's geometry.
 *	Arguments: w - the Form3d widget.
 *                 query_only - TRUE if this is only a query.
 *                 width, height - the new width and height.
 *                 ret_width, ret_height - the actual size the form is allowed
 *                                         to resize to.
 *	Returns: TRUE of children may always be resized.
 */

static Boolean
ChangeForm3dGeometry(w, query_only, width, height, ret_width, ret_height)
Widget w;
Boolean query_only;
Dimension width, height;
Dimension *ret_width, *ret_height;
{
    Form3dWidget fw = (Form3dWidget) w;
    Boolean always_resize_children;
    XtGeometryResult result;
    XtWidgetGeometry request, return_request;

    BEGINMESSAGE(ChangeForm3dGeometry)
    /*
     * If we are already at the desired size then there is no need
     * to ask our parent of we can change size.
     */

    if ( (width == fw->core.width) && (height == fw->core.height) ) {
        ENDMESSAGE(ChangeForm3dGeometry)
	return(TRUE);
    }

    request.width = width;
    request.height = height;
    request.request_mode = CWWidth | CWHeight;
    if (query_only)
	request.request_mode |= XtCWQueryOnly;

    /*
     * Do no invoke the resize rules if our size changes here.
     */

    fw->form.resize_is_no_op = TRUE;

    result = XtMakeGeometryRequest(w, &request, &return_request);
    if (result == XtGeometryAlmost) {
	request = return_request;
	(void) XtMakeGeometryRequest(w, &request, &return_request);
	always_resize_children = FALSE;
    }
    else
	always_resize_children = (result == XtGeometryYes);

    fw->form.resize_is_no_op = FALSE;

    if (ret_width != NULL)
	*ret_width = request.width;
    if (ret_height != NULL)
	*ret_height = request.height;

    ENDMESSAGE(ChangeForm3dGeometry)
    return(always_resize_children);
}

/*	Function Name: Layout
 *	Description: Moves all the children around.
 *	Arguments: fw - the Form3d widget.
 *                 width, height - ** UNUSED **.
 *                 force_relayout - will force the children to be
 *                                 moved, even if some go past the edge
 *                                 of the form.
 *	Returns: True if the children are allowed to move from their
 *               current locations to the new ones.
 */

/* ARGSUSED */
static Boolean Layout(fw, width, height, force_relayout)
    Form3dWidget fw;
    Dimension width, height;
    Boolean force_relayout;
{
    int num_children = fw->composite.num_children;
    WidgetList children = fw->composite.children;
    Widget *childP;
    Dimension maxx, maxy;
    Boolean ret_val;

    BEGINMESSAGE(Layout)
    for (childP = children; childP - children < num_children; childP++) {
	Form3dConstraints form = (Form3dConstraints)(*childP)->core.constraints;
	form->form.layout_state = LayoutPending;
    }

    maxx = maxy = 1;
    for (childP = children; childP - children < num_children; childP++) {
	if (XtIsManaged(*childP)) {
	    register Form3dConstraints form;
	    register Position x, y;

	    form = (Form3dConstraints)(*childP)->core.constraints;

	    LayoutChild(*childP);

	    x = form->form.new_x + (*childP)->core.width + 
		((*childP)->core.border_width << 1);
	    if (x > (int)maxx)
		maxx = x;

	    y = form->form.new_y + (*childP)->core.height +
		((*childP)->core.border_width << 1);
	    if (y > (int)maxy)
		maxy = y;
	}
    }

    fw->form.preferred_width = (maxx += fw->form.default_spacing);
    fw->form.preferred_height = (maxy += fw->form.default_spacing);

    if (fw->form.resize_in_layout) {
	Boolean always_resize_children;

	always_resize_children = 
	    ChangeForm3dGeometry( (Widget) fw, FALSE, maxx, maxy, 
				(Dimension *)NULL, (Dimension *)NULL);

	fw->form.old_width  = fw->core.width;
	fw->form.old_height = fw->core.height;

	ret_val = (always_resize_children || ( (fw->core.width >= maxx) &&
					      (fw->core.height >= maxy)));

	if (force_relayout)
	    ret_val = TRUE;

	if (ret_val)
	    ResizeChildren((Widget) fw);
    }
    else
	ret_val = False;

    fw->form.needs_relayout = False;

    ENDMESSAGE(Layout)
    return ret_val;
}

/*	Function Name: ResizeChildren
 *	Description: Resizes all children to new_x and new_y.
 *	Arguments: w - the form widget.
 *	Returns: none.
 */

static void ResizeChildren(w) 
Widget w;
{
    Form3dWidget fw = (Form3dWidget) w;
    int num_children = fw->composite.num_children;
    WidgetList children = fw->composite.children;
    Widget *childP;

    BEGINMESSAGE(ResizeChildren)

    for (childP = children; childP - children < num_children; childP++) {
	Form3dConstraints form;

	if (!XtIsManaged(*childP)) 
	    continue;

	form = (Form3dConstraints)(*childP)->core.constraints;
	if (fw->form.no_refigure) {
/* 
 * I am changing the widget wrapper w/o modifing the window.  This is
 * risky, but I can get away with it since I am the parent of this
 * widget, and he must ask me for any geometry changes.
 *
 * The window will be updated when no_refigure is set back to False.
 */	
	    (*childP)->core.x = form->form.new_x;
	    (*childP)->core.y = form->form.new_y;
	}
	else
	    XtMoveWidget(*childP, form->form.new_x, form->form.new_y);
    }
    ENDMESSAGE(ResizeChildren)
}


static void LayoutChild(w)
    Widget w;
{
    Form3dConstraints form = (Form3dConstraints)w->core.constraints;
    Widget ref;

    BEGINMESSAGE(LayoutChild)
    switch (form->form.layout_state) {

      case LayoutPending:
	form->form.layout_state = LayoutInProgress;
	break;

      case LayoutDone:
        {
        ENDMESSAGE(LayoutChild)
	return;
        }
      case LayoutInProgress:
	{
	String subs[2];
	Cardinal num_subs = 2;
	subs[0] = w->core.name;
	subs[1] = w->core.parent->core.name;
	XtAppWarningMsg(XtWidgetToApplicationContext(w),
			"constraintLoop","xawForm3dLayout","XawToolkitError",
   "constraint loop detected while laying out child '%s' in Form3dWidget '%s'",
			subs, &num_subs);
        ENDMESSAGE(LayoutChild)
	return;
	}
    }

    form->form.new_x = form->form.dx;
    form->form.new_y = form->form.dy;
    if ((ref = form->form.horiz_base) != (Widget)NULL) {
	Form3dConstraints ref_form = (Form3dConstraints) ref->core.constraints;

	LayoutChild(ref);
	form->form.new_x += (ref_form->form.new_x + 
			     ref->core.width + (ref->core.border_width << 1));
    }
    if ((ref = form->form.vert_base) != (Widget)NULL) {
	Form3dConstraints ref_form = (Form3dConstraints) ref->core.constraints;

	LayoutChild(ref);
	form->form.new_y += (ref_form->form.new_y + 
			     ref->core.height + (ref->core.border_width << 1));
    }

    form->form.layout_state = LayoutDone;

    ENDMESSAGE(LayoutChild)
}


static Position TransformCoord(loc, old, new, type)
    register Position loc;
    Dimension old, new;
    XtEdgeType type;
{
    BEGINMESSAGE(TransformCoord)
    if (type == XtRubber) {
        if ( ((int) old) > 0)
	    loc = (int)(loc * new) / (int)old;
    }
    else if (type == XtChainBottom || type == XtChainRight)
      loc += (Position)new - (Position)old;

    /* I don't see any problem with returning values less than zero. */

    ENDMESSAGE(TransformCoord)
    return (loc);
}

static void Resize(w)
    Widget w;
{
    Form3dWidget fw = (Form3dWidget)w;
    WidgetList children = fw->composite.children;
    int num_children = fw->composite.num_children;
    Widget *childP;
    Position x, y;
    Dimension width, height;

    BEGINMESSAGE(Resize)
    if (!fw->form.resize_is_no_op)
	for (childP = children; childP - children < num_children; childP++) {
	    Form3dConstraints form= (Form3dConstraints)(*childP)->core.constraints;
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
	    
	    XtConfigureWidget(*childP,x,y, (Dimension)width, (Dimension)height,
			      (*childP)->core.border_width );
	}

    fw->form.old_width = fw->core.width;
    fw->form.old_height = fw->core.height;
    ENDMESSAGE(Resize)
}

/*
 * I don't want to even think about what ``Almost'' would mean - Chris.
 */

/* ARGSUSED */
static XtGeometryResult GeometryManager(w, request, reply)
    Widget w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;	/* RETURN */
{
    Dimension old_width, old_height;
    Form3dWidget fw = (Form3dWidget) XtParent(w);
    Form3dConstraints form = (Form3dConstraints) w->core.constraints;
    XtWidgetGeometry allowed;
    XtGeometryResult ret_val;

    BEGINMESSAGE(GeometryManager)

    INFSMESSAGE(name of requesting widget:,XtName(w))
    INFIMESSAGE(width:,w->core.width)  INFIMESSAGE(height:,w->core.height)    

    if ((request->request_mode & ~(XtCWQueryOnly | CWWidth | CWHeight)) ||
	!form->form.allow_resize) {

	/* If GeometryManager is invoked during a SetValues call on a child
         * then it is necessary to compute a new layout if ConstraintSetValues
         * allowed any constraint changes. */

	if (fw->form.needs_relayout) 
	    (*((Form3dWidgetClass)fw->core.widget_class)->form_class.layout)
		(fw, 0, 0, True);
        INFMESSAGE(denied)
        ENDMESSAGE(GeometryManager)
	return(XtGeometryNo);
    }

    if (request->request_mode & CWWidth) allowed.width = request->width;
    else allowed.width = w->core.width;

    if (request->request_mode & CWHeight) allowed.height = request->height;
    else allowed.height = w->core.height;

    if (allowed.width == w->core.width && allowed.height == w->core.height) {

	/* If GeometryManager is invoked during a SetValues call on a child
         * then it is necessary to compute a new layout if ConstraintSetValues
         * allowed any constraint changes. */

	if (fw->form.needs_relayout) 
	    (*((Form3dWidgetClass)fw->core.widget_class)->form_class.layout)
		(fw, 0, 0, True);
        ENDMESSAGE(GeometryManager)
	return(XtGeometryNo);
    }

    /*
     * Remember the old size, and then set the size to the requested size.
     */

    old_width = w->core.width;
    old_height = w->core.height;
    w->core.width = allowed.width;
    w->core.height = allowed.height;

    if (request->request_mode & XtCWQueryOnly) {
	Boolean always_resize_children;
	Dimension ret_width, ret_height;

	fw->form.resize_in_layout = FALSE;

	(*((Form3dWidgetClass)fw->core.widget_class)->form_class.layout)
  	                                 ( fw, w->core.width, w->core.height,
					   FALSE );

	/*
	 * Reset the size of this child back to what it used to be.
	 */

	w->core.width = old_width;
	w->core.height = old_height;

	fw->form.resize_in_layout = TRUE;

	always_resize_children = ChangeForm3dGeometry(w, TRUE, 
				   fw->form.preferred_width,
				   fw->form.preferred_height,
				   &ret_width, &ret_height);

	if (always_resize_children || 
	    ((ret_width >= fw->form.preferred_width) &&
	     (ret_height >= fw->form.preferred_height)))
	{
	    ret_val = XtGeometryYes;
	}
	else
	    ret_val = XtGeometryNo;
    }
    else {
	if ((*((Form3dWidgetClass)fw->core.widget_class)->form_class.layout)
	                                  ( fw, w->core.width, w->core.height,
					    FALSE))
	{
	    form->form.virtual_width = w->core.width;   /* reset virtual */
	    form->form.virtual_height = w->core.height; /* width and height. */
	    if (fw->form.no_refigure) {
/* 
 * I am changing the widget wrapper w/o modifing the window.  This is
 * risky, but I can get away with it since I am the parent of this
 * widget, and he must ask me for any geometry changes.
 *
 * The window will be updated when no_refigure is set back to False.
 */	
		form->form.deferred_resize = True;
		ret_val = XtGeometryDone;
	    }
	    else 
		ret_val = XtGeometryYes;
	}
	else {
	    w->core.width = old_width;
	    w->core.height = old_height;
	    ret_val = XtGeometryNo;
	}
    }

    INFIMESSAGE(width:,w->core.width)  INFIMESSAGE(height:,w->core.height)    
    ENDMESSAGE(GeometryManager)
    return(ret_val);
}


/* ARGSUSED */
static Boolean SetValues(gcurrent, grequest, gnew, args, num_args)
    Widget gcurrent, grequest, gnew;
    ArgList args;
    Cardinal *num_args;
{
    Form3dWidget current = (Form3dWidget) gcurrent;
    Form3dWidget new = (Form3dWidget) gnew;
    Boolean redisplay = FALSE;
    Boolean alloc_top_pixel = FALSE;
    Boolean alloc_bot_pixel = FALSE;
    Boolean alloc_top_pxmap = FALSE;
    Boolean alloc_bot_pxmap = FALSE;

    BEGINMESSAGE(SetValues)

    if (new->threeD.shadow_width != current->threeD.shadow_width)
	redisplay = TRUE;
    if (new->threeD.be_nice_to_cmap != current->threeD.be_nice_to_cmap) {
	if (new->threeD.be_nice_to_cmap) {
	    alloc_top_pxmap = TRUE;
	    alloc_bot_pxmap = TRUE;
	} else {
	    alloc_top_pixel = TRUE;
	    alloc_bot_pixel = TRUE;
	}
	redisplay = TRUE;
    }
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.top_shadow_contrast != current->threeD.top_shadow_contrast)
	alloc_top_pixel = TRUE;
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.bot_shadow_contrast != current->threeD.bot_shadow_contrast)
	alloc_bot_pixel = TRUE;
    if (alloc_top_pixel)
	AllocTopShadowPixel (gnew);
    if (alloc_bot_pixel)
	AllocBotShadowPixel (gnew);
    if (alloc_top_pxmap)
	AllocTopShadowPixmap (gnew);
    if (alloc_bot_pxmap)
	AllocBotShadowPixmap (gnew);
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.top_shadow_pixel != current->threeD.top_shadow_pixel)
	alloc_top_pixel = TRUE;
    if (!new->threeD.be_nice_to_cmap &&
	new->threeD.bot_shadow_pixel != current->threeD.bot_shadow_pixel)
	alloc_bot_pixel = TRUE;
    if (new->threeD.be_nice_to_cmap) {
	if (alloc_top_pxmap) {
	    XtReleaseGC (gcurrent, current->threeD.top_shadow_GC);
	    AllocTopShadowGC (gnew);
	    redisplay = True;
	}
	if (alloc_bot_pxmap) {
	    XtReleaseGC (gcurrent, current->threeD.bot_shadow_GC);
	    AllocBotShadowGC (gnew);
	    redisplay = True;
	}
    } else {
	if (alloc_top_pixel) {
	    if (new->threeD.top_shadow_pxmap) {
		XFreePixmap (XtDisplay (gnew), new->threeD.top_shadow_pxmap);
		new->threeD.top_shadow_pxmap = (Pixmap) NULL;
	    }
	    XtReleaseGC (gcurrent, current->threeD.top_shadow_GC);
	    AllocTopShadowGC (gnew);
	    redisplay = True;
	}
	if (alloc_bot_pixel) {
	    if (new->threeD.bot_shadow_pxmap) {
		XFreePixmap (XtDisplay (gnew), new->threeD.bot_shadow_pxmap);
		new->threeD.bot_shadow_pxmap = (Pixmap) NULL;
	    }
	    XtReleaseGC (gcurrent, current->threeD.bot_shadow_GC);
	    AllocBotShadowGC (gnew);
	    redisplay = True;
	}
    }
   ENDMESSAGE(SetValues)
    return (redisplay);
}



/* ARGSUSED */
static void ConstraintInitialize(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    Form3dConstraints form = (Form3dConstraints)new->core.constraints;
    Form3dWidget fw = (Form3dWidget)new->core.parent;

    BEGINMESSAGE(ConstraintInitialize)

    form->form.virtual_width = (int) new->core.width;
    form->form.virtual_height = (int) new->core.height;

    if (form->form.dx == default_value)
        form->form.dx = fw->form.default_spacing;

    if (form->form.dy == default_value)
        form->form.dy = fw->form.default_spacing;

    form->form.deferred_resize = False;
    ENDMESSAGE(ConstraintInitialize)
}

/*ARGSUSED*/
static Boolean ConstraintSetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
  register Form3dConstraints cfc = (Form3dConstraints) current->core.constraints;
  register Form3dConstraints nfc = (Form3dConstraints) new->core.constraints;

  BEGINMESSAGE(ConstraintSetValues)

  if (cfc->form.top          != nfc->form.top         ||
      cfc->form.bottom       != nfc->form.bottom      ||
      cfc->form.left         != nfc->form.left        ||
      cfc->form.right        != nfc->form.right       ||
      cfc->form.dx           != nfc->form.dx          ||
      cfc->form.dy           != nfc->form.dy          ||
      cfc->form.horiz_base   != nfc->form.horiz_base  ||
      cfc->form.vert_base    != nfc->form.vert_base) {

      Form3dWidget fp = (Form3dWidget) XtParent(new);

    /* If there are no subclass ConstraintSetValues procedures remaining
     * to be invoked, and if there is no geometry request about to be
     * made, then invoke the new layout now; else defer it. */

    if (XtClass(XtParent(new))  == form3dWidgetClass	&&
	current->core.x		== new->core.x		&&
	current->core.y		== new->core.y		&&
	current->core.width	== new->core.width	&&
	current->core.height	== new->core.height	&&
	current->core.border_width == new->core.border_width)
	Layout(fp, 0, 0, True);
    else fp->form.needs_relayout = True;
  }
  ENDMESSAGE(ConstraintSetValues)
  return( FALSE );
}

static void ChangeManaged(w)
    Widget w;
{
  Form3dWidget fw = (Form3dWidget)w;
  Form3dConstraints form;
  WidgetList children, childP;
  int num_children = fw->composite.num_children;
  Widget child;

  BEGINMESSAGE(ChangeManaged)
  /*
   * Reset virtual width and height for all children.
   */
  
  for (children = childP = fw->composite.children ;
       childP - children < num_children; childP++) {
    child = *childP;
    if (XtIsManaged(child)) {
      form = (Form3dConstraints)child->core.constraints;

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
  (*((Form3dWidgetClass)w->core.widget_class)->form_class.layout)
  	                                 ((Form3dWidget) w, w->core.width, 
					  w->core.height, TRUE);
  ENDMESSAGE(ChangeManaged)
}


static XtGeometryResult PreferredGeometry( widget, request, reply  )
    Widget widget;
    XtWidgetGeometry *request, *reply;
{
    Form3dWidget w = (Form3dWidget)widget;

    BEGINMESSAGE(PreferredGeometry)
    reply->width = w->form.preferred_width;
    reply->height = w->form.preferred_height;
    reply->request_mode = CWWidth | CWHeight;
    if (  request->request_mode & (CWWidth | CWHeight) ==
	    (CWWidth | CWHeight)
	  && request->width == reply->width
	  && request->height == reply->height)
       {
        ENDMESSAGE(PreferredGeometry)
	return XtGeometryYes;
    }
    else {
       if (reply->width == w->core.width && reply->height == w->core.height) {
          ENDMESSAGE(PreferredGeometry)
	  return XtGeometryNo;
       }
       else {
          ENDMESSAGE(PreferredGeometry)
	  return XtGeometryAlmost;
       }
    }
}


/**********************************************************************
 *
 * Public routines
 *
 **********************************************************************/

/* 
 * Set or reset figuring (ignored if not realized)
 */

void
#if NeedFunctionPrototypes
XawForm3dDoLayout(Widget w,
#if NeedWidePrototypes
		int doit)
#else
		Boolean doit)
#endif
#else
XawForm3dDoLayout(w, doit)
Widget w;
Boolean doit;
#endif
{
    register Widget *childP;
    register Form3dWidget fw = (Form3dWidget)w;
    register int num_children = fw->composite.num_children;
    register WidgetList children = fw->composite.children;

    if ( ((fw->form.no_refigure = !doit) == TRUE) || !XtIsRealized(w) )
    return;

    for (childP = children; childP - children < num_children; childP++) {
	register Widget w = *childP;
	if (XtIsManaged(w)) {
	    Form3dConstraints form = (Form3dConstraints)w->core.constraints;

	    /*
	     * Xt Configure widget is too smart, and optimizes out
	     * my changes.
	     */

	    XMoveResizeWindow(XtDisplay(w), XtWindow(w),
			      w->core.x, w->core.y, 
			      w->core.width, w->core.height);

	    if (form->form.deferred_resize &&
		XtClass(w)->core_class.resize != (XtWidgetProc) NULL) {
		(*(XtClass(w)->core_class.resize))(w);
		form->form.deferred_resize = False;
	    }
	}
    }
}

static void Destroy (w)
     Widget w;
{
    Form3dWidget tdw = (Form3dWidget) w;
    XtReleaseGC (w, tdw->threeD.top_shadow_GC);
    XtReleaseGC (w, tdw->threeD.bot_shadow_GC);
    if (tdw->threeD.top_shadow_pxmap)
	XFreePixmap (XtDisplay (w), tdw->threeD.top_shadow_pxmap);
    if (tdw->threeD.bot_shadow_pxmap)
	XFreePixmap (XtDisplay (w), tdw->threeD.bot_shadow_pxmap);
}


static void Redisplay (w, event, region)
    Widget w;
    XEvent *event;		/* unused */
    Region region;		/* unused */
{
    _Form3dDrawShadows (w, event, region, True);
}


static void
_Form3dDrawShadows (gw, event, region, out)
    Widget gw;
    XEvent *event;
    Region region;
    Boolean out;
{
    XPoint	pt[6];
    Form3dWidget tdw = (Form3dWidget) gw;
    Dimension	s = tdw->threeD.shadow_width;
    /* 
     * draw the shadows using the core part width and height, 
     * and the threeD part shadow_width.
     *
     *	no point to do anything if the shadow_width is 0 or the
     *	widget has not been realized.
     */
    BEGINMESSAGE(_Form3dDrawShadows) 
    if((s > 0) && XtIsRealized (gw)){

	Dimension	h = tdw->core.height;
	Dimension	w = tdw->core.width;
	Dimension	wms = w - s;
	Dimension	hms = h - s;
	Display		*dpy = XtDisplay (gw);
	Window		win = XtWindow (gw);
	GC		top, bot;

    INFIMESSAGE(height,h)  INFIMESSAGE(width,w)

	if (out) {
	    top = tdw->threeD.top_shadow_GC;
	    bot = tdw->threeD.bot_shadow_GC;
	} else {
	    top = tdw->threeD.bot_shadow_GC;
	    bot = tdw->threeD.top_shadow_GC;
	}

	/* top-left shadow */
	if ((region == NULL) ||
	    (XRectInRegion (region, 0, 0, w, s) != RectangleOut) ||
	    (XRectInRegion (region, 0, 0, s, h) != RectangleOut)) {

	    pt[0].x = 0;	pt[0].y = h;
	    pt[1].x =		pt[1].y = 0;
	    pt[2].x = w;	pt[2].y = 0;
	    pt[3].x = wms;	pt[3].y = s;
	    pt[4].x =		pt[4].y = s;
	    pt[5].x = s;	pt[5].y = hms;
	    XFillPolygon (dpy, win, top, pt, 6,Complex,CoordModeOrigin);
	}

	/* bottom-right shadow */
	if ((region == NULL) ||
	    (XRectInRegion (region, 0, hms, w, s) != RectangleOut) ||
	    (XRectInRegion (region, wms, 0, s, h) != RectangleOut)) {

	    pt[0].x = 0;	pt[0].y = h;
	    pt[1].x = w;	pt[1].y = h;
	    pt[2].x = w;	pt[2].y = 0; 
	    pt[3].x = wms;	pt[3].y = s;
	    pt[4].x = wms;	pt[4].y = hms;
	    pt[5].x = s;	pt[5].y = hms; 
	    XFillPolygon (dpy, win, bot, pt,6, Complex,CoordModeOrigin);
	}
    }
    ENDMESSAGE(_Form3dDrawShadows) 
}

static void Realize (gw, valueMask, attrs)
    Widget gw;
    XtValueMask *valueMask;
    XSetWindowAttributes *attrs;
{
 /* 
  * This is necessary because Simple doesn't have a realize method
  * XtInheritRealize in the ThreeD class record doesn't work.  This
  * daisychains through Simple to the Core class realize method
  */
    (*form3dWidgetClass->core_class.superclass->core_class.realize)
	 (gw, valueMask, attrs);
}



