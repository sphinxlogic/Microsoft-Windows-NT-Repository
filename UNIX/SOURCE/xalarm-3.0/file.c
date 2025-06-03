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
 * Deal with the appointments alarm file.
 */


#include "xalarm.h"
#include "dates.h"

#include <X11/Xaw/Dialog.h>

/* 
 * We use fcntl() to lock files, or we can use flock() or lockf():
 */

#if defined (USEFLOCK)
#include <sys/file.h>
#elif defined (USELOCKF)
#include <unistd.h>
#elif not defined (SEEK_SET)
#define SEEK_SET 0
#endif


#define		ALARMFILEFORMAT		"\n%s %s %2d - %s\t%s\n"
#define		DIALOGVALUE(widget)	(XtNewString (XawDialogGetValueString (widget)))
#define		UPCASE(ch)		(((ch) >= 'a') and ((ch) <= 'z') ? \
					 ((ch) - 'a' + 'A') : (ch))


void		SaveAlarmSettings(), AlarmDying(), RestartDiedAlarms();
int		ParseAlarmFile();
static Boolean	SaveAlarm();
static void	RenameFile();
extern Boolean	AppointmentWithin();
extern long	TimeToMilliSeconds();
extern unsigned long DateToMilliSeconds();
extern String	ReplaceNewlines(), HomeDirectory(), DisplayName(), getenv();
extern void	DoAlarm(), EditedText(), MakeConfirmMessage(), EnsureNotGrownOffScreen();
/* extern FORKRET	fork(); */
extern time_t	time();
extern struct tm *localtime();



extern AlarmData	xalarm;



/* 
 * Parse each line of the alarm file, if it's around.  If the 
 * appointment is within that given, fork and do the alarm.  Returns
 * number forked.
 */

int ParseAlarmFile (days, argv, argc)
  String  *argv;
  int 	   days, argc;
{
    FILE     *file;
    String    timestr, datestr, alarmfilepath = getenv ("XALARMFILEPATH"),
	      home = HomeDirectory ();
    Boolean   failed = True;
    char      alarmpath[TEXT], filename[TEXT], line[TEXT];
    int       newargc, endofdate, start, finish = 0, kids = 0;
    
    (void) sprintf (alarmpath, "%s/.xalarms", HomeDirectory ());
    if (NONNIL (String, alarmfilepath))
	(void) sprintf (ENDOF (alarmpath), ":%s", alarmfilepath);

    do {
	/* 
	 * Try each file in the path list.
	 */
	start = finish;
	while ((alarmpath[finish] != '\0') and (alarmpath[finish] != ':'))
	    finish++;

	if (alarmpath[start] == '/') {
	    (void) strncpy (filename, alarmpath+start, finish-start);
	    filename[finish-start] = '\0';
	} else {
	    (void) sprintf (filename, "%s/", home);
	    (void) strncpy (ENDOF (filename), alarmpath+start, finish-start);
	    filename[finish-start + strlen (home)+1] = '\0';
	}

	if (NONNIL (FILE *, (file = fopen (filename, "r")))) {
	    /* 
	     * Deal with this file.
	     */
	    failed = False;

	    while (NONNIL (String, fgets (line, TEXT, file))) {
		endofdate = 0;
		if (AppointmentWithin (days, line, &timestr, &datestr, &endofdate)) {
		    while (isspace (line[endofdate]))
			endofdate++;
		    
		    newargc = argc;
		    argv[newargc++] = XtNewString ("-time");
		    argv[newargc++] = timestr;
		    argv[newargc++] = XtNewString ("-date");
		    argv[newargc++] = datestr;
		    
		    if (line[endofdate] != '\0') {
			line[strlen (line) - 1] = '\0';
			argv[newargc++] = line+endofdate;
		    }

		    switch ((int) fork ()) {
		     case -1:
			perror ("xalarm");
			exit (-1);
		     case 0:
			DoAlarm (argv, newargc);
		     default:
			kids++;
		    }
		}
	    }
	}
	
	while (alarmpath[finish] == ':')
	    finish++;
    } while (alarmpath[finish] != '\0');

    if (failed)
	fprintf (stderr, "Failed to open any of these alarm files:\n%s\n", alarmpath);

    return (kids);
}



/* 
 * Save the alarm settings in the alarm file.
 */

void SaveAlarmSettings (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    String   message = DIALOGVALUE (xalarm.confirmwidget);
    char     buf[TEXT], filename[TEXT];
    
    if (*message != '\0')
	XtVaSetValues (xalarm.messagewidget, XtNlabel,
		       (XtArgVal) (xalarm.messagestr = message), NULL);
    
    MakeConfirmMessage (buf);
    (void) sprintf (filename, "%s/%s", HomeDirectory (), XALARMFILE);
    if (not SaveAlarm (filename))
	(void) sprintf (ENDOF (buf), "\nCouldn't open %s", filename);
    else {
	(void) sprintf (ENDOF (buf), "\nSaved in %s", filename);
	EditedText ((Widget) NULL, (XtPointer) NULL, (XtPointer) False);
    }

    XtVaSetValues (xalarm.confirmwidget, XtNlabel, (XtArgVal) buf, NULL);
    EnsureNotGrownOffScreen (XtParent (XtParent (xalarm.confirmwidget)));
}



/* 
 * Save the alarm in the alarm-died file.  Can be resurrected later.
 */

void AlarmDying (display)
  Display  *display;
{
    char   filename[TEXT];

    if (xalarm.saveonshutdown) {
	(void) sprintf (filename, "%s/%s.died", HomeDirectory (), XALARMFILE);
	if (not SaveAlarm (filename))
	    (void) fprintf (stderr, "xalarm: Couldn't open %s\n", filename);
    }

    exit (0);
}



/* 
 * Save the current alarm.
 */

static Boolean SaveAlarm (filename)
  String   filename;
{
    FILE 	  *file;
    static char    days[][4] = {WEEKDAYS}, months[][4] = {MONTHS};
#if not (defined (USEFLOCK) or defined (USELOCKF))
    struct flock   lock;
#endif
    time_t 	   now;
    struct tm 	  *alarmtime;

    (void) umask (63);
    if (NONNIL (FILE *, (file = fopen (filename, "a")))) {
#if defined (USEFLOCK)
	flock (fileno (file), LOCK_EX);
#elif defined (USELOCKF)
	lockf (fileno (file), F_LOCK, (long) 0);
#else
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = lock.l_len = (long) 0;
	fcntl (fileno (file), F_SETFL, O_SYNC);
	fcntl (fileno (file), F_SETLKW, &lock);
#endif

	(void) time (&now);
	now += ((TimeToMilliSeconds (xalarm.timestr) +
		 DateToMilliSeconds (xalarm.datestr)) / MSECSIN1SEC);
	alarmtime = localtime (&now);

	days[alarmtime->tm_wday][0] = UPCASE (days[alarmtime->tm_wday][0]);
	months[alarmtime->tm_mon][0] = UPCASE (months[alarmtime->tm_mon][0]);

#if defined (USELSEEK)
	lseek (fileno (file), (off_t) 0, L_XTND);
#else
	fseek (file, (long) 0, 2);
#endif

	(void) fprintf (file, ALARMFILEFORMAT, days[alarmtime->tm_wday],
			months[alarmtime->tm_mon], alarmtime->tm_mday,
			xalarm.timestr, ReplaceNewlines (xalarm.messagestr));

	fflush (file);
#if defined (USEFLOCK)
	flock (fileno (file), LOCK_UN);
#elif defined (USELOCKF)
	lockf (fileno (file), F_ULOCK, (long) 0);
#else
	lock.l_type = F_UNLCK;
	fcntl (fileno (file), F_SETLK, &lock);
#endif
	fclose (file);
    }

    return (NONNIL (FILE *, file));
}



/* 
 * Check to see if there are any old alarms, then restart them.
 */

void RestartDiedAlarms (argv, argc)
  String  *argv;
  int 	   argc;
{
    Display  *display;
    char      diedfilename[TEXT], livefilename[TEXT], tmp[TEXT];
    int       kids;
    
    (void) sprintf (diedfilename, "%s/%s.died", HomeDirectory (), XALARMFILE);
    if (access (diedfilename, R_OK) == 0)
	if (NIL (Display *, (display = XOpenDisplay (DisplayName (argv, argc)))))
	    (void) fprintf (stderr, "xalarm: Can't open display to restart alarms.\n");
	else {
	    /* Save the current alarm file. */
	    (void) sprintf (livefilename, "%s/%s", HomeDirectory (), XALARMFILE);
	    (void) sprintf (tmp, "%s/%s.%05d", HomeDirectory (), XALARMFILE, getpid ());
	    if (access (livefilename, R_OK) == 0)
		RenameFile (livefilename, tmp);
	    RenameFile (diedfilename, livefilename);
	    
	    /* Parse this file as if it were the alarm file. */
	    xalarm.saveonshutdown = True;
	    kids = ParseAlarmFile (MAXDAYS, argv, argc);
	    while (kids-- > 0)
		(void) wait ((int *) NULL);
	    (void) unlink (livefilename);
	    
	    /* Restore the alarm file. */
	    if (access (tmp, R_OK) == 0)
		RenameFile (tmp, livefilename);
	    XCloseDisplay (display);
	}
}



/* 
 * Rename the file.
 */

static void RenameFile (from, to)
  String   from, to;
{
#if defined (USERENAME)
    (void) rename (from, to);
#else
    (void) unlink (to);
    (void) link (from, to);
    (void) unlink (from);
#endif
}
