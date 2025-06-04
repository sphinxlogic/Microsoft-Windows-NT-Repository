
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xmisc.c,v 1.6 1993/02/04 18:22:38 ricks Exp $";
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
 * xmisc.c: routines for handling miscellaneous x functions
 *
 */

#include "copyright.h"
#ifndef VMS
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#else
#include <decw$include/X.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#endif
#include "config.h"
#include <stdio.h>
#include "utils.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#ifdef USE_DEC_CURSOR
#include <X11/decwcursor.h>
#else
#include <X11/cursorfont.h>
#endif /* USE_DEC_CURSOR */
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/StringDefs.h>
#include <decw$include/Shell.h>
#ifdef USE_DEC_CURSOR
#include <decw$cursor.h>
#else
#include <decw$include/cursorfont.h>
#endif /* USE_DEC_CURSOR */
#endif
#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#else
#include <Xm/Protocols.h>
#endif
#ifdef VMS
#include <Xm/XmP.h>
#include <Xm/TextP.h>
#endif
#else
#ifndef VMS
#include <X11/DwtAppl.h>
#else
#include <decw$include/DwtAppl.h>
#endif
#endif
#include "xthelper.h"
#include "resources.h"
#include "news.h"
#include "internals.h"
#include "xrn.h"
#include "xmisc.h"


/* XRN icon */

#include "xrn.xbm"
#include "calvin.icon"
#include "bigicon.xbm"
#include "smallicon.xbm"

#define MOTIF_WM 1
#define DEC_WM 2

#ifndef XtNiconifyPixmap
#define XtNiconifyPixmap "iconifyPixmap"
#endif

static Pixmap icon_pixmap = (Pixmap) 0;
static Pixmap big_pixmap = (Pixmap) 0;
static Pixmap small_pixmap = (Pixmap) 0;
static Pixmap icon_i_pixmap = (Pixmap) 0;
static Pixmap big_i_pixmap = (Pixmap) 0;
static Pixmap small_i_pixmap = (Pixmap) 0;
static int WhichWM = -1;


static Pixmap
getpm()
{
    unsigned int width, height;
    unsigned char *bits;

    if (app_resources.calvin) {
	width = calvin_width;
	height = calvin_height;
	bits = calvin_bits;
    } else {
	width = xrn_width;
	height = xrn_height;
	bits = xrn_bits;
    }

    return XCreateBitmapFromData(XtDisplay(TopLevel), XtScreen(TopLevel)->root,
				 (char *) bits, width, height);
}

void
xmSetIconAndName(it)
IconType it;
{
    static char	*PrevName = NULL, *OldName = NULL;
    static Pixmap     PrevPm = None, OldPm = None;
    char	*name;
    Pixmap	pm;
    Arg		arg;
#ifdef MOTIF
    static	char *lostName = "mxrn: Connection Lost";
    static	char *newName = "mxrn (New Groups)";
#else
    static	char *lostName = "dxrn: Connection Lost";
    static	char *newName = "dxrn (New Groups)";
#endif

    if (OldPm == None) {
	XtSetArg(arg, XtNiconPixmap, &OldPm);
  	XtGetValues(TopLevel, &arg, 1);
 	PrevPm = OldPm;
    }

    if (app_resources.iconPixmap == None) {
	app_resources.iconPixmap = getpm();
    }

    if (app_resources.unreadIconPixmap == None) {
	app_resources.unreadIconPixmap = app_resources.iconPixmap;
    }

    switch (it) {
	case InitIcon :
	    PrevPm = pm = app_resources.iconPixmap;
	    PrevName = name = OldName = app_resources.iconName;
	    break;
	case UnreadIcon :
	    PrevPm = pm = app_resources.unreadIconPixmap;
	    PrevName = name = app_resources.unreadIconName;
	    break;
	case ReadIcon :
	    PrevPm = pm = app_resources.iconPixmap;
	    PrevName = name = app_resources.iconName;
	    break;
	case BusyIcon :
	    if ((pm = app_resources.busyIconPixmap) == None)
		pm = OldPm;
	    if ((name = app_resources.busyIconName) == NULL)
 		name = OldName;
 	    break;
	case LostIcon:
	    pm = PrevPm;
	    name = lostName;
	    break;
	case NewGroupsIcon:
	    pm = PrevPm;
	    name = newName;
	    break;
 	case PrevIcon :
 	    pm = PrevPm;
 	    name = PrevName;
 	    break;
    }

    if (OldPm != pm) {
 	XtSetArg(arg, XtNiconPixmap, pm);
  	XtSetValues(TopLevel, &arg, 1);
 	OldPm = pm;
    }

    if (name == NULL) {
 	name = app_resources.iconName;
    }

    if (OldName != name) {
 	XSetIconName(XtDisplay(TopLevel),XtWindow(TopLevel),name);
 	XFlush(XtDisplay(TopLevel));
	OldName = name;
    }
}
 
void
xmIconCreate(whichWidget)
Widget whichWidget;
{
    unsigned char *bits, *i_bits;
    int width, height;
    Arg arg[2];
    XIconSize *icon_size_list;
    int numsizes;
    Display *dpy;
    
    if (WhichWM == -1) {
	dpy = XtDisplay(whichWidget);
	if (XGetIconSizes(dpy, RootWindow(dpy, DefaultScreen(dpy)),
			&icon_size_list, &numsizes)) {
	    if (numsizes > 0) {
		if (icon_size_list[0].width_inc > 1) {
		    WhichWM = DEC_WM;
		} else {
		    WhichWM = MOTIF_WM;
		}
	    } else {
		WhichWM = MOTIF_WM;
	    }
	    XFree(icon_size_list);
	} else {
	    WhichWM = MOTIF_WM;
        }
    }	
	
    if (app_resources.calvin) {
	width = calvin_width;
	height = calvin_height;
	bits = calvin_bits;
	i_bits = calvin_bits;
    } else {
	width = xrn_width;
	height = xrn_height;
	bits = xrn_bits;
	i_bits = xrn_i_bits;
    }

    if (WhichWM != DEC_WM) {
	
    /* XXX user sets iconPixmap resource, converter does the right thing.. */
	XtSetArg(arg[0], XtNiconPixmap, &icon_pixmap);
	XtGetValues(whichWidget, arg, 1);
	if (icon_pixmap == (Pixmap) 0) {
	    icon_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char *) bits, width, height);
	    icon_i_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char *) i_bits, width, height);
	    XtSetArg(arg[0], XtNiconPixmap, icon_pixmap);
	    XtSetValues(whichWidget, arg, 1);
	}

    } else {
	/* DEC window manager */
	if (big_pixmap == (Pixmap) 0) {
	    big_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char *) bigicon_bits,
				       bigicon_width, bigicon_height);
	}
	if (big_i_pixmap == (Pixmap) 0) {
	    big_i_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char *) bigicon_i_bits,
				       bigicon_i_width, bigicon_i_height);
	}
	if (small_pixmap == (Pixmap) 0) {
	    small_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char * ) smallicon_bits,
				       smallicon_width, smallicon_height);
	}
	if (small_i_pixmap == (Pixmap) 0) {
	    small_i_pixmap = XCreateBitmapFromData(XtDisplay(TopLevel),
				       XtScreen(TopLevel)->root,
				       (char * ) smallicon_i_bits,
				       smallicon_i_width, smallicon_i_height);
	}
	XtSetArg(arg[0], XtNiconPixmap, big_pixmap);
	XtSetArg(arg[1], XtNiconifyPixmap, small_pixmap);
	XtSetValues(whichWidget, arg, 2);
    }
    xmSetIconAndName(InitIcon);
    if (app_resources.iconGeometry != NIL(char)) {
	int scr, x, y, junk;
	Arg args[2];

	for(scr = 0;	/* yyuucchh */
	    XtScreen(TopLevel) != ScreenOfDisplay(XtDisplay(TopLevel), scr);
	    scr++);

	    XGeometry(XtDisplay(TopLevel), scr, app_resources.iconGeometry,
		  "", 0, 0, 0, 0, 0, &x, &y, &junk, &junk);
	    XtSetArg(args[0], XtNiconX, x);
	    XtSetArg(args[1], XtNiconY, y);
	    XtSetValues(whichWidget, args, XtNumber(args));
    }
    return;
}


static Boolean grabbed = False;
static Widget grabWidget = (Widget) 0;
#ifndef USE_DEC_CURSOR
/* 
 * create the normal and busy xrn cursors
 */

void
busyCursor()
{
    static Cursor BusyCursor = (Cursor) 0;
    Arg args[2];
    
    /* define an appropriate busy cursor */
    if (BusyCursor == (Cursor) 0) {
	XColor colors[2];

	colors[0].pixel = app_resources.pointer_foreground;
	colors[1].pixel = app_resources.pointer_background;
	XQueryColors(XtDisplay(TopLevel),
		     DefaultColormap(XtDisplay(TopLevel),
				     DefaultScreen(XtDisplay(TopLevel))),
		     colors, 2);
	BusyCursor = XCreateFontCursor(XtDisplay(TopLevel), XC_watch);
	XRecolorCursor(XtDisplay(TopLevel), BusyCursor,
		       &colors[0], &colors[1]);
#ifdef MOTIF
	grabWidget = XtCreateWidget("grabWidget", xmPushButtonWidgetClass,
				    TopLevel, NULL, 0);
#else
	grabWidget = DwtPushButtonCreate(TopLevel, "grabWidget", NULL, 0);
#endif
    }
    XDefineCursor(XtDisplay(TopLevel), XtWindow(TopLevel), BusyCursor);
    XDefineCursor(XtDisplay(TopLevel), XtWindow(TopList), BusyCursor);
    if (app_resources.twoWindows && XtIsRealized(ArtTopLevel)) {
	XDefineCursor(XtDisplay(TopLevel), XtWindow(ArticleText), BusyCursor);
    }
    if (ComposeTopLevel != (Widget) 0) {
	XDefineCursor(XtDisplay(ComposeTopLevel),
		XtWindow(ComposeTopLevel), BusyCursor);
	XDefineCursor(XtDisplay(ComposeText),
		XtWindow(ComposeText), BusyCursor);
	XDefineCursor(XtDisplay(HeaderText),
		XtWindow(HeaderText), BusyCursor);
    }
    if (!grabbed) {
	XtAddGrab(grabWidget, True, False);
	grabbed = True;
    }
    if (app_resources.busyIcon) {
	if (WhichWM == DEC_WM) {
	    XtSetArg(args[0], XtNiconPixmap, big_i_pixmap);
	    XtSetArg(args[1], XtNiconifyPixmap, small_i_pixmap);
	    XtSetValues(TopLevel, args, 2);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 2);
	} else {
	    XtSetArg(args[0], XtNiconPixmap, icon_i_pixmap);
	    XtSetValues(TopLevel, args, 1);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 1);
	}
    }
    XFlush(XtDisplay(TopLevel));
    
    return;
}
#else /* USE_DEC_CURSOR */
/* 
 * create the normal and busy xrn cursors
 */

void
busyCursor()
{
    static Cursor BusyCursor = (Cursor) 0;
    static Font cursorFont;
    static XColor colors[2];
    Arg args[2];

    /* define an appropriate busy cursor */
    if (BusyCursor == (Cursor) 0) {
	colors[0].pixel = app_resources.pointer_foreground;
	colors[1].pixel = app_resources.pointer_background;
	XQueryColors(XtDisplay(TopLevel),
		     DefaultColormap(XtDisplay(TopLevel),
				     DefaultScreen(XtDisplay(TopLevel))),
		     colors, 2);
	cursorFont = XLoadFont(XtDisplay(TopLevel), "decw$cursor");
	BusyCursor = XCreateGlyphCursor(XtDisplay(TopLevel),
			cursorFont, cursorFont,
			decw$c_wait_cursor, decw$c_wait_cursor+1,
			&colors[0], &colors[1]);
#ifdef MOTIF
	grabWidget = XtCreateWidget("grabWidget", xmPushButtonWidgetClass,
				    TopLevel, NULL, 0);
#else
	grabWidget = DwtPushButtonCreate(TopLevel, "grabWidget", NULL, 0);
#endif
    }
    XDefineCursor(XtDisplay(TopLevel), XtWindow(TopLevel), BusyCursor);
    XDefineCursor(XtDisplay(TopLevel), XtWindow(TopList), BusyCursor);
    if (app_resources.twoWindows && XtIsRealized(ArtTopLevel)) {
	XDefineCursor(XtDisplay(TopLevel), XtWindow(ArticleText), BusyCursor);
    }
    if (ComposeTopLevel != (Widget) 0) {
	XDefineCursor(XtDisplay(ComposeTopLevel),
		XtWindow(ComposeTopLevel), BusyCursor);
	XDefineCursor(XtDisplay(ComposeText),
		XtWindow(ComposeText), BusyCursor);
	XDefineCursor(XtDisplay(HeaderText),
		XtWindow(HeaderText), BusyCursor);
    }
    if (!grabbed) {
	XtAddGrab(grabWidget, True, False);
	grabbed = True;
    }
    if (app_resources.busyIcon) {
	if (WhichWM == DEC_WM) {
	    XtSetArg(args[0], XtNiconPixmap, big_i_pixmap);
	    XtSetArg(args[1], XtNiconifyPixmap, small_i_pixmap);
	    XtSetValues(TopLevel, args, 2);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 2);
	} else {
	    XtSetArg(args[0], XtNiconPixmap, icon_i_pixmap);
	    XtSetValues(TopLevel, args, 1);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 1);
	}
    }
    XFlush(XtDisplay(TopLevel));
    
    return;
}
#endif

void
unbusyCursor()
{
    Arg args[2];
    XUndefineCursor(XtDisplay(TopLevel), XtWindow(TopLevel));
    XUndefineCursor(XtDisplay(TopLevel), XtWindow(TopList));
    if (app_resources.twoWindows && XtIsRealized(ArtTopLevel)) {
	XUndefineCursor(XtDisplay(TopLevel), XtWindow(ArticleText));
    }
    if (ComposeTopLevel != (Widget) 0) {
	XUndefineCursor(XtDisplay(ComposeTopLevel), XtWindow(ComposeTopLevel));
	XUndefineCursor(XtDisplay(ComposeText), XtWindow(ComposeText));
	XUndefineCursor(XtDisplay(HeaderText), XtWindow(HeaderText));
    }
    if (grabbed) {
	XtRemoveGrab(grabWidget);
    }
    grabbed = False;
    if (app_resources.busyIcon) {
	if (WhichWM == DEC_WM) {
	    XtSetArg(args[0], XtNiconPixmap, big_pixmap);
	    XtSetArg(args[1], XtNiconifyPixmap, small_pixmap);
	    XtSetValues(TopLevel, args, 2);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 2);
	} else {
	    XtSetArg(args[0], XtNiconPixmap, icon_pixmap);
	    XtSetValues(TopLevel, args, 1);
	    if (ComposeTopLevel != (Widget) 0)
		XtSetValues(ComposeTopLevel, args, 1);
	}
    }
    XFlush(XtDisplay(TopLevel));
    
    return;
}

/*ARGSUSED*/
void
CBbusyCursor(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    busyCursor();
    return;
}


/*ARGSUSED*/
void
CBunbusyCursor(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    unbusyCursor();
    return;
}

/* 
 * find out the number of lines in the article display
 */

int
articleLines()
{
    Dimension height = 0;
    Arg arg[1];

#ifdef MOTIF
    XtSetArg(arg[0], XmNrows, &height);
#else
    XtSetArg(arg[0], DwtNrows, &height);
#endif
    XtGetValues(ArticleText, arg, 1);
    if (height < 1 || height > 60) height = 60;
    return(height);
}
/*
 * the following hack allows X11R3 stuff (XUI) to build
 * on systems with X11R4 header files (MOTIF).
 * Brought to you because XUI apps won't build on systems
 * with MOTIF installed otherwise.
 */

#ifndef MOTIF
#ifndef XtWindowOfObject
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
Window XtWindowOfObject(object)
Widget object;
{
    return object->core.window;
}
#endif /* XtWindowOfObject */
#endif /* MOTIF */

#if defined(MOTIF)

/*
 * Function to wrap the text in a text widget (insert proper hard returns)
 */

char *
xmWrapTextWidget(tw)
Widget tw;
{
#if !defined(MOTIF_V10)
char		*oldtextPtr,
		*newtextPtr;
Cardinal	numlines,
		maxnewlines,
		numnewlinesinserted=0,
		line,
		numchars,
		newlen=0,
		oldlen;
XmTextPosition	startpos = 0,
		nextstartpos,
		oldtoppos,
		newtoppos;
long      	byte_count, cvt_status;
XmTextWidget	textwidget = (XmTextWidget) tw;
#ifdef VMS
OutputData	outData;
#endif

/*
** The following contstant is defined in Xm/TextOutP.h
** Note: this is the unsigned int equivalent of -1
*/
#define XRNPASTENDPOS 2147483647

    /* If wordwrap off, just bail out */
#ifndef VMS
    if (!_XmTextShouldWordWrap(textwidget))
	return (XmTextGetString((Widget)textwidget));
#else
    outData = ((XmTextRec *)textwidget)->text.output->data;
    if ((!outData->wordwrap) || outData->scrollhorizontal)
	return (XmTextGetString(tw));
#endif

    /* turn off text widget display or SetTopCharacter calls will scroll */
    _XmTextDisableRedisplay(textwidget,False);

    /* get original text string */
    oldtextPtr = XmTextGetString(tw);

    /* get number of lines in the line number table - one windowfull */
    numlines = _XmTextNumLines(textwidget);

    /* get position of top line so we can put it back at the end */
    oldtoppos = XmTextGetTopCharacter(tw);

    /*
     ** No way to get total # of lines, so make initial allocation at 3 pages.
     ** Realloc if we run out of room
     */
    maxnewlines = numlines * 3;
    oldlen = strlen(oldtextPtr);
    newtextPtr = XtMalloc(oldlen + maxnewlines);
    newtextPtr[0] = '\0';

    /* set top position to very beginning */
    XmTextSetTopCharacter(tw, 0);

    /* get line info for first line */
    _XmTextLineInfo(textwidget,0,&startpos,NULL);
	
    /* do until no more lines */
    do {
	/* for each window full */
	for (line=1;line<=numlines;line++) {
	    /*
	     * Skip this for the last line of the window,
	     * unless it's the only line
	     */
	    if ((line < numlines) || (numlines == 1)) {
		_XmTextLineInfo(textwidget,line,&nextstartpos,NULL);
		/* if no more lines, break */
		if (nextstartpos == XRNPASTENDPOS) {
		    break;
		} else {
		    /*
		     * append line to new buffer - if soft wrapped, add a 
		     * NEWLINE
		    */
		    numchars = nextstartpos-startpos;
		    memcpy(&newtextPtr[newlen], &oldtextPtr[startpos], 
			   numchars);
		    newlen += numchars;
		    if (oldtextPtr[nextstartpos-1] != '\n') {
			newtextPtr[newlen] = '\n';
			newlen++;
			numnewlinesinserted++;
		    };
		    startpos = nextstartpos;
		};
	    };
	};
	/* if not out of lines yet ... */
	if (nextstartpos != XRNPASTENDPOS) {
	    /*
	     * position to next windowfull -
	     * forces text widget to format lines
	     */
	    XmTextScroll(tw,numlines-1);
	    numlines = _XmTextNumLines(textwidget);
	    /* if need more memory, get it */
	    if (numnewlinesinserted + numlines > maxnewlines) {
		maxnewlines = numnewlinesinserted * 2;	/* double it */
		newtextPtr = XtRealloc(newtextPtr, oldlen+maxnewlines);
	    };
	};
    } while (nextstartpos != XRNPASTENDPOS);

    /* append the last line */
    numchars = strlen(&oldtextPtr[startpos]);
    if (numchars > 0) {
	memcpy(&newtextPtr[newlen], &oldtextPtr[startpos], numchars);
	newlen += numchars;
    };

    /* NULL terminate the string */
    newtextPtr[newlen] = '\0';

    /* reset the top position and turn the display back on */
    XmTextSetTopCharacter(tw, oldtoppos);
    _XmTextEnableRedisplay(textwidget);

    XtFree(oldtextPtr);
    return(newtextPtr);

#else /* MOTIF_V10 */
    return(XmTextGetString(tw));
#endif /* MOTIF_V10 */
}

/*
 * Handler for close protocol
 */
void
xmAddWMHandler(shell, func)
Widget	shell;
XtCallbackProc func;
{
#ifndef MOTIF_V10
    static Atom DeleteWindowAtom = None;

    if (DeleteWindowAtom == None) {
	DeleteWindowAtom = XmInternAtom(XtDisplay(shell),
					"WM_DELETE_WINDOW", False);
    }
    XmAddWMProtocolCallback(shell, DeleteWindowAtom, func, NULL);
#endif
}
#endif /* MOTIF */
