/* SList.c

*****************************************************************************
*                                                                           *
*  COPYRIGHT (c) 1989  BY                                                   *
*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                   *
*  ALL RIGHTS RESERVED.                                                     *
*                                                                           *
*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED    *
*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE    *
*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER    *
*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY    *
*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY    *
*  TRANSFERRED.                                                             * 
*                                                                           *
*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE    *
*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT    *
*  CORPORATION.                                                             *
*                                                                           *
*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS    *
*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                  *
*                                                                           *
*****************************************************************************

        This module implements the SList widget.

The SList widget can be used to present a list of options to the user.
By using Button1 on the mouse, the user can select one or more options,
and can scroll through the list using the scrollbar or keyboard.

The widget has a callback that is used to ask the application for
the contents of the list items when it is necessary for refreshing the
display (or when initially displaying the list).

The SList widget can be created by calls to XtCreateWidget or in UIL.  Its
interface with the application is simple and consists of the following calls:

SListInitializeForDRM() - must be called before any SList widgets are
    created, if you are using UIL to create them.
                
SListCreate(pW, nameP, argsP, argCnt) - is a convenience routine (also
    used by UIL/DRM) to create a SList widget.  You will probably not
    call this routine directly from an application.

SListLoad(w, sLineCnt) - may be called at any time to pass a
    new count to the SList widget.  If the
    SList widget is visible, it is repainted with the new list contents.

SListSelectAll(w, select) - may be called at any time to select or
    unselect all the entries in the list.

SListUnhighlight(w) - may be called to unhighlight the highlighted line,
    if any.
SListHighlight(w) - Select the highlighted line.

There are three callbacks made by the widget:

(1) select_callback is made whenever the user upclicks MB1 in the list.
(2) double_click_callback is made whenever the user double-clicks on an
    item in the list.
(3) get_data_callback is used to determine the contents of a list item
    for display.

MODIFICATION HISTORY:

*/
#include "config.h"
#define SLIST
#ifdef MOTIF
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
extern void _XmDrawShadow();
#if (XmVERSION == 1) && (XmREVISION == 0)
#define MOTIF_V10
#endif
#if (XmVERSION > 1) || (XmREVISION > 1)
/* For Motif 1.2 */
#include <Xm/PrimitiveP.h>
#endif
#include <Xm/XmP.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#ifdef USE_MRM
#include <Mrm/MrmAppl.h>
#endif
#else
#ifdef unix
#include <X11/DECwDwtWidgetProg.h>
#include <X11/Xatom.h>
#else
#include <decw$include/DECwDwtWidgetProg.h>
#include <decw$include/Xatom.h>
#endif
#endif /* MOTIF */
#include "slistP.h"
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

/*
 * Forward declarations
 */
static int ComputeMaxYAdjust();
static int YToLine();
static void SetYAdjust();
static void Flip();
static void Draw();
static void CallSelectCallbacks();
static void CallDoubleClickCallbacks();
static void CallGetDataCallback();
static void SetLineSelection();
static void NewScrollPosition();
static void NewSelectPosition();
static XtCallbackProc scrollCallback();
static void Mb1PressInList();
static void ClickTimer();
static void ButtonReleaseHandler();
static void ButtonPressHandler();
static void PointerMotionHandler();
static void DrawExposedLines();
static void SaveExposeRegion();
static void StartScrollGrop();
static void Repaint();
static void DoExpose();
static XtGeometryResult QueryGeometry();
static void DoInitialize();
static void DoRealize();
static void DoResize();
static void DoDestroy();
XtActionProc slistForwardPage();
XtActionProc slistBackwardPage();
XtActionProc slistForwardLine();
XtActionProc slistBackwardLine();
static void ClassInitialize();

static XtCallbackRec VSCallBack[] = 
{
    {(XtCallbackProc)   scrollCallback, (caddr_t) NULL},
    {NULL,              (caddr_t) NULL},
};
#if defined (MOTIF) && !defined (MOTIF_V10)
static char slistBindings[] =
    "Shift ~Meta ~Alt <Key>Tab: PrimitivePrevTabGroup()\n\
     ~Meta ~Alt <Key>Tab:       PrimitiveNextTabGroup()\n\
     <FocusIn>:                 PrimitiveFocusIn()\n\
     <FocusOut>:                PrimitiveFocusOut()\n\
     <Key>osfPageDown:          forward-page()\n\
     <Key>osfPageUp:            backward-page()\n\
     <Key>osfDown:              forward-line()\n\
     <Key>osfUp:                backward-line()";
#else
static char slistBindings[] =
    "Ctrl ~Shift ~Meta ~Alt <Key>Next: forward-page()\n\
     Ctrl ~Shift ~Meta ~Alt <Key>Prior:backward-page()\n\
     Ctrl ~Shift ~Meta ~Alt <Key>Down: forward-line()\n\
     Ctrl ~Shift ~Meta ~Alt <Key>Up:   backward-line()";
#endif

static XtActionsRec slistActionsTable[] = {
    {"forward-page",    (XtActionProc) slistForwardPage},
    {"backward-page",   (XtActionProc) slistBackwardPage},
    {"forward-line",    (XtActionProc) slistForwardLine},
    {"backward-line",   (XtActionProc) slistBackwardLine},
#if defined (MOTIF) && !defined (MOTIF_V10)
    {"PrimitiveFocusIn",(XtActionProc) _XmPrimitiveFocusIn},
    {"PrimitiveFocusOut",(XtActionProc) _XmPrimitiveFocusOut},
#endif
};

/*
 * Widget resource tables
 */
#ifndef XtCCallback
#define XtCCallback "Callback"
#endif
#ifndef XtRCallback
#define XtRCallback "Callback"
#endif
#define Offset(x) XtOffset(SListWidget, x)

#ifdef MOTIF_V10
extern void _XmForegroundColorDefault();
#endif

static XtResource resources[] = {
#ifdef MOTIF
  {XmNfont, XmCFontList, XmRFontList, sizeof(XmFontList *),
      Offset(slist.fontRecP), XmRString, "fixed"},
  {SListNcols, SListCCols, XtRInt, sizeof(int),
      Offset(slist.cols), XtRImmediate, (caddr_t)1},
  {SListNrows, SListCRows, XmRInt, sizeof(int),
      Offset(slist.rows), XmRImmediate, (caddr_t)1},
  {SListNselectCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
      Offset(slist.selectCallback), XmRCallback, (caddr_t)0},
  {SListNdoubleClickCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
      Offset(slist.doubleClickCallback), XmRCallback, (caddr_t)0},
  {SListNgetDataCallback, XmCCallback, XmRCallback, sizeof(XtCallbackList),
      Offset(slist.getDataCallback), XmRCallback, (caddr_t)0},
#ifdef MOTIF
#ifdef MOTIF_V10
  {SListNforeground, SListCForeground, XmRPixel, sizeof(Pixel),
      Offset(slist.foreground), XmRCallProc,
      (caddr_t) _XmForegroundColorDefault},
#else
  {SListNforeground, SListCForeground, XmRPixel, sizeof(Pixel),
      Offset(slist.foreground), XmRString, XtExtdefaultforeground},
#endif
#endif
  {SListNtextMarginWidth, SListCTextMarginWidth, XmRInt, sizeof(int),
      Offset(slist.textMarginWidth), XmRImmediate, (caddr_t)4},
  {SListNdoubleClickDelay, SListCDoubleClickDelay, XmRInt, sizeof(int),
      Offset(slist.ClickInterval), XmRImmediate, (caddr_t) 250}
#else
  {DwtNfont, DwtCFontList, DwtRFontList, sizeof(DwtFontRec *),
      Offset(slist.fontRecP), XtRString, "fixed"},
  {SListNcols, SListCCols, XtRInt, sizeof(int),
      Offset(slist.cols), XtRImmediate, (caddr_t)1},
  {SListNrows, SListCRows, XtRInt, sizeof(int),
      Offset(slist.rows), XtRImmediate, (caddr_t)1},
  {SListNselectCallback, XtCCallback, XtRCallback, sizeof(DwtCallbackPtr),
      Offset(slist.selectCallback), XtRCallback, (caddr_t)0},
  {SListNdoubleClickCallback, XtCCallback, XtRCallback, sizeof(DwtCallbackPtr),
      Offset(slist.doubleClickCallback), XtRCallback, (caddr_t)0},
  {SListNgetDataCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
      Offset(slist.getDataCallback), XtRCallback, (caddr_t)0},
  {SListNforeground, SListCForeground, XtRPixel, sizeof(Pixel),
      Offset(slist.foreground), XtRString, DwtSForegroundDefault},
  {SListNtextMarginWidth, SListCTextMarginWidth, XtRInt, sizeof(int),
      Offset(slist.textMarginWidth), XtRImmediate, (caddr_t)4},
  {SListNdoubleClickDelay, SListCDoubleClickDelay, XtRInt, sizeof(int),
      Offset(slist.ClickInterval), XtRImmediate, (caddr_t) 250}
#endif
};

/*
 * The widget class record
 */
externaldef(slistwidgetclassrec)
SListClassRec slistwidgetclassrec = {
    {/* core_class fields       */
#ifdef MOTIF
        /* superclass           */      (WidgetClass) &xmPrimitiveClassRec,
#else
        /* superclass           */      (WidgetClass) &widgetClassRec,
#endif
        /* class_name           */      "SList",
        /* widget_size          */      sizeof(SListWidgetRec),
        /* class_initialize     */      ClassInitialize,
        /* class_part_initialize */     NULL,
        /* class_inited         */      FALSE,
        /* initialize           */      (XtInitProc) DoInitialize,
        /* initialize_hook      */      NULL,
        /* realize              */      (XtRealizeProc) DoRealize,
        /* actions              */      NULL,
        /* num_actions          */      0,
        /* resources            */      (XtResource *) resources,
        /* num_resources        */      (int) XtNumber(resources),
        /* xrm_class            */      NULLQUARK,
        /* compress_motion      */      TRUE,
        /* compress_exposure    */      TRUE,
        /* compress_enterleave  */      FALSE,
        /* visible_interest     */      FALSE,
        /* destroy              */      (XtWidgetProc) DoDestroy,
        /* resize               */      (XtWidgetProc) DoResize,
        /* expose               */      (XtExposeProc) DoExpose,
        /* set_values           */      NULL,
        /* set_values_hook      */      NULL,
        /* set_values_almost    */      XtInheritSetValuesAlmost, 
        /* get_values_hook      */      NULL,
        /* accept_focus         */      NULL,
        /* version              */      (XtVersionType) XtVersionDontCheck,
        /* callback_private     */      NULL,
        /* tm_table             */      NULL,
        /* query_geometry       */      QueryGeometry,
        /* display_accelerator  */      NULL,
        /* extension            */      NULL
  },
#ifdef MOTIF
  {     /* border_highlight     */      (XtWidgetProc) _XtInherit,
        /* border_unhighlight   */      (XtWidgetProc) _XtInherit,
        /* translations         */      NULL,
        /* arm_and_activate     */      (XtActionProc) _XtInherit,
        /* syn_resources        */      NULL,
        /* num_syn_resources    */      0,
        /* extension            */      NULL,
  },
#endif
  {	/* translations		*/	NULL,
  }
};

externaldef (slistwidgetclass)
SListClass slistwidgetclass = &slistwidgetclassrec;

/*
 * Widget private routines
 */

static void CheckSize(w)
    SListWidget w;
{
#ifndef MOTIF
    if (!XtIsRealized(w)) return;
/*
 * Hacky: handle resizing of the parent widget by resizing myself
 */
    if (w->core.parent->core.width  != w->slist.parentWidth ||
        w->core.parent->core.height != w->slist.parentHeight) {
	w->core.width = w->slist.vScroll->core.x - 2;
	w->core.height = w->core.parent->core.height - 2 * w->core.border_width;
	w->slist.knownHeight = 0;
	DoResize(w);		
	w->slist.parentWidth = w->core.parent->core.width;
	w->slist.parentHeight = w->core.parent->core.height;
    }
#endif
}
static int ComputeMaxYAdjust(w)
    SListWidget w;
{
    int maxYAdjust = w->slist.LineCnt*w->slist.cellHeight - w->slist.textHeight;

    if (maxYAdjust < 0) maxYAdjust = 0;

    return (maxYAdjust);
}
static int YToLine(w, y)
    SListWidget w;
    int y;
{
    int line;

    if (y < 0) line = -1;
    else if (y >= w->slist.textHeight)
        line = w->slist.topLine + w->slist.rows - 1;
    else {
        line = (y + w->slist.YAdjust) / w->slist.cellHeight;
        if (line < 0) line = -1;
        else if (line >= w->slist.LineCnt) line = -1;
    }

    return (line);
}

static void SetYAdjust(w, yAdjust)
    SListWidget w;
    int yAdjust;
{
    int maxYAdjust = ComputeMaxYAdjust (w);

    if (yAdjust > maxYAdjust) yAdjust = maxYAdjust;
    if (yAdjust < 0) yAdjust = 0;

    w->slist.YAdjust = yAdjust;
}

static void Flip(w, line)
    SListWidget w;
    int line;
{
    if (line < 0 || line >= w->slist.LineCnt ||
	line < w->slist.topLine || line > w->slist.topLine + w->slist.rows)
	return;

    if (XtIsRealized (w)) XFillRectangle (XtDisplay (w), XtWindow(w),
        w->slist.flipgc,
        w->slist.leftMarginWidth + w->slist.textX + 1,
        (line - w->slist.topLine) * w->slist.cellHeight + w->slist.textY + 1,
        w->slist.textWidth - w->slist.leftMarginWidth - w->slist.textX - 2,
        w->slist.cellHeight - 2);
}

static void Draw(w, line)
    SListWidget w;
    int line;
{
    int select;
    char *text;

    int x = w->slist.leftMarginWidth + w->slist.textX + 1;
    int y = (line - w->slist.topLine) * w->slist.cellHeight +
	w->slist.textY + 1;
    int maxCols, textWidth;

    textWidth = w->core.width - w->slist.leftMarginWidth -
#ifdef MOTIF
        2*w->primitive.shadow_thickness -
#endif
	2 * w->core.border_width + 6;
    maxCols = (textWidth / w->slist.cellWidth) - 1;

    if (!XtIsRealized (w)) return;

    if (line - w->slist.topLine >= w->slist.rows) return;

    if (line < 0 || y < 0 || y >= (int) w->core.height) return;
    if (line >= w->slist.LineCnt) {
        XClearArea (XtDisplay (w), XtWindow (w), x, y, 
            w->slist.textWidth - x - 1,
            w->slist.cellHeight - 2, 0);
	return;
    }

    select = w->slist.selectMapP[line];
    CallGetDataCallback(w, line, &text);
    if (text == NULL) text = "-----";
    if (select) {
        XFillRectangle (XtDisplay (w), XtWindow (w),
            w->slist.normgc, x, y,
            w->slist.textWidth - x - 1,
            w->slist.cellHeight - 2);
    } else {
        XClearArea (XtDisplay (w), XtWindow (w), x, y, 
            w->slist.textWidth - x - 1,
            w->slist.cellHeight - 2, 0);
    }
    XDrawString (
        XtDisplay (w), XtWindow (w),
        (select ? w->slist.invgc : w->slist.normtxtgc),
        x + w->slist.textMarginWidth,
        y + w->slist.yFontAdjust, text, MIN(strlen(text), maxCols));
}

static void CallSelectCallbacks(w, eventP)
    SListWidget w;
    XEvent *eventP;
{
    SListCallbackStruct cbData;

    cbData.reason = SListCRSelect;
    cbData.event = eventP;
    cbData.select = w->slist.selectParity;
    if (w->slist.upLine >= w->slist.downLine) {
        cbData.item_number = w->slist.downLine;
        cbData.item_count = w->slist.upLine - w->slist.downLine + 1;
	cbData.select_count = SListGetSelectionCount(w);
    } else {
        cbData.item_number = w->slist.upLine;
        cbData.item_count = w->slist.downLine - w->slist.upLine + 1;
	cbData.select_count = SListGetSelectionCount(w);
    }
    XtCallCallbacks ((Widget) w, SListNselectCallback, &cbData);
}
static void CallDoubleClickCallbacks(w, eventP)
    SListWidget w;
    XEvent *eventP;
{
    SListCallbackStruct cbData;

    cbData.reason = SListCRDoubleClick;
    cbData.event = eventP;
    cbData.item_number = w->slist.downLine;
    cbData.item_count = 1;
    XtCallCallbacks ((Widget) w, SListNdoubleClickCallback, &cbData);
}

static void CallGetDataCallback(w, line, ptr)
SListWidget w;
int line;
char **ptr;
{
    SListCallbackStruct cbData;

    cbData.reason = SListCRGetData;
    cbData.event = NULL;
    cbData.select = 0;
    cbData.item_number = line;
    cbData.item_count = 1;
    cbData.data = ptr;
    *ptr = NULL;
    XtCallCallbacks ((Widget) w, SListNgetDataCallback, &cbData);
}

static void SetLineSelection(w, line, selected)
    SListWidget w;
    int line, selected;
{
    int select;

    if ((line == -1) || (line == w->slist.LineCnt)) return;
    select = w->slist.selectMapP[line];

    if (select == selected) return;

    w->slist.selectMapP[line] = selected;
    Flip (w, line);
}

static void NewScrollPosition(w, y)
    SListWidget w;
    int y;
{    
    int maxY = w->slist.rows * w->slist.cellHeight;
    int maxTop = w->slist.LineCnt - w->slist.rows;
    Arg vsbArg[1];
    
    if (y < 0 || y >= maxY) { 
        if (y < 0) {
            if (w->slist.topLine) {
		w->slist.topLine--;
                w->slist.YAdjust -= w->slist.cellHeight;
                if (w->slist.YAdjust < 0) w->slist.YAdjust = 0;
                StartScrollGrop(w);
		if (w->slist.vScroll != (Widget) NULL) {
#ifdef MOTIF
		    XtSetArg(vsbArg[0], XmNvalue, w->slist.topLine);
#else
		    XtSetArg(vsbArg[0], DwtNvalue, w->slist.topLine);
#endif
		    XtSetValues(w->slist.vScroll, vsbArg, 1);
		}
            }
        } else {
            maxY = ComputeMaxYAdjust(w);
	    if (w->slist.topLine < maxTop) {
		w->slist.topLine++;
                w->slist.YAdjust += w->slist.cellHeight;
                if (w->slist.YAdjust > maxY) w->slist.YAdjust = maxY;
                StartScrollGrop(w);
		if (w->slist.vScroll != (Widget) NULL) {
#ifdef MOTIF
		    XtSetArg(vsbArg[0], XmNvalue, w->slist.topLine);
#else
		    XtSetArg(vsbArg[0], DwtNvalue, w->slist.topLine);
#endif
		    XtSetValues(w->slist.vScroll, vsbArg, 1);
		}
            }
        }
    }
}

static void NewSelectPosition(w, y)
    SListWidget w;
    int y;
{
    int line;

    if (y < 0) y = 0;
    if (y >= w->slist.textHeight) y = w->slist.textHeight;
    line = YToLine (w, y);
    if (line == -1) line = 0;

    while (line != w->slist.mouseLine) {
        if (w->slist.mouseLine < line)
            if (w->slist.mouseLine < w->slist.downLine) {
                int select = w->slist.selectMapP[w->slist.mouseLine];
                SetLineSelection (w, (w->slist.mouseLine)++, select);
            } else SetLineSelection (w, ++(w->slist.mouseLine), w->slist.selectParity);
        else
            if (w->slist.mouseLine > w->slist.downLine) {
                int select = w->slist.selectMapP[w->slist.mouseLine];
                SetLineSelection (w, (w->slist.mouseLine)--, select);
            } else SetLineSelection (w, --(w->slist.mouseLine), w->slist.selectParity);
    }
}   

static XtCallbackProc scrollCallback(w, closure, call_data)
    Widget w;
    caddr_t closure;
#ifdef MOTIF
    XmScrollBarCallbackStruct *call_data;
#else
    DwtScrollBarCallbackStruct *call_data;
#endif
{
    SListWidget sl = (SListWidget) closure;

    switch (call_data->reason) {
#ifdef MOTIF
        case XmCR_INCREMENT:
        case XmCR_DECREMENT:
        case XmCR_PAGE_INCREMENT:
        case XmCR_PAGE_DECREMENT:
        case XmCR_DRAG:
#else
        case DwtCRUnitInc:
        case DwtCRUnitDec:
        case DwtCRPageInc:
        case DwtCRPageDec:
        case DwtCRDrag:
#endif
	    if (call_data->value < 0) call_data->value = 0;
	    if (call_data->value >= sl->slist.LineCnt)
		call_data->value = sl->slist.LineCnt - 1;
	    sl->slist.topLine = call_data->value;
            sl->slist.YAdjust = call_data->value * sl->slist.cellHeight;
            StartScrollGrop (sl);
            break;

    }
    return 0;
}

static void Mb1PressInList(w, eventP)
    SListWidget w;
    XButtonEvent *eventP;
{
    int line;
    int needsRepaint = 0;
    int oldDownLine;
    int i, start, end;

    CheckSize(w);
    oldDownLine = w->slist.downLine;
    w->slist.mouseLine = w->slist.downLine = w->slist.upLine = line =
        YToLine (w, eventP->y - w->slist.textY);
    /*
     * check for pending timer
     */
    if (w->slist.ClickTimerID != 0) {
        if (w->slist.downLine != w->slist.ClickLine) {  /* Same line? */
            XtRemoveTimeOut(w->slist.ClickTimerID);     /* No, not a double click */
            CallSelectCallbacks(w, NULL);       /* deliver initial selection */
            w->slist.ClickTimerID = 0;          /* Zap the click timer */
        } else {
            w->slist.listIsGrabbed = 1;         /* grabbed (for up-click) */
            return;                             /* else wait for up-click */
        }
    }
    /*
     * On MB1 down, deselect all. No deselect for Shift-MB1 or Ctrl-MB1.
     */
    if (!(eventP->state & (ShiftMask | ControlMask))) {
        w->slist.highlightedLine = -1;
        for (i=0;i <w->slist.LineCnt; i++) {
            if (w->slist.selectMapP[i] != 0) {
                needsRepaint = 1;
            }
            w->slist.selectMapP[i] = 0;
        }
    }
    if (needsRepaint)
        Repaint (w);

    w->slist.listIsGrabbed = 1;

    if (eventP->state & ShiftMask) {
	/* Extended select */
	if (oldDownLine < line)
	    start = oldDownLine;
	else start = line; 
	if (oldDownLine > line)
	    end = oldDownLine;
	else end = line; 
	for (i = start; i <= end; i++) {
	    if (i >= 0 && i < w->slist.LineCnt) {
		SetLineSelection (w, i, 1);
	    }
	}
	w->slist.selectParity = 1;
	return;
    }
    if ((line != -1) && (line < w->slist.LineCnt)) {
        w->slist.selectParity = (w->slist.selectMapP[line] == 0);
        SetLineSelection (w, line, w->slist.selectParity);
    } else w->slist.selectParity = 1;
}

static void ClickTimer(w, id)
    SListWidget w;
    XtIntervalId id;
{
/*
 * Here when the timer expires
 */

    if (w->slist.downLine != -1) {
	if (w->slist.DownCount > 1) {
	    CallDoubleClickCallbacks (w, NULL); /* call double click procs */
	} else {
	    CallSelectCallbacks (w, NULL);  /* call select procs */
	}
    }
    w->slist.ClickTimerID = 0;
}
static void ButtonReleaseHandler(w, closure, eventP)
    SListWidget w;
    Opaque closure;
    XButtonEvent *eventP;
{
    if (w->slist.listIsGrabbed) {
        int y = eventP->y - w->slist.textY;

        /* If we're still Scrolling, treat the upclick as though it were
         * at the top or bottom edge of the list.  This prevents unseen
         * lines from being selected. */

        if (y < 0)  {
/*	    Flip(w, 0); 	not necesary */
	    y = 0;
	} else
	    if (y >= w->slist.textHeight) y = w->slist.textHeight - 1;
        w->slist.upLine = YToLine(w, y);
        if (w->slist.upLine == -1)
                w->slist.upLine = w->slist.LineCnt - 1;
        NewScrollPosition (w, 0);           /* shut off scrolling */

        w->slist.listIsGrabbed = 0;
        if (w->slist.upLine != w->slist.downLine) {     /* same line? if not...*/
            if (w->slist.ClickTimerID != 0) {           /* timer pending? */
                XtRemoveTimeOut(w->slist.ClickTimerID);/* Then cancel it */
                w->slist.ClickTimerID = 0;
            }
            CallSelectCallbacks (w, eventP);    /* call select procs */
        } else {
/*
 * same line - initiate double-click timer
 */
            if (w->slist.ClickTimerID == 0) {           /* first time */
                w->slist.ClickTimerID = XtAppAddTimeOut (
				XtWidgetToApplicationContext((Widget) w),
                                (unsigned long) w->slist.ClickInterval,
                                (XtTimerCallbackProc) ClickTimer,
                                (caddr_t) w);
                w->slist.DownCount = 1;
                w->slist.DownTime = eventP->time;
                w->slist.ClickLine = w->slist.upLine;
            } else {
                w->slist.DownCount++;                   /* count another down */
            }
        }
    } 
}

static void ButtonPressHandler(w, closure, eventP)
    SListWidget w;
    Opaque closure;
    XButtonEvent *eventP;
{

    /* Handle chorded cancel by turning the ButtonPress into a ButtonRelease. */

    if (w->slist.listIsGrabbed) {
        ButtonReleaseHandler (w, closure, eventP);
        return;
    }

    if (!w->slist.LineCnt) return;                      /* nothing to do */

    if (eventP->button == Button1)
        Mb1PressInList (w, eventP);

}

static void PointerMotionHandler(w, closure, eventP)
    SListWidget w;
    Opaque closure;
    XMotionEvent *eventP;
{
    if (w->slist.listIsGrabbed) {
        NewSelectPosition (w, eventP->y - w->slist.textY);
        NewScrollPosition (w, eventP->y - w->slist.textY);
    } 
}

static void DrawExposedLines(w)
    SListWidget w;
{
    int line, LineCnt;
    char *exposeMapP;

    if (!w->slist.anyAreExposed) return;        /* nothing to do */

    LineCnt = w->slist.LineCnt;
    for (line=0, exposeMapP=w->slist.exposeMapP; line<LineCnt; line++, exposeMapP++)
        if (*exposeMapP) {
            Draw (w, line);
            *exposeMapP = 0;
        }

    w->slist.anyAreExposed = 0;
}

static void SaveExposeRegion(w, x, vY, width, height)
    SListWidget w;
    int x, vY, width, height;
{
    int firstLine, lastLine, line;

    firstLine = (vY - w->slist.textY) / w->slist.cellHeight;
    if (firstLine < 0) firstLine = 0;
    if (firstLine >= w->slist.LineCnt) return;

    lastLine = (vY - w->slist.textY + height - 1) / w->slist.cellHeight;
    if (lastLine >= w->slist.LineCnt) lastLine = w->slist.LineCnt - 1;
    if (lastLine < 0) return;

    for (line=firstLine; line <= lastLine; line++) {
        w->slist.exposeMapP[line] = 1;
        w->slist.anyAreExposed = 1;
    }
}

static void StartScrollGrop(w)
    SListWidget w;
{
    SaveExposeRegion (w, 0, w->slist.YAdjust, w->slist.textWidth,
        w->slist.textHeight);

    DrawExposedLines (w);
}

static void Repaint(w)
    SListWidget w;
{
    if (XtIsRealized (w)) XClearArea (XtDisplay (w), XtWindow(w),
        w->slist.textX, w->slist.textY,
        w->slist.textWidth, w->slist.textHeight, 0);

    SaveExposeRegion (w, 0, w->slist.YAdjust, w->slist.textWidth,
        w->slist.textHeight);

    DrawExposedLines (w);
}

static void DoExpose(w, eventP)
    SListWidget w;
    XEvent *eventP;
{
    CheckSize(w);
    if (!XtIsRealized(w)) return;

/*    if (XtIsRealized (w)) XClearArea (XtDisplay (w), XtWindow (w),
        eventP->xexpose.x,
        eventP->xexpose.y, 
        eventP->xexpose.width, eventP->xexpose.height, 0);*/

#ifdef MOTIF
    _XmDrawShadow(XtDisplay(w), XtWindow(w),
	w->primitive.top_shadow_GC, w->primitive.bottom_shadow_GC,
	w->primitive.shadow_thickness, 0, 0, w->core.width, w->core.height);
#endif
    SaveExposeRegion (w,
        eventP->xexpose.x,
        eventP->xexpose.y + w->slist.YAdjust,
        eventP->xexpose.width,
        eventP->xexpose.height);

    /* If this is not a primary Expose event, simply return. */

    if (eventP->xexpose.count) return;

    DrawExposedLines (w);
}
static XtGeometryResult QueryGeometry(w, proposed, answer)
SListWidget w;
XtWidgetGeometry *proposed, *answer;
{
#define Set(bit) (proposed->request_mode & bit)

    /* Return preferred width + Height */
    answer->request_mode = CWWidth | CWHeight;

    answer->width = w->slist.cellWidth * w->slist.cols +
        w->slist.leftMarginWidth + 
#ifdef MOTIF
        2*w->primitive.shadow_thickness +
#endif
	2 * w->core.border_width + 6;

#ifdef MOTIF
    answer->height = w->slist.rows * w->slist.cellHeight +
        4*w->primitive.shadow_thickness;
#else
    answer->height = w->slist.rows * w->slist.cellHeight + 2;
#endif

    if (Set(CWWidth) && proposed->width == answer->width &&
        Set(CWHeight) && proposed->height == answer->height) {
        return XtGeometryYes;
    } 
    if (answer->width == w->core.width && answer->height == w->core.height) {
        return XtGeometryNo;
    }
    return XtGeometryAlmost;
}
#undef Set

static void DoInitialize(request, w)
    SListWidget request, w;
{
    XFontStruct *fontP;
    XGCValues	values;
    short	fontIndex;
    Arg         vsbArgs[50];
    int         i;
    int         requiredWidth;
    unsigned long charWidth = 0;

    w->slist.YAdjust = 0;
    w->slist.listIsGrabbed = 0;
    w->slist.selectMapP =       (char*) 0;
    w->slist.LineCnt = w->slist.anyAreExposed = 0;
    w->slist.exposeMapP = (char*) 0;
    w->slist.mapLen =   0;

    w->slist.topLine = 0;
    w->slist.highlightedLine =  -1;

    w->slist.ClickTimerID = w->slist.DownCount = w->slist.ClickLine = 0;

#ifdef MOTIF
    if (w->slist.fontRecP == NULL) {
        fontP = XLoadQueryFont(XtDisplay(w), "fixed");
    } else {
        _XmFontListSearch((XmFontList)w->slist.fontRecP, "default",
		 &fontIndex, &fontP);
    }
    if (!fontP)
        fontP = XLoadQueryFont(XtDisplay(w), "fixed");
    w->slist.fontP = fontP;
#else
    /* Simply use the first font in the font list. */

    if (w->slist.fontRecP == NULL) {
        fontP = w->slist.fontP = XLoadQueryFont(XtDisplay(w), "fixed");
    } else {
        fontP = w->slist.fontP = w->slist.fontRecP->font;
    }
#endif
    /* Compute some useful constants. */

    w->slist.cellHeight = fontP->max_bounds.descent + fontP->max_bounds.ascent + 2;
    w->slist.yFontAdjust = fontP->max_bounds.ascent;

    /* Make a reasonable initial size for the widget if none was specified. */

    if ((!XGetFontProperty(fontP, XA_QUAD_WIDTH, &charWidth)) || charWidth==0){
        if (fontP->per_char && fontP->min_char_or_byte2 <= '0' &&
                               fontP->max_char_or_byte2 >= '0')
            charWidth = fontP->per_char['0' - fontP->min_char_or_byte2].width;
        else
            charWidth = fontP->max_bounds.width;
    }
    if (charWidth == 0) charWidth = 1;
    requiredWidth = w->slist.cols * charWidth +
#ifdef MOTIF
	2*w->primitive.shadow_thickness +
#endif
        2 * (w->core.border_width + 6);

    if ((int) w->core.width < requiredWidth) {
        w->core.width =  requiredWidth;
    }
    w->slist.cellWidth = charWidth;

    if (!w->slist.rows) {
        w->slist.rows = (int) w->core.height / w->slist.cellHeight;
    } else {
#ifdef MOTIF
        w->core.height = w->slist.rows * w->slist.cellHeight +
	    4 * w->primitive.shadow_thickness;
#else
        w->core.height = w->slist.rows * w->slist.cellHeight + 2;
#endif
    }

    w->slist.leftMarginWidth = w->core.border_width;
    w->slist.knownHeight = w->core.height;
    w->slist.knownWidth = w->core.width;
#ifdef MOTIF
    w->slist.textX = w->slist.textY = 2 * w->core.border_width + 
	w->primitive.shadow_thickness;
    w->slist.textWidth = w->core.width - 4*w->core.border_width - 
	2 * w->primitive.shadow_thickness;
#else
    w->slist.textX = w->slist.textY = 0;
    w->slist.textWidth = w->core.width;
#endif
    w->slist.textHeight = w->slist.rows * w->slist.cellHeight;

    /* Load graphics contexts. */

    values.font = fontP->fid;
    values.foreground = w->slist.foreground;
    values.background = w->core.background_pixel;
    values.function = GXcopy;
    w->slist.normgc = XtGetGC ((Widget) w,
        GCFont | GCForeground | GCBackground | GCFunction, &values);

    w->slist.normtxtgc = XtGetGC ((Widget) w,
        GCFont | GCForeground | GCBackground | GCFunction, &values);

    values.font = fontP->fid;
    values.foreground = w->core.background_pixel;
    values.background = w->slist.foreground;
    values.function = GXcopy;
    w->slist.invgc = XtGetGC((Widget) w,
        GCFont | GCForeground | GCBackground | GCFunction, &values);

    values.function = GXinvert;
    values.plane_mask = w->core.background_pixel ^ w->slist.foreground;
    w->slist.flipgc = XtGetGC ((Widget) w,
        GCFunction | GCPlaneMask, &values);

    i = 0;
    VSCallBack[0].closure = (caddr_t)w;
#ifdef MOTIF
    XtSetArg(vsbArgs[i], XmNorientation,      XmVERTICAL);              i++;
    XtSetArg(vsbArgs[i], XmNheight,           w->core.height);          i++;
    XtSetArg(vsbArgs[i], XmNvalue,            w->slist.YAdjust);        i++;
    if (w->slist.LineCnt > w->slist.rows) {
        XtSetArg(vsbArgs[i], XmNminimum,      0);                       i++;
        XtSetArg(vsbArgs[i], XmNmaximum,      w->slist.LineCnt);        i++;
        XtSetArg(vsbArgs[i], XmNsliderSize,   w->slist.rows);           i++;
    } else {
        XtSetArg(vsbArgs[i], XmNminimum,      0);                       i++;
        XtSetArg(vsbArgs[i], XmNmaximum,      5);                       i++;
        XtSetArg(vsbArgs[i], XmNsliderSize,   5);                       i++;
    }
    XtSetArg(vsbArgs[i], XmNincrement, 1);                              i++;
    XtSetArg(vsbArgs[i], XmNpageIncrement, w->slist.rows-1);            i++;
    XtSetArg(vsbArgs[i], XmNincrementCallback, VSCallBack);             i++;
    XtSetArg(vsbArgs[i], XmNdecrementCallback, VSCallBack);             i++;
    XtSetArg(vsbArgs[i], XmNpageIncrementCallback, VSCallBack);         i++;
    XtSetArg(vsbArgs[i], XmNpageDecrementCallback, VSCallBack);         i++;
    XtSetArg(vsbArgs[i], XmNdragCallback,    VSCallBack);               i++;
    w->slist.vScroll = XtCreateManagedWidget("vpaneScroll",
        xmScrollBarWidgetClass, XtParent (w), vsbArgs, i);

    XmScrolledWindowSetAreas(XtParent(w), (Widget) NULL, w->slist.vScroll,
		(Widget) w);
#else
    XtSetArg(vsbArgs[i], DwtNorientation,     DwtOrientationVertical);  i++;
    XtSetArg(vsbArgs[i], DwtNheight,          w->core.height);          i++;
    XtSetArg(vsbArgs[i], DwtNvalue,           w->slist.YAdjust);        i++;
    if (w->slist.LineCnt > w->slist.rows) {
        XtSetArg(vsbArgs[i], DwtNminValue,    0);                       i++;
        XtSetArg(vsbArgs[i], DwtNmaxValue,    w->slist.LineCnt);        i++;
        XtSetArg(vsbArgs[i], DwtNshown,       w->slist.rows);           i++;
	XtSetArg(vsbArgs[i], DwtNpageInc,     w->slist.rows-1);		i++;
    } else {
        XtSetArg(vsbArgs[i], DwtNminValue,    0);                       i++;
        XtSetArg(vsbArgs[i], DwtNmaxValue,    1);                       i++;
    }
    XtSetArg(vsbArgs[i], DwtNinc,             1);                       i++;
    XtSetArg(vsbArgs[i], DwtNunitIncCallback, VSCallBack);              i++;
    XtSetArg(vsbArgs[i], DwtNunitDecCallback, VSCallBack);              i++;
    XtSetArg(vsbArgs[i], DwtNpageIncCallback, VSCallBack);              i++;
    XtSetArg(vsbArgs[i], DwtNpageDecCallback, VSCallBack);              i++;
    XtSetArg(vsbArgs[i], DwtNdragCallback,    VSCallBack);              i++;
    w->slist.vScroll = XtCreateManagedWidget("vpaneScroll",
        scrollwidgetclass, XtParent (w), vsbArgs, i);
    DwtScrollWindowSetAreas(XtParent(w), NULL, w->slist.vScroll, w);
    w->slist.parentWidth = w->slist.parentHeight = 0;
#endif
    XtAugmentTranslations((Widget) w,
	(XtTranslations) slistwidgetclassrec.slist_class.translations);
}

static void DoRealize(w, maskP, attributesP)
    SListWidget w;
    Mask *maskP;
    XSetWindowAttributes *attributesP;
{

    Boolean ResizeNeeded = False;

    if (w->core.width != w->slist.knownWidth ||
        w->core.height != w->slist.knownHeight)
            ResizeNeeded = True;

    /* Make sure the window is automatically cleared whenever it is resized. */

    *maskP |= CWBitGravity;
    attributesP->bit_gravity = ForgetGravity;

    XtCreateWindow ((Widget) w, InputOutput, CopyFromParent, *maskP,attributesP);

    /* Establish passive button grabs and declare event handlers. */

    XGrabButton (
        XtDisplay (w),                                  /* display */
        1,                                              /* button_grab */
        AnyModifier,                                    /* modifiers */
        XtWindow (w),                                   /* window */
        0,                                              /* owner_events */
        (ButtonPressMask | ButtonReleaseMask | Button1MotionMask),
                                                        /* event_mask */
        GrabModeAsync,                                  /* pointer_mode */
        GrabModeAsync,                                  /* keyboard_mode */
        None,                                           /* confine_to */
        None);                                          /* cursor */

    XtAddRawEventHandler ((Widget) w, ButtonPressMask, 0, 
				(XtEventHandler) ButtonPressHandler, 0);
    XtAddRawEventHandler ((Widget) w, PointerMotionMask, 0,
				(XtEventHandler) PointerMotionHandler, 0);
    XtAddRawEventHandler ((Widget) w, ButtonReleaseMask, 0, 
				(XtEventHandler) ButtonReleaseHandler, 0);

    w->slist.leftMarginWidth = w->core.border_width;

    if (ResizeNeeded)
        DoResize (w);
}

static void DoResize(w)
    SListWidget w;
{
    Arg vsbArgs[30];
    int i;
    Boolean resized = False;

    if (w->core.height != w->slist.knownHeight) {
        resized = True;
        w->slist.knownHeight = w->core.height;
        SetYAdjust (w, 0);
#ifdef MOTIF
        w->slist.rows =	(int) (w->core.height - 
		2*w->primitive.shadow_thickness) / w->slist.cellHeight;
#else
        w->slist.rows = w->core.height / w->slist.cellHeight;
#endif
	w->slist.textHeight = w->slist.rows * w->slist.cellHeight;
        if (w->slist.vScroll != (Widget) NULL) {
            i = 0;
#ifdef MOTIF
            XtSetArg(vsbArgs[i], XmNvalue, 0);i++;
            XtSetArg(vsbArgs[i], XmNincrement, 1);i++;
            XtSetArg(vsbArgs[i], XmNpageIncrement, w->slist.rows-1);i++;
            if (w->slist.LineCnt > w->slist.rows) {
                XtSetArg(vsbArgs[i], XmNminimum,0);i++;
                XtSetArg(vsbArgs[i], XmNmaximum,w->slist.LineCnt);i++;
                XtSetArg(vsbArgs[i], XmNsliderSize, w->slist.rows);i++;
            } else {
                XtSetArg(vsbArgs[i], XmNminimum,0);i++;
                XtSetArg(vsbArgs[i], XmNmaximum,5);i++;
                XtSetArg(vsbArgs[i], XmNsliderSize,5);i++;
            }
            XtSetValues(w->slist.vScroll, vsbArgs, i);
#else
            XtSetArg(vsbArgs[i], DwtNvalue, 0);i++;
            if (w->slist.LineCnt > w->slist.rows) {
                XtSetArg(vsbArgs[i], DwtNminValue,0);i++;
                XtSetArg(vsbArgs[i], DwtNmaxValue,w->slist.LineCnt);i++;
                XtSetArg(vsbArgs[i], DwtNshown, w->slist.rows);i++;
		XtSetArg(vsbArgs[i], DwtNpageInc, w->slist.rows-1);i++;
            } else {
                XtSetArg(vsbArgs[i], DwtNminValue,0);i++;
                XtSetArg(vsbArgs[i], DwtNmaxValue,1);i++;
            }
            XtSetValues(w->slist.vScroll, vsbArgs, i);
#endif
        }
    }
    if (w->core.width != w->slist.knownWidth) {
        resized = True;
        w->slist.textWidth = w->core.width - 2 * (w->slist.textX);
        w->slist.knownWidth = w->core.width;
    }
    if (resized && w->core.window != (Window) NULL) {
        XMoveResizeWindow(XtDisplay(w), XtWindow(w),
                w->core.x, w->core.y, w->core.width, w->core.height);
     }
}

static void DoDestroy(w)
    SListWidget w;
{
}
XtActionProc slistForwardPage(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    SListWidget w = (SListWidget) widget;
    int topLine = w->slist.topLine;

    if (topLine < 0) topLine = -1;
    else if (topLine > w->slist.LineCnt) topLine = w->slist.LineCnt;
    SListSetTop(w, topLine + w->slist.rows - 1);
    return 0;
}
XtActionProc slistBackwardPage(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    SListWidget w = (SListWidget) widget;
    int topLine;

    topLine = w->slist.topLine;
    if (topLine < 0) topLine = -1;
    else if (topLine > w->slist.LineCnt) topLine = w->slist.LineCnt;
    SListSetTop(w, topLine - w->slist.rows + 1);
    return 0;
}
XtActionProc slistForwardLine(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    SListWidget w = (SListWidget) widget;
    int topLine = SListGetHighlight(w);
    int line;
    Boolean needsRepaint = 0;

    if (topLine >= 0) SListUnhighlight(w, topLine);
    for (line=0;line <w->slist.LineCnt; line++) {
	if (w->slist.selectMapP[line] != 0 && line != topLine) {
	    needsRepaint = 1;
	}
	w->slist.selectMapP[line] = 0;
    }
    if (needsRepaint)
        Repaint (w);

    if (topLine < 0) topLine = w->slist.topLine;
    if (topLine < 0) topLine = -1;
    topLine++;
    if (topLine >= w->slist.LineCnt) topLine = w->slist.LineCnt - 1;
    if (topLine < w->slist.topLine)
	SListSetTop(w, topLine);
    if (topLine >= (w->slist.topLine + w->slist.rows)) {
	SListSetTop(w, topLine - w->slist.rows + 1);
    }
    SListHighlight(w, topLine);
    return 0;
}
XtActionProc slistBackwardLine(widget, event, params, num_params)
Widget widget;
XEvent *event;
char **params;
Cardinal *num_params;
{
    SListWidget w = (SListWidget) widget;
    int topLine = SListGetHighlight(w);

    int line;
    Boolean needsRepaint = 0;

    if (topLine >= 0) SListUnhighlight(w, topLine);
    for (line=0;line <w->slist.LineCnt; line++) {
	if (w->slist.selectMapP[line] != 0 && line != topLine) {
	    needsRepaint = 1;
	}
	w->slist.selectMapP[line] = 0;
    }
    if (needsRepaint)
        Repaint (w);

    if (topLine < 0) topLine = w->slist.topLine;
    if (topLine < 0) topLine = 0;
    topLine--;
    if (topLine < 0) topLine = 0;
    SListHighlight(w, topLine);
    if (topLine < w->slist.topLine)
	SListSetTop(w, topLine);
    if (topLine >= (w->slist.topLine + w->slist.rows)) {
	SListSetTop(w, topLine - w->slist.rows + 1);
    }
    return 0;
}

static void ClassInitialize()
{
    slistwidgetclassrec.core_class.actions = slistActionsTable;
    slistwidgetclassrec.core_class.num_actions = XtNumber(slistActionsTable);
    slistwidgetclassrec.slist_class.translations =
	(String) XtParseTranslationTable(slistBindings);
    XtAddActions(slistActionsTable, XtNumber(slistActionsTable));
}

/*
 * Public routines
 */
void SListLoad(w, LineCnt)
    SListWidget w;
    int LineCnt;
{
    int line;
    char *exposeMapP;
    char *selectMapP;
    Arg vsbArgs[10];
    int i;

    w->slist.LineCnt = LineCnt;
    w->slist.topLine = 0;

    /* Reallocate the expose and select maps if not big enough. */

    if (w->slist.mapLen < LineCnt) {
        w->slist.mapLen = LineCnt + 100;
        XtFree (w->slist.exposeMapP);
        w->slist.exposeMapP =  XtMalloc (w->slist.mapLen);
        XtFree (w->slist.selectMapP);
        w->slist.selectMapP =  XtMalloc (w->slist.mapLen);
    }

    /* Clear the maps. */

    for (line=0, exposeMapP=w->slist.exposeMapP, selectMapP = w->slist.selectMapP;
         line<LineCnt; line++) {
        *(exposeMapP++) = 0;
        *(selectMapP++) = 0;
    }

    w->slist.highlightedLine = -1;
    w->slist.anyAreExposed = w->slist.YAdjust = 0;
    SetYAdjust (w, 0);
    if (w->slist.vScroll != (Widget) NULL) {
        i = 0;
#ifdef MOTIF
        XtSetArg(vsbArgs[i], XmNvalue, 0);i++;
        XtSetArg(vsbArgs[i], XmNincrement, 1);i++;
        XtSetArg(vsbArgs[i], XmNpageIncrement, w->slist.rows-1);i++;
        if (w->slist.LineCnt > w->slist.rows) {
            XtSetArg(vsbArgs[i], XmNminimum,0);i++;
            XtSetArg(vsbArgs[i], XmNmaximum,w->slist.LineCnt);i++;
            XtSetArg(vsbArgs[i], XmNsliderSize, w->slist.rows);i++;
        } else {
            XtSetArg(vsbArgs[i], XmNminimum,0);i++;
            XtSetArg(vsbArgs[i], XmNmaximum,5);i++;
            XtSetArg(vsbArgs[i], XmNsliderSize,5);i++;
        }
#else
        XtSetArg(vsbArgs[i], DwtNvalue, 0);i++;
        XtSetArg(vsbArgs[i], DwtNminValue, 0);i++;
        if (w->slist.LineCnt > w->slist.rows) {
            XtSetArg(vsbArgs[i], DwtNminValue, 0);i++;
            XtSetArg(vsbArgs[i], DwtNmaxValue, w->slist.LineCnt);i++;
            XtSetArg(vsbArgs[i], DwtNshown, w->slist.rows);i++;
	    XtSetArg(vsbArgs[i], DwtNpageInc, w->slist.rows-1);i++;
        } else {
            XtSetArg(vsbArgs[i], DwtNminValue, 0);i++;
            XtSetArg(vsbArgs[i], DwtNmaxValue, 1);i++;
        }
#endif
        XtSetValues(w->slist.vScroll, vsbArgs, i);
    }

    Repaint (w);
}

int SListGetTop(w)
    SListWidget w;
{
    int topLine = w->slist.topLine;

    if (topLine < 0) topLine = -1;
    else if (topLine > w->slist.LineCnt) topLine = w->slist.LineCnt;
    return topLine;
}
int SListGetBottom(w)
    SListWidget w;
{
    int botLine = w->slist.topLine + w->slist.rows;

    if (botLine < 0) botLine = -1;
    else if (botLine > w->slist.LineCnt) botLine = w->slist.LineCnt;
    return botLine;
}
int SListGetRows(w)
    SListWidget w;
{
    return w->slist.rows;
}
void SListSetTop(w, item)
    SListWidget w;
    int item;
{
    int topLine;
    Arg vsbArg[1];

    topLine = w->slist.topLine;
    if (topLine < 0) topLine = -1;
    else if (topLine > w->slist.LineCnt) topLine = w->slist.LineCnt;
    if (item > w->slist.LineCnt - w->slist.rows)
        item = w->slist.LineCnt - w->slist.rows;
    if (item < 0) item = 0;
    if (item == topLine) return;
    w->slist.YAdjust = item * w->slist.cellHeight;
    w->slist.topLine = item;
    if (w->slist.vScroll != (Widget) NULL) {
#ifdef MOTIF
        XtSetArg(vsbArg[0], XmNvalue, item);
#else
        XtSetArg(vsbArg[0], DwtNvalue, item);
#endif
        XtSetValues(w->slist.vScroll, vsbArg, 1);
    }
    Repaint(w);
}
void SListUpdateLine(w, line)
    SListWidget w;
    int line;
{
    Draw(w, line);
}
void SListSelectAll(w, select)
    SListWidget w;
    int select;
{
    int line;
    int needsRepaint = 0;

    for (line=0; line <w->slist.LineCnt; line++) {
        if (line != w->slist.highlightedLine &&
            w->slist.selectMapP[line] != select) {
            needsRepaint = 1;
            w->slist.selectMapP[line] = select;
        }
    }
    if (needsRepaint)
        Repaint (w);
}

void SListUnhighlight(w)
    SListWidget w;
{
    if (w->slist.highlightedLine == -1) return;

    SetLineSelection(w, w->slist.highlightedLine, False);
    w->slist.highlightedLine = -1;
}
void SListHighlight(w, line)
    SListWidget w;
    int line;
{
    if (line == w->slist.highlightedLine) return;       /* don't set it again */
    if (w->slist.highlightedLine != -1) {
        SetLineSelection(w, w->slist.highlightedLine, False);
    }    

    w->slist.highlightedLine = line;
    SetLineSelection(w, line, True);
}
int SListGetHighlight(w)
    SListWidget w;
{
    return w->slist.highlightedLine;
}

Widget SListCreate(pW, nameP, argsP, argCnt)
    Widget pW;
    char *nameP;
    Arg *argsP;
    int argCnt;
{
    return (XtCreateWidget (nameP, (WidgetClass) slistwidgetclass, pW,
		argsP, argCnt));
}

Boolean SListIsSelected(sl, line)
    SListWidget sl;
    int line;
{
    if (line > sl->slist.mapLen || line < 0)
        return False;

    return sl->slist.selectMapP[line];
}
int SListGetSelectionCount(sl)
    SListWidget sl;
{
    int count = 0;
    int i;

    for (i=0;i < sl->slist.LineCnt; i++) {
	if (sl->slist.selectMapP[i]) count++; 
    }
    return count;
}
#ifndef MOTIF
void SListInitializeForDRM()
{
    DwtRegisterClass (DRMwcUnknown, "SList", "SListCreate", SListCreate,
        slistwidgetclass);
}
#else
#ifdef USE_MRM
void SListInitializeForMRM()
{
    MrmRegisterClass (MrmwcUnknown, "SList", "SListCreate", SListCreate,
        slistwidgetclass);
}
#endif
#endif
