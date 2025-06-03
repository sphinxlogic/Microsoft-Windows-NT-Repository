/*
      Copyright (c) 1991, 1992 by Simon Marshall, University of Hull, UK

		   If you still end up late, don't blame me!
				       
  Permission to use, copy, modify, distribute, and sell this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
	both that copyright notice and this permission notice appear in
			   supporting documentation.
				       
  This software is provided AS IS with no warranties of any kind.  The author
    shall have no liability with respect to the infringement of copyrights,
     trade secrets or any patents by this file or any part thereof.  In no
      event will the author be liable for any lost revenue or profits or
	      other special, indirect and consequential damages.
*/

/* 
 * The stuff needed for the alarm call widget to appear on the screen, 
 * and the stuff for snoozing it.
 */


#include "xalarm.h"
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>


#define		NAMEFORMAT	"%s: %s@%s"
#define		REALNAMEFORMAT	" (%s)"
#define		TIMEFORMAT	" (%d:%02d)"



void		CreateAlarmWidget(), WakeUp(), ShowClickToZero(), ShowSnoozeValue();
SIGRET		ResetAlarm();
static void	SnoozeAmount(), Snooze(), TimeTracker();
extern void	PopupAndAsk(), AddTimeOuts(), SetWMName(), SetGeometry(),
		EnsureNotGrownOffScreen(), Audio(), QuitWarning(), Quit();
extern unsigned long DateToMilliSeconds();
extern long	TimeToMilliSeconds();
extern XtIntervalId ResetTracker();
extern String	getenv(), UserName(), MachineName();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData        xalarm;



/* 
 * Create the widget that holds the alarm itself.
 */

void CreateAlarmWidget (alarmmessage)
  String   alarmmessage;
{
    Widget 	  alarm, alarmbox, message, buttonbox,
		  quit, snooze, snooze1, snooze5, snooze15, snoozetime;
    static char   snoozetimetranslations [] =
		      "<EnterWindow>:	highlight() ShowClickToZero() \n\
		       <LeaveWindow>:	ShowSnoozeValue() unhighlight()";

    alarm = XtVaCreatePopupShell ("Alarm!", transientShellWidgetClass, xalarm.toplevel,
				  NULL);

    alarmbox = XtVaCreateManagedWidget ("alarm", formWidgetClass, alarm, NULL);

    buttonbox = XtVaCreateManagedWidget ("buttons", formWidgetClass, alarmbox, NULL);

    CreateManagedButton (quit, "quit", buttonbox, Quit, NULL);
    CreateManagedButton (snooze, "snooze", buttonbox, Snooze, NULL);
    CreateManagedButton (snooze1, "snooze1", buttonbox, SnoozeAmount, 1);
    CreateManagedButton (snooze5, "snooze5", buttonbox, SnoozeAmount, 5);
    CreateManagedButton (snooze15, "snooze15" ,buttonbox, SnoozeAmount, 15);
    CreateManagedButton (snoozetime, "snoozetime", buttonbox, SnoozeAmount, 0);

    if (NONNIL (String, alarmmessage))
	message = XtVaCreateManagedWidget ("message", labelWidgetClass, alarmbox, XtNlabel,
					   (XtArgVal) (xalarm.messagestr = alarmmessage),
					   NULL);
    else {
	message = XtVaCreateManagedWidget ("message", labelWidgetClass, alarmbox, NULL);
	XtVaGetValues (message, XtNlabel, &xalarm.messagestr, NULL);
    }

    XtOverrideTranslations (snoozetime,
			    XtParseTranslationTable (snoozetimetranslations));

    xalarm.alarmwidget = alarm;
    xalarm.snoozetimebutton = snoozetime;
    xalarm.messagewidget = message;
}



/* 
 * This is for the snoozetime button which doubles as the label showing how 
 * much time the snooze is selected for.
 */

void ShowClickToZero ()
{
    XtVaSetValues (xalarm.snoozetimebutton, XtNlabel, (XtArgVal) "Zero time", NULL);
    EnsureNotGrownOffScreen (xalarm.alarmwidget);
}
  


void ShowSnoozeValue ()
{  
    char   buf[TEXT];

    MAKETIMESTRING (buf, (int) (xalarm.timeout / MSECSIN1MIN));
    XtVaSetValues (xalarm.snoozetimebutton, XtNlabel, (XtArgVal) buf, NULL);
    EnsureNotGrownOffScreen (xalarm.alarmwidget);
}



/* 
 * Add to the amount that is currently the amount to snooze by.
 */

static void SnoozeAmount (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    int   mins = (int) clientdata;

    if (mins == 0) {
	xalarm.timeout = xalarm.dateout = 0;
	ShowClickToZero ();
    } else {
	xalarm.timeout += (mins*MSECSIN1MIN);
	ShowSnoozeValue ();
    }
}



/* 
 * Time to sleep.  Add the time outs & disappear.
 */

static void Snooze (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    AddTimeOuts ();
    if (ISVALID (xalarm.pesterid)) {
	XtRemoveTimeOut (xalarm.pesterid);
	xalarm.pesterid = INVALID;
    }
    if (ISVALID (xalarm.alarmid)) {
	XtRemoveTimeOut (xalarm.alarmid);
	xalarm.alarmid = INVALID;
    }

    XtPopdown (xalarm.alarmwidget);
}



/* 
 * Time to wake up.
 */

void WakeUp (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    xalarm.timeout = xalarm.dateout = 0;
    xalarm.saveonshutdown = False;
    SnoozeAmount ((Widget) NULL, (XtPointer) xalarm.snooze, (XtPointer) NULL);

    if (ISNONID (xalarm.alarmid))
	TimeTracker ((XtPointer) xalarm.alarmwidget, (XtIntervalId) NULL);
    SetGeometry (xalarm.alarmwidget);
    XtPopup (xalarm.alarmwidget, XtGrabNone);
    XFlush (XtDisplay (xalarm.alarmwidget));
    Audio (xalarm.alarmaudio);

    if (xalarm.pester > 0) {
	XRaiseWindow (XtDisplay (xalarm.alarmwidget), XtWindow (xalarm.alarmwidget));
	xalarm.pesterid =
	    XtAppAddTimeOut (xalarm.appcon, TIMEOUT (xalarm.pester * MSECSIN1MIN),
			     (XtTimerCallbackProc) WakeUp, (XtPointer) NULL);
    }

    if (ISWIDGET (xalarm.warningwidget))
	QuitWarning ((Widget) NULL,
		     (XtPointer) (XtParent (XtParent (xalarm.warningwidget))),
		     (XtPointer) NULL);
}



/* 
 * Track the time for this widget.  Puts a clock in the WM name,
 * along with the user's name etc.
 */

static void TimeTracker (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    String 	 user = UserName(), host = MachineName(), name = getenv ("NAME");
    char 	 wmname[TEXT];
    time_t 	 now;
    struct tm 	*clock;

    (void) time (&now);
    clock = localtime (&now);
    now -= (time_t) xalarm.offtime;

    (void) sprintf (wmname, NAMEFORMAT, xalarm.proggie, user, host);
    if (NONNIL (String, name))
	(void) sprintf (ENDOF (wmname), REALNAMEFORMAT, name);
    if (now >= 60) {
	(void) strcat (wmname, " +");
	MAKETIMESTRING (ENDOF (wmname), now / SECSIN1MIN);
    }
    (void) sprintf (ENDOF (wmname), TIMEFORMAT, clock->tm_hour, clock->tm_min);

    SetWMName ((Widget) clientdata, wmname);

    xalarm.alarmid = ResetTracker (TimeTracker, clientdata, (int) (xalarm.offtime % 60));
}



/* 
 * Let's popup to reset our alarm.
 */

SIGRET ResetAlarm (sig, code, scp, addr)
  int 		      sig, code;
  struct sigcontext  *scp;
  char 		     *addr;
{
    unsigned long   timeout = SUMTIMEOUTS (xalarm.dateout, xalarm.timeout);
    time_t 	    now;
    int 	    i;

    now = time ((time_t *) NULL);
    timeout -= (now - xalarm.settime);

    for (i=0; i<xalarm.numwarnings; i++)
	if (timeout  > xalarm.warnings[i])
	    XtRemoveTimeOut (xalarm.timeoutid[i]);
    XtRemoveTimeOut (xalarm.timeoutid[xalarm.numwarnings]);

    if (ISVALID (xalarm.pesterid)) {
	XtRemoveTimeOut (xalarm.pesterid);
	xalarm.pesterid = INVALID;
    }

    xalarm.dateout = DateToMilliSeconds (xalarm.datestr);
    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr);

    PopupAndAsk ();
}
