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
 * The stuff for dealing with warnings; processing to get the times & 
 * the fns for popping up the warnings themselves.
 */



#include "xalarm.h"

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>


#define		WARNINGFORMAT	"Warning!  (%d:%02d)"
#define		WARNING		"Warning: %s due in "


void		SetWarningTimes(), Warning(), QuitWarning();
static void	TimeTracker(), Reset();
extern XtIntervalId ResetTracker();
extern SIGRET	ResetAlarm();
extern long	TimeToMilliSeconds();
extern void	SetGeometry(), SetWMName(), Audio(), Quit();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData	xalarm;



/* 
 * Process the string to extract warning times.  Currently just gives 
 * a message if a warning time is invalid.
 */

void SetWarningTimes (warnings)
  String   warnings;
{
    String 	    warnstr = warnings;
    char 	    warning[TEXT];
    unsigned long   biggest;
    int 	    i, j, bigindex;
	            
    xalarm.numwarnings = 0;
    while ((*warnstr != '\0') and (ISVALID (xalarm.numwarnings))) {
	warning[i=0] = '+';
	while ((*warnstr != ',') and (*warnstr != '\0'))
	    warning[++i] = *warnstr++;
	warning[++i] = '\0';

	switch (xalarm.warnings[xalarm.numwarnings] =
		(unsigned long) TimeToMilliSeconds (warning)) {
	 case 0:
	    break;
	 case INVALID:
	    xalarm.numwarnings = INVALID;
	    break;
	 default:
	    xalarm.numwarnings++;
	    break;
	}

	while (*warnstr == ',')
	    warnstr++;
    }
    if ((warnstr != warnings) and (*(warnstr-1) == ','))
	xalarm.numwarnings = INVALID;

    /* 
     * Sort it, why not?
     */
    for (i=0; i<xalarm.numwarnings-1; i++) {
	biggest = xalarm.warnings[bigindex = i];
	for (j=i+1; j<xalarm.numwarnings; j++)
	    if (xalarm.warnings[j] > biggest)
		biggest = xalarm.warnings[bigindex = j];
	xalarm.warnings[bigindex] = xalarm.warnings[i];
	xalarm.warnings[i] = biggest;
    }
}



/* 
 * pop up a warning.  includes a dismiss & quit button.
 */

void Warning (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    static Widget   popup = NONWIDGET;
    Widget 	    warningbox, dismiss, reset, quit;

    if (ISNONWIDGET (popup)) {
	popup = XtVaCreatePopupShell ("Warning!", transientShellWidgetClass,
				      xalarm.toplevel, NULL);
	warningbox = XtVaCreateManagedWidget ("warning", formWidgetClass, popup, NULL);

	CreateManagedButton (dismiss, "dismiss", warningbox, QuitWarning, popup);
	xalarm.warningwidget = XtVaCreateManagedWidget ("message", labelWidgetClass,
							warningbox, NULL);
	CreateManagedButton (reset, "reset", warningbox, Reset, popup);
	CreateManagedButton (quit, "quit", warningbox, Quit, NULL);
    }

    if (ISNONID (xalarm.warningid))
	TimeTracker ((XtPointer) popup, (XtIntervalId) NULL);
    SetGeometry (popup);
    Audio (xalarm.warningaudio);
    XtPopup (popup, XtGrabNone);
    XRaiseWindow (XtDisplay (popup), XtWindow (popup));
    XFlush (XtDisplay (popup));
}
  
	     

/* 
 * Track the time for this widget.  Puts a clock in the WM name, and 
 * keeps the warning message upto date.
 */

static void TimeTracker (clientdata, id)
  XtPointer 	 clientdata;
  XtIntervalId 	 id;
{
    char 	buf[TEXT], wmname[TEXT], warnmessage[TEXT], *wm = warnmessage;
    int 	mins, count;
    time_t 	now;
    struct tm  *clock;
    
    (void) time (&now);
    clock = localtime (&now);

    if ((mins = (int) (((unsigned long) 59 + xalarm.offtime - (unsigned long) now)
		       / SECSIN1MIN)) == 0)
	xalarm.warningid = NONID;
    else {
	(void) sprintf (wmname, WARNINGFORMAT, clock->tm_hour, clock->tm_min);
	SetWMName ((Widget) clientdata, wmname);

	if (xalarm.warningwords == 0)
	    (void) sprintf (buf, WARNING, xalarm.proggie+1);
	else {
	    (void) sprintf (warnmessage, "`%s", xalarm.messagestr);
	    count = xalarm.warningwords;
	    while ((count-- > 0) and (*wm != '\0')) {
		while (isspace (*wm))
		    *wm++ = ' ';
		while (not isspace (*wm) and (*wm != '\0'))
		    wm++;
	    }
	    (void) sprintf (wm, "%s", (*wm == '\0') ? "' " : "...' ");
	    (void) sprintf (buf, WARNING, warnmessage);
	}
	MAKETIMESTRING (ENDOF (buf), mins);

	XtVaSetValues (xalarm.warningwidget, XtNlabel, (XtArgVal) buf, NULL);

	xalarm.warningid = ResetTracker (TimeTracker, clientdata,
					 (int) (xalarm.offtime % 60));
    }
}



/* 
 * Popdown the warning window & popup to reset the alarm.
 */

static void Reset (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    QuitWarning (widget, clientdata, calldata);
    (void) ResetAlarm ((int) NULL, (int) NULL, (struct sigcontext *) NULL,
		       (char *) NULL);
}



/* 
 * Popdown the warning window.
 */

void QuitWarning (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    if (ISID (xalarm.warningid)) {
	XtRemoveTimeOut (xalarm.warningid);
	xalarm.warningid = NONID;
    }
    XtPopdown ((Widget) clientdata);
}
