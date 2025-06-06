/*
 * $XFree86: mit/clients/xmag/Scale.c,v 2.0 1993/08/01 05:51:55 dawes Exp $
 * $XConsortium: Scale.c,v 1.15 91/08/26 11:00:15 gildea Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
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
 * Author:  Davor Matic, MIT X Consortium
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h> 
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>
#include "ScaleP.h"
#include <X11/Xosdefs.h>

#define myrint(x) floor(x + 0.5)

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif

#if defined(ISC) && defined(__STDC__)
#ifdef ISC30
extern double atof(const char *);
#else
extern double atof();
#endif
#endif

#define streq(a,b) (strcmp( (a), (b) ) == 0)
#define min(x, y) (x > y ? y : x)
#define max(x, y) (x < y ? y : x)

#define DefaultBufferSize 1024
#define DefaultScaleFactor NULL

#define Offset(field) XtOffsetOf(ScaleRec, scale.field)

static XtResource resources[] = {
{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(foreground_pixel), XtRString, (XtPointer) XtDefaultForeground},
{XtNgravity, XtCGravity, XtRGravity, sizeof(XtGravity),
     Offset(gravity), XtRImmediate, (XtPointer) "ForgetGravity"},
{XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
     Offset(internal_width), XtRImmediate, (XtPointer) 2},
{XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
     Offset(internal_height), XtRImmediate, (XtPointer) 2},
{XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
     Offset(resize), XtRImmediate, (XtPointer) True},
{XtNautoscale, XtCAutoscale, XtRBoolean, sizeof(Boolean),
     Offset(autoscale), XtRImmediate, (XtPointer) True},
{XtNproportional, XtCProportional, XtRBoolean, sizeof(Boolean),
     Offset(proportional), XtRImmediate, (XtPointer) True},
{XtNscaleX, XtCScaleFactor, XtRString, sizeof(String),
     Offset(scale_x_str), XtRImmediate, (XtPointer) DefaultScaleFactor},
{XtNscaleY, XtCScaleFactor, XtRString, sizeof(String),
     Offset(scale_y_str), XtRImmediate, (XtPointer) DefaultScaleFactor},
{XtNaspectRatio, XtCAspectRatio, XtRString, sizeof(String),
     Offset(aspect_ratio_str), XtRImmediate, (XtPointer) "1.0"},
{XtNprecision, XtCPrecision, XtRString, sizeof(String),
     Offset(precision_str), XtRImmediate, (XtPointer) "0.001"},
{XtNimage, XtCImage, XtRImage, sizeof(XImage*),
     Offset(image), XtRImmediate, (XtPointer) NULL},
{XtNpasteBuffer, XtCPasteBuffer, XtRBoolean, sizeof(Boolean),
     Offset(paste_buffer), XtRImmediate, (XtPointer) False},
{XtNbufferSize, XtCBufferSize, XtRCardinal, sizeof(Cardinal),
     Offset(buffer_size), XtRImmediate, (XtPointer) DefaultBufferSize},
{XtNuserData, XtCuserData, XtRuserData, sizeof(XtPointer),
     Offset(userData), XtRImmediate, (XtPointer) NULL},
{ XtNvisual, XtCvisual, XtRVisual, sizeof(Visual*),
     Offset(visual), XtRImmediate, CopyFromParent}
};

#undef Offset

static void ClassInitialize();
static void Initialize();
static void Realize();
static void Redisplay();
static void Resize();
static void Destroy();
static Boolean SetValues();

void SWUnscale();
void SWAutoscale();
void SWInitialSize();
void RequestSelection();
void GrabSelection();
 
static XtActionsRec actions[] =
{
{"unscale", SWUnscale},
{"autoscale", SWAutoscale},
{"initial-size", SWInitialSize},
{"paste", RequestSelection},
{"cut", GrabSelection}
};

static char translations[] =
"\
 <Key>u:           unscale()\n\
 <Key>a:           autoscale()\n\
 <Key>i:           initial-size()\n\
";

ScaleClassRec scaleClassRec = {
{   /* core fields */
    /* superclass		*/	(WidgetClass) &simpleClassRec,
    /* class_name		*/	"Scale",
    /* widget_size		*/	sizeof(ScaleRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/      XtExposeCompressMaximal|
                                        XtExposeGraphicsExposeMerged,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	TRUE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/      translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { 
    /* change_sensitive		*/	XtInheritChangeSensitive,
  }
};
 
WidgetClass scaleWidgetClass = (WidgetClass) &scaleClassRec;
    


/*
 * Private Procedures
 */


static void ClassInitialize()
{
} 



void GetGC(sw)
    ScaleWidget sw;
{
    XGCValues values;
    
    values.foreground = sw->scale.foreground_pixel;
    values.background = sw->core.background_pixel;
    values.function = GXcopy;
    
    sw->scale.gc = XtGetGC((Widget) sw,
			   GCForeground | 
			   GCBackground | 
			   GCFunction,
			   &values);
}



void Proportional();

void GetInitialScaleValues(sw)
    ScaleWidget sw;
{
    if (sw->scale.proportional) {
	sw->scale.scale_x = sw->scale.scale_y =
	    ((sw->scale.aspect_ratio > 1.0) ?
	     sw->scale.aspect_ratio : 1.0 / sw->scale.aspect_ratio) *
		 (sw->scale.precision > 1.0 ?
		  sw->scale.precision : 1.0);
	Proportional(sw); /* need to cut them down to proper values */
    }
    else
	sw->scale.scale_x = sw->scale.scale_y = 1.0;
}



void GetRectangleBuffer(sw, buffer_size)
    ScaleWidget sw;
    Cardinal buffer_size;
    /*
     * This procedure will realloc a new rectangles buffer.
     * If the new buffer size is less than nrectangles, some
     * information will be lost.
     */
{
    if (buffer_size == 0) {
	buffer_size = DefaultBufferSize;
	XtWarning("buffer size has to be a positive number greater than zero");
    }
    sw->scale.rectangles = (XRectangle *)
	XtRealloc((char *) sw->scale.rectangles, 
		  buffer_size * sizeof(XRectangle));
    sw->scale.buffer_size = buffer_size;
}



/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
    ScaleWidget new_sw = (ScaleWidget) new;
    
    new_sw->scale.table.x = (Position *) NULL;
    new_sw->scale.table.y = (Position *) NULL;
    new_sw->scale.table.width = (Dimension *) NULL;
    new_sw->scale.table.height = (Dimension *) NULL;
    
    new_sw->scale.nrectangles = 0;
    new_sw->scale.rectangles = (XRectangle *) NULL;
    
    GetRectangleBuffer(new_sw, new_sw->scale.buffer_size);
    
    GetGC(new_sw);
    
    if (new_sw->scale.image != NULL) {
	if (new_sw->core.width == 0)
	    new_sw->core.width = 
		new_sw->scale.image->width + 2 * new_sw->scale.internal_width;
	if (new_sw->core.height == 0)
	    new_sw->core.height = 
		new_sw->scale.image->height + 2 *new_sw->scale.internal_height;
    }
    else {
	if (new_sw->core.width == 0)
	    new_sw->core.width = 1 + 2 * new_sw->scale.internal_width;
	if (new_sw->core.height == 0)
	    new_sw->core.height = 1 + 2 * new_sw->scale.internal_height;
	new_sw->scale.image = XCreateImage(XtDisplay(new),
				       DefaultVisual(XtDisplay(new), 
					        DefaultScreen(XtDisplay(new))),
				       1, XYBitmap, 0, 
				       XtCalloc(1, sizeof(char)),
				       1, 1, 8, 0);
    }

    if ((new_sw->scale.aspect_ratio = 
	 atof(new_sw->scale.aspect_ratio_str)) < 0.0) {
	new_sw->scale.aspect_ratio = 1.0;
	XtWarning("AspectRatio has to be a positive number. (forced to 1.0)");
    }

    if ((new_sw->scale.precision =
         atof(new_sw->scale.precision_str)) < 0.0) {
	new_sw->scale.precision = 0.001;
	XtWarning("Precision has to be a positive number. (forced to 0.001)");
    }
    
    if (new_sw->scale.scale_x_str == DefaultScaleFactor
	|| 
	new_sw->scale.scale_y_str == DefaultScaleFactor)
	GetInitialScaleValues(new_sw);
    else {
	if ((new_sw->scale.scale_x = 
	     atof(new_sw->scale.scale_x_str)) < 0.0) {
	    new_sw->scale.scale_x = 1.0;
	    XtWarning("ScaleValue has to be a positive number. (forced to 1.0)");
	}
	if ((new_sw->scale.scale_y = 
	     atof(new_sw->scale.scale_y_str)) < 0.0) {
	    new_sw->scale.scale_y = 1.0;
	    XtWarning("ScaleValue has to be a positive number. (forced to 1.0)");
	}
    }
}



void BuildTable(sw)
    ScaleWidget sw;
    /* 
     * This procedure builds scaling table for image in the scale struct
     * Requires image, scale_x and scale_y to be set properly
     */
{
    Position x, y;
    
    XtFree((char *) sw->scale.table.x);
    XtFree((char *) sw->scale.table.y);
    XtFree((char *) sw->scale.table.width);
    XtFree((char *) sw->scale.table.height);
    sw->scale.table.x = 
	(Position *) XtMalloc(sizeof(Position) * sw->scale.image->width);
    sw->scale.table.y = 
	(Position *) XtMalloc(sizeof(Position) * sw->scale.image->height);
    sw->scale.table.width = 
	(Dimension *) XtMalloc(sizeof(Dimension) * sw->scale.image->width);
    sw->scale.table.height = 
	(Dimension *) XtMalloc(sizeof(Dimension) * sw->scale.image->height);
    
    /* Build the scaling table */	
    for (x = 0; x < sw->scale.image->width; x++) {
	sw->scale.table.x[(int) x] = (Position) myrint(sw->scale.scale_x * x);
	sw->scale.table.width[(int) x] = (Dimension)
	    myrint(sw->scale.scale_x *(x + 1)) - myrint(sw->scale.scale_x * x);
    }
    for (y = 0; y < sw->scale.image->height; y++) {
	sw->scale.table.y[(int) y] = (Position) myrint(sw->scale.scale_y * y);
	sw->scale.table.height[(int) y] = (Dimension)
	    myrint(sw->scale.scale_y *(y + 1)) - myrint(sw->scale.scale_y * y);
    }
}



void FlushRectangles(sw, drawable, gc)
    ScaleWidget sw;
    Drawable drawable;
    GC gc;
{
    XFillRectangles(XtDisplay(sw), drawable, gc, 
		    sw->scale.rectangles, sw->scale.nrectangles);
    
    sw->scale.nrectangles = 0;
}



void FillRectangle(sw, drawable, gc, x, y, width, height)
    ScaleWidget sw;
    Drawable drawable;
    GC gc;
    Position x, y;
    Dimension width, height;
{
    
    if (sw->scale.nrectangles == sw->scale.buffer_size)
	FlushRectangles(sw, drawable, gc);

    sw->scale.rectangles[(int) sw->scale.nrectangles].x = x;
    sw->scale.rectangles[(int) sw->scale.nrectangles].y = y;
    sw->scale.rectangles[(int) sw->scale.nrectangles].width = width;
    sw->scale.rectangles[(int) sw->scale.nrectangles].height = height;

    ++sw->scale.nrectangles;
}



void ScaleImage(sw, drawable, img_x, img_y, dst_x, dst_y, img_width,img_height)
    ScaleWidget sw;
    Drawable drawable;
    Position img_x, img_y;
    Position dst_x, dst_y;
    Dimension img_width, img_height;
    /* 
     * This procedure scales image into the specified drawable
     * It assumes scaling table was already built
     */
{
    GC gc;
    XGCValues gcv;
    Position x, y;
    Pixel pixel;
    
    /* Clip the img coordinates */
    img_x = min(max(img_x, 0), (Position) sw->scale.image->width - 1);
    img_y = min(max(img_y, 0), (Position) sw->scale.image->height - 1);
    img_width = 
      min(img_width, (Dimension)(sw->scale.image->width - (Dimension)img_x));
    img_height = 
      min(img_height, (Dimension)(sw->scale.image->height - (Dimension)img_y));

    if (sw->scale.scale_x == 1.0 && sw->scale.scale_y == 1.0) 
	XPutImage(XtDisplay(sw), drawable, sw->scale.gc, sw->scale.image, 
		  img_x, img_y, dst_x, dst_y, 
		  img_width, img_height);
    else {
	dst_x = dst_x - sw->scale.table.x[(int) img_x];
	dst_y = dst_y - sw->scale.table.y[(int) img_y];

	gc = XCreateGC(XtDisplay(sw), drawable, 0, NULL);

	gcv.function = GXcopy;
	XChangeGC(XtDisplay(sw), gc, GCFunction, &gcv);

	/* make sure gc knows the right background */
	gcv.background = sw->core.background_pixel;
	XChangeGC(XtDisplay(sw), gc, GCBackground, &gcv);	

	/* Set the background of drawable.  If the most frequent color
	   is the background color it can speed up scaling process. */
	gcv.foreground = gcv.background;
	XChangeGC(XtDisplay(sw), gc, GCForeground, &gcv);
	XFillRectangle(XtDisplay(sw), drawable, gc, 
		       sw->scale.table.x[(int) img_x] + dst_x, 
		       sw->scale.table.y[(int) img_y] + dst_y, 
		       sw->scale.table.x[(int) img_x + img_width - 1] - 
		       sw->scale.table.x[(int) img_x], 
		       sw->scale.table.y[(int) img_y + img_height - 1] - 
		       sw->scale.table.y[(int) img_y]);
	
	if (sw->scale.image->format == XYBitmap) {
	    for (x = img_x; x < img_x + (Position)img_width; x++)
		for (y = img_y; y < img_y + (Position)img_height; y++) {
		    pixel = XGetPixel(sw->scale.image, x, y);
		    if (pixel) /* Do not draw background */
			FillRectangle(sw, drawable, sw->scale.gc, 
				      sw->scale.table.x[(int) x] + dst_x, 
				      sw->scale.table.y[(int) y] + dst_y, 
				      sw->scale.table.width[(int) x], 
				      sw->scale.table.height[(int) y]);
		}
	    FlushRectangles(sw, drawable, sw->scale.gc);
	}
	else {
	    for (x = img_x; x < img_x + (Position)img_width; x++)
		for (y = img_y; y < img_y + (Position)img_height; y++) {
		    pixel = XGetPixel(sw->scale.image, x, y);
		    if (pixel != gcv.background) { /* Do not draw background */
			if (gcv.foreground != pixel) { /* Change fg to pixel */
			    gcv.foreground = pixel;
			    XChangeGC(XtDisplay(sw), gc, GCForeground, &gcv);
			}
			XFillRectangle(XtDisplay(sw), drawable, gc,
				       sw->scale.table.x[(int) x] + dst_x, 
				       sw->scale.table.y[(int) y] + dst_y, 
				       sw->scale.table.width[(int) x], 
				       sw->scale.table.height[(int) y]);
		    }
		}
	}
	XFreeGC(XtDisplay(sw), gc);
    }
}



int FindPixel(sw, x, y, img_x, img_y, img_pixel)
    ScaleWidget sw;
    Position x, y; /* (x,y) == (0,0) where image starts in sw window*/
    Position *img_x, *img_y;
    Pixel    *img_pixel;
{
    if (*img_x < 0 || *img_x >= sw->scale.image->width 
	||
	*img_y < 0 || *img_y >= sw->scale.image->height)
	return (-1);
    
    if (sw->scale.table.x[(int) *img_x] >= x) {
	--*img_x;
	return FindPixel(sw, x, y, img_x, img_y, img_pixel);
    }
    if (sw->scale.table.x[(int) *img_x] + 
	(Position)sw->scale.table.width[(int) *img_x] < x) {
	++*img_x;
	return FindPixel(sw, x, y, img_x, img_y, img_pixel);
    }
    if (sw->scale.table.y[(int) *img_y] >= y) {
	--*img_y;
	return FindPixel(sw, x, y, img_x, img_y, img_pixel);
    }
    if (sw->scale.table.y[(int) *img_y] + 
	(Position)sw->scale.table.height[(int) *img_y] < y) {
	++*img_y;
	return FindPixel(sw, x, y, img_x, img_y, img_pixel);
    }
    
    *img_pixel = XGetPixel(sw->scale.image, *img_x, *img_y);
    
    return (0);
}



int SWGetImagePixel(w, x, y, img_x, img_y, img_pixel)
    Widget w;
    Position x, y;
    Position *img_x, *img_y;
    Pixel    *img_pixel;
{
    ScaleWidget sw = (ScaleWidget) w;
    
    x -= sw->scale.x;
    y -= sw->scale.y;
    
    *img_x = (Position) floor(x / sw->scale.scale_x);
    *img_y = (Position) floor(y / sw->scale.scale_y);
    
    return FindPixel(sw, x, y, img_x, img_y, img_pixel);
}



/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    ScaleWidget sw = (ScaleWidget) w;
    Position  x, y, img_x, img_y;
    Dimension width, height;

    if (event->type == Expose) {

	if (event->xexpose.x < sw->scale.x) {
	    x = 0;
	    width = event->xexpose.width -
		(sw->scale.x - event->xexpose.x);
	}
	else {
	    x = event->xexpose.x - sw->scale.x;
	    width = event->xexpose.width;
	}
	
	if (event->xexpose.y < sw->scale.y) {
	    y = 0;
	    height = event->xexpose.height -
		(sw->scale.y - event->xexpose.y);
	}
	else {
	    y = event->xexpose.y - sw->scale.y;
	    height = event->xexpose.height;
	}
	
	img_x = min(max((Position) floor((float) x 
					 / (float) sw->scale.scale_x), 0),
		    (Position) sw->scale.image->width - 1);

	img_y = min(max((Position) floor((float) y
					 / (float) sw->scale.scale_y), 0),
		    (Position) sw->scale.image->height - 1);
	
	if (sw->core.visible) {  
	    ScaleImage(sw, XtWindow(w),
		       img_x, img_y,
		       sw->scale.x + sw->scale.table.x[(int) img_x],
		       sw->scale.y + sw->scale.table.y[(int) img_y],
		       (Dimension) ceil((float) width
					/ sw->scale.scale_x) + 1, 
		       (Dimension) ceil((float) height
					/ sw->scale.scale_y) + 1);
	}
    }
}



void TryResize(sw)
    ScaleWidget sw;
{
    Dimension width, height;
    XtGeometryResult result;

    width = (Dimension) 
	floor(sw->scale.image->width * sw->scale.scale_x)
	    + 2 * sw->scale.internal_width;
    height = (Dimension) 
	floor(sw->scale.image->height * sw->scale.scale_y)
	    + 2 * sw->scale.internal_height;
    
    while ((result =   
/* SUPPRESS 530 */XtMakeResizeRequest((Widget)sw,width,height,&width,&height))
	   == XtGeometryAlmost);  
    
    if (result != XtGeometryNo) {
	sw->core.width = width;
	sw->core.height = height;
    }
}



void Precision(sw)
    ScaleWidget sw;
{
    if (sw->scale.scale_x != 1.0)
	sw->scale.scale_x = floor(sw->scale.scale_x / sw->scale.precision)
	    		    * sw->scale.precision;

    if (sw->scale.scale_y != 1.0)
	sw->scale.scale_y = floor(sw->scale.scale_y / sw->scale.precision)
			    * sw->scale.precision;
}



void Proportional(sw)
    ScaleWidget sw;
{
    float scale_x, scale_y;

    scale_x = sw->scale.scale_y / sw->scale.aspect_ratio;
    scale_y = sw->scale.scale_x * sw->scale.aspect_ratio;
    
    if (scale_x <= sw->scale.scale_x && scale_y <= sw->scale.scale_y) {
	if (scale_x > scale_y)
	    sw->scale.scale_x = scale_x;
	else
	    sw->scale.scale_y = scale_y;
    }
    else if (scale_x <= sw->scale.scale_x)
	sw->scale.scale_x = scale_x;
    else if (scale_y <= sw->scale.scale_y)
	sw->scale.scale_y = scale_y;
    else {
	float x_ratio, y_ratio;
	
	x_ratio = scale_x / sw->scale.scale_x;
	y_ratio = scale_y / sw->scale.scale_y;
	
	if (x_ratio < y_ratio)
	    sw->scale.scale_y /= x_ratio;
	else
	    sw->scale.scale_x /= y_ratio;
    }

    if (fabs(sw->scale.scale_x / sw->scale.scale_y * sw->scale.aspect_ratio 
	    - 1.0) > sw->scale.precision)
	XtWarning("can not preserve aspect ratio");
}



void GetScaledSize(sw)
    ScaleWidget sw;
{
    sw->scale.width = (Dimension)
	max(myrint(sw->scale.scale_x * sw->scale.image->width), 1);
    sw->scale.height = (Dimension)
	max(myrint(sw->scale.scale_y * sw->scale.image->height), 1);
}



void GetScaleValues(sw)
    ScaleWidget sw;
{
    /*
     * Make sure to subtract internal width and height.
     */
    
    sw->scale.scale_x = 
	(float) max((int)(sw->core.width - 2 * sw->scale.internal_width), 1)
	    / (float) sw->scale.image->width;

    sw->scale.scale_y =
	(float) max((int)(sw->core.height - 2 * sw->scale.internal_height), 1)
	    / (float) sw->scale.image->height;
}



void Unscale(sw)
    ScaleWidget sw;
{
    sw->scale.scale_x = sw->scale.scale_y = 1.0;
 
    GetScaledSize(sw);		
    
    BuildTable(sw);
}



void Autoscale(sw)
    ScaleWidget sw;
{
    GetScaleValues(sw);

    if (sw->scale.proportional) Proportional(sw);

    Precision(sw);

    GetScaledSize(sw);
    
    BuildTable(sw);
}



void PositionImage(sw)
    ScaleWidget sw;
{
    /*
     * Set as if for ForgegGravity (that is center the image)
     */
    sw->scale.x = (Position) 
	(sw->core.width - sw->scale.width) / 2;
    sw->scale.y = (Position)
	(sw->core.height - sw->scale.height) / 2;
    
/*****
    if (sw->scale.gravity & WestGravity) { 
    }
    if (sw->scale.gravity & EastGravity) { 
    }
    if (sw->scale.gravity & NorthGravity) {
    }
    if (sw->scale.gravity & SouthGravity) {
    }
*****/
}



static void Resize(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w;
    
    if (sw->scale.autoscale) Autoscale(sw);
    
    PositionImage(sw);
}



static void Realize(wid, vmask, attr)
        Widget wid;
        Mask *vmask;
        XSetWindowAttributes *attr;
{
  ScaleWidget sw = (ScaleWidget) wid;
  XtCreateWindow(wid, (unsigned int) InputOutput,
		 (Visual *) sw->scale.visual, *vmask, attr);
}
 


static void Destroy(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w; 

    XtFree((char *) sw->scale.table.x);
    XtFree((char *) sw->scale.table.y);
    XtFree((char *) sw->scale.table.width);
    XtFree((char *) sw->scale.table.height);
    
    XtFree((char *) sw->scale.rectangles);

    XtReleaseGC(w, sw->scale.gc);

    XDestroyImage(sw->scale.image);
}



/* ARGSUSED */
static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    ScaleWidget cur_sw = (ScaleWidget) current;
    /* ScaleWidget req_sw = (ScaleWidget) request; */
    ScaleWidget new_sw = (ScaleWidget) new;
    Boolean redisplay = False;
    int i;
    
    for (i = 0; i < *num_args; i++) {
	if (streq(XtNbackground, args[i].name)) {
	    XSetBackground(XtDisplay(new), new_sw->scale.gc, 
			   new_sw->core.background_pixel);
	}
	if (streq(XtNforeground, args[i].name)) {
	    XSetForeground(XtDisplay(new), new_sw->scale.gc, 
			   new_sw->scale.foreground_pixel);
	}
	if (streq(XtNimage, args[i].name)) {
	    XDestroyImage(cur_sw->scale.image);
	    if (new_sw->scale.image == NULL)
		new_sw->scale.image = XCreateImage(XtDisplay(new),
						DefaultVisual(XtDisplay(new), 
						DefaultScreen(XtDisplay(new))),
						   1, XYBitmap, 0, 
						   XtCalloc(1, sizeof(char)),
						   1, 1, 8, 0);
	    else
		new_sw->scale.image = 
		    XSubImage(new_sw->scale.image, 
			      0, 0, 
			      new_sw->scale.image->width, 
			      new_sw->scale.image->height);

	    if (new_sw->scale.resize)
		TryResize(new_sw);
	    if (new_sw->scale.autoscale)
		Autoscale(new_sw);
	    else {
		GetScaledSize(new_sw);
		BuildTable(new_sw);
	    }
	    PositionImage(new_sw);
	    redisplay = True;
	}

	if (streq(XtNuserData, args[i].name)) 
	  new_sw->scale.userData = (XtPointer)args[i].value;

	if (streq(XtNbufferSize, args[i].name)) {
	    if (new_sw->scale.buffer_size != cur_sw->scale.buffer_size) {
		GetRectangleBuffer(new_sw, new_sw->scale.buffer_size);
	    }
	}

	if (streq(XtNaspectRatio, args[i].name)) {
	    if ((new_sw->scale.aspect_ratio = 
		 atof(new_sw->scale.aspect_ratio_str)) < 0.0) {
		new_sw->scale.aspect_ratio = cur_sw->scale.aspect_ratio;
		XtWarning("AspectRatio has to be a positive number.");
	    }
	    else if (new_sw->scale.aspect_ratio != cur_sw->scale.aspect_ratio){
		if (new_sw->scale.proportional) {
		    Proportional(new_sw);
		    Precision(new_sw);
		    GetScaledSize(new_sw);
		    BuildTable(new_sw);
		    PositionImage(new_sw);
		    redisplay = True;
		}
	    }
	}
	
	if (streq(XtNproportional, args[i].name)) {
	    if (new_sw->scale.proportional != cur_sw->scale.proportional) {
		if (new_sw->scale.proportional) Proportional(new_sw);
		Precision(new_sw);
		GetScaledSize(new_sw);
		BuildTable(new_sw);
		PositionImage(new_sw);
		redisplay = True;
	    }
	}
	
	if (streq(XtNscaleX, args[i].name)
	    || 
	    streq(XtNscaleY, args[i].name)) {
	    if (new_sw->scale.scale_x_str == DefaultScaleFactor
		|| 
		new_sw->scale.scale_y_str == DefaultScaleFactor)
		GetInitialScaleValues(new_sw);
	    else {
		if ((new_sw->scale.scale_x = 
		     atof(new_sw->scale.scale_x_str)) < 0.0) {
		    new_sw->scale.scale_x = cur_sw->scale.scale_x;
		    XtWarning("ScaleValue has to be a positive number.");
		}
		if ((new_sw->scale.scale_y = 
		     atof(new_sw->scale.scale_y_str)) < 0.0) {
		    new_sw->scale.scale_y = cur_sw->scale.scale_y;
		    XtWarning("ScaleValue has to be a positive number.");
		}
	    }
	    if (new_sw->scale.scale_x != cur_sw->scale.scale_x
		||
		new_sw->scale.scale_y != cur_sw->scale.scale_y) {
		
		/*?*?*?*?*?*?*?*?*?*?*?*?*?*?**?*?*?*?*?*?*?*?*?***?*/
		fprintf(stderr, "================>>%f %f\n",
			new_sw->scale.scale_x, new_sw->scale.scale_y);
		
		if (new_sw->scale.proportional) Proportional(new_sw);
		Precision(new_sw);
		if (new_sw->scale.resize)
		    TryResize(new_sw);
		GetScaledSize(new_sw);		
		BuildTable(new_sw);
		PositionImage(new_sw);
		redisplay = True;
	    }
	}
	
	if (streq(XtNprecision, args[i].name)) {
	    if ((new_sw->scale.precision = 
		 atof(new_sw->scale.precision_str)) < 0.0) {
		new_sw->scale.precision = cur_sw->scale.precision;
		XtWarning("Precision has to be a positive number.");
	    }
	    if (new_sw->scale.precision != cur_sw->scale.precision) {
		if (new_sw->scale.proportional) Proportional(new_sw);
		Precision(new_sw);
		GetScaledSize(new_sw);		
		BuildTable(new_sw);
		PositionImage(new_sw);
		redisplay = True;
	    }
	}
    }		    
    return(redisplay);
}



void SWUnscale(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w;

    Unscale(sw);
    PositionImage(sw);
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
}



void SWAutoscale(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w;

    Autoscale(sw);
    PositionImage(sw);
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
}



void SWInitialSize(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w;
    
    GetInitialScaleValues(sw);

    if (sw->scale.proportional) Proportional(sw);
    Precision(sw);
    if (sw->scale.resize)
	TryResize(sw);
    GetScaledSize(sw);		
    BuildTable(sw);
    PositionImage(sw);
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
}



void SWSetImage(w, image)
    Widget w;
    XImage *image;
{
    int n;
    Arg wargs[2];
    
    n = 0;
    XtSetArg(wargs[n], XtNimage, (XtArgVal) image); n++;
    XtSetValues(w, wargs, n);
}



extern void SWRequestSelection();

void RequestSelection(w, event)
    Widget w;
    XEvent *event;
{
    SWRequestSelection(w, event->xbutton.time);
}



extern void SWGrabSelection();

void GrabSelection(w, event)
    Widget w;
    XEvent *event;
{
    SWGrabSelection(w, event->xbutton.time);
}



Pixmap SWGetPixmap(w)
    Widget w;
{
    ScaleWidget sw = (ScaleWidget) w;
    Pixmap pixmap;

    pixmap = XCreatePixmap(XtDisplay(w), XtWindow(w), 
			   sw->scale.width, 
			   sw->scale.height, 
			   sw->scale.image->depth);
    ScaleImage(sw, pixmap, 
	       0, 0, 0, 0, 
	       (Dimension) sw->scale.image->width, 
	       (Dimension) sw->scale.image->height);
    
    return(pixmap);
}
