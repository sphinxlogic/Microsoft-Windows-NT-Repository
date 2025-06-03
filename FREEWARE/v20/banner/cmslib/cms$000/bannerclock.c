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
*3I
 **
 **  26th September 1989		Remove everything not connect with the clocks
*3E
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*6D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*6E
*6I
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
*6E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 #define hoursize	3
 #define minsize		2
 #define	secsize		1
 
*3I
 
*3E
 /* 
  * Following are the forward, and external routine declarations used by
  * this module
  */
 
 void BannerClock ();
*3D
 void BannerPaintAlarm();
 void BannerUpdateWindows ();
 
*3E
*4D
 static void BannerDrawHand ();
*4E
*4I
 void BannerDrawHand ();
*4E
 
*3D
 void BannerAlarmAst ();
 void BannerExecuteAlarm();
 
 extern void BannerDigitalClock();
 extern void BannerWriteDate ();
 extern void BannerGlobe ();
*3E
*3I
 
*3E
 
 /*
  * now our runtime data structures
  */
 
*3D
 #define bell_width 17
 #define bell_height 17
*3E
 
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
*3D
 extern	 Pixmap	     BannerBellPixmap;
 
 static long clocktimer[2] = {-10*1000*1000, -1}; /* 1 second wakeups */
 static long purgadr[2] = {512, 2147483647};	/* Working set purge address */
 
 static int BannerAlarmCount = 0;
 
 static update_count = 0;
 static alarm_x = 0;
 static alarm_y = 0;
 static alarm_increment = 5;
 static alarm_outstanding = 0;
*3E
*3I
 
*3E
 
 static int clockminx;
 static int clockmaxx;
 static int clockminy;
 static int clockmaxy;
 static int clockzero;
 
*3D
 typedef struct numtim_struct	
 	{
 	    short int	year;
 	    short int	month;
 	    short int	day;
 	    short int	hour;
 	    short int	minute;
 	    short int	second;
 		  int	tenths;
 	} numtim_itemlist;
 
 static numtim_itemlist numtim;
 
 static long lasttime = -1;	/* Last time we came through the ast */
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerUpdateWindows()
 **
 **      This routine is out client message event routine. When a client message
 **	is recieveed this routine is called, to handle the update of the 
 **	screen.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
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
 BannerUpdateWindows()
 {
     float newhours, newminutes, newseconds;
     int digitaldone = 0;
     int drawhands = 0;
 
 
 /*
  * check if we should purge our working set
  */
     if (!Bnr.ws_purged)
 	{
 	Bnr.ws_purged = 1;
 	SYS$PURGWS (&purgadr);
 	}
 /*
  * do the things we must always do
  */
 
     if (Bnr.Param_Bits.cube)
*3E
*2I
*3D
 	{
 	BannerRotationSwitch();
*3E
*2E
*3D
 	BannerCube();
*3E
*2I
*3D
 	}
*3E
*2E
*3D
 /*
  * first check out if a second has really passed since the last time, if 
  * not we will do nothing.
  */
 
     SYS$NUMTIM (&numtim, NULL);
 
 /*
  * If a real second hasnt passed then forget it.
  */
     if (numtim.second == lasttime)
 	return;
 
     lasttime = numtim.second;
 
 /*
  * first see if a total refresh is need, if so call the appropriate 
  * action routines.
  */
 
     if (numtim.day != Bnr.current_day)
 	{
 	if (Bnr.Param_Bits.digital_clock)
 	    BannerWriteDate (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));
 	drawhands = 1;
 	Bnr.ws_purged = 0;
 	Bnr.current_day = numtim.day;
 	}
 
 /*
  * check if we have any alarms outstanding
  */
     if (Bnr.alarm_count != 0)
 	BannerExecuteAlarm ();
 
 /*
  * Now rotate the world if we want it, by 1 click.
  */
 
     if (Bnr.Param_Bits.world )
 	BannerGlobe (0, 0, Bnr.cube_width, Bnr.cube_height, 0);
 
     if (Bnr.Param_Bits.hanoi)
 	BannerHanoi();
 
     if (Bnr.Param_Bits.message_window)
 	BannerMessage();
 
 /*
  * First see if we need to do the rest this cycle
  */
 
     if (Bnr.Param_Bits.lck_window )
 	BannerLCK ();
 
     if (Bnr.Param_Bits.cwps_window )
 	BannerCWPS ();
 
     if (Bnr.Param_Bits.pe_window )
 	BannerPE ();
 
     if (Bnr.Param_Bits.monitor )
 	BannerMon ();
 
     if (Bnr.Param_Bits.cpu_window )
 	BannerCpu ();
 
     if (Bnr.Param_Bits.process_window )
 	BannerProcess ();
 
*3E
*2D
     if (Bnr.Param_Bits.cube)
 	BannerRotationSwitch();
 
*2E
*3D
     newhours = numtim.hour + (numtim.minute / 60.0);
     if (newhours > 12.0)
 	newhours = newhours - 12.0;
 
     newminutes = numtim.minute;
     newseconds = numtim.second;
 
 
 /* 
  * now refresh the clock hands.
  */
 
     if (Clk.minutes != newminutes)
     {
 	if (Bnr.Param_Bits.analog_clock)
 	{
 	    BannerDrawHand (Clk.hours, 12.0, Clk.hour_hand, hoursize, 1);
 	    BannerDrawHand (Clk.minutes, 60.0, Clk.minute_hand, minsize, 1);
 	    drawhands =1;
 	}
 	Clk.hours = newhours;
 	Clk.minutes = newminutes;
 	if (Bnr.Param_Bits.digital_clock)
 	    {
 	    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));
 	    digitaldone=1;
 	    }
     }
 
     if (Bnr.Param_Bits.seconds)
     {
 	if (Bnr.Param_Bits.analog_clock)
 	    {
 	    BannerDrawHand (Clk.seconds, 60.0, Clk.second_hand, secsize, 1);
 	    Clk.seconds = newseconds;
 	    BannerDrawHand (Clk.seconds, 60.0, Clk.second_hand, secsize, 0);
 	    drawhands = 1;
 	    }
 	if (Bnr.Param_Bits.digital_clock && digitaldone == 0)
 	    BannerDigitalClock (XtDisplay(Bnr.clock_widget), 
 		XtWindow(Bnr.clock_widget));    	
     }
 
 /*
  * Now paint new hands for hours minutes etc.
  */    
 	if (drawhands)
 	{
 	    BannerDrawHand (Clk.hours, 12.0, Clk.hour_hand, hoursize, 0);
 	    BannerDrawHand (Clk.minutes, 60.0, Clk.minute_hand, minsize, 0);
 	}
 
 }
*3E
*3I
 
*3E
 
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
*4D
 static void	
*4E
*4I
 void	
*4E
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
*3D
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerPaintAlarm
 **
 **      This routine paints the little alram picture in the clock window
 **	if we have an alarm set.
 **
 **  FORMAL PARAMETERS:
 **
 **      [@description_or_none@]
 **
 **  IMPLICIT INPUTS:
 **
 **      [@description_or_none@]
 **
 **  IMPLICIT OUTPUTS:
 **
 **      [@description_or_none@]
 **
 **  {@function_value_or_completion_codes@}
 **
 **      [@description_or_none@]
 **
 **  SIDE EFFECTS:
 **
 **      [@description_or_none@]
 **
 **--
 **/
 void	
 BannerPaintAlarm()
 {
 
     if (BannerAlarmCount > 0 &&
 	(Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock))
 	XCopyPlane (XtDisplay(Bnr.clock_widget),
 		BannerBellPixmap,
 		XtWindow(Bnr.clock_widget),
 		BannerGC,
 		0,
 		0,
 		bell_width,
 		bell_height,
 		Bnr.clock_width - bell_width - 3,
 		3,
 		1);
     else
 	if (BannerAlarmCount == 0 &&
 	    (Bnr.Param_Bits.analog_clock || Bnr.Param_Bits.digital_clock))
 	    {
 	    XSetBackground (XtDisplay(Bnr.clock_widget), BannerGC,
 		Bnr.foreground);
 	    XSetForeground (XtDisplay(Bnr.clock_widget), BannerGC,
 		Bnr.background);
 
 	    XFillRectangle (XtDisplay(Bnr.clock_widget), 
 		    XtWindow (Bnr.clock_widget),
 		    BannerGC, 
 		    Bnr.clock_width - bell_width - 3,
 		    3,
 		    bell_width,
 		    bell_height);
 
 	    XSetForeground (XtDisplay(Bnr.clock_widget), BannerGC,
 		Bnr.foreground);
 	    XSetBackground (XtDisplay(Bnr.clock_widget), BannerGC,
 		Bnr.background);	
 	    }
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerEstablishAlarm (hour, minute, text);
 **
 **      This routine establishes an AST to generate an alarm.
 **
 **  FORMAL PARAMETERS:
 **
 **      As above
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
 BannerEstablishAlarm(hour, minute, text)
 int hour, minute;
 char *text;
 {
 int time[2];
 int desc[2];
 char str[20];
 /*
  * Convert our time, to a quadword
  */
 
     BannerAlarmCount++;
     BannerPaintAlarm ();
 
     sprintf (str, "-- %2D:%2D", hour, minute);
     desc[0] = 8;
     desc[1] = &str;
     SYS$BINTIM (&desc, &time); 
 /*
  * establish the AST for the alarm
  */
     SYS$SETIMR (NULL, &time, BannerAlarmAst, text);
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerAlarmAst (text);
 **
 **      This routine is the ast routine for the alarm process, it 
 **	establishes the fact the Alarm has arrived, and records it in
 **	our data structures.
 **
 **  FORMAL PARAMETERS:
 **
 **      Tghe alarm text itself.
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
 BannerAlarmAst(text)
 char *text;
 {
 Bnr$Alarm *ouralarm;
 
 
 /* 
  * Priority up!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.base_priority, NULL);
 /*
  * store the alarm text, and increment the alarm count.
  */
 
     if (Bnr.alarm_list == NULL)
 	{
 	Bnr.alarm_list = XtMalloc (sizeof(int));
 	ouralarm = (*Bnr.alarm_list)[0] = XtMalloc (sizeof(Bnr$Alarm));
 	}
     else
 	{
 	int new;
 
 	new = XtMalloc(	(Bnr.alarm_count+1) * sizeof(int));
 	memcpy (new, Bnr.alarm_list, (Bnr.alarm_count) * sizeof(int));
 	XtFree (Bnr.alarm_list);
 	Bnr.alarm_list = new;
 	ouralarm = (*Bnr.alarm_list)[Bnr.alarm_count] = XtMalloc (sizeof(Bnr$Alarm));
 	}
     ouralarm->alarm = text;
     Bnr.alarm_count++;
 
 /* 
  * Priority down!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.current_priority, NULL);
 
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerExecuteAlarm ();
 **
 **      This routine displayes message box's for all alarms that need 
 **	Executing at this point in time.
 **
 **  FORMAL PARAMETERS:
 **
 **
 **  IMPLICIT INPUTS:
 **
 **
 **  IMPLICIT OUTPUTS:
 **
 **
 **  COMPLETION CODES:
 **
 **
 **  SIDE EFFECTS:
 **
 **
 **--
 **/
 void	
 BannerExecuteAlarm()
 {
     Widget alarmwidget = 0;
     int class;
     Bnr$Alarm *ouralarm;
 /*
  * go through each alarm, and display a message for it.
  */
 
 /*
  * Turn AST's off
  */
 
     SYS$SETAST(0);
 
     while (Bnr.alarm_count > 0)
 	{
 	char *cs_string;
 /*
  * Work out incrememental x,y positions.
  */
 	BannerAlarmCount--;
 	BannerPaintAlarm ();
 
 	if (alarm_x == 0 ||
 	    alarm_increment == 0)
 	    {
 	    alarm_x = Bnr.clock_x;
 	    alarm_y = Bnr.clock_y;
 	    alarm_increment = 5;
 	    }
 	alarm_x = alarm_x + 10;
 	alarm_y = alarm_y + 10;
 	alarm_increment--;
 
 	DwtFetchWidget (Bnr.drm_prt,
 	    "BANNER_ALARM_MESSAGE",
 	    Bnr.dp_id,
 	    &alarmwidget,
 	    &class);
 
 	ouralarm = (*Bnr.alarm_list)[Bnr.alarm_count-1];
 	cs_string = DwtLatin1String (ouralarm->alarm);
 	BannerSetArg (alarmwidget, DwtNlabel, cs_string);
 	XtFree (cs_string);
 	XtFree (ouralarm->alarm);
 	XtFree ((*Bnr.alarm_list)[Bnr.alarm_count-1]);
 	BannerSetArg (alarmwidget, DwtNx, alarm_x);
 	BannerSetArg (alarmwidget, DwtNy, alarm_y);
 	XtManageChild (alarmwidget);
 	XtRealizeWidget (alarmwidget);
 	if (Bnr.Param_Bits.bell)
 	    Xbell (XtDisplay (alarmwidget), Bnr.bell_volume);
 	alarmwidget = 0;
 	Bnr.alarm_count--;    
 	}        
 
     if (Bnr.alarm_list != NULL)
 	{
 	XtFree (Bnr.alarm_list);
 	Bnr.alarm_list = ( Bnr$Alarm *) NULL;
 	}
 /*
  * Turn AST's on
  */
 
     SYS$SETAST(1);
 
 }
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerSecondTimerAst ();
 **
 **	This routine sets up an AST which calls itself, on a 1 second interval.
 **	For each call it sends a client message to wake up the main processing 
 **	loop.
 **
 **  FORMAL PARAMETERS:
 **
 **	None
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
 
 static XClientMessageEvent clientmessage;
 
 void	
 BannerSecondTimerAst()
 {
 
 
 /* 
  * Priority up!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.base_priority, NULL);
 /*
  * now send the client message to our selves!
  */
 
     clientmessage.type = ClientMessage;
     clientmessage.display = XtDisplay (Bnr.main_widget);
     clientmessage.window = XtWindow (Bnr.main_widget);
     clientmessage.format = 32;
 
     XSendEvent (XtDisplay (Bnr.main_widget),
 	XtWindow (Bnr.main_widget),
 	True,
 	(unsigned long) 0,
 	&clientmessage);
 
 /*
  * now reset an AST for 1 seconds time
  */
 
 /*    SYS$SETIMR (NULL, &clocktimer, BannerSecondTimerAst, NULL); */
 
       XtAddTimeOut (1000, BannerSecondTimerAst, 0);
 
 /* 
  * Priority down!
  */
 
     if (Bnr.current_priority != Bnr.base_priority)
 	SYS$SETPRI (NULL, NULL, Bnr.current_priority, NULL);
 
 }
*3E
