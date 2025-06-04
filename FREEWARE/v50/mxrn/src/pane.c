#ifdef VMS
/*
 * Note: Using #module in Unix will get interpreted by the preprocessor as a 
 *	 control line and blow up. Do not uncomment the following line.
 *
 * module PANE "3.2"
 */
#else
#ifndef lint
static char rcsid[] = "$Header: Pane.c,v 1.1 89/01/03 16:59:31 gringort Exp $";
#endif lint
#endif VMS
/*
 * Copyright 1988, 1989 by Digital Equipment Corporation, Maynard, Massachusetts.
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
/*
 * V1.0a    12-Jan-1988	MRR
 *	    Add our logicals to #includes.
 * V1.0b    26-Feb-1988	RDB
 *	    Fix bugs with allowresizing & geometry manager
 * V1.0c    01-Mar-1988	PMB
 *	    Merge with latest copy of pane widget.
 * V1.0d    17-Mar-1988	RDB
 *	    fix major bugs and enhancements
 * V1.1	    1-Apr-1988	MR
 *	    Make work with BL7.2 - add class_part_initialize,
 *	    add time argument to AcceptFocus.
 * V1.2	    13-Apr-1988	JV
 *	    Make work with BL7.6 - changed DwtNcallback to DwtNmapCallback
 * V1.3	    20-Apr-1988	MRR
 *	    Upward compatibility support.
 * V1.4	    13-Jun-1988	MRR
 *	    Add help support.
 * V1.5	    30-Jun-1988	MRR
 *	    Support for Help-MBx.
 * V1.6	    19-Jul-1988	MRR
 *	    Help modifier changed to @Help.
 * V1.7	    20-Jul-1988	RDB
 *	    Initialization Bug
 * V1.8	    01-Sep-1988	RDB
 *	    Resize on DwtPaneSetMax error
 *	    Knob draging problem
 * V1.9	    07-Sep-1988	RDB
 *	    Color changes on 8 plane GPX on Knob drag
 * V1.10    08-Sep-1988	RDB
 *	    Extra knob appearing when managing and unmanaging widgets
 * V1.11    06-Oct-1988 RDB
 *	    During Resize check if widget height also changed.
 * V1.12    19-Oct-1988	RDB
 *	    change cursor to vertical or horizontial cursor and 
 *	    MB2 & MB3 pushes fixed
 * V1.13    31-Oct-1988 RDB
 *	    Add DwtPaneAddChild calls
 * V1.14    01-Nov-1988	RDB
 *	    Change resize to resizable and allowresizing to resize
 * V1.15    07-Nov-1988	RDB
 *	    Allow shared panes
 * V1.16    29-Nov-1988 RDB
 *	    Add Destory routine
 *	    Add SetValues routine
 *	    Fix SetConstraintValues routine
 *	    Add DeleteChild routine
 *	    Add FocusCallbacks
 *	    Add MapCallbacks
 *	    Fix FocusNext, FocusPrev routines
 *	    Fix compiledtextbindings
 *	    Add VMS routines for all the public routines.
 * V1.17    06-Jan-1989	JG
 *	    Make work on PMAX
 * V2.0     15-Feb-1989 MRR
 *	    AcceptFocus should return what child's accept_focus returns.
 * V2.1     22-Feb-1989 RDB
 *	    Fix set min and max resize but when pane is shared
 * V2.2	    1-Mar-1989	MRR
 *	    GC's gotten with XtGetGC should be removed with XtDestroyGC,
 *	    not XFreeGC.
 * V2.3	    7-Mar-1989	RDB
 *	    Orientation (w) defined incorrectly
 * V2.4	    9-Mar-1989	MRR
 *	    CvtStringToOrientation and DwtPrivate no longer needed.
 * V2.5	    9-Mar-1989	MRR
 *	    Copy DwtUpdateCallback here and rename as LclUpdateCallback.
 * V2.6	    14-Mar-1989	RDB
 *	    Change the default width and height to be 200 instead of 100
 * V2.7	    23-Mar-1989	RDB
 *	    change resource default for Foreground
 * V2.8	    24-mar-1989	RDB
 *	    Make knobs mullions
 * V2.9	    30-Mar-1989	RDB
 *	    Redo definitions for panewidgetclass, panewidgetclassrec, mullions
 *	    Style guide compliancy on vertical & horizontal cursor
 * V2.10    04-Apr-1989	RDB
 *	    Change MullionSize to be something like borderwidth &
 *	    add MullionLength to be the total height of a mullion
 * V2.11    05-Apr-1989	RDb
 *	    Make LclUpdateCallback static
 * V2.12    03-May-1989	RDB
 *	    MB2 & MB3 cursor changes
 * V2.13    12-May-1989	RDB
 *	    make widget_size the PanePart not PaneRec
 * V2.14    15-May-1989	RDB
 *	    Geometry Manager doesn't allow widgets to resize
 *	    DwtPaneMakeViewable error in selecting right subwidget
 * V3.0	    22-Aug-1989 MRR
 *	    Copied to V3 library.
 * V3.1	    29-Aug-1989	RDB
 *	    Fix DwtPaneMakeViewable error in setting the widget position
 * V3.2	    30-AUG-1989 RDB
 *	    Remove Const from Joel
 * V3.3	    23-Oct-1989	RDB
 *	    New version of pane widget
 */
#define PANE

#ifdef VMS
#include <decw$include/DwtWidget.h>
#include <DECW$CURSOR.h>
#include "PaneP.h"
#include "Pane.h"
#else
#define NOT_VMS_V1 /* required for part offsets to work before Ultrix 2.2 */ 
#include <X11/DwtWidget.h>
#include <X11/Convert.h>
#include <X11/decwcursor.h>
#include "PaneP.h"
#include "Pane.h"
#endif

#ifdef __STDC__
#undef DwtField
#undef DwtPartOffset
#define DwtField(widget, offsetrecord, part, variable, type) \
	(*(type *)(((char *) (widget)) + offsetrecord[Dwt##part##Index] + \
		XtOffset(part##Part *, variable)))
#define DwtPartOffset(part, variable) \
        ((Dwt##part##Index) << HALFLONGBITS) + XtOffset(part##Part *, variable)
#endif

#define NoMask                     0L
 
#ifndef VMS
extern char *DwtDescToNull();  /* jmg */
#endif VMS
 
/*
 * 'Length' is defined as height for vpanes, and width for hpanes.  'Breadth'
 * is the other dimension.
 */
 
#define DwtPaneIndex (DwtConstraintIndex + 1)
 
/* Macros to access pane fields */
#define PaneField(w,class,field,type)				\
	DwtField((w),						\
		 ((PaneClass)(w)->core.widget_class)->		\
		 	pane_class.paneoffsets,class,		\
		 field,						\
		 type)
 
#define X(w)		PaneField(w,Core,x,Position)
#define Y(w)		PaneField(w,Core,y,Position)
 
#define Width(w)		PaneField(w,Core,width,Dimension)
#define Height(w)		PaneField(w,Core,height,Dimension)
#define BackgroundPixel(w)	PaneField(w,Core,background_pixel,Pixel)
 
#define Children(w)		PaneField(w,Composite,children,WidgetList)
#define NumChildren(w)		PaneField(w,Composite,num_children,Cardinal)
 
#define MullionSize(w)		PaneField(w,Pane,mullionsize,Dimension)
#define MullionLength(w)	PaneField(w,Pane,mullionlength,Dimension)
#define LeftMullion(w)		PaneField(w,Pane,leftmullion,Boolean)
#define Orientation(w)		PaneField(w,Pane,orientation,unsigned char)
#define PaneNumChildren(w)	PaneField(w,Pane,num_children,Cardinal)
#define PaneChildren(w)		PaneField(w,Pane,children,WidgetList)
#define InterSub(w)		PaneField(w,Pane,intersub,Dimension)
#define ResizeMode(w)		PaneField(w,Pane,resize_mode,unsigned char)
#define Foreground(w)		PaneField(w,Pane,foreground,Pixel)
#define InvGC(w)		PaneField(w,Pane,invgc,GC)
#define WhichAdjust(w)		PaneField(w,Pane,whichadjust,Widget)
#define OrigLoc(w)		PaneField(w,Pane,origloc,Position)
#define OverrideText(w)		PaneField(w,Pane,overridetext,Boolean)
#define LastHadFocus(w)		PaneField(w,Pane,lasthadfocus,Widget)
#define HelpCallback(w)		PaneField(w,Pane,helpcallback,DwtCallbackStruct)
 
#define FindLength(width, height) \
    ((Orientation(pane) == DwtOrientationVertical) ? (height) : (width))
#define FindBreadth(width, height) \
    ((Orientation(pane) == DwtOrientationVertical) ? (width) : (height))
 
#define FindWidth(length, breadth) \
    ((Orientation(pane) == DwtOrientationVertical) ? (breadth) : (length))
#define FindHeight(length, breadth) \
    ((Orientation(pane) == DwtOrientationVertical) ? (length) : (breadth))
 
#define FindStartLoc(x, y) \
    ((Orientation(pane) == DwtOrientationVertical) ? (y) : (x))
 
/* Use direct method for Length, safe because core fields guaranteed
 * to be a same offset */
#define Length(widget)	FindLength(widget->core.width, widget->core.height)
#define Breadth(widget)	FindBreadth(widget->core.width, widget->core.height)
#define StartLoc(widget)FindStartLoc(widget->core.x, widget->core.y)

#define EventToTime(evtTime, event)					  \
    switch ((event)->type) {						  \
        case KeyPress: 							  \
        case KeyRelease: 						  \
		(evtTime) = (event)->xkey.time; break;			  \
        case ButtonPress: 						  \
        case ButtonRelease: 						  \
 		(evtTime) = (event)->xbutton.time; break;		  \
	case MotionNotify: 						  \
		(evtTime) = (event)->xmotion.time; break;		  \
	case EnterNotify: 						  \
	case LeaveNotify: 						  \
		(evtTime) = (event)->xcrossing.time; break;		  \
	case PropertyNotify:						  \
		(evtTime) = (event)->xproperty.time; break;		  \
	case SelectionClear: 						  \
		(evtTime) = (event)->xselectionclear.time; break; 	  \
	case SelectionRequest: 						  \
		(evtTime) = (event)->xselectionrequest.time; break;	  \
	case SelectionNotify: 						  \
		(evtTime) = (event)->xselection.time; break;		  \
	default: 							  \
		(evtTime) = CurrentTime; break;				  \
    }
 
static Dimension	resource_min	    = 1;
static Dimension	resource_max	    = 10000;
static Dimension	resource_mullion    = 2;
static short		resource_spacing = 1;
static unsigned char	resource_vert = DwtOrientationVertical;
static Position 	resource_0_position 	= 0;
 
static DwtPartResource resources[] = {
 
    {	DwtNorientation, 			/* fixed -- jg */
	DwtCOrientation,
	XtROrientation, 
	sizeof(unsigned char),
	DwtPartOffset(Pane,orientation), 
	DwtROrientation, 
	(caddr_t) &resource_vert},
 
    {	DwtNmullionSize, 			/* fixed -- jg */
	DwtCMullionSize, 
	XtRDimension, 
	sizeof(Dimension),
	DwtPartOffset(Pane,mullionsize), 
	XtRDimension, 
	(caddr_t) &resource_mullion},
 
    {	DwtNforeground,  			/* fixed -- jg */
	DwtCForeground, 
	XtRPixel, 
	sizeof(Pixel),
	DwtPartOffset(Pane,foreground), 
	XtRString, 
	DwtSForegroundDefault},
 
    {	DwtNspacing,  			/* fixed -- jg */
	DwtCSpacing, 
	XtRShort, 
	sizeof(short),
	DwtPartOffset(Pane,intersub), 
	XtRShort, 
	(caddr_t) &resource_spacing},
 
    {	DwtNresize, 			/* fixed -- jg */
	DwtCResize, 
	DwtRResize, 
	sizeof(int),
	DwtPartOffset(Pane,resize_mode),
	XtRImmediate, 
	(caddr_t) DwtResizeGrowOnly},
 
    {	DwtNoverrideText,		/* fixed -- jg */
	DwtCOverrideText, 
	XtRBoolean, 
	sizeof(Boolean),
	DwtPartOffset(Pane,overridetext), 
	XtRImmediate, 
	(caddr_t) FALSE},
 
    {   DwtNmapCallback,
	XtCCallback, 
	XtRCallback, 
	sizeof(DwtCallbackPtr),
	DwtPartOffset(Pane,map_callback), 
	XtRCallback, 
	(caddr_t) NULL},	/* fixed -- jg */
 
    {   DwtNunmapCallback,
	XtCCallback, 
	XtRCallback, 
	sizeof(DwtCallbackPtr),
	DwtPartOffset(Pane,unmap_callback), 
	XtRCallback, 
	(caddr_t) NULL},
 
    {	DwtNfocusCallback,	/* fixed -- jg */
	XtCCallback, 
	XtRCallback, 
	sizeof(DwtCallbackPtr),
	DwtPartOffset(Pane,focus_callback), 
	XtRCallback, 
	(caddr_t) NULL},
 
    {	DwtNhelpCallback,	/* fixed -- jg */
	XtCCallback, 
	XtRCallback, 
	sizeof(DwtCallbackPtr),
	DwtPartOffset(Pane,helpcallback), 
	XtRCallback, 
	(caddr_t) NULL},
};
 
static XtResource constraints[] = {
    {	DwtNmin, 
	DwtCMin,
	XtRDimension, 
	sizeof(Dimension),
	XtOffset(Constraints,min), 
	XtRDimension, 
	(caddr_t) &resource_min},
 
    {	DwtNmax, 
	DwtCMax, 
	XtRDimension, 
	sizeof(Dimension),
	XtOffset(Constraints,max), 
	XtRDimension, 
	(caddr_t) &resource_max},
 
    {	DwtNposition,
	DwtCPosition,
	XtRPosition, 
	sizeof(Position),
	XtOffset(Constraints,position), 
	XtRPosition,
	(caddr_t) &resource_0_position},
 
    {	DwtNresizable,
	XtCBoolean, 
	XtRBoolean, 
	sizeof(Boolean),
	XtOffset(Constraints,resizable), 
	XtRImmediate, 
	(caddr_t) TRUE},
 
    {	DwtNsharedFlag,
	DwtCSharedFlag, 
	XtRInt, 
	sizeof(int),
	XtOffset(Constraints,sharedflag), 
	XtRImmediate, 
	(caddr_t) NoSharedPaneMask},
 
};
 
/*  0xff09 = Tab  */
static char *textbindings =
    "Shift<KeyPress>0xff09:	focus-prev()\n\
     <KeyPress>0xff09: 		focus-next()";
 
 
static XtTranslations compiledtextbindings = NULL;
    
static void LclUpdateCallback();
static void Help();
static void HandleMullionEvents();
static void HandleCancelMullionEvents();
static void GrabFocus();
static void FocusNext();
static void FocusPrev();
static Boolean AcceptFocus();
static XtGeometryResult GeometryManager();
static Boolean SetConstraintValues();
static void MullionInitialize();
static void ChangeManaged();
static void InsertChild();
static void DeleteChild();
 
static char DefaultMullionTranslation[] =
       "<EnterWindow>:			                mullion-enter()\n\
        <LeaveWindow>:					mullion-exit()\n\
        ~Shift ~Ctrl ~Mod1 ~@Help<Btn1Down>:            mullion-start()\n\
        ~Shift ~Ctrl ~Mod1 ~@Help Button1<PtrMoved>:    mullion-adjust()\n\
        ~Shift ~Ctrl ~Mod1 ~@Help <Btn1Up>:             mullion-end()\n\
        ~Shift ~Ctrl ~Mod1 ~@Help<Btn2Down>:            mullion-cancel()\n\
        ~Shift ~Ctrl ~Mod1 ~@Help<Btn3Down>:            mullion-cancel()\n\
        @Help<Btn1Up>:					Help()";
 
static char DefaultTranslation[] =
    "@Help<BtnDown>:    				Help()";
 
static XtActionsRec ActionsTable[] = 
    {
	{"mullion-enter",		(XtActionProc) HandleMullionEvents},
	{"mullion-exit",		(XtActionProc) HandleMullionEvents},
	{"mullion-start",		(XtActionProc) HandleMullionEvents},
	{"mullion-adjust",		(XtActionProc) HandleMullionEvents},
	{"mullion-end",		(XtActionProc) HandleMullionEvents},
	{"mullion-cancel",		(XtActionProc) HandleCancelMullionEvents},
	{"grab-focus",		(XtActionProc) GrabFocus},
	{"focus-next",		(XtActionProc) FocusNext},
	{"focus-prev",		(XtActionProc) FocusPrev},
    	{"Help",                	(XtActionProc) Help},
        {NULL, NULL}
    };
 
static Cursor VPaneCursor=NULL, HPaneCursor=NULL;

externaldef(panewidgetclassrec) PaneClassRec panewidgetclassrec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "Pane",
    /* widget_size        */	sizeof(PanePart),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initial */	NULL,
    /* class_inited       */	FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */	NULL,
    /* realize            */    Realize,
    /* actions            */    ActionsTable,
    /* num_actions	  */	XtNumber(ActionsTable),
    /* resources          */    (XtResource *) resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    NULL,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    AcceptFocus,
    /* version		  */	XtVersionDontCheck,
    /* callbacks          */    NULL,
    /* tm_table           */    DefaultTranslation,
    /* disp accelerator   */    NULL,
    /* extension	  */	NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child	  */	InsertChild,
    /* delete_child	  */	DeleteChild,
    /* extension	  */	NULL
  },{
/* constraint_class fields */
    /* constraint resource list            */ (XtResourceList) constraints,
    /* number of constraints in list       */ XtNumber(constraints),
    /* size of constraint record           */ sizeof(ConstraintsRec),
    /* constraint initialization           */ NULL,
    /* constraint destroy proc             */ NULL,
    /* constraint set_values proc          */ SetConstraintValues,
    /* extension			   */ NULL
  },{
    /* offsets		  */	NULL,
    /* mumble		  */	0	/* Make C compiler happy   */
  }
};
 
externaldef(panewidgetclass) PaneClass panewidgetclass = &panewidgetclassrec;
 
static struct _WidgetClassRec mullionClassRec = {
    /* superclass         */  (WidgetClass) &widgetClassRec,
    /* class name         */   "Mullion",
    /* size               */   sizeof(MullionRec),
    /* class initialize   */   NULL,
    /* class_part_initial */   NULL,
    /* class_inited       */   FALSE,
    /* initialize         */   MullionInitialize,
    /* initialize_hook    */   NULL,
    /* realize            */   XtInheritRealize,
    /* actions            */   ActionsTable,
    /* num_actions        */   XtNumber(ActionsTable),
    /* resources	  */   NULL,
    /* num_resources      */   0,
    /* xrm_class          */   NULLQUARK,
    /* compress_motion    */   TRUE,
    /* compress_exposure  */   TRUE,
    /* compress enterleave*/   TRUE,
    /* visible_interest   */   FALSE,
    /* destroy            */   NULL,
    /* resize             */   NULL,
    /* expose             */   NULL,
    /* set_values         */   NULL,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   NULL,
    /* get_values_hook    */   NULL,
    /* accept_focus       */   NULL,
    /* version		  */   XtVersionDontCheck,
    /* callbacks          */   NULL,
    /* tm_table           */   DefaultMullionTranslation,
    /* query_geometry	  */   XtInheritQueryGeometry,
    /* display_accelerator*/   NULL,
    /* extension	  */   NULL
};
 
static WidgetClass mullionWidgetClass = (WidgetClass) &mullionClassRec;
 
#define DataFromWidget(subwidget) (Constraints)subwidget->core.constraints
 
static void MullionInitialize(unused_request, w)
Widget unused_request, w;
{
    PaneWidget pane = (PaneWidget) w->core.parent;
    w->core.border_pixel = Foreground(pane);
    w->core.border_width = MullionSize(pane);
    w->core.width = FindWidth(InterSub(pane),Breadth(pane));
    w->core.height = FindHeight(InterSub(pane),Breadth(pane));
    w->core.x = w->core.y = -99;
}
 

/*
 * Figure out what length of the window we're actually using.  Ideally, this
 * will be the same as the window's length, but it will often be different
 * (for example, when we don't have enough windows to take up the entire
 * window's length.)
 */
 
static Dimension LengthUsed(pane)
PaneWidget pane;
{
    Dimension result;
    int i, cnt;
    Constraints subdata, nextsubdata;
    Boolean allfixed;

    if (PaneNumChildren(pane) == 0) return 0;
    result = 0;
    for (i=0 ; i<PaneNumChildren(pane); i++)
	{
	result += Length(PaneChildren(pane)[i]);
	subdata = DataFromWidget(PaneChildren(pane)[i]);
	if (i < PaneNumChildren(pane) - 1) 
	    { 
	    result += MullionLength(pane);
	    } 
	}
    return result;
}
 
 

/*
 * Set the dheight field of each subwidget to be the appropriate height if we
 * want to move the border below the given subwidget by the given number of
 * pixels.
 */
 
static void RefigureDlengths(pane, sub, delta)
PaneWidget pane;
Widget sub;
int delta;
{
    int cur_above, min_above, max_above;
    int cur_below, min_below, max_below;
    int used;
    Boolean currentlyabove;
    Widget subwidget;
    Constraints subdata;
    int i, which, temp, newlength;
    
    /*
     * if we allow resizing of the pane we will just reset the child 
     * between the min and the max if it isn't already.
     */
    if (ResizeMode(pane) != DwtResizeFixed) {
	for (i=0 ; i<PaneNumChildren(pane) ; i++) {
	    subwidget = PaneChildren(pane)[i];
	    subdata = DataFromWidget(subwidget);
	    /*
	     * This only looks tough. If the ResizeMode(pane) == GrowOnly and
	     * the length is less than 0 then DONT reset the dlength to be the
	     * new length. Otherwise make the newlength the dlength.
	     */
	    if (ResizeMode(pane) != DwtResizeGrowOnly &&
		    Length(subwidget) > subdata->dlength)
			subdata->dlength = Length(subwidget);
	    /* if the subwidget cannot be resized set the min and max to
	     * the created length of the widget.
	     */ 
	    if (subdata->resizable)
		{
		if (subdata->dlength < subdata->min)
		    subdata->dlength = subdata->min;
		if (subdata->dlength >subdata->max)
		    subdata->dlength = subdata->max;
		}
	    else
		{
		subdata->min = subdata->max = subdata->dlength;
		}
	}/* end for */
	return;
    }
    /* Ok no resizing allowed so we have to be a little more particular
     */
    cur_above = min_above = max_above = 0;
    cur_below = min_below = max_below = 0;
    currentlyabove = TRUE;
    /* Find the length above & including the changed widget & the length
     * below this widget. Total above & below on current, maximium and
     * minimium
     */
    for (i=0 ; i<PaneNumChildren(pane) ; i++) {
	subwidget = PaneChildren(pane)[i];
	subdata = DataFromWidget(subwidget);
	if (currentlyabove) {
	    cur_above += Length(subwidget);
	    min_above += subdata->min;
	    max_above += subdata->max;
	} else {
	    cur_below += Length(subwidget);
	    min_below += subdata->min;
	    max_below += subdata->max;
	}
	if (subwidget == sub) {
	    which = i;
	    currentlyabove = FALSE;
	}
    } /* end for loop */
    if (currentlyabove)
	XtWarning("Never found the subwidget in Pane.");
    used = LengthUsed(pane);
    if (used < Length(pane)) min_below -= Length(pane) - used;
    else max_below += used - Length(pane);
    if (delta < min_above - cur_above) delta = min_above - cur_above;
    if (delta < cur_below - max_below) delta = cur_below - max_below;
    if (delta > max_above - cur_above) delta = max_above - cur_above;
    if (delta > cur_below - min_below) delta = cur_below - min_below;
    temp = delta;
    /* a for loop is easier to read here. Also why continue if temp = 0?
     * And further more there is no good reason to reset temp.
     */
    for (i = which; i >= 0; i--)
	{
	subwidget = PaneChildren(pane)[i];
	subdata = DataFromWidget(subwidget);
	/* only make changes if we can resize this widget
	 */
	if (subdata->resizable)
	    {
	    newlength = Length(subwidget) + temp;
	    if (newlength < (int) subdata->min) newlength = subdata->min;
	    if (newlength > (int) subdata->max) newlength = subdata->max;
	    subdata->dlength = newlength;
	    }
	else
	    {
	    subdata->min = subdata->max = subdata->dlength = Length(subwidget);
	    }
	temp -= (subdata->dlength - Length(subwidget));
	}
    /* take what ever was gained on the top and put it on the bottom
     */
    temp = -delta;
    for (i = which+1; i < PaneNumChildren(pane); i++)
	{
	subwidget = PaneChildren(pane)[i];
	subdata = DataFromWidget(subwidget);
	/* only make changes if we can resize this widget
	 */
	if (subdata->resizable)
	    {
	    newlength = Length(subwidget) + temp;
	    if (newlength < (int) subdata->min) newlength = subdata->min;
	    if (newlength > (int) subdata->max) newlength = subdata->max;
	    subdata->dlength = newlength;
	    }
	else
	    {
	    subdata->min = subdata->max = subdata->dlength = Length(subwidget);
	    }
	temp -= (subdata->dlength - Length(subwidget));
	}
}
 
 

static void TryToFillPane(pane)
PaneWidget pane;
{
    int used, length;
    length = Length(pane);
    used = LengthUsed(pane);
    if (PaneNumChildren(pane) > 0)
	RefigureDlengths(pane, PaneChildren(pane)[PaneNumChildren(pane)-1],
			 length - used);
}
 

static void CommitNewSizes(pane, dontnotify)
PaneWidget pane;
Widget dontnotify;
{
    XWindowChanges changes;
    Position loc;
    int i, cnt; 
    Widget subwidget, nextsubwidget, mullion;
    Constraints subdata, nextsubdata;
    Boolean allfixed;
    loc = 0;
    for (i=0 ; i<PaneNumChildren(pane) ; i++) 
	{
	/* Do the subwidget first
	 */
	subwidget = PaneChildren(pane)[i];
	subdata = DataFromWidget(subwidget);
	/* depending on the orientation we'll change the subwidgets
	 * x, width or y, height.
	 */
	changes.x = FindWidth(loc, 0);
	changes.y = FindHeight(loc, 0);
	changes.width = FindWidth(subdata->dlength, Breadth(pane));
	changes.height = FindHeight(subdata->dlength, Breadth(pane));
	changes.border_width = 0;
	if (changes.x != subwidget->core.x || changes.y != subwidget->core.y ||
	      changes.width != subwidget->core.width ||
	      changes.height != subwidget->core.height ||
	      changes.border_width != subwidget->core.border_width) 
	    {
	    subwidget->core.x = changes.x;
	    subwidget->core.y = changes.y;
	    subwidget->core.width = changes.width;
	    subwidget->core.height = changes.height;
	    subwidget->core.border_width = changes.border_width;
	    if (XtIsRealized(subwidget))    
		{
		XConfigureWindow(XtDisplay(subwidget), XtWindow(subwidget),
		    (unsigned) CWX |CWY | CWWidth | CWHeight | CWBorderWidth,
		    &changes);
		}
	    if (subwidget != dontnotify &&
		 XtClass(subwidget)->core_class.resize != (XtWidgetProc) NULL)
		{
		(*(subwidget->core.widget_class->core_class.resize))(subwidget);
		}
	    }
	loc += Length(subwidget);

	/* Now position the mullion 
	 */
	mullion = subdata->mullion;
 
	/* Get any new width and height
	 */
	changes.width = FindWidth(InterSub(pane),Breadth(pane));
	changes.height = FindHeight(InterSub(pane),Breadth(pane));
	changes.border_width = MullionSize(pane);
 
	/* Move this mullion where they can't get to it if
	 * the subwidget is the last child
	 */
	if (i < PaneNumChildren(pane) - 1) 
	    { 
	    changes.x = FindWidth(loc, 0);
	    changes.y = FindHeight(loc, 0);
	    loc += MullionLength(pane);
	    } 
	else 
	    {
    	    changes.x = changes.y = -99;
	    loc += InterSub(pane);
	    }
	    mullion->core.x = changes.x;
	    mullion->core.y = changes.y;
	    mullion->core.width = changes.width;
	    mullion->core.height = changes.height;
	    mullion->core.border_width = changes.border_width;
	    if (XtIsRealized(mullion))
		{
		XConfigureWindow(XtDisplay(mullion), XtWindow(mullion),
		    (unsigned) CWX | CWY | CWWidth | CWHeight | CWBorderWidth,
		    &changes);
		XRaiseWindow(XtDisplay(mullion), XtWindow(mullion));
		}
    }
}
 
 

/*
 * Invert the given border.
 */
 
static void InvertIt(pane, loc, mullionlength)
PaneWidget pane;
Position loc;
Dimension mullionlength;
{
    loc += mullionlength / 2 + 1;   /* Deal with X's definition that wide */
				    /* lines are centered around the */
				    /* specified points.  */
 
    XDrawLine(XtDisplay(pane), XtWindow(pane), InvGC(pane),
	      FindWidth(loc, 0), FindHeight(loc, 0),
	      FindWidth(loc, Breadth(pane)), FindHeight(loc, Breadth(pane)));
}
 
 

/*
 * Move the magic borders to the location specified by the current dlength
 * values. 
 */
 
static void MoveMagicBorders(pane)
PaneWidget pane;
{
    Position loc;
    int i, cnt;
    Constraints subdata, nextsubdata;
    Boolean allfixed;

    loc = 0;
    for (i=0 ; i<PaneNumChildren(pane) ; i++)
	{
	subdata = DataFromWidget(PaneChildren(pane)[i]);
	loc += subdata->dlength;
	if (loc != subdata->magicborder) 
	    {
	    if (PaneChildren(pane)[i] == WhichAdjust(pane))
		{
		if (subdata->magicborder != OrigLoc(pane)) 
		    InvertIt(pane, subdata->magicborder, MullionLength(pane));
		if (loc != OrigLoc(pane)) 
		    InvertIt(pane, loc, MullionLength(pane)); 
		}
	    else
		{
		if (subdata->magicborder != 
		   (StartLoc(PaneChildren(pane)[i]) + Length(PaneChildren(pane)[i])))
		    InvertIt(pane, subdata->magicborder, MullionLength(pane));
		if (loc != 
		   (StartLoc(PaneChildren(pane)[i]) + Length(PaneChildren(pane)[i])))
		    InvertIt(pane, loc, MullionLength(pane));
		}
	    subdata->magicborder = loc;
	    }
	if (i < PaneNumChildren(pane) - 1) 
	    {
	    loc += MullionLength(pane);
	    }
	else 
	    {
	    loc += InterSub(pane);
	    }
	}
}
 

/*
 * Turn on the magic borders.
 */
 
static void DrawMagicBorders(pane)
PaneWidget pane;
{
    Position loc;
    int i, cnt;
    Constraints subdata, nextsubdata;
    Boolean allfixed;

    loc = 0;
    for (i=0 ; i<PaneNumChildren(pane) ; i++) {
	subdata = DataFromWidget(PaneChildren(pane)[i]);
	loc += subdata->dlength;
	subdata->magicborder = loc;
	if (i < PaneNumChildren(pane) - 1) 
	    { 
	    loc += MullionLength(pane);
	    } 
	else 
	    {
	    loc += InterSub(pane);
	    }
    }
}
 

/*
 * Turn off the magic borders.
 */
 
static void EraseMagicBorders(pane)
PaneWidget pane;
{
    int		i;
    Constraints subdata;
    for (i=0 ; i<PaneNumChildren(pane) ; i++) 
	{
	if (PaneChildren(pane)[i] == WhichAdjust(pane))
	    {
	    subdata = DataFromWidget(PaneChildren(pane)[i]);
	    if (subdata->magicborder != OrigLoc(pane))
		InvertIt(pane, subdata->magicborder, MullionLength(pane));
	    }
	else
	    {
	    subdata = DataFromWidget(PaneChildren(pane)[i]);
	    if (subdata->magicborder != 
	       (StartLoc(PaneChildren(pane)[i]) + Length(PaneChildren(pane)[i])))
		InvertIt(pane, subdata->magicborder, MullionLength(pane));
	    }
	}
    XClearWindow(XtDisplay(pane), XtWindow(pane));
}
 
 

/*
 * Find where (lengthwise) in the pane window the given event is pointing.
 * (The event was delivered to the given widget.)
 */
 
static Position FindWherePointing(pane, widget, event)
PaneWidget pane;
Widget widget;
XEvent *event;
{
    Position x, y;
    x = event->xbutton.x;
    y = event->xbutton.y;
    while (widget != (Widget) pane) {
	x += widget->core.x;
	y += widget->core.y;
	widget = widget->core.parent;
    }
    return FindLength(x, y);
}
 
 

/*
 * Given that the button down event was delivered to the mullion,
 * figure out which window is above the border that the user is trying to
 * adjust.  
 */
 
static Widget FindWhichAdjust(pane, widget, event)
PaneWidget pane;
Widget widget;
XEvent *event;
{
    Widget result;
    Position where, loc;
    int i;
    Constraints subdata;

    where = FindWherePointing(pane, widget, event);
    loc = MullionLength(pane)/2 + 1;
    result = PaneChildren(pane)[0];
    for (i=0 ; i<PaneNumChildren(pane) ; i++) {
	widget = PaneChildren(pane)[i];
	if (loc < where) result = widget;
	    else break;
	subdata = DataFromWidget(widget);
	loc += Length(widget) + MullionLength(pane);
    }
    return result;
}
 

/*
 * Handle button events in the mullion window.
 */
 
static void HandleMullionEvents(widget, event)
Widget widget;
XEvent *event;
{
    PaneWidget pane = (PaneWidget) widget->core.parent;
    if (PaneNumChildren(pane) == 0) return;
    switch (event->type) {
      case EnterNotify:
	if (Orientation(pane) == DwtOrientationVertical)
	    {
	    XDefineCursor (XtDisplay(widget->core.parent), XtWindow(widget->core.parent), VPaneCursor);
	    }
	else
	    {
	    XDefineCursor (XtDisplay(widget->core.parent), XtWindow(widget->core.parent), HPaneCursor);
	    }
	LeftMullion(pane) = False;
	break;
      case LeaveNotify:
	if (WhichAdjust(pane) == NULL)
	    {
	    XUndefineCursor (XtDisplay(widget->core.parent), XtWindow(widget->core.parent));
	    }
	else
	    {
	    LeftMullion(pane) = True;
	    }
	break;
      case ButtonPress:
	WhichAdjust(pane) = FindWhichAdjust(pane, widget, event);
/*
 * The OrigLoc is the Origicnal location of the dividing line between two panes. It is
 * calculated by adding the starting location (x or y depending on orientation) to
 * the length (width or height depending on orientation) 
 */
	OrigLoc(pane) = StartLoc(WhichAdjust(pane)) + Length(WhichAdjust(pane));
	RefigureDlengths(pane, WhichAdjust(pane), 0);
	DrawMagicBorders(pane);
	break;
      case MotionNotify:
	if (WhichAdjust(pane) == NULL) break;
	RefigureDlengths(pane, WhichAdjust(pane),
			 FindWherePointing(pane, widget, event) -
			     OrigLoc(pane));
	MoveMagicBorders(pane);
	break;
      case ButtonRelease:
/* 
 * make sure the operation wasn't canceled 
*/
	if (WhichAdjust(pane) != NULL) 
	    {
	    RefigureDlengths(pane, WhichAdjust(pane),
			     FindWherePointing(pane, widget, event) -
				 OrigLoc(pane));
	    MoveMagicBorders(pane);
	    EraseMagicBorders(pane);
	    CommitNewSizes(pane, (Widget) NULL);
	    WhichAdjust(pane) = NULL;
	    }
	if (LeftMullion(pane))
	    {
	    XUndefineCursor (XtDisplay(widget->core.parent), XtWindow(widget->core.parent));
	    LeftMullion(pane) = False;
	    }
	break;
    }
}
 
/*
 * Handle cancel (MB2 & MB3 press) button events in the mullion window.
 */
 
static void HandleCancelMullionEvents(widget)
Widget widget;
{
    PaneWidget pane = (PaneWidget) widget->core.parent;
    if (PaneNumChildren(pane) == 0) return;
    if (LeftMullion(pane))
	{
	EraseMagicBorders(pane);
	XUndefineCursor (XtDisplay(widget->core.parent), XtWindow(widget->core.parent));
	LeftMullion(pane) = False;
	}
    WhichAdjust(pane) = NULL;
}

static void ClassInitialize(unused_w)
Widget unused_w;
{
    DwtResolvePartOffsets(panewidgetclass,
		 &panewidgetclassrec.pane_class.paneoffsets);
    compiledtextbindings = XtParseTranslationTable (textbindings);
}
    
 
 
 

/*
 * Initialize this instance of pane.
 */
 
static void Initialize(unused_request, w)
Widget unused_request, w;
{
    PaneWidget pane = (PaneWidget) w;
    XGCValues values;
 
/*
**  Create the pane cursors...
*/
    if (VPaneCursor == NULL)
	{
    /*
     *  Local data for the pane cursor
     *	Note: Normally one would do this sort of thing in ClassInitialize but,
     *	setting up cursors requires a live widget which ClassInit doesn't have.
     */
	XColor cursor_fore, cursor_back;
	Font   cursor_font;
	int    cursor_wait;
 
    /*
    **  Set up the colors
    */
	cursor_fore.pixel = 0;
	cursor_fore.red   = 65535;
	cursor_fore.green = 65535;
	cursor_fore.blue  = 65535;
 
	cursor_back.pixel = 0;
	cursor_back.red   = 0;
	cursor_back.green = 0;
	cursor_back.blue  = 0;
 
	cursor_font = XLoadFont (XtDisplay(w), "decw$cursor");
	cursor_wait = decw$c_vpane_cursor;
	VPaneCursor = XCreateGlyphCursor (XtDisplay(w), cursor_font, cursor_font, cursor_wait, 
						     cursor_wait + 1, &cursor_fore, &cursor_back);
	cursor_wait = decw$c_hpane_cursor;
	HPaneCursor = XCreateGlyphCursor (XtDisplay(w), cursor_font, cursor_font, cursor_wait, 
						     cursor_wait + 1, &cursor_fore, &cursor_back);
	XUnloadFont (XtDisplay(w), cursor_font);
	}
 
    if (Width(pane) == 0) Width(pane) = 200; /* Better default?%%% */
    if (Height(pane) == 0) Height(pane) = 200;
    PaneNumChildren(pane) = 0;
    PaneChildren(pane) = NULL;
    WhichAdjust(pane) = NULL;
    OrigLoc(pane) = 0;
    LastHadFocus(pane) = NULL;
    MullionLength(pane) = (MullionSize(pane) * 2) + InterSub(pane);
    values.function = GXinvert;
    values.foreground = BackgroundPixel(pane) ^ Foreground(pane);
    if (values.foreground == 0) values.foreground = 1;
    values.line_width = InterSub(pane);
    if (values.line_width == 1)
	values.line_width = 0;	/* Take advantage of fast server lines. */
    values.plane_mask = BackgroundPixel(pane) ^ Foreground(pane);
    values.fill_style = FillSolid;
    values.subwindow_mode = IncludeInferiors;
    InvGC(pane) = XtGetGC(w,
			       (XtGCMask) GCFunction | GCForeground
			       | GCLineWidth | GCPlaneMask | GCFillStyle
			       | GCSubwindowMode,
			       &values);
}
 

/*
 * Realize the pane widget.
 */
 
static void Realize(w, vMask, attributes)
Widget w;
Mask *vMask;
XSetWindowAttributes *attributes;
{
    PaneWidget pane = (PaneWidget) w;
    Mask valueMask = *vMask;
 
    if (ResizeMode(pane) != DwtResizeFixed) {
/*
 * For now the ResizeMode is forced to DwtResizeFixed so that you can move the
 * mullion once you get this beast up. If the user wants to change the pane widget
 * back he should call DwtPaneAllowResizing, do the work in adjusting the widget
 * and then reset the value back.
 * 
 * This needs to be changed and modeled after the DIALOG widget. See the 
 * adapt_to_kids routine.
 */
	ResizeMode(pane) = DwtResizeFixed;
	TryToFillPane(pane);
	CommitNewSizes(pane, (Widget) NULL);
    }
    
    attributes->bit_gravity = NorthWestGravity;
    attributes->background_pixel = Foreground(pane);
    valueMask |= CWBitGravity | CWBackPixel;
    
    XtCreateWindow(w, (unsigned int) InputOutput, (Visual *) CopyFromParent,
		   valueMask, attributes);
}
 

/*
 * Destroy the pane widget.
 */
 
static void Destroy(w)
Widget w;
{
    PaneWidget pane = (PaneWidget) w;
 
/* Free the memory allocated for the children that are displayed and those in 
 * the composite field. We created and allocated space for them it so we
 * must Free it.
 */
    XtFree((char *)Children(pane));
    XtFree((char *)PaneChildren(pane));
 
/*
 * Remove all callbacks that were created
 */
    XtRemoveAllCallbacks ((Widget)pane, DwtNhelpCallback);
    XtRemoveAllCallbacks ((Widget)pane, DwtNfocusCallback);
    XtRemoveAllCallbacks ((Widget)pane, DwtNmapCallback);
    XtRemoveAllCallbacks ((Widget)pane, DwtNunmapCallback);
/*
 *  Free the InvGC that was created
 */
    XtDestroyGC (InvGC(pane));
}
 

/*
 * The purpose of this routine is to check and see in the Vertical 
 * orientation if a change in width will also change a subwidgets height
 */
 
static void CheckHeightChanges(pane)
    PaneWidget pane;
{
    XtWidgetGeometry intended, reply;
    Widget subwidget;
    Constraints subdata;
    int i;
 
    if (Orientation(pane) != DwtOrientationVertical) return; 
 
    intended.request_mode = CWWidth;		/* ask about this width */
 
    for (i=0 ; i<PaneNumChildren(pane); i++) 
	{
	subwidget = PaneChildren(pane)[i];
	subdata = DataFromWidget(subwidget);
	if (!subdata->resizable)
	    {
	    intended.width = Width(pane);   /* if it affects the size */
					    /* he'll tell us */
 
	    switch (XtQueryGeometry (subwidget, &intended, &reply))
		{
		case XtGeometryAlmost:		/* he wants to compromise */
 
		    if ((reply.request_mode & CWHeight) &&
			    subwidget->core.height != reply.height)
			{
			/* take height he suggests */
			subwidget->core.height = reply.height;	
			RefigureDlengths(pane,subwidget,subdata->dlength -
					    reply.height);
			}
		    break;
 
		case XtGeometryYes:
		case XtGeometryNo:
			/* he agrees, no problem w/ */
			/* current height */
		    break;
		} /* end case */
	    } /* end if */
	} /* end for loop */
}
 

/*
 * The pane widget has been resized; handle everything.
 */
 
static void Resize(w)
Widget w;
{
    PaneWidget pane = (PaneWidget) w;
    CheckHeightChanges(pane);
    TryToFillPane(pane);
    CommitNewSizes(pane, (Widget) NULL);
}
 

/*
 * Resources in the pane widget have changed; handle it.
 */
 
static Boolean SetValues (old, unused_request, new)
    Widget old, unused_request, new;
{
    PaneWidget	oldpane = (PaneWidget) old;
    PaneWidget	newpane = (PaneWidget) new;
    Boolean	redisplay = FALSE;
    XGCValues	values;
 
/*
 * Update the callbacks
 */
    LclUpdateCallback (	    &(oldpane->pane.map_callback),
			 (Widget) newpane, &(newpane->pane.map_callback), DwtNmapCallback);
    LclUpdateCallback (	    &(oldpane->pane.unmap_callback),
			 (Widget) newpane, &(newpane->pane.unmap_callback), DwtNunmapCallback);
    LclUpdateCallback (	    &(oldpane->pane.helpcallback),
			 (Widget) newpane, &(newpane->pane.helpcallback), DwtNhelpCallback);
    LclUpdateCallback (	    &(oldpane->pane.focus_callback),
			 (Widget) newpane, &(newpane->pane.focus_callback), DwtNfocusCallback);
 
/*
 * For now ignore orientation changes
 */
    if (Orientation(newpane) != Orientation(oldpane))
	{
	Orientation(newpane) = Orientation(oldpane);
	}
 
/*
 * For now ignore resize mode change
 */
    if (ResizeMode(newpane) != ResizeMode(oldpane))
	{
	ResizeMode(newpane) = ResizeMode(oldpane);
	}
/*
 * Something should be done with foreground here but I don't think that works
 * anyway so wait till you fix it latter
 */
 
/*
 * Check the mullion and spacing for changes. If so set redisplay
 */
    if ((MullionSize(newpane) != MullionSize(oldpane)) ||
	(InterSub(newpane) != InterSub(oldpane)))
	    {
/*
 * Change the inverted lines width to InterSub new length
 */
	    if (InterSub(newpane) != InterSub(oldpane))
		{
		values.line_width = InterSub(newpane);
		if (values.line_width == 1)
		    values.line_width = 0;	/* Take advantage of fast server lines. */
		XChangeGC (XtDisplay(newpane),InvGC(newpane), GCLineWidth, &values);
		}
#ifdef bugfixed
/*
 * Currently there is a bug in the XConfirgureWindow which prevents reseting of
 * the border width. In the mean time don't allow any change in MullionSize.
 */
	    MullionLength(newpane) = (MullionSize(newpane) * 2) + InterSub(newpane);
#else
	    MullionSize(newpane) = MullionSize(oldpane);
#endif
/* 
 * if the mullions need to be changed in width and or height they will be changed
 * in the CommitNewSizes routine. 
 */
	    redisplay = TRUE;
	    TryToFillPane(newpane);
	    CommitNewSizes(newpane, (Widget) NULL);
	    }
 
    return redisplay;
}	

/*
 * SetValue on the callback is interpreted as replacing
 * all callbacks
 */

static void
LclUpdateCallback (rstruct, s, sstruct, argname)
    DwtCallbackStructPtr rstruct;		/* the real callback list */
    Widget s;					/* the scratch widget*/
    DwtCallbackStructPtr sstruct;		/* the scratch callback list */
    char           *argname;
{
    DwtCallbackPtr list;

    /*
     * if a new callback has been specified in the scratch widget,
     * remove and deallocate old callback and init new 
     */
    if (rstruct->ecallback != sstruct->ecallback)
    {
	list = (DwtCallbackPtr)sstruct->ecallback;
	/*
	 *  Copy the old callback list into the new widget, since
	 *  XtRemoveCallbacks needs the "real" widget
    	 */
        *sstruct = *rstruct;
	XtRemoveAllCallbacks(s, argname);
	sstruct->ecallback = NULL;
	XtAddCallbacks(s, argname, list);
    }
}
 
 
 

static Boolean SetConstraintValues(current, unused_widget, newWidget)
    Widget   current, unused_widget, newWidget;
{
    Constraints cur, new;
    Cardinal	position;
    Dimension	min, max;
    Boolean	resizable;
    Cardinal	sharedflag;
    Boolean	do_add_widget = FALSE;
 
    cur = DataFromWidget(current);
    new = DataFromWidget(newWidget);
    position = new->position;
    min = new->min;
    max = new->max;
    resizable = new->resizable;
    sharedflag = new->sharedflag;
 
    if (max == 0)
	max = resource_max;
    if (min == 0)
	min = resource_min;
/*
 * We have to reset all of these because the address of newWidget what is 
 * acutally stored in the pane widget. If you use the current widget you'll
 * stack dump.
 */
    if (position != cur->position)
	{
	do_add_widget = TRUE;
	new->position = cur->position;
	}
    if (min != cur->min)
	{
	do_add_widget = TRUE;
	new->min = cur->min;
	}
    if (max != cur->max)
	{
	do_add_widget = TRUE;
	new->max = cur->max;
	}
    if (resizable != cur->resizable)
	{
	do_add_widget = TRUE;
	new->resizable = cur->resizable;
	}
    if (sharedflag != cur->sharedflag)
	{
	do_add_widget = TRUE;
	new->sharedflag = cur->sharedflag;
	}
 
/* 
 * This is just like DwtPaneAddWidget so I'll just call it and 
 * let it take care of everything.
 */
    if (do_add_widget)
	{
	DwtPaneAddWidget (newWidget, position, min, max, resizable, sharedflag);
	/* We must reset all of these because it is possible they have changed
	 * and if we don't change them they will just get changed back to what
	 * they were when we leave this routine.
	 */
	current->core.x = newWidget->core.x;
	current->core.y = newWidget->core.y;
	current->core.width = newWidget->core.width;
	current->core.height = newWidget->core.height;
	}
    return TRUE;
}
 

/*
 * One of our subwidgets is trying to resize itself. For now we will only
 * deal with DwtResizeFixed. Anything else won't work right.
 */
 
static XtGeometryResult GeometryManager(subwidget, request, reply)
Widget subwidget;
XtWidgetGeometry *request, *reply;
{
    XtGeometryResult result;
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
    Dimension rlength, rbreadth;

    if ((request->request_mode & CWWidth) == 0)
	request->width = subwidget->core.width;
    if ((request->request_mode & CWHeight) == 0)
	request->height = subwidget->core.height;
    rlength = FindLength(request->width, request->height);
    rbreadth = FindBreadth(request->width, request->height);
    subdata = DataFromWidget(subwidget);
    result = XtGeometryAlmost;
    if (rlength < subdata->min) 
	{
	rlength = subdata->min;
	} 
    else if (rlength > subdata->max) 
	{
	rlength = subdata->max;
	};
    if (rbreadth > Breadth(pane))
	{
	    result = XtGeometryNo;
	    rbreadth = Breadth(pane);
	};
    if (rbreadth == Breadth(pane)) 
	{
	if (subdata->dlength == rlength)
	    {
	    /*
	     * I know this seams bizare since everthing looks like it is 
	     * already set. However the Text widget does things in a funny
	     * order and it resets the height back so we must redo the 
	     * CommitNewSizes to get the widget back in sinc with the
	     * constraint.
	     */
	    CommitNewSizes(pane, (Widget) NULL); 
	    result = XtGeometryDone;
	    }
	else
	    {
	    RefigureDlengths(pane, subwidget, rlength - subdata->dlength);
	    CommitNewSizes(pane, (Widget) NULL); 
	    if (subdata->dlength == FindLength(request->width, request->height)) 
		result = XtGeometryDone;
	    };
	}
    if (result != XtGeometryDone) 
	{
	*reply = *request;
	rbreadth = Breadth(pane);
	reply->width = FindWidth(rlength, rbreadth);
	reply->height = FindHeight(rlength, rbreadth);
	}
    return result;
}
 
 
 

/*
 * One of our subwidgets has changed its managed status.
 */
 
static void ChangeManaged(w)
Widget w;
{
    XWindowChanges changes;
    PaneWidget pane = (PaneWidget) w;
    Widget subwidget, cursubwidget;
    Constraints subdata, cursubdata;
    int position, i;
 
    PaneNumChildren(pane) = 0;
    for (i=(NumChildren(pane)+1)/2 ; i<NumChildren(pane); i++) 
	{
	subwidget = Children(pane)[i];
	if (XtClass (subwidget) == mullionWidgetClass)
	    {
	    continue;
	    }
	subdata = DataFromWidget(subwidget);
 
/* 
 * Recreate the PaneChildren list by only adding those subwidgets which are
 * managed and if shared, are in view. If a subwidget fails the test
 * move his mullion into oblivion. Like the rest of the mullions that don't need 
 * to be seen.
 */
	if (subwidget->core.managed && 
		(subdata->sharedflag == NoSharedPaneMask ||
		subdata->sharedflag == (SharedPaneMask | ViewableInPaneMask)))
	    {
	    if (subdata->sharedflag & SharedPaneMask)
		{
		XtSetMappedWhenManaged (subwidget, TRUE);
		}
	    /*
	     * NOTE:The only reason we do the realizing here is because if
	     *	    it isn't realized the mullions don't show up on top. Once
	     *	    mullions are removed you can get rid of this if statement
	     */
	    if (XtIsRealized(pane) && !(XtIsRealized(subwidget)))
		{
		XtRealizeWidget(subwidget);
		};
	    PaneChildren(pane)[(PaneNumChildren(pane))++] = subwidget;
	    }
	else if (subwidget->core.being_destroyed)
	    {
	/*
	 * This subwidget is in the process of being destroyed.
	 * If this widget is shared then give the pane to first subwidget sharing 
	 * the same position.
	 */
	    for (position=(NumChildren(pane)/2)+subdata->position; 
		 position <= NumChildren(pane); 
		 position++)
		{
		cursubwidget = Children(pane) [position-1];
		cursubdata = DataFromWidget (cursubwidget);
		if (cursubdata->position != subdata->position ||
		    cursubwidget == subwidget)
		    continue;
		DwtPaneMakeViewable (cursubwidget);
	    /*
	     * It is necessary to add the subwidget to the PaneChildren list if
	     * we have already gone passed it. It will be in the correct order
	     * since PaneChildren are based on position.
	     */
		if (position < i)
		    PaneChildren(pane)[(PaneNumChildren(pane))++] = subwidget;
		break;
		}
	    }
	else
	    {
	    /*
	     * UnMap any shared panes that aren't used
	     */
	    if (subdata->sharedflag & SharedPaneMask)
		{
		XtSetMappedWhenManaged (subwidget, FALSE);
		XtSetMappedWhenManaged (subdata->mullion, FALSE);
		}
	    else
		{
		/*
		 * Move the mullion widget into oblivion
		 */
		changes.x = changes.y = -99;
		if (changes.x != subdata->mullion->core.x || 
			changes.y != subdata->mullion->core.y)
		    {
		    subdata->mullion->core.x = changes.x;
		    subdata->mullion->core.y = changes.y;
		    if (XtIsRealized(subdata->mullion))
			XConfigureWindow(XtDisplay(subdata->mullion), 
				XtWindow(subdata->mullion), CWX | CWY, &changes);
		    }
		}
	    }
    }
/*
 * Ok now you only have those children that are managed so fill the pane.
 */
    TryToFillPane(pane);
    CommitNewSizes(pane, (Widget) NULL);
}
 

static void RememberFocus(w, unused_tag, cb)
Widget w;
caddr_t unused_tag;
DwtAnyCallbackStruct * cb;
{
    DwtAnyCallbackStruct focus_cb;
    PaneWidget pane = (PaneWidget) XtParent(w);
    LastHadFocus(pane) = w;
 
    focus_cb.reason = DwtCRFocus;
    focus_cb.event = cb->event;
    XtCallCallbacks ((Widget)pane, DwtNfocusCallback, cb);
}
 
 

/*
 * A new subwidget has been added.
 */
static DwtCallback focus_cb[2] = {RememberFocus, NULL, NULL};
 
static void InsertChild(subwidget)
Widget subwidget;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Widget cursubwidget;
    Constraints subdata, cursubdata;
    int i, position;
 
 
    /* check for mullion class here since when we create the mullion this code
     * will get called again. Get the desired length from the actual length
     * and then create a mullion for this widget. Finally position the widget
     * where the user wants it
     */
    if (XtClass(subwidget) != mullionWidgetClass) 
	{
	subdata = (Constraints)subwidget->core.constraints;
	subdata->dlength = Length(subwidget);
	if (subdata->max == 0)
	    subdata->max = resource_max;
	if (subdata->min == 0)
	    subdata->min = resource_min;
	if (subdata->max < subdata->min) 
	    subdata->max = subdata->min;
 
	subdata->mullion = XtCreateWidget("mullion", (WidgetClass) mullionWidgetClass, (Widget) pane,
				       (ArgList) NULL, (Cardinal) 0);
	XtManageChild(subdata->mullion);
	if (subdata->position)
	    {
	    position = subdata->position + ((NumChildren(pane)+1)/2);
	    if (position > (NumChildren(pane)+1))
		{
		position = (NumChildren(pane)+1);
		}
	    }
	else
	    {
	    position = (NumChildren(pane)+1); 
	    }
	subdata->position = position - ((NumChildren(pane)+1)/2); 
	position--;
	} 
    else 
	{
	position = (NumChildren(pane)/2);
	}
    Children(pane) = 
        (WidgetList) XtRealloc((caddr_t) Children(pane),
    	(unsigned) (NumChildren(pane) + 1) * sizeof(Widget));
 
    /* Ripple children up one space from "position" */
    for (i = NumChildren(pane); i > position; i--)
	{
	if (XtClass (Children(pane)[i-1]) != mullionWidgetClass &&
	    XtClass (subwidget) != mullionWidgetClass)
	    {
	    cursubwidget = Children(pane)[i-1];
	    cursubdata = (Constraints)cursubwidget->core.constraints;
	    /* 
	     * Check to see if the position of the current subwidget is less
	     * than that of the subwidget we are creating. If it is then that
	     * means there are other subwidgets below this one which are also
	     * shared and I should exit at this point to store this new 
	     * subwidget in the list of children.
	     */
	    if (cursubdata->position < subdata->position)
		{
		break;
		}
	    else 
		{
		/*
		 * Check to see if the position of the current subwidget and
		 * the created subwidget are the same. If they are and 
		 * the new one is shared, then set the old one to
		 * shared as well. Also set the current subwidgets inview to
		 * false if the subwidget will be in view.
		 *
		 * Otherwise, if the subwidget isn't shared the new one just 
		 * wants the currents ones position so bump it up by one.
		 */
		if ((cursubdata->position == subdata->position) && 
			(subdata->sharedflag & SharedPaneMask))
		    {
		    if (subdata->sharedflag & ViewableInPaneMask) 
			{
			cursubdata->sharedflag = SharedPaneMask;
			}
		    else
			{
			/* 
			 * Only set those that aren't shared. Those that are 
			 * shared are already set and since the new one does
			 * not want to be viewable we will just keep what we
			 * have.
			 */
			if (cursubdata->sharedflag == NoSharedPaneMask)
			    {
			    cursubdata->sharedflag = SharedPaneMask | ViewableInPaneMask;
			    }
			}
		    }
		else if (subdata->sharedflag == NoSharedPaneMask)
		    {
		    cursubdata->position++;
		    }
		} /* end else */
	    } /* end if */
        Children(pane)[i] = Children(pane)[i-1];
	}
    Children(pane)[i] = subwidget;
    NumChildren(pane)++;
    PaneChildren(pane) = (WidgetList)
	XtRealloc((char *)PaneChildren(pane),
		  (Cardinal) NumChildren(pane) * sizeof(Widget));
 
    /* this allows text widgets tabs to work like normal 
     */
    if (OverrideText(pane) && XtIsSubclass(subwidget, textwidgetclass)) 
	{
	XtOverrideTranslations(subwidget, compiledtextbindings);
	if (LastHadFocus(pane) == NULL)
	    LastHadFocus(pane) = subwidget;
	}
    if (XtHasCallbacks(subwidget, DwtNfocusCallback) != XtCallbackNoList)
	XtAddCallbacks(subwidget, DwtNfocusCallback, focus_cb);
}
    
 

/*
 * A subwidget has been removed.
 */
 
static void DeleteChild(subwidget) 
Widget subwidget;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
    int i, position;
 
/*
 * Destroy will destroy mullion and the child. If it is a mullion being 
 * deleted skip the stuff about subdata and shared position.
 */
    if (XtClass(subwidget) != mullionWidgetClass) 
	{
	subdata = DataFromWidget(subwidget);
    /* 
     * destory the mullion 
     */
	XtDestroyWidget (subdata->mullion);
	}
/*
 * Ok now we need to remove this widget from the composite list 
 */
    for (position = 0; position < NumChildren(pane); position++) 
	{
        if (Children(pane) [position] == subwidget) 
	    {
            break;
	    }
	}
 
    if (position == NumChildren(pane)) return;
 
    /* Ripple children down one space from "position" */
    NumChildren(pane)--;
    for (i = position; i < NumChildren(pane); i++) 
	{
        Children(pane) [i] = Children(pane) [i+1];
	}
} 
 
 

/*---------------------------------------------------*/
/* this routine will be called from the widget's     */
/* main event handler via the translation manager.   */
/*---------------------------------------------------*/
 
static void Help(w, event)
Widget w;
XEvent *event;
{
    DwtAnyCallbackStruct temp;
 
    temp.reason = DwtCRHelpRequested;
    temp.event = event;
 
    if (XtClass(w) == (WidgetClass) panewidgetclass)
	{
    	XtCallCallbacks(w, DwtNhelpCallback, &temp);
	}
    else
	{
	XtCallCallbacks(XtParent(w), DwtNhelpCallback, &temp);
	};
}
 

/*
 *************************************************************************
 *
 * Public creation entry points
 *
 *************************************************************************
 */
/*
 * low level create entry point
 */
 
Widget DwtPaneCreate (p, name, al, ac)
    Widget  p;				/* parent widget */
    char    *name;			/* pane widget name */
    ArgList al;
    Cardinal ac;    
{
    return XtCreateWidget(name, (WidgetClass) panewidgetclass, p, al, ac);
}
 
#ifdef  VMS
Widget DWT$PANE_CREATE (p, name$dsc, al, ac)
    Widget			*p;		/* parent widget */
    struct dsc$descriptor_s    	*name$dsc;
    ArgList			al;
    int				*ac;    
{
    PaneWidget pane;
    char *name;
 
    name  = DwtDescToNull(name$dsc);        
    pane = (PaneWidget) XtCreateWidget(name, (WidgetClass) panewidgetclass, *p, al, *ac);
 
    XtFree(name);
    return (Widget) pane;
}
#endif  /* VMS */ 
    
/*
 * high level pane create routine
 */
 
Widget DwtHVPane(p, 
	      name, 
	      x, y, 
	      width, height,
	      orientation,
	      mapcallback, helpcallback)
 
    Widget         p;				/* parent widget */
    char          *name;			/* pane widget name */
    Position	   x, y;			/* location of pane widget */
    Dimension	   width, height;		/* size of pane widget */
    int		   orientation;			/* horizontal or vertical
    DwtCallbackPtr mapcallback;			/* callback for data struct */
    DwtCallbackPtr helpcallback;		/* help requested */
{
    Arg al[25];
    Cardinal ac = 0;
 
    /*
     * set up the parameters we are given
     */
 
    XtSetArg (al[ac], XtNname,		name);		ac++;
    XtSetArg (al[ac], XtNx,		x);		ac++;
    XtSetArg (al[ac], XtNy,		y);		ac++;
    XtSetArg (al[ac], XtNwidth,		width);		ac++;
    XtSetArg (al[ac], XtNheight,	height);	ac++;
    XtSetArg (al[ac], DwtNorientation,	orientation);	ac++;
 
 
    if (mapcallback != NULL) 
	{
	XtSetArg (al[ac], DwtNmapCallback,     mapcallback);
	ac++;
	}
 
    if (helpcallback != NULL) 
	{
	XtSetArg (al[ac], DwtNhelpCallback,    helpcallback);
	ac++;
	}
 
    return XtCreateWidget (name, (WidgetClass) panewidgetclass, p, al, ac);
}

#ifdef  VMS 
Widget DWT$H_V_PANE(p, 
	      name$dsc, 
	      x, y, 
	      width, height,
	      orientation,
	      mapcallback, helpcallback)
 
    Widget        *p;				/* parent widget */
    struct dsc$descriptor_s  	*name$dsc;	/* form widget name */
    Position	  *x, *y;			/* location of form widget */
    Dimension	  *width, *height;		/* size of the widget */
    int		  *orientation;			/* pane orientation */
    DwtCallbackPtr *mapcallback;		/* callback */
    DwtCallbackPtr *helpcallback;		/* help requested */
{
    PaneWidget pane;
    Arg al[25];
    int ac = 0;
    char    *name;
 
    name  = DwtDescToNull(name$dsc);
    /*
     * set up the parameters we are given
     */
 
    XtSetArg (al[ac], XtNname,	      name);	    ac++;
    XtSetArg (al[ac], XtNname,		name);		ac++;
    XtSetArg (al[ac], XtNx,		x);		ac++;
    XtSetArg (al[ac], XtNy,		y);		ac++;
    XtSetArg (al[ac], XtNwidth,		width);		ac++;
    XtSetArg (al[ac], XtNheight,	height);	ac++;
    XtSetArg (al[ac], DwtNorientation,	orientation);	ac++;
 
 
    if (mapcallback != NULL) 
	{
	XtSetArg (al[ac], DwtNmapCallback,     mapcallback);
	ac++;
	}
 
    if (helpcallback != NULL) 
	{
	XtSetArg (al[ac], DwtNhelpCallback,    helpcallback);
	ac++;
	}
 
    pane = (PaneWidget)XtCreateWidget (name, (WidgetClass) panewidgetclass, *p, al, ac);
 
    XtFree(name);
    return (Widget) pane;
}
#endif  /* VMS */ 
    
/*
 * high level pane add widget routine
 */
 
void DwtPaneAddWidget(subwidget, position, min, max, resizable, sharedflag)
    Widget	subwidget;
    Cardinal	position;
    Dimension	min, max;
    Boolean	resizable;
    Cardinal	sharedflag;
{
    XWindowChanges changes;
    PaneWidget	pane = (PaneWidget) subwidget->core.parent;
    Widget	cursubwidget;
    Constraints subdata, cursubdata, cursubdata2;
    int		i, curpos, lastsharedposition;
    Boolean	subnotfound=TRUE, subnotplaced=TRUE, cursubnotreset=TRUE;
 
/*
 * Get the position the window is currently in
 */
    subdata = DataFromWidget(subwidget);
    curpos = subdata->position;
 
/*
 * Compare that to the position the user desires and change the windows around
 */
    if (position > (NumChildren(pane)/2))
	position = (NumChildren(pane)/2);
    if (position != curpos && position != 0)
	{
	if (position < curpos)
	    {
	    for (i = NumChildren(pane); i >= position*2; i--)
		{
		cursubwidget = Children(pane)[i-1];
		cursubdata = DataFromWidget(cursubwidget);
		/* 
		 * Check to see if the position of the current subwidget is less
		 * than that of the subwidget. If it is then that
		 * means there are other subwidgets below this one which are also
		 * shared and I should exit at this point to store this new 
		 * subwidget in the list of children.
		 */
		if (cursubdata->position < position)
		    {
		    Children(pane)[i] = subwidget;
		    subdata->position = position;
		    break;
		    }
		/*
		 * if subwidget came from a shared position we need to make 
		 * sure that at least one of the old widgets that shared the 
		 * position will now be displayed
		 */
		if (cursubdata->position == subdata->position && 
			subdata->sharedflag & SharedPaneMask)
		    {
		    if (cursubnotreset)
			{
			cursubdata->sharedflag = SharedPaneMask | ViewableInPaneMask;
			cursubnotreset = FALSE;
			}
		    }
		/*
		 * move the subwidgets up one unless 
		 * the current subwidget is subwidget we are trying to change or
		 * the current subs position > the current position or
		 */
		if (cursubwidget == subwidget || subnotfound)
		    {
		    if (cursubwidget == subwidget)
			{
			subnotfound = FALSE;
			continue;
			}
		    }
		else
		    {
		    Children(pane)[i] = Children(pane)[i-1];
		    }
		/*
		 * Reposition the current subwidgets position as appropriate
		 */
		if (subnotfound)
		    {
		    /*
		     * This is a little tricky here. This will before you find
		     * the subwidget. If the subwidget was not formerly 
		     * shared and is not to be shared || the subwidget was formerly
		     * shared and will be shared at its new position we will not
		     * change the position. For the other two conditions we do.
		     * In one case it is necessary to subtract and the other add
		     * to the position
		     */
		    if ((subdata->sharedflag == NoSharedPaneMask) &&
			(sharedflag & SharedPaneMask))
			    cursubdata->position--;
		    else if ((subdata->sharedflag & SharedPaneMask) &&
			(sharedflag == NoSharedPaneMask))
			    cursubdata->position++;
		    }
		else
		    {
		    /*
		     * Only change the position if our subwidget wont be shared.
		     * Otherwise the position will still be occupied.
		     */
		    if (sharedflag == NoSharedPaneMask)
			cursubdata->position++;
		    }
		/*
		 * Check to see if the position of the current subwidget and
		 * the subwidget's new position are the same. If they are and 
		 * the new one is shared, then set the old one to
		 * shared as well. Also set the current subwidgets inview to
		 * false if the subwidget will be in view.
		 *
		 * Otherwise, if the subwidget isn't shared the new one just 
		 * wants the currents ones position so bump it up by one.
		 */
		if(cursubdata->position == position && 
			sharedflag & SharedPaneMask)
		    {
		    if (sharedflag & ViewableInPaneMask) 
			{
			cursubdata->sharedflag = SharedPaneMask;
			}
		    else
			{
			/* 
			 * Only set those that aren't shared. Those that are 
			 * shared are already set and since the new one does
			 * not want to be viewable we will just keep what we
			 * have.
			 */
			if (cursubdata->sharedflag == NoSharedPaneMask)
			    {
			    cursubdata->sharedflag = SharedPaneMask | ViewableInPaneMask;
			    }
			}
		    }
		}   /* end for loop */
	    }
	else	/* position > curpos */
	    {
	    for (i = (NumChildren(pane)/2)+curpos; i <= NumChildren(pane); i++)
		{
		cursubwidget = Children(pane)[i-1];
		cursubdata = DataFromWidget(cursubwidget);
		/*
		 * Even though you start at the current position it is possible
		 * for the position to be less than the current position due to
		 * shared panes.
		 */
		if (cursubdata->position < curpos || cursubwidget == subwidget
		    || (subnotfound))
		    {
		    if (cursubwidget == subwidget)
			    subnotfound = FALSE;
		    continue;
		    }
		/*
		 * if subwidget came from a shared position we need to make 
		 * sure that at least one of the old widgets that shared the 
		 * position will now be displayed
		 */
		if (cursubdata->position == subdata->position && 
			subdata->sharedflag & SharedPaneMask)
		    {
		    if (cursubnotreset)
			{
			cursubdata->sharedflag = SharedPaneMask | ViewableInPaneMask;
			cursubnotreset = FALSE;
			}
		    }
		/*
		 * move the subwidgets up one unless 
		 * the current subwidget is subwidget we are trying to change or
		 * the current subs position > the desired position or
		 * the current subs position = the desired postion and the sub
		 *   we were working with was previously shared.
		 */
		if (cursubdata->position > position ||
		     ((subdata->sharedflag & SharedPaneMask) && 
		       cursubdata->position == position))
		    {
		    /* 
		     * Place subwidget only once after you moved all subs
		     */
		    if (subnotplaced)
			{
			Children(pane)[i-2] = subwidget;
			subdata->position = position;
			subnotplaced=FALSE;
			}
		    }
		else
		    {
		    Children(pane)[i-2] = Children(pane)[i-1];
		    }
		/*
		 * Reposition the current subwidgets position as appropriate
		 */
		if (subnotplaced)
		    {
		    /*
		     * Only change the position if our subwidget wasn't shared.
		     * Otherwise the position will still be occupied.
		     */
		    if (subdata->sharedflag == NoSharedPaneMask)
			cursubdata->position--;
		    }
		else
		    {
		    /*
		     * This is a little tricky here. This will happen after you
		     * placed the subwidget. If the subwidget was not formerly 
		     * shared and is not to be shared || the subwidget was formerly
		     * shared and will be shared at its new position we will not
		     * change the position. For the other two conditions we do.
		     * In one case it is necessary to subtract and the other add
		     * to the position
		     */
		    if ((subdata->sharedflag == NoSharedPaneMask) &&
			(sharedflag & SharedPaneMask))
			    cursubdata->position--;
		    else if ((subdata->sharedflag & SharedPaneMask) &&
			(sharedflag == NoSharedPaneMask))
			    cursubdata->position++;
		    }
		/*
		 * Check to see if the position of the current subwidget and
		 * the subwidget's new position are the same. If they are and 
		 * the new one is shared, then set the old one to
		 * shared as well. Also set the current subwidgets inview to
		 * false if the subwidget will be in view.
		 *
		 * Otherwise, if the subwidget isn't shared the new one just 
		 * wants the currents ones position so bump it up by one.
		 */
		if(cursubdata->position == position && 
			sharedflag & SharedPaneMask)
		    {
		    if (sharedflag & ViewableInPaneMask) 
			{
			cursubdata->sharedflag = SharedPaneMask;
			}
		    else
			{
			/* 
			 * Only set those that aren't shared. Those that are 
			 * shared are already set and since the new one does
			 * not want to be viewable we will just keep what we
			 * have.
			 */
			if (cursubdata->sharedflag == NoSharedPaneMask)
			    {
			    cursubdata->sharedflag = SharedPaneMask | ViewableInPaneMask;
			    }
			}
		    }
		}   /* end for loop */
	    }			    
	/*
	 * Now that the position has been properly set we can take care of 
	 * the shared flag. 
	 */
	subdata->sharedflag = sharedflag;
	/*
	 * Just for grins lets loop back through and make sure that if a 
	 * subwidget says that it is shared it really is.
	 */
	for (i=NumChildren(pane)/2; i<NumChildren(pane); i++) 
	    {
	    cursubwidget = Children(pane)[i];
	    cursubdata = DataFromWidget(cursubwidget);
	    if (cursubdata->sharedflag & SharedPaneMask)
		{
		if (i+1 != NumChildren(pane))	/* Dont check passed bountry */
		    {
		    cursubdata2 = DataFromWidget(Children(pane)[i+1]);
		    if (cursubdata->position == cursubdata2->position)
			lastsharedposition = cursubdata->position;
		    }
		if (lastsharedposition != cursubdata->position)
		    {
		    cursubdata->sharedflag = NoSharedPaneMask;
		    XtSetMappedWhenManaged (cursubwidget, TRUE);
		    }
		}
	    }
	}
    else
	{
	/*
	 * There was no change in position so if the user wants this subwidget
	 * to be viewable lets do it for him.
	 */
	if (sharedflag & ViewableInPaneMask)
	    DwtPaneMakeViewable (subwidget);
	}
 
    subdata->resizable = resizable;
    if (max == 0)
	max = resource_max;
    if (min == 0)
	min = resource_min;
    if (max < min) 
	max = min;
    if (subdata->resizable)
	{
	subdata->min = min;
	subdata->max = max;
	}
    if (subwidget->core.managed)
	{
	PaneNumChildren(pane) = 0;
	for (i=NumChildren(pane)/2; i<NumChildren(pane); i++) 
	    {
	    cursubwidget = Children(pane)[i];
	    cursubdata = DataFromWidget(cursubwidget);
 
    /* 
     * Recreate the PaneChildren list by only adding those subwidgets which are
     * managed and if shared, are in view. If a subwidget fails the test
     * move his mullion into oblivion. Like the rest of the mullions that don't need 
     * to be seen.
     */
	    if (cursubwidget->core.managed && 
		    (cursubdata->sharedflag == NoSharedPaneMask ||
		    cursubdata->sharedflag == (SharedPaneMask | ViewableInPaneMask)))
		{
		if (cursubdata->sharedflag & SharedPaneMask)
		    {
		    XtSetMappedWhenManaged (cursubwidget, TRUE);
		    }
		PaneChildren(pane)[(PaneNumChildren(pane))++] = cursubwidget;
		}
	    else
		{
		/*
		 * UnMap any shared panes that aren't used
		 */
		if (cursubdata->sharedflag & SharedPaneMask)
		    {
		    XtSetMappedWhenManaged (cursubwidget, FALSE);
		    XtSetMappedWhenManaged (cursubdata->mullion, FALSE);
		    }
		else
		    {
		    /*
		     * Move the mullion widget into oblivion
		     */
		    changes.x = changes.y = -99;
		    if (changes.x != cursubdata->mullion->core.x || 
			    changes.y != cursubdata->mullion->core.y)
			{
			cursubdata->mullion->core.x = changes.x;
			cursubdata->mullion->core.y = changes.y;
			if (XtIsRealized(cursubdata->mullion))
			    XConfigureWindow(XtDisplay(cursubdata->mullion), 
				    XtWindow(cursubdata->mullion), CWX | CWY, &changes);
			}
		    }
		}
	    } /* end for loop */
	} /* end if subwidget is managed */
    /*
     * Ok now you only have those children that are managed so fill the pane.
     */
/*
 * Check to see if the dlength for the subwidget is greater than the max. If it
 * is then I must call RefigureDlengths instead of TryToFillPane. TryToFillPane
 * will use the current length which hasn't been change so it will pass a delta
 * change to RefigureDlengths of 0 which does zippo. Since I know the amount I
 * need to change I'll just call RefigureDlengths directly.
 */
	if (subdata->dlength > subdata->max)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - subdata->max);
	    }
	else if (subdata->dlength < subdata->min)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - subdata->min);
	    }
	else
	    {
	    TryToFillPane(pane);
	    }
	CommitNewSizes(pane, (Widget) NULL);
}

#ifdef  VMS 
void DWT$PANE_ADD_WIDGET(subwidget, 
	      position,
	      min, max, 
	      resizable,
	      sharedflag)
 
    Widget        *subwidget;			/* widget */
    Cardinal	  *position;			/* position of the subwidget */
    Dimension	  *min, *max;			/* min & max size of the widget */
    Boolean	  *resizable;			/* is the widget resizeable */
    Cardinal	  *sharedflag;			/* shared flag */
{
    DwtPaneAddWidget (*subwidget,*position,*min,*max,*resizable,*sharedflag);
}
#endif  /* VMS */ 
 
void DwtPaneGetMinMax(subwidget, min, max)
Widget subwidget;
Dimension *min, *max;
{
    Constraints subdata;
    subdata = DataFromWidget(subwidget);
    if (subdata) {
	*min = subdata->min;
	*max = subdata->max;
    } else
	XtWarning("DwtPaneGetMinMax called with bad widget.");
}

#ifdef  VMS 
void DWT$PANE_GET_MIN_MAX(subwidget, 
	      min, max)
    Widget        subwidget;			/* widget */
    Dimension	  *min, *max;			/* min & max size of the widget */
{
    Constraints subdata;
    subdata = DataFromWidget(subwidget);
    if (subdata) {
	*min = subdata->min;
	*max = subdata->max;
    } else
	XtWarning("DwtPaneGetMinMax called with bad widget.");
}
#endif  /* VMS */ 
 

/*
 * NOTE: In setting the min or max in DwtPaneSetMinMax, DwtPaneSetMin, or
 *	DwtPaneSetMax we donot check for valid min, max values. Values less
 *	than 1 will certianly cause stack dumps and min > max will also 
 *	cause some form of headache, though I haven't tried it yet. Most
 *	DW code doesn't check for valid attributes so I see no reason to
 *	do so here either.
 */
 
void DwtPaneSetMinMax(subwidget, min, max)
Widget subwidget;
Dimension min, max;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
 
    if (max == 0)
	max = resource_max;
    if (min == 0)
	min = resource_min;
    if (max < min) 
	max = min;
    subdata = DataFromWidget(subwidget);
    if (subdata->resizable)
	{
	subdata->min = min;
	subdata->max = max;
	}

    if (subdata->sharedflag & SharedPaneMask)
	{
	Widget subwidget2;
	Constraints subdata2;
	int i, visable_child=NULL;

	for (i=(NumChildren(pane)+1)/2 ; i<NumChildren(pane); i++) 
	    {
	    subwidget2 = Children(pane)[i];
	    if (XtClass (subwidget) == mullionWidgetClass)
		{
		continue;
		}
	    subdata2 = DataFromWidget(subwidget2);
	    if ((subdata2->position != subdata->position) ||
		(subwidget2 == subwidget))
		{
		continue;
		};
	    subdata2->min = subdata->min;
	    subdata2->max = subdata->max;
	    if (subdata2->sharedflag & ViewableInPaneMask)
		{
		visable_child = i;
		}
	    }
	if (visable_child != NULL)
	    {
	    subwidget = Children(pane)[visable_child];
	    subdata = DataFromWidget(subwidget);
	    }
	}
    /* only try to fill pane and commit sizes if we really need to
     */
    if (subdata->dlength < subdata->min || subdata->dlength > subdata->max)
	{
/*
 * Check to see if the dlength for the subwidget is greater than the max. If it
 * is then I must call RefigureDlengths instead of TryToFillPane. TryToFillPane
 * will use the current length which hasn't been change so it will pass a delta
 * change to RefigureDlengths of 0 which does zippo. Since I know the amount I
 * need to change I'll just call RefigureDlengths directly.
 */
	if (subdata->dlength > subdata->max)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - subdata->max);
	    }
	else if (subdata->dlength < subdata->min)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - subdata->min);
	    }
	else
	    {
	    TryToFillPane(pane);
	    }
	CommitNewSizes(pane, (Widget) NULL);
	}
}
 
#ifdef  VMS
void DWT$PANE_SET_MIN_MAX(subwidget, 
	      min, max)
    Widget        *subwidget;			/* widget */
    Dimension	  *min, *max;			/* min & max size of the widget */
{
    DwtPaneSetMinMax(*subwidget,*min,*max);
}
#endif  /* VMS */ 

void DwtPaneSetConstraints(subwidget, min, max, height, resizable)
Widget subwidget;
Dimension min, max, height;
Boolean resizable;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
 
    if (max == 0)
	max = resource_max;
    if (min == 0)
	min = resource_min;
    if (max < min) 
	max = min;

    subdata = DataFromWidget(subwidget);
    subdata->min = min;
    subdata->max = max;

    /* only try to fill pane and commit sizes if we really need to
     */
    if (subdata->dlength != height)
	{
/*
 * Check to see if the dlength for the subwidget is greater than the max. If it
 * is then I must call RefigureDlengths instead of TryToFillPane. TryToFillPane
 * will use the current length which hasn't been change so it will pass a delta
 * change to RefigureDlengths of 0 which does zippo. Since I know the amount I
 * need to change I'll just call RefigureDlengths directly.
 */
	if (subdata->dlength > height)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - height);
	    }
	else if (subdata->dlength < height)
	    {
	    RefigureDlengths (pane, subwidget, subdata->dlength - height);
	    }
	else
	    {
	    TryToFillPane(pane);
	    }
	CommitNewSizes(pane, (Widget) NULL);
	}
    subdata->resizable = resizable;
}

void DwtPaneSetMin(subwidget, min)
Widget subwidget;
Dimension min;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
    if (min == 0)
	min = resource_min;
    subdata = DataFromWidget(subwidget);
    if (subdata->resizable)
	{
	subdata->min = min;
	if (subdata->max < subdata->min)
	    subdata->max = subdata->min;
	}

    if (subdata->sharedflag & SharedPaneMask)
	{
	Widget subwidget2;
	Constraints subdata2;
	int i, visable_child=NULL;

	for (i=(NumChildren(pane)+1)/2 ; i<NumChildren(pane); i++) 
	    {
	    subwidget2 = Children(pane)[i];
	    if (XtClass (subwidget) == mullionWidgetClass)
		{
		continue;
		}
	    subdata2 = DataFromWidget(subwidget2);
	    if ((subdata2->position != subdata->position) ||
		(subwidget2 == subwidget))
		{
		continue;
		};
	    subdata2->min = subdata->min;
	    if (subdata2->sharedflag & ViewableInPaneMask)
		{
		visable_child = i;
		}
	    }
	if (visable_child != NULL)
	    {
	    subwidget = Children(pane)[visable_child];
	    subdata = DataFromWidget(subwidget);
	    }
	}

    if (subdata->dlength < subdata->min)
	{
	RefigureDlengths (pane, subwidget, subdata->dlength - subdata->min);
	CommitNewSizes(pane, (Widget) NULL);
	}
}
 
#ifdef  VMS
void DWT$PANE_SET_MIN(subwidget, min)
    Widget        *subwidget;			/* widget */
    Dimension	  *min;				/* min size of the widget */
{
    DwtPaneSetMin(*subwidget,*min);
}
#endif  /* VMS */ 

void DwtPaneSetMax(subwidget, max)
Widget subwidget;
Dimension max;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints subdata;
    if (max == 0)
	max = resource_max;
    subdata = DataFromWidget(subwidget);
    if (max < subdata->min) 
	max = subdata->min;
    if (subdata->resizable)
	{
	subdata->max = max;
	}
    if (subdata->sharedflag & SharedPaneMask)
	{
	Widget subwidget2;
	Constraints subdata2;
	int i, visable_child=NULL;

	for (i=(NumChildren(pane)+1)/2 ; i<NumChildren(pane); i++) 
	    {
	    subwidget2 = Children(pane)[i];
	    if (XtClass (subwidget) == mullionWidgetClass)
		{
		continue;
		}
	    subdata2 = DataFromWidget(subwidget2);
	    if ((subdata2->position != subdata->position) ||
		(subwidget2 == subwidget))
		{
		continue;
		};
	    subdata2->max = subdata->max;
	    if (subdata2->sharedflag & ViewableInPaneMask)
		{
		visable_child = i;
		}
	    }
	if (visable_child != NULL)
	    {
	    subwidget = Children(pane)[visable_child];
	    subdata = DataFromWidget(subwidget);
	    }
	}
    if (subdata->dlength > subdata->max)
	{
	RefigureDlengths (pane, subwidget, subdata->dlength - subdata->max);
	CommitNewSizes(pane, (Widget) NULL);
	}
}
 
#ifdef  VMS
void DWT$PANE_SET_MAX(subwidget, max)
    Widget        *subwidget;			/* widget */
    Dimension	  *max;				/* max size of the widget */
{
    DwtPaneSetMax(*subwidget,*max);
}
#endif  /* VMS */ 

/* this needs to be incorporated into set values */
void DwtPaneAllowResizing(pane, allowtype)
PaneWidget pane;
int allowtype;
{
    ResizeMode(pane) = allowtype;
    if (!allowtype) TryToFillPane(pane);
}
 
 

void DwtPaneMakeViewable(subwidget)
Widget subwidget;
{
    PaneWidget pane = (PaneWidget) subwidget->core.parent;
    Constraints cursubdata, subdata;
    Widget cursubwidget, curmullion, mullion;
    XWindowChanges changes;
    int	viewable, notviewable, position;
 
/*
 * The current subwidget is the one occuppying the same position in the 
 * PaneChildren Widget list. If not then we need to go down the list till we
 * find the one that is sharing this position
 */
    subdata = DataFromWidget(subwidget);
    for (position = subdata->position; position > 0; position--)
	{
	cursubwidget = PaneChildren(pane)[position - 1];
	cursubdata = DataFromWidget(cursubwidget);
	if (cursubdata->position == subdata->position)
	    break;
	};
 
/*
 * If this subwidget is the one currently displayed or is not in a shared pane
 * or it is not managed then don't bother doing any of this work.
 */
    if (subwidget == cursubwidget || !subwidget->core.managed || 
	    subdata->sharedflag == NoSharedPaneMask)
	return;
 
    viewable = SharedPaneMask | ViewableInPaneMask;
    notviewable = SharedPaneMask;
 
/*
 * Get the subdata of widget currently viewable and set the mullions
 */
    curmullion = cursubdata->mullion;
    mullion = subdata->mullion;
 
 
/*
 * Set the changes based on the current widget attributes
 */
    changes.x = cursubwidget->core.x;
    changes.y = cursubwidget->core.y;
    changes.width = cursubwidget->core.width;
    changes.height = cursubwidget->core.height;
    changes.border_width = 0;
    if (changes.x != subwidget->core.x || changes.y != subwidget->core.y ||
	  changes.width != subwidget->core.width ||
	  changes.height != subwidget->core.height ||
	  changes.border_width != subwidget->core.border_width) 
	{
	subwidget->core.x = changes.x;
	subwidget->core.y = changes.y;
	subwidget->core.width = changes.width;
	subwidget->core.height = changes.height;
	subwidget->core.border_width = changes.border_width;
	if (XtIsRealized(subwidget))    
	    {
	    XConfigureWindow(XtDisplay(subwidget), XtWindow(subwidget),
			     CWX |CWY | CWWidth | CWHeight | CWBorderWidth,
			     &changes);
	    }
	if (XtClass(subwidget)->core_class.resize != (XtWidgetProc) NULL)
	    {
	    (*(subwidget->core.widget_class->core_class.resize))(subwidget);
	    }
	}
 
/*
 * Set the current widgets and subwidget's shared pane attributes
 */
    cursubdata->sharedflag = notviewable;
    subdata->sharedflag = viewable;

/*
 * Set the min and the max values based on the currently visable widget
 */
    subdata->min = cursubdata->min;
    subdata->max = cursubdata->max;
 
/*
 * Unmap the current widget and map the subwidget by setting mapped_when_managed
 */
    XtSetMappedWhenManaged (subwidget, TRUE);
    XtSetMappedWhenManaged (cursubwidget, FALSE);
 
/*
 * If the cursubwidget was the last to have focus change it to the subwidget
 */
    if (LastHadFocus(pane) == cursubwidget)
	LastHadFocus(pane) = subwidget;
 
/*
 * Set the changes based on the current mullions attributes
 */
    changes.x = curmullion->core.x;
    changes.y = curmullion->core.y;
    if (changes.x != mullion->core.x || changes.y != mullion->core.y) 
	{
	mullion->core.x = changes.x;
	mullion->core.y = changes.y;
	if (XtIsRealized(subwidget))    
	    {
	    XConfigureWindow(XtDisplay(mullion), XtWindow(mullion),
			     CWX | CWY, &changes);
	    }
	}
/*
 * Unmap the current widget and map the subwidget by setting mapped_when_managed
 */
    XtSetMappedWhenManaged (mullion, TRUE);
    XtSetMappedWhenManaged (curmullion, FALSE);
 
    PaneChildren(pane)[position -  1] = subwidget;
    
}
 
#ifdef  VMS
void DWT$PANE_MAKE_VIEWABLE(subwidget)
    Widget        *subwidget;			/* widget */
{
    DwtPaneMakeViewable(*subwidget);
}
#endif  /* VMS */ 

unsigned int PaneInitializeForDRM ()
{
int	stat;
 
/*  Initialize DRM                                                            */
/*  This MUST be done before the XtInitialize call...                         */
/*                                                                            */
DwtInitializeDRM ();
 
stat = DwtRegisterClass (DRMwcUnknown, "pane_widget",
	"DwtPaneCreate", DwtPaneCreate,  panewidgetclass);
 
if (stat != DRMSuccess)
	{
	printf ("pane widget registration failed\n");
	return stat;
	};
 
return stat;
}

#define AcceptFocusProc(w) ((w)->core.widget_class->core_class.accept_focus)
static Boolean AcceptFocus(w, time)
Widget w;
Time *time;
{
    PaneWidget pane = (PaneWidget) w;
    int i;
    Boolean took_focus;

    if (LastHadFocus(pane) == NULL) {
	for (i=0 ;
	     i<PaneNumChildren(pane) && LastHadFocus(pane) == NULL;
	     i++) {
	    if (AcceptFocusProc(PaneChildren(pane)[i]))
		LastHadFocus(pane) = PaneChildren(pane)[i];
	}
	if (LastHadFocus(pane) == NULL) return ((Boolean) FALSE);
    }
    if (AcceptFocusProc(LastHadFocus(pane)) != NULL)
	{
	took_focus = (*AcceptFocusProc(LastHadFocus(pane)))(LastHadFocus(pane), time);
	return (took_focus);
	}
    return ((Boolean) FALSE);
}
 
 

static void GrabFocus(w, event)
Widget w;
XEvent *event;
{
    Time time;
 
    EventToTime(time,event);  /* get the time out of the event */
    AcceptFocus(XtParent(w), &time);
}
 
 

static void FocusNext(w, event)
Widget w;
XEvent *event;
{
    PaneWidget pane = (PaneWidget) XtParent(w);
    Time time;
    int i, j;
    EventToTime(time,event);  /* get the time out of the event */
    for (i=0 ; i<PaneNumChildren(pane); i++)
	if (PaneChildren(pane)[i] == w) break;
    for (j=i+1 ; j<PaneNumChildren(pane) ; j++) {
	if (AcceptFocusProc(PaneChildren(pane)[j]) != NULL) {
	    LastHadFocus(pane) = PaneChildren(pane)[j];
	    AcceptFocus(XtParent(w), &time);
	    return;
	}
    }
}
 

static void FocusPrev(w, event)
Widget w;
XEvent *event;
{
    PaneWidget pane = (PaneWidget) XtParent(w);
    Time time;
    int i, j;
    EventToTime(time,event);  /* get the time out of the event */
    for (i=0 ; i<PaneNumChildren(pane); i++)
	if (PaneChildren(pane)[i] == w) break;
    for (j=i-1 ; j>=0 ; j--) {
	if (AcceptFocusProc(PaneChildren(pane)[j]) != NULL) {
	    LastHadFocus(pane) = PaneChildren(pane)[j];
	    AcceptFocus(XtParent(w), &time);
	    return;
	}
    }
}
 
/*  DEC/CMS REPLACEMENT HISTORY, Element PANE.C */
/*  *3    30-AUG-1989 15:59:18 BRINKLEY "const removed" */
/*  *2    29-AUG-1989 16:42:50 BRINKLEY "bug fixed" */
/*  *1    22-AUG-1989 16:55:37 RYAN "Initial elements for V3" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PANE.C */
