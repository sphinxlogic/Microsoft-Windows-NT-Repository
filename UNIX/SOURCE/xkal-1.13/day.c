/*
 *	day.c : The day windows that display a single day's appoints.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: day.c,v 2.2 91/03/13 13:31:09 ferguson Exp $
 */
#include <stdio.h>
#include <ctype.h>
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
#include "app-resources.h"
#include "patchlevel.h"
#ifdef USE_ALERT
#include "alert.h"
#endif

/*
 * Functions defined in this file
 */
DayFormData *createDayFormData(),*createPopupDayFormData();
void setDayFormData();
void checkpointAppoints();
int timetopos();
void postotime();

static void textEventProc(),dismissProc(),selectProc(),unselectProc();
static char *skipLabel();

/*
 * Data defined in this file
 */
DayFormData *currentDayFormData;
int numDisplayedAppoints = -1;		/* total number of text widgets */
int displayStartTime;			/* time of earliest appoint (mins) */
int displayEndTime;			/* time of latest appoint (mins) */
int displayInc;				/* interval between appoints (mins) */
int displayFirstNotePos;		/* index of first notes entry */

static int dayAppointsChanged;		/* fast, local test for changes */
static Msgrec **apps;			/* used for lookupEntry() to set */
static int *appFlags;			/* used in setDayFormData() */

#define CALLBACK_PROC(NAME)     static void NAME(w,client_data,call_data) \
					Widget w; \
					caddr_t client_data,call_data;


/*	-	-	-	-	-	-	-	-	*/
/* Creation routines */
/*
 * createDayFormData() : Creates a new dayForm. The first time this
 *	routine is called, it sets several globals to prevent having
 *	to parse them from the resources every time. malloc()'s tons
 *	of space, for all the variable length stuff we allow.
 */
DayFormData *
createDayFormData(parent)
Widget parent;
{
    DayFormData *d;
    DayTextData *t;
    Arg args[3];
    int i,n;

    if (numDisplayedAppoints == -1) {
	displayStartTime = strtotime(appResources.appsStartTime);
	displayEndTime = strtotime(appResources.appsEndTime);
	displayInc = strtotime(appResources.appsIncrement);
	displayFirstNotePos = (displayEndTime-displayStartTime)/displayInc+1;
	numDisplayedAppoints = displayFirstNotePos+appResources.numNotes;
	apps = (Msgrec **)XtCalloc(numDisplayedAppoints,sizeof(Msgrec *));
	appFlags = (int *)XtCalloc(numDisplayedAppoints,sizeof(int));
    }
    d = XtNew(DayFormData);
    d->form = XtCreateManagedWidget("dayForm",formWidgetClass,parent,
								NULL,ZERO);
    if (appResources.bothShown || !appResources.useTitlebar) {
	d->date = XtCreateManagedWidget("dayLabel",labelWidgetClass,d->form,
								NULL,ZERO);
    } else {
	d->date = NULL;
    }
    d->items = (DayTextData **)XtCalloc(numDisplayedAppoints,
							sizeof(DayTextData));
    i = 0;
    XtSetArg(args[0],XtNfromVert,d->date);
    for (n = displayStartTime; n <= displayEndTime; n += displayInc) {
	t = d->items[i++] = XtNew(DayTextData);
	XtSetArg(args[1],XtNfromHoriz,NULL);
	XtSetArg(args[2],XtNlabel,timetostr(n));
	t->time = XtCreateManagedWidget("appointLabel",labelWidgetClass,
							d->form,args,THREE);
	XtSetArg(args[1],XtNfromHoriz,t->time);
	t->text = XtCreateManagedWidget("appointText",asciiTextWidgetClass,
							d->form,args,TWO);
	XtAddEventHandler(t->text,KeyPressMask|ButtonPressMask,False,
							textEventProc,d);
	XtSetArg(args[0],XtNfromVert,t->time);
    }
    for (n = 0; n < appResources.numNotes; n++) {
	t = d->items[i++] = XtNew(DayTextData);
	XtSetArg(args[1],XtNfromHoriz,NULL);
	XtSetArg(args[2],XtNlabel,appResources.notesLabel);
	t->time = XtCreateManagedWidget("appointLabel",labelWidgetClass,
							d->form,args,THREE);
	XtSetArg(args[1],XtNfromHoriz,t->time);
	t->text = XtCreateManagedWidget("appointText",asciiTextWidgetClass,
							d->form,args,TWO);
	XtAddEventHandler(t->text,KeyPressMask|ButtonPressMask,False,
							textEventProc,d);
	XtSetArg(args[0],XtNfromVert,t->time);
    }
    d->shell = toplevel;	/* may be overridden */
    d->msgText = (String *)XtCalloc(numDisplayedAppoints,sizeof(String));
    d->msg = (Msgrec **)XtCalloc(numDisplayedAppoints,sizeof(Msgrec *));
    d->changed = False;
    d->editing = False;
    return(d);
}

/*
 * createPopupDayFormData() : Creates a popup dayForm by wrapping one of
 *	the above dayForms in a shell and adding some buttons.
 */
DayFormData *
createPopupDayFormData()
{
    Widget shell,form,select,dismiss,unselect;
    DayFormData *d;

    shell = XtCreatePopupShell("popupDayShell",topLevelShellWidgetClass,
							toplevel,NULL,ZERO);
    form = XtCreateManagedWidget("popupDayForm",formWidgetClass,shell,
								NULL,ZERO);
    dismiss = XtCreateManagedWidget("dismissButton",commandWidgetClass,form,
								NULL,ZERO);
    select = XtCreateManagedWidget("selectButton",commandWidgetClass,form,
								NULL,ZERO);
    unselect = XtCreateManagedWidget("unselectButton",commandWidgetClass,form,
								NULL,ZERO);
    d = createDayFormData(form);
    d->shell = shell;		/* override default */
    XtAddCallback(select,"callback",selectProc,d);
    XtAddCallback(dismiss,"callback",dismissProc,d);
    XtAddCallback(unselect,"callback",unselectProc,d);
    XtPopup(shell,XtGrabNone);
    return(d);
}

/*	-	-	-	-	-	-	-	-	*/
/* Event handlers */

/*ARGSUSED*/
/*
 * textEventProc() : Called whenever the user types in any slot. This
 *	provides a quick check as to whether the appoints have changed.
 *	Note that this does NOT detect, eg., selection pastes, as documented
 *	in the BUGS section of the man page.
 */
static void
textEventProc(w,client_data,event,continue_flag)
Widget w;
XtPointer client_data;
XEvent *event;
Boolean *continue_flag;
{
    DayFormData *d = (DayFormData *)client_data;

    d->changed = True;
}

/*
 * dismissProc() : Callback for dismiss button - Destroy a dayForm unless
 *	one of it's appoints is being edited.
 */
/*ARGSUSED*/
CALLBACK_PROC(dismissProc)
{
    DayFormData *d = (DayFormData *)client_data;
    int i;

    if (d->editing) {
	XBell(display,0);
#ifdef USE_ALERT
	alert("Error: Can't dimiss day while editing it.");
#else
	fprintf(stderr,"\007%s: can't dimiss day while editing it\n",program);
#endif
	return;
    }
    checkpointAppoints(d);
    if (currentDayFormData == d)
	currentDayFormData = NULL;
    XtPopdown(d->shell);
    XtDestroyWidget(d->shell);
    for (i=0; i < numDisplayedAppoints; i++)
	XtFree(d->msgText[i]);
    XtFree(d->msgText);
}

/*
 * selectProc() : Callback for select button - Make this dayForm the
 *	current one, resensitizing its widgets.
 */
/*ARGSUSED*/
CALLBACK_PROC(selectProc)
{
    DayFormData *d = (DayFormData *)client_data;

    checkpointAppoints(d);
    if (currentDayFormData == d)
	return;
    if (currentDayFormData != NULL)
	XtSetSensitive(currentDayFormData->form,False);
    XtSetSensitive(d->form,True);
    currentDayFormData = d;
}

/*
 * unselectProc() : Callback for unselect button - Desensitize this dayForm's
 *	widgets, and make there be no current dayForm.
 */
/*ARGSUSED*/
CALLBACK_PROC(unselectProc)
{
    DayFormData *d = (DayFormData *)client_data;

    checkpointAppoints(d);
    XtSetSensitive(d->form,False);
    currentDayFormData = NULL;
}

/*	-	-	-	-	-	-	-	-	*/
/* Set/reset routines */

/*
 * clearDayFormData() : Clear all the text widgets (slots) and free the
 *	memory used to hold their (original) contents.
 */
void
clearDayFormData(d)
DayFormData *d;
{
    Arg args[1];
    int i;

    XtSetArg(args[0],XtNlabel,"");
    if (d->date != NULL)
	XtSetValues(d->date,args,ONE);
    for (i=0; i < numDisplayedAppoints; i++) {
	XtSetArg(args[0],XtNstring,"");
	XtSetValues(d->items[i]->text,args,ONE);
	XtFree(d->msgText[i]);
	d->msgText[i] = NULL;
    }
}

/*
 * setDayFormData() : Sets all the text widgets and the label and/or
 *	titlebar. Saves these texts in malloc()'d space so we can test
 *	later and see if they've changed. Looks after relabelling items
 *	that had to be moved due to collisions.
 */
void
setDayFormData(d,day,month,year)
DayFormData *d;
int day,month,year;
{
    Arg args[2];
    char text[32];
    int dow,i,j,n,pos;
    Boolean exact;

    /*
     * set the label at the top of the day
     */
    dow = computeDOW(day,month,year);
    if (appResources.bothShown || !appResources.useTitlebar) {
	sprintf(text,"%s %d %s %d",shortDowStr[dow-1],day,
						shortMonthStr[month-1],year);
	XtSetArg(args[0],XtNlabel,text);
	XtSetValues(d->date,args,ONE);
    }
    if (appResources.useTitlebar) {
	sprintf(text,"Xkal %.2f - %s %d %s %d",XKAL_VERSION,shortDowStr[dow-1],
					day,shortMonthStr[month-1],year);
	XStoreName(display,XtWindow(d->shell),text);
    }
    /*
     * clear the saved text strings
     */
    for (i=0; i < numDisplayedAppoints; i++) {
	XtFree(d->msgText[i]);
	d->msgText[i] = NULL;
	d->msg[i] = NULL;
    }
    /*
     * get the appoints for today
     */
    n = lookupEntry(day,month,year,-1,-1,numDisplayedAppoints,apps,False);
    if (n < 0) {
#ifdef USE_ALERT
	alert("Error: too many entries (>%d) for %s %d %s %d.",
					numDisplayedAppoints,
					shortDowStr[dow-1],day,
					shortMonthStr[month-1],year);
#else
	fprintf(stderr,"\007%s: too many entries (>%d) for %s %d %s %d\n",
					program,numDisplayedAppoints,
					shortDowStr[dow-1],day,
					shortMonthStr[month-1],year);
#endif
	n = numDisplayedAppoints;
    }
    for (i=0; i < n; i++)
	appFlags[i] = False;
    /*
     * set the msgText[]'s for any appoints without conflicts
     */
    for (i=0; i < n; i++) {
	pos = timetopos(apps[i]->hour,apps[i]->mins,&exact);
	/*
	 * skip appoints that need to be labelled no matter what
	 */
	if (!exact)
	    continue;
	/*
	 * no conflict -> save string and set flag to say it's done
	 */
	if (d->msgText[pos] == NULL) {
	    d->msgText[pos] = XtNewString(apps[i]->text);
	    d->msg[pos] = apps[i];
	    appFlags[i] = True;
	}
    }
    /*
     * move appoints that had conflicts
     */
    for (i=0; i < n; i++) {
	if (!appFlags[i]) {
	    pos = timetopos(apps[i]->hour,apps[i]->mins,&exact);
	    /*
	     * pos can actually be open if we skipped it above
	     */
	    if (d->msgText[pos] == NULL) {
		j = pos;
	    } else {
		/*
	 	 * scan up for empty slot
	 	 */
	 	for (j=pos+1; j < numDisplayedAppoints; j++)
		    if (d->msgText[j] == NULL)
			break;
		/*
		 * scan down if still not placed
	 	 */
		if (j == numDisplayedAppoints) {
		    for (j=pos-1; j >= 0; j--)
			if (d->msgText[j] == NULL)
			    break;
		}
	    }
	    /*
	     * now figure out how to label it and save the new string
	     */
	    if (!appResources.rearrangeSilently || !exact) {
	        if (apps[i]->hour == -1) {
		    if (j < displayFirstNotePos) {
			/*
			 * was note, now not in notes -> label
			 */
			d->msgText[j] = XtMalloc(strlen(apps[i]->text)+
					strlen(appResources.notesLabel)+1);
			sprintf(d->msgText[j],"[%s] %s",appResources.notesLabel,
								apps[i]->text);
		    } else {
			/*
			 * was note, still in notes -> no label
			 */
			d->msgText[j] = XtNewString(apps[i]->text);
		    }
		} else {
		    /*
		     * non-note -> label
		     */
		    d->msgText[j] = XtMalloc(strlen(apps[i]->text)+9);
		    sprintf(d->msgText[j],"[%s] %s",
				timetostr(apps[i]->hour*60+apps[i]->mins),
				apps[i]->text);
		}
	    } else {
		/*
		 * user requested no labels
		 */
		d->msgText[j] = XtNewString(apps[i]->text);
	    }
	    d->msg[j] = apps[i];
	}
    }
    /*
     * set/clear the actual widgets
     */
    for (i=0; i < numDisplayedAppoints; i++) {
	if (d->msgText[i])
	    XtSetArg(args[0],XtNstring,d->msgText[i]);
	else
	    XtSetArg(args[0],XtNstring,"");
	if (d->msg[i] && d->msg[i]->system)
	    XtSetArg(args[1],XtNeditType,XawtextRead);
	else
	    XtSetArg(args[1],XtNeditType,XawtextEdit);
	XtSetValues(d->items[i]->text,args,TWO);
    }
    d->day = day;
    d->month = month;
    d->year = year;
}

/*
 * checkpointAppoints() : Transfers the appoints from the text widgets
 *	to the DB, adding or deleting entries as required. Looks after
 *	parsing back the labelling that was possibly added above.
 */
void
checkpointAppoints(d)
DayFormData *d;
{
    Arg args[2];
    int i,type,h,m,n;
    char *new,c;

    if (!d->changed)
	return;
    XtSetArg(args[0],XtNeditType,&type);
    XtSetArg(args[1],XtNstring,&new);
    for (i=0; i < numDisplayedAppoints; i++) {
	XtGetValues(d->items[i]->text,args,TWO);
	if (type == XawtextRead)
	    continue;
	if (d->msgText[i] && strcmp(d->msgText[i],new) != 0) {
	    deleteEntry(0,d->year,d->month,d->day,d->msg[i]->hour,
					d->msg[i]->mins,d->msg[i]->text);
	    d->msg[i] = NULL;
	    appointsChanged = True;
	}
	if (*new != '\0' && (!d->msgText[i] ||
				strcmp(d->msgText[i],new) != 0)) {
	    if (*new == '[') {
		n = strtotime(new+1);
		if (n == -1) {
		    h = m = -1;		/* and leave trash in text */
		} else {
		    h = n / 60;
		    m = n % 60;
		    new = skipLabel(new);
		}
	    } else {
		postotime(i,&h,&m);
	    }
	    d->msg[i] = addEntry(0,d->year,d->month,d->day,h,m,new,False,
						appResources.defaultLevel);
	    XtFree(d->msgText[i]);
	    d->msgText[i] = XtNewString(new);
	    appointsChanged = True;
	}
    }
    d->changed = False;
    if (currentDayFormData != NULL)
	shadeButton(currentDayFormData->buttonData,(GC)NULL,0,0);
}

/*
 * skipLabels() : Skip over "[...]" in a string.
 */
static char *
skipLabel(s)
char *s;
{
    if (*s != '[')
	return(s);
    while (*s != ']')
	s += 1;
    s += 1;
    while (*s && isspace(*s))
	s += 1;
    return(s);
}

/*	-	-	-	-	-	-	-	-	*/
/* Various conversion routines (see also util.c) */

/*
 * timetopos(h,m,exactp) : Returns the appropriate place in the DayForm for
 *	the given time. *exactp is set to True if this is exact, False if it
 *	has been rounded (and hence if the appoint should be labelled).
 */
int
timetopos(h,m,exactp)
int h,m;
Boolean *exactp;
{
    int t;

    if (h == -1) {
	*exactp = True;
	return(displayFirstNotePos);
    }
    if (m != -1)
	t = h * 60 + m;
    else
	t = h * 60;
    if (t < displayStartTime || t > displayEndTime) {
	*exactp = False;
	return(displayFirstNotePos);
    } else {
	*exactp = (t % displayInc == 0);
	return((t-displayStartTime)/displayInc);
    }
}

/*
 * postotime(p,h,m) : Sets *h and *m to a time corresponding to the position
 *	given by p.
 */
void
postotime(p,h,m)
int p;
int *h,*m;
{
    int t;

    if (p >= displayFirstNotePos) {
	*h = *m = -1;
    } else {
	t = displayStartTime + p * displayInc;
	*h = t / 60;
	*m = t % 60;
    }
}

