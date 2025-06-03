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
 * Main bit of prog, just deal with parsing command line options etc 
 * (well, letting XtAppInitialize() do it) & setting up stuff by 
 * calling out to other fns.
 */



#include "xalarm.h"


int		main();
void		DoAlarm();
static void	SetAlarm();
extern Instance	PreParseArgList();
extern String   Concat();
extern SIGRET	ResetAlarm(), KillAlarm();
extern void	Initialise(), StartDaemon(), CreateAlarmWidget(), PopupAndAsk(),
		AddTimeOuts(), SetWarningTimes(), AlarmDying(), RestartDiedAlarms(),
		EnteredTime(), EnteredDate(), EnteredWarnings(), Confirmed(),
		ShowSnoozeValue(), ShowClickToZero(), Quit();
extern int	ParseAlarmFile();
extern long	TimeToMilliSeconds();
extern unsigned long DateToMilliSeconds();



static XtResource resources[] = {
    {XtNtime,		XtCTime,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, time),	XtRString,		(XtPointer) ""},
    {XtNdate,		XtCDate,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, date),	XtRString,		(XtPointer) ""},
    {XtNwarnings,	XtCWarnings,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, warnings),	XtRString,		(XtPointer) ""},
    {XtNwarningwords,	XtCWarningwords,XtRInt,			sizeof (int),
     XtOffsetOf (ApplData,warningwords),XtRImmediate,		(XtPointer) 0},
    {XtNpester,		XtCPester,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, pester),	XtRString,		(XtPointer) "0"},
    {XtNsnooze,		XtCSnooze,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, snooze),	XtRString,		(XtPointer) "0"},
    {XtNconfirm,	XtCConfirm,	XtRBoolean,		sizeof (Boolean),
     XtOffsetOf (ApplData, confirm),	XtRImmediate,		(XtPointer) True},
    {XtNkill,		XtCKill,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, kill),	XtRString,		(XtPointer) NULL},
    {XtNreset,		XtCReset,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, reset),	XtRString,		(XtPointer) NULL},
    {XtNlist,		XtCList,	XtRBoolean,		sizeof (Boolean),
     XtOffsetOf (ApplData, list),	XtRImmediate,		(XtPointer) False},
    {XtNvolume,		XtCVolume,	XtRInt,			sizeof (int),
     XtOffsetOf (ApplData, volume),	XtRImmediate,		(XtPointer) 50},
    {XtNversion,	XtCVersion,	XtRBoolean,		sizeof (Boolean),
     XtOffsetOf (ApplData, version),	XtRImmediate,		(XtPointer) False},
    {XtNhelp,		XtCHelp,	XtRBoolean,		sizeof (Boolean),
     XtOffsetOf (ApplData, help),	XtRImmediate,		(XtPointer) False},
    {XtNname,		XtCName,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, proggie),	XtRString,		(XtPointer) "xalarm"},
    {XtNgeometry,	XtCGeometry,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, geometry),	XtRString,		(XtPointer) NOGEOMETRY},
    {XtNalarmaudio,	XtCAlarmaudio,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, alarmaudio),	XtRString,		(XtPointer) ""},
    {XtNwarnaudio,	XtCWarnaudio,	XtRString,		sizeof (String),
     XtOffsetOf (ApplData, warnaudio),	XtRString,		(XtPointer) ""},
    {XtNquiet,		XtCQuiet,	XtRBoolean,		sizeof (Boolean),
     XtOffsetOf (ApplData, quiet),	XtRImmediate,		(XtPointer) False},
};
	    


static XrmOptionDescRec options[] = {
    {"-time",		".time",	XrmoptionSepArg,	(XtPointer) ""},
    {"-t",		".time",	XrmoptionSepArg,	(XtPointer) ""},
    {"-date",		".date",	XrmoptionSepArg,	(XtPointer) ""},
    {"-warn",		".warnings",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-w",		".warnings",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-nowarn",		".warnings",	XrmoptionNoArg,		(XtPointer) ""},
    {"-nw",		".warnings",	XrmoptionNoArg,		(XtPointer) ""},
    {"-warnwords",	".warningwords",XrmoptionSepArg,	(XtPointer) NULL},
    {"-ww",		".warningwords",XrmoptionSepArg,	(XtPointer) NULL},
    {"-nowarnwords",	".warningwords",XrmoptionNoArg,		(XtPointer) "0"},
    {"-nww",		".warningwords",XrmoptionNoArg,		(XtPointer) "0"},
    {"-confirm",	".confirm",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-c",		".confirm",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-noconfirm",	".confirm",	XrmoptionNoArg,		(XtPointer) "False"},
    {"-nc",		".confirm",	XrmoptionNoArg,		(XtPointer) "False"},
    {"-pester",		".pester",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-p",		".pester",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-nopester",	".pester",	XrmoptionNoArg,		(XtPointer) "0"},
    {"-np",		".pester",	XrmoptionNoArg,		(XtPointer) "0"},
    {"-snooze",		".snooze",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-s",		".snooze",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-list",		".list",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-l",		".list",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-kill",		".kill",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-reset",		".reset",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-r",		".reset",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-alarmaudio",	".alarmaudio",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-aa",		".alarmaudio",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-noalarmaudio",	".alarmaudio",	XrmoptionNoArg,		(XtPointer) QUIET},
    {"-naa",		".alarmaudio",	XrmoptionNoArg,		(XtPointer) QUIET},
    {"-warningaudio",	".warningaudio",XrmoptionSepArg,	(XtPointer) NULL},
    {"-warnaudio",	".warningaudio",XrmoptionSepArg,	(XtPointer) NULL},
    {"-wa",		".warningaudio",XrmoptionSepArg,	(XtPointer) NULL},
    {"-nowarningaudio",	".warningaudio",XrmoptionNoArg,		(XtPointer) QUIET},
    {"-nowarnaudio",	".warningaudio",XrmoptionNoArg,		(XtPointer) QUIET},
    {"-nwa",		".warningaudio",XrmoptionNoArg,		(XtPointer) QUIET},
    {"-quiet",		".quiet",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-q",		".quiet",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-volume",		".volume",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-v",		".volume",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-version",	".version",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-help",		".help",	XrmoptionNoArg,		(XtPointer) "True"},
    {"-geometry",	".geometry",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-geom",		".geometry",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-g",		".geometry",	XrmoptionSepArg,	(XtPointer) NULL},
    {"-d",		".display",	XrmoptionSepArg,	(XtPointer) NULL},
};



static XtActionsRec actions[] = {
    {"Quit",		(XtActionProc) Quit},
    {"EnteredTime",	(XtActionProc) EnteredTime},
    {"EnteredDate",	(XtActionProc) EnteredDate},
    {"EnteredWarnings",	(XtActionProc) EnteredWarnings},
    {"Confirmed",	(XtActionProc) Confirmed},
    {"ShowClickToZero",	(XtActionProc) ShowClickToZero},
    {"ShowSnoozeValue",	(XtActionProc) ShowSnoozeValue}
};



AlarmData       xalarm;



/* 
 * The main entry.  DoAlarm() and StartDaemon() never return.
 */

int main (argc, argv)
  int 	   argc;
  String  *argv;
{
    Instance   instance;
    String    *args;
    int        days;

    instance = PreParseArgList (argv, &argc, &days, &args);

    RestartDiedAlarms (args, argc);

    switch (instance) {
     case RestartOnly:
	break;
     case Alarm:
	xalarm.saveonshutdown = True;
	DoAlarm (args, argc);
     case Daemon:
	xalarm.saveonshutdown = False;
	StartDaemon (days, args, argc);
     case File:
	xalarm.saveonshutdown = False;
	(void) ParseAlarmFile (days, args, argc);
    }

    return (0);
}



/* 
 * Do the actual alarm as given.
 */

void DoAlarm (argv, argc)
  String  *argv;
  int 	   argc;
{
    ApplData   data;

    xalarm.toplevel = XtAppInitialize (&xalarm.appcon, "XAlarm",
				       options, XtNumber (options), &argc, argv,
				       (String *) NULL, (ArgList) NULL, 0);

    XtGetApplicationResources (xalarm.toplevel, &data, resources, XtNumber (resources),
			       (ArgList) NULL, 0);

    XtAppAddActions (xalarm.appcon, actions, XtNumber (actions));

    /* 
     * First initialise; exit or fork.
     * We have to make the alarm widget first, as SetAlarm() may popup 
     * a dialog box which may need to know the alarm widget's label.
     */

    Initialise (*argv, &data);
    CreateAlarmWidget (Concat (argv, argc));
    SetAlarm (&data);

    XtAppMainLoop (xalarm.appcon);
}       
	       
	       

/* 
 * If the time is not given, pop up and ask for it, otherwise add the 
 * time outs.  Get the warnings first, tho.
 */

static void SetAlarm (data)
  ApplData  *data;
{
    char   time[TEXT];

    (void) sprintf (time, "+%s", data->pester);
    xalarm.pester = (int) (TimeToMilliSeconds (time) / MSECSIN1MIN);
    (void) sprintf (time, "+%s", data->snooze);
    xalarm.snooze = (int) (TimeToMilliSeconds (time) / MSECSIN1MIN);

    xalarm.warningwords = data->warningwords;
    xalarm.confirm = data->confirm;
    xalarm.proggie = data->proggie;
    xalarm.geometry = data->geometry;
    xalarm.volume = (data->volume - 50) * 2;
    if (data->quiet)
	xalarm.alarmaudio = xalarm.warningaudio = QUIET;
    else {
	xalarm.alarmaudio = data->alarmaudio;
	xalarm.warningaudio = data->warnaudio;
    }

    xalarm.warningwidget = NONWIDGET;
    xalarm.whenid = xalarm.warningid = xalarm.alarmid = xalarm.pesterid = NONID;

    /* 
     * Get the date & time from the date & time string,
     * and the warnings from the warnings string...
     */
    xalarm.dateout = DateToMilliSeconds (xalarm.datestr = data->date);
    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr = data->time);
    SetWarningTimes (xalarm.warningsstr = data->warnings);

    /* 
     * If ``invalid'' (may not have been given anyway), popup.
     * If ok, but we want confirmation, popup to check.  (this is cheating!)
     * There is no waiting for confirmation if the timeout is 0 - the user
     * will soon realise that there was something wrong...
     */
    if ((ISINVALID (xalarm.dateout)) or
	(ISINVALID (xalarm.timeout)) or
	(ISINVALID (xalarm.numwarnings)) or
	(xalarm.confirm and ((xalarm.timeout != 0) or (xalarm.dateout != 0))))
	PopupAndAsk ();
    else
	AddTimeOuts ();

    (void) XSetIOErrorHandler ((void *) AlarmDying);
    (void) signal (SIGUSR1, (SIGRET (*)()) ResetAlarm);
    (void) signal (SIGUSR2, (SIGRET (*)()) KillAlarm);
}
