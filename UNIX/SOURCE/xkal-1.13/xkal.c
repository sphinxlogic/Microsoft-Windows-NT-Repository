/*
 *	xkal.c : graphical appointment tool
 *
 *	George Ferguson, ferguson@cs.rochester.edu,  27 Oct 1990.
 *	Version 1.1 - 27 Feb 1991.
 *
 *	$Id: xkal.c,v 2.2 91/02/28 11:23:24 ferguson Exp $
 *
 */
#ifndef lint
static char ident[] = "@(#) xkal version 1.1 - ferguson@cs.rochester.edu";
#endif
#include <stdio.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Drawing.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Cardinals.h>	
#include "EzMenu.h"
#include "app-resources.h"
#include "resources.h"
#include "month.h"
#include "day.h"
#include "edit.h"
#include "edit-defaults.h"
#include "db.h"
#include "util.h"
#include "date-strings.h"
#include "patchlevel.h"
#ifdef USE_ALERT
#include "alert.h"
#endif

/*	-	-	-	-	-	-	-	-	*/
/*
 * Functions defined in this file:
 */
int main();
void quit(),quit_nosave(),save(),next(),prev(),today(),setNumMonths();
void focusThisItem(), focusNoItem(), focusNextItem(), focusPrevItem();
void escapeToSystem();
void timeoutProc();

static void initGraphics(), initGCs(), initWidgets(), createMonthFormDatas();
static void setMonths(), selectCurrentDay();
static void listAppoints(), checkAndSaveAppoints();
static int countAppoints();
static void cvtStringToPixels(), cvtStringToPixmaps(), syntax();
static Pixmap readBitmap();

/*
 * Global graphics data
 */
Display *display;
Screen *screen;
Window root;
GC dateGC1,dateGC2,dateGC3,dateGC12,emptyGC,*shadeGC;

/*
 * Global widget data
 */
XtAppContext app_con;
Widget toplevel;
Widget bothForm,allForm;
AppResources appResources;
MonthFormData *monthFormData1,*monthFormData2[2];
MonthFormData *monthFormData3[3],*monthFormData12[12];

/*
 * Other global data
 */
char *program;
int currentDay,currentMon,currentYear;		/* today */
int mainDay,mainMon,mainYear;			/* display */
int appointsChanged = False;			/* need to save? */
XtIntervalId timeoutId;				/* auto update */

/*
 * Action binding table
 */
static XtActionsRec cmdActionsTable[] = {
	{ "xkal-quit", quit },
	{ "xkal-exit", quit_nosave },
	{ "xkal-save", save },
	{ "xkal-next", next },
	{ "xkal-prev", prev },
	{ "xkal-today", today },
	{ "xkal-numMonths", setNumMonths },
	{ "xkal-system", escapeToSystem },
	{ "xkal-focus-this-item", focusThisItem },
	{ "xkal-focus-no-item", focusNoItem },
	{ "xkal-focus-next-item", focusNextItem },
	{ "xkal-focus-prev-item", focusPrevItem },
	{ "xkal-edit-appoint", editAppoint },
	{ "xkal-focus-next-edit-item", focusNextEditItem },
	{ "xkal-focus-prev-edit-item", focusPrevEditItem },
	{ "xkal-edit-defaults", editDefaults },
	{ "xkal-focus-next-defaults-item", focusNextDefaultsItem },
	{ "xkal-focus-prev-defaults-item", focusPrevDefaultsItem },
};

/*
 *	Widget and non-widget resources if the application defaults
 *	file can't be found.
 *	[ Generated automatically from Xkal.ad. ]
 */
static String fallbackResources[] = {
#include "Xkal.ad.h"
	NULL
};

#define ACTION_PROC(NAME)	void NAME(w,event,params,num_params) \
					Widget w; \
					XEvent *event; \
					String *params; \
					Cardinal *num_params;

/*	-	-	-	-	-	-	-	-	*/

int
main(argc,argv)
int argc;
char **argv;
{
    int i;

    program = *argv;
    for (i=1; i < argc; i++)		/* quick check for non-interactive */
	if (strncmp(argv[i],"-l",2) == 0 || strncmp(argv[i],"-s",2) == 0 ||
						strncmp(argv[i],"-v",2) == 0)
	    break;
    if (i != argc)
	getResources(&argc,argv);	/* found, so don't need display */
    else
	initGraphics(&argc,argv);	/* not found, so interactive mode */
    if (argc > 1) {
	syntax(argc,argv);
	XtDestroyApplicationContext(app_con);
	exit(-1);
    }
    initDb();
    initDateStrings();
    getCurrentDate(&currentDay,&currentMon,&currentYear);
    if (appResources.systemAppoints && *appResources.systemAppoints)
	readDb(appResources.systemAppoints,True);
    if (appResources.personalAppoints && *appResources.personalAppoints)
	readDb(appResources.personalAppoints,False);
    if (appResources.date != NULL && *(appResources.date) != '\0') {
	parseDate(appResources.date,&currentDay,&currentMon,&currentYear);
	if (currentYear < 1901) {
	    fprintf(stderr,"%s: invalid year (too early): %d\n",program,
								currentYear);
	    XtDestroyApplicationContext(app_con);
	    exit(-1);
	}
    }
    if (appResources.version) {
	printf("xkal %.2f - George Ferguson (ferguson@cs.rochester.edu)\n",
								XKAL_VERSION);
	exit(0);
    }
    if (appResources.listOnly) {
	listAppoints(currentDay,currentMon,currentYear);
    }
    if (appResources.listOnly || appResources.silent) {
	exit(countAppoints(currentDay,currentMon,currentYear,
						 appResources.exitUsesLevels));
    }
    mainDay = currentDay;
    mainMon = currentMon;
    mainYear = currentYear;
    initGCs();
    initWidgets();
    XtRealizeWidget(toplevel);
    if (currentDayFormData != NULL)
	selectCurrentDay();
    if (appResources.checkpointInterval > 0)
	timeoutId = XtAppAddTimeOut(app_con,
			(unsigned long)(appResources.checkpointInterval*60000),
			timeoutProc,NULL);
    XtAppMainLoop(app_con);
}

/*	-	-	-	-	-	-	-	-	*/
/* Initialization routines */

/*
 * initGraphics() : Initialze X connection and toplevel widget.
 */
static void
initGraphics(argcp,argv)
int *argcp;
char **argv;
{
    char *rev,*strchr();

    toplevel = XtAppInitialize(&app_con, "Xkal",
			       xkalOptions, XtNumber(xkalOptions),
			       argcp,argv,fallbackResources,NULL,ZERO);
    XtGetApplicationResources(toplevel,(XtPointer)&appResources,
                              xkalResources,XtNumber(xkalResources),NULL,ZERO);
    rev = strchr(fallbackResources[0],'$');
    if (rev && strcmp(appResources.revision,rev) != 0) {
	fprintf(stderr,"%s: app-defaults release not %s\n",program,rev);
	fprintf(stderr,"%s: you may have an outdated app-defaults file\n",
								program);
    }
    XtAppAddActions(app_con,cmdActionsTable,XtNumber(cmdActionsTable));
    display = XtDisplay(toplevel);
    screen = XtScreen(toplevel);
    root = RootWindowOfScreen(screen);
}

/*
 * initGCs() : Initialize GC's for drawing operations.
 */
static void
initGCs()
{
    XGCValues values;
    XtGCMask mask;
    Pixel *pixels;
    Pixmap *pixmaps;
    int i;

    /*
     * initialize date GC's
     */
    values.foreground = (unsigned long)1;
    values.background = (unsigned long)0;
    values.function = GXset;
    mask = GCForeground | GCBackground | GCFunction | GCFont;
    values.font = appResources.dateFont1;
    dateGC1 = XCreateGC(display,root,mask,&values);
    values.font = appResources.dateFont2;
    dateGC2 = XCreateGC(display,root,mask,&values);
    values.font = appResources.dateFont3;
    dateGC3 = XCreateGC(display,root,mask,&values);
    values.font = appResources.dateFont12;
    dateGC12 = XCreateGC(display,root,mask,&values);
    /*
     * initialize empty GC
     */
    values.function = GXcopy;
    values.fill_style = FillTiled;
    values.tile = readBitmap(appResources.noDayShade);
    mask = GCForeground | GCBackground | GCFunction | GCFillStyle | GCTile;
    emptyGC = XCreateGC(display,root,mask,&values);
    /*
     * initialize shading GC's
     */
    values.function = GXcopy;
    mask = GCFunction;
    shadeGC = (GC *)XtCalloc(appResources.maxLevel+1,sizeof(GC));
    shadeGC[0] = XCreateGC(display,root,mask,&values);
    if (DefaultDepthOfScreen(screen) > 1) {	/* COLOR */
	pixels = (Pixel *)XtCalloc(appResources.maxLevel,sizeof(Pixel));
	cvtStringToPixels(appResources.colors,appResources.maxLevel,pixels);
	for (i=1; i <= appResources.maxLevel; i++) {
	    values.foreground = pixels[i-1];
	    values.fill_style = FillSolid;
	    values.function = GXcopy;
	    mask = GCForeground | GCBackground | GCFunction;
	    shadeGC[i] = XCreateGC(display,root,mask,&values);
	}
	XtFree(pixels);
    } else {					/* B&W */
	pixmaps = (Pixmap *)XtCalloc(appResources.maxLevel,sizeof(Pixmap));
	cvtStringToPixmaps(appResources.shades,appResources.maxLevel,pixmaps);
	for (i=1; i <= appResources.maxLevel; i++) {
	    values.background = (unsigned long)0;
	    values.tile = pixmaps[i-1];
	    values.fill_style = FillTiled;
	    values.function = GXcopy;
	    mask = GCForeground | GCBackground | GCTile | GCFillStyle |
								GCFunction;
	    shadeGC[i] = XCreateGC(display,root,mask,&values);
	}
	XtFree(pixmaps);
    }
}

/*
 * initWidgets() : Create necessary widgets and menus (ie. those that
 *	aren't created on the fly).
 */
static void
initWidgets()
{
    bothForm = XtCreateManagedWidget("bothForm",formWidgetClass,toplevel,
								NULL,ZERO);
    XtCreateManagedWidget("fileButton",menuButtonWidgetClass,bothForm,
								NULL,ZERO);
    XtCreatePopupShell("fileMenu",ezMenuWidgetClass,bothForm,NULL,ZERO);
    XtCreateManagedWidget("viewButton",menuButtonWidgetClass,bothForm,
								NULL,ZERO);
    XtCreatePopupShell("viewMenu",ezMenuWidgetClass,bothForm,NULL,ZERO);
    XtCreateManagedWidget("otherButton",menuButtonWidgetClass,bothForm,
								NULL,ZERO);
    XtCreatePopupShell("otherMenu",ezMenuWidgetClass,bothForm,NULL,ZERO);
    allForm = XtCreateManagedWidget("allMonthsForm",formWidgetClass,bothForm,
								NULL,ZERO);
    createMonthFormDatas(appResources.numMonths);
    if (appResources.bothShown)
	currentDayFormData = createDayFormData(bothForm);
    setMonths();
}

/*
 * createMonthFormDatas(n) : Create n monthForms as children of the
 *	allForm widget.
 */
static void
createMonthFormDatas(n)
int n;
{
    char *name = "monthFormXX_XX";
    int i;

    switch (n) {
	case 1: monthFormData1 = createMonthFormData(allForm,"monthForm1",1);
		XtManageChild(monthFormData1->form);
		break;
	case 2: for (i=0; i < 2; i++) {
		    sprintf(name,"monthForm2_%d",i+1);
		    monthFormData2[i] = createMonthFormData(allForm,name,2);
		    XtManageChild(monthFormData2[i]->form);
		}
		break;
	case 3: for (i=0; i < 3; i++) {
		    sprintf(name,"monthForm3_%d",i+1);
		    monthFormData3[i] = createMonthFormData(allForm,name,3);
		    XtManageChild(monthFormData3[i]->form);
		}
		break;
	case 12: for (i=0; i < 12; i++) {
		    sprintf(name,"monthForm12_%d",i+1);
		    monthFormData12[i] = createMonthFormData(allForm,name,12);
		    XtManageChild(monthFormData12[i]->form);
		}
		break;
    }
}

/*
 * setMonths() : Redraw the months.
 */
static void
setMonths()
{
    int i,prevMon,prevYear,nextMon,nextYear;

    XawFormDoLayout(allForm,False);
    switch (appResources.numMonths) {
	case 1:  setMonthFormData(monthFormData1,1,mainMon,mainYear);
		 break;
	case 2:	 if (mainMon == 12) {
		     nextMon = 1;
		     nextYear = mainYear+1;
		 } else {
		     nextMon = mainMon+1;
		     nextYear = mainYear;
		 }
		 setMonthFormData(monthFormData2[0],2,mainMon,mainYear);
		 setMonthFormData(monthFormData2[1],2,nextMon,nextYear);
		 break;
	case 3:  prevMon = mainMon - 1;
		 nextMon = mainMon + 1;
		 prevYear = nextYear = mainYear;
		 if (mainMon == 1) {
		     prevMon = 12;
		     prevYear = mainYear - 1;
		 } else if (mainMon == 12) {
		     nextMon = 1;
		     nextYear = mainYear + 1;
		 }
		 setMonthFormData(monthFormData3[0],3,prevMon,prevYear);
		 setMonthFormData(monthFormData3[1],3,mainMon,mainYear);
		 setMonthFormData(monthFormData3[2],3,nextMon,nextYear);
		 break;
	case 12: for (i=0; i < 12; i++)
		     setMonthFormData(monthFormData12[i],12,i+1,mainYear);
		 break;
    }
    XawFormDoLayout(allForm,True);
}

/*
 * selectCurrentDay() : Higlight current day after figuring out which
 *	form its in.
 */
static void
selectCurrentDay()
{
    switch (appResources.numMonths) {
	case 1: selectDay(monthFormData1,mainDay,mainMon,mainYear);
		break;
	case 2: selectDay(monthFormData2[0],mainDay,mainMon,mainYear);
		break;
	case 3: selectDay(monthFormData3[1],mainDay,mainMon,mainYear);
		break;
	case 12: selectDay(monthFormData12[mainMon-1],mainDay,mainMon,mainYear);
		 break;
    }
}

/*	-	-	-	-	-	-	-	-	*/

/*
 * quit() : Save appoints if changed, then quit.
 */
/*ARGSUSED*/
ACTION_PROC(quit)
{
    checkAndSaveAppoints(False);
    XtDestroyApplicationContext(app_con);
    exit(0);
}

/*
 * quit_nosave() : Quit without saving.
 */
/*ARGSUSED*/
ACTION_PROC(quit_nosave)
{
    XtDestroyApplicationContext(app_con);
    exit(0);
}

/*
 * next() : Display next month.
 */
/*ARGSUSED*/
ACTION_PROC(next)
{
    if (appResources.numMonths < 12) {
	if (mainMon == 12) {
	    mainMon = 1;
	    mainYear += 1;
	} else {
	    mainMon += 1;
	}
    } else {
	mainYear += 1;
    }
    setMonths();
}

/*
 * prev() : Display previous month.
 */
/*ARGSUSED*/
ACTION_PROC(prev)
{
    if (appResources.numMonths < 12) {
	if (mainMon == 1) {
	    mainMon = 12;
	    mainYear -= 1;
	} else {
	    mainMon -= 1;
	}
    } else {
	mainYear -= 1;
    }
    setMonths();
}

/*
 * today() : Highlight today, possibly resetting months. Also, if date
 *	was not given as resource, then get it again in case we've been
 *	runnign for a while.
 */
/*ARGSUSED*/
ACTION_PROC(today)
{
    /* Reset the current date in case we've been running for a long time. */
    if (appResources.date == NULL || *(appResources.date) == '\0')
	getCurrentDate(&currentDay,&currentMon,&currentYear);
    mainDay = currentDay;
    if (mainMon != currentMon || mainYear != currentYear) {
	mainMon = currentMon;
	mainYear = currentYear;
	setMonths();
    }
    selectCurrentDay();
}

/*
 * setNumMonths(n) : Change the number of months being displayed, possibly
 *	creating new monthForms. n should be 1, 2, 3, or 12.
 */
/*ARGSUSED*/
ACTION_PROC(setNumMonths)
{
    int n,i;

    if (*num_params != ONE) {
	fprintf(stderr,"%s: bad parms to xkal-numMonths()\n",program);
	return;
    }
    n = atoi(params[0]);
    if (n != 1 && n != 2 && n != 3 && n != 12) {
	fprintf(stderr,"%s: bad num for xkal-numMonths(): %d\n",program,n);
	return;
    }
    if (n == appResources.numMonths)
	return;
    XawFormDoLayout(bothForm,False);
    switch (appResources.numMonths) {
	case 1: XtUnmanageChild(monthFormData1->form);
		break;
	case 2: for (i=0; i < 2; i++)
		    XtUnmanageChild(monthFormData2[i]->form);
		break;
	case 3: for (i=0; i < 3; i++)
		    XtUnmanageChild(monthFormData3[i]->form);
		break;
	case 12: for (i=0; i < 12; i++)
		    XtUnmanageChild(monthFormData12[i]->form);
		break;
    }
    appResources.numMonths = n;
    switch (appResources.numMonths) {
	case 1: if (monthFormData1 == NULL)
		    createMonthFormDatas(1);
		setMonths();
		XtManageChild(monthFormData1->form);
		break;
	case 2: if (monthFormData2[0] == NULL)
		    createMonthFormDatas(2);
		setMonths();
		for (i=0; i < 2; i++)
		    XtManageChild(monthFormData2[i]->form);
		break;
	case 3: if (monthFormData3[0] == NULL)
		    createMonthFormDatas(3);
		setMonths();
		for (i=0; i < 3; i++)
		    XtManageChild(monthFormData3[i]->form);
		break;
	case 12: if (monthFormData12[0] == NULL)
		     createMonthFormDatas(12);
		 setMonths();
		 for (i=0; i < 12; i++)
		     XtManageChild(monthFormData12[i]->form);
		 break;
    }
    XawFormDoLayout(bothForm,True);
}

/*
 * save() : Save appoints regardless of whether they've changed or not.
 */
/*ARGSUSED*/
ACTION_PROC(save)
{
    checkAndSaveAppoints(True);
}

/*
 * escapeToSystem(str) : execute "str" by the shell.
 */
/*ARGSUSED*/
ACTION_PROC(escapeToSystem)
{
    if (*num_params != 1) {
#ifdef USE_ALERT
	alert("Error: too many arguments to xkal-system().");
#else
	fprintf(stderr,"\007%s: too many arguments to xkal-system()\n",program);
#endif
	return;
    }
    system(params[0]);
}

/*
 * focusThisItem() : Force input focus to this widget.
 */
/*ARGSUSED*/
ACTION_PROC(focusThisItem)
{
    XWindowAttributes attrs;

    XGetWindowAttributes(display,XtWindow(w),&attrs);
    if (attrs.map_state != IsViewable)
	return;
    XSetInputFocus(display,XtWindow(w),RevertToParent,CurrentTime);
}

/*
 * focusNoItem() : Unset the input focus.
 */
/*ARGSUSED*/
ACTION_PROC(focusNoItem)
{
    XSetInputFocus(display,PointerRoot,RevertToParent,CurrentTime);
}

/*
 * focusNextItem() : Switch input focus to next appointment slot, wrapping
 *	at bottom.
 */
/*ARGSUSED*/
ACTION_PROC(focusNextItem)
{
    int i;

    for (i=0; i < numDisplayedAppoints; i++)
	if (w == currentDayFormData->items[i]->text)
	    break;
    if (i == numDisplayedAppoints)
	return;
    else if (i == numDisplayedAppoints-1)
	i = 0;
    else
	i += 1;
    XSetInputFocus(display,XtWindow(currentDayFormData->items[i]->text),
						RevertToParent,CurrentTime);
}

/*
 * focusPrevItem() : Switch input focus to previous appointment slot, wrapping
 *	at top.
 */
/*ARGSUSED*/
ACTION_PROC(focusPrevItem)
{
    int i;

    for (i=0; i < numDisplayedAppoints; i++)
	if (w == currentDayFormData->items[i]->text)
	    break;
    if (i == numDisplayedAppoints)
	return;
    else if (i == 0)
	i = numDisplayedAppoints-1;
    else
	i -= 1;
    XSetInputFocus(display,XtWindow(currentDayFormData->items[i]->text),
						RevertToParent,CurrentTime);
}

/*
 * timeoutProc() : Timer callback - saves appoints if they've changed and
 *	resets timer.
 */
/*ARGSUSED*/
void
timeoutProc(data,id)
XtPointer data;
XtIntervalId *id;
{
    checkAndSaveAppoints(False);
    if (appResources.checkpointInterval > 0)
	timeoutId = XtAppAddTimeOut(app_con,
			(unsigned long)(appResources.checkpointInterval*60000),
			timeoutProc,NULL);
}

/*	-	-	-	-	-	-	-	-	*/
/* Miscellaneous routines */

/*
 * listAppoints() : Print all appoints for day to stdout.
 */
static void
listAppoints(day,mon,year)
int day,mon,year;
{
    Msgrec **apps;
    int dow,n,i;

    dow = computeDOW(day,mon,year);
    printf("%s %d %s %d\n",shortDowStr[dow-1],day,shortMonthStr[mon-1],year);
    n = lookupEntry(day,mon,year,-1,-1,-1,NULL,False);
    if (n == 0)
	return;
    apps = (Msgrec **)XtCalloc(n,sizeof(Msgrec *));
    (void)lookupEntry(day,mon,year,-1,-1,n,apps,False);
    for (i=0; i < n; i++)
	if (apps[i]->hour == -1)
	    printf("\t%s %s\n",appResources.notesLabel,apps[i]->text);
	else if (apps[i]->mins == -1)
	    if (apps[i]->hour >= 12)
		printf("\t%2dpm %s\n",apps[i]->hour-12,apps[i]->text);
	    else
		printf("\t%2dam %s\n",apps[i]->hour,apps[i]->text);
	else
	    printf("\t%2d:%02d %s\n",apps[i]->hour,apps[i]->mins,apps[i]->text);
    XtFree(apps);
}

/*
 * countAppoints() : Return number of appoints (flag = False) or criticality
 *	total (flag = True) for day.
 */
static int
countAppoints(day,mon,year,flag)
int day,mon,year;
Boolean flag;
{
    
    return(lookupEntry(day,mon,year,-1,-1,-1,NULL,flag));
}

/*
 * checkAndSaveAppoints() : Transfer data from dayForm to DB, then save
 *	appoints if they've changed.
 */
static void
checkAndSaveAppoints(force)
Boolean force;
{
    if (currentDayFormData != NULL)
	checkpointAppoints(currentDayFormData);
    if (force || appointsChanged)
	writeDb(appResources.personalAppoints,False);
    appointsChanged = False;
}

/*	-	-	-	-	-	-	-	-	*/
/* Miscellaneous conversion routines */

/*
 * cvtStringToPixels() : Converts a string that is a whitespace-separated
 *	list of pixel names, to an array of Pixels.
 */
static void
cvtStringToPixels(str,max,pixels)
char *str;
int max;
Pixel *pixels;
{
    XColor screen_in_out,exact;
    char buf[64];
    int i,j;

    strcpy(buf,XtDefaultBackground);
    for (i=0; i < max; i++) {
	if (*str) {
	    while (isspace(*str))
		str += 1;
	    if (!*str)
		break;
	    j = 0;
	    while (*str && j < 63 && !isspace(*str))
		buf[j++] = *str++;
	    buf[j] = '\0';
	    if (*str && !isspace(*str)) {
		fprintf(stderr,"%s: color name too long: %s\n",program,buf);
		while (*str && !isspace(*str))
		    str += 1;
	    }
	}
	XAllocNamedColor(display,DefaultColormapOfScreen(screen),buf,
							&screen_in_out,&exact);
	pixels[i] = screen_in_out.pixel;
    }
}

/*
 * cvtStringToPixmaps() : Converts a string that is a whitespace-separated
 *	list of bitmap names, to an array of Pixmaps.
 */
static void
cvtStringToPixmaps(str,max,pixmaps)
char *str;
int max;
Pixmap *pixmaps;
{
    char buf[64];
    int i,j;

    strcpy(buf,"gray");
    for (i=0; i < max; i++) {
	if (*str) {
	    while (isspace(*str))
		str += 1;
	    if (!*str)
		break;
	    j = 0;
	    while (*str && j < 63 && !isspace(*str))
		buf[j++] = *str++;
	    buf[j] = '\0';
	    if (*str && !isspace(*str)) {
		fprintf(stderr,"%s: shade name too long: %s\n",program,buf);
		while (*str && !isspace(*str))
		    str += 1;
	    }
	}
	pixmaps[i] = readBitmap(buf);
    }
}

/*
 * readBitmap() : Reads the given file using XmuLocateBitmapFile protocol,
 *	then converts to Pixmap and returns it. If not found, prints error
 *	message and returns a default Pixmap.
 */
static Pixmap
readBitmap(name)
char *name;
{
    Pixmap b;
    int w,h,xhot,yhot;

    b = XmuLocateBitmapFile(screen,name,NULL,ZERO,&w,&h,&xhot,&yhot);
    if (b == NULL) {
	fprintf(stderr,"%s: couldn't find bitmap \"%s\"\n",program,name);
	return(XCreatePixmap(display,screen,2,2,DefaultDepthOfScreen(screen)));
    } else
	return(XmuCreatePixmapFromBitmap(display,root,b,w,h,
					DefaultDepthOfScreen(screen),1,0));
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * syntax() : Print whatever caused the error and the usage message.
 */
static void
syntax(argc,argv)
int argc;
char **argv;
{
    argv += 1;
    if (argc > 2 || (strcmp(*argv,"-help") != 0 && strcmp(*argv,"-?") != 0)) {
	fprintf(stderr,"%s: bad argument(s): ",program);
	while (--argc)
	    fprintf(stderr,"%s ",*argv++);
	fprintf(stderr,"\n");
    }
    fprintf(stderr,"usage: %s",program);
    fprintf(stderr,"\t-appoints file\tuse file for personal appointments\n");
    fprintf(stderr,"\t\t-date date\tuse given date for `today'\n");
    fprintf(stderr,"\t\t-numMonths 1|2|3|12\tstart displaying 1, 2, 3 or 12 months\n");
    fprintf(stderr,"\t\t-bothShown\tput day display next to month(s)\n");
    fprintf(stderr,"\t\t-nobothShown\tpopup days as needed\n");
    fprintf(stderr,"\t\t-version\tprint version info to stdout\n");
    fprintf(stderr,"\t\t-listOnly\tprint appoints to stdout\n");
    fprintf(stderr,"\t\t-silent\t\texit with status == number of appoints\n");
    fprintf(stderr,"\t\t-exitUsesLevels\texit status is criticality total\n");
    fprintf(stderr,"\t\t-noexitUsesLevels\texit status is number of appoints\n");
    fprintf(stderr,"\t\t-opaqueDates\tprint day numbers opaquely\n");
    fprintf(stderr,"\t\t-noopaqueDates\tprint day numbers transparently\n");
    fprintf(stderr,"\t\t-dowLabels\tprint day of week at top of columns\n");
    fprintf(stderr,"\t\t-nodowLabels\tdon't print dow at top of columns\n");
    fprintf(stderr,"\t\t-titlebar\tuse titlebars to display dates\n");
    fprintf(stderr,"\t\t-notitlebar\tdon't use titlebars to display dates\n");
    fprintf(stderr,"\t\t-checkpointInterval mins\tmins between autosaves\n");
    fprintf(stderr,"\t\t-xrm 'resource: value'\tpass arbitrary resources\n");
}
