/*
 *	$XConsortium: scrollbar.c /main/47 1996/12/01 23:47:08 swick $
 *	$XFree86: xc/programs/xterm/scrollbar.c,v 3.24 1999/08/21 13:49:07 dawes Exp $
 */

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include <xterm.h>

#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/Scrollbar.h>

#include <data.h>
#include <error.h>
#include <menu.h>

/* Event handlers */

static void ScrollTextTo PROTO_XT_CALLBACK_ARGS;
static void ScrollTextUpDownBy PROTO_XT_CALLBACK_ARGS;


/* resize the text window for a terminal screen, modifying the
 * appropriate WM_SIZE_HINTS and taking advantage of bit gravity.
 */

static void
ResizeScreen(register XtermWidget xw, int min_width, int min_height)
{
	register TScreen *screen = &xw->screen;
#ifndef nothack
	XSizeHints sizehints;
	long supp;
#endif
	XtGeometryResult geomreqresult;
	Dimension reqWidth, reqHeight, repWidth, repHeight;
#ifndef NO_ACTIVE_ICON
	struct _vtwin *saveWin = screen->whichVwin;

	/* all units here want to be in the normal font units */
	screen->whichVwin = &screen->fullVwin;
#endif /* NO_ACTIVE_ICON */

	/*
	 * I'm going to try to explain, as I understand it, why we
	 * have to do XGetWMNormalHints and XSetWMNormalHints here,
	 * although I can't guarantee that I've got it right.
	 *
	 * In a correctly written toolkit program, the Shell widget
	 * parses the user supplied geometry argument.  However,
	 * because of the way xterm does things, the VT100 widget does
	 * the parsing of the geometry option, not the Shell widget.
	 * The result of this is that the Shell widget doesn't set the
	 * correct window manager hints, and doesn't know that the
	 * user has specified a geometry.
	 *
	 * The XtVaSetValues call below tells the Shell widget to
	 * change its hints.  However, since it's confused about the
	 * hints to begin with, it doesn't get them all right when it
	 * does the SetValues -- it undoes some of what the VT100
	 * widget did when it originally set the hints.
	 *
	 * To fix this, we do the following:
	 *
	 * 1. Get the sizehints directly from the window, going around
	 *    the (confused) shell widget.
	 * 2. Call XtVaSetValues to let the shell widget know which
	 *    hints have changed.  Note that this may not even be
	 *    necessary, since we're going to right ahead after that
	 *    and set the hints ourselves, but it's good to put it
	 *    here anyway, so that when we finally do fix the code so
	 *    that the Shell does the right thing with hints, we
	 *    already have the XtVaSetValues in place.
	 * 3. We set the sizehints directly, this fixing up whatever
	 *    damage was done by the Shell widget during the
	 *    XtVaSetValues.
	 *
	 * Gross, huh?
	 *
	 * The correct fix is to redo VTRealize, VTInitialize and
	 * VTSetValues so that font processing happens early enough to
	 * give back responsibility for the size hints to the Shell.
	 *
	 * Someday, we hope to have time to do this.  Someday, we hope
	 * to have time to completely rewrite xterm.
	 */

	TRACE(("ResizeScreen(min_width=%d, min_height=%d) xw=%#lx\n",
		min_width, min_height, (long) xw))

#ifndef nothack
	/*
	 * NOTE: If you change the way any of the hints are calculated
	 * below, make sure you change the calculation both in the
	 * sizehints assignments and in the XtVaSetValues.
	 */

	if (! XGetWMNormalHints(screen->display, XtWindow(XtParent(xw)),
				&sizehints, &supp))
	     sizehints.flags = 0;
	sizehints.base_width = min_width;
	sizehints.base_height = min_height;
	sizehints.width_inc = FontWidth(screen);
	sizehints.height_inc = FontHeight(screen);
	sizehints.min_width = sizehints.base_width + sizehints.width_inc;
	sizehints.min_height = sizehints.base_height + sizehints.height_inc;
	sizehints.flags |= (PBaseSize|PMinSize|PResizeInc);
	/* These are obsolete, but old clients may use them */
	sizehints.width = (screen->max_col + 1) * FontWidth(screen)
	     + min_width;
	sizehints.height = (screen->max_row + 1) * FontHeight(screen)
	     + min_height;
#endif

	/*
	 * Note: width and height are not set here because they are
	 * obsolete.
	 */
	XtVaSetValues(XtParent(xw),
		      XtNbaseWidth, min_width,
		      XtNbaseHeight, min_height,
		      XtNwidthInc, FontWidth(screen),
		      XtNheightInc, FontHeight(screen),
		      XtNminWidth, min_width + FontWidth(screen),
		      XtNminHeight, min_height + FontHeight(screen),
		      NULL);

	reqWidth = screen->fullVwin.f_width * (screen->max_col + 1) + min_width;
	reqHeight = screen->fullVwin.f_height * (screen->max_row + 1) + min_height;

	TRACE(("...requesting screensize chars %dx%d, pixels %dx%d\n",
		(screen->max_row + 1),
		(screen->max_col + 1),
		reqHeight, reqWidth))

	geomreqresult = XtMakeResizeRequest ((Widget)xw, reqWidth, reqHeight,
					     &repWidth, &repHeight);

	if (geomreqresult == XtGeometryAlmost) {
	    TRACE(("...almost, retry screensize %dx%d\n", repHeight, repWidth))
	    geomreqresult = XtMakeResizeRequest ((Widget)xw, repWidth,
						  repHeight, NULL, NULL);
	}
	XSync(screen->display, FALSE);	/* synchronize */
	if(XtAppPending(app_con))
		xevents();

#ifndef nothack
	XSetWMNormalHints(screen->display, XtWindow(XtParent(xw)), &sizehints);
#endif
#ifndef NO_ACTIVE_ICON
 	screen->whichVwin = saveWin;
#endif /* NO_ACTIVE_ICON */
}

void DoResizeScreen (register XtermWidget xw)
{
    int border = 2 * xw->screen.border;
    ResizeScreen (xw, border + xw->screen.fullVwin.scrollbar, border);
}


static Widget
CreateScrollBar(XtermWidget xw, int x, int y, int height)
{
	Widget scrollWidget;

	static Arg argList[] = {
	   {XtNx,		(XtArgVal) 0},
	   {XtNy,		(XtArgVal) 0},
	   {XtNheight,		(XtArgVal) 0},
	   {XtNreverseVideo,	(XtArgVal) 0},
	   {XtNorientation,	(XtArgVal) XtorientVertical},
	   {XtNborderWidth,	(XtArgVal) 1},
	};

	argList[0].value = (XtArgVal) x;
	argList[1].value = (XtArgVal) y;
	argList[2].value = (XtArgVal) height;
	argList[3].value = (XtArgVal) xw->misc.re_verse;

	scrollWidget = XtCreateWidget("scrollbar", scrollbarWidgetClass,
	  (Widget)xw, argList, XtNumber(argList));
        XtAddCallback (scrollWidget, XtNscrollProc, ScrollTextUpDownBy, 0);
        XtAddCallback (scrollWidget, XtNjumpProc, ScrollTextTo, 0);
	return (scrollWidget);
}


void
ScrollBarReverseVideo(register Widget scrollWidget)
{
	Arg args[4];
	Cardinal nargs = XtNumber(args);
	unsigned long bg, fg, bdr;
	Pixmap bdpix;

	XtSetArg(args[0], XtNbackground, &bg);
	XtSetArg(args[1], XtNforeground, &fg);
	XtSetArg(args[2], XtNborderColor, &bdr);
	XtSetArg(args[3], XtNborderPixmap, &bdpix);
	XtGetValues (scrollWidget, args, nargs);
	args[0].value = (XtArgVal) fg;
	args[1].value = (XtArgVal) bg;
	nargs--;				/* don't set border_pixmap */
	if (bdpix == XtUnspecifiedPixmap) {	/* if not pixmap then pixel */
	    args[2].value = args[1].value;	/* keep border visible */
	} else {				/* ignore since pixmap */
	    nargs--;				/* don't set border pixel */
	}
	XtSetValues (scrollWidget, args, nargs);
}


void
ScrollBarDrawThumb(register Widget scrollWidget)
{
	register TScreen *screen = &term->screen;
	register int thumbTop, thumbHeight, totalHeight;

	thumbTop    = screen->topline + screen->savedlines;
	thumbHeight = screen->max_row + 1;
	totalHeight = thumbHeight + screen->savedlines;

	XawScrollbarSetThumb(scrollWidget,
	 ((float)thumbTop) / totalHeight,
	 ((float)thumbHeight) / totalHeight);
}

void
ResizeScrollBar(TScreen *screen)
{
	XtConfigureWidget(
		screen->scrollWidget,
#ifdef SCROLLBAR_RIGHT
        	(term->misc.useRight)
			? (term->core.width -
			   screen->scrollWidget->core.width -
			   screen->scrollWidget->core.border_width)
			: -1,
#else
		-1,
#endif
		-1,
		screen->scrollWidget->core.width,
		screen->fullVwin.height + screen->border * 2,
		screen->scrollWidget->core.border_width);
	ScrollBarDrawThumb(screen->scrollWidget);
}

void
WindowScroll(register TScreen *screen, int top)
{
	register int i, lines;
	register int scrolltop, scrollheight, refreshtop;
	register int x = 0;

	if (top < -screen->savedlines)
		top = -screen->savedlines;
	else if (top > 0)
		top = 0;
	if((i = screen->topline - top) == 0) {
		ScrollBarDrawThumb(screen->scrollWidget);
		return;
	}

	if(screen->cursor_state)
		HideCursor();
	lines = i > 0 ? i : -i;
	if(lines > screen->max_row + 1)
		lines = screen->max_row + 1;
	scrollheight = screen->max_row - lines + 1;
	if(i > 0)
		refreshtop = scrolltop = 0;
	else {
		scrolltop = lines;
		refreshtop = scrollheight;
	}
	x = OriginX(screen);
	scrolling_copy_area(screen, scrolltop, scrollheight, -i);
	screen->topline = top;

	ScrollSelection(screen, i);

	XClearArea(
	    screen->display,
	    VWindow(screen),
	    (int) x,
	    (int) refreshtop * FontHeight(screen) + screen->border,
	    (unsigned) Width(screen),
	    (unsigned) lines * FontHeight(screen),
	    FALSE);
	ScrnRefresh(screen, refreshtop, 0, lines, screen->max_col + 1, False);

	ScrollBarDrawThumb(screen->scrollWidget);
}

void
ScrollBarOn (XtermWidget xw, int init, int doalloc)
{
	register TScreen *screen = &xw->screen;
	register int i, j, k;

	if(screen->fullVwin.scrollbar)
		return;

	if (init) {			/* then create it only */
	    if (screen->scrollWidget) return;

	    /* make it a dummy size and resize later */
	    if ((screen->scrollWidget = CreateScrollBar (xw, -1, - 1, 5))
		== NULL) {
		Bell(XkbBI_MinorError,0);
		return;
	    }

	    return;

	}

	if (!screen->scrollWidget) {
	    Bell (XkbBI_MinorError,0);
	    Bell (XkbBI_MinorError,0);
	    return;
	}

	if (doalloc && screen->allbuf) {
	    /* FIXME: this is not integrated well with Allocate */
	    if((screen->allbuf =
		(ScrnBuf) realloc((char *) screen->visbuf,
				  (unsigned) MAX_PTRS*(screen->max_row + 2 +
						screen->savelines) *
				  sizeof(char *)))
	    		== NULL)
	        Error (ERROR_SBRALLOC);
	    screen->visbuf = &screen->allbuf[MAX_PTRS * screen->savelines];
	    memmove( (char *)screen->visbuf, (char *)screen->allbuf,
		   MAX_PTRS * (screen->max_row + 2) * sizeof (char *));
	    for (i = k = 0; i < screen->savelines; i++) {
		k += BUF_HEAD;
		for (j = BUF_HEAD; j < MAX_PTRS; j++) {
		    if((screen->allbuf[k++] =
			(Char *)calloc((unsigned) screen->max_col + 1, sizeof(Char))) ==
			NULL)
			Error (ERROR_SBRALLOC2);
		}
	    }
	}

	ResizeScrollBar (screen);
	XtRealizeWidget (screen->scrollWidget);
	screen->fullVwin.scrollbar = screen->scrollWidget->core.width +
	     screen->scrollWidget->core.border_width;

	ScrollBarDrawThumb(screen->scrollWidget);
	DoResizeScreen (xw);

#ifdef SCROLLBAR_RIGHT
	/*
	 * Adjust the scrollbar position if we're asked to turn on scrollbars
	 * for the first time after the xterm is already running.  That makes
	 * the window grow after we've initially configured the scrollbar's
	 * position.  (There must be a better way).
	 */
	if (term->misc.useRight
	 && screen->fullVwin.fullwidth < term->core.width)
	    XtVaSetValues(screen->scrollWidget,
		XtNx, screen->fullVwin.fullwidth - screen->scrollWidget->core.border_width,
		NULL);
#endif

	XtMapWidget(screen->scrollWidget);
	update_scrollbar ();
	if (screen->visbuf) {
	    XClearWindow (screen->display, XtWindow (term));
	    Redraw ();
	}
}

void
ScrollBarOff(register TScreen *screen)
{
	if(!screen->fullVwin.scrollbar)
		return;
	XtUnmapWidget(screen->scrollWidget);
	screen->fullVwin.scrollbar = 0;
	DoResizeScreen (term);
	update_scrollbar ();
	if (screen->visbuf) {
	    XClearWindow (screen->display, XtWindow (term));
	    Redraw ();
	}
}

/*
 * Toggle the visibility of the scrollbars.
 */
void
ToggleScrollBar(XtermWidget w)
{
    register TScreen *screen = &w->screen;

    if (screen->fullVwin.scrollbar) {
	ScrollBarOff (screen);
    } else {
	ScrollBarOn (w, FALSE, FALSE);
    }
    update_scrollbar();
}

/*ARGSUSED*/
static void
ScrollTextTo(
	Widget scrollbarWidget GCC_UNUSED,
	XtPointer client_data GCC_UNUSED,
	XtPointer call_data)
{
	float *topPercent = (float *) call_data;
	register TScreen *screen = &term->screen;
	int thumbTop;	/* relative to first saved line */
	int newTopLine;

/*
   screen->savedlines : Number of offscreen text lines,
   screen->maxrow + 1 : Number of onscreen  text lines,
   screen->topline    : -Number of lines above the last screen->max_row+1 lines
*/

	thumbTop = (int) (*topPercent * (screen->savedlines + screen->max_row+1));
	newTopLine = thumbTop - screen->savedlines;
	WindowScroll(screen, newTopLine);
}

/*ARGSUSED*/
static void
ScrollTextUpDownBy(
	Widget scrollbarWidget GCC_UNUSED,
	XtPointer client_data GCC_UNUSED,
	XtPointer call_data)
{
	long pixels = (long) call_data;

	register TScreen *screen = &term->screen;
	register int rowOnScreen, newTopLine;

	rowOnScreen = pixels / FontHeight(screen);
	if (rowOnScreen == 0) {
		if (pixels < 0)
			rowOnScreen = -1;
		else if (pixels > 0)
			rowOnScreen = 1;
	}
	newTopLine = screen->topline + rowOnScreen;
	WindowScroll(screen, newTopLine);
}


/*
 * assume that b is lower case and allow plural
 */
static int
specialcmplowerwiths (char *a, char *b)
{
    register char ca, cb;

    if (!a || !b) return 0;

    while (1) {
	ca = char2lower(*a);
	cb = *b;
	if (ca != cb || ca == '\0') break;  /* if not eq else both nul */
	a++, b++;
    }
    if (cb == '\0' && (ca == '\0' || (ca == 's' && a[1] == '\0')))
      return 1;

    return 0;
}

static long
params_to_pixels (TScreen *screen, String *params, Cardinal n)
{
    register int mult = 1;
    register char *s;

    switch (n > 2 ? 2 : n) {
      case 2:
	s = params[1];
	if (specialcmplowerwiths (s, "page")) {
	    mult = (screen->max_row + 1) * FontHeight(screen);
	} else if (specialcmplowerwiths (s, "halfpage")) {
	    mult = ((screen->max_row + 1) * FontHeight(screen)) >> 1;
	} else if (specialcmplowerwiths (s, "pixel")) {
	    mult = 1;
	} else {
	    /* else assume that it is Line */
	    mult = FontHeight(screen);
	}
	mult *= atoi (params[0]);
	break;
      case 1:
	mult = atoi (params[0]) * FontHeight(screen);	/* lines */
	break;
      default:
	mult = screen->scrolllines * FontHeight(screen);
	break;
    }
    return mult;
}


/*ARGSUSED*/
void HandleScrollForward (
    Widget gw,
    XEvent *event GCC_UNUSED,
    String *params,
    Cardinal *nparams)
{
    if (IsXtermWidget(gw)) {
    	register TScreen *screen = &((XtermWidget)gw)->screen;
	long amount = params_to_pixels (screen, params, *nparams);
	ScrollTextUpDownBy (gw, (XtPointer) 0, (XtPointer)amount);
    }
    return;
}


/*ARGSUSED*/
void HandleScrollBack (
    Widget gw,
    XEvent *event GCC_UNUSED,
    String *params,
    Cardinal *nparams)
{
    if (IsXtermWidget(gw)) {
    	register TScreen *screen = &((XtermWidget)gw)->screen;
	long amount = -params_to_pixels (screen, params, *nparams);
	ScrollTextUpDownBy (gw, (XtPointer) 0, (XtPointer)amount);
    }
    return;
}
