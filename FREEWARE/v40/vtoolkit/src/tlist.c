/* tlist.c - TList widget implementation module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the TList widget.

MODIFICATION HISTORY:

01-Jul-1994 (sjk) Bugfix - when neither colorList nor colorNameList are defined, set default foreground.  Thanks Jean-Paul!

03-Dec-1993 (sjk) Even more portability work.  Thanks Roy!

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

07-Oct-1993 (sjk) Bugfix XtSetValues to be XtVaSetValues.  Thanks Bill!

16-Sep-1993 (sjk) Bugfix typedef of colorNameList on Alpha/OSF.

14-Sep-1993 (sjk) Reapply fix from 27-May-1992 - Bugfix TList window resized too small confused scroll bars.

03-Sep-1993 (sjk/nfc) VToolkit V2.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

07-Nov-1991 (sjk) Bugfix font list being copied too late and XtSetValues of TListNfontTable resource.

06-Nov-1991 (sjk) More cleanup.

31-Jul-1991 (sjk) Cleanup for UNIX compile.

16-Jul-1991 (sjk) Set vertical scrollbar increment based on character height.  Change scrollbar adjustment sequence to
	avoid MOTIF warning message.

09-Apr-1991 (sjk) Add TListNcolorNameList resource.

03-Apr-1991 (sjk) Upgrade to MOTIF.  Use standard toolkit scrollbars and use VFrame widget as a parent to handle geometry
	management.  Add horizontal scrolling.

28-Nov-1991 (sjk) Add TListNscrollDelay resource.

06-Aug-1990 (sjk) Bugfix - New lines weren't properly exposed if the window could display more than the maximum number of
	lines in the history list.

26-Jul-1990 (sjk) Bugfix - Don't look at per_char information if there is none.  Some fixed fonts do not have this structure
	allocated.

30-Apr-1990 (sjk) Bugfix - Support TListAddLine and TListReset calls before the widget has been realized.

20-Apr-1990 (sjk) More portability work.

13-Apr-1990 (sjk) Bugfix - Window jittered if it was shorter than one character height plus top and bottom margins.

26-Mar-1990 (sjk) Add TListReset special routine.

05-Mar-1990 (sjk) Cast widget class record to proper type.

16-Feb-1990 (sjk) Add word wrap.

11-Oct-1989 (sjk) Initial entry, based on VList widget.
*/

#include <ctype.h>
#include "vtoolkit.h"
#include "vframe.h"
#include "tlist.h"
#include <X11/cursorfont.h>

typedef struct {					/* widget class contribution */
    int			notused;			/* not needed */
} TListClassPart;

typedef struct _TListClassRec {				/* widget class record */
    CoreClassPart	core_class;			/* core fields */
    TListClassPart	tlist_class;			/* private fields */
} TListClassRec, *TListClass;

typedef struct {					/* line array */
    char		*textP;				/* pointer to text for this line */
    int			lineWidth;			/* width of line in pixels (including margins) */
    char		coloridx;       		/* gc vector index */
    char		exposed;			/* line needs repaint (T/F) */
    char		fontidx;           		/* font index */
    char		continuation;			/* this is a continuation line (T/F) */
} TLine;

typedef struct _TListRec {				/* Widget record */
    CorePart		core;				/* Core widget fields */

    /* Resource fields */
    XmFontList		fontTableP;			/* font list */
    Pixmap		colorList;			/* color palette pixmap */
    char		**colorNamesPP;			/* color name list */
    Pixel		foreground;			/* color to use for text */
    int			marginHeight;			/* margin height in pixels */
    int			marginWidth;			/* margin width in pixels */
    int			maxLineCnt;			/* maximum number of lines */
    int			scrollDelay;			/* scroll delay */
    int			scrollFactor;			/* autoscroll speed factor */
    int			wordWrap;			/* word wrap enabled (T/F) */

    /* Private fields */
    int			beginZeroInit;			/* must be first */

    int			anyAreExposed;			/* some exposures may be pending */
    int			buttonIsDown;			/* we have the pointer grabbed (T/F) */
    int			charHeight;			/* char height in pixels */
    int			charWidth;			/* average character width */
    int			colorCnt;			/* number of colors in the color table */
    Pixel		*colorTableP;			/* address of color table */
    int			currentColoridx;		/* color set into gc */
    int			currentFontidx;			/* font set into gc */
    int			externalXAdjust;		/* acknowleged scrolling position */
    int			externalYAdjust;		/* acknowleged scrolling position */
    XFontStruct		**fontPP;			/* address of font pointer array*/
    int                 fontCnt;        		/* number of fonts in fontTable */
    int			freezeList;			/* freeze list updates */
    int			gcCnt;				/* current size of GC arrays */
    Widget		hScrollW;			/* horizontal scrollbar */
    int			ignoreResize;			/* ignore resize callback */
    int			ignoreScrollValueChanged;	/* ignore scrollbar value changed callback */
    int			internalXAdjust;		/* desired scrolling position */
    int			internalYAdjust;		/* desired scrolling position */
    int			logicalWidth;			/* width in pixels of widest line */
    int			maxPendingTLineCnt; 		/* lines allocated */
    GC			normgc;   			/* normal graphics context */
    int			pendingTLineCnt; 		/* lines used in pendingTLine array */
    int			requestedXAdjust;		/* requested scrolling position */
    int			requestedYAdjust;		/* requested scrolling position */
    TLine		*pendingTLinesP;		/* array of pending entries */
    int			scrollIsActive;			/* XCopyArea in progress */
    int			shownScrollXValue;		/* current setting for horizontal scrollbar */
    int			shownScrollYValue;		/* current setting for vertical scrollbar */
    int			tLineCnt;			/* lines used in TLine array */
    TLine		*tLinesP;			/* address of TLine array */
    int			updateIsActive;			/* update timer is pending (T/F) */
    Widget		vScrollW;			/* vertical scrollbar */
    int			xScrollStrength;		/* autoscroll speed factor */
    int			yScrollStrength;		/* autoscroll speed factor */
    int			yFontAdjust;			/* adjustment for text positioning */

    char		endZeroInit;			/* must be last */
} TListWidgetRec, *TListWidget;

static int ComputeLineWidth(TListWidget w, int fontidx, char *textP)
{
    return (2 * w->marginWidth + VtkComputeTextWidth (w->fontPP[fontidx], textP));
}

static int GetSegmentStrlen(TListWidget w, int width, int fontidx, char *textP)
{
    XFontStruct *fontP;
    unsigned char *chP = (unsigned char *)textP;
    XCharStruct *charP;
    unsigned int firstCol, numCols, c;
    int textStrlen = strlen (textP);
    int segmentStrlen = 0;
    int maxSegmentStrlen;

    if (!textStrlen) return (0);

    fontP = w->fontPP[fontidx];
    charP = fontP->per_char;
    firstCol = fontP->min_char_or_byte2;
    numCols = fontP->max_char_or_byte2 - firstCol + 1;

    while (textStrlen--) {
	c = *(chP++) - firstCol;

	if (c < numCols)
	    if (charP) width -= charP[c].width;
	    else width -= fontP->max_bounds.width;

	if (width > 0) segmentStrlen++;
	else break;
    }

    if (!segmentStrlen) segmentStrlen = 1;	/* at least one character */
    maxSegmentStrlen = segmentStrlen;		/* remember just in case we can't find any preceding whitespace */

    /* Back up to whitespace. */

    while (segmentStrlen && textP[segmentStrlen] && (!isspace (textP[segmentStrlen]))) segmentStrlen--;

    /* Since we can't break it, just display as much as possible. */

    if (!segmentStrlen) segmentStrlen = maxSegmentStrlen;

    return (segmentStrlen);
}

static void SetHScrollBarValue(TListWidget w)
{
    Widget hScrollW = w->hScrollW;

    if (w->requestedXAdjust == w->shownScrollXValue) return;
    if ((!hScrollW) || (!XtIsManaged (hScrollW))) return;

    w->ignoreScrollValueChanged = 1;

    XtVaSetValues (hScrollW,
	XmNvalue, w->requestedXAdjust,
	0);

    w->shownScrollXValue = w->requestedXAdjust;

    w->ignoreScrollValueChanged = 0;
}

static void SetVScrollBarValue(TListWidget w)
{
    Widget vScrollW = w->vScrollW;

    if (w->requestedYAdjust == w->shownScrollYValue) return;
    if ((!vScrollW) || (!XtIsManaged (vScrollW))) return;

    w->ignoreScrollValueChanged = 1;

    XtVaSetValues (vScrollW,
	XmNvalue, 	w->requestedYAdjust,
	0);

    w->shownScrollYValue = w->requestedYAdjust;

    w->ignoreScrollValueChanged = 0;
}

static void AdjustHScrollBar(TListWidget w)
{
    int logicalWidth, physicalWidth;
    Widget hScrollW = w->hScrollW;

    if ((!hScrollW) || (!XtIsManaged (hScrollW))) return;

    if ((physicalWidth = w->core.width) <= w->charWidth) physicalWidth = w->charWidth + 1; 
    if (w->wordWrap) logicalWidth = physicalWidth; else logicalWidth = w->logicalWidth;
    if (logicalWidth < physicalWidth) logicalWidth = physicalWidth;

    w->ignoreScrollValueChanged = 1;

    XtVaSetValues (hScrollW,
	XmNsliderSize,     	physicalWidth,
	XmNmaximum,  		logicalWidth,
	XmNvalue,     		w->requestedXAdjust,
	XmNpageIncrement,   	physicalWidth - w->charWidth,
	XmNincrement,	      	w->charWidth,
	0);

    w->shownScrollXValue = w->requestedXAdjust;

    w->ignoreScrollValueChanged = 0;
}

static void AdjustVScrollBar(TListWidget w)
{
    int logicalHeight, physicalHeight;
    Widget vScrollW = w->vScrollW;

    if ((!vScrollW) || (!XtIsManaged (vScrollW))) return;

    if ((physicalHeight = w->core.height) <= w->charHeight) physicalHeight = w->charHeight + 1;
    logicalHeight = 2 * w->marginHeight + w->tLineCnt * w->charHeight;
    if (logicalHeight < physicalHeight) logicalHeight = physicalHeight;

    w->ignoreScrollValueChanged = 1;

    XtVaSetValues (vScrollW,
	XmNsliderSize,		physicalHeight,
    	XmNmaximum,  		logicalHeight,
    	XmNvalue,     		w->requestedYAdjust,
    	XmNpageIncrement,   	physicalHeight - w->charHeight,
    	XmNincrement,       	w->charHeight,
	0);

    w->shownScrollYValue = w->requestedYAdjust;

    w->ignoreScrollValueChanged = 0;
} 

static void ResetRequestedAndExternalAdjust(TListWidget w)
{
    w->requestedXAdjust = w->externalXAdjust = w->internalXAdjust;	AdjustHScrollBar (w);
    w->requestedYAdjust = w->externalYAdjust = w->internalYAdjust;	AdjustVScrollBar (w);
}

static void SetInternalXYAdjust(TListWidget w, int xAdjust, int yAdjust)
{
    int maxXAdjust, maxYAdjust, physicalWidth, physicalHeight;

    if (w->wordWrap) maxXAdjust = 0;			/* no horizontal scrolling if word wrap is enabled */
    else {
        if ((physicalWidth = w->core.width) <= w->charWidth) physicalWidth = w->charWidth + 1;
        maxXAdjust = w->logicalWidth - physicalWidth;
	if (maxXAdjust < 0) maxXAdjust = 0;		/* window is not full */
    }

    if ((physicalHeight = w->core.height) <= w->charHeight) physicalHeight = w->charHeight + 1;
    maxYAdjust = 2 * w->marginHeight + w->tLineCnt * w->charHeight - physicalHeight;
    if (maxYAdjust < 0) maxYAdjust = 0;			/* window is not full */

    if (xAdjust > maxXAdjust) xAdjust = maxXAdjust;
    if (xAdjust < 0) xAdjust = 0;

    if (yAdjust > maxYAdjust) yAdjust = maxYAdjust;
    if (yAdjust < 0) yAdjust = 0;

    w->internalXAdjust = xAdjust;
    w->internalYAdjust = yAdjust;
}

static void MakeLastLineVisible(TListWidget w)
{
    SetInternalXYAdjust (w, 0, w->tLineCnt * w->charHeight + 2 * w->marginHeight - w->core.height);
}

static void Draw(TListWidget w, int line, TLine *tLineP)
{
    int x = w->marginWidth 			   - w->requestedXAdjust;
    int y = w->marginHeight + line * w->charHeight - w->requestedYAdjust;
    XFontStruct *fontP;
    Pixel color;

    tLineP->exposed = 0;

    if (!VtkIsRealized (w)) return;		/* no window yet */
    if (!tLineP->textP) return;			/* no text to draw */

    if (w->colorCnt) {
	if (w->currentColoridx != tLineP->coloridx) {
	    color = w->colorTableP[tLineP->coloridx];
	    XSetForeground (XtDisplay (w), w->normgc, color);
	    w->currentColoridx = tLineP->coloridx;
	}
    }

    if (w->currentFontidx != tLineP->fontidx) {
	fontP = w->fontPP[tLineP->fontidx];
	w->yFontAdjust = fontP->max_bounds.ascent;
	XSetFont (XtDisplay (w), w->normgc, fontP->fid);
	w->currentFontidx = tLineP->fontidx;
    }

    XDrawString (XtDisplay (w), XtWindow (w), w->normgc, x, y + w->yFontAdjust, tLineP->textP, strlen (tLineP->textP));
}

static void StartPendingScroll(TListWidget w)
{
    if (w->scrollIsActive) return; /* wait for pending op to complete */

    /* Check for autoscrolling */

    if (w->xScrollStrength || w->yScrollStrength)
	SetInternalXYAdjust (w,
	    w->internalXAdjust + w->xScrollStrength / w->scrollFactor,
	    w->internalYAdjust + w->yScrollStrength / w->scrollFactor);

    if ((w->internalXAdjust == w->requestedXAdjust) && (w->internalYAdjust == w->requestedYAdjust)) return;

    if (VtkIsRealized (w)) {
	XCopyArea (XtDisplay (w), XtWindow (w), XtWindow (w), w->normgc,
	    w->internalXAdjust - w->requestedXAdjust, w->internalYAdjust - w->requestedYAdjust,
	    w->core.width, w->core.height, 0, 0);

        w->scrollIsActive = 1;
	w->requestedXAdjust = w->internalXAdjust;		SetHScrollBarValue (w);
	w->requestedYAdjust = w->internalYAdjust;		SetVScrollBarValue (w);
    } else {
	ResetRequestedAndExternalAdjust (w);
    }
}

static void StartUpdateScroll(TListWidget w, int shuffleCnt)
{
    XCopyArea (XtDisplay (w), XtWindow (w), XtWindow (w), w->normgc,
	0, shuffleCnt * w->charHeight, w->core.width, w->core.height, 0, 0);

    w->scrollIsActive = 1;
}

static void HValueChangedCallback(Widget unused_sbW, XtPointer ow, XtPointer cbDataOP)
{
    TListWidget w = (TListWidget)ow;
    XmScrollBarCallbackStruct *cbDataP = (XmScrollBarCallbackStruct *)cbDataOP;

    if (w->ignoreScrollValueChanged) return;

    w->shownScrollXValue = w->internalXAdjust = cbDataP->value;

    StartPendingScroll (w);
}

static void VValueChangedCallback(Widget unused_sbW, XtPointer ow, XtPointer cbDataOP)
{
    TListWidget w = (TListWidget)ow;
    XmScrollBarCallbackStruct *cbDataP = (XmScrollBarCallbackStruct *)cbDataOP;

    if (w->ignoreScrollValueChanged) return;

    w->shownScrollYValue = w->internalYAdjust = cbDataP->value;

    StartPendingScroll (w);
}

static void FreeLines(TLine *tLineP, int freeCnt)
{
    while (freeCnt--) XtFree ((tLineP++)->textP);
}

static int ShuffleLines(TLine *tLineP, int oldCnt, int removeCnt)
{
    memmove (tLineP, &tLineP[removeCnt], (oldCnt - removeCnt) * sizeof(TLine));

    return (oldCnt - removeCnt);
}

static int CopyLines(TLine *toTLineP, TLine *fromTLineP, int oldCnt, int copyCnt)
{
    memmove (&toTLineP[oldCnt], fromTLineP, copyCnt * sizeof(TLine));

    return (oldCnt + copyCnt);
}

static void CopyLinesFromPending(TListWidget w, int copyCnt, int recomputeLogicalWidth)
{
    int line, oldLogicalWidth;

    w->tLineCnt = CopyLines (w->tLinesP, w->pendingTLinesP, w->tLineCnt, copyCnt);
    w->pendingTLineCnt = ShuffleLines (w->pendingTLinesP, w->pendingTLineCnt, copyCnt);

    w->anyAreExposed = 1;

    if (w->wordWrap) return;

    oldLogicalWidth = w->logicalWidth;

    if (recomputeLogicalWidth) {
	w->logicalWidth = 2 * w->marginWidth;

	for (line = 0; line < w->tLineCnt; line++)
	    if (w->tLinesP[line].lineWidth > w->logicalWidth) w->logicalWidth = w->tLinesP[line].lineWidth;
    } else {
	for (line = w->tLineCnt - copyCnt; line < w->tLineCnt; line++)
	    if (w->tLinesP[line].lineWidth > w->logicalWidth) w->logicalWidth = w->tLinesP[line].lineWidth;
    }

    if (w->logicalWidth != oldLogicalWidth) AdjustHScrollBar (w);
}

static void MoveLinesFromList(TListWidget w, int cnt)
{
    int line;
    int recomputeLogicalWidth = 0;

    if (!w->wordWrap)
	for (line = 0; line < cnt; line++)
	    if (w->tLinesP[line].lineWidth == w->logicalWidth) {
		recomputeLogicalWidth = 1;
		break;
	    }

    FreeLines (w->tLinesP, cnt);
    w->tLineCnt = ShuffleLines (w->tLinesP, w->tLineCnt, cnt);

    CopyLinesFromPending (w, cnt, recomputeLogicalWidth);
}

static void DrawNewLines(TListWidget w, int cnt)
{
    int line;
    TLine *tLineP;

    for (line = w->tLineCnt - cnt, tLineP = (&w->tLinesP[line]); line < w->tLineCnt; line++, tLineP++) Draw (w, line, tLineP);
}

static void StartSingleUpdate(XtPointer ow, XtIntervalId *unused_idP)
{
    TListWidget w = (TListWidget)ow;

    w->updateIsActive = 0;

    if (!w->pendingTLineCnt) return;

    if (w->maxLineCnt > w->tLineCnt) {				/* we're able to add a line without shuffling */
	CopyLinesFromPending (w, 1, 0);
	AdjustVScrollBar (w);
	DrawNewLines (w, 1);
	MakeLastLineVisible (w);
	StartPendingScroll (w);
	if (!w->scrollIsActive) StartSingleUpdate ((XtPointer)w, 0);
	return;
    }

    /* Add a line by shuffling. */

    MoveLinesFromList (w, 1);
    StartUpdateScroll (w, 1);
}

static void StartPendingUpdate(TListWidget w)
{
    int shuffleCnt, addCnt, remCnt;

    if (!w->pendingTLineCnt) return;	/* nothing to do */
    if (w->scrollIsActive) return;	/* wait for scroll to complete */
    if (w->freezeList) return;		/* wait for button release */

    w->anyAreExposed = 1;		/* new lines may need exposure */

    /* Phase 1.  Make sure we're positioned at the bottom of the list. */

    MakeLastLineVisible (w);
    StartPendingScroll (w);
    if (w->scrollIsActive) return;

    if (w->scrollDelay && VtkIsRealized (w)) {
	if (w->updateIsActive) return;
	XtAddTimeOut (w->scrollDelay, StartSingleUpdate, (XtPointer)w);
	w->updateIsActive = 1;
	return;
    }

    /* Phase 2.  Add as many entries as possible without shuffling. */

    if ((remCnt = (w->maxLineCnt - w->tLineCnt))) {			/* how much space remains in the TLine array */
	addCnt = w->pendingTLineCnt;					/* how many need to be posted */
	if (addCnt > remCnt) addCnt = remCnt;				/* how many we can move without shuffling */

	CopyLinesFromPending (w, addCnt, 0);
	AdjustVScrollBar (w);
	DrawNewLines (w, addCnt);
	MakeLastLineVisible (w);
	StartPendingScroll (w);

	if (w->scrollIsActive) return;					/* wait for scrolling to complete */
	if (!w->pendingTLineCnt) return;				/* we're all done */
    }

    /* Phase 3.  Add remainder by shuffling. */

    /* If the pending list is larger than the maximum allowed active list, prune it from the beginning. */

    if (w->pendingTLineCnt > w->maxLineCnt) {
	remCnt = w->pendingTLineCnt - w->maxLineCnt;

	FreeLines (w->pendingTLinesP, remCnt);
	w->pendingTLineCnt = ShuffleLines (w->pendingTLinesP, w->pendingTLineCnt, remCnt);
    }

    shuffleCnt = w->pendingTLineCnt;
    MoveLinesFromList (w, w->pendingTLineCnt);

    if (VtkIsRealized (w) && shuffleCnt) StartUpdateScroll (w, shuffleCnt);
}

static void StartFreeze(TListWidget w)
{
    w->freezeList = 1;
}

static void EndFreeze(TListWidget w)
{
    w->freezeList = 0;
    StartPendingUpdate (w);
}

static void ScrollButtonPressHandler(Widget scrollW, XtPointer ow, XEvent *eventOP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if ((eventP->button != Button1)
      || (eventP->state & (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))) return;

    StartFreeze (w);
}

static void ScrollButtonReleaseHandler(Widget scrollW, XtPointer ow, XEvent *eventOP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (!w->freezeList) return;

    EndFreeze (w);
}

static void NewMousePosition(TListWidget w, int x, int y)
{
    if (!w->tLineCnt) return;

    w->xScrollStrength = w->yScrollStrength = 0;

    /* Determine autoscroll strength. */

    if (x < 0) 				w->xScrollStrength = x;				/* to the left */
    else if (x >= w->core.width) 	w->xScrollStrength = x - w->core.width;		/* to the right */

    if (y < 0) 				w->yScrollStrength = y;				/* above top */
    else if (y >= w->core.height) 	w->yScrollStrength = y - w->core.height;	/* below bottom */

    StartPendingScroll (w);
}

static void ButtonPressHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (eventP->button != Button1) return;

    StartFreeze (w);
    w->buttonIsDown = 1;
    NewMousePosition (w, eventP->x, eventP->y);
}

static void PointerMotionHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    XMotionEvent *eventP = (XMotionEvent *)eventOP;

    if (!w->buttonIsDown) return;

    NewMousePosition (w, eventP->x, eventP->y);
}

static void ButtonReleaseHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (eventP->button != Button1) return;
    if (!w->buttonIsDown) return;

    NewMousePosition (w, eventP->x, eventP->y);

    w->xScrollStrength = w->yScrollStrength = 0;
    w->buttonIsDown = 0;

    EndFreeze (w);
}

static void DrawExposedLines(TListWidget w)
{
    int line;
    TLine *tLineP;

    if (!w->anyAreExposed) return;	/* nothing to do */
    if (w->scrollIsActive) return;	/* wait for scroll to complete */

    for (line = 0, tLineP = w->tLinesP; line < w->tLineCnt; line++, tLineP++) if (tLineP->exposed) Draw (w, line, tLineP);

    w->anyAreExposed = 0;
}

static void SaveExposeRegion(TListWidget w, int vY, int height)
{
    int firstLine, lastLine, line;

    firstLine = (vY - w->marginHeight) / w->charHeight;
    if (firstLine < 0) firstLine = 0;
    if (firstLine >= w->tLineCnt) return;

    lastLine = (vY + height - 1 - w->marginHeight) / w->charHeight;
    if (lastLine >= w->tLineCnt) lastLine = w->tLineCnt - 1;
    if (lastLine < 0) return;

    for (line = firstLine; line <= lastLine; line++) w->tLinesP[line].exposed = w->anyAreExposed = 1;
}

static void DoExpose(Widget ow, XEvent *eventP, Region unused_region)
{
    TListWidget w = (TListWidget)ow;

    XClearArea (XtDisplay (w), XtWindow (w),
	eventP->xexpose.x + w->externalXAdjust - w->requestedXAdjust,
	eventP->xexpose.y + w->externalYAdjust - w->requestedYAdjust,
	eventP->xexpose.width, eventP->xexpose.height, 0);

    SaveExposeRegion (w,
	eventP->xexpose.y + w->externalYAdjust,
	eventP->xexpose.height);

    /* If this is not a primary Expose event, simply return. */

    if (eventP->xexpose.count) return;

    DrawExposedLines (w);
}

static void HandleNonmaskableEvent(Widget ow, XtPointer unused_closure, XEvent *eventP, Boolean *unused_continueP)
{
    TListWidget w = (TListWidget)ow;
    int type = eventP->xany.type;

    if (type == GraphicsExpose)
	SaveExposeRegion (w,
	    eventP->xgraphicsexpose.y + w->requestedYAdjust,
	    eventP->xgraphicsexpose.height);

    /* Handle completion of the scroll operation. */

    if ((type == NoExpose) || ((type == GraphicsExpose) && (!eventP->xgraphicsexpose.count))) {
	/* The external display is consistent with the last requested. */

	w->externalXAdjust = w->requestedXAdjust;
	w->externalYAdjust = w->requestedYAdjust;
	w->scrollIsActive = 0;

	DrawExposedLines (w);
	StartPendingUpdate (w);
	StartPendingScroll (w);
    }
}

static void InitializeTLineEntry(TListWidget w, TLine *tLineP, char *textP, int textStrlen, int fontidx,
	int coloridx, int continuation)
{
    tLineP->fontidx = fontidx;
    tLineP->coloridx = coloridx;
    tLineP->exposed = 1;
    tLineP->continuation = continuation;

    if (textStrlen) {
	tLineP->textP = (char *)XtMalloc (textStrlen + 1);
	memcpy (tLineP->textP, textP, textStrlen);
	tLineP->textP[textStrlen] = 0;		/* ascizify */
    } else tLineP->textP = 0;

    if (!w->wordWrap) tLineP->lineWidth = ComputeLineWidth (w, fontidx, tLineP->textP);
}

static void MakeSureLastEntryIsFree(TListWidget w)
{
    if (w->tLineCnt < w->maxLineCnt) return;

    FreeLines (w->tLinesP, 1);
    w->tLineCnt = ShuffleLines (w->tLinesP, w->tLineCnt, 1);
}

static void AddOneLine(TListWidget w, char *textP, int textStrlen, int fontidx, int coloridx, int continuation)
{
    /* Extend the pendingTLine array if necessary. */

    if (w->pendingTLineCnt == w->maxPendingTLineCnt) {
	w->maxPendingTLineCnt += 8;		/* somewhat arbitrary */
	w->pendingTLinesP = (TLine *)XtRealloc ((char *)w->pendingTLinesP, w->maxPendingTLineCnt * sizeof (TLine));
    }

    InitializeTLineEntry (w, &w->pendingTLinesP[w->pendingTLineCnt++], textP, textStrlen, fontidx, coloridx, continuation);
}

void TListAddLine(Widget ow, char *textP, int fontidx, int coloridx)
{
    TListWidget w = (TListWidget)ow;
    int segmentStrlen, continuation, width;
    int textStrlen = strlen (textP);

    fontidx = (unsigned int)fontidx % w->fontCnt;

    if (w->colorCnt) coloridx = (unsigned int)coloridx % w->colorCnt;
    else	     coloridx = 0;

    if (!textStrlen)			             AddOneLine (w, 0, 0, 0, 0, 0);
    else if ((!w->wordWrap) || (!VtkIsRealized (w))) AddOneLine (w, textP, textStrlen, fontidx, coloridx, 0);
    else {
	continuation = 0;

	width = w->core.width - 2 * w->marginWidth;
	if (width <= 0) width = 1;

	while ((segmentStrlen = GetSegmentStrlen (w, width, fontidx, textP))) {
	    AddOneLine (w, textP, segmentStrlen, fontidx, coloridx, continuation);

	    continuation = 1;
	    textP += segmentStrlen;
	}
    }

    StartPendingUpdate (w);
}

void TListReset(Widget ow)
{
    TListWidget w = (TListWidget)ow;

    FreeLines (w->tLinesP, w->tLineCnt);
    w->tLineCnt = 0;

    FreeLines (w->pendingTLinesP, w->pendingTLineCnt);
    w->pendingTLineCnt = 0;

    w->logicalWidth = 2 * w->marginWidth;

    SetInternalXYAdjust (w, 0, 0);
    ResetRequestedAndExternalAdjust (w);

    w->scrollIsActive = 0;

    if (VtkIsRealized (w)) XClearArea (XtDisplay (w), XtWindow (w), 0, 0, w->core.width, w->core.height, 0);
}

static void DoInitialize(Widget unused_requestOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    TListWidget w = (TListWidget)ow;

    VtkSetFieldsToZero ((char *)&w->beginZeroInit, (char *)&w->endZeroInit);

    w->currentFontidx = w->currentColoridx = (-1);

    w->tLinesP = (TLine *)XtMalloc (w->maxLineCnt * sizeof (TLine));

    /* Register an event handler for nonmaskable events.  In particular,
     * we are interested in GraphicsExpose and NoExpose events. */

    XtAddEventHandler ((Widget)w, 0, 1, HandleNonmaskableEvent, w);

    if (!w->fontTableP) w->fontTableP = VtkGetDefaultFontList ((Widget)w);

    VtkGetFontA (w->fontTableP, &w->fontPP, &w->fontCnt);
    VtkGetCharWidthAndHeight (w->fontPP, w->fontCnt, &w->charWidth, &w->charHeight);
}

static void DoResize(Widget ow)
{
    TListWidget w = (TListWidget)ow;
    TLine *tLineP, *oldTLineP;
    int i, textStrlen, oldTLineCnt, segmentStrlen, continuation, width;
    int fontidx, coloridx;
    char *textP = 0;
    char *segmentP;

    if (!VtkIsRealized (w)) return;
    if (w->ignoreResize) return;

    /* If we are not word wrapping, position to end-of-list and repaint. */

    if (!w->wordWrap) {
	MakeLastLineVisible (w);
	ResetRequestedAndExternalAdjust (w);
        return;
    }

    /* Steal the old list.  Rebuild it from scratch. */

    oldTLineP = w->tLinesP;
    oldTLineCnt = w->tLineCnt;
    w->tLinesP = (TLine *)XtMalloc (w->maxLineCnt * sizeof (TLine));
    w->tLineCnt = 0;

    /* Concatenate any pending entries to the old list. */

    if (w->pendingTLineCnt) {
	oldTLineP = (TLine *)XtRealloc ((char *)oldTLineP, (oldTLineCnt + w->pendingTLineCnt) * sizeof (TLine));

	oldTLineCnt = CopyLines (oldTLineP, w->pendingTLinesP, oldTLineCnt, w->pendingTLineCnt);

	w->pendingTLineCnt = 0;
    }

    /* Discard partial lines at the top of the list. */

    for (i = 0, tLineP = oldTLineP; (i < oldTLineCnt) && tLineP->continuation; i++, tLineP++) XtFree (tLineP->textP);

    while (i < oldTLineCnt) {
	if (tLineP->textP) textStrlen = strlen (tLineP->textP);
	else textStrlen = 0;

	textP = (char *)XtRealloc (textP, textStrlen + 1);
	textP[0] = 0;

	if (tLineP->textP) {
	    strcat (textP, tLineP->textP);
	    XtFree (tLineP->textP);
	}

        fontidx = tLineP->fontidx;
	coloridx = tLineP->coloridx;
        i++;
	tLineP++;

	/* Concatenate with continuation lines. */

	while ((i < oldTLineCnt) && tLineP->continuation) {
	    if (tLineP->textP) {
		textStrlen += strlen (tLineP->textP);
	        textP = (char *)XtRealloc (textP, textStrlen + 1);
	        strcat (textP, tLineP->textP);
	        XtFree (tLineP->textP);
	    }

	    i++;
	    tLineP++;
	}
    
	if (!textStrlen) {
     	    MakeSureLastEntryIsFree (w);
	    InitializeTLineEntry (w, (&w->tLinesP[w->tLineCnt++]), 0, 0, 0, 0, 0);
        } else {
	    continuation = 0;

	    width = w->core.width - 2 * w->marginWidth;
	    if (width <= 0) width = 1;

	    segmentP = textP;
	    while ((segmentStrlen = GetSegmentStrlen (w, width, fontidx, segmentP))) {
      	        MakeSureLastEntryIsFree (w);
    		InitializeTLineEntry (w, (&w->tLinesP[w->tLineCnt++]), segmentP, segmentStrlen, fontidx, coloridx, continuation);

		continuation = 1;
		segmentP += segmentStrlen;
	    }
	}
    }

    XtFree (textP);
    XtFree ((char *)oldTLineP);

    MakeLastLineVisible (w);
    ResetRequestedAndExternalAdjust (w);
}

static void ResizeCallback(Widget vFrameW, XtPointer ow, XtPointer unused_cbDataP)
{
    TListWidget w = (TListWidget)ow;

    if (!VtkIsRealized (w)) return;

    VFrameArrangeWidgets (vFrameW, (Widget)w, w->hScrollW, w->vScrollW);
}

static void DoRealize(Widget ow, XtValueMask *maskP, XSetWindowAttributes *attributesP)
{
    TListWidget w = (TListWidget)ow;
    Widget hScrollW, vScrollW;
    XGCValues values;

    if (!w->core.width)  w->core.width  = 1;
    if (!w->core.height) w->core.height = 1;

    XtCreateWindow ((Widget)w, InputOutput, CopyFromParent, *maskP, attributesP);

    if (w->colorList) 	      w->colorCnt = VtkCopyColors     ((Widget)w, w->colorList, 		  &w->colorTableP);
    else if (w->colorNamesPP) w->colorCnt = VtkCopyColorNames ((Widget)w, w->colorNamesPP, w->foreground, &w->colorTableP);

    values.background = w->core.background_pixel;
    values.function   = GXcopy;
    w->normgc = XCreateGC (XtDisplay (w), XtWindow (w), GCBackground | GCFunction, &values);

    /* Use default color if both colorList and colorNameList are undefined. */

    if (!w->colorCnt) XSetForeground (XtDisplay (w), w->normgc, w->foreground);

    /* Establish a passive grab on MB1. */

    XGrabButton (
	XtDisplay (w),							/* display */
	1,								/* button_grab */
	AnyModifier,							/* modifiers */
	XtWindow (w),							/* window */
	0,								/* owner_events */
	(ButtonPressMask | ButtonReleaseMask | Button1MotionMask),	/* event_mask */
	GrabModeAsync,							/* pointer_mode */
	GrabModeAsync,							/* keyboard_mode */
	None,								/* confine_to */
	XCreateFontCursor (XtDisplay (w), XC_fleur));			/* cursor */

    /* Register raw event handlers for the events that we will receive as a result of our passive button grab. */

    XtAddRawEventHandler ((Widget)w, ButtonPressMask,   0, ButtonPressHandler,   0);
    XtAddRawEventHandler ((Widget)w, PointerMotionMask, 0, PointerMotionHandler, 0);
    XtAddRawEventHandler ((Widget)w, ButtonReleaseMask, 0, ButtonReleaseHandler, 0);

    /* Take care of our scrollbars and parent's resize callback.  Trap button events on the scrollbar to freeze list updates. */

    w->hScrollW = (Widget)VFrameGetScrollBarWidget (w->core.parent, XmHORIZONTAL);
    w->vScrollW = (Widget)VFrameGetScrollBarWidget (w->core.parent, XmVERTICAL);

    /* If word wrap is taken as the default, set its value based on the presence or absence of a horizontal scrollbar. */

    if (w->wordWrap == (-1))
	if (w->hScrollW) w->wordWrap = 0;
	else		 w->wordWrap = 1;

    if ((hScrollW = w->hScrollW)) {
	XtAddCallback (hScrollW, XmNdragCallback, 	  HValueChangedCallback, w);
	XtAddCallback (hScrollW, XmNvalueChangedCallback, HValueChangedCallback, w);

        XtVaSetValues (hScrollW,
	    XmNminimum,			0,
	    XmNincrement,		1,
	    XmNpageIncrement,		1,
	    0);

	XtAddRawEventHandler (hScrollW, ButtonPressMask,   0, ScrollButtonPressHandler,   w);
	XtAddRawEventHandler (hScrollW, ButtonReleaseMask, 0, ScrollButtonReleaseHandler, w);
    }

    if ((vScrollW = w->vScrollW)) {
	XtAddCallback (vScrollW, XmNdragCallback, 	  VValueChangedCallback, w);
	XtAddCallback (vScrollW, XmNvalueChangedCallback, VValueChangedCallback, w);
    
	XtVaSetValues (vScrollW,
	    XmNminimum, 		0,
	    XmNincrement,		1,
	    XmNpageIncrement, 		1,
	    0);

	XtAddRawEventHandler (vScrollW, ButtonPressMask,   0, ScrollButtonPressHandler,   w);
	XtAddRawEventHandler (vScrollW, ButtonReleaseMask, 0, ScrollButtonReleaseHandler, w);
    }

    /* Trap resize events on our parent (VFrame) widget. */

    XtAddCallback (w->core.parent, VFrameNresizeCallback, ResizeCallback, w);

    VFrameArrangeWidgets (w->core.parent, (Widget)w, w->hScrollW, w->vScrollW);

    DoResize ((Widget)w);
}

static void DoDestroy(Widget unused_ow)
{
    /* to-be-done */
}

#define Changed(f) (w->f != oldW->f)

static Boolean DoSetValues(Widget oldOW, Widget unused_refOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    TListWidget oldW = (TListWidget)oldOW;
    TListWidget w = (TListWidget)ow;

    int recomputeLayout = 0;

    if (Changed (marginWidth))	recomputeLayout = 1;
    if (Changed (marginHeight))	recomputeLayout = 1;
    if (Changed (maxLineCnt))	recomputeLayout = 1;
    if (Changed (wordWrap)) 	recomputeLayout = 1;			/***** needs to rebuild list when disabling (sjk) */

    if (Changed (fontTableP)) {
	VtkGetFontA (w->fontTableP, &w->fontPP, &w->fontCnt);
	VtkGetCharWidthAndHeight (w->fontPP, w->fontCnt, &w->charWidth, &w->charHeight);
	recomputeLayout = 1;
    }

    if (recomputeLayout) {
	w->ignoreResize = 1;						/* don't do resize more than once */
	VFrameArrangeWidgets (w->core.parent, (Widget)w, w->hScrollW, w->vScrollW);
	w->ignoreResize = 0;
	DoResize ((Widget)w);
	return (1);
    }

    return (0);
}

#define XX(n, c, r, s, o, dr, dv)	{n, c, r, sizeof(s), XtOffset (TListWidget, o), dr, (char *)(dv)}

static XtResource resources[] = {

XX (TListNcolorList,	 XtCValue,	XtRPixmap,	Pixmap,		colorList,	XtRImmediate,	0			),
XX (TListNcolorNameList, XtCValue,	XtRPointer,	char **,	colorNamesPP,	XtRImmediate,	0			),
XX (TListNfontTable,	 XmCFontList,	XmRFontList,	XmFontList,	fontTableP,	XtRString,	0			),
XX (TListNforeground,    XtCForeground, XtRPixel, 	Pixel,	   	foreground,   	XtRString, 	XtDefaultForeground	),
XX (TListNmarginHeight,  XtCHeight, 	XtRInt,     	int,	   	marginHeight, 	XtRImmediate, 	6			),
XX (TListNmarginWidth,   XtCWidth, 	XtRInt,      	int,	   	marginWidth,  	XtRImmediate, 	8			),
XX (TListNmaxLineCnt,    XtCValue, 	XtRInt,      	int,	   	maxLineCnt,   	XtRImmediate, 	200			),
XX (TListNscrollDelay,   XtCValue, 	XtRInt,      	int, 		scrollDelay,	XtRImmediate,	0			),
XX (TListNscrollFactor,  XtCValue, 	XtRInt,      	int,	   	scrollFactor, 	XtRImmediate, 	4			),
XX (TListNwordWrap,      XtCValue, 	XtRInt,      	int,	   	wordWrap,     	XtRImmediate, 	-1			),

};

externaldef(tlistwidgetclassrec) TListClassRec tlistwidgetclassrec = {
    {/* core_class fields	*/
	/* superclass		*/	&widgetClassRec,
	/* class_name	  	*/	"TList",
	/* widget_size	  	*/	sizeof(TListWidgetRec),
	/* class_initialize   	*/    	0,
	/* class_part_initialize */	0,
	/* class_inited       	*/	0,
	/* initialize	  	*/	DoInitialize,
	/* initialize_hook	*/	0,
	/* realize		*/	DoRealize,
	/* actions		*/    	0,
	/* num_actions	  	*/	0,
	/* resources	  	*/	resources,
	/* num_resources	*/	XtNumber(resources),
	/* xrm_class	  	*/	0,
	/* compress_motion	*/	0,
	/* compress_exposure  	*/	0,
	/* compress_enterleave	*/	0,
	/* visible_interest	*/	0,
	/* destroy		*/	DoDestroy,
	/* resize		*/	DoResize,
	/* expose		*/	DoExpose,
	/* set_values	  	*/	DoSetValues,
	/* set_values_hook	*/	0,
	/* set_values_almost  	*/	(XtAlmostProc)_XtInherit,
	/* get_values_hook    	*/	0,
	/* accept_focus	  	*/	0,
	/* version		*/	XtVersionDontCheck,
	/* callback_private   	*/	0,
	/* tm_table		*/	0,
	/* query_geometry	*/	0,
	/* display_accelerator	*/	0,
	/* extension		*/	0
  }
};

externaldef(tlistwidgetclass) WidgetClass tlistwidgetclass = (WidgetClass)&tlistwidgetclassrec;

Widget TListCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
{
    return (XtCreateWidget (nameP, tlistwidgetclass, pW, argsP, argCnt));
}

void TListInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "TList", "TListCreate", TListCreate, tlistwidgetclass);
}
