/*
 *	edit-defaults.c : Modify default parameters
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Feb 1991.
 *
 *	$Id: edit-defaults.c,v 2.1 91/02/28 11:21:17 ferguson Exp $
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Cardinals.h>
#include "xkal.h"
#include "month.h"
#include "day.h"
#include "app-resources.h"
#ifdef USE_ALERT
#include "alert.h"
#endif

/*
 * Functions declared in this file
 */
void editDefaults();
void focusNextDefaultsItem(),focusPrevDefaultsItem();

static void initDefaultsShell(),setDefaultsItems(),setText();
static void applyProc(),revertProc(),dismissProc();

/*
 * Data declared in this file
 */
static Widget defaultsShell,defaultsForm;
static Widget applyButton,revertButton,dismissButton;
static Widget pAppsLabel,pAppsText,bakExtLabel,bakExtText,dateLabel,dateText;
static Widget numMonLabel,numMonText;
static Widget chkIntLabel,chkIntText,reaSilLabel,reaSilText;
static Widget defLevLabel,defLevText,levDelLabel,levDelText;

#define ACTION_PROC(NAME)	void NAME(w,event,params,num_params) \
					Widget w; \
					XEvent *event; \
					String *params; \
					Cardinal *num_params;

#define CALLBACK_PROC(NAME)	static void NAME(w,client_data,call_data) \
					Widget w; \
					caddr_t client_data,call_data;

/*	-	-	-	-	-	-	-	-	*/
/* External interface (action) procedure */
/*
 * editDefaults() : Pops up (possibly creating) the defaults editor,
 *	and fills it with the information from the current values of
 *	the application defaults.
 */
/*ARGSUSED*/
ACTION_PROC(editDefaults)
{
    if (defaultsShell == NULL)
	initDefaultsShell();
    setDefaultsItems();
    XtPopup(defaultsShell,XtGrabNone);
}

/*	-	-	-	-	-	-	-	-	*/
/* Initialization procedures */
/*
 * initDefaultsShell() : Create the popup defaults editor.
 */
static void
initDefaultsShell()
{
    defaultsShell = XtCreatePopupShell("defaultsShell",topLevelShellWidgetClass,
							toplevel,NULL,ZERO);
    defaultsForm = XtCreateManagedWidget("defaultsForm",formWidgetClass,
						defaultsShell,NULL,ZERO);
    applyButton = XtCreateManagedWidget("applyButton",commandWidgetClass,
							defaultsForm,NULL,ZERO);
    XtAddCallback(applyButton,"callback",applyProc,NULL);
    revertButton = XtCreateManagedWidget("revertButton",commandWidgetClass,
							defaultsForm,NULL,ZERO);
    XtAddCallback(revertButton,"callback",revertProc,NULL);
    dismissButton = XtCreateManagedWidget("dismissButton",commandWidgetClass,
							defaultsForm,NULL,ZERO);
    XtAddCallback(dismissButton,"callback",dismissProc,NULL);

    pAppsLabel = XtCreateManagedWidget("personalAppointsLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    pAppsText = XtCreateManagedWidget("personalAppointsText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    bakExtLabel = XtCreateManagedWidget("backupExtensionLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    bakExtText = XtCreateManagedWidget("backupExtensionText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    dateLabel = XtCreateManagedWidget("dateLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    dateText = XtCreateManagedWidget("dateText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    numMonLabel = XtCreateManagedWidget("numMonthsLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    numMonText = XtCreateManagedWidget("numMonthsText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    chkIntLabel = XtCreateManagedWidget("checkpointIntervalLabel",
				labelWidgetClass,defaultsForm,NULL,ZERO);
    chkIntText = XtCreateManagedWidget("checkpointIntervalText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    reaSilLabel = XtCreateManagedWidget("rearrangeSilentlyLabel",
				labelWidgetClass,defaultsForm,NULL,ZERO);
    reaSilText = XtCreateManagedWidget("rearrangeSilentlyText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    defLevLabel = XtCreateManagedWidget("defaultLevelLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    defLevText = XtCreateManagedWidget("defaultLevelText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
    levDelLabel = XtCreateManagedWidget("levelDelimLabel",labelWidgetClass,
							defaultsForm,NULL,ZERO);
    levDelText = XtCreateManagedWidget("levelDelimText",
				asciiTextWidgetClass,defaultsForm,NULL,ZERO);
}

/*
 * setDefaultsItems() : Sets the values in the defaults editor from the
 *	current state of the application resources.
 */
static void
setDefaultsItems()
{
    char buf[8];

    setText(pAppsText,appResources.personalAppoints);
    setText(bakExtText,appResources.backupExtension);
    setText(dateText,appResources.date);
    sprintf(buf,"%d",appResources.numMonths);
    setText(numMonText,buf);
    sprintf(buf,"%d",appResources.checkpointInterval);
    setText(chkIntText,buf);
    setText(reaSilText,appResources.rearrangeSilently ? "True" : "False");
    sprintf(buf,"%d",appResources.defaultLevel);
    setText(defLevText,buf);
    setText(levDelText,appResources.levelDelim);
}

/*
 * setText() : Set the value of a text item.
 */
static void
setText(item,text)
Widget item;
char *text;
{
    Arg args[1];

    XtSetArg(args[0],XtNstring,text);
    XtSetValues(item,args,ONE);
}

/*	-	-	-	-	-	-	-	-	*/
/* Callback procedures */
/*
 * applyProc() : Callback for apply button - Set the application resources
 *	from the items on the defaults editor panel. Some of these require
 *	special action when changed, and this routine does that.
 */
/*ARGSUSED*/
CALLBACK_PROC(applyProc)
{
    Arg args[1];
    char *s;
    int n;

    XtSetArg(args[0],XtNstring,&(appResources.personalAppoints));
    XtGetValues(pAppsText,args,ONE);
    XtSetArg(args[0],XtNstring,&(appResources.backupExtension));
    XtGetValues(bakExtText,args,ONE);
    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(dateText,args,ONE);
    if (strcmp(s,appResources.date) != 0) {
	appResources.date = XtNewString(s);		/* leak! */
	parseDate(appResources.date,&currentDay,&currentMon,&currentYear);
    }
    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(numMonText,args,ONE);
    if (strcmp(s,"1") != 0 && strcmp(s,"2") != 0 &&
				strcmp(s,"3") != 0 && strcmp(s,"12") != 0) {
#ifdef USE_ALERT
	alert("Error: numMonths must be 1, 2, 3, or 12.");
#else
	fprintf(stderr,"\007%s: numMonths must be 1, 2, 3, or 12\n",program);
#endif
    } else if (n != appResources.numMonths) {
	n = ONE;
	setNumMonths((Widget)NULL,(XEvent *)NULL,&s,&n);
    }
    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(chkIntText,args,ONE);
    n = atoi(s);
    if (n != appResources.checkpointInterval) {
	appResources.checkpointInterval = n;
	XtRemoveTimeOut(timeoutId);
	if (n > 0)
	    timeoutId = XtAppAddTimeOut(app_con,(unsigned long)(n*60000),
							    timeoutProc,NULL);
    }
    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(reaSilText,args,ONE);
    n  = (strcasecmp(s,"True") == 0 || strcmp(s,"1") == 0);
    if (n != appResources.rearrangeSilently) {
	appResources.rearrangeSilently = n;
	if (currentDayFormData != NULL) {
	    checkpointAppoints(currentDayFormData);
	    setDayFormData(currentDayFormData);
	}
    }
    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(defLevText,args,ONE);
    appResources.defaultLevel = atoi(s);
    XtSetArg(args[0],XtNstring,&(appResources.levelDelim));
    XtGetValues(levDelText,args,ONE);
}

/*
 * revertProc() : Callback for revert button - Reset the text strings to
 *      the way they were when editDefaults was called last. Does NOT undo
 *      any changed that were made.
 */
/*ARGSUSED*/
CALLBACK_PROC(revertProc)
{
    setDefaultsItems();
}

/*
 * dismissProc() : Callback for dismiss button - Pop down the editor.
 */
/*ARGSUSED*/
CALLBACK_PROC(dismissProc)
{
    XtPopdown(defaultsShell);
}

/*	-	-	-	-	-	-	-	-	*/
/* Action procedures */
/*
 * focusNextDefaultstem() : Move input focus to next defaults editor item,
 *	wrapping at bottom.
 */
/*ARGSUSED*/
ACTION_PROC(focusNextDefaultsItem)
{
    if (w == pAppsText)
	w = bakExtText;
    else if (w == bakExtText)
	w = dateText;
    else if (w == dateText)
	w = numMonText;
    else if (w == numMonText)
	w = chkIntText;
    else if (w == chkIntText)
	w = reaSilText;
    else if (w == reaSilText)
	w = defLevText;
    else if (w == defLevText)
	w = levDelText;
    else if (w == levDelText)
	w = pAppsText;
    else
	return;
    XSetInputFocus(display,XtWindow(w),RevertToParent,CurrentTime);
}

/*
 * focusNextDefaultsItem() : Move input focus to previous defaults editor item,
 *	wrapping at top.
 */
/*ARGSUSED*/
ACTION_PROC(focusPrevDefaultsItem)
{
    if (w == pAppsText)
	w = levDelText;
    else if (w == bakExtText)
	w = pAppsText;
    else if (w == dateText)
	w = bakExtText;
    else if (w == numMonText)
	w = dateText;
    else if (w == chkIntText)
	w = numMonText;
    else if (w == reaSilText)
	w = chkIntText;
    else if (w == defLevText)
	w = reaSilText;
    else if (w == levDelText)
	w = defLevText;
    else
	return;
    XSetInputFocus(display,XtWindow(w),RevertToParent,CurrentTime);
}
