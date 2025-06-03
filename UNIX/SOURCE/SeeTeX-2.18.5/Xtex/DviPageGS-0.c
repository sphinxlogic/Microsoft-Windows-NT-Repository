/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 * This file created by Shankar Ramamoorthy (shankar@cse.ucsc.edu) 04/92.
 * Most of this comes from Tim Thiesen's Ghostview --- thanks Tim.
 * 
 */ 

#include <stdio.h>
#include <sys/time.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Atoms.h>
#include <stdio.h>
#include <assert.h>
#include <varargs.h>
#include <math.h>
#include <ctype.h>

#include <signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#define NON_BLOCKING_IO

#ifdef NON_BLOCKING_IO
#include <fcntl.h>
/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define O_NONBLOCK O_NDELAY
#endif
#endif

#include <errno.h>
/* BSD 4.3 errno.h does not declare errno */
extern int errno;
/* Both error returns are checked for non-blocking I/O. */
/* Manufacture the other error code if only one exists. */
#if !defined(EWOULDBLOCK) && defined(EAGAIN)
#define EWOULDBLOCK EAGAIN
#endif
#if !defined(EAGAIN) && defined(EWOULDBLOCK)
#define EAGAIN EWOULDBLOCK
#endif

#include "xtex.h"
#include "dvi-simple.h"
#include "DviPageP.h"
#include "DviPageGS-0.h"
#include "DviPageGS-1.h"


/* GV_BUFSIZ is set to the minimum POSIX PIPE_MAX to ensure that
 * nonblocking writes to ghostscript will work properly.
 * GV_BUFSIZ must be 257 or greater to be able to read the largest
 * PostScript input line allowed.  (257 <= GV_BUFSIZ <= 512)
 */
#define GV_BUFSIZ 512

/* length calculates string length at compile time */
/* can only be used with character constants */
#define length(a) (sizeof(a)-1)
#define iscomment(a, b) (strncmp(a, b, length(b)) == 0)

static Boolean broken_pipe = False;

#ifdef __STDC__
static SIGVAL CathPipe (int);
static int Output (XtPointer, int *, XtInputId *);
static void InterpreterFailed (DviPageWidget);
static Bool checkIfGhostMsg (Display *, XEvent *, char *);
#else
static SIGVAL CathPipe ();
static int Output ();
static void InterpreterFailed ();
static Bool checkIfGhostMsg ();
#endif

static SIGVAL
CatchPipe(i)
    int i;
{
    broken_pipe = True;
#ifdef SIGNALRETURNSINT
    return 0;
#endif
}


/* 
 *
 * WriteBytes - Write bytes to ghostscript using non-blocking I/O.
 * Also, pipe signals are caught during writing.  The return
 * values are checked and the appropriate action is taken.  I do
 * this at this low level, because it may not be appropriate for
 * SIGPIPE to be caught for the overall application.
 *
 */
void
WriteBytes (w, ghostInputStr)
DviPageWidget w;
char *ghostInputStr;
	{
int bytes, bytesLeft;
SIGVAL (*oldsig)();
	
	/* Sanity check */
	if ((w->dviPage.ghostPID == -1) || (w->dviPage.ghostInputFD == -1))
		/* Something wrong */
		return;

	oldsig = signal(SIGPIPE, CatchPipe);
	for (bytesLeft = strlen (ghostInputStr); bytesLeft; )
		{
		bytes = write (w->dviPage.ghostInputFD, ghostInputStr, bytesLeft);
		signal(SIGPIPE, oldsig);
	
		if (broken_pipe) 
			{
			broken_pipe = False;
			/* Interpreter failed */
			InterpreterFailed (w);
			return;
			} 
		else if (bytes == -1) 
			{
			if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) 
				{
				/* Something bad happened */
				InterpreterFailed (w);
				return;
				}
			}
		else 
			{
			bytesLeft -= bytes;
			ghostInputStr += bytes;
			}
		}

	return;
	}

/*
 *
 * Output - receive I/O from ghostscript's stdout and stderr.
 * Pass this to the application via the output_callback. 
 *
 */
static int
Output(client_data, source, id)
XtPointer client_data;
int *source;
XtInputId *id;
	{
DviPageWidget w = (DviPageWidget) client_data;
char buf[GV_BUFSIZ+1];
int bytes = 0;

#ifdef DEBUG
	fprintf (stderr, "Output called.\n");
#endif
	
	if (*source == w->dviPage.ghostOutputFD) 
		{
		bytes = read (w->dviPage.ghostOutputFD, buf, GV_BUFSIZ);
		if (bytes == 0)
			{
			/* EOF occurred */
			close (w->dviPage.ghostOutputFD);
			w->dviPage.ghostOutputFD = -1;
			XtRemoveInput (w->dviPage.ghostOutputId);
			w->dviPage.ghostOutputId = None;
#ifdef DEBUG
			fprintf (stderr, "EOF on ghostscript stdout.\n");
			fflush (stderr);
#endif
			return;
			}
		else if (bytes == -1) 
			{
			/* Something bad happened */
			InterpreterFailed (w);
			return;
			}
		}
	else if (*source == w->dviPage.ghostErrorFD) 
		{
		bytes = read (w->dviPage.ghostErrorFD, buf, GV_BUFSIZ);
		if (bytes == 0) 
			{
			/* EOF occurred */
			close (w->dviPage.ghostErrorFD);
			w->dviPage.ghostErrorFD = -1;
			XtRemoveInput(w->dviPage.ghostErrorId);
			w->dviPage.ghostErrorId = None;
#ifdef DEBUG
			fprintf (stderr, "EOF on ghostscript stderr.\n");
			fflush (stderr);
#endif
			return;
			}
		else if (bytes == -1) 
			{
			/* Something bad happened */
			InterpreterFailed (w);
			return;
			}
		}

	if (bytes > 0) 
		{
		buf[bytes] = '\0';
#ifdef DEBUG
		fprintf (stderr, "Output :<%s>\n", buf);
		fflush (stderr);
#endif
		/*
		XtCallCallbackList((Widget) w, w->dviPage.ghostOutput, (XtPointer) buf);
		*/
		DialogMessage (buf, False);
		}
	}

/* This routine starts the interpreter.  It sets the DISPLAY and 
 * GHOSTVIEW environment variables.  The GHOSTVIEW environment variable
 * contains the Window that ghostscript should write on.
 *
 * This routine also opens pipes for stdout and stderr and initializes
 * application input events for them.  If input to ghostscript is not
 * from a file, a pipe for stdin is created.  This pipe is setup for
 * non-blocking I/O so that the user interface never "hangs" because of
 * a write to ghostscript.
 */
void
StartInterpreter (w)
DviPageWidget w;
	{
DviPageWidgetClass wc = (DviPageWidgetClass) XtClass(w);
int std_in[2];
int std_out[2];
int std_err[2];
char buf[GV_BUFSIZ];
static char gsEnv[GV_BUFSIZ], dispEnv[GV_BUFSIZ], layEnv[GV_BUFSIZ];
char *gsSearchPath;
#define NUM_ARGS 100
char *argv[NUM_ARGS];
int argc = 0;
int ret;
XWindowAttributes attrs;
float trueDpi;
XEvent event;

	if (w->dviPage.ghostPID != -1)
		StopInterpreter (w);

	if (w->dviPage.ghostPixmap == -1)
		/* Unable to get pixmap; dont run interpreter. */
		return;
	
	/* Prepare a <DONE> event in case exec fails */
	event.xclient.type = ClientMessage;
	event.xclient.display = XtDisplay(w);
	event.xclient.window = XtWindow (w);
	event.xclient.message_type = XmuInternAtom(XtDisplay(w),
		wc->dviPage_class.done);
	event.xclient.format = 32;
	event.xclient.data.l[0] = None;
	event.xclient.data.l[1] = w->dviPage.ghostPixmap;

	argv[argc++] = w->dviPage.interpreter;
	argv[argc++] = "-dQUIET";
	argv[argc++] = "-dNOPAUSE";
	gsSearchPath = XtMalloc ((strlen (w->dviPage.ghostSearchPath) + 3) *
		sizeof (char));
	sprintf (gsSearchPath, "-I%s", w->dviPage.ghostSearchPath);
	argv[argc++] = gsSearchPath;
	argv[argc++] = "-";
	argv[argc++] = NULL;
	
	ret = pipe(std_in);
	if (ret == -1) 
		{
		perror("Could not create pipe");
		return;
		}
	
	ret = pipe(std_out);
	if (ret == -1) 
		{
		close (std_in[0]);
		close (std_in[1]);
		perror("Could not create pipe");
		return;
		}
	ret = pipe(std_err);
	if (ret == -1) 
		{
		close (std_in[0]);
		close (std_in[1]);
		close (std_out[0]);
		close (std_out[1]);
		perror("Could not create pipe");
		return;
		}

	/* 
	 * 72.0 seems to be the one that works --- don't mess with it.
	 * See also DviPage2.c
	 */
	trueDpi = (w->dviPage.trueDpi == 0.0) ? 72.0 : w->dviPage.trueDpi;

	/* Set up environment and property for ghostscript */
	sprintf (buf, "0 0 0 0 %d %d %f %f 0 0 0 0 %d %d",
		w->dviPage.pixelsWide, w->dviPage.pixelsHigh,
		trueDpi, trueDpi,
		w->dviPage.pixelsWide, w->dviPage.pixelsHigh);
	XChangeProperty(XtDisplay (w), XtWindow (w), 
		XmuInternAtom (XtDisplay(w), wc->dviPage_class.ghostview), 
		XA_STRING, 8, PropModeReplace, (unsigned char *) buf, 
		strlen (buf));

	sprintf (buf, "[ exec ] %s\n", argv[0]);
	DialogMessage (buf, False);

	w->dviPage.ghostPID = fork();
	
	if (w->dviPage.ghostPID == 0) 
		{
		/* child */
		sprintf (gsEnv, "GHOSTVIEW=%d %d", XtWindow (w), w->dviPage.ghostPixmap);
		putenv (gsEnv);
		sprintf( dispEnv,"DISPLAY=%s",XDisplayString(XtDisplay(w)));
		putenv (dispEnv);

		close (std_out[0]);
		close (std_err[0]);
		dup2 (std_out[1], 1);
		close (std_out[1]);
		dup2 (std_err[1], 2);
		close (std_err[1]);

		close(std_in[1]);
		dup2(std_in[0], 0);
		close(std_in[0]);

		execvp (argv[0], argv);
		/* Failed */
		perror (argv[0]);
		XSendEvent(XtDisplay(w), XtWindow (w), False, NoEventMask, &event);
		XFlush (XtDisplay (w));
		exit (-1);
		}
	else
		{
		{
#ifdef NON_BLOCKING_IO
		int result;
#endif
		close(std_in[0]);
		
#ifdef NON_BLOCKING_IO
		result = fcntl(std_in[1], F_GETFL, 0);
		result = result | O_NONBLOCK;
		result = fcntl(std_in[1], F_SETFL, result);
#endif

		w->dviPage.ghostInputFD = std_in[1];
		w->dviPage.ghostInputId = None;
		}

		close (std_out[1]);
		w->dviPage.ghostOutputFD = std_out[0];
		w->dviPage.ghostOutputId = 
			XtAppAddInput(XtWidgetToApplicationContext ((Widget) w), std_out[0],
			(XtPointer) XtInputReadMask, 
			(XtInputCallbackProc) Output, (XtPointer) w);

		close(std_err[1]);
		w->dviPage.ghostErrorFD = std_err[0];
		w->dviPage.ghostErrorId = 
			XtAppAddInput(XtWidgetToApplicationContext ((Widget) w), std_err[0],
			(XtPointer) XtInputReadMask, 
			(XtInputCallbackProc) Output, (XtPointer) w);

		WriteBytes (w, w->dviPage.gsPreamble);
		WriteBytes (w, "\n");

		/* Send a spurious showpage --- this causes a clear to white
		 */
		WriteBytes (w, "@xtexGhostShowpage\n");

		/* Wait for acknowledgement from ghostscript */
		waitCopyAndAcknowledge (w, False, 0, 0, 0, 0, 0, 0);

		XtFree (dispEnv);
		XtFree (gsSearchPath);
		}
	}

void
StopInterpreter (w)
DviPageWidget w;
	{	

	if (w->dviPage.ghostPID >= 0)
		{
		kill (w->dviPage.ghostPID, SIGTERM);
		wait (0);
		w->dviPage.ghostPID = -1;
		}

	if (w->dviPage.ghostInputFD >= 0)
		{
		close (w->dviPage.ghostInputFD);
		if (w->dviPage.ghostInputId != None)
			XtRemoveInput (w->dviPage.ghostInputId);
		}

	if (w->dviPage.ghostOutputFD >= 0)
		{
		/*
		 * Make sure there's nothing.
		 * We need to do this because we are not doing neat event driven
		 * programming --- we wait for events from gs, and if we get a done
		 * event, we kill the interp and remove input notification.
		 */
		Output ((XtPointer) w, &w->dviPage.ghostOutputFD,
			&w->dviPage.ghostOutputId);
		close (w->dviPage.ghostOutputFD);
		if (w->dviPage.ghostOutputId != None)
			XtRemoveInput (w->dviPage.ghostOutputId);
		}

	if (w->dviPage.ghostErrorFD >= 0)
		{
		/*
		 * Make sure there's nothing.
		 * We need to do this because we are not doing neat event driven
		 * programming --- we wait for events from gs, and if we get a done
		 * event, we kill the interp and remove input notification.
		 */
		Output ((XtPointer) w, &w->dviPage.ghostErrorFD,
			&w->dviPage.ghostErrorId);
		close (w->dviPage.ghostErrorFD);
		if (w->dviPage.ghostErrorId != None)
			XtRemoveInput (w->dviPage.ghostErrorId);
		}
	}

static void
InterpreterFailed (w)
DviPageWidget w;
	{
	/*
	DialogMessage ("***ERROR*** Ghostscript ended unexpectedly.\n", False);
	StopInterpreter(w);
	*/
	}

static Bool
checkIfGhostMsg (display, event, arg)
Display *display;
XEvent *event;
char *arg;
	{
DviPageWidget w = (DviPageWidget) arg;
DviPageWidgetClass wc = (DviPageWidgetClass) XtClass (w);

	if (event->type == ClientMessage)
		{
		/*
		 * Right type of event.
		 * Make sure this is from ghostscript
		 *
		 */
		if ((event->xclient.message_type == 
					XmuInternAtom(display, wc->dviPage_class.page)) || 
				(event->xclient.message_type ==
					XmuInternAtom(display, wc->dviPage_class.done)))
			{
			/*
			 * Right type of message.
			 * Make sure its for this page.
			 *
			 */
			return (w->dviPage.ghostPixmap == event->xclient.data.l[1]);
			}
		else
			return (False);
		}
	else
		return (False);
	}

void
waitCopyAndAcknowledge (w, copyFlag, ox, oy, width, height, rx, ry)
DviPageWidget w;
Bool copyFlag;
int ox, oy, width, height, rx, ry;
	{
DviPageWidgetClass wc = (DviPageWidgetClass) XtClass (w);
XEvent event;

	/* Make sure interpreter got started */
	if (w->dviPage.ghostPID == -1)
		return;

	/* Wait for message from ghostscript */
	XPeekIfEvent (XtDisplay (w), &event, checkIfGhostMsg, (XtPointer) w);
	/* Get the event, deleting it from the event queue */
	XIfEvent (XtDisplay (w), &event, checkIfGhostMsg, (XtPointer) w);

	/* Process event */
	if (event.xclient.message_type ==
			XmuInternAtom(XtDisplay(w), wc->dviPage_class.page))
		{
		if (copyFlag == True)
			{
			/* Copy rectangle from pixmap to window */
			XCopyArea (XtDisplay (w), w->dviPage.ghostPixmap, XtWindow (w),
				w->dviPage.paintGC, ox, oy, width, height, rx, ry);
			/*
			XDrawRectangle (XtDisplay (w), XtWindow (w),
				w->dviPage.paintGC, rx, ry, width, height);
			*/
			}
	
		/* Send a NEXT event to ghostscript */
		w->dviPage.ghostMsgWin = event.xclient.data.l[0];
		event.xclient.type = ClientMessage;
		event.xclient.display = XtDisplay(w);
		event.xclient.window = w->dviPage.ghostMsgWin;
		event.xclient.message_type = XmuInternAtom(XtDisplay(w),
			wc->dviPage_class.next);
		event.xclient.format = 32;
		XSendEvent(XtDisplay(w), w->dviPage.ghostMsgWin, False, 0, &event);
		/*
		XFlush (XtDisplay (w));
		*/
		}
	else if (event.xclient.message_type ==
					XmuInternAtom(XtDisplay(w), wc->dviPage_class.done))
		{
		/*
		DialogMessage ("***ERROR*** Ghostscript ended unexpectedly.\n", False);
		*/
		/* Failed; just draw a rectangle. */
		if (copyFlag == True)
			XCopyArea (XtDisplay (w), w->dviPage.ghostPixmap, XtWindow (w),
				w->dviPage.paintGC, ox, oy, width, height, rx, ry);
			/*
			XDrawRectangle (XtDisplay (w), XtWindow (w), w->dviPage.paintGC, 
				rx, ry, width, height);
			*/
		StopInterpreter(w);
		DialogMessage ("[ gs ] done\n", False);
		}
	else
		{
		DialogMessage ("***ERROR*** Unknown message type from Ghostscript\n",
			False);
		StopInterpreter(w);
		}
	}
