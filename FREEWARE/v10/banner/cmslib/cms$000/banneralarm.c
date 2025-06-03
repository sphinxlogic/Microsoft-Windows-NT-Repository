 #module BannerAlarm "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code to manage the Banner Alarm window, 
 **  and the AST's for alarm processing.
 **
 **  AUTHORS:
 **
 **      Jim Saunders	26th September 1989	    Original Version
 **
 **
 **  CREATION DATE:     26th September 1989
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
*4D
 #include <stdio.h>
 #include <decw$include/DECwDwtWidgetProg.h>
*4E
*4I
 
 #include "stdio.h"
 #include "MrmAppl.h"
 
 
*4E
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 /* 
  * Following are the forward, and external routine declarations used by
  * this module
  */
 
 void BannerPaintAlarm();
 void BannerAlarmAst ();
 void BannerExecuteAlarm();
 
 
 /*
  * now our runtime data structures
  */
 
 #define bell_width 17
 #define bell_height 17
 
 extern	 Bnr$_Blk    Bnr;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 extern	 Pixmap	     BannerBellPixmap;
 
 
 static int BannerAlarmCount = 0;
 
 static alarm_x = 0;
 static alarm_y = 0;
 static alarm_increment = 5;
 static alarm_outstanding = 0;
 
 
 
 
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
 
*4D
 	DwtFetchWidget (Bnr.drm_prt,
*4E
*4I
 	MrmFetchWidget (Bnr.drm_prt,
*4E
 	    "BANNER_ALARM_MESSAGE",
 	    Bnr.dp_id,
 	    &alarmwidget,
 	    &class);
 
 	ouralarm = (*Bnr.alarm_list)[Bnr.alarm_count-1];
*4D
 	cs_string = DwtLatin1String (ouralarm->alarm);
 	BannerSetArg (alarmwidget, DwtNlabel, cs_string);
*4E
*4I
*7D
 /*
  *#ifdef ORIGINAL_CODE
  * DwtLatin1String (ouralarm->alarm)
  *#endif
  */
 	cs_string =  XmStringCreate(ouralarm, XmSTRING_DEFAULT_CHARSET);
*7E
*7I
 	cs_string =  XmStringCreate(ouralarm->alarm, XmSTRING_DEFAULT_CHARSET);
*7E
 
*6D
 	BannerSetArg (alarmwidget, XmNlabelString, cs_string);
*6E
*6I
 	BannerSetArg (alarmwidget, XmNmessageString, cs_string);
*6E
*4E
 	XtFree (cs_string);
 	XtFree (ouralarm->alarm);
 	XtFree ((*Bnr.alarm_list)[Bnr.alarm_count-1]);
*4D
 	BannerSetArg (alarmwidget, DwtNx, alarm_x);
 	BannerSetArg (alarmwidget, DwtNy, alarm_y);
*4E
*4I
 	BannerSetArg (alarmwidget, XmNx, alarm_x);
 	BannerSetArg (alarmwidget, XmNy, alarm_y);
*4E
 	XtManageChild (alarmwidget);
 	XtRealizeWidget (alarmwidget);
 	if (Bnr.Param_Bits.bell)
*5D
 	    Xbell (XtDisplay (alarmwidget), Bnr.bell_volume);
*5E
*5I
 	    XBell (XtDisplay (alarmwidget), Bnr.bell_volume);
*5E
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
