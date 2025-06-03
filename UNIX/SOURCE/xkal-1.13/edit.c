/*
 *	edit.c : Edit an appointment in detail
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Feb 1991.
 *
 *	$Id: edit.c,v 2.1 91/02/28 11:21:20 ferguson Exp $
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
#include "db.h"
#include "util.h"
#include "date-strings.h"
#include "app-resources.h"			/* for notesLabel */
#ifdef USE_ALERT
#include "alert.h"
#endif

/*
 * Functions declared in this file
 */
void editAppoint();
void focusNextEditItem(),focusPrevEditItem();

static void initEditShell(),setEditItems(),setText();
static void applyProc(),revertProc(),deleteProc(),dismissProc();
static Boolean getCurrentEditMsg();

/*
 * Data declared in this file
 */
static Widget editShell,editForm;
static Widget applyButton,revertButton,deleteButton,dismissButton;
static Widget dowLabel,dowText,dayLabel,dayText,monLabel,monText;
static Widget yearLabel,yearText,timeLabel,timeText;
static Widget textLabel,textText,levelLabel,levelText;
static DayFormData *currentEditFormData;
static Msgrec currentEditMsg;

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
 * editAppoint() : Pops up (possibly creating) the appointment editor,
 *	and fills it with the information from the appointment corresponding
 *	to the text widget taht this action is invoked from.
 */
/*ARGSUSED*/
ACTION_PROC(editAppoint)
{
    Arg args[1];
    int i,type;

    if (currentDayFormData == NULL)
	return;
    for (i=0; i < numDisplayedAppoints; i++)
	if (w == currentDayFormData->items[i]->time ||
	    w == currentDayFormData->items[i]->text)
	    break;
    if (i == numDisplayedAppoints)
	return;
    if (w == currentDayFormData->items[i]->time)
	w = currentDayFormData->items[i]->text;
    XtSetArg(args[0],XtNeditType,&type);
    XtGetValues(w,args,ONE);
    if (type != XawtextEdit) {
	XBell(display,0);
	return;
    }
    if (editShell == NULL)
	initEditShell();
    checkpointAppoints(currentDayFormData);
    currentEditFormData = currentDayFormData;
    currentEditFormData->editing = True;
    if (currentEditFormData->msg[i] == NULL) {
	currentEditMsg.dow = 0;
	currentEditMsg.day = currentEditFormData->day;
	currentEditMsg.month = currentEditFormData->month;
	currentEditMsg.year = currentEditFormData->year;
	postotime(i,&(currentEditMsg.hour),&(currentEditMsg.mins));
	currentEditMsg.text = "";
	currentEditMsg.level = appResources.defaultLevel;
    } else {
	currentEditMsg.dow = currentEditFormData->msg[i]->dow;
	currentEditMsg.day = currentEditFormData->msg[i]->day;
	currentEditMsg.month = currentEditFormData->msg[i]->month;
	currentEditMsg.year = currentEditFormData->msg[i]->year;
	currentEditMsg.hour = currentEditFormData->msg[i]->hour;
	currentEditMsg.mins = currentEditFormData->msg[i]->mins;
	currentEditMsg.text = XtNewString(currentEditFormData->msg[i]->text);
	currentEditMsg.level = currentEditFormData->msg[i]->level;
    }
    setEditItems(&currentEditMsg);
    XtPopup(editShell,XtGrabNone);
}

/*	-	-	-	-	-	-	-	-	*/
/* Initialization procedures */

/*
 * initEditShell() : Create the popup appointment editor.
 */
static void
initEditShell()
{
    editShell = XtCreatePopupShell("editShell",topLevelShellWidgetClass,
							toplevel,NULL,ZERO);
    editForm = XtCreateManagedWidget("editForm",formWidgetClass,
							editShell,NULL,ZERO);
    applyButton = XtCreateManagedWidget("applyButton",commandWidgetClass,
							editForm,NULL,ZERO);
    XtAddCallback(applyButton,"callback",applyProc,NULL);
    revertButton = XtCreateManagedWidget("revertButton",commandWidgetClass,
							editForm,NULL,ZERO);
    XtAddCallback(revertButton,"callback",revertProc,NULL);
    deleteButton = XtCreateManagedWidget("deleteButton",commandWidgetClass,
							editForm,NULL,ZERO);
    XtAddCallback(deleteButton,"callback",deleteProc,NULL);
    dismissButton = XtCreateManagedWidget("dismissButton",
					commandWidgetClass,editForm,NULL,ZERO);
    XtAddCallback(dismissButton,"callback",dismissProc,NULL);
    dowLabel = XtCreateManagedWidget("dowLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    dowText = XtCreateManagedWidget("dowText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    dayLabel = XtCreateManagedWidget("dayLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    dayText = XtCreateManagedWidget("dayText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    monLabel = XtCreateManagedWidget("monLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    monText = XtCreateManagedWidget("monText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    yearLabel = XtCreateManagedWidget("yearLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    yearText = XtCreateManagedWidget("yearText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    timeLabel = XtCreateManagedWidget("timeLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    timeText = XtCreateManagedWidget("timeText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    textLabel = XtCreateManagedWidget("textLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    textText = XtCreateManagedWidget("textText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    levelLabel = XtCreateManagedWidget("levelLabel",labelWidgetClass,
							editForm,NULL,ZERO);
    levelText = XtCreateManagedWidget("levelText",asciiTextWidgetClass,
							editForm,NULL,ZERO);
    XtRealizeWidget(editShell);		/* so we can set titlebar */
}

/*
 * setEditItems() : Sets the values in the appointment editor text items
 *	to values ffrom the given appoint.
 */
static void
setEditItems(msg)
Msgrec *msg;
{
    char buf[8];

    if (msg->dow == 0)
	setText(dowText,"");
    else
	setText(dowText,shortDowStr[msg->dow-1]);
    if (msg->day == 0)
	setText(dayText,"");
    else {
	sprintf(buf,"%d",msg->day);
	setText(dayText,buf);
    }
    if (msg->month == 0)
	setText(monText,"");
    else
	setText(monText,shortMonthStr[msg->month-1]);
    if (msg->year == 0)
	setText(yearText,"");
    else {
	sprintf(buf,"%d",msg->year);
	setText(yearText,buf);
    }
    if (msg->hour == -1)
	setText(timeText,appResources.notesLabel);
    else
	setText(timeText,timetostr(msg->hour*60+msg->mins));
    setText(textText,msg->text);
    sprintf(buf,"%d",msg->level);
    setText(levelText,buf);
}

/*
 * setText() : Set the given text item's value to the given string.
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
 * applyProc() : Callback for apply button - Add an entry with the fields
 *	given in the editor panel to the DB. Removes any other that matches
 *	exactly (including the text). Resets the month display to reflect
 *	the change (and the day within the month).
 */
/*ARGSUSED*/
CALLBACK_PROC(applyProc)
{
    Msgrec msg;

    if (!getCurrentEditMsg(&msg))
	return;
    deleteEntry(msg.dow,msg.year,msg.month,msg.day,msg.hour,msg.mins,msg.text);
    addEntry(msg.dow,msg.year,msg.month,msg.day,msg.hour,msg.mins,msg.text,
							    False,msg.level);
    appointsChanged = True;
    setDayFormData(currentEditFormData,currentEditFormData->day,
					currentEditFormData->month,
					currentEditFormData->year);
    shadeButton(currentEditFormData->buttonData,(GC)NULL,0,0);
}

/*
 * revertProc() : Callback for revert button - Reset the text strings to
 *	the way they were when editAppoint was called last. Does NOT undo
 *	any changed that were made.
 */
/*ARGSUSED*/
CALLBACK_PROC(revertProc)
{
    setEditItems(&currentEditMsg);
}

/*
 * deleteProc() : Callback for delete button - Removes any appoint that
 *	exactly matches the items on the editor panel. Updates the month
 *	display to reflect the change.
 */
/*ARGSUSED*/
CALLBACK_PROC(deleteProc)
{
    Msgrec msg;

    checkpointAppoints(currentEditFormData);
    if (!getCurrentEditMsg(&msg))
	return;
    if (deleteEntry(msg.dow,msg.year,msg.month,msg.day,msg.hour,msg.mins,
								msg.text)) {
	appointsChanged = True;
	setDayFormData(currentEditFormData,currentEditFormData->day,
					currentEditFormData->month,
					currentEditFormData->year);
	shadeButton(currentEditFormData->buttonData,(GC)NULL,0,0);
    }
}

/*
 * dismissProc() : Callback for dismiss button - Pop down the editor.
 */
/*ARGSUSED*/
CALLBACK_PROC(dismissProc)
{
    currentEditFormData->editing = False;
    XtFree(currentEditMsg.text);
    XtPopdown(editShell);
}

/*	-	-	-	-	-	-	-	-	*/
/* Action procedures */
/*
 * focusNextEditItem() : Move input focus to next editor item, wrapping
 *	at bottom.
 */
/*ARGSUSED*/
ACTION_PROC(focusNextEditItem)
{
    if (w == dowText)
	w = dayText;
    else if (w == dayText)
	w = monText;
    else if (w == monText)
	w = yearText;
    else if (w == yearText)
	w = timeText;
    else if (w == timeText)
	w = textText;
    else if (w == textText)
	w = levelText;
    else if (w == levelText)
	w = dowText;
    else
	return;
    XSetInputFocus(display,XtWindow(w),RevertToParent,CurrentTime);
}

/*
 * focusPrevEditItem() : Move input focus to previous editor item, wrapping
 *	at top.
 */
/*ARGSUSED*/
ACTION_PROC(focusPrevEditItem)
{
    if (w == dowText)
	w = levelText;
    else if (w == dayText)
	w = dowText;
    else if (w == monText)
	w = dayText;
    else if (w == yearText)
	w = monText;
    else if (w == timeText)
	w = yearText;
    else if (w == textText)
	w = timeText;
    else if (w == levelText)
	w = textText;
    else
	return;
    XSetInputFocus(display,XtWindow(w),RevertToParent,CurrentTime);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * getCurrentEditMsg() : Fills in the given Msgrec with values taken from
 *	the editor text items.
 */
static Boolean
getCurrentEditMsg(msg)
Msgrec *msg;
{
    Arg args[1];
    char *s;
    int i,t;

    XtSetArg(args[0],XtNstring,&s);
    XtGetValues(dowText,args,ONE);
    if (!*s)
	msg->dow = 0;
    else {
	for (i=0; i < 7; i++)
	    if (strcasecmp(s,longDowStr[i]) == 0 ||
		strcasecmp(s,shortDowStr[i]) == 0)
		break;
	if (i == 7) {
#ifdef USE_ALERT
	    alert("Error: bad dow string: \"%s\".",s);
#else
	    fprintf(stderr,"\007%s: bad dow string: \"%s\"\n",program,s);
#endif
	    return(False);
	} else
	    msg->dow = i+1;
    }
    XtGetValues(dayText,args,ONE);
    msg->day = atoi(s);			/* no checking! */
    XtGetValues(monText,args,ONE);
    if (!*s)
	msg->month = 0;
    else {
	for (i=0; i < 7; i++)
	    if (strcasecmp(s,longMonthStr[i]) == 0 ||
		strcasecmp(s,shortMonthStr[i]) == 0)
		break;
	if (i == 7) {
#ifdef USE_ALERT
	    alert("Error: bad month string: \"%s\".",s);
#else
	    fprintf(stderr,"\007%s: bad month string: \"%s\"\n",program,s);
#endif
	    return(False);
	} else
	    msg->month = i+1;
    }
    XtGetValues(yearText,args,ONE);
    msg->year = atoi(s);			/* no checking! */
    XtGetValues(timeText,args,ONE);
    if (!*s || strcasecmp(s,appResources.notesLabel) == 0)
	msg->hour = msg->mins = -1;
    else {
	t = strtotime(s);
	msg->hour = t / 60;
	msg->mins = t % 60;
    }
    XtGetValues(textText,args,ONE);
    msg->text = s;
    XtGetValues(levelText,args,ONE);
    msg->level = atoi(s);			/* no checking! */
    return(True);
}
