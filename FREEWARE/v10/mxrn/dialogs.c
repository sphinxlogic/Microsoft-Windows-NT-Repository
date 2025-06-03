
#if !defined(lint) &&!defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/dialogs.c,v 1.7 1993/02/04 18:22:16 ricks Exp $";
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
 * dialogs.c: create simple moded dialog boxes
 */

#include "copyright.h"
#include "config.h"
#include <stdio.h>
#include "utils.h"
#ifndef VMS
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#else
#include <decw$include/StringDefs.h>
#include <decw$include/Intrinsic.h>
#endif
#ifdef MOTIF
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>
#include <Xm/BulletinBP.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/SelectioBP.h>
#include <Xm/RowColumn.h>
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#include <X11/Protocols.h>
#else
#include <Xm/Protocols.h>
#endif /* MOTIF_V10 */
#else /* MOTIF */
#ifndef VMS
#include <X11/DECwDwtWidgetProg.h>
#else
#include <decw$include/DECwDwtWidgetProg.h>
#endif /* VMS */
#endif /* MOTIF */
#ifndef VMS
#include <X11/Shell.h>
#else
#include <decw$include/Shell.h>
#endif
#include "xthelper.h"
#include "xmisc.h"
#include "xrn.h"
#include "dialogs.h"
#include "resources.h"

#ifdef MOTIF
Widget
CreateDialog(parent, title, question, textField, args, count)
Widget parent;           /* parent window                         */
char *title;		 /* title for the dialog box              */
char *question;          /* question for the dialog box           */
char *textField;	 /* default text field                    */
struct DialogArg *args;  /* description of the buttons            */
unsigned int count;      /* number of buttons                     */
/*
 * create a simple moded dialog box
 */
{
    Widget popup;
    Arg dargs[10];
    Arg bargs[10];
    Widget button;
    int i, j;
    Dimension x, y, width;
    char realTitle[80];
    XmString labelString;
    XmString titleString;
    static XtCallbackRec callbacks[] = {
	    {(XtCallbackProc) CBbusyCursor, NULL},
	    {NULL, NULL},
	    {(XtCallbackProc) CBunbusyCursor, NULL},
	    {NULL, NULL},
    };

    static Arg rowArgs[] = {
	{XmNorientation, XmHORIZONTAL},
	{XmNpacking,	 XmPACK_COLUMN},
	{XmNspacing,	 20}
    };

    Widget rowcol;
    XmSelectionBoxWidget dialog;

    /* override does not get titlebar, transient does */
    
    sprintf(realTitle, "%s: %s", app_resources.progName, title);
    i = 0;
    XtSetArg(dargs[i], XtNallowShellResize, True); i++;
    XtSetArg(dargs[i], XmNtextColumns, 40); i++;
    titleString = XmStringLtoRCreate(realTitle, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(dargs[i], XmNdialogTitle, titleString);i++;
    labelString = XmStringLtoRCreate(question, XmSTRING_DEFAULT_CHARSET);

    /* create the dialog box */
    if (textField != DIALOG_NOTEXT || count > 3) {
	XtSetArg(dargs[i], XtNinput, True); i++;
        XtSetArg(dargs[i], XmNselectionLabelString, labelString); i++;
	XtSetArg(dargs[i], XmNresizePolicy, XmRESIZE_GROW);i++;
	XtSetArg(dargs[i], XmNwidth, 200);i++;
	popup = (Widget) XmCreatePromptDialog(parent,title,dargs,i);
	if (textField != DIALOG_NOTEXT) {
	    i = 0;
	    XtSetArg(dargs[i], XmNcolumns, 25);i++;
	    XtSetArg(dargs[i], XmNvalue, textField);i++;
	    XtSetValues(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT), dargs, i);
	} else {
	    XtUnmanageChild(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT));
	}
	XmStringFree(labelString);
	XmStringFree(titleString);
	
    /* Create the buttons */
    /* Button 1: */
	
	callbacks[1].callback = args[0].handler;
	callbacks[1].closure = args[0].data;

        labelString = XmStringLtoRCreate(args[0].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	button = (Widget) XmSelectionBoxGetChild(popup,XmDIALOG_OK_BUTTON);
	j = 0;
	XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	XtSetValues(button, bargs, j);
	XmStringFree(labelString);

    /* Button 2: */
	
	if (count < 2) {
	    XtUnmanageChild(XmSelectionBoxGetChild(popup,
				XmDIALOG_CANCEL_BUTTON));
	} else {
	    callbacks[1].callback = args[1].handler;
	    callbacks[1].closure = args[1].data;

	    labelString = XmStringLtoRCreate(args[1].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    button = (Widget) XmSelectionBoxGetChild(popup,
							XmDIALOG_CANCEL_BUTTON);
	    j = 0;
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	    XtSetValues(button, bargs, j);
	    XmStringFree(labelString);
	}
    /* Button 3: */
	
	if (count < 3) {
	    XtUnmanageChild(XmSelectionBoxGetChild(popup,
				XmDIALOG_HELP_BUTTON));
	} else {
	    callbacks[1].callback = args[2].handler;
	    callbacks[1].closure = args[2].data;

	    labelString = XmStringLtoRCreate(args[2].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    button = (Widget) XmSelectionBoxGetChild(popup,
							XmDIALOG_HELP_BUTTON);
	    j = 0;
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	    XtSetValues(button, bargs, j);
	    XmStringFree(labelString);
	}

    /* Button 4: */
	
	if (count < 5) {
	    if (count > 3) {
		callbacks[1].callback = args[3].handler;
		callbacks[1].closure = args[3].data;

		labelString = XmStringLtoRCreate(args[3].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
		button = (Widget) XmSelectionBoxGetChild(popup,
					XmDIALOG_APPLY_BUTTON);
		XtManageChild(button);
		j = 0;
		XtSetArg(bargs[j], XmNlabelString, labelString); j++;
		XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

		XtSetValues(button, bargs, j);
		XmStringFree(labelString);
	    }
	}

    /* Button 5: */

	if (count > 4) {
	    XmSelectionBoxWidget selb = (XmSelectionBoxWidget) popup;
	    selb->selection_box.adding_sel_widgets = True;
	    rowcol = XmCreateRowColumn(popup, "dialogrc", rowArgs,
					XtNumber(rowArgs));
	    selb->selection_box.adding_sel_widgets = False;

	    callbacks[1].callback = args[3].handler;
	    callbacks[1].closure = args[3].data;

	    labelString = XmStringLtoRCreate(args[3].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    j = 0;
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;
	    button = (Widget) XmCreatePushButtonGadget(rowcol, 
				args[3].buttonName, bargs, j);
	    XtManageChild(button);

	    callbacks[1].callback = args[4].handler;
	    callbacks[1].closure = args[4].data;

	    j = 0;
	    labelString = XmStringLtoRCreate(args[4].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;
	    button = (Widget) XmCreatePushButtonGadget(rowcol, 
				args[4].buttonName, bargs, j);
	    XtManageChild(button);
	    XmStringFree(labelString);
	    SB_ApplyButton(popup) = rowcol;
	    XtManageChild(rowcol);
	}

    } else {	/* No text field */

	XtSetArg(dargs[i], XtNinput, False); i++;
        XtSetArg(dargs[i], XmNmessageString, labelString); i++;
	popup = (Widget) XmCreateQuestionDialog(parent,title,dargs,i);
	XmStringFree(labelString);
	XmStringFree(titleString);
	
    /* Create the buttons */
    /* Button 1: */
	
	callbacks[1].callback = args[0].handler;
	callbacks[1].closure = args[0].data;

        labelString = XmStringLtoRCreate(args[0].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	button = (Widget) XmMessageBoxGetChild(popup,XmDIALOG_OK_BUTTON);
	j = 0;
	XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	XtSetValues(button, bargs, j);
	XmStringFree(labelString);

    /* Button 2: */
	
	if (count < 2) {
	    XtUnmanageChild(XmMessageBoxGetChild(popup,
				XmDIALOG_CANCEL_BUTTON));
	} else {
	    callbacks[1].callback = args[1].handler;
	    callbacks[1].closure = args[1].data;

	    labelString = XmStringLtoRCreate(args[1].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    button = (Widget) XmMessageBoxGetChild(popup,
					XmDIALOG_CANCEL_BUTTON);
	    j = 0;
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	    XtSetValues(button, bargs, j);
	    XmStringFree(labelString);
	}
    /* Button 3: */
	
	if (count < 3) {
	    XtUnmanageChild(XmMessageBoxGetChild(popup,
				XmDIALOG_HELP_BUTTON));
	} else {
	    callbacks[1].callback = args[2].handler;
	    callbacks[1].closure = args[2].data;

	    labelString = XmStringLtoRCreate(args[2].buttonName,
					 XmSTRING_DEFAULT_CHARSET);
	    button = (Widget) XmMessageBoxGetChild(popup,XmDIALOG_HELP_BUTTON);
	    j = 0;
	    XtSetArg(bargs[j], XmNlabelString, labelString); j++;
	    XtSetArg(bargs[j], XmNactivateCallback, callbacks);j++;

	    XtSetValues(button, bargs, j);
	    XmStringFree(labelString);
	}
    }
    unbusyCursor();
    XtRealizeWidget(popup);
#ifndef MOTIF_V10
    if (textField != DIALOG_NOTEXT) {
	/* Doing this twice works around a Motif bug */
	XmProcessTraversal(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT),
				XmTRAVERSE_CURRENT);
	XmProcessTraversal(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT),
				XmTRAVERSE_CURRENT);
    }
#endif
    return(popup);
}
#else /* not MOTIF */
Widget
CreateDialog(parent, title, question, textField, args, count)
Widget parent;           /* parent window                         */
char *title;             /* title of the dialog box               */
char *question;		 /* question for the dialog box           */
char *textField;	 /* default text field                    */
struct DialogArg *args;  /* description of the buttons            */
unsigned int count;      /* number of buttons                     */
/*
 * create a simple moded dialog box
 */
{
    DialogWidget popup;
    Widget text;
    Widget button4;
    Widget button5;
    Widget defaultButton;
    Arg dargs[30];
    Arg bargs[10];
    int i, j;
    Dimension x, y, width;
    DwtCompString titleString;
    DwtCompString labelString;
    char realTitle[80];
    static XtCallbackRec callbacks[] = {
	    {busyCursor, NULL},
	    {NULL, NULL},
	    {unbusyCursor, NULL},
	    {NULL, NULL},
    };

    /* override does not get titlebar, transient does */
    
    sprintf(realTitle, "%s: %s", app_resources.progName, title);
    i = 0;
    XtSetArg(dargs[i], XtNallowShellResize, True); i++;
    XtSetArg(dargs[i], DwtNchildOverlap, False); i++;
    XtSetArg(dargs[i], DwtNunits, DwtFontUnits); i++;
    titleString = DwtLatin1String(realTitle);
    XtSetArg(dargs[i], DwtNtitle, titleString);i++;
    labelString = DwtLatin1String(question);

    /* create the dialog box */
    if (textField != DIALOG_NOTEXT) {
	XtSetArg(dargs[i], DwtNinput, True); i++;
        XtSetArg(dargs[i], DwtNlabel, labelString); i++;
	popup = (DialogWidget) DwtCautionBoxCreate((Widget)parent,(char *)title,
			dargs, i);
	XtFree(labelString);
	XtFree(titleString);
    /* create the SText */
 
	i = 0;
	XtSetArg(dargs[i], DwtNvalue, textField);i++;
	XtSetArg(dargs[i], DwtNeditable, TRUE); i++;
	XtSetArg(dargs[i], DwtNx, 40);i++;
	XtSetArg(dargs[i], DwtNy, 100);i++;
	XtSetArg(dargs[i], DwtNrows, 1);i++;
	XtSetArg(dargs[i], DwtNcols, 40);i++;
	XtSetArg(dargs[i], DwtNresizeWidth, FALSE);i++;

	text = DwtSTextCreate(popup, "text", dargs, i);
	XtManageChild(text);
	
	i = 0;
	XtSetArg(dargs[i], DwtNuserData, text); i++;
	XtSetValues(popup, dargs, i);

    /* Create the buttons */
    /* Button 1: */
	
	callbacks[1].callback = args[0].handler;
	callbacks[1].closure = args[0].data;

        labelString = DwtLatin1String(args[0].buttonName);
	j = 0;
	XtSetArg(dargs[j], DwtNyesLabel, labelString); j++;
	XtSetArg(dargs[j], DwtNyesCallback, callbacks);j++;

	XtSetValues(popup, dargs, j);
	XtFree(labelString);

    /* Button 2: */
	
	if (count < 2) {
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtNoButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    XtUnmanageChild(defaultButton);
	} else {
	
	    callbacks[1].callback = args[1].handler;
	    callbacks[1].closure = args[1].data;

	    labelString = DwtLatin1String(args[1].buttonName);
	    j = 0;
	    XtSetArg(dargs[j], DwtNnoLabel, labelString); j++;
	    XtSetArg(dargs[j], DwtNnoCallback, callbacks);j++;

	    XtSetValues(popup, dargs, j);
	    XtFree(labelString);
	}
    /* Button 3: */
	
	if (count < 3) {
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtCancelButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    XtUnmanageChild(defaultButton);
	} else {
	    callbacks[1].callback = args[2].handler;
	    callbacks[1].closure = args[2].data;

	    labelString = DwtLatin1String(args[2].buttonName);
	    j = 0;
	    XtSetArg(dargs[j], DwtNcancelLabel, labelString); j++;
	    XtSetArg(dargs[j], DwtNcancelCallback, callbacks);j++;

	    XtSetValues(popup, dargs, j);
	    XtFree(labelString);
	}

    /* Button 4: */

	if (count > 3) {
	    callbacks[1].callback = args[3].handler;
	    callbacks[1].closure = args[3].data;

	    labelString = DwtLatin1String(args[3].buttonName);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtCancelButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, &x); j++;
	    XtSetArg(bargs[j], DwtNy, &y); j++;
	    XtSetArg(bargs[j], DwtNwidth, &width); j++;
	    XtGetValues(defaultButton, bargs, j);
	    x = x + width + 10;
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, x); j++;
	    XtSetArg(bargs[j], DwtNy, y); j++;
	    XtSetArg(bargs[j], DwtNlabel, labelString); j++;
	    XtSetArg(bargs[j], DwtNactivateCallback, callbacks);j++;
	    button4 = DwtPushButtonGadgetCreate(popup,args[4].buttonName,
		bargs, j);
	    XtManageChild(button4);
	    XtFree(labelString);
	}

    /* Button 5: */

	if (count > 4) {
	    callbacks[1].callback = args[4].handler;
	    callbacks[1].closure = args[4].data;

	    labelString = DwtLatin1String(args[4].buttonName);
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, &x); j++;
	    XtSetArg(bargs[j], DwtNy, &y); j++;
	    XtSetArg(bargs[j], DwtNwidth, &width); j++;
	    XtGetValues(button4, bargs, j);
	    x = x + width + 10;
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, x); j++;
	    XtSetArg(bargs[j], DwtNy, y); j++;
	    XtSetArg(bargs[j], DwtNlabel, labelString); j++;
	    XtSetArg(bargs[j], DwtNactivateCallback, callbacks);j++;
	    button5 = DwtPushButtonGadgetCreate(popup,args[4].buttonName,
		bargs, j);
	    XtManageChild(button5);
	    XtFree(labelString);
	}

    } else {	/* No text field */

	XtSetArg(dargs[i], DwtNinput, False); i++;
        XtSetArg(dargs[i], DwtNlabel, labelString); i++;
	popup = (DialogWidget) DwtCautionBoxCreate(parent,title,dargs,i);
	XtFree(titleString);
	XtFree(labelString);
	
    /* Create the buttons */
    /* Button 1: */
	
	callbacks[1].callback = args[0].handler;
	callbacks[1].closure = args[0].data;

        labelString = DwtLatin1String(args[0].buttonName);
	j = 0;
	XtSetArg(dargs[j], DwtNyesLabel, labelString); j++;
	XtSetArg(dargs[j], DwtNyesCallback, callbacks);j++;

	XtSetValues(popup, dargs, j);
	XtFree(labelString);

    /* Button 2: */
	
	if (count < 2) {
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtNoButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    XtUnmanageChild(defaultButton);
	} else {
	    callbacks[1].callback = args[1].handler;
	    callbacks[1].closure = args[1].data;

	    labelString = DwtLatin1String(args[1].buttonName);
	    j = 0;
	    XtSetArg(dargs[j], DwtNnoLabel, labelString); j++;
	    XtSetArg(dargs[j], DwtNnoCallback, callbacks);j++;

	    XtSetValues(popup, dargs, j);
	    XtFree(labelString);
	}
    /* Button 3: */
	
	if (count < 3) {
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtCancelButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    XtUnmanageChild(defaultButton);
	} else {
	    callbacks[1].callback = args[2].handler;
	    callbacks[1].closure = args[2].data;

	    labelString = DwtLatin1String(args[2].buttonName);
	    j = 0;
	    XtSetArg(dargs[j], DwtNcancelLabel, labelString); j++;
	    XtSetArg(dargs[j], DwtNcancelCallback, callbacks);j++;

	    XtSetValues(popup, dargs, j);
	    XtFree(labelString);
	}

    /* Button 4: */

	if (count > 3) {
	    callbacks[1].callback = args[3].handler;
	    callbacks[1].closure = args[3].data;

	    labelString = DwtLatin1String(args[3].buttonName);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtCancelButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultButton, &defaultButton);j++;
	    XtGetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNdefaultPushbutton, DwtYesButton);j++;
	    XtSetValues(popup, bargs, j);
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, &x); j++;
	    XtSetArg(bargs[j], DwtNy, &y); j++;
	    XtSetArg(bargs[j], DwtNwidth, &width); j++;
	    XtGetValues(defaultButton, bargs, j);
	    x = x + width + 10;
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, x); j++;
	    XtSetArg(bargs[j], DwtNy, y); j++;
	    XtSetArg(bargs[j], DwtNlabel, labelString); j++;
	    XtSetArg(bargs[j], DwtNactivateCallback, callbacks);j++;
	    button4 = DwtPushButtonGadgetCreate(popup,args[4].buttonName,
		bargs, j);
	    XtManageChild(button4);
	    XtFree(labelString);
	}

    /* Button 5: */

	if (count > 4) {
	    callbacks[1].callback = args[4].handler;
	    callbacks[1].closure = args[4].data;

	    labelString = DwtLatin1String(args[4].buttonName);
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, &x); j++;
	    XtSetArg(bargs[j], DwtNy, &y); j++;
	    XtSetArg(bargs[j], DwtNwidth, &width); j++;
	    XtGetValues(button4, bargs, j);
	    x = x + width + 10;
	    j = 0;
	    XtSetArg(bargs[j], DwtNx, x); j++;
	    XtSetArg(bargs[j], DwtNy, y); j++;
	    XtSetArg(bargs[j], DwtNlabel, labelString); j++;
	    XtSetArg(bargs[j], DwtNactivateCallback, callbacks);j++;
	    button5 = DwtPushButtonGadgetCreate(popup,args[4].buttonName,
		bargs, j);
	    XtManageChild(button5);
	    XtFree(labelString);
	}
    }
    unbusyCursor();
    XtRealizeWidget(popup);

    return((Widget)popup);
}
#endif /* MOTIF */

#ifdef DECFOCUSPATCH
void
FocusPopUp(popup, data, event)
Widget popup;
caddr_t data;
XEvent *event;
{
    if (event->type == MapNotify) {
	XSetInputFocus(XtDisplay(popup), XtWindow(popup),
		     RevertToPointerRoot,  CurrentTime);
	XtRemoveEventHandler(popup, XtAllEvents, True, FocusPopUp, 0);
    }
    return;
}
#endif

extern Boolean allowErrors;
void
PopUpDialog(popup)
Widget popup;
{

    unbusyCursor();
    if (!allowErrors)
	xthCenterWidgetOverCursor(popup);
    XtManageChild(popup);
    XtPopup(XtParent(popup), XtGrabExclusive);
#ifdef MOTIF
#ifndef MOTIF_V10

    if (XtIsSubclass(popup, xmSelectionBoxWidgetClass)) {
	Widget widget;

	widget = XmSelectionBoxGetChild(popup, XmDIALOG_TEXT);
	if (widget != (Widget) NULL && XtIsManaged(widget)) {
	    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
	    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
	} else {
	    widget = XmSelectionBoxGetChild(popup, XmDIALOG_OK_BUTTON);
	    if (widget != (Widget) NULL && XtIsManaged(widget)) {
		XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
		XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
	    }
	}
    }
    if (XtIsSubclass(popup, xmMessageBoxWidgetClass)) {
	Widget widget;

	widget = XmMessageBoxGetChild(popup, XmDIALOG_OK_BUTTON);
	if (widget != (Widget) NULL && XtIsManaged(widget)) {
	    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
	    XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
	}
    }
#endif
#endif
    return;
}


void
PopDownDialog(dialog)
Widget dialog;
/*
 * pop down the dialog (do not destroy, it will be used again)
 */
{
    XtUnmanageChild(dialog);
    XtUnmapWidget(XtParent(dialog));
    XtPopdown(XtParent(dialog));
    return;
}



char *
GetDialogValue(popup)
Widget popup;
{
#ifdef MOTIF
    return XmTextGetString(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT));
#else
    Arg	args[4];
    Widget w;

    XtSetArg(args[0], DwtNuserData, &w);
    XtGetValues(popup, args, 1);

    return DwtSTextGetString(w);
#endif
}

void
SetDialogValue(popup, text)
Widget popup;
char *text;
{
#ifdef MOTIF
    XmTextSetString(XmSelectionBoxGetChild(popup, XmDIALOG_TEXT), text);
#else
    Arg	args[4];
    Widget w;

    XtSetArg(args[0], DwtNuserData, &w);
    XtGetValues(popup, args, 1);

    DwtSTextSetString(w, text);
#endif
}

/*
 * simple confirmation box
 */

int CBretVal;

/* ARGSUSED */
static void
cbHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    CBretVal = (int) client_data;
    return;
}


static XtCallbackRec ConfirmOkCallback[] = {
	{(XtCallbackProc) cbHandler, (caddr_t) XRN_CB_CONTINUE},
	{NULL, NULL},
};
static XtCallbackRec ConfirmCancelCallback[] = {
	{(XtCallbackProc) cbHandler, (caddr_t) XRN_CB_ABORT},
	{NULL, NULL},
};
static Widget ConfirmBox = NULL;
#ifdef MOTIF
extern Atom XmInternAtom _ARGUMENTS((Display *, String, int));
int
ConfirmationBox(parent, message)
Widget parent;
char *message;
/*
 * pop up a confirmation box and return either 'XRN_CB_ABORT' or 'XRN_CB_CONTINUE'
 */
{
    XEvent ev;
    XtInputMask mask;
    Arg	   args[5];
    XmString	labelString, titleString;
    Atom	WmTakeFocusAtom;

    CBretVal = -1;

    labelString = XmStringLtoRCreate(message, XmSTRING_DEFAULT_CHARSET);
    titleString = XmStringLtoRCreate("mxrn: Confirm", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNmessageString, labelString);
    XtSetArg(args[1], XmNokCallback, ConfirmOkCallback);
    XtSetArg(args[2], XmNcancelCallback, ConfirmCancelCallback);
    XtSetArg(args[3], XmNdefaultButtonType, XmDIALOG_OK_BUTTON);
    XtSetArg(args[4], XmNdialogTitle, titleString);
    if (ConfirmBox == (Widget) NULL) {
        ConfirmBox = (Widget) XmCreateQuestionDialog(parent,"confirmBox",
				    args, XtNumber(args));
	XtUnmanageChild(XmMessageBoxGetChild(ConfirmBox,XmDIALOG_HELP_BUTTON));
	WmTakeFocusAtom = XmInternAtom (XtDisplay(ConfirmBox),
					"WM_TAKE_FOCUS", False);
	XmDeactivateWMProtocol(XtParent(ConfirmBox), WmTakeFocusAtom);
    } else {
	XtSetValues(ConfirmBox, args, XtNumber(args));
    }
    XmStringFree(labelString);
    XmStringFree(titleString);
#else /* not MOTIF */
int
ConfirmationBox(parent, message)
Widget parent;
char *message;
/*
 * pop up a confirmation box and return either 'XRN_CB_ABORT' or 'XRN_CB_CONTINUE'
 */
{
    XEvent  ev;
    XtInputMask mask;
    Arg	    args[7];
    DwtCompString labelString, titleString;

    CBretVal = -1;

    labelString = DwtLatin1String(message);
    titleString = DwtLatin1String("dxrn: Confirm");
    XtSetArg(args[0], DwtNlabel, labelString);
    XtSetArg(args[1], DwtNyesCallback, ConfirmOkCallback);
    XtSetArg(args[2], DwtNnoCallback, ConfirmCancelCallback);
    XtSetArg(args[3], DwtNcancelCallback, ConfirmCancelCallback);
    XtSetArg(args[4], DwtNdefaultPushbutton, DwtYesButton);
    XtSetArg(args[5], DwtNstyle, DwtModeless);
    XtSetArg(args[6], DwtNtitle, titleString);
    if (ConfirmBox == (Widget) NULL) {
	ConfirmBox= (Widget) DwtCautionBoxCreate(parent,"confirmBox",
			args, XtNumber(args));
    } else {
	XtSetValues(ConfirmBox, args, XtNumber(args));
    }
    XtFree(labelString);
    XtFree(titleString);
#endif /* MOTIF */
    PopUpDialog(ConfirmBox);

    for(;;) {
	XtProcessEvent(XtIMAll);
	if (CBretVal != -1) {
	    PopDownDialog(ConfirmBox);
	    while ((mask = XtPending()) != 0) {
		if (mask == XtIMTimer) {
		    XtProcessEvent(XtIMAll);
		} else {
		    XtNextEvent(&ev);
		    XtDispatchEvent(&ev);
		}
	    }
	    return(CBretVal);
	}
    }
}
