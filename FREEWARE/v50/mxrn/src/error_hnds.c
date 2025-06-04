
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/error_hnds.c,v 1.6 1993/01/11 02:14:53 ricks Exp $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * error_handlers.c: routines for error/signal handling
 *
 */

#include "copyright.h"
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <signal.h>
#ifndef VMS
#include <X11/Intrinsic.h>
#else
#include <decw$include/Intrinsic.h>
#endif
#include <errno.h>
#include "dialogs.h"
#include "xmisc.h"
#include "xthelper.h"
#include "news.h"
#include "internals.h"
#include "xrn.h"
#include "mesg.h"
#include "error_hnds.h"
#include "resources.h"
#include "newsrcfile.h"
#ifdef __STDC__
#include <stdlib.h>
#include <time.h>
#ifndef VMS
#include <unistd.h>
#else
#include <unixlib.h>
extern int sys$putmsg();
#endif
#endif

extern void close_server();
static int retryNotifier();

/*
 * error handlers - major purpose is to close down 'rn' cleanly
 */

Boolean allowErrors = False;

/*ARGSUSED*/
static int
xrnXIOError(display)
Display *display;
{
    XRNState &= ~XRN_X_UP;
    sprintf(error_buffer, "XIO Error: %s", errmsg(errno));
    ehErrorExitXRN(error_buffer);
    return 0;
}


/*ARGSUSED*/
static int
xrnXError(display, event)
Display *display;
XErrorEvent *event;
{
    char buffer[1024];

    if (allowErrors) return 0;
#ifdef ERRORBUG
	if (event->request_code == 4) 		/* destroy window errors */
	return 0;				/* Bug workaround */
#endif

    if (event->request_code == 42) return 0;	/* Ignore 'set input focus' errs */
    if (event->request_code == 91) {		/* Colormap errors */
#ifdef MOTIF
	(void) fprintf(stderr, "mxrn: colormap query failure\n");
#else
	(void) fprintf(stderr, "dxrn: colormap query failure\n");
#endif
	return 0;
    }
    XGetErrorText(display, event->error_code, buffer, sizeof(buffer));
#ifdef MOTIF
    (void) fprintf(stderr, "mxrn: X Error: %s\n", buffer);
#else
    (void) fprintf(stderr, "dxrn: X Error: %s\n", buffer);
#endif
    (void) fprintf(stderr, "    serial number: %d\n", event->serial);
    (void) fprintf(stderr, "    error code:  %d\n", (int) event->error_code);
    (void) fprintf(stderr, "    request code:  %d\n", (int) event->request_code);
    (void) fprintf(stderr, "    minor code:  %d\n", (int) event->minor_code);
    (void) fprintf(stderr, "    resource id: %d\n", (int) event->resourceid);
    if (app_resources.dumpCore) {
	XRNState &= ~XRN_X_UP;
	ehErrorExitXRN("X Error");
    } else {
	retryNotifier("X Error");
    }
    return 0;
}

static int
xrnXtWarning()
{
/* Ignore warnings - specifically focus grabs */
    return 0;
}

/*ARGSUSED*/
static int
xrnXtError(errorMessage)
String errorMessage;
{
#define XTERRORINTRO "X Toolkit Error: "
    char buffer[80];
    if (allowErrors) return 0;

    strcpy(buffer, XTERRORINTRO);
    strncat(buffer, errorMessage, sizeof(buffer) - sizeof(XTERRORINTRO));
#ifdef MOTIF
    (void) fprintf(stderr, "mxrn: %s\n", buffer);
#else
    (void) fprintf(stderr, "dxrn: %s\n", buffer);
#endif
    if (app_resources.dumpCore) {
	XRNState &= ~XRN_X_UP;
	ehErrorExitXRN(buffer);
    } else {
	retryNotifier(buffer);
    }
#undef XTERRORINTRO
    return 0;
}

void
ehInstallErrorHandlers()
{
#ifndef NOERRHAND
    XtSetWarningMsgHandler((XtErrorMsgHandler)xrnXtWarning);
    XtSetErrorHandler((XtErrorHandler)xrnXtError);
    XSetErrorHandler(xrnXError);
    XSetIOErrorHandler(xrnXIOError);
#endif
    return;
}


static void
sig_catcher(signo)
int signo;
{
    char buffer[80];
    
    /* allow HUP's and INT's to do a clean exit */
    if ((signo == SIGHUP) || (signo == SIGINT)) {
	ehCleanExitXRN();
    }

    (void) sprintf(buffer, "Caught signal (%d), cleaned up .newsrc and removed temp files", signo);

    (void) signal(signo, SIG_DFL);
    ehSignalExitXRN(buffer);
    (void) kill(getpid(), signo);
}


void
ehInstallSignalHandlers()
{

    if (! app_resources.dumpCore) {
	int i;
#if defined(sun) || defined(ultrix) || defined(_ANSI_C_SOURCE) || defined(__osf__) || defined(sgi) || defined(__vms)
	  void (*oldcatcher)();
#else
	  int (*oldcatcher)();
#endif

	for (i = 1; i <= SIGTERM; i++) {
	    switch(i) {
#ifdef SIGSTOP
	    case SIGSTOP:
#endif
#ifdef SIGTSTP
	    case SIGTSTP:
#endif
#ifdef SIGCONT
	    case SIGCONT:
#endif
		break;

	    case SIGPIPE:
		(void) signal(i, SIG_IGN);
		break;

	    default:
#ifdef __STDC__
		oldcatcher = signal(i, sig_catcher);
#else
		oldcatcher = signal(i, (SIG_PF0) sig_catcher);
#endif
		if (oldcatcher == SIG_IGN) {
		    (void) signal(i, SIG_IGN);
		}
		break;
	    }
	}
    }
    return;
}


static int retry;
static int die = 0;

static void
myAbort()
{
    die = 1;
    return;
}    


static void
myExit()
{
#ifdef VMS
    exit(1);
#else
    exit(-1);
#endif
}


static void
Retry()
{
    retry = 1;
    return;
}

static void
deathNotifier(message)
char *message;
{
    XEvent ev;

    static struct DialogArg args[] = {
	{"Exit", myAbort, (caddr_t) -1},
    };

    die = 0;
    
    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
#ifdef MOTIF
	(void) fprintf(stderr, "mxrn: %s\n", message);
#else
	(void) fprintf(stderr, "dxrn: %s\n", message);
#endif
	return;
    }

    /* XXX unmap icon */
    XtMapWidget(TopLevel);
    PopUpDialog(CreateDialog(TopLevel, "Fatal Error",
		message, DIALOG_NOTEXT, args, XtNumber(args)));

    while (!die) {
	XtNextEvent(&ev);
	XtDispatchEvent(&ev);
    }

    return;
}
static int
retryNotifier(message)
char *message;
{
    XEvent ev;
    Widget dialog;
    
    static struct DialogArg args[] = {
	{"Exit", myAbort, (caddr_t) -1},
	{"Retry", Retry, (caddr_t) -1},
    };

    die = retry = 0;
    
    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
#ifdef MOTIF
	(void) fprintf(stderr, "mxrn: %s\n", message);
#else
	(void) fprintf(stderr, "dxrn: %s\n", message);
#endif
	return 0;
    }

    /* XXX unmap icon */
    XtMapWidget(TopLevel);
    dialog = CreateDialog(TopLevel, "Warning Retry",
			  message, DIALOG_NOTEXT, args,
			  XtNumber(args));
    PopUpDialog(dialog);

    while (!retry && !die) {
	XtNextEvent(&ev);
	XtDispatchEvent(&ev);
    }

    PopDownDialog(dialog);
    
    return retry;
}

static Widget warnWidget = (Widget) NULL;
static void unmapWarn()
{
   PopDownDialog(warnWidget);
   XtDestroyWidget(warnWidget);
   warnWidget = (Widget) NULL;
}
void
warning(message)
char *message;
{
    static struct DialogArg args[] = {
	{"OK", unmapWarn, (caddr_t) -1},
    };

    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
	(void) fprintf(stderr, "%s\n", message);
	return;
    }

    /* XXX unmap icon */
    XtMapWidget(TopLevel);
    if (warnWidget != (Widget) NULL)
	return;
    PopUpDialog(warnWidget =
        CreateDialog(TopLevel, "Warning",
		message, DIALOG_NOTEXT, args, XtNumber(args)));

    return;
}

extern Boolean printPending;
void
flushPrintQueue()
{
    char buffer[1024], pname[512];
#ifdef VMS
    if (printPending) {
	printPending = False;
	(void) sprintf(pname, "%sXRN_PRINT_%u.LIS", app_resources.tmpDir,
		getpid());
	(void) sprintf(buffer, "%s %s",
		    app_resources.printCommand, pname);
	(void) system(buffer);
    }
#else
    if (printPending) {
	printPending = False;
	(void) sprintf(pname, "%sXrn_print_%u", app_resources.tmpDir,
		getpid());
	(void) sprintf(buffer, "%s %s",
		    app_resources.printCommand, pname);
	(void) system(buffer);
    }
#endif	
    return;
}

#define XRN_NORMAL_EXIT_BUT_NO_UPDATE 2
#define XRN_NORMAL_EXIT 1
#define XRN_ERROR_EXIT 0
#define XRN_SIGNAL_EXIT -1
      
static void
exitXRN(status)
int status;
{
    static int beenHere = 0;

    /*
     * immediate exit, exitXRN was called as a result of something in
     * itself
     */ 
    if (beenHere == 1) {
	exit(-1);
    }
    
    beenHere = 1;
    flushPrintQueue(); 

    if ((XRNState & XRN_NEWS_UP) == XRN_NEWS_UP) {
	/* XXX is this really needed?  does free files... */
	releaseNewsgroupResources();

	if (status != XRN_NORMAL_EXIT_BUT_NO_UPDATE) {
	    if (!updatenewsrc()) {
#ifdef MOTIF
		fprintf(stderr, "mxrn: .newsrc file update failed\n");
#else
		fprintf(stderr, "dxrn: .newsrc file update failed\n");
#endif
	    }
	}
    }

    close_server();

    /* clean up the lock */
    removeLock();

    return;
}


extern void CustomizeSave();
extern Boolean getUpdateStatus();
static int ExitType;

static Widget saveQuestion = (Widget) 0;

static void saveAndExit()
{
    CustomizeSave();
    exitXRN(ExitType);
    exit(0);
}

static void justExit()
{
    exitXRN(ExitType);
    exit(0);
}

static void noExit()
{
    PopDownDialog(saveQuestion);
}

static struct DialogArg customArgs[] = {
    {"Yes", saveAndExit, (caddr_t) -1},
    {"No",  justExit,    (caddr_t) -1},
    {"Continue", noExit, (caddr_t) -1},
};

static void
checkCustomSave(exitType)
int exitType;
{
    ExitType = exitType;

    if (saveQuestion == (Widget) 0) {
	saveQuestion = CreateDialog(TopLevel, "Save settings",
	    "Customize settings have not been saved\nSave them before exiting?",
	    DIALOG_NOTEXT, customArgs, XtNumber(customArgs));
    }
    PopUpDialog(saveQuestion);
    return;
}

void
ehNoUpdateExitXRN()
{
    if (getUpdateStatus()) {
	checkCustomSave(XRN_NORMAL_EXIT_BUT_NO_UPDATE);
    } else {
	exitXRN(XRN_NORMAL_EXIT_BUT_NO_UPDATE);
	exit(0);
    }
}

void
ehCleanExitXRN()
{
    if (getUpdateStatus()) {
	checkCustomSave(XRN_NORMAL_EXIT);
    } else {
	exitXRN(XRN_NORMAL_EXIT);
	exit(0);
    }
}


void
ehErrorExitXRN(message)
char *message;
{
    exitXRN(XRN_ERROR_EXIT);
    deathNotifier(message);
    exit(-1);
}

int
ehErrorRetryXRN(message, save)
char *message;
Boolean save;
{
    int retryFlag;
     
    retryFlag = retryNotifier(message);

    if (!retryFlag && save) {
	exitXRN(XRN_ERROR_EXIT);
	exit(-1);
    }
    
    return retryFlag;
}
    
void
ehSignalExitXRN(message)
char *message;
{
    exitXRN(XRN_SIGNAL_EXIT);
    deathNotifier(message);
    return;
}    
#ifdef VMS
#include <descrip.h>
static char	ErrMessage[255];

static int grabErrorText(descr)
struct dsc$descriptor_s *descr;
{
    char *p;
    short s;
    s = (short)descr->dsc$w_length;
    p = (char *)descr->dsc$a_pointer;
    strncpy(ErrMessage, p, s);    
    ErrMessage[s] = '\0';
    mesgPane(XRN_SERIOUS | XRN_APPEND, "%s", ErrMessage);
    return 0;
}
void
ehVMSerror(text, sts, stv)
char *text;
int sts;
int stv;
{
    int args[4];
    mesgDisableRedisplay();
    mesgPane(XRN_SERIOUS, text);
    args[0] = 2;
    args[1] = sts;
    args[2] = stv;
    args[3] = 0;
    sys$putmsg(args, grabErrorText, 0, 0);
    mesgEnableRedisplay();
}
#endif
