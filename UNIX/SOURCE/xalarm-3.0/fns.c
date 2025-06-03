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
 * Just a few bits and bobs.
 */


#include "xalarm.h"
#include "fns.h"

#if defined (USEGETPWUID)
#include <pwd.h>
#else
extern String	getenv();
#endif

#if defined (USEUNAME)
#include <sys/utsname.h>
#endif


#define		FILEOPTION1	"-file"
#define		FILEOPTION2	"-f"
#define		DAEMONOPTION1	"-daemon"
#define		DAEMONOPTION2	"-demon"
#define		RESTARTONLY1	"-restartonly"
#define		RESTARTONLY2	"-restart"

#define		COMMANDLIST	"echo xalarms: `%s`"
#define		COMMANDRESET	"%s -USR1 `%s`"
#define		COMMANDKILL	"%s -USR2 `%s`"
#define		DOWNCASE(ch)	(((ch) >= 'A') and ((ch) <= 'Z') ? \
				 ((ch) - 'A' + 'a') : (ch))



String		Concat(), ReplaceNewlines(), NextWord(), DisplayName(),
		UserName(), HomeDirectory(), MachineName();
Boolean		IsInteger();
Instance	PreParseArgList();
XtIntervalId	ResetTracker();
SIGRET		KillAlarm();
void		Initialise(), AddTimeOuts(), SetWMName(), StartDaemon(),
		SetGeometry(), EnsureNotGrownOffScreen(), Audio(), Quit();
static void	CentreWidgetUnderPointer(), ParseGeometry();
static SIGRET	KillDaemon();
extern void	DoAlarm(), Warning(), WakeUp(), PopupAndAsk();
/* extern FORKRET	fork(); */
extern int	ParseAlarmFile(), DaysTo();
extern long	TimeToMilliSeconds();
extern unsigned long DateToMilliSeconds();
extern time_t	time();
extern struct tm *localtime();



extern AlarmData   xalarm;


/* 
 * Just give a bit of help etc.
 * Fork off a child otherwise.
 */

void Initialise (proggie, data)
  String     proggie;
  ApplData  *data;
{
    char   getpids[TEXT], command[TEXT];

    if (NIL (String, data->proggie))
	data->proggie = proggie;

    if (data->version)
	(void) printf ("%s: using xalarm version %s\n", data->proggie, XALARMVERSION);

    if (data->help) {
	(void) printf ("Usage: %s [options] [text]\n\n", data->proggie);
	(void) printf ("Default values for these resources/options are used by %s,\n\t",
		       data->proggie);
	(void) printf ("but may be over-ruled.\n");
	(void) printf ("Usual X options, plus:\n\t");
	(void) printf ("-file +days|date\tSet alarms up to date from alarm file(s).\n\t");
	(void) printf ("-daemon +days|date\tStart a daemon to look at alarm file(s).\n\t");
	(void) printf ("-date +days|date\tDate at which to trigger %s.\n\t",
		       data->proggie+1);
	(void) printf ("-time +time|time\tTime at which to trigger %s.\n\t",
		       data->proggie+1);
	(void) printf ("-[no]confirm\t\t[Don't] ask for confirmation.\n\t");
	(void) printf ("-warn time[,time...]\tTimes before %s for warnings.\n\t",
		       data->proggie+1);
	(void) printf ("-warnwords number\tWords from %s message in warning.\n\t",
		       data->proggie+1);
	(void) printf ("-list\t\t\tList xalarm process numbers (pids).\n\t");
	(void) printf ("-reset pid|all\t\tReset xalarm process number pid/all xalarms.\n\t");
	(void) printf ("-kill pid|all\t\tKill xalarm process number pid/all xalarms.\n\t");
	(void) printf ("-restart\t\tOnly try to restart previous alarms.\n\t");
	(void) printf ("-alarmaudio method\tWhat noise to make on the alarm.\n\t");
	(void) printf ("-warningaudio method\tWhat noise to make on warnings.\n\t");
	(void) printf ("-volume percentage\tPercentage volume for the terminal bell.\n\t");
	(void) printf ("-quiet\t\t\tDon't make a sound.\n\t");
	(void) printf ("-pester time\t\tTime after which %s re-triggers.\n\t",
		       data->proggie+1);
	(void) printf ("-snooze time\t\tSet initial alarm snooze time value.\n\t");
	(void) printf ("-nowarn\t\t\tNo warnings.\n\t");
	(void) printf ("-nowarnwords\t\tNo words from %s message in warning.\n\t",
		       data->proggie+1);
	(void) printf ("-nopester\t\tDon't persecute after %s triggers.\n",
		       data->proggie+1);
    }

    if (data->list) {
	(void) sprintf (getpids, GETXALARMPIDS, getpid ());
	(void) sprintf (command, COMMANDLIST, getpids);
	if (system (command) != 0)
	    perror (data->proggie);
    }

    if (NONNIL (String, data->reset))
	if (not STREQUAL (data->reset, ALL)) {
	    if (kill (atoi (data->reset), SIGUSR1) == -1)
		perror (data->proggie);
	} else {
	    (void) sprintf (getpids, GETXALARMPIDS, getpid ());
	    (void) sprintf (command, COMMANDRESET, KILLPATH, getpids);
	    if (system (command) != 0)
		perror (data->proggie);
	}

    if (NONNIL (String, data->kill))
	if (not STREQUAL (data->kill, ALL)) {
	    if (kill (atoi (data->kill), SIGUSR2) == -1)
		perror (data->proggie);
	} else {
	    (void) sprintf (getpids, GETXALARMPIDS, getpid ());
	    (void) sprintf (command, COMMANDKILL, KILLPATH, getpids);
	    if (system (command) != 0)
		perror (data->proggie);
	}

    if ((data->list) or (data->version) or (data->help) or
	(NONNIL (String, data->kill)) or (NONNIL (String, data->reset)))
	exit (0);

    /* 
     * Fork & exit the parent.  If the fork fails, carry on anyway.
     * Is this OK?  Should we sleep a bit first?
     */

    switch ((int) fork ()) {
     case -1:
	perror (data->proggie);
     case 0:
	break;
     default:
	exit (0);
    }
}



/* 
 * Check to see if we're to do the appointments thing, and copy out 
 * the arg list, then clear the original to prevent others snooping.
 */

Instance PreParseArgList (argv, argc, days, args)
  String  *argv, **args;
  int 	  *argc, *days;
{
    String     ch;
    Instance   instance = Alarm;
    int        last = *argc, i;

    *args = (String *) XtCalloc (*argc+4, sizeof (String *));
    (*args)[0] = XtNewString (argv[0]);
    *argc = 1;

    for (i=1; i<last; i++)
	if ((STREQUAL (argv[i], RESTARTONLY1)) or (STREQUAL (argv[i], RESTARTONLY2)))
	    instance = RestartOnly;
	else if ((STREQUAL (argv[i], FILEOPTION1)) or
		 (STREQUAL (argv[i], FILEOPTION2))) {
	    if (++i == last) {
		(void) fprintf (stderr, "No file date to parse alarm file with.\n");
		exit (-1);
	    } else if (ISINVALID (*days = DaysTo (argv[i], instance = File))) {
		(void) fprintf (stderr, "Can't use file date: %s", xalarm.errormessage);
		exit (-1);
	    }
	} else if ((STREQUAL (argv[i], DAEMONOPTION1)) or
		 (STREQUAL (argv[i], DAEMONOPTION2))) {
	    if (++i == last) {
		(void) fprintf (stderr, "No daemon date to parse alarm file with.\n");
		exit (-1);
	    } else if (ISINVALID (*days = DaysTo (argv[i], instance = Daemon))) {
		(void) fprintf (stderr, "Can't use daemon date: %s", xalarm.errormessage);
		exit (-1);
	    } else if (*days == 0) {
		(void) fprintf (stderr, "Zero days means daemon would not sleep!\n");
		exit (-1);
	    }
	} else
	    /* Just copy the option.
	     */
	    (*args)[(*argc)++] = XtNewString (argv[i]);

    for (i=1; i<last; i++) {
	ch = argv[i];
	while (*ch != '\0')
	    *(ch++) = ' ';
    }

    return (instance);
}



/* 
 * Fork off an xalarm process which will repeatedly parse the
 * alarm file.
 */

void StartDaemon (days, argv, argc)
  String  *argv;
  int 	   days, argc;
{
    Display    *display;
    String 	displayname = DisplayName (argv, argc);
    int 	pid, kids;
    time_t 	abstime;
    struct tm  *now;

    /* 
     * Fork off the daemon.
     */

    switch (pid = (int) fork ()) {
     case -1:
	perror ("xalarm");
	exit (-1);
     case 0:
	(void) signal (SIGUSR1, SIG_IGN);
	(void) signal (SIGUSR2, (SIGRET (*)()) KillDaemon);
	break;
     default:
	(void) printf ("xalarm: Daemon started (%d).\n", pid);
	exit (0);
    }

    /* 
     * Open the display for the daemon and parse & sleep until we 
     * loose the connection.
     */

    if (NONNIL (Display *, (display = XOpenDisplay (displayname)))) {
	(void) time (&abstime);
	now = localtime (&abstime);

	while (True) {
	    /* 
	     * Start any alarms in the alarm file, then wait for them 
	     * to finish.
	     */
	    if (ISWEEKLYD (days))
		kids = ParseAlarmFile (7 - now->tm_wday, argv, argc);
	    else
		kids = ParseAlarmFile (days - 1, argv, argc);

	    while (kids-- > 0)
		(void) wait ((int *) NULL);

	    /* 
	     * Sleep until the next time, then test the connection.
	     */
	    (void) time (&abstime);
	    now = localtime (&abstime);
	    (void) sleep ((unsigned)
		(30 + ((ISWEEKLYD (days) ?
			(days = 7) - now->tm_wday + 1 : days) * SECSIN1DAY) -
		 (now->tm_hour * SECSIN1HR) - (now->tm_min * SECSIN1MIN) - now->tm_sec));

	    XSync (display, False);
	}
    }
}



/* 
 * Signal handler for a daemon - it just kills the process.
 */

static SIGRET KillDaemon (sig, code, scp, addr)
  int 		      sig, code;
  struct sigcontext  *scp;
  char 		     *addr;
{
    (void) fprintf (stderr, "xalarm: Daemon killed (%d).\n", getpid ());
    exit (0);
}



/* 
 * Signal handler for a normal alarm - it just kills the process.
 */

SIGRET KillAlarm (sig, code, scp, addr)
  int 		      sig, code;
  struct sigcontext  *scp;
  char 		     *addr;
{
    Quit ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}



/* 
 * We add the time outs for the alarm & warnings.
 * The warnings are added only if there is time enuf to go.
 * ie. if "-time +10 -warn 15" you will get nowt.
 * "-time +10 -warn 9" will, however, give you a warning in 1 min.
 */

void AddTimeOuts ()
{
    unsigned long   timeout = SUMTIMEOUTS (xalarm.dateout, xalarm.timeout);
    int 	    i;
    
    xalarm.settime = (unsigned long) time ((time_t *) NULL);
    xalarm.offtime = xalarm.settime + (timeout / MSECSIN1SEC);

    for (i=0; i<xalarm.numwarnings; i++)
	if (timeout > xalarm.warnings[i])
	    xalarm.timeoutid[i] =
		XtAppAddTimeOut (xalarm.appcon,
				 TIMEOUT (timeout - xalarm.warnings[i]),
				 (XtTimerCallbackProc) Warning,
				 (XtPointer) (xalarm.warnings[i] / MSECSIN1MIN));
    xalarm.timeoutid[xalarm.numwarnings] =
	XtAppAddTimeOut (xalarm.appcon, TIMEOUT (timeout),
			 (XtTimerCallbackProc) WakeUp, (XtPointer) NULL);
}
  
   

/* 
 * Set the window manager name of the given widget.
 */

void SetWMName (widget, name)
  Widget   widget;
  String   name;
{
    Display 	   *display = XtDisplay (xalarm.toplevel);
    String 	    wmname[1];
    XTextProperty   property;

    if (not XtIsRealized (widget))
	XtRealizeWidget (widget);

    wmname[0] = name;
    if (XStringListToTextProperty (wmname, 1, &property)) {
	XSetWMName (display, XtWindow (widget), &property);
	XSetWMIconName (display, XtWindow (widget), &property);
	XtFree (property.value);
    }
}



/* 
 * Reset the tracker so it calls itself.
 */

XtIntervalId ResetTracker (tracker, clientdata, triggered)
  XtTimerCallbackProc 	tracker;
  XtPointer 		clientdata;
  int 			triggered;
{
    time_t 	now;
    struct tm  *clock;

    (void) time (&now);
    clock = localtime (&now);
    /*
     * 60 seconds from when it triggered.
     */
    return (XtAppAddTimeOut (xalarm.appcon, 
			     TIMEOUT (MSECSIN1SEC * (60 - clock->tm_sec + triggered)),
			     tracker, clientdata));
}



/* 
 * Set the geometry of the given widget.
 */

void SetGeometry (widget)
  Widget   widget;
{
    Dimension 	width, height, borderwidth;
		
    if (not XtIsRealized (widget))
	XtRealizeWidget (widget);

    XtVaGetValues (widget, XtNwidth, &width, XtNheight, &height,
		   XtNborderWidth, &borderwidth, NULL);

    if (STREQUAL (xalarm.geometry, NOGEOMETRY))
	CentreWidgetUnderPointer (widget, width, height, borderwidth);
    else
	ParseGeometry (widget, width, height, borderwidth, xalarm.geometry);
}



/* 
 * Move the given widget so that it is directly underneath the pointer.
 */

static void CentreWidgetUnderPointer (widget, width, height, borderwidth)
  Widget      widget;
  Dimension   width, height, borderwidth;
{
    Window 	   root, child;
    int 	   x, y, dummy;
    unsigned int   mask;

    width += (borderwidth * 2);
    height += (borderwidth * 2);

    if (XQueryPointer (XtDisplay (widget), XtWindow (widget),
		       &root, &child, &x, &y, &dummy, &dummy, &mask)) {
	x = MAX (0, MIN (x - ((int) width / 2),
			 XWidthOfScreen (XtScreen (widget)) - (int) width));
	y = MAX (0, MIN (y - ((int) height / 2),
			 XHeightOfScreen (XtScreen (widget)) - (int) height));
	XtVaSetValues (widget, XtNx, (XtArgVal) x, XtNy, (XtArgVal) y, NULL);
    }
}



/* 
 * Move +/ resize the given widget so that it has the given geometry.
 */

static void ParseGeometry (widget, currentwidth, currentheight, borderwidth, geometry)
  Widget      widget;
  Dimension   currentwidth, currentheight, borderwidth;
  String      geometry;
{
    unsigned int   width = (unsigned int) currentwidth,
		   height = (unsigned int) currentheight;
    int 	   x = 0, y = 0, mask;

    mask = XParseGeometry (geometry, &x, &y, &width, &height);

    if (mask & WidthValue)
	XtVaSetValues (widget, XtNwidth, (XtArgVal) width, NULL);
    if (mask & HeightValue)
	XtVaSetValues (widget, XtNheight, (XtArgVal) height, NULL);
    if (mask & (WidthValue | HeightValue))
	XtVaSetValues (widget, XtNallowShellResize, (XtArgVal) False, NULL);

    if (not (mask & (XValue | YValue)))
	CentreWidgetUnderPointer (widget, width, height, borderwidth);
    else {
	if (mask & XNegative)
	    x = x + XWidthOfScreen (XtScreen (widget)) - (int) width;
	if (mask & YNegative)
	    y = y + XHeightOfScreen (XtScreen (widget)) - (int) height;
	XtVaSetValues (widget, XtNx, (XtArgVal) x, XtNy, (XtArgVal) y, NULL);
    }
}



/* 
 * Make sure that the widget's right side is visible.
 * For resizing popups.
 */

void EnsureNotGrownOffScreen (widget)
  Widget   widget;
{
    Position 	x;
    Dimension 	width, borderwidth, screenwidth = XWidthOfScreen (XtScreen (widget));

    XtVaGetValues (widget, XtNx, &x, XtNwidth, &width, XtNborderWidth, &borderwidth,
		   NULL);
    width += (borderwidth * 2);
    if (screenwidth > width)
	XtVaSetValues (widget, XtNx, (XtArgVal) MIN (x, screenwidth - width), NULL);
}



/* 
 * Pull out any display arg.
 */

String DisplayName (argv, argc)
  String  *argv;
  int 	   argc;
{
    String   displayname = (String) NULL;
    int      i;

    for (i=1; i<argc; i++)
	if ((strcmp (argv[i], "-display") == 0) or (strcmp (argv[i], "-d") == 0))
	    if (i < argc-1)
		displayname = argv[i+1];
	    else {
		(void) printf ("xalarm: No display to open.\n");
		exit (-1);
	    }

    return (displayname);
}



/* 
 * Make a sound, if that's what's wanted.
 */
void Audio (sound)
  String   sound;
{
    if ((STREQUAL (sound, BELL)) or (STREQUAL (sound, BEEP)))
	XBell (XtDisplay (xalarm.toplevel), xalarm.volume);
    else if (not STREQUAL (sound, QUIET))
	if (system (sound) != 0)
	    perror (sound);
}



/* 
 * Returns a string concated from the given array of strings.
 * Separates the strings with a newline.
 *
 * Hacked from various books on X & Xt.
 */

String Concat (strings, n)
  String  *strings;
  int 	   n;
{
    String 	   buffer;
    unsigned int   i, len = 0;

    if (n <= 1)
	return ((String) NULL);

    for (i=1; i<n; i++)
	len += strlen (strings[i]);
    len += (n-1);

    buffer = XtMalloc (len+1);
    buffer[0] = '\0';
    for (i=1; i<n; i++) {
	if (i > 1)
	    (void) strcat (buffer, "\n");
	(void) strcat (buffer, strings[i]);
    }

    return (buffer);
}



/* 
 * Replace each newline by a space.  Returns the new string.
 */

String ReplaceNewlines (str)
  String   str;
{
    String   s, newstr = XtNewString (str);

    if (NONNIL (String, s = newstr))
	do
	    if (*s == '\n')
		*s = ' ';
	while (*(s++) != '\0');

    return (newstr);
}



/* 
 * Return the next word in str, starting at position chpos in the 
 * string.  The returned word is in lower case.
 */

String NextWord (str, chpos)
  String   str;
  int 	  *chpos;
{
    String   word;
    int      start, i;

    while (isspace (str[*chpos]))
	(*chpos)++;

    start = *chpos;
    while ((not isspace (str[*chpos])) and (str[*chpos] != '\0'))
	(*chpos)++;

    if ((str[*chpos] == '\0') and (start == *chpos))
	return ("");
    else {
	word = XtMalloc (*chpos - start + 1);
	for (i=0; i<(*chpos-start); i++)
	    word[i] = DOWNCASE (str[start+i]);
	word[*chpos-start] = '\0';
	
	return (word);
    }
}



/* 
 * Returns true iff the string contains a valid integer.
 */

Boolean IsInteger (str)
  String   str;
{
    if (*str == '+')
	str++;
    while (*str != '\0')
	if (not isdigit (*str++))
	    return (False);
    return (True);
}



/* 
 * Return the user, users' home directory & host names.
 */

String UserName ()
{
    static String   name = (String) NULL;

    if (NIL (String, name))
#if defined (USEGETLOGIN)
	name = (String) getlogin ();
#elif defined (USECUSERID)
	(void) cuserid (name = XtMalloc ((Cardinal) L_cuserid + 1));
#elif defined (USEGETPWUID)
	name = XtNewString (getpwuid (getuid ())->pw_name);
#else
	name = getenv ("USER");
#endif

    return ((NIL (String, name)) ? NOTKNOWN : name);
}



String HomeDirectory ()
{
    static String   name = (String) NULL;

    if (NIL (String, name))
#if defined (USEGETPWUID)
	name = XtNewString (getpwuid (getuid ())->pw_dir);
#else
	name = getenv ("HOME");
#endif

    return ((NIL (String, name)) ? NOTKNOWN : name);
}



String MachineName ()
{
#if defined (USEGETHOSTNAME)
    static char   name[TEXT];

    return ((gethostname (name, TEXT) < 0) ? NOTKNOWN : name);
#elif defined (USEUNAME)
    static struct utsname   name;

    return ((uname (&name) < 0) ? NOTKNOWN : name.nodename);
#else
    return (NOTKNOWN);
#endif
}



/* 
 * This function generates a random number and stuffs it down the pipe.
 */

void Quit (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    XtDestroyApplicationContext (xalarm.appcon);
    exit (0);
}
