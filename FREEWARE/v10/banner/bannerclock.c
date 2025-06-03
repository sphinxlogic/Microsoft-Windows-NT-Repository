#module BannerClock "V1.0"

/*
**++
**  FACILITY:
**
**      The DECwindows Banner program
**
**  ABSTRACT:
**
**      This routine contains all the code to do with, the clock, analog and
**	digital, and the AST routine which runs updating the screen, and 
**	sending client messages. The client messages activate the update 
**	procedure in this module.
**
**  AUTHORS:
**
**      Jim Saunders
**
**
**  CREATION DATE:     10th June 1988
**
**  MODIFICATION HISTORY:
**
**  26th September 1989		Remove everything not connect with the clocks
**--
**/


/*
**
**  INCLUDE FILES
**
**/

#include "stdio.h"
#include "MrmAppl.h"


#include "Banner"


/*
**
**  MACRO DEFINITIONS
**
**/

#define hoursize	3
#define minsize		2
#define	secsize		1


/* 
 * Following are the forward, and external routine declarations used by
 * this module
 */

void BannerClock ();
void BannerDrawHand ();



/*
 * now our runtime data structures
 */


extern	 Bnr$_Blk    Bnr;
extern	 Clk$_Blk    Clk;
extern	 GC	     BannerGC;
extern	 XGCValues   GcValues;


static int clockminx;
static int clockmaxx;
static int clockminy;
static int clockmaxy;
static int clockzero;



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerClock (xmin, ymin, xmax, ymax)
**
**      This routine draws the initial setup for the analog clock.
**
**  FORMAL PARAMETERS:
**
**      xmin		Min x pos
**	ymin		Min y pos
**	xmax		Max x pos
**	ymax		Max y pos
**
**  IMPLICIT INPUTS:
**
**      none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
void	
BannerClock(xmin, ymin, xmax, ymax)
int xmin, ymin, xmax, ymax;
{
    float radius1, radius2, radius, status;
    int width, height, x;

/* 
 * First work out the current size of the clock, its center point, and 
 * the particular radius for each hand, and the boarder etc.
 */

    clockminx = xmin;
    clockmaxx = xmax;
    clockminy = ymin;
    clockmaxy = ymax;
    if (Bnr.clock_width <= Bnr.clock_height)
		    clockzero = Bnr.clock_width;
		else
		    clockzero = Bnr.clock_height;
    clockzero = clockzero / 2;

    width = xmax - xmin;
    height = ymax - ymin;    
    
    Clk.center = clockzero;
    
    radius2 = Clk.center;
    radius1 = Clk.center * 0.9;
    radius = Clk.center * 0.8;
    Clk.minute_hand = Clk.second_hand = Clk.center * 0.8;
    Clk.hour_hand = Clk.center * 0.6;

/*
 * now draw the clock outline
 */

    status = 0.0;
    for (x=11;  x >= 0;  x--)
	{
	int x_min, x_max, y_min, y_max;
	float starter, sin, cos;
	int start_x, start_y, end_x, end_y;

/*
 * get the sine and cosin of the current position.
 */
	MTH$SINCOS (&status, &sin, &cos);
	starter = radius1;
	if ( (x - ((x / 3) * 3)) == 2)	/* MOD 3, are we on a major tick? */
	    {
	    GcValues.line_width = minsize;
	    XChangeGC (XtDisplay(Bnr.dp_id), BannerGC, GCLineWidth, &GcValues);
	    starter = radius;
	    } 

	start_x = Clk.center + (cos * starter);
	start_y = Clk.center + (sin * starter);
	end_x	= Clk.center + (cos * radius2);
	end_y	= Clk.center + (sin * radius2);

	x_min = start_x + xmin;
	y_min = start_y + ymin;
	x_max = end_x + xmin;
	y_max = end_y + ymin;

	XDrawLine (XtDisplay(Bnr.clock_widget), XtWindow(Bnr.clock_widget), 
	    BannerGC, x_min, y_min, x_max, y_max);

	GcValues.line_width = 0;
	XChangeGC (XtDisplay(Bnr.dp_id), BannerGC, GCLineWidth, &GcValues);

	status = status + 0.523598767;
	}

    BannerDrawHand (Clk.hours, 12.0, Clk.hour_hand, hoursize, 0);
    BannerDrawHand (Clk.minutes, 60.0, Clk.minute_hand, minsize, 0);

    if (Bnr.Param_Bits.seconds)
	BannerDrawHand (Clk.seconds, 60.0, Clk.second_hand, secsize, 0);

    BannerPaintAlarm ();
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	BannerDrawHand (handvalue, maxvalue, radius, atb, erase)
**
**      This routine will draw one of the clock hands on the screen.
**
**  FORMAL PARAMETERS:
**
**      handvalue	Position of hand
**	maxvalue	Largest it can be
**	radius		Radius of hand
**	atb
**	erase		Erase or draw this hand
**
**  IMPLICIT INPUTS:
**
**	none
**
**  IMPLICIT OUTPUTS:
**
**      none
**
**  COMPLETION CODES:
**
**      none
**
**  SIDE EFFECTS:
**
**      none
**
**--
**/
void	
BannerDrawHand(handvalue, maxvalue, radius, atb, erase)
float handvalue, maxvalue, radius;
int atb, erase;
{
    int zerox, zeroy, xmax, ymax, endx, endy;
    float cos, sin, maxangle, angle;

/*
 * calculate the angle of the hand.
 */
    angle = 6.2831852 - (((handvalue / maxvalue) * 6.2831852) - 1.5707963);

    MTH$SINCOS (&angle, &sin, &cos);
    endx = Clk.center + (cos*radius);
    endy = Clk.center - (sin*radius);

    GcValues.line_width = atb;
    XChangeGC (XtDisplay(Bnr.dp_id), BannerGC, GCLineWidth, &GcValues);


    if (erase == 1)
	{
	XSetForeground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.background);
	XSetBackground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.foreground);
	}
    else
	{
	XSetForeground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.foreground);
	XSetBackground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.background);
	}

    xmax = endx + clockminx;
    ymax = endy + clockminy;

    zerox = clockminx + clockzero;
    zeroy = clockminy + clockzero;

    XDrawLine (XtDisplay(Bnr.clock_widget), XtWindow(Bnr.clock_widget), 
	BannerGC, zerox, zeroy, xmax, ymax);

    GcValues.line_width = 0;
    XChangeGC (XtDisplay(Bnr.dp_id), BannerGC, GCLineWidth, &GcValues);

    XSetForeground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.foreground);
    XSetBackground (XtDisplay(Bnr.dp_id), BannerGC, Bnr.background);
}
