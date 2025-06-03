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
 * Stuff for getting the time the alarm is due to go off, and 
 * confirmation if needed.
 */



#include "xalarm.h"
#include "dates.h"

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Dialog.h>


#define		TIMEINSTRUCTIONS	"Enter time (as [+]time[am/pm]):"
#define		DATEINSTRUCTIONS	"Enter date (as +days|date):"
#define		WARNINGINSTRUCTIONS	"Enter warnings (as time[,time...]):"
#define		CONFIRMFORMAT		"%s %s %d, %02d:%02d (in %d+%d:%02d), "
#define		WHENFORMAT		"When?  (%d:%02d)"
#define		EDITTIME		(0)
#define		EDITDATE		(1)
#define		EDITWARNINGS		(2)
#define		EDITED(str, widget)	(strcmp (str, XawDialogGetValueString (widget)))
#define		DIALOGVALUE(widget)	(XtNewString (XawDialogGetValueString (widget)))
#define		UPCASE(ch)		(((ch) >= 'a') and ((ch) <= 'z') ? \
					 ((ch) - 'a' + 'A') : (ch))



void		PopupAndAsk(), EnteredTime(), EnteredDate(), EnteredWarnings(),
		Confirmed(), MakeConfirmMessage(), EditedText();
static void	PopupAndConfirm(), Popup(), PopupNext(), Edit(), Cancel(), TimeTracker();
extern XtIntervalId ResetTracker();
extern String	HomeDirectory();
extern void	Quit(), AddTimeOuts(), SetWarningTimes(), SetGeometry(),
		SaveAlarmSettings(), SetWMName(), EnsureNotGrownOffScreen();
extern long	TimeToMilliSeconds();
extern unsigned long DateToMilliSeconds();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData	xalarm;



/* 
 * just pop up a dialog widget to get an at/in time.  won't pop down 
 * until a valid time is given.  EnteredTime() does this.
 */

void PopupAndAsk ()
{
    static Widget   popup = NONWIDGET;
    Widget 	    when, gettime, getdate, getwtime, confirm;

    if (ISNONWIDGET (popup)) {
	popup = XtVaCreatePopupShell ("When?", transientShellWidgetClass,
				      xalarm.toplevel, NULL);
	when = XtVaCreateManagedWidget ("when", formWidgetClass, popup, NULL);

	/* 
	 * Widget for time input.
	 */
	gettime = XtVaCreateManagedWidget ("time", dialogWidgetClass, when,
					   XtNvalue, (XtArgVal) xalarm.timestr, NULL);
	XawDialogAddButton (gettime, "ok", EnteredTime, (XtPointer) NULL);
	XawDialogAddButton (gettime, "editdate", Edit, (XtPointer) EDITDATE);
	XawDialogAddButton (gettime, "editwarnings", Edit, (XtPointer) EDITWARNINGS);
	XawDialogAddButton (gettime, "quit", Quit, (XtPointer) NULL);

	/* 
	 * Widget for date input.
	 */
	getdate = XtVaCreateWidget ("date", dialogWidgetClass, when,
				    XtNvalue, (XtArgVal) xalarm.datestr, NULL);
	XawDialogAddButton (getdate, "ok", EnteredDate, (XtPointer) NULL);
	XawDialogAddButton (getdate, "edittime", Edit, (XtPointer) EDITTIME);
	XawDialogAddButton (getdate, "editwarnings", Edit, (XtPointer) EDITWARNINGS);
	XawDialogAddButton (getdate, "quit", Quit, (XtPointer) NULL);

	/* 
	 * Widget for warning times input.
	 */
	getwtime = XtVaCreateWidget ("warnings", dialogWidgetClass, when,
				     XtNvalue, (XtArgVal) xalarm.warningsstr, NULL);
	XawDialogAddButton (getwtime, "ok", EnteredWarnings, (XtPointer) NULL);
	XawDialogAddButton (getwtime, "edittime", Edit, (XtPointer) EDITTIME);
	XawDialogAddButton (getwtime, "editdate", Edit, (XtPointer) EDITDATE);
	XawDialogAddButton (getwtime, "quit", Quit, (XtPointer) NULL);

	/* 
	 * Widget for confirmation.
	 */
	confirm = XtVaCreateWidget ("confirm", dialogWidgetClass, when,
				    XtNvalue, (XtArgVal) xalarm.messagestr, NULL);
	XawDialogAddButton (confirm, "ok", Confirmed, (XtPointer) popup);
	XawDialogAddButton (confirm, "cancel", Cancel, (XtPointer) NULL);
	XawDialogAddButton (confirm, "save", SaveAlarmSettings, (XtPointer) NULL);
	XawDialogAddButton (confirm, "quit", Quit, (XtPointer) NULL);

	/* 
	 * Set the callback for the value widget in each Dialog.
	 */
	AddValueCallback (xalarm.gettimewidget = gettime, EditedText, True);
	AddValueCallback (xalarm.getdatewidget = getdate, EditedText, True);
	AddValueCallback (xalarm.getwtimewidget = getwtime, EditedText, True);
	AddValueCallback (xalarm.confirmwidget = confirm, EditedText, True);
	xalarm.dialog = gettime;
	xalarm.savebutton = XtNameToWidget (confirm, "save");
    }

    if (ISNONID (xalarm.whenid))
	TimeTracker ((XtPointer) popup, (XtIntervalId) NULL);
    SetGeometry (popup);
    XtPopup (popup, XtGrabExclusive);
    XRaiseWindow (XtDisplay (popup), XtWindow (popup));
    PopupNext ();
    XFlush (XtDisplay (popup));
}



/* 
 * Track the time for this widget.  Puts a clock in the WM name, and 
 * keeps the confirmation time message upto date.
 */

static void TimeTracker (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    char 	wmname[TEXT], buf[TEXT];
    time_t 	now;
    struct tm  *clock;
    
    (void) time (&now);
    clock = localtime (&now);

    (void) sprintf (wmname, WHENFORMAT, clock->tm_hour, clock->tm_min);
    SetWMName ((Widget) clientdata, wmname);

    if (xalarm.dialog == xalarm.confirmwidget) {
	/* 
	 * Make sure that the alarm is not now out of date.
	 */
	xalarm.dateout = DateToMilliSeconds (xalarm.datestr);
	xalarm.timeout = TimeToMilliSeconds (xalarm.timestr);
	if ((xalarm.timeout == 0) and (xalarm.dateout == 0) and
	    (xalarm.timestr[0] != '+'))
	    Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS);
	else {
	    MakeConfirmMessage (buf);
	    if (not XtIsSensitive (xalarm.savebutton))
		(void) sprintf (ENDOF (buf), "\nSaved in %s/%s",
				HomeDirectory (), XALARMFILE);
	    XtVaSetValues (xalarm.confirmwidget, XtNlabel, (XtArgVal) buf, NULL);
	}
    }

    xalarm.whenid = ResetTracker (TimeTracker, clientdata, 0);
}



/* 
 * Popup the next dialog, if more needs to be entered.
 */

static void PopupNext ()
{
    if (ISINVALID (xalarm.timeout) or
	EDITED (xalarm.timestr, xalarm.gettimewidget))
	Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS);
    else if (ISINVALID (xalarm.dateout) or
	     EDITED (xalarm.datestr, xalarm.getdatewidget))
	Popup (xalarm.getdatewidget, DATEINSTRUCTIONS);
    else if (ISINVALID (xalarm.numwarnings) or
	     EDITED (xalarm.warningsstr, xalarm.getwtimewidget))
	Popup (xalarm.getwtimewidget, WARNINGINSTRUCTIONS);
    else
	PopupAndConfirm ();
}



/* 
 * If invalid, stay.  Otherwise, get the rest, if any.
 */

void EnteredTime (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    char   message[TEXT];

    xalarm.timeout = TimeToMilliSeconds
	(xalarm.timestr = DIALOGVALUE (xalarm.gettimewidget));

    if (ISVALID (xalarm.timeout))
	PopupNext ();
    else {
	(void) sprintf (message, "%s%s", xalarm.errormessage, TIMEINSTRUCTIONS);
	XtVaSetValues (xalarm.gettimewidget, XtNlabel, (XtArgVal) message, NULL);
    }
}



/* 
 * If invalid, stay.  Otherwise, get the rest, if any.
 */

void EnteredDate (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    char   message[TEXT];

    xalarm.dateout = DateToMilliSeconds
	(xalarm.datestr = DIALOGVALUE (xalarm.getdatewidget));

    if (ISVALID (xalarm.dateout))
	PopupNext ();
    else {
	(void) sprintf (message, "%s%s", xalarm.errormessage, DATEINSTRUCTIONS);
	XtVaSetValues (xalarm.getdatewidget, XtNlabel, (XtArgVal) message, NULL);
    }
}



/* 
 * If invalid, stay.  Otherwise, get the rest, if any.
 */

void EnteredWarnings (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    char   message[TEXT];

    SetWarningTimes (xalarm.warningsstr = DIALOGVALUE (xalarm.getwtimewidget));

    if (ISVALID (xalarm.numwarnings))
	PopupNext ();
    else {
	(void) sprintf (message, "%s%s", xalarm.errormessage, WARNINGINSTRUCTIONS);
	XtVaSetValues (xalarm.getwtimewidget, XtNlabel, (XtArgVal) message, NULL);
    }
}



/* 
 * Just toggle whether i/t concerns alarm time/date/warning time(s).
 */

static void Edit (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    switch ((int) clientdata) {
     case EDITTIME:
	Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS);
	break;
     case EDITDATE:
	Popup (xalarm.getdatewidget, DATEINSTRUCTIONS);
	break;
     case EDITWARNINGS:
	Popup (xalarm.getwtimewidget, WARNINGINSTRUCTIONS);
	break;
    }
}



/* 
 * Set the sensitivity of the save button.
 */

void EditedText (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    static Boolean   edited = True;

    if (not (Boolean) clientdata)
	XtSetSensitive (xalarm.savebutton, edited = False);
    else if (not edited)
	XtSetSensitive (xalarm.savebutton, edited = True);
}



/* 
 * Popup to confirm, showing the time which the alarm will trigger.
 */

static void PopupAndConfirm ()
{
    char   message[TEXT];

    MakeConfirmMessage (message);
    Popup (xalarm.confirmwidget, message);
}



/* 
 * Make the message that should be displayed as the label in the 
 * confirmation window.
 */

void MakeConfirmMessage (message)
  String   message;
{
    static char     strings[][4] = {WEEKDAYS, MONTHS};
    time_t 	    now;
    struct tm 	   *alarmtime;
    unsigned long   msecsout;
    int 	    count = 0, i;

    msecsout = SUMTIMEOUTS (DateToMilliSeconds (xalarm.datestr),
			    TimeToMilliSeconds (xalarm.timestr));
    (void) time (&now);
    now += (time_t) (msecsout / MSECSIN1SEC);
    alarmtime = localtime (&now);

    strings[alarmtime->tm_wday][0] = UPCASE (strings[alarmtime->tm_wday][0]);
    strings[alarmtime->tm_mon+7][0] = UPCASE (strings[alarmtime->tm_mon+7][0]);
    msecsout += (59 * MSECSIN1SEC);
    (void) sprintf (message, CONFIRMFORMAT,
		    strings[alarmtime->tm_wday], strings[alarmtime->tm_mon+7],
		    alarmtime->tm_mday, alarmtime->tm_hour, alarmtime->tm_min,
		    msecsout / MSECSIN1DAY, (msecsout / MSECSIN1HR) % 24,
		    (msecsout / MSECSIN1MIN) % 60);
    msecsout -= (59 * MSECSIN1SEC);

    for (i=0; i<xalarm.numwarnings; i++)
	if (msecsout > xalarm.warnings[i])
	    count++;

    if (count == 0)
	(void) sprintf (ENDOF (message), "no warnings:");
    else 
	(void) sprintf (ENDOF (message), "warning%s: ", PLURAL (count));

    for (i=0; i<xalarm.numwarnings; i++)
	if (xalarm.timeout > xalarm.warnings[i]) {
	    MAKETIME (ENDOF (message), xalarm.warnings[i] / MSECSIN1MIN);
	    if (--count > 0)
		(void) strcat (message, ",");
	}
}



/* 
 * Yow!  Let's go!
 */

void Confirmed (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    String   message = DIALOGVALUE (xalarm.confirmwidget);

    if (*message != '\0')
	XtVaSetValues (xalarm.messagewidget,
		       XtNlabel, (XtArgVal) (xalarm.messagestr = message), NULL);

    /* 
     * Reset the timeout, since the gap between entering 
     * and confirming may be significant.
     */

    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr);
    XtRemoveTimeOut (xalarm.whenid); 
    xalarm.whenid = NONID;
    AddTimeOuts ();

    XtPopdown (XtParent (XtParent (xalarm.confirmwidget)));
}



/* 
 * Swap back to gettime widget & remove kbd focus.
 */

static void Cancel (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS);
}



/* 
 * Show this widget, and set its label.
 * If there's not enough room for the new widget, move it.
 */

static void Popup (widget, label)
  Widget   widget;
  String   label;
{
    Widget   form = XtParent (widget);

    XtVaSetValues (widget, XtNlabel, (XtArgVal) label, NULL);

    XawFormDoLayout (form, False);
    if (xalarm.dialog != widget) {
	XtManageChild (widget);
	XtUnmanageChild (xalarm.dialog);
	xalarm.dialog = widget;
    }
    XawFormDoLayout (form, True);

    EnsureNotGrownOffScreen (XtParent (form));
}
