
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/killfile.c,v 1.1 1993/01/11 02:15:02 ricks Exp $";
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
 * killfile.c: kill file editor
 */

#include "copyright.h"
#include <stdio.h>
#ifndef VMS
#include <sys/types.h>
#include <sys/stat.h>
#else /* VMS */
#include <descrip.h>
#include <types.h>
#include <stat.h>
#endif /* VMS */
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
#include <time.h>
#include <signal.h>
#include "xthelper.h"
#include "resources.h"
#include "xrn.h"
#include "xmisc.h"
#include "butdefs.h"
#include "news.h"
#include "mesg.h"
#include "error_hnds.h"
#include "internals.h"

extern void addTimeOut();
extern void removeTimeOut();
#ifdef VMS
extern int lib$spawn();
#endif
char error_buffer[2048];

/* entire widget */
static Widget KillTopLevel = (Widget) 0;
/* text window */
static Widget KillText = (Widget) 0;
static Widget editDismiss;
static Widget editSave;
static Boolean KillMapped = False;
static char KillFileName[512];


#ifndef VMS

#ifdef esv
static int forkpid;
#else
static pid_t forkpid;
#endif

#ifdef __STDC__
static void
catch_sigchld(signo)
int signo;
{
    if (signo != SIGCHLD) {
	/* whoops! */
	return;
    }
    (void) signal(SIGCHLD, SIG_DFL);
    if (forkpid != wait(0)) {
	/* whoops! */
	return;
    }
    return;
}
#else
static int
catch_sigchld(signo)
int signo;
{
    if (signo != SIGCHLD) {
	/* whoops! */
	return 1;
    }
    (void) signal(SIGCHLD, SIG_DFL);
    if (forkpid != wait(0)) {
	/* whoops! */
	return 1;
    }
    return 1;
}
#endif /* __STDC__ */
#endif /* VMS */

/*ARGSUSED*/
static void
editDismissFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    KillMapped = False;
    XtUnmapWidget(KillTopLevel);
    return;
}

/*ARGSUSED*/
static void
editSaveFunction(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal *count;
{
    FILE *fp;
    char *str = NULL;
    char *ptr = NULL;

#ifdef MOTIF
    ptr = str = XmTextGetString(KillText);
#else
    ptr = str = DwtSTextGetString(KillText);
#endif
    if ((fp = fopen(KillFileName, "w")) != NULL) {
	while (*str) {
	    if (fputc(*str, fp) == EOF) {
		mesgPane(XRN_SERIOUS, "Error writing kill file (%s): %s",
			 KillFileName, errmsg(errno));
		break;
	    }
	    ++str;
	}
        --str;
	if (*str != '\n') fputc('\n', fp);
	fclose(fp);
    } else {
	mesgPane(XRN_SERIOUS, "Can not open the kill file (%s): %s",
		KillFileName, errmsg(errno));
    }
    if (ptr) XtFree(ptr);

    KillMapped = False;
    XtUnmapWidget(KillTopLevel);
    return;
}

#ifndef MOTIF
#define XmNlabelString DwtNlabel
#define XmNactivateCallback DwtNactivateCallback
#define XmNborderWidth DwtNborderWidth
#endif

BUTTON(editDismiss)
BUTTON(editSave)

/* Used to fork editors when the widget editor is not being used. */

static void
externalKillFileEdit()
{
    char buffer[1024];
    int sysStat;
    int maxdesc;
#ifdef VMS
    struct dsc$descriptor_s cmd_desc;
    int one = 1;		/* cli$m_nowait */
#endif
	
    /*
     * app_resources.editorCommand is a sprintf'able string with a %s where the
     * file name should be placed.  The result should be a command that
     * handles all editing and windowing.
     *
     * Examples are:
     *
     *   emacsclient %s
     *   xterm -e vi %s
     *   xterm -e microEmacs %s
     *
     */
     (void) sprintf(buffer, app_resources.editorCommand, KillFileName);
#ifndef VMS
#ifdef VFORK_SUPPORTED
    if ((forkpid = vfork()) == 0) {
#else
    if ((forkpid = fork()) == 0) {
#endif
	int i;

#ifdef hpux
	maxdesc = _NFILE;
#else
#ifdef SVR4
#include <ulimit.h>
	maxdesc = ulimit(UL_GDESLIM);
#else
	maxdesc = getdtablesize();
#endif
#endif
	for (i = 3; i < maxdesc; i++) {
	    (void) close(i);
	}
	(void) execl("/bin/csh", "csh", "-f", "-c", buffer, 0);
	(void) fprintf(stderr, "execl of %s failed\n", buffer);
	(void) _exit(127);
    }
    if (forkpid < 0) {
	sprintf(error_buffer, "Can not execute editor (%s)", errmsg(errno));
	infoNow(error_buffer);
	return;
    } else {
#ifdef __STDC__
	signal(SIGCHLD, catch_sigchld);
#else
	signal(SIGCHLD, (SIG_PF0) catch_sigchld);
#endif
    }
#else /* VMS */
    cmd_desc.dsc$b_class = DSC$K_CLASS_S;
    cmd_desc.dsc$b_dtype = DSC$K_DTYPE_T;
    cmd_desc.dsc$w_length = strlen(buffer);
    cmd_desc.dsc$a_pointer = buffer;

    sysStat = lib$spawn(&cmd_desc,0,0,&one);
    if ((sysStat & 1) != 1) {
	mesgPane(XRN_SERIOUS, "Error return from spawn - %d, aborting",
		sysStat);
	return;
    }
#endif
    return;
}

void
EditKillFile(group)
struct newsgroup *group;
/*
 * brings up a new vertical pane, not moded
 *
 * the pane consists of 3 parts: title bar, scrollable text window,
 * button box
 */
{
    static Widget pane, buttonBox, label;
    Arg paneArgs[4];
    Position x = 0;
    Position y = 0;
    Dimension width = 0;
    Dimension height = 0;
#ifdef MOTIF
    Widget frame;
#endif
    int c, i;
    static char *ptr = NULL;
    char *end;
    static int ptrlen = 0;
    struct stat buf;
    FILE *filefp;
    char labelString[128];

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
	{XmNrows,		(XtArgVal) 20},
	{XmNcolumns,		(XtArgVal) 80},
	{XmNwordWrap, 		(XtArgVal) FALSE},
	{XmNscrollVertical, 	(XtArgVal) TRUE},
	{XmNscrollHorizontal, 	(XtArgVal) TRUE},
	{XmNeditMode,  		(XtArgVal) XmMULTI_LINE_EDIT},
	{XmNeditable,		(XtArgVal) TRUE},
#else
	{DwtNrows,		(XtArgVal) 20},
	{DwtNcols,		(XtArgVal) 80},
	{DwtNwordWrap, 		(XtArgVal) FALSE},
	{DwtNscrollVertical, 	(XtArgVal) TRUE},
	{DwtNeditable,		(XtArgVal) TRUE},
#endif
    };


    if (ptr) {
	ptr[0] = '\0';
    }
    if (group) {
	strcpy(KillFileName, localKillFile(group, 1));
    } else {
	strcpy(KillFileName, globalKillFile());
    }
    if (app_resources.editorCommand != NIL(char)) {
	externalKillFileEdit();
	return;
    }
    if ((stat(KillFileName, &buf) != -1) &&
	((filefp = fopen(KillFileName, "r")) != NULL)) {
	buf.st_size = buf.st_size + 2;
	if (ptrlen == 0) {
	    ptr = XtMalloc(buf.st_size);
	    ptrlen = buf.st_size;
	} else {
	    if (buf.st_size > ptrlen) {
		ptr = XtRealloc(ptr, buf.st_size);
		ptrlen = buf.st_size;
	    }
	}
	end = ptr;
	while ((c = getc(filefp)) != EOF) {
	    *end = c;
	    end++;
	}
	*end = '\0';
	(void) fclose(filefp);
    }
    sprintf(labelString, "Editing Kill File for group %s", group->name);
#ifdef MOTIF
    labelArgs[0].value = (XtArgVal) XmStringLtoRCreate(labelString,
		XmSTRING_DEFAULT_CHARSET);
#else
    labelArgs[0].value = (XtArgVal) DwtLatin1String(labelString);
#endif

    if (KillTopLevel != (Widget) 0 && !KillMapped) {
#ifdef MOTIF
	XmTextSetString(KillText, ptr);
	XtSetValues(label, labelArgs, XtNumber(labelArgs));
	XmStringFree((XmString)labelArgs[0].value);
#else
	DwtSTextSetString(KillText, ptr);
	XtSetValues(label, labelArgs, XtNumber(labelArgs));
	XtFree(labelArgs[0].value);
#endif
	XtMapWidget(KillTopLevel);
#if defined(MOTIF) && !defined(MOTIF_V10)
	XmProcessTraversal(KillText, XmTRAVERSE_CURRENT);
#endif
	KillMapped = True;
	return;
    }
    if (KillTopLevel == (Widget) 0) {
	KillTopLevel = XtCreatePopupShell("Kill File Edit",
				topLevelShellWidgetClass,
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
				    KillTopLevel, paneArgs, XtNumber(paneArgs));
#else
	pane = DwtPaneCreate(KillTopLevel, "killEditPane", 
				    paneArgs, 3);
	XtManageChild(pane);
#endif

#ifdef MOTIF
	label = XtCreateManagedWidget("label", xmLabelWidgetClass, pane,
				      labelArgs, XtNumber(labelArgs));

	XmStringFree((XmString)labelArgs[0].value);
	frame = XtCreateManagedWidget("killFrame", xmFrameWidgetClass,
					pane, NULL, 0);
	KillText = XmCreateScrolledText(frame, "text", 
					 textArgs, XtNumber(textArgs));
	XmTextSetString(KillText, ptr);
#else
	label = DwtLabelCreate(pane, "label", 
				      labelArgs, XtNumber(labelArgs));

	XtFree(labelArgs[0].value);
	XtManageChild(label);
	KillText = DwtSTextCreate(pane, "text", 
					 textArgs, XtNumber(textArgs));
	DwtSTextSetString(KillText, ptr);
#endif
	XtManageChild(KillText);

#ifdef MOTIF
	buttonBox = XtCreateManagedWidget("box", xmRowColumnWidgetClass, pane,
					  boxArgs, XtNumber(boxArgs));
	editSaveArgs[1].value = (XtArgVal) XmStringLtoRCreate(
						"Save",
						XmSTRING_DEFAULT_CHARSET);
	editSave = XtCreateManagedWidget("save", xmPushButtonWidgetClass,
			      buttonBox, editSaveArgs, XtNumber(editSaveArgs));
    
	XmStringFree((XmString) editSaveArgs[1].value);
	editDismissArgs[1].value = (XtArgVal) XmStringLtoRCreate(
						"Dismiss",
						XmSTRING_DEFAULT_CHARSET);
	editDismiss = XtCreateManagedWidget("dismiss", xmPushButtonWidgetClass,
			      buttonBox, editDismissArgs, XtNumber(editDismissArgs));
    
	XmStringFree((XmString) editDismissArgs[1].value);
#else
	buttonBox = DwtMenuCreate(pane, "box", 
					  boxArgs, XtNumber(boxArgs));
	XtManageChild(buttonBox);
	editSaveArgs[1].value = (XtArgVal) DwtLatin1String("Save");
	XtManageChild(DwtPushButtonCreate(buttonBox, "save",
			      editSaveArgs, XtNumber(editSaveArgs)));
    
	XtFree(editSaveArgs[1].value);
	editDismissArgs[1].value = (XtArgVal) DwtLatin1String("Dismiss");
	XtManageChild(DwtPushButtonCreate(buttonBox, "dismiss",
			      editDismissArgs, XtNumber(editDismissArgs)));
    
	XtFree(editDismissArgs[1].value);
#endif
	XtRealizeWidget(KillTopLevel);

	XtPopup(KillTopLevel, XtGrabNone);
#if defined(MOTIF) && !defined(MOTIF_V10)
	XmProcessTraversal(editDismiss, XmTRAVERSE_CURRENT);
#endif
	KillMapped = True;

	/* xthCenterWidgetOverCursor(KillTopLevel); */

    }
    return;
}

