/*
 * $XConsortium: Clock.c,v 1.24 91/05/22 17:20:31 converse Exp $
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
 */

/*
 * Clock.c
 *
 * a NeWS clone clock
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#ifdef VMS
#include <XMU/Converters.h>
#include <X11/shape.h>
#endif
#ifndef VMS
#include <X11/Xmu/Converters.h>
#include <X11/extensions/shape.h>
#endif
#include "ClockP.h"
#include <X11/Xos.h>
#include <stdio.h>
#include <math.h>
#ifdef VMS
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif
#endif


#ifdef X_NOT_STDC_ENV
extern struct tm *localtime();
#endif

#define offset(field) XtOffsetOf(ClockRec, clock.field)
#define goffset(field) XtOffsetOf(WidgetRec, core.field)

static XtResource resources[] = {
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
        goffset(width), XtRImmediate, (XtPointer) 120},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	goffset(height), XtRImmediate, (XtPointer) 120},
    {XtNminute, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(minute), XtRString, XtDefaultForeground},
    {XtNhour, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(hour), XtRString, XtDefaultForeground},
    {XtNjewel, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(jewel), XtRString, XtDefaultForeground},

    {XtNnight, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(night), XtRString, XtDefaultForeground},
    {XtNsky, XtCBackground, XtRPixel, sizeof (Pixel),
	offset(sky), XtRString, XtDefaultBackground},
    {XtNmoon, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(moon), XtRString, XtDefaultForeground},
    {XtNstars, XtCForeground, XtRPixel, sizeof (Pixel),
	offset(stars), XtRString, XtDefaultForeground},
	
    {XtNhourTZAdjust, "TimeZone", XtRInt, sizeof(int),
        offset(hour_zone_adjust), XtRInt, 0},

    {XtNnightStart, "NightStart", XtRInt, sizeof(int),
        offset(night_start), XtRInt, 0},
    {XtNmoonClock, "MoonClock", XtRBoolean, sizeof(Boolean),
    	offset(moonclock), XtRBoolean, False},        
        
    {XtNbackingStore, XtCBackingStore, XtRBackingStore, sizeof (int),
    	offset (backing_store), XtRString, "default"},
    {XtNborderSize, XtCBorderSize, XtRFloat, sizeof (float),
	offset (border_size), XtRString, "0.1"},
    {XtNjewelSize, XtCBorderSize, XtRFloat, sizeof (float),
	offset (jewel_size), XtRString, "0.0"},
    {XtNshapeWindow, XtCShapeWindow, XtRBoolean, sizeof (Boolean),
	offset (shape_window), XtRImmediate, (XtPointer) True},
    {XtNtransparent, XtCTransparent, XtRBoolean, sizeof (Boolean),
	offset (transparent), XtRImmediate, (XtPointer) False},
};

#undef offset
#undef goffset

static void 	new_time();

static void Initialize(), Realize(), Destroy(), Redisplay(), Resize();

# define BORDER_SIZE(w)    ((w)->clock.border_size)
# define WINDOW_WIDTH(w)    (2.0 - BORDER_SIZE(w)*2)
# define WINDOW_HEIGHT(w)   (2.0 - BORDER_SIZE(w)*2)
# define MINUTE_WIDTH(w)    (0.05)
# define HOUR_WIDTH(w)	    (0.05)
# define JEWEL_SIZE(w)	    ((w)->clock.jewel_size)
# define MINUTE_LENGTH(w)   (JEWEL_Y(w) - JEWEL_SIZE(w) / 2.0)
# define HOUR_LENGTH(w)	    (MINUTE_LENGTH(w) * 0.6)
# define JEWEL_X(w)	    (0.0)
# define JEWEL_Y(w)	    (1.0 - (BORDER_SIZE(w) + JEWEL_SIZE(w)))

static void ClassInitialize();

ClockClassRec clockClassRec = {
    { /* core fields */
    /* superclass		*/	&widgetClassRec,
    /* class_name		*/	"Clock",
    /* size			*/	sizeof(ClockRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
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
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    }
};

static void ClassInitialize()
{
    XtAddConverter( XtRString, XtRBackingStore, XmuCvtStringToBackingStore,
		    NULL, 0 );
}

WidgetClass clockWidgetClass = (WidgetClass) &clockClassRec;

/* ARGSUSED */
static void Initialize (greq, gnew)
    Widget greq, gnew;
{
    ClockWidget w = (ClockWidget)gnew;
    XtGCMask	valuemask;
    XGCValues	myXGCV;
    int shape_event_base, shape_error_base;

    valuemask = GCForeground;

    if (w->clock.transparent)
    {
	;
    }
    else
    {
    	myXGCV.foreground = w->clock.minute;
    	w->clock.minuteGC = XtGetGC(gnew, valuemask, &myXGCV);

    	myXGCV.foreground = w->clock.hour;
    	w->clock.hourGC = XtGetGC(gnew, valuemask, &myXGCV);

    	myXGCV.foreground = w->clock.jewel;
    	w->clock.jewelGC = XtGetGC(gnew, valuemask, &myXGCV);
    
    	myXGCV.foreground = w->core.background_pixel;
    	w->clock.eraseGC = XtGetGC(gnew, valuemask, &myXGCV);

    	if(w->clock.moonclock)
    	{
    		w->clock.skyGC = (GC) 0;
    		
    		myXGCV.foreground = w->clock.moon;
    		w->clock.moonGC = XtGetGC(gnew, valuemask, &myXGCV);
    		
    		myXGCV.foreground = w->clock.stars;
    		w->clock.starsGC = XtGetGC(gnew, valuemask, &myXGCV);
    	}
    	
    }

    /* wait for Realize to add the timeout */
    w->clock.interval_id = 0;

    if (w->clock.shape_window && !XShapeQueryExtension (XtDisplay (w), 
							&shape_event_base, 
							&shape_error_base))
      w->clock.shape_window = False;

    w->clock.shape_mask = 0;
    w->clock.shapeGC = 0;
    w->clock.shape_width = 0;
    w->clock.shape_height = 0;
    w->clock.polys_valid = 0;
    w->clock.day = w->core.border_pixel;
}

static void Resize (w)
    ClockWidget	w;
{
    XGCValues	xgcv;
    Widget	parent;
    XWindowChanges	xwc;
    int		face_width, face_height;
    int		x, y;
    Pixmap	shape_mask;

    if (!XtIsRealized((Widget) w))
	return;

    /*
     * compute desired border size
     */

    SetTransform (&w->clock.maskt,
		  0, w->core.width,
		  w->core.height, 0,
		  -1.0, 1.0,
 		  -1.0, 1.0);

    face_width = abs (Xwidth (BORDER_SIZE(w), BORDER_SIZE(w), &w->clock.maskt));
    face_height = abs (Xheight (BORDER_SIZE(w), BORDER_SIZE(w), &w->clock.maskt));

    /*
     *  shape the windows and borders
     */

    if (w->clock.shape_window) {

	SetTransform (&w->clock.t,
			face_width, w->core.width - face_width,
			w->core.height - face_height, face_height,
			-WINDOW_WIDTH(w)/2, WINDOW_WIDTH(w)/2,
			-WINDOW_HEIGHT(w)/2, WINDOW_HEIGHT(w)/2);
    
	/*
	 * allocate a pixmap to draw shapes in
	 */

	if (w->clock.shape_mask &&
	    (w->clock.shape_width != w->core.width ||
	     w->clock.shape_height != w->core.height))
	{
	    XFreePixmap (XtDisplay (w), w->clock.shape_mask);
	    w->clock.shape_mask = None;
	}
	
	if (!w->clock.shape_mask)
	{
	    w->clock.shape_mask = XCreatePixmap (XtDisplay (w), XtWindow (w),
				    w->core.width, w->core.height, 1);
	}
	shape_mask = w->clock.shape_mask;
    	if (!w->clock.shapeGC)
            w->clock.shapeGC = XCreateGC (XtDisplay (w), shape_mask, 0, &xgcv);

	/* erase the pixmap */
    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 0);
    	XFillRectangle (XtDisplay (w), shape_mask, w->clock.shapeGC,
			0, 0, w->core.width, w->core.height);
    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 1);

	/*
	 * draw the bounding shape.  Doing this first
	 * eliminates extra exposure events.
	 */

	if (w->clock.border_size > 0.0 || !w->clock.transparent)
	{
	    TFillArc (XtDisplay (w), shape_mask,
			    w->clock.shapeGC, &w->clock.maskt,
			    -1.0, -1.0,
			    2.0, 2.0,
			    0, 360 * 64);
	}

	if (w->clock.transparent)
	{
	    if (w->clock.border_size > 0.0)
	    {
	    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 0);
	    	TFillArc (XtDisplay (w), shape_mask,
			    	w->clock.shapeGC, &w->clock.t,
			    	-WINDOW_WIDTH(w)/2, -WINDOW_HEIGHT(w)/2,
			    	WINDOW_WIDTH(w), WINDOW_HEIGHT(w),
			    	0, 360 * 64);
	    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 1);
	    }
	    paint_jewel (w, shape_mask, w->clock.shapeGC);
	    paint_hands (w, shape_mask, w->clock.shapeGC, w->clock.shapeGC);
	}
	/*
	 * Find the highest enclosing widget and shape it
	 */

	x = 0;
	y = 0;
	for (parent = (Widget) w; XtParent (parent); parent = XtParent (parent)) {
	    x = x + parent->core.x + parent->core.border_width;
	    y = y + parent->core.y + parent->core.border_width;
	}

	XShapeCombineMask (XtDisplay (parent), XtWindow (parent), ShapeBounding,
			    x, y, shape_mask, ShapeSet);

	/* erase the pixmap */
    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 0);
    	XFillRectangle (XtDisplay (w), shape_mask, w->clock.shapeGC,
			0, 0, w->core.width, w->core.height);
    	XSetForeground (XtDisplay (w), w->clock.shapeGC, 1);

	/*
	 * draw the clip shape
	 */

	if (w->clock.transparent)
	{
	    paint_jewel (w, shape_mask, w->clock.shapeGC);
	    paint_hands (w, shape_mask, w->clock.shapeGC, w->clock.shapeGC);
	}
	else
	{
	    TFillArc (XtDisplay (w), shape_mask,
			    w->clock.shapeGC, &w->clock.t,
			    -WINDOW_WIDTH(w)/2, -WINDOW_HEIGHT(w)/2,
			    WINDOW_WIDTH(w), WINDOW_HEIGHT(w),
			    0, 360 * 64);
	}

	XShapeCombineMask (XtDisplay (w), XtWindow (w), ShapeClip, 
		    0, 0, shape_mask, ShapeSet);

    } else
    {
    	/*
     	 * reconfigure the widget to split the availible
     	 * space between the window and the border
     	 */

     	if (face_width > face_height)
	    xwc.border_width = face_height;
    	else
	    xwc.border_width = face_width;
    	xwc.width = w->core.width - xwc.border_width * 2;
    	xwc.height = w->core.height - xwc.border_width * 2;
    	XConfigureWindow (XtDisplay (w), XtWindow (w),
			    CWWidth|CWHeight|CWBorderWidth,
			    &xwc);
    
    	SetTransform (&w->clock.t,
	    0, xwc.width,
	    xwc.height, 0,
	    -WINDOW_WIDTH(w)/2, WINDOW_WIDTH(w)/2,
	    -WINDOW_HEIGHT(w)/2, WINDOW_HEIGHT(w)/2);
    }
}
 
static void Realize (gw, valueMask, attrs)
     Widget gw;
     XtValueMask *valueMask;
     XSetWindowAttributes *attrs;
{
     ClockWidget	w = (ClockWidget)gw;

    if (w->clock.backing_store != Always + WhenMapped + NotUseful) {
     	attrs->backing_store = w->clock.backing_store;
	*valueMask |= CWBackingStore;
    }
    if (w->clock.transparent)
    {
	attrs->background_pixel = w->clock.minute;
	*valueMask |= CWBackPixel;
	*valueMask &= ~CWBackPixmap;
    }
    XtCreateWindow( gw, (unsigned)InputOutput, (Visual *)CopyFromParent,
		     *valueMask, attrs );
    if (!w->clock.transparent)
	Resize (w);
    new_time ((XtPointer) gw, 0);
}

static void Destroy (gw)
     Widget gw;
{
     ClockWidget w = (ClockWidget)gw;
     if (w->clock.interval_id) XtRemoveTimeOut (w->clock.interval_id);
     if (! w->clock.transparent) {
	 XtReleaseGC(gw, w->clock.minuteGC);
	 XtReleaseGC(gw, w->clock.hourGC);
	 XtReleaseGC(gw, w->clock.jewelGC);
	 XtReleaseGC(gw, w->clock.eraseGC);
	 if(w->clock.moonclock)
	 {
	   XtReleaseGC(gw, w->clock.moonGC);
	   XtReleaseGC(gw, w->clock.starsGC);
	   if(w->clock.skyGC) XFreeGC(XtDisplay(gw), w->clock.skyGC);
	 }
	   
     }
     if (w->clock.shapeGC)
	XFreeGC(XtDisplay(gw), w->clock.shapeGC);
    if (w->clock.shape_mask)
	XFreePixmap (XtDisplay (w), w->clock.shape_mask);

    
}

/* ARGSUSED */
static void Redisplay(gw, event, region)
     Widget gw;
     XEvent *event;
     Region region;
{
    ClockWidget	w;

    w = (ClockWidget) gw;
    if (!w->clock.transparent)
    {
    	paint_jewel(w, XtWindow (w), w->clock.jewelGC);
    	paint_moon (w, 1);
    	paint_hands(w, XtWindow (w), w->clock.minuteGC, w->clock.hourGC);
    }
}

/*
 * routines to draw the hands and jewel
 */

#ifndef PI	/* may be found in <math.h> */
# define PI (3.14159265358979323846)
#endif

/*
 * converts a number from 0..1 representing a clockwise radial distance
 * from the 12 oclock position to a radian measure of the counter-clockwise
 * distance from the 3 oclock position
 */

static double
clock_to_angle (clock)
double	clock;
{
	if (clock >= .75)
		clock -= 1.0;
	return -2.0 * PI * clock + PI / 2.0;
}

/* ARGSUSED */
static void new_time (client_data, id)
     XtPointer client_data;
     XtIntervalId *id;		/* unused */
{
        ClockWidget	w = (ClockWidget)client_data;
	long		now;
	struct tm	*tm;
	int             isAM;
	XSetWindowAttributes xswa;
	
	if (!w->clock.transparent)
	if (w->clock.polys_valid) {
		paint_hands (w, XtWindow (w), w->clock.eraseGC, w->clock.eraseGC);
		check_jewel (w, XtWindow (w), w->clock.jewelGC);
	}

	(void) time (&now);
	tm = localtime (&now);

	/* Timezone Adjust */
	tm->tm_hour += w->clock.hour_zone_adjust;
	if(tm->tm_hour > 23) tm->tm_hour -= 24;
	else if(tm->tm_hour <  0) tm->tm_hour = 24 + tm->tm_hour;

	/* Night Start */
        if(w->clock.night_start >= 12)
	  isAM = tm->tm_hour < (w->clock.night_start-12) || tm->tm_hour >= w->clock.night_start;
        else isAM = tm->tm_hour >= w->clock.night_start && tm->tm_hour < w->clock.night_start+12;

	if (tm->tm_hour >= 12)
	{
		tm->tm_hour -= 12;
        }
	w->clock.hour_angle = clock_to_angle ((((double) tm->tm_hour) +
				((double) tm->tm_min) / 60.0) / 12.0);
	w->clock.minute_angle =
		clock_to_angle (((double) tm->tm_min) / 60.0);
	/*
	 * add the timeout before painting the hands, that may
	 * take a while and we'd like the clock to keep up
	 * with time changes.
	 */
	w->clock.interval_id = 
	    XtAppAddTimeOut (XtWidgetToApplicationContext((Widget) w),
			     (60 - tm->tm_sec) * 1000, new_time, client_data);
	compute_hands (w);

	if(isAM)
	{
	  w->core.border_pixel = w->clock.night; 
	  xswa.border_pixel    = w->clock.night;   /* Night Color */
        }
        else 
	{
	  w->core.border_pixel = w->clock.day;
	  xswa.border_pixel    = w->clock.day;
        }

        XChangeWindowAttributes(XtDisplay(w), XtWindow(w), CWBorderPixel, &xswa);

	if (w->clock.transparent)
	    Resize (w);
	else
	{
	    paint_moon  (w, 0);
	    paint_hands (w, XtWindow (w), w->clock.minuteGC, w->clock.hourGC);
        }    
	    
} /* new_time */

paint_jewel (w, d, gc)
ClockWidget w;
Drawable    d;
GC	    gc;
{
    if (JEWEL_SIZE(w) > 0.0)
    {
	TFillArc (XtDisplay (w), d, gc, &w->clock.t,
			JEWEL_X(w) - JEWEL_SIZE(w) / 2.0,
 			JEWEL_Y(w) - JEWEL_SIZE(w) / 2.0,
 			JEWEL_SIZE(w), JEWEL_SIZE(w), 0, 360 * 64);
    }
}

#define sqr(x)	((x)*(x))

/*
 * check to see if the polygon intersects the circular jewel
 */

check_jewel_poly (w, poly)
ClockWidget	w;
TPoint		poly[POLY_SIZE];
{
    double	a2, b2, c2, d2;
    double	x, y, size;
    int	i;

    if (JEWEL_SIZE(w) > 0.0)
    {
	x = JEWEL_X(w);
	y = JEWEL_Y(w);
	size = JEWEL_SIZE(w);
	/*
	 * check each side of the polygon to see if the
	 * distance from the line to the center of the
	 * circular jewel is less than the radius.
	 */
	for (i = 0; i < POLY_SIZE-1; i++) {
		a2 = sqr (poly[i].x - x) + sqr (poly[i].y - y);
		b2 = sqr (poly[i+1].x - x) + sqr (poly[i+1].y - y);
		c2 = sqr (poly[i].x - poly[i+1].x) + sqr (poly[i].y - poly[i+1].y);
		d2 = a2 + b2 - c2;
		if (d2 <= sqr (size) &&
		    a2 <= 2 * c2 && b2 <= 2 * c2 ||
 		    a2 <= sqr (size) ||
		    b2 <= sqr (size))
			return 1;
	}
    }
    return 0;
}

check_jewel (w, d, gc)
ClockWidget	w;
Drawable	d;
GC		gc;
{
	if (!w->clock.polys_valid || JEWEL_SIZE(w) <= 0.0)
		return;
	if ((MINUTE_LENGTH(w) >= (JEWEL_Y(w) - JEWEL_SIZE(w)/2.0) &&
	     check_jewel_poly (w, w->clock.minute_poly)) ||
	    (HOUR_LENGTH(w) >= (JEWEL_Y(w) - JEWEL_SIZE(w)/2.0) &&
	     check_jewel_poly (w, w->clock.minute_poly)))
	{
		paint_jewel (w, d, gc);
	}
}

/*
 * A hand is a rectangle with a triangular cap at the far end.
 * This is represented with a five sided polygon.
 */

compute_hand (w, a, l, width, poly)
ClockWidget	w;
double		a, l, width;
TPoint		poly[POLY_SIZE];
{
	double	c, s;

	c = cos(a);
	s = sin(a);
	poly[0].x = c * l;	
	poly[0].y = s * l;
	poly[1].x = (l - width) * c - s * width;
	poly[1].y = (l - width) * s + c * width;
	poly[2].x = (-width) * c - s * width;
	poly[2].y = (-width) * s + c * width;
	poly[3].x = (-width) * c + s * width;
	poly[3].y = (-width) * s - c * width;
	poly[4].x = (l - width) * c + s * width;
	poly[4].y = (l - width) * s - c * width;
	poly[5].x = poly[0].x;
	poly[5].y = poly[0].y;
}

compute_hands (w)
ClockWidget	w;
{
	compute_hand (w, w->clock.minute_angle,
		MINUTE_LENGTH(w), MINUTE_WIDTH(w), w->clock.minute_poly);
	compute_hand (w, w->clock.hour_angle,
		HOUR_LENGTH(w), HOUR_WIDTH(w), w->clock.hour_poly);
	w->clock.polys_valid = 1;
}

paint_hand (w, d, gc, poly)
ClockWidget	w;
Drawable	d;
GC		gc;
TPoint		poly[POLY_SIZE];
{
	TFillPolygon (XtDisplay (w), d, gc, &w->clock.t, poly, POLY_SIZE,
			Convex, CoordModeOrigin);
}

/* static void */ paint_moon(w, newsize)
ClockWidget	w;
int		newsize;
{
    static XPoint xp[20];
    Pixel  skyPixel;
    static Pixel  skyGC_Pixel;
    static int    mw_x0, mw_y0, mw_x, mw_y, mw_w, mw_h, moon_dx, moon_dy, moon_x0, moon_y0, moon_d;
    int i;
    static double phase;
    static int phase_counter = 0;
    static int start_a, stop_a;

    if(w->clock.transparent || w->clock.moonclock == False) return;

    /* current color of sky */
    skyPixel = (w->core.border_pixel == w->clock.night) ? w->clock.night : w->clock.sky;
    
    /* is the sky GC not initialized ? */
    if(w->clock.skyGC == (GC)0)
    {
      XGCValues xgcv;
      xgcv.foreground = skyPixel;
      w->clock.skyGC = XCreateGC(XtDisplay(w),XtWindow(w),GCForeground,&xgcv);
      skyGC_Pixel = skyPixel;
    }
    else if(skyPixel != skyGC_Pixel)
         {
           XGCValues xgcv;
           xgcv.foreground = skyPixel;
           XChangeGC(XtDisplay(w), w->clock.skyGC, GCForeground, &xgcv);
           skyGC_Pixel = skyPixel;
         } 
         
    if(newsize) phase_counter = 0;

    if(phase_counter == 0)
    {
        /* SkyWindow Dimensions */          
    	mw_w  = w->core.width  * 0.3;
    	mw_h  = w->core.height * 0.3;
    	mw_x  = w->core.x + (w->core.width -  mw_w) / 2;  /* horizontal-center */
    	mw_y  = w->core.y + w->core.height * 0.6 ;
    	mw_x0 = mw_x + mw_w / 2;  /* center sky window */
    	mw_y0 = mw_y + mw_h / 2;
    	
    	moon_d  = (mw_w / 2) * 0.46;   /* moon diameter */
    	moon_dx = (mw_w / 2)*0.9 - moon_d / 2; 
        moon_dy = (mw_h / 2)*0.9 - moon_d / 2;
    
        /* Calculating the Moon-Phase is a very time consuming process, so
           we update the phase every 10 minutes.
           phase is: -1(new)..0(full)..+1(new) */
        GetMoonPhase(&phase); 
        
    	/* Mond-Mittelpunkt */
    	moon_x0 = mw_x0 + sin(phase * M_PI/2.0) * moon_dx;
    	moon_y0 = mw_y0 - cos(phase * M_PI/2.0) * moon_dy;

    	if(phase < -0.5)
    	{
    	  start_a = 64 * 270;
    	  stop_a  = 64 * 180;
    	}
    	else if(phase < 0.5)
    	     {
    	       start_a = 0;
    	       stop_a  = 64 * 360;
    	     }
    	     else
    	     {
    	       start_a = 64 * 90;
    	       stop_a  = 64 * 180;
    	     }      
    	
    }
        	
    /* draw the sky */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.skyGC,
    mw_x, mw_y, mw_w, mw_h,
    0, 64 * 180);

    /* the moon */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.moonGC,
    moon_x0 - moon_d / 2,
    moon_y0 - moon_d / 2,
    moon_d, moon_d,
    start_a, stop_a);

    /* upper left moon mask, skycolor */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.skyGC,
    (mw_x0 + moon_dx) - moon_d / 2,
    mw_y0 - moon_d / 2, 
    moon_d, moon_d,
    0, 64 * 180);

    /* lower left moon mask, background-color */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.eraseGC,
    (mw_x0 + moon_dx) - moon_d / 2,
    mw_y0 - moon_d / 2, 
    moon_d, moon_d,
    64 * 180, 64 * 180);

    /* upper right moon mask, skycolor */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.skyGC,
    (mw_x0 - moon_dx) - moon_d / 2,
    mw_y0 - moon_d / 2, 
    moon_d, moon_d,
    0, 64 * 180);

    /* lower right moon mask, background-color */
    XFillArc(XtDisplay(w),XtWindow(w),w->clock.eraseGC,
    (mw_x0 - moon_dx) - moon_d / 2,
    mw_y0 - moon_d / 2, 
    moon_d, moon_d,
    64 * 180, 64 * 180);

    /* draw a few random "stars" */
    if(phase_counter-- == 0)
    {
      phase_counter = 10;
    
      for(i=0; i < 20; i++)

      {
        double x;
#ifdef VMS
       int rand(void);
       x = rand();
       xp[i].x = mw_x + x * mw_w;
       xp[i].y = mw_y0 - rand() * (sin(x*M_PI) * mw_h / 2.0);

#else

        extern double drand48();

        /* the stars appear only in the sky */
        x = drand48();
        xp[i].x = mw_x + x * mw_w;
        xp[i].y = mw_y0 - drand48() * (sin(x*M_PI) * mw_h / 2.0);
#endif
     }
    }
    
    XDrawPoints(XtDisplay(w), XtWindow(w), w->clock.starsGC, xp, 20, CoordModeOrigin);
    
}    

    
paint_hands (w, d, minute_gc, hour_gc)
ClockWidget	w;
Drawable	d;
GC		minute_gc, hour_gc;
{
    if (w->clock.polys_valid) {
	paint_hand (w, d, hour_gc, w->clock.hour_poly);
	paint_hand (w, d, minute_gc, w->clock.minute_poly);
    }
}
