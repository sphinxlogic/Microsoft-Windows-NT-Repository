
/* This program was adopted from David A Curry. It has gone through major
   changes in code, functionality and appearance, and may not resemble the 
   original code. Folloowing is the comment from the original program.
*/

/*
 * xpostit.c - Post-It Notes for the X Window System.
 *
 * Based on an X10R4 application.  This one is for X11R3 and uses the
 * Xt toolkit and the Athena widgets.
 *
 * All the "#ifdef notdef" stuff below is what *should* be used to start
 * this up.  This is because allegedly, XtInitialize() and XtMainLoop()
 * are obsolete.  Unfortunately, since pieces of the widgets use the
 * defaultAppContext instead of the user-supplied one, the "right" way
 * doesn't work.  Sigh.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   to work with VMS DECwindows environment. Look for ifndef VMS and
   ifdef VMS to see changes.
*/

/* Modified by:
            Jatin Desai (VIA::DESAI) , Core Applications Group
	    110 Spitbrook Road, Nashua, NH 03060
	    Digital Equipment Corporation

   To remove Athena widget set code. This version now only works with
   DECwindows toolkit widgets. It should work on both VMS and Ultrix
   although I haven't tried to even compile on Ultrix. You can no
   longer search for ifdef VMS to look for changes from original
   version.
*/

/* Modified by:
            Simon Graham SMAUG::GRAHAM), T&N IBM Interconnect Group
	    Digital Equipment Corporation
	    550, King St, Littleton, MA 01460

    To add the username and hostname to notes that are sent to other people.
*/

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "xpostit.h"

/*
 * Command line options and the resources they set.
 */
static XrmOptionDescRec options[] = {
    { "-bs",	".bufSize",		XrmoptionSepArg,	NULL },
    { "-dir",	".noteDir",		XrmoptionSepArg,	NULL },
    { "-sb",	".scrollBar",		XrmoptionNoArg,		"true" },
    { "-so",	".scrollOnOverflow",	XrmoptionNoArg,		"true" },
    { "-sv",	".saveNotes",		XrmoptionNoArg,		"true" }
};

/*
 * Resources we maintain besides those maintained by the toolkit.
 */
static XtResource resources[] = {
#define offset(field)	XtOffset(AppResPtr,field)
    { "bufSize", "BufSize", XtRInt, sizeof(int),
      offset(buf_size), XtRImmediate, (caddr_t) DefaultBufSize },
    { "noteDir", "NoteDir", XtRString, sizeof(String),
      offset(note_dir), XtRString, DefaultNoteDir },
    { "saveNotes", "SaveNotes", XtRBoolean, sizeof(Boolean),
      offset(save_notes), XtRImmediate, (caddr_t) False },
    { "scrollBar", "Scroll", XtRBoolean, sizeof(Boolean),
      offset(scroll_bar), XtRImmediate, (caddr_t) False },
    { "scrollOnOverflow", "Scroll", XtRBoolean, sizeof(Boolean),
      offset(scroll_ovf), XtRImmediate, (caddr_t) False },
    { "sendList", "SendList", XtRString, sizeof(String),
      offset(send_list), XtRString, "NODE::0" },
    { "receiveNotes", "ReceiveNotes", XtRBoolean, sizeof(Boolean),
      offset(receive_notes), XtRImmediate, (caddr_t) True },
#undef offset
};

AppRes	app_res;		/* xpostit application resources	*/
Widget	toplevel;		/* top level application shell widget	*/
Screen	*screen;		/* pointer to the screen of the display	*/
Display	*display;		/* pointer to the display we're on	*/
char    OurName[RemoteUserSize];/* our name to use when sending notes   */

XtAppContext appcontext;
Time	global_time;
Time	TimeOfEvent();

main(argc, argv)
char **argv;
int argc;
{
	Arg args[4];
	char *appname;
	char *rindex();
	char **saved_argv;
	Boolean setsigs = False;
	register int nargs, saved_argc;

	/*
	 * Ignore signals for now, but record whether they were
	 * already ignored or not so we can catch them later if
	 * need be.
	 */
	if ((signal(SIGQUIT, SIG_IGN) != SIG_IGN) &&
	    (signal(SIGINT, SIG_IGN) != SIG_IGN))
		setsigs = True;

	/*
	 * Save arguments for setting in the WM_COMMAND property
	 * of the application shell.
	 */
	saved_argv = (char **) SafeAlloc((argc + 1) * sizeof(char *));

	for (saved_argc = 0; saved_argc < argc; saved_argc++)
		saved_argv[saved_argc] = argv[saved_argc];
	saved_argv[saved_argc] = NULL;

	/*
	 * Get application name.
	 */
#ifndef VMS
	if ((appname = rindex(*argv, '/')) == NULL)
		appname = *argv;
	else
		appname++;
#else
	appname = "XPostit";
#endif
	/*
	 * Get our name for use in sending notes
	 */
#ifdef VMS
	cuserid(OurName);
	strcat(OurName,"@");
	strcat(OurName,getenv("SYS$NODE"));
	OurName[strlen(OurName)-2] = '\0';  /* remove trailing colons */

	/*
	** lowercase the string...
	*/
	{
	    register char *w;

	    for (w=OurName; *w != '\0'; w++)
		*w = _tolower(*w);
	}
#else
....something else....
#endif

	/*
	 * Initialize the toolkit.
	 */
	XtToolkitInitialize();

	/*
	 * Create an application context for our application.
	 */
	appcontext = XtCreateApplicationContext();

	/*
	 * Open and initialize the display.
	 */
	display = XtOpenDisplay(appcontext, NULL, appname, PostItNoteClass,
			options, XtNumber(options), &argc, argv);

	screen = DefaultScreenOfDisplay(display);

	nargs = 0;
	SetArg(XtNscreen, screen);
	SetArg(XtNargc, saved_argc);
	SetArg(XtNargv, saved_argv);
	
	/*
	 * Create the top level application shell.
	 */
	toplevel = XtAppCreateShell(PostItNoteClass, PostItNoteClass,
			applicationShellWidgetClass, display, args, nargs);

	/*
	 * If we need to handle signals, do it now.
	 */
	if (setsigs) {
		signal(SIGQUIT, ByeBye);
		signal(SIGINT, ByeBye);
	}

	/*
	 * Now get any resources we're interested in.
	 */
	XtGetApplicationResources(toplevel, &app_res, resources,
			XtNumber(resources), argv, argc);

	/*
	 * Construct the path to the directory notes are
	 * stored in.
	 */
	SetNoteDir();

	/*
	 * Create the plaid and menu widgets.
	 */
	CreatePlaidWidget();

	/* Set the icon and iconize pixmaps */

	SetIcons(toplevel);

#ifndef VMS
	CreateMenuWidget();

	/*
	 * Realize the top level and flush the server, which will
	 * let the user position the plaid window and map it.
	 */
	XtRealizeWidget(toplevel);
	XFlush(display);
#else
	XtRealizeWidget(toplevel);
	CreateMenuWidget();
#endif

	/* Initialize remote stuff if user asked for it. */

	if (app_res.receive_notes)
	    ReceiveInit();

	/*
	 * Load the notes the user has saved, and create widgets
	 * for them.
	 */
	LoadSavedNotes();

	/*
	 * Never returns.
	 */

	while (True)
	    {
	    XEvent	event;
	    Time	time;

	    XtAppNextEvent(appcontext, & event);

	    time = TimeOfEvent(& event);
	    if (time != CurrentTime)
		global_time = time;

	    XtDispatchEvent(& event);
	    }
}

