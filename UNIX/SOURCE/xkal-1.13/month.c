/*
 *	month.c : Widget creation and action binding for the month
 *		  forms, and their sub-widgets.
 *
 *      George Ferguson, ferguson@cs.rochester.edu, 27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: month.c,v 2.1 91/02/28 11:21:22 ferguson Exp $
 */
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Cardinals.h>
#include "xkal.h"
#include "month.h"
#include "day.h"
#include "db.h"
#include "app-resources.h"
#include "date-strings.h"

/*
 * Functions defined in this file
 */
MonthFormData *createMonthFormData();
void setMonthFormData();
void selectDay();
void shadeButton();

static void dayButtonCallbackProc();
static void getGCAndXY();

/*
 * Data defined in this file
 */
static Widget radioGroup;

#define CALLBACK_PROC(NAME)     static void NAME(w,client_data,call_data) \
					Widget w; \
					caddr_t client_data,call_data;


/*	-	-	-	-	-	-	-	-	*/
/*
 * createMonthFormData() : Returns a monthFormData whose Form is unmanaged.
 */
MonthFormData *
createMonthFormData(parent,name,num)
Widget parent;
char *name;
int num;		/* 1,2,3,12 */
{
    MonthFormData *m;
    DayButtonData *d;
    Widget widget;
    Arg args[3];
    char text[16];
    int row,col,index;
    int w,h;

    m = XtNew(MonthFormData);
    m->form = XtCreateWidget(name,formWidgetClass,parent,NULL,ZERO);
    switch (num) {
	case 1: w = appResources.dateWidth1;
		h = appResources.dateHeight1;
		break;
	case 2: w = appResources.dateWidth2;
		h = appResources.dateHeight2;
		break;
	case 3: w = appResources.dateWidth3;
		h = appResources.dateHeight3;
		break;
	case 12: w = appResources.dateWidth12;
		 h = appResources.dateHeight12;
		 break;
    }
    sprintf(text,"monthLabel%d",num);
    m->label = XtCreateManagedWidget(text,labelWidgetClass,m->form,NULL,ZERO);
    if (appResources.dowLabels) {
	sprintf(text,"dowLabel%d",num);
	XtSetArg(args[0],XtNfromVert,m->label);
	XtSetArg(args[1],XtNfromHoriz,NULL);
	index = appResources.dowOffset;
	for (col=0; col < 7; col++) {
	    XtSetArg(args[2],XtNlabel,shortDowStr[index]);
	    widget = XtCreateManagedWidget(text,labelWidgetClass,m->form,
								args,THREE);
	    index = (index == 6) ? 0 : index+1;
	    XtSetArg(args[1],XtNfromHoriz,widget);
	}
	XtSetArg(args[0],XtNfromVert,widget);
    } else {
	XtSetArg(args[0],XtNfromVert,m->label);
    }
    sprintf(text,"dayButton%d",num);
    for (row=0; row < 6; row++) {
	XtSetArg(args[1],XtNfromHoriz,NULL);
	for (col=0; col < 7; col++) {
	    d = m->days[row*7+col] = XtNew(DayButtonData);
	    d->button = XtCreateManagedWidget(text,toggleWidgetClass,
							m->form,args,TWO);
	    XtAddCallback(d->button,"callback",dayButtonCallbackProc,d);
	    XtSetArg(args[2],XtNradioData,d->button);
	    XtSetValues(d->button,args+2,ONE);
	    if (radioGroup == NULL)
		radioGroup = d->button;
	    XawToggleChangeRadioGroup(d->button,radioGroup);
	    d->pixmap = XCreatePixmap(display,root,w,h,
						DefaultDepthOfScreen(screen));

	    XtSetArg(args[1],XtNfromHoriz,d->button);
	}
        XtSetArg(args[0],XtNfromVert,d->button);
    }
    return(m);
}

/*
 * setMonthFormData() : Draw the individual days in the month, including
 *	the date and shading for criticality.
 */
void
setMonthFormData(m,num,month,year)
MonthFormData *m;
int num,month,year;
{
    DayButtonData *d;
    Arg args[1];
    GC gc;
    char text[16];
    int first,numDays;
    int i,x,y;

    getGCAndXY(num,&gc,&x,&y);
    XawFormDoLayout(m->form,False);
    XawToggleUnsetCurrent(radioGroup);
    sprintf(text,"%s %d",longMonthStr[month-1],year);
    XtSetArg(args[0],XtNlabel,text);
    XtSetValues(m->label,args,ONE);
    first = firstDOW(month,year)-1-appResources.dowOffset;
    numDays = lastDay(month,year);
    for (i=0; i < 42; i++) {
	d = m->days[i];
	if (i < first || i >= first+numDays) {
	    d->day = 0;
	    XFillRectangle(display,d->pixmap,emptyGC,0,0,50,50);
	    XtSetArg(args[0],XtNbitmap,d->pixmap);
	    XtSetValues(d->button,args,ONE);
	} else {
	    d->day = i-first+1;
	    d->month = month;
	    d->year = year;
	    shadeButton(d,gc,x,y);
	}
    }
    XawFormDoLayout(m->form,True);
    m->month = month;
    m->year = year;
}

/*
 * setGCAndXY() : Depending on num, parses the appropriate geometry
 *	string into *xp and *yp, and puts the correct GC* into gcp.
 *	This function is here so it can be outside the loop in
 *	setMonthFormData(), but really belongs in shadeButton.
 */
static void
getGCAndXY(num,gcp,xp,yp)
GC *gcp;
int *xp,*yp;
{
    int w,h;

    switch (num) {
	case 1: XParseGeometry(appResources.datePosition1,xp,yp,&w,&h);
		*gcp = dateGC1;
		break;
	case 2: XParseGeometry(appResources.datePosition2,xp,yp,&w,&h);
		*gcp = dateGC2;
		break;
	case 3: XParseGeometry(appResources.datePosition3,xp,yp,&w,&h);
		*gcp = dateGC3;
		break;
	case 12: XParseGeometry(appResources.datePosition12,xp,yp,&w,&h);
		 *gcp = dateGC12;
		 break;
    }
}

/*
 * shadeButton() : Shades a dayButton. This is global so it can be called
 *	after the appointments have changed without redrawing everything.
 */
void
shadeButton(d,gc,x,y)
DayButtonData *d;
GC gc;
int x,y;
{
    Arg args[1];
    char text[4];
    int n;

    if (gc == (GC)NULL)
	getGCAndXY(appResources.numMonths,&gc,&x,&y);
    n = lookupEntry(d->day,d->month,d->year,-1,-1,-1,NULL,True);
    if (n > appResources.maxLevel)
	n = appResources.maxLevel;
    XFillRectangle(display,d->pixmap,shadeGC[n],0,0,
			appResources.dateWidth1,appResources.dateHeight1);
    sprintf(text,"%d",d->day);
    if (appResources.opaqueDates)
	XDrawImageString(display,d->pixmap,gc,x,y,text,strlen(text));
    else
	XDrawString(display,d->pixmap,gc,x,y,text,strlen(text));
    XtSetArg(args[0],XtNbitmap,d->pixmap);
    XtSetValues(d->button,args,ONE);
}

/*
 * selectDay() : Used to highlight a day (usually on startup or from
 *	the "today" action) without the user having clicked on it.
 */
void
selectDay(m,day,mon,year)
MonthFormData *m;
int day,mon,year;
{
    int which;

    which = firstDOW(mon,year)-appResources.dowOffset+day-2;
    XawToggleSetCurrent(radioGroup,m->days[which]->button);
}

/*
 * dayButtonCallbackProc() : The callback when a day is clicked on (or
 *	at least when the "notify" action is invoked). Pops up a new
 *	dayForm is there isn't currently one, and sets it or the current
 *	one to the new day's appoints.
 */
/*ARGSUSED*/
CALLBACK_PROC(dayButtonCallbackProc)
{
    DayButtonData *d = (DayButtonData *)client_data;

    if (d->day == 0)
	return;
    if (currentDayFormData == NULL)
	currentDayFormData = createPopupDayFormData();
    else
	checkpointAppoints(currentDayFormData);
    currentDayFormData->buttonData = d;
    if (XawToggleGetCurrent(radioGroup) != NULL)
	setDayFormData(currentDayFormData,d->day,d->month,d->year);
    else
	clearDayFormData(currentDayFormData);
}
