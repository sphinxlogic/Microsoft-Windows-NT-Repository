#ifndef lint
static char sccsid[] = "@(#)logout.c	3.8 96/03/15 xlockmore";

#endif

/*-
 * logout.c: handle compile-time optional logout
 *
 * See xlock.c for copying information.
 *
 * xclosedown code
 * Copyright 1990 by Janet Carson
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Revision History:
 * 27-Jul-95: put back in logout.c (oops).
 *            Window shutdown program by Janet L. Carson,
 *            Baylor College of Medicine.
 *            Main procedure modified for use w/ xlock by
 *            Anthony Thyssen <anthony@cit.gu.edu.au>.
 * 24-Feb-95: fullLock rewritten to handle non-default group names from
 *            Dale A. Harris <rodmur@ecst.csuchico.edu>
 * 13-Feb-95: Heath A. Kehoe <hakehoe@icaen.uiowa.edu>.
 *            Mostly taken from bomb.c
 * 1994:      bomb.c written.  Copyright (c) 1994 Dave Shield
 *            Liverpool Computer Science
 */

#if defined( AUTO_LOGOUT ) || defined( LOGOUT_BUTTON )

#include "xlock.h"
#ifdef SYSLOG
#include <syslog.h>
#endif
#include <sys/signal.h>

extern Bool inroot, inwindow, nolock;
extern char *ProgramName;

/* 
 * This file contains a function called logoutUser() that, when called,
 * will (try) to log out the user.
 *
 * A portable way to do this is to simply kill all of the user's processes,
 * but this is a really ugly way to do it (it kills background jobs that
 * users may want to remain running after they log out).
 *
 * If your system provides for a cleaner/easier way to log out a user,
 * you may implement it here.
 *
 * For example, on some systems, one may define for the users an environment
 * variable named XSESSION that contains the pid of the X session leader
 * process.  So, to log the user out, we just need to kill that process,
 * and the X session will end.  Of course, a user can defeat that by
 * changing the value of XSESSION; so we can fall back on the ugly_logout()
 * method.
 *
 * If you can't log the user out (and you don't want to use the brute
 * force method) simply return from logoutUser(), and xlock will continue
 * on it's merry way (only applies if AUTO_LOGOUT or LOGOUT_BUTTON is
 * defined.)
 */

#define NAP_TIME        5	/* Sleep between shutdown attempts */

void
ugly_logout(void)
{

#ifndef KILL_ALL_OTHERS
#define KILL_ALL_OTHERS -1
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGTERM, SIG_IGN);
#endif

	(void) kill(KILL_ALL_OTHERS, SIGHUP);
	(void) sleep(NAP_TIME);
	(void) kill(KILL_ALL_OTHERS, SIGTERM);
	(void) sleep(NAP_TIME);

#ifdef SYSLOG
	syslog(LOG_NOTICE, "xlock: failed to exit - sending kill (uid %d)\n",
	       getuid());
#endif

	(void) kill(KILL_ALL_OTHERS, SIGKILL);
	(void) sleep(NAP_TIME);

#ifdef SYSLOG
	syslog(LOG_WARNING, "xlock: still won't exit - suicide (uid %d)\n",
	       getuid());
#endif

	(void) kill(getpid(), SIGKILL);
	exit(-1);

}

#ifdef CLOSEDOWN_LOGOUT

/* Logout the user by contacting the display and closeing all windows */

static void closedown();	/* the closedown function from xclosedown.c */

void
logoutUser(void)
{
#ifdef SYSLOG
	syslog(LOG_INFO, "xlock: expired. closing down %s\n", getenv("DISPLAY"));
#endif

	closedown(dsp);
/* ugly_logout(); */
	exit(-1);
}

#else /* !CLOSEDOWN_LOGOUT */

#ifdef SESSION_LOGOUT

/* sample logoutUser (described above) */

void
logoutUser(void)
{
	char       *pidstr;
	int         pid;

#ifdef SYSLOG
	syslog(LOG_INFO, "xlock: expired. closing down (uid %d) on %s\n",
	       getuid(), getenv("DISPLAY"));
#endif

	pidstr = getenv("XSESSION");
	if (pidstr) {
		pid = atoi(pidstr);
		kill(pid, SIGTERM);
		(void) sleep(5);
	}
/* ugly_logout(); */
	(void) sleep(1);
}

#else

void
logoutUser(void)
{

#ifdef SYSLOG
	syslog(LOG_INFO, "xlock: expired. closing down (uid %d) on %s\n",
	       getuid(), getenv("DISPLAY"));
#endif

	ugly_logout();
}

#endif /* SESSION_LOGOUT */
#endif /* CLOSEDOWN_LOGOUT */


	/*
	 *  Determine whether to "fully" lock the terminal, or
	 *    whether the time-limited version should be imposed.
	 *
	 *  Policy:
	 *      Members of staff can fully lock
	 *        (hard-wired user/group names + file read at run time)
	 *      Students (i.e. everyone else)
	 *          are forced to use the time-limit version.
	 *
	 *  An alternative policy could be based on display location
	 */
#define FULL_LOCK       1
#define TEMP_LOCK       0

/* assuming only staff file is needed */
#ifndef STAFF_FILE
#define STAFF_FILE      "/usr/remote/etc/xlock.staff"
#endif

#undef passwd
#undef pw_name
#undef getpwnam
#undef getpwuid
#include <pwd.h>
#include <grp.h>
#include <sys/param.h>
#include <errno.h>
#ifndef NGROUPS
#include <limits.h>		/* or <sys/param.h> */
#define NGROUPS NGROUPS_MAX
#endif

int
fullLock(void)
{
	uid_t       uid;
	int         ngroups = NGROUPS;
	gid_t       mygidset[NGROUPS];
	int         ngrps, i;
	struct passwd *pwp;
	struct group *gp;
	FILE       *fp;
	char        buf[BUFSIZ];

	if ((uid = getuid()) == 0)
		return (FULL_LOCK);	/* root */
	if (inwindow || inroot || nolock)
		return (FULL_LOCK);	/* (mostly) harmless user */

	pwp = getpwuid(uid);
	if ((ngrps = getgroups(ngroups, mygidset)) == -1)
		perror(ProgramName);

	if ((fp = fopen(STAFF_FILE, "r")) == NULL)
		return (TEMP_LOCK);

	while ((fgets(buf, BUFSIZ, fp)) != NULL) {
		char       *cp;

		if ((cp = (char *) strchr(buf, '\n')) != NULL)
			*cp = '\0';
		if (!strcmp(buf, pwp->pw_name))
			return (FULL_LOCK);
		if ((gp = getgrnam(buf)) != NULL)	/* check all of user's groups */
			for (i = 0; i < ngrps; ++i)
				if (gp->gr_gid == mygidset[i])
					return (FULL_LOCK);
	}

	return (TEMP_LOCK);
}


/* 
 *  Window shutdown program by Janet L. Carson, Baylor College of Medicine.
 *  Version 1.0, placed in /contrib on 2/12/90.
 *
 *  Please send comments or fixes to jcarson@bcm.tmc.edu
 */

static Display *d;
static Atom wm_delete_window;
static Atom wm_protocols;
static Atom wm_state;
static int  err_occurred = False;

/* 
 *  I'm probably going to get some BadWindow errors as I kill clients
 *  which have multiple windows open and then try to kill them again
 *  on another of their windows.  I'm just going to plow right through!
 *  The flag is set back to false in recurse_tree and kill_tree...
 */

static int
err_handler(Display * dpy, XErrorEvent * err)
{
	err_occurred = True;
	return 0;
}

/* 
 *  Looking for properties...
 */

static int
has_property(Window w, Atom prop)
{
	int         nprops, j, retval = 0;
	Atom       *list = XListProperties(d, w, &nprops);

	if (err_occurred)
		return 0;

	for (j = 0; j < nprops; j++) {
		if (list[j] == prop) {
			retval = 1;
			break;
		}
	}

	if (nprops)
		XFree((caddr_t) list);

	return retval;
}

static int
has_wm_state(Window w)
{
	return has_property(w, wm_state);
}

static int
has_wm_protocols(Window w)
{
	return has_property(w, wm_protocols);
}

/* 
 *  Send a WM_PROTOCOLS WM_DELETE_WINDOW message to a window
 */

static void
send_delete_message(Window w)
{
	XClientMessageEvent xclient;

	xclient.type = ClientMessage;
	xclient.send_event = True;
	xclient.display = d;
	xclient.window = w;
	xclient.message_type = wm_protocols;
	xclient.format = 32;
	xclient.data.l[0] = wm_delete_window;

	XSendEvent(d, w, False, 0, (XEvent *) & xclient);
}

/* 
 *  To shutdown a top level window:  if the window participates
 *  in WM_DELETE_WINDOW, let the client shut itself off.  Otherwise,
 *  do an XKillClient on it.
 */

static void
handle_top_level(Window w)
{
	Atom       *prots;
	int         nprots, j;

	if (has_wm_protocols(w)) {
		XGetWMProtocols(d, w, &prots, &nprots);

		if (err_occurred)
			return;

		for (j = 0; j < nprots; j++)
			if (prots[j] == wm_delete_window) {
				send_delete_message(w);
				break;
			}
		if (j == nprots)	/* delete window not found */
			XKillClient(d, w);

		XFree((caddr_t) prots);
	} else
		XKillClient(d, w);
}

/* 
 *  recurse_tree: look for top level windows to kill all the way down
 *  the window tree.  This pass is "nice"--I'll use delete_window protocol
 *  if the window supports it.  If I get an error in the middle, I'll start
 *  over again at the same level, because reparenting window managers throw
 *  windows back up to the root...
 */

static void
recurse_tree(Window w)
{
	Window      root, parent, *kids;
	unsigned int nkids;
	int         j;
	int         wm_state;

	for (;;) {
		XQueryTree(d, w, &root, &parent, &kids, &nkids);
		if (err_occurred) {
			err_occurred = False;
			return;
		}
		for (j = 0; j < nkids; j++) {
			wm_state = has_wm_state(kids[j]);

			if (err_occurred)
				break;

			if (wm_state) {
				handle_top_level(kids[j]);
				if (err_occurred)
					break;
			} else
				recurse_tree(kids[j]);
		}

		XFree((caddr_t) kids);

		/* when I get all the way through a level without an error, I'm done  */

		if (err_occurred)
			err_occurred = False;
		else
			return;

	}
}

/* 
 *  This is the second pass--anything left gets an XKillClient!
 */

static void
kill_tree(Window w)
{
	Window      root, parent, *kids;
	unsigned int nkids;
	int         j;

	for (;;) {
		XQueryTree(d, w, &root, &parent, &kids, &nkids);
		if (err_occurred) {
			err_occurred = False;
			return;
		}
		for (j = 0; j < nkids; j++) {
			XKillClient(d, kids[j]);
			if (err_occurred)
				break;
		}

		XFree((caddr_t) kids);

		/* when I get all the way through a level without an error, I'm done  */

		if (err_occurred)
			err_occurred = False;
		else
			return;
	}
}

/* 
 *  Main program
 */

static void
closedown(Display * disp)
{
	int         j;

	/* init global variables */

	d = disp;		/* hack to allow a display from another module */
	wm_state = XInternAtom(d, "WM_STATE", False);
	wm_protocols = XInternAtom(d, "WM_PROTOCOLS", False);
	wm_delete_window = XInternAtom(d, "WM_DELETE_WINDOW", False);

	/* synchronize -- so I'm aware of errors immediately */

	XSynchronize(d, True);

	/* use my error handler from here on out */

	(void) XSetErrorHandler(err_handler);

	/* start looking for windows to kill -- be nice on pass 1 */

	for (j = 0; j < ScreenCount(d); j++)
		recurse_tree(RootWindow(d, j));

	/* wait for things to clean themselves up */

	(void) sleep(5);

	/* this will forcibly kill anything that's still around --
	   this second pass may or may not be needed... */

	for (j = 0; j < ScreenCount(d); j++)
		kill_tree(RootWindow(d, j));
}

#endif
