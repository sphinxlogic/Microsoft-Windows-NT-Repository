
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/mesg.c,v 1.6 1993/01/11 02:15:02 ricks Exp $";
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
 * mesg.c: message box
 */

#include "copyright.h"
#include <stdio.h>
#include "config.h"
#include "utils.h"
#ifndef VMS
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#else
#include <decw$include/cursorfont.h>
#include <decw$include/StringDefs.h>
#include <decw$include/Intrinsic.h>
#include <decw$include/Shell.h>
#endif
#ifdef MOTIF
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextP.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#endif
#else /* MOTIF */
#ifndef DwtNmenuExtendLastRow
#define DwtNmenuExtendLastRow "menuExtendLastRow"
#endif
#ifndef VMS
#include <X11/DECwDwtWidgetProg.h>
#else
#include <decw$include/DECwDwtWidgetProg.h>
#endif /* VMS */
#include "Pane.h"
#endif /* MOTIF */
#include "xthelper.h"
#include "resources.h"
#include "xrn.h"
#include "mesg.h"
#if defined(__STDC__) && !defined(NOSTDHDRS)
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <time.h>
#include "xmisc.h"
#include "butdefs.h"

extern void addTimeOut();
extern void removeTimeOut();

char error_buffer[2048];

/* entire widget */
static Widget MesgTopLevel = (Widget) 0;
/* text window */
static Widget MesgText = (Widget) 0;
static Widget dismiss;

static Boolean MesgMapped = False;

#define MESG_SIZE 4096
static char MesgString[MESG_SIZE];
/*
 * If you have a window larger than 512 characters across, or there is
 * an info message to be displayed that is longer than 512 characters,
 * then someone should be shot!
 */
static char InfoString[512];

/* whether or not we should hold off redisplaying the pane */
static int delay_redisplay = 0;

/*ARGSUSED*/
static void
dismissFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    MesgMapped = False;
    XtUnmapWidget(MesgTopLevel);
    return;
}

#ifndef MOTIF
#define XmNlabelString DwtNlabel
#define XmNactivateCallback DwtNactivateCallback
#define XmNborderWidth DwtNborderWidth
#endif

BUTTON(dismiss)

static void displayMesgString()
{
    if (delay_redisplay || (! MesgText))
	return;

#ifdef MOTIF
    XmTextSetString(MesgText, MesgString);
    XmTextShowPosition(MesgText, (XmTextPosition) strlen(MesgString));
#else
    DwtSTextSetString(MesgText, MesgString);
    DwtTextShowPosition(MesgText, strlen(MesgString));
#endif
}

void mesgDisableRedisplay()
{
    delay_redisplay = 1;
    return;
}

void mesgEnableRedisplay()
{
    delay_redisplay = 0;
    displayMesgString();
    return;
}

/*ARGSUSED*/
/*VARARGS2*/
#if defined(__STDC__) && !defined(NOSTDHDRS)
void
mesgPane(int type, char *fmtString, ...)
#else
void
mesgPane(type, fmtString, va_alist)
int type;		/* XRN_INFO, XRN_SERIOUS */
char *fmtString;
#endif
#if !defined(__STDC__) || defined(NOSTDHDRS)
    va_dcl
#endif
/*
 * brings up a new vertical pane, not moded
 *
 * the pane consists of 3 parts: title bar, scrollable text window,
 * button box
 */
{
    va_list args;
    Widget pane, buttonBox, label;
    Arg paneArgs[4];
    Position x = 0;						    /* ggh */
    Position y = 0;						    /* ggh */
    Dimension width = 0;					    /* ggh */
    Dimension height = 0;					    /* ggh */
#ifdef MOTIF
    Widget frame;
#endif
    static Arg labelArgs[] = {
#ifdef MOTIF
	{XmNlabelString, (XtArgVal) NULL},
	{XmNskipAdjust, (XtArgVal) True},
#else
	{DwtNlabel,	(XtArgVal) NULL},
#endif
    };
    static Arg boxArgs[] = {
#ifdef MOTIF
	{XmNnumColumns,		(XtArgVal) 1},
	{XmNadjustLast, 	(XtArgVal) False},
	{XmNorientation,	(XtArgVal) XmHORIZONTAL},
	{XmNpacking,		(XtArgVal) XmPACK_COLUMN},
	{XmNallowResize,	(XtArgVal) True},
	{XmNskipAdjust, 	(XtArgVal) True},
#else
	{DwtNresizeHeight,	(XtArgVal) True},
	{DwtNresizeWidth,	(XtArgVal) False},
	{DwtNorientation,	(XtArgVal) DwtOrientationHorizontal},
	{DwtNmenuPacking,	(XtArgVal) DwtMenuPackingTight},
	{DwtNmenuExtendLastRow, (XtArgVal) False},
	{DwtNborderWidth,	(XtArgVal) 3},
	{DwtNentryBorder,	(XtArgVal) 2},
	{DwtNchildOverlap,	(XtArgVal) False},
	{DwtNmenuNumColumns,	(XtArgVal) 4},
	{DwtNmin,		(XtArgVal) 30},
#endif
    };
    static Arg shellArgs[] = {
	{XtNinput, (XtArgVal) True},
	{XtNsaveUnder, (XtArgVal) False},
    };
    static Arg textArgs[] = {
#ifdef MOTIF
	{XmNrows,		(XtArgVal) 10},
	{XmNcolumns,		(XtArgVal) 80},
	{XmNwordWrap, 		(XtArgVal) TRUE},
	{XmNscrollVertical, 	(XtArgVal) TRUE},
	{XmNeditMode,  		(XtArgVal) XmMULTI_LINE_EDIT},
	{XmNeditable,		(XtArgVal) FALSE},
#else
	{DwtNrows,		(XtArgVal) 10},
	{DwtNcols,		(XtArgVal) 80},
	{DwtNwordWrap, 		(XtArgVal) TRUE},
	{DwtNscrollVertical, 	(XtArgVal) TRUE},
	{DwtNeditable,		(XtArgVal) FALSE},
#endif
    };
    time_t tm;
    char addBuff2[MESG_SIZE];

#if defined(__STDC__) && !defined(NOSTDHDRS)
    va_start(args, fmtString);
#else    
    va_start(args);
#endif

    tm = time(0);

    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
	(void) fprintf(stderr, "%-24.24s: ", ctime(&tm));
	(void) vfprintf(stderr, fmtString, args);
	(void) fprintf(stderr, "\n");
	return;
    }

    if ((type == XRN_INFO) && (app_resources.info == False)) {
	(void) vsprintf(InfoString, fmtString, args);
	infoNow(InfoString);
	return;
    }

    if (MesgTopLevel != (Widget) 0 && !MesgMapped) {
	(void) vsprintf(addBuff2, fmtString, args);
	(void) sprintf(MesgString, "%-24.24s: %s", ctime(&tm), addBuff2);
#ifdef MOTIF
	XmTextSetString(MesgText, MesgString);
	XmTextShowPosition (MesgText, (XmTextPosition) strlen(MesgString));
#else
	DwtSTextSetString(MesgText, MesgString);
	DwtTextShowPosition(MesgText, strlen(MesgString));
#endif
	XtMapWidget(MesgTopLevel);
#if defined(MOTIF) && !defined(MOTIF_V10)
	XmProcessTraversal(dismiss, XmTRAVERSE_CURRENT);
#endif
	MesgMapped = True;
	return;
    }
    if (MesgTopLevel == (Widget) 0) {
	MesgTopLevel = XtCreatePopupShell("Information", topLevelShellWidgetClass,
					  TopLevel, shellArgs, XtNumber(shellArgs));

	XtSetArg(paneArgs[0], XtNx, &x);
	XtSetArg(paneArgs[1], XtNy, &y);
	XtSetArg(paneArgs[2], XtNwidth, &width);
	XtSetArg(paneArgs[3], XtNheight, &height);
	XtGetValues(TopLevel, paneArgs, XtNumber(paneArgs));
	XtSetArg(paneArgs[0], XtNx, x);
	XtSetArg(paneArgs[1], XtNy, y);
	XtSetArg(paneArgs[2], XtNwidth, width);
	XtSetArg(paneArgs[3], XtNheight, height);
#ifdef MOTIF
	pane = XtCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				    MesgTopLevel, paneArgs, XtNumber(paneArgs));
#else
	pane = DwtPaneCreate(MesgTopLevel, "mesgPane", 
				    paneArgs, 3);
	XtManageChild(pane);
#endif

	(void) vsprintf(addBuff2, fmtString, args);
	(void) sprintf(MesgString, "%-24.24s: %s", ctime(&tm), addBuff2);

	if (labelArgs[0].value == 0) {
#ifdef MOTIF
	    labelArgs[0].value = (XtArgVal) XmStringLtoRCreate(
		"Information (can be left up or dismissed)",
		XmSTRING_DEFAULT_CHARSET);
#else
	    labelArgs[0].value = (XtArgVal) DwtLatin1String(
		"Information (can be left up or dismissed)");
#endif
	}
#ifdef MOTIF
	label = XtCreateManagedWidget("label", xmLabelWidgetClass, pane,
				      labelArgs, XtNumber(labelArgs));

	XmStringFree((XmString)labelArgs[0].value);
	frame = XtCreateManagedWidget("mesgFrame", xmFrameWidgetClass,
					pane, NULL, 0);
	MesgText = XmCreateScrolledText(frame, "text", 
					 textArgs, XtNumber(textArgs));
#else
	label = DwtLabelCreate(pane, "label", 
				      labelArgs, XtNumber(labelArgs));

	XtFree(labelArgs[0].value);
	XtManageChild(label);
	MesgText = DwtSTextCreate(pane, "text", 
					 textArgs, XtNumber(textArgs));
#endif
	XtManageChild(MesgText);

#ifdef MOTIF
	buttonBox = XtCreateManagedWidget("box", xmRowColumnWidgetClass, pane,
					  boxArgs, XtNumber(boxArgs));
	dismissArgs[1].value = (XtArgVal) XmStringLtoRCreate(
						"Dismiss",
						XmSTRING_DEFAULT_CHARSET);
	dismiss = XtCreateManagedWidget("dismiss", xmPushButtonWidgetClass,
			      buttonBox, dismissArgs, XtNumber(dismissArgs));
    
	XmStringFree((XmString) dismissArgs[1].value);
#else
	buttonBox = DwtMenuCreate(pane, "box", 
					  boxArgs, XtNumber(boxArgs));
	XtManageChild(buttonBox);
	dismissArgs[1].value = (XtArgVal) DwtLatin1String("Dismiss");
	XtManageChild(DwtPushButtonCreate(buttonBox, "dismiss",
			      dismissArgs, XtNumber(dismissArgs)));
    
	XtFree(dismissArgs[1].value);
#endif
	XtRealizeWidget(MesgTopLevel);

	XtPopup(MesgTopLevel, XtGrabNone);
#if defined(MOTIF) && !defined(MOTIF_V10)
	XmProcessTraversal(dismiss, XmTRAVERSE_CURRENT);
#endif
	MesgMapped = True;

	/* xthCenterWidgetOverCursor(MesgTopLevel); */

	displayMesgString();
    } else {
	long len;
	long newlen;
	char addBuff[MESG_SIZE];

	(void) vsprintf(addBuff2, fmtString, args);
	(void) sprintf(addBuff, "%-24.24s: %s", ctime(&tm), addBuff2);
	len = strlen(MesgString);
	newlen = strlen(addBuff);

	if ((len + 10 + newlen) > MESG_SIZE) {
	    (void) strcpy(MesgString, addBuff);
	} else {	
	    if (! (type & XRN_APPEND)) {
		(void) strcat(&MesgString[len], "\n--------\n");
		(void) strcat(&MesgString[len + 10], addBuff);
	    } else {
		(void) strcat(&MesgString[len], "\n");
		(void) strcat(&MesgString[len + 1], addBuff);
	    }
	}

	displayMesgString();
    }
    
    return;
}


void
info(msg)
char *msg;
/*
 * put an informational 'msg' into the top information label
 */
{
    Arg infoLineArg[1];
#ifdef MOTIF
    XmString labelString;
#else
    DwtCompString labelString;
#endif

    if ((XRNState & XRN_X_UP) == XRN_X_UP) {
#ifdef MOTIF
	labelString = XmStringLtoRCreate(msg, XmSTRING_DEFAULT_CHARSET);
	XtSetArg(infoLineArg[0], XmNlabelString, labelString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XmStringFree(labelString);
#else
	labelString = DwtLatin1String(msg);
	XtSetArg(infoLineArg[0], DwtNlabel, labelString);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
	XtFree(labelString);
#endif
    } else {
	(void) fprintf(stderr, "XRN: %s\n", msg);
    }
    return;
}


void
infoNow(msg)
char *msg;
/*
 * put an informational 'msg' into the top information label and force an update
 */
{
    info(msg);
    if ((XRNState & XRN_X_UP) == XRN_X_UP) {
	xthHandleAllPendingEvents();
    }
    return;
}

