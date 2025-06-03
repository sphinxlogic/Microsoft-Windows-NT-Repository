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

#ifndef xalarm_h
#define xalarm_h

/* 
 * Bits & bobs #defines, needed by most/all bits & bobs files.
 */

#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <stdio.h>
#include <ctype.h>
#include <signal.h>

#include "patchlevel.h"

/* 
 * Type of instantiation of xalarm.
 */

typedef enum {
    Alarm, File, Daemon, RestartOnly
} Instance;

/* 
 * Application data - from the parsing of the command line args.
 */

typedef struct {
    String    kill, reset, snooze, proggie, geometry, time, date, warnings, pester,
	      warnaudio, alarmaudio;
    Boolean   confirm, list, quiet, version, help;
    int       warningwords, volume;
} ApplData;


/* 
 * Global xalarm data, stuffed under one structure.
 */

typedef struct {
    XtAppContext    appcon;
    XtIntervalId    timeoutid[100], whenid, warningid, alarmid, pesterid;
    Widget 	    toplevel, alarmwidget, messagewidget, snoozetimebutton,
		    dialog, gettimewidget, getdatewidget, getwtimewidget, confirmwidget,
		    warningwidget, savebutton;
    Boolean 	    confirm, saveonshutdown;
    String 	    proggie, geometry, messagestr, timestr, datestr, warningsstr,
		    warningaudio, alarmaudio;
    char	    errormessage[1024];
    unsigned long   dateout, warnings[100], settime, offtime;
    long	    timeout;
    int 	    snooze, numwarnings, warningwords, pester, volume;
} AlarmData;

/* 
 * macro names...
 */

#define		XtNtime		"time"
#define		XtCTime		"Time"
#define		XtNdate		"date"
#define		XtCDate		"Date"
#define		XtNwarnings	"warnings"
#define		XtCWarnings	"Warnings"
#define		XtNwarningwords	"warningwords"
#define		XtCWarningwords "Warningwords"
#define		XtNpester	"pester"
#define		XtCPester	"Pester"
#define		XtNsnooze	"snooze"
#define		XtCSnooze	"Snooze"
#define		XtNconfirm	"confirm"
#define		XtCConfirm	"Confirm"
#define		XtNreset	"reset"
#define		XtCReset	"Reset"
#define		XtNlist		"list"
#define		XtCList		"List"
#define		XtNkill		"kill"
#define		XtCKill		"Kill"
#define		XtNalarmaudio	"alarmaudio"
#define		XtCAlarmaudio	"Alarmaudio"
#define		XtNwarnaudio	"warningaudio"
#define		XtCWarnaudio	"Warningaudio"
#define		XtNquiet	"quiet"
#define		XtCQuiet	"Quiet"
#define		XtNvolume	"volume"
#define		XtCVolume	"Volume"
#define		XtNversion	"version"
#define		XtCVersion	"Version"
#define		XtNhelp		"help"
#define		XtCHelp		"Help"

/* 
 * A R4 problem, I think, not apparent in R5.
 */

#ifndef XtNname
#define		XtNname		"name"
#endif
#ifndef XtCName
#define		XtCName		"Name"
#endif

/* 
 * This is cos of a bug of some sort spotted on a Sparc 2 (SunOS 4.1.1), X11r4,
 * whereby X crashes if given a zero timeout.  Just woof it up a bit.
 */

#if defined (SparcArchitecture)
#define		TIMEOUT(t)	((unsigned long) ((t) < 100 ? 100 : (t)))
#else
#define		TIMEOUT(t)	((unsigned long) (t))
#endif

/* 
 * And the rest...
 */

#define		CreateManagedButton(widget, name, parent, callback, clientdata) \
    widget = XtVaCreateManagedWidget (name, commandWidgetClass, parent, NULL); \
    XtAddCallback (widget, XtNcallback, (XtCallbackProc) callback, \
		   (XtPointer) clientdata)

#define		AddValueCallback(widget, callback, clientdata) { \
    Widget   source; \
    XtVaGetValues (XtNameToWidget (widget, "value"), XtNtextSource, &source, NULL); \
    XtAddCallback (source, XtNcallback, (XtCallbackProc) callback, \
		   (XtPointer) clientdata); \
    }

#if defined (SIGNALRETURNSINT)
#define		SIGRET		int
#else
#define		SIGRET		void
#endif

#if defined (FORKRETURNSSHORT)
#define		FORKRET		short
#elif defined (FORKRETURNSLONG)
#define		FORKRET		long
#elif defined (FORKRETURNSPID_T)
#define		FORKRET		pid_t
#else
#define		FORKRET		int
#endif

#define		or		||
#define		and		&&
#define		not		!

#define		XALARMFILE	".xalarms"
#define		NOTKNOWN	"UNKNOWN"
#define		NOGEOMETRY	"NOGEOM"
#define		ALL		"all"
#define		BELL		"bell"
#define		BEEP		"beep"
#define		QUIET		"quiet"
#define		INVALID		(-1)
#define		NONWIDGET	((Widget) -1)
#define		NONID		((XtIntervalId) -1)
#define		DAEMONWEEKLY	(INVALID-1)
#define		TEXT		1024

#define		LEAPYEAR(y)	(((y) % 400 == 0) or \
				 (((y) % 4 == 0) and ((y) % 100 != 0)))
#define		DAYSINFEB(y)	((LEAPYEAR ((y))) ? 29 : 28)
#define		DAYSINYEAR(y)	((LEAPYEAR ((y))) ? 366 : 365)

#define		SECSIN1MIN	(60)
#define		SECSIN1HR	(60*SECSIN1MIN)
#define		SECSIN1DAY	(24*SECSIN1HR)

#define		MSECSIN1SEC	(1000)
#define		MSECSIN1MIN	(60*MSECSIN1SEC)
#define		MSECSIN1HR	(60*MSECSIN1MIN)
#define		MSECSIN1DAY	(24*MSECSIN1HR)

#define		ISVALID(x)	((x) != (INVALID))
#define		ISINVALID(x)	((x) == (INVALID))
#define		ISWIDGET(x)	((x) != (NONWIDGET))
#define		ISNONWIDGET(x)	((x) == (NONWIDGET))
#define		ISID(x)		((x) != (NONID))
#define		ISNONID(x)	((x) == (NONID))
#define		ISWEEKLYD(x)	((x) == (DAEMONWEEKLY))

#define		PLURAL(x)	((((int) (x)) == 1) ? "" : "s")
#define		ENDOF(str)	((str) + strlen (str))

#define		SUMTIMEOUTS(a, b) \
    (unsigned long) (((b) < 0) ? (a) - (unsigned long) (-b) : (a) + (unsigned long) (b))

#define		MAX(x, y)	((x) > (y) ? (x) : (y))
#define		MIN(x, y)	((x) < (y) ? (x) : (y))

#define		STREQUAL(a, b)	(strncmp ((a), (b), strlen (b)) == 0)
#define		ERRORIS(error)	(STREQUAL (xalarm.errormessage, error))
#define		NIL(type, x)	((x) == (type) NULL)
#define		NONNIL(type, x)	((x) != (type) NULL)

#define		MAKETIMESTRING(buf, mins) \
    				if ((mins) >= 60) \
    (void) sprintf (buf, "%d:%02d hr%s", (int) (mins)/60, (int) (mins)%60, \
		    PLURAL ((mins)/60)); \
				else \
    (void) sprintf (buf, "%d min%s", (int) (mins), PLURAL (mins))

#define		MAKETIME(buf, mins) \
    				if ((mins) >= 60) \
    (void) sprintf (buf, "%d:%02d", (int) (mins)/60, (int) (mins)%60); \
				else \
    (void) sprintf (buf, "%d", (int) (mins))

#define		RESETERROR()	xalarm.errormessage[0] = '\0'

#define		ADDERROR(message, madeto) \
    if (NIL (String, madeto)) \
	(void) sprintf (xalarm.errormessage, "%s\n", message); \
    else if (*madeto == '\0') \
	(void) sprintf (xalarm.errormessage, "%s  (at end)\n", message); \
    else \
	(void) sprintf (xalarm.errormessage, "%s  (at `%s')\n", message, madeto)

/*
 * DON'T PUT ANYTHING AFTER THIS!!!
 */
#endif
