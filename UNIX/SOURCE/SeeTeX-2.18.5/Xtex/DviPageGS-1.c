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
 */ 

#include <stdio.h>
#include <sys/time.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <assert.h>
#include <varargs.h>
#include <math.h>
#include <ctype.h>

#include "xtex.h"
#include "dvi-simple.h"
#include "DviPageP.h"
#include "page.h"

#include "DviPageGS-1.h"

#ifdef __STDC__
static void callbackGhostOutput (Widget, XtPointer, XtPointer);
#else
static void callbackGhostOutput ();
#endif

/*
 *
 * Routines for (re-)setting interface fields on various events.
 *
 */

/*
 * Initialize interface fields to indicate no interpreter.
 */
void
gsintInitialize (w)
DviPageWidget w;
	{
	w->dviPage.ghostPID = -1;
	w->dviPage.ghostMsgWin = None;
	w->dviPage.ghostInputId = w->dviPage.ghostOutputId =
		w->dviPage.ghostErrorId = None;
	w->dviPage.ghostInputFD = w->dviPage.ghostOutputFD =
		w->dviPage.ghostErrorFD = -1;
	w->dviPage.ghostPixmap = None;
	w->dviPage.ghostPixWidth = w->dviPage.ghostPixHeight = 
		w->dviPage.ghostPixDepth = 0;
	}

/*
 * Install callback and event handlers.
 */
void
gsintRealize (w)
DviPageWidget w;
	{
	XtAddCallback ((Widget) w, XtNghostOutput, callbackGhostOutput, 
		(XtPointer) 0);
	gsintResize (w);
	}

void
gsintResize (w)
DviPageWidget w;
	{
	/* Allocate pixmap for gs */
	if (w->dviPage.ghostPixmap != None)
		XFreePixmap (XtDisplay (w), w->dviPage.ghostPixmap);
	
	w->dviPage.ghostPixmap = XCreatePixmap (XtDisplay (w), XtWindow (w),
		w->dviPage.pixelsWide, w->dviPage.pixelsHigh, w->dviPage.ghostPixDepth);
	
	if (w->dviPage.ghostPixmap == None)
		DialogMessage ("Couldn't get pixmap; won't run interpreter.\n", False);
	else
		{
		w->dviPage.ghostPixWidth = w->dviPage.pixelsWide;
		w->dviPage.ghostPixHeight = w->dviPage.pixelsHigh;
		}
	
	if (w->dviPage.ghostPID != -1)
		StopInterpreter (w);
	}

/*
 * Destroy interface fields.
 */
void
gsintDestroy (w)
DviPageWidget w;
	{
	if (w->dviPage.ghostPID >= 0)
		StopInterpreter (w);

	/* Delete all the callback lists */
	XtRemoveAllCallbacks ((Widget) w, XtNghostOutput);

	/* Just to be safe */
	gsintInitialize (w);
	}

/*
 *
 * Callback and event handlers.
 *
 */
static void
callbackGhostOutput (gw, client_data, call_data)
Widget gw;
XtPointer client_data, call_data;
	{
	DialogMessage ((char *) call_data, False);
	}	
