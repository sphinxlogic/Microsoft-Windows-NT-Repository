/* vlist.c - VList widget implementation module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the VList widget.

MODIFICATION HISTORY:

08-Aug-1994 (sjk) Bugfix - clear LabelInfo array to zeroes when first allocated to avoid possible strange widths
	being looked at before widget has been realized.  Thanks to Dave Campbell for this!

12-May-1994 (sjk) Bugfix - don't overflow XCopyArea height when inserting a line in the middle of the list.

20-Jan-1993 (sjk) Bugfix - don't try to change GCs in DoSetValues prior to widget realization.  The GCs are not created yet!

03-Dec-1993 (sjk) Even more portability work.  Thanks Roy!

29-Oct-1993 (sjk) Add VListNselectionIsLocked resource.

22-Oct-1993 (sjk) Add VListNgridStyle and VListNgridPixmap resources.

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

17-Sep-1993 (sjk) Fix typedefs from VToolkit notes 162.  Thanks Phil!
	Reapply change from 18-MAY-1993 notesfile - Make closurePP argument optional on VListGetAllSelectedLines.
	Reapply bugfix from 05-May-1992 - Fix timing problem with quick add/remove combination and scrolling.
	Reapply bugfix from 10-Apr-1991 - Allow dynamic changing of background color.
	Add VListLineToCell from notesfile note 12.

16-Sep-1993 (sjk) Bugfix Alpha/OSF typedef problem.  Add (partial) DoDestroy callback. Thanks Jean-Paul!

13-Sep-1993 (sjk) Bugfix when setting labelCount to zero.  (VToolkit note 161)

08-Sep-1993 (sjk/nfc) VToolkit V2.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

03-Jan-1992 (sjk) Add VListLine.type field.  Support changing VListNfontTable and VListNcolorList resources with XtSetValues.

30-Dec-1991 (sjk) Integrated changes from JE_BRENNAN.  Support XtGetValues for VListNlabels and VListNfields resources.
	Modified LabelInfo and removed FieldInfo structure.  Changed logic so that user-supplied label and field lists are
	cloned rather than embedded in LabelInfo and FieldInfo lists.

11-Dec-1991 (sjk) Bugfix - Avoid divide by zero when calling VListComputePreferredWidth before widget is realized
	and avoid divide by zero in histogram scaling if min and max values are the same.

11-Dec-1991 (sjk) Add VListNallowDeadspace resource.

19-Nov-1991 (sjk) Add VListNspaceTiersEqually resource.

19-Nov-1991 (sjk) Add VListChangeCell* routines, allowing application to pass field/row combination rather than
	requiring line closure to be passed.  These new routines are more efficient than the VListChangeLine* routines
	because the ClosureToLine search is avoided.

19-Nov-1991 (sjk) Change default value for VListNspaceGroupsEqually to false, the old default.

15-Nov-1991 (sjk) Add VListNspaceGroupsEqually resource.

08-Nov-1991 (sjk) Add VListNdragArmCallback and VListNdragDropCallback.  This incorporates drag-and-drop
	logic into the widget itself reducing application complexity.

06-Nov-1991 (sjk) Defer VListMakeLineVisible calls while scroll bar is grabbed by user to avoid "stealing" scroll position.

31-Oct-1991 (sjk) Add VListNfieldResizePolicy and VListField.resizePolicy.  Reduce flicker when adding lines.

28-Oct-1991 (sjk) Add VListNselectButton resource.  Honor VFieldWidth as a minimum width for the field (not including margins).
    Implement multi-line segmented text.  Add VListGetFieldLines routine.  Change default VListNdashOn resource value
    to workaround SPX server bug.

25-Oct-1991 (sjk) Add VListNiconMarginWidth and VListNiconMarginHeight resources, and compute rowHeight dynamically
	based on actual contents.  Reformat source and other cosmetic changes.

27-Sep-1991 (sjk) Bugfix - invalidate w->yFontAdjust if w->rowHeight resource is changed.

26-Aug-1991 (sjk) Check for change in field and label counts in DoSetValues.

29-Aug-1990 (sjk) Add background color support on a per-line basis.

28-Aug-1990 (sjk) Bugfix VListGetAllLines to not return deleted lines that have not yet been purged from the line array.

28-Aug-1990 (sjk) Bugfix ClosureToLine routine so that deleted lines are ignored.  This allows the application to re-use
	closure values while deleted lines are asynchronously purged from the line array.

16-Aug-1990 (sjk) Add VListGetAllLines routine.

31-Jul-1990 (sjk) Bugfix cellHeight vs. tierHeight (introduced when icon support was added).

20-Jul-1990 (sjk) Add icon support.

10-May-1990 (sjk) Use portable font routines, part of MOTIF BL1.1 port.

20-Apr-1990 (sjk) More portability work.

28-Mar-1990 (sjk) VListSelectStyleNone.

27-Mar-1990 (sjk) Multi-segment histogram bars.

21-Mar-1990 (sjk) More work on the multiple repaint problem.

19-Mar-1990 (sjk) Improve expose and resize handling to avoid (minimize?) likelihood of multiple repaints.  Clear entire
	window on partial expose during batch update.

16-Mar-1990 (sjk) Bugfix - When copying field information, the grid pointers were not being properly set to zero for
	text-type fields.

09-Mar-1990 (sjk) Properly handle font and color indices that are negative.  Add VListGetLineCount routine.

08-May-1990 (sjk) Change default value for VListNfieldSpacingWidth to zero.

06-Mar-1990 (sjk) Add histogram support.

06-Mar-1990 (sjk) Remove VListChangeLinePixel.  Applications must change to use VListChangeLineColoridx instead.  Remove
	~PixelMask usage that was added on 21-Feb-1990.  Use existence / non-existence of colorTable to infer whether
	coloridx is a color index or a pixel value.

05-Mar-1990 (sjk) Add VListNuserData resource to be consistent with (most) toolkit widgets.  Add selectionCount field to
    VListSelectCallbackStruct to return the (new) number of selected lines.

21-Feb-1990 (sjk) Add support for direct specification of a pixel color value in addition to the indirect coloridx specification.

16-Feb-1990 (sjk) Fix bug in the width calculation of 8-bit characters.

15-Feb-1990 (sjk) Change VListVisibleInWindow option to ensure that the upper/left corner of the line item is made visible.
	Previously, if the item was wider than the window, the window was sometimes scrolled to show the bottom/right corner
	(depending on the initial position).

31-Jan-1990 (sjk) If slave widgets (scrollbars and header) have not been passed to the VList widget by the time it is realized,
	sniff the parent widget's child list to try to find them ourselves.  This gets around a DRM bug that was keeping VList
	from working under DECwindows V1.

31-Jan-1990 (sjk) Make necessary changes to improve portability to RISC.

26-Jan-1990 (sjk) Add VListSetAutoScrollStrength routine.  Add VListSelectCallbackStruct.selectChangesP and .lineCount.

23-Jan-1990 (sjk) Make widget a subclass of composite.  Add stippled text.

22-Jan-1990 (sjk) Add header label (dashed line) padding options.

19-Jan-1990 (sjk) Add MOTIF support.

16-Jan-1990 (sjk) Bugfix - Column layout was sometimes recomputed unnecessarily.  The column width reference count was not
	maintained properly in all cases.

11-Jan-1990 (sjk) Add segmented string support.  Delete lines in reverse order to minimize shuffling.

10-Jan-1990 (sjk) Bugfix - In VListSelectStyleSingle, downclick must not select any line that is not directly under the mouse.
	Also, don't return a closure on a double-click that is over deadspace.

09-Jan-1990 (sjk) Add VListComputePreferredWidth routine.

05-Jan-1990 (sjk) Bugfix - VListGetLineInfo returns 1 on success.  Bugfix - don't try to redraw lines that are not visible during
    an autoscrolled drag select.

04-Jan-1990 (sjk) Don't allow user to assign a zero closure value.

03-Jan-1990 (sjk) Add VListAlignmentLeft and ~Right, and remove column and tier fields from VListSelectCallbackStruct.  Bugfix
	recalculation of logicalHeight and tierCnt when adding and removing a line.

28-Dec-1989 (sjk) Version X2.0 update.  Contributions from Tom Foley, Keith Fieldhouse, and Dave Burleigh, among others.

17-Nov-1988 (sjk) Changes for stand-alone usage and public consumption.
*/

#include "vtoolkit.h"
#include "vlistp.h"
#include "vframe.h"
#include "vheader.h"

#define FOR_EACH_LINE_L(w, line, lineInfoP, firstLine, increment)						\
    for (lineInfoP = LineToLineInfo (w, firstLine), line = firstLine; 						\
	line < (w)->lineCnt;											\
	lineInfoP += (increment), line += (increment))

#define FOR_EACH_LINE(w, line, lineInfoP)									\
    FOR_EACH_LINE_L (w, line, lineInfoP, 0, 1)

#define FOR_EACH_LINE_IN_RECORD(w, line, lineInfoP, field, vFieldP, firstLine)					\
    for (line = firstLine, vFieldP = FieldToVField (w, 0),							\
	lineInfoP = LineToLineInfo (w, line), field = 0;							\
	field < (w)->fieldCnt;											\
	line++, vFieldP++, lineInfoP++, field++) 								\

#define FOR_EACH_FIELD(w, field, vFieldP)									\
    for (vFieldP = (w)->fieldsP, field = 0;									\
	field < (w)->fieldCnt;											\
	vFieldP++, field++)

#define FOR_EACH_LABEL(w, label, labelInfoP, vLabelP)								\
    for (labelInfoP = (w)->labelInfosP, vLabelP = (w)->labelsP, label = 0;					\
	label < (w)->labelCnt;											\
	labelInfoP++, vLabelP++, label++)

#define FOR_EACH_SEGMENT_L(lineInfoP, segment, segmentInfoP, firstSegment)					\
    for (segmentInfoP = SegmentToSegmentInfo (lineInfoP, firstSegment), segment = firstSegment;			\
	segment < lineInfoP->vLine.segmentCount;								\
	segmentInfoP++, segment++) 

#define FOR_EACH_SEGMENT(lineInfoP, segment, segmentInfoP)							\
    FOR_EACH_SEGMENT_L (lineInfoP, segment, segmentInfoP, 0)

#define FOR_EACH_GRID(vFieldP, grid, gridP)									\
    for (gridP = vFieldP->gridsP, grid = 0;									\
	grid < vFieldP->gridCount;										\
	gridP++, grid++)

#define FOR_EACH_GROUP(w, group)										\
    for (group = 0; group < (w)->groupCnt; group++)

#define FOR_EACH_BAR(vLineP, bar, barP)										\
    for (barP = vLineP->barsP, bar = 0;										\
	bar < vLineP->barCount;											\
	barP++, bar++)

#define LineToLineInfo(w, line)				(&(w)->lineInfosP[line])
#define FieldToVField(w, field)				(&(w)->fieldsP[field])
#define ColumnToColumnInfo(w, column)			(&(w)->columnInfosP[column])
#define SegmentToSegmentInfo(lineInfoP, segment)	(&lineInfoP->segmentInfosP[segment])

#define LineToFirstLineInRecord(w, line)		((line) / (w)->fieldCnt * (w)->fieldCnt)
#define GroupFieldToColumn(w, group, field)		((group) * (w)->fieldCnt + (field))
#define ColumnToGroup(w, column)			((column) / (w)->fieldCnt)
#define ColumnToFirstColumnInGroup(w, column)		(ColumnToGroup (w, column) * (w)->fieldCnt)
#define ColumnGroupToField(w, column, group)            ((column) - (group) * (w)->fieldCnt)

#define FontidxToFont(w, fontidx)			((w)->fontPP[((unsigned int)(fontidx) % (w)->fontCnt)])

#define LineToRow(w, line)				((line) / w->fieldCnt)
#define LineToField(w, line)				((line) % w->fieldCnt)

static int ClosureToLine(VListWidget w, Opaque closure)
{
    int line;
    LineInfo *lineInfoP;

    FOR_EACH_LINE (w, line, lineInfoP) if (lineInfoP->vLine.closure == closure) return (line);
										return (-1);
}

static int RowToFirstLine(VListWidget w, int row)
{
    if ((row < 0) || (row >= w->rowCnt)) return (-1);
    			  		 return (row * w->fieldCnt);
}

static int FieldRowToLine(VListWidget w, int field, int row)
{
    if ((field < 0) || (field >= w->fieldCnt) || (row < 0) || (row >= w->rowCnt)) return (-1);
    return (row * w->fieldCnt + field);
}


static int CellIsVisible(VListWidget w, int x, int y, int width, int height)
{
    /* Cast width and height to avoid unsigned comparison problems. */

    if ((x >= (int)w->core.width) || ((x + width) <= 0) || (y >= (int)w->core.height) || ((y + height) <= 0)) return (0);
    													      return (1);
}

static void ColumnTierToCell(VListWidget w, int column, int tier, int *xP, int *yP, int *widthP, int *heightP)
{
    ColumnInfo *columnInfoP = ColumnToColumnInfo (w, column);

    *xP      = columnInfoP->x - w->requestedXAdjust;
    *yP      = w->topMarginHeight + tier * w->tierHeight - w->requestedYAdjust;
    *widthP  = columnInfoP->width;
    *heightP = w->cellHeight;
}

static void GroupFieldToLines(VListWidget w, int group, int field, int *firstLineP, int *lastLineP)
{
    int firstLine = (*firstLineP) = group * w->tierCnt * w->fieldCnt + field;
    int lastLine  = firstLine + (w->tierCnt - 1) * w->fieldCnt;

    if (lastLine >= w->lineCnt) lastLine = (w->lineCnt - w->fieldCnt + field);

    *lastLineP = lastLine;
}

static int ColumnTierToLine(VListWidget w, int column, int tier)
{
    int line;

    if ((column < 0) || (column >= w->columnCnt) || (tier < 0) || (tier >= w->tierCnt)) return (-1);
    line = ((column/w->fieldCnt) * (w->tierCnt-1) + tier) * w->fieldCnt + column;
    if (line >= w->lineCnt) 								return (-1);
    			    								return (line);
}

static void LineToColumnTier(VListWidget w, int line, int *columnP, int *tierP)
{
    int row   = line / w->fieldCnt;
    int group = row  / w->tierCnt;

    *columnP = line - (row - group) * w->fieldCnt;
    *tierP   = row - group 	    * w->tierCnt;
}

static int VXToColumn(VListWidget w, int vX)
{
    int column;
    ColumnInfo *columnInfoP;

    for (column = 0, columnInfoP = ColumnToColumnInfo (w, 0); (column < w->columnCnt) && (vX >= columnInfoP->x);
      column++, columnInfoP++) {}

    if (column == w->columnCnt) {
	columnInfoP--;
	if (vX >= (columnInfoP->x + columnInfoP->width)) return (column);
    }

    return (column - 1);
}

static int VYToTier(VListWidget w, int vY)
{
    int tier;

    if (vY < w->topMarginHeight) 				         return (-1);
    if ((tier = (vY - w->topMarginHeight) / w->tierHeight) > w->tierCnt) return (w->tierCnt);
    									 return (tier);
}

static int RectangleToLines(VListWidget w, int vX, int vY, int width, int height, 
	int *firstColumnP, int *lastColumnP, int *firstTierP, int *lastTierP)
{
    int firstColumn, lastColumn, firstTier, lastTier;

    firstColumn = VXToColumn (w, vX);
    lastColumn  = VXToColumn (w, vX + width - 1);

    if ((firstColumn == lastColumn) && ((firstColumn == (-1)) || (firstColumn == w->columnCnt))) return (0);

    if (firstColumn == (-1))         firstColumn++;
    if (lastColumn  == w->columnCnt) lastColumn--;

    firstTier = VYToTier (w, vY);
    lastTier  = VYToTier (w, vY + height - 1);

    if ((firstTier == lastTier) && ((firstTier == (-1)) || (firstTier == w->tierCnt))) return (0);

    if (firstTier == (-1))       firstTier++;
    if (lastTier  == w->tierCnt) lastTier--;

    *firstColumnP = firstColumn;
    *lastColumnP  = lastColumn;
    *firstTierP   = firstTier;
    *lastTierP    = lastTier;

    return (1);
}

static void XYToColumnTier(VListWidget w, int x, int y, int *columnP, int *tierP)
{
    int column;

    if (x < 0) *columnP = (-1);
    else if (x >= w->core.width) *columnP = w->columnCnt;
    else {
        column = VXToColumn (w, x + w->externalXAdjust);

	if ((w->selectUnit == VListSelectUnitRow) && (column != (-1)) && (column != w->columnCnt))
	    column = ColumnToFirstColumnInGroup (w, column);

	*columnP = column;
    }

    if (y < 0) 			  *tierP = (-1);
    else if (y >= w->core.height) *tierP = w->tierCnt;
    else 			  *tierP = VYToTier (w, y + w->externalYAdjust);
}

static void ColumnTierToFieldRow(VListWidget w, int column, int tier, int *fieldP, int *rowP)
{
    int group;

    if (column < 0) {
	group   = 0;
	*fieldP = (-1);
    } else if (column == w->columnCnt) {
	group   = w->groupCnt - 1;
	*fieldP = w->fieldCnt;
    } else {
        group   = ColumnToGroup (w, column);
	*fieldP = ColumnGroupToField (w, column, group);
    }

    if (tier < 0) tier = 0;
    if (tier >= w->tierCnt) tier = w->tierCnt - 1;

    *rowP = group * w->tierCnt + tier;
}

static int ValueToX(int minX, int maxX, int minValue, int maxValue, int value)
{
    int vRange = maxValue - minValue;

    if (!vRange) vRange = 1;

    return (minX + (((value - minValue) * (maxX - minX) + (vRange - 1)) / vRange));
}

static int ValuesToXs(int minX, int maxX, int minValue, int maxValue, int lowValue, int highValue, int *leftXP, int *rightXP)
{
    if (lowValue > highValue) {
	int t     = lowValue;
	lowValue  = highValue;
	highValue = t;
    }

    if ((highValue < minValue) || (lowValue > maxValue)) {
	*leftXP  = (-1);
	*rightXP = (-1);
	return (0);
    }

    if (highValue > maxValue) highValue = maxValue;
    if (lowValue  < minValue) lowValue  = minValue;

    *leftXP  = ValueToX (minX, maxX, minValue, maxValue, lowValue);
    *rightXP = ValueToX (minX, maxX, minValue, maxValue, highValue);

    return (1);
}

static void MinMax(int a, int b, int *minP, int *maxP)
{
    if (a < b) {*minP = a; *maxP = b;}
    else       {*minP = b; *maxP = a;}
}


static void GetPixmapGeometry(VListWidget w, Pixmap p, int *widthP, int *heightP)
{
    Window rootWindow;
    int x, y;
    unsigned int borderWidth, depth;

    if (!p) {
	(*widthP) = (*heightP) = 0;
	return;
    }

    if (p == w->latestPixmapId) {
	(*widthP)  = w->latestPixmapWidth;
	(*heightP) = w->latestPixmapHeight;
	return;
    }

    XGetGeometry (VtkDisplay (w), p, &rootWindow, &x, &y, (unsigned int *)widthP, (unsigned int *)heightP, &borderWidth, &depth);

    w->latestPixmapId     = p;
    w->latestPixmapWidth  = (*widthP);
    w->latestPixmapHeight = (*heightP);
}

static void GetTextGeometry(VListWidget w, int fontidx, char *textP, int textStrlen, int *widthP, int *heightP, int *ascentP)
{
    XFontStruct *fontP;
    unsigned char *chP;
    XCharStruct *charP;
    unsigned int firstCol, numCols, c, width;

    if (!textStrlen) {
	(*widthP) = (*heightP) = (*ascentP) = 0;
	return;
    }

    fontP 	= FontidxToFont (w, fontidx);
    chP 	= (unsigned char *)textP;
    charP 	= fontP->per_char;
    firstCol 	= fontP->min_char_or_byte2;
    numCols 	= fontP->max_char_or_byte2 - firstCol + 1;
    width 	= 0;

    while (textStrlen--) {
	c = (*(chP++)) - firstCol;
	if (c < numCols) 
	    if (charP) width += charP[c].width;
	    else       width += fontP->max_bounds.width;	/* FIXED font has no per-char information */
    }

    (*widthP)  = width;
    (*heightP) = fontP->max_bounds.descent + fontP->max_bounds.ascent;
    (*ascentP) = fontP->max_bounds.ascent;
}

static int ComputeBodyX(int x, int width, int bodyWidth, int marginWidth, int alignment)
{
    switch (alignment) {
	case VListAlignmentTopLeft	:
	case VListAlignmentLeft		:
	case VListAlignmentBottomLeft	: return (x + marginWidth);

	case VListAlignmentTop		:
	case VListAlignmentCenter	:
	case VListAlignmentBottom	: return (x + (width - bodyWidth) / 2);

	case VListAlignmentTopRight	:
	case VListAlignmentRight	:
	case VListAlignmentBottomRight	: return (x + width - marginWidth - bodyWidth);
    }

    return (0);		/* control does not reach here */
}

static int GetChunkGeometry(LineInfo *lineInfoP, int firstSegment, int *widthP, int *maxAscentP, int *maxDescentP)
{
    int segment, width = 0, maxAscent = 0, maxDescent = 0, descent;
    SegmentInfo *segmentInfoP;

    FOR_EACH_SEGMENT_L (lineInfoP, segment, segmentInfoP, firstSegment) {
	width += segmentInfoP->bodyWidth;

	if (maxAscent  < segmentInfoP->maxAscent) 					 maxAscent  = segmentInfoP->maxAscent;
	if (maxDescent < (descent = segmentInfoP->bodyHeight - segmentInfoP->maxAscent)) maxDescent = descent;

	if (segmentInfoP->vSegment.mask & VListSegmentNewLineMask) {
	    segment++;
	    break;
	}
    }

    (*widthP) 	   = width;
    (*maxAscentP)  = maxAscent;
    (*maxDescentP) = maxDescent;

    return (segment - firstSegment);
}

static void GetChunkLayout(int bodyX, int bodyWidth, int maxAscent, int maxDescent, 
	int chunkWidth, int alignment, int *chunkXP, 
	int *chunkHeightP, int *leftMarginWidthP, 
	int *rightMarginXP, int *rightMarginWidthP)
{
    int chunkX, rightMarginX;

    (*chunkHeightP) 	 		= maxAscent + maxDescent;
    (*chunkXP)	    	 = chunkX 	= ComputeBodyX (bodyX, bodyWidth, chunkWidth, 0, alignment);
    (*leftMarginWidthP)  		= chunkX - bodyX;
    (*rightMarginXP) 	 = rightMarginX = chunkX + chunkWidth;
    (*rightMarginWidthP) 		= bodyX + bodyWidth - rightMarginX;
}

static void ComputeHistInfo(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP)
{
    lineInfoP->bodyWidth  = vFieldP->width;
    lineInfoP->bodyHeight = w->histHeight;
}

static void ComputeIconInfo(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP)
{
    GetPixmapGeometry (w, lineInfoP->vLine.icon, (int *)&lineInfoP->bodyWidth, (int *)&lineInfoP->bodyHeight);
}

static void ComputeTextInfo(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP)
{
    int segment, segmentCnt, maxAscent, maxDescent, width;
    SegmentInfo *segmentInfoP;

    if (!lineInfoP->vLine.textP) lineInfoP->textStrlen = 0;
    else 			 lineInfoP->textStrlen = strlen (lineInfoP->vLine.textP);

    if (!lineInfoP->vLine.segmentCount) {
	GetTextGeometry (w, lineInfoP->vLine.fontidx, lineInfoP->vLine.textP, lineInfoP->textStrlen,
	    (int *)&lineInfoP->bodyWidth, (int *)&lineInfoP->bodyHeight, &lineInfoP->maxAscent);

	return;
    }

    FOR_EACH_SEGMENT (lineInfoP, segment, segmentInfoP)
	if (segmentInfoP->vSegment.type == VListSegmentTypeText)
	    GetTextGeometry (w, segmentInfoP->vSegment.fontidx,
	        lineInfoP->vLine.textP + segmentInfoP->vSegment.textPosition, segmentInfoP->vSegment.textStrlen,
	        (int *)&segmentInfoP->bodyWidth, (int *)&segmentInfoP->bodyHeight, 
		&segmentInfoP->maxAscent);
	else {
	    GetPixmapGeometry (w, segmentInfoP->vSegment.icon, (int *)&segmentInfoP->bodyWidth,
			(int *)&segmentInfoP->bodyHeight);
	    segmentInfoP->maxAscent = segmentInfoP->bodyHeight / 2;
	}

    lineInfoP->bodyWidth = lineInfoP->bodyHeight = segment = 0;

    while ((segmentCnt = GetChunkGeometry (lineInfoP, segment, &width, &maxAscent, &maxDescent))) {
	if (lineInfoP->bodyWidth < width) lineInfoP->bodyWidth = width;
	lineInfoP->bodyHeight += (maxAscent + maxDescent);
	segment += segmentCnt;
    }
}

static void ComputeCellInfo(VListWidget w, VListField *vFieldP, LineInfo *lineInfoP)
{
    switch (lineInfoP->vLine.type) {
      case VListLineTypeText	: ComputeTextInfo (w, lineInfoP, vFieldP);		break;
      case VListLineTypeIcon	: ComputeIconInfo (w, lineInfoP, vFieldP);		break;
      case VListLineTypeHist	: ComputeHistInfo (w, lineInfoP, vFieldP);		break;
    }
}

static void SupplySegmentDefaults(VListWidget w, LineInfo *lineInfoP)
{
    int segment;
    SegmentInfo *segmentInfoP;
    VListLine *vLineP = (&lineInfoP->vLine);
    VListSegment *vSegmentP;

    FOR_EACH_SEGMENT (lineInfoP, segment, segmentInfoP) {
	vSegmentP = (&segmentInfoP->vSegment);

	if (!(vSegmentP->mask & VListSegmentTypeMask))     vSegmentP->type     = VListSegmentTypeText;
	if (!(vSegmentP->mask & VListSegmentFontidxMask))  vSegmentP->fontidx  = vLineP->fontidx;
	if (!(vSegmentP->mask & VListSegmentColoridxMask)) vSegmentP->coloridx = vLineP->coloridx;
	if (!(vSegmentP->mask & VListSegmentStippledMask)) vSegmentP->stippled = vLineP->stippled;
	if (!(vSegmentP->mask & VListSegmentIconMask))     vSegmentP->icon     = vLineP->icon;
    }
}

static void SupplyBarDefaults(VListWidget w, VListLine *vLineP)
{
    int bar;
    VListBar *barP;

    FOR_EACH_BAR (vLineP, bar, barP) {
	if (!(barP->mask & VListBarLowValueMask))  barP->lowValue  = 0;
	if (!(barP->mask & VListBarHighValueMask)) barP->highValue = 100;
	if (!(barP->mask & VListBarColoridxMask))  barP->coloridx  = vLineP->coloridx;
	if (!(barP->mask & VListBarStippledMask))  barP->stippled  = vLineP->stippled;
    }
}

static void SupplyFieldDefaults(VListWidget w, VListField *vFieldP)
{
    if (!(vFieldP->mask & VListFieldTypeMask))         vFieldP->type 		= VListFieldTypeText;
    if (!(vFieldP->mask & VListFieldAlignmentMask))    vFieldP->alignment 	= w->alignment;
    if (!(vFieldP->mask & VListFieldMaxValueMask))     vFieldP->maxValue 	= 100;
    if (!(vFieldP->mask & VListFieldMinValueMask))     vFieldP->minValue 	= 0;
    if (!(vFieldP->mask & VListFieldResizePolicyMask)) vFieldP->resizePolicy 	= w->fieldResizePolicy;

    if (!(vFieldP->mask & VListFieldGridsMask)) {
	vFieldP->gridsP    = 0;
	vFieldP->gridCount = 0;
    }

    if (!(vFieldP->mask & VListFieldMarginWidthMask))
	switch (vFieldP->type) {
	  case VListFieldTypeText	: vFieldP->marginWidth = w->textMarginWidth;		break;
	  case VListFieldTypeHist 	: vFieldP->marginWidth = w->histMarginWidth;		break;
	  case VListFieldTypeIcon 	: vFieldP->marginWidth = w->iconMarginWidth;		break;
	}

    if (!(vFieldP->mask & VListFieldMarginHeightMask))
	switch (vFieldP->type) {
	  case VListFieldTypeText	: vFieldP->marginHeight = w->textMarginHeight;		break;
	  case VListFieldTypeHist 	: vFieldP->marginHeight = w->histMarginHeight;		break;
	  case VListFieldTypeIcon 	: vFieldP->marginHeight = w->iconMarginHeight;		break;
	}

    if (!(vFieldP->mask & VListFieldWidthMask))
	switch (vFieldP->type) {
	  case VListFieldTypeText	: vFieldP->width = 0;					break;
	  case VListFieldTypeIcon	: vFieldP->width = 0;					break;
	  case VListFieldTypeHist	: vFieldP->width = w->histWidth;			break;
	}
}

static void SupplyGridDefaults(VListWidget w, VListGrid *gridP)
{
    if (!(gridP->mask & VListGridRowMask))   gridP->row   = 0;
    if (!(gridP->mask & VListGridValueMask)) gridP->value = 0;
    if (!(gridP->mask & VListGridTextMask))  gridP->textP = 0;

    if (!(gridP->mask & VListGridDashMask)) {
	gridP->dashOn  = w->gridDashOn;
	gridP->dashOff = w->gridDashOff;
    }

    if (!(gridP->mask & VListGridColoridxMask))
	if (!w->colorList) gridP->coloridx = w->foreground;
	else 		   gridP->coloridx = 0;
}

static void SupplyLabelDefaults(VListWidget w, VListLabel *vLabelP, int label)
{
    if (!(vLabelP->mask & VListLabelTextMask))		vLabelP->textP 		= "";
    if (!(vLabelP->mask & VListLabelRowMask)) 		vLabelP->row 	 	= 0;
    if (!(vLabelP->mask & VListLabelFirstFieldMask)) 	vLabelP->firstField 	= label % w->fieldCnt;
    if (!(vLabelP->mask & VListLabelLastFieldMask))  	vLabelP->lastField  	= vLabelP->firstField;
    if (!(vLabelP->mask & VListLabelAlignmentMask))  	vLabelP->alignment  	= w->alignment;
    if (!(vLabelP->mask & VListLabelPadMask)) 		vLabelP->pad 	 	= w->labelPadOption;
}

static void SupplyLineDefaults(VListWidget w, VListLine *vLineP, VListField *vFieldP)
{
    if (!(vLineP->mask & VListLineTextMask))      vLineP->textP   	= 0;
    if (!(vLineP->mask & VListLineFontidxMask))   vLineP->fontidx 	= 0;
    if (!(vLineP->mask & VListLineSelectMask))    vLineP->select    	= 0;
    if (!(vLineP->mask & VListLineSensitiveMask)) vLineP->sensitive 	= 1;
    if (!(vLineP->mask & VListLineStippledMask))  vLineP->stippled  	= 0;
    if (!(vLineP->mask & VListLineIconMask)) 	  vLineP->icon 		= 0;
    if (!(vLineP->mask & VListLineTypeMask))	  vLineP->type	 	= vFieldP->type;
    
    if ((!(vLineP->mask & VListLineClosureMask)) || (!vLineP->closure))
	vLineP->closure = (Opaque)(++w->lastUsedClosure);
    
    if (!(vLineP->mask & VListLineColoridxMask))
	if (!w->colorList) vLineP->coloridx	= w->foreground;
	else 	           vLineP->coloridx   	= 0;

    if (!(vLineP->mask & VListLineBackgroundMask))
	if (!w->colorList) vLineP->background 	= w->core.background_pixel;
	else 	           vLineP->background 	= (-1);

    if (!(vLineP->mask & VListLineSegmentsMask)) {
	vLineP->segmentsP    	= 0;
	vLineP->segmentCount 	= 0;
    }

    if (!(vLineP->mask & VListLineBarsMask)) {
	vLineP->barsP    	= 0;
	vLineP->barCount 	= 0;
    }
}

static int ComputeBodyY(int y, int height, int bodyHeight, int marginHeight, int alignment)
{
    switch (alignment) {
	case VListAlignmentTopLeft	:
	case VListAlignmentTop		:
	case VListAlignmentTopRight	: return (y + marginHeight);

	case VListAlignmentLeft		:
	case VListAlignmentCenter	:
	case VListAlignmentRight	: return (y + (height - bodyHeight) / 2);

	case VListAlignmentBottomLeft	: 
	case VListAlignmentBottom	: 
	case VListAlignmentBottomRight	: return (y + height - marginHeight - bodyHeight);
    }

    return (0);			/* control does not reach here */
}

static void FreeFonts(VListWidget w)
{
    XtFree ((char *)w->fontPP);
}

static void FreeColors(VListWidget w)
{
    XtFree ((char *)w->colorTableP);
}

static void FreeLabels(VListWidget w)
{
    LabelInfo *labelInfoP;
    VListLabel *vLabelP;
    int label;

    FOR_EACH_LABEL (w, label, labelInfoP, vLabelP) XtFree (vLabelP->textP);

    XtFree ((char *)w->labelInfosP);		w->labelInfosP = 0;
    XtFree ((char *)w->labelsP);		w->labelsP     = 0;
}

static void FreeGrids(VListField *vFieldP)
{
    int grid;
    VListGrid *gridP;

    FOR_EACH_GRID (vFieldP, grid, gridP) XtFree (gridP->textP);
    XtFree ((char *)vFieldP->gridsP);
}

static void FreeFields(VListWidget w)
{
    int field;
    VListField *vFieldP;

    FOR_EACH_FIELD (w, field, vFieldP) FreeGrids (vFieldP);
    XtFree ((char *)w->fieldsP);
}

static void FreeLines(VListWidget w)
{
    int line;
    LineInfo *lineInfoP;

    FOR_EACH_LINE (w, line, lineInfoP) {
	XtFree (lineInfoP->vLine.textP);
	XtFree ((char *)lineInfoP->vLine.barsP);
	XtFree ((char *)lineInfoP->segmentInfosP);
    }

    XtFree ((char *)w->lineInfosP);
    w->lineInfosP = 0;
    w->maxLineCnt = 0;
}

static void CopyColors(VListWidget w)
{
    int i;
    int width, height;
    XImage *imageP;

    if (!w->colorList) return;		/* no table was supplied */

    /* What is the width of the color pixmap? */

    GetPixmapGeometry (w, w->colorList, &width, &height);

    /* Allocate the color table. */

    w->colorCnt    = width;
    w->colorTableP = (Pixel *)XtMalloc (w->colorCnt * sizeof (Pixel));

    /* Get an image of the color pixmap, copy the pixel values from that image to the color table, and destroy the image. */

    imageP = XGetImage (VtkDisplay (w), w->colorList, 0, 0, width, 1, -1, ZPixmap);

    for (i = 0; i < width; i++) w->colorTableP[i] = XGetPixel (imageP, i, 0);

    XDestroyImage (imageP);
}

static void CopyFonts(VListWidget w)
{
    int height;
    int maxAscent;

    if (!w->fontTableP) w->fontTableP = (XmFontList)VtkGetDefaultFontList ((Widget) w);

    VtkGetFontA (w->fontTableP, &w->fontPP, &w->fontCnt);

    GetTextGeometry (w, 0, "0", 1, (int *)&w->charWidth, &height, &maxAscent);	/* constant for horizontal scrolling increment */
}

static void CopyLabels(VListWidget w)
{
    VListLabel *newVLabelP, *vLabelP;
    LabelInfo *labelInfoP;
    int label;

    if (!w->labelCnt) return;

    newVLabelP = w->labelsP;
    w->labelsP     = (VListLabel *)XtMalloc (w->labelCnt * sizeof (VListLabel));

    w->labelInfosP = (LabelInfo  *)XtCalloc (w->labelCnt, sizeof (LabelInfo));

    FOR_EACH_LABEL (w, label, labelInfoP, vLabelP) {
	*vLabelP = (*newVLabelP++);
        SupplyLabelDefaults (w, vLabelP, label);
	if (vLabelP->textP) vLabelP->textP = XtNewString (vLabelP->textP);
    }
}

static void CopyGrids(VListWidget w, VListField *vFieldP)
{
    int grid;
    VListGrid *newGridP, *gridP;

    if (!vFieldP->gridCount) return;

    newGridP = vFieldP->gridsP;
    vFieldP->gridsP = (VListGrid *)XtMalloc (vFieldP->gridCount * sizeof (VListGrid));

    FOR_EACH_GRID (vFieldP, grid, gridP) {
	*gridP = (*(newGridP++));
	SupplyGridDefaults (w, gridP);
	if (gridP->textP) gridP->textP = XtNewString (gridP->textP);
    }
}

static void CopyFields(VListWidget w)
{
    VListField *newVFieldP, *vFieldP;
    int field;

    newVFieldP = w->fieldsP;
    w->fieldsP = (VListField *)XtMalloc (w->fieldCnt * sizeof (VListField));

    FOR_EACH_FIELD (w, field, vFieldP) {
	if (newVFieldP) *vFieldP = (*(newVFieldP++));
	else vFieldP->mask = 0;

        SupplyFieldDefaults (w, vFieldP);
	CopyGrids (w, vFieldP);
    }
}

static void CopySegments(LineInfo *lineInfoP)
{
    int segCnt, segment;
    VListSegment *segmentP;
    SegmentInfo *segmentInfoP;

    if (!(segCnt = lineInfoP->vLine.segmentCount)) return;

    lineInfoP->segmentInfosP = (SegmentInfo *)XtRealloc ((char *)lineInfoP->segmentInfosP, sizeof (SegmentInfo) * segCnt);

    segmentP = lineInfoP->vLine.segmentsP;
    FOR_EACH_SEGMENT (lineInfoP, segment, segmentInfoP) segmentInfoP->vSegment = (*(segmentP++));
}

static void CopyBars(LineInfo *lineInfoP)
{
    int barCnt, bar;
    VListBar *oldBarP, *newBarP;

    if (!(barCnt = lineInfoP->vLine.barCount)) {
        lineInfoP->vLine.barsP = 0;
        return;
    }

    oldBarP = lineInfoP->vLine.barsP;
    newBarP = lineInfoP->vLine.barsP = (VListBar *)XtMalloc (sizeof (VListBar) * barCnt);

    for (bar = 0; bar < barCnt; bar++, oldBarP++, newBarP++) *newBarP = (*oldBarP);
}

static void ComputeLabelInfo(VListWidget w)
{
    LabelInfo *labelInfoP;
    VListLabel *vLabelP;
    int label, rows, fields, field, grid;
    VListField *vFieldP;
    VListGrid *gridP;

    if ((!w->vHeaderW) || (!w->labelCnt)) {
	w->headerHeight = 0;
	return;
    }

    rows = 1;
    FOR_EACH_LABEL (w, label, labelInfoP, vLabelP) {
	labelInfoP->textWidth = VHeaderComputeLabelWidth (w->vHeaderW, vLabelP->textP);

	fields = vLabelP->lastField - vLabelP->firstField + 1;

        labelInfoP->widthPerColumn = (labelInfoP->textWidth + 2 * w->textMarginWidth + fields - 1) / fields;

	if (vLabelP->row >= rows) rows = vLabelP->row + 1;
    }

    FOR_EACH_FIELD (w, field, vFieldP)
	FOR_EACH_GRID (vFieldP, grid, gridP) if (gridP->textP && (gridP->row >= rows)) rows = gridP->row + 1;

    w->headerHeight = VHeaderComputeLogicalHeight (w->vHeaderW, rows);
}

static void SetHeaderLabels(VListWidget w)
{
    int field, grid, label, group, firstColumn, lastColumn, vhLabelCnt;
    int gridLabelCnt = 0, gridX, column, leftX, rightX, textWidth, x;
    ColumnInfo *firstColumnInfoP, *lastColumnInfoP, *columnInfoP;
    LabelInfo *labelInfoP;
    VListLabel *vLabelP;
    VHeaderLabel *vhLabelsP, *vhLabelP;
    VListField *vFieldP;
    VListGrid *gridP;

    if (!w->vHeaderW) return;

    FOR_EACH_FIELD (w, field, vFieldP) FOR_EACH_GRID (vFieldP, grid, gridP) if (gridP->textP) gridLabelCnt++;

    vhLabelCnt = (w->labelCnt + gridLabelCnt) * w->groupCnt;
    if (!vhLabelCnt) return;

    vhLabelP = vhLabelsP = (VHeaderLabel *)XtMalloc (vhLabelCnt * sizeof (VHeaderLabel));

    FOR_EACH_GROUP (w, group)
	FOR_EACH_LABEL (w, label, labelInfoP, vLabelP) {
	    vhLabelP->textP 	= vLabelP->textP;
	    vhLabelP->textWidth = labelInfoP->textWidth;
	    vhLabelP->row 	= vLabelP->row;
	    vhLabelP->pad 	= vLabelP->pad;

	    firstColumn = GroupFieldToColumn (w, group, vLabelP->firstField);
	    lastColumn  = GroupFieldToColumn (w, group, vLabelP->lastField);

	    firstColumnInfoP = ColumnToColumnInfo (w, firstColumn);
	    lastColumnInfoP  = ColumnToColumnInfo (w, lastColumn);

	    vhLabelP->x     = firstColumnInfoP->x + w->textMarginWidth;
	    vhLabelP->width = lastColumnInfoP->x + lastColumnInfoP->width - w->textMarginWidth - vhLabelP->x;
	    vhLabelP->textX = ComputeBodyX (vhLabelP->x, vhLabelP->width, labelInfoP->textWidth, 0, vLabelP->alignment);

	    vhLabelP++;
	}

    if (gridLabelCnt)
	FOR_EACH_FIELD (w, field, vFieldP)
	    FOR_EACH_GRID (vFieldP, grid, gridP) if (gridP->textP) {
		textWidth = VHeaderComputeLabelWidth (w->vHeaderW, gridP->textP);

		FOR_EACH_GROUP (w, group) {
		    column 	= GroupFieldToColumn (w, group, field);
		    columnInfoP = ColumnToColumnInfo (w, column);

		    leftX  = columnInfoP->x + vFieldP->marginWidth;
		    rightX = columnInfoP->x + columnInfoP->width - vFieldP->marginWidth - 1;

		    gridX = ValueToX (leftX, rightX, vFieldP->minValue, vFieldP->maxValue, gridP->value);

		    x = gridX - (textWidth + 1) / 2;

		    vhLabelP->textP 	= gridP->textP;
		    vhLabelP->textWidth = textWidth;
		    vhLabelP->row 	= gridP->row;
		    vhLabelP->pad 	= VHeaderLabelPadWithBlanks;
		    vhLabelP->x 	= x;
		    vhLabelP->width 	= textWidth;
		    vhLabelP->textX 	= x;
		    vhLabelP++;
		}
	    }

    XtVaSetValues (w->vHeaderW,
	VHeaderNlabelCount, vhLabelCnt,
    	VHeaderNlabels,     vhLabelsP,
	0);

    XtFree ((char *)vhLabelsP);
}

static void SetStippled(VListWidget w, int stippled, int x, int y)
{
    if (!stippled) {
	if (w->currentStippled) {
	    XSetFillStyle (VtkDisplay (w), w->normgc, FillSolid);
	    XSetFillStyle (VtkDisplay (w), w->invgc,  FillSolid);
	    XSetFillStyle (VtkDisplay (w), w->flipgc, FillSolid);
	    w->currentStippled = 0;
	}

	return;
    }

    if (!w->stippleBitmap) {
	w->stippleBitmap = VtkGetStippleBitmap ((Widget)w);

	XSetStipple (VtkDisplay (w), w->normgc, w->stippleBitmap);
	XSetStipple (VtkDisplay (w), w->invgc,  w->stippleBitmap);
	XSetStipple (VtkDisplay (w), w->flipgc, w->stippleBitmap);
    }

    if (!w->currentStippled) {
	XSetFillStyle (VtkDisplay (w), w->normgc, FillStippled);
	XSetFillStyle (VtkDisplay (w), w->invgc,  FillStippled);
	XSetFillStyle (VtkDisplay (w), w->flipgc, FillStippled);
	w->currentStippled = 1;
    }

    if ((x != w->currentStippleX) || (y != w->currentStippleY)) {
	XSetTSOrigin (VtkDisplay (w), w->normgc, x, y);
	XSetTSOrigin (VtkDisplay (w), w->invgc,  x, y);
	XSetTSOrigin (VtkDisplay (w), w->flipgc, x, y);
	w->currentStippleX = x;
	w->currentStippleY = y;
    }
}

static void SetDashes(VListWidget w, int dashOn, int dashOff, int dashOffset)
{
    char dashList[2];

    dashOffset %= (dashOn + dashOff);

    if ((dashOn == w->currentDashOn) && (dashOff == w->currentDashOff) && (dashOffset == w->currentDashOffset)) return;

    dashList[0] = dashOn;
    dashList[1] = dashOff;
    XSetDashes (VtkDisplay (w), w->normgc, dashOffset, dashList, 2);

    w->currentDashOn     = dashOn;
    w->currentDashOff    = dashOff;
    w->currentDashOffset = dashOffset;
}

static void SetColors(VListWidget w, int coloridx, int background)
{
    Pixel foregroundPixel, backgroundPixel;

    if (!w->colorList) {
	foregroundPixel = (Pixel)coloridx;
	backgroundPixel = (Pixel)background;
    } else {
	foregroundPixel = w->colorTableP[((unsigned int)coloridx % w->colorCnt)];

	if (background == (-1)) backgroundPixel = w->core.background_pixel;
	else 		        backgroundPixel = w->colorTableP[((unsigned int)background % w->colorCnt)];
    }

    if ((foregroundPixel == w->currentForeground) && (backgroundPixel == w->currentBackground)) return;

    w->currentForeground = foregroundPixel;
    w->currentBackground = backgroundPixel;

    XSetForeground (VtkDisplay (w), w->normgc, foregroundPixel);
    XSetBackground (VtkDisplay (w), w->normgc, backgroundPixel);

    XSetBackground (VtkDisplay (w), w->invgc,  foregroundPixel);
    XSetForeground (VtkDisplay (w), w->invgc,  backgroundPixel);

    XSetPlaneMask (VtkDisplay (w), w->flipgc, backgroundPixel ^ foregroundPixel);
}

static void SetFont(VListWidget w, int fontidx)
{
    if (fontidx == w->currentFontidx) return;

    w->currentFontP   = FontidxToFont (w, fontidx);
    w->currentFontidx = fontidx;

    XSetFont (VtkDisplay (w), w->normgc, w->currentFontP->fid);
    XSetFont (VtkDisplay (w), w->invgc,  w->currentFontP->fid);
}

static void SetInternalXYAdjust(VListWidget w, int xAdjust, int yAdjust)
{
    /* Keep both adjustments within bounds. */

    {int maxXAdjust = w->logicalWidth - w->core.width;
    if ((maxXAdjust <= 0) || (xAdjust <= 0)) w->internalXAdjust = 0;
    else if (xAdjust > maxXAdjust) 	     w->internalXAdjust = maxXAdjust;
    else 				     w->internalXAdjust = xAdjust;}

    {int maxYAdjust = w->logicalHeight - w->core.height;
    if ((maxYAdjust <= 0) || (yAdjust <= 0)) w->internalYAdjust = 0;
    else if (yAdjust > maxYAdjust) 	     w->internalYAdjust = maxYAdjust;
    else 				     w->internalYAdjust = yAdjust;}
}

static void SetSlaveValues(VListWidget w)
{
    w->ignoreScrollValueChanged = 1;

    if ((w->requestedXAdjust != w->shownScrollXAdjust) && w->hScrollW)
	XtVaSetValues (w->hScrollW, XmNvalue, w->shownScrollXAdjust = w->requestedXAdjust, 0);

    if ((w->requestedYAdjust != w->shownScrollYAdjust) && w->vScrollW)
	XtVaSetValues (w->vScrollW, XmNvalue, w->shownScrollYAdjust = w->requestedYAdjust, 0);

    w->ignoreScrollValueChanged = 0;

    if ((w->requestedXAdjust != w->shownHeaderXAdjust) && w->vHeaderW && XtIsManaged (w->vHeaderW))
	XtVaSetValues (w->vHeaderW, VHeaderNxAdjust, w->shownHeaderXAdjust = w->requestedXAdjust, 0);
}

static void SetPosition(VListWidget w)
{
    w->externalXAdjust = w->requestedXAdjust = w->internalXAdjust;
    w->externalYAdjust = w->requestedYAdjust = w->internalYAdjust;
    SetSlaveValues (w);
}

static void StartPendingScroll(VListWidget w)
{
    int x, y, width, height;

    if (!w->layoutIsValid)  return;
    if (w->scrollIsActive)  return;

    /* Check for autoscrolling */

    if (w->xScrollStrength || w->yScrollStrength)
	SetInternalXYAdjust (w, w->internalXAdjust + w->xScrollStrength, w->internalYAdjust + w->yScrollStrength);

    /* If the scroll position is already consistent, just return. */

    if ((w->internalXAdjust == w->requestedXAdjust) && (w->internalYAdjust == w->requestedYAdjust)) return;

    /* Compute the CopyArea parameters and ask the server to scroll the window contents.  Keep track of the fact that the
     * server is busy, and keep track of the scroll position we requested the server to establish. */

    x = w->internalXAdjust - w->requestedXAdjust;
    y = w->internalYAdjust - w->requestedYAdjust;
    width  = w->core.width;
    height = w->core.height;

    w->requestedXAdjust = w->internalXAdjust;
    w->requestedYAdjust = w->internalYAdjust;
    SetSlaveValues (w);

    SetStippled (w, 0, 0, 0);
    w->scrollSerial = XNextRequest (VtkDisplay (w));
    XCopyArea (VtkDisplay (w), VtkWindow (w), VtkWindow (w), w->normgc, x, y, width, height, 0, 0);
    w->scrollIsActive = 1;
}

void VListSetAutoScrollPosition(Widget ow, int x, int y)
{
    VListWidget w = (VListWidget)ow;
    int s;

    if (!VtkIsRealized (w)) return;

    if (x < 0) 			 s = x;
    else if (x >= w->core.width) s = x - w->core.width;
    else 			 s = 0;
    w->xScrollStrength = s * w->autoScrollRate / 100;

    if (y < 0) 			  s = y;
    else if (y >= w->core.height) s = y - w->core.height;
    else 			  s = 0;
    w->yScrollStrength = s * w->autoScrollRate / 100;

    if (w->xScrollStrength || w->yScrollStrength) StartPendingScroll (w);
}

static void ComputeLogicalHeight(VListWidget w)
{
    if ((!w->allowDeadspace) || (w->columnStyle != VListColumnStyleSingle))
	w->logicalHeight = w->tierCnt * w->tierHeight + 2 * w->marginHeight - w->rowSpacingHeight;
    else
	w->logicalHeight = (w->tierCnt - 1) * w->tierHeight + w->core.height;
}

static void HPageIncCallback(Widget unused_sbW, XtPointer ow, XtPointer unused_cbDataP)
{
    VListWidget w = (VListWidget)ow;
    int maxXAdjust, xAdjust;

    xAdjust = w->internalXAdjust + w->core.width - w->charWidth;

    if (w->logicalWidth <= w->core.width) maxXAdjust = 0;
    else 				  maxXAdjust = w->logicalWidth - w->core.width;

    if (xAdjust > maxXAdjust) xAdjust = maxXAdjust;

    XtVaSetValues (w->hScrollW, XmNvalue, w->shownScrollXAdjust = xAdjust, 0);

    w->internalXAdjust = xAdjust;
    StartPendingScroll (w);
}

static void HPageDecCallback(Widget unused_sbW, XtPointer ow, XtPointer unused_cbDataP)
{
    VListWidget w = (VListWidget)ow;
    int xAdjust;

    xAdjust = w->internalXAdjust - w->core.width + w->charWidth;
    if (xAdjust < 0) xAdjust = 0;

    XtVaSetValues (w->hScrollW, XmNvalue, w->shownScrollXAdjust = xAdjust, 0);

    w->internalXAdjust = xAdjust;
    StartPendingScroll (w);
}

static void HValueChangedCallback(Widget unused_sbW, XtPointer ow, XtPointer cbDataOP)
{
    VListWidget w = (VListWidget)ow;
    XmScrollBarCallbackStruct *cbDataP = (XmScrollBarCallbackStruct *)cbDataOP;

    if (w->ignoreScrollValueChanged) return;

    w->shownScrollXAdjust = w->internalXAdjust = cbDataP->value;

    if (!VtkIsRealized (w)) SetPosition (w); else StartPendingScroll (w);
}

static void VPageIncCallback(Widget unused_sbW, XtPointer ow, XtPointer unused_cbDataP)
{
    VListWidget w = (VListWidget)ow;
    int yAdjust, maxYAdjust;

    yAdjust = w->internalYAdjust + w->core.height - w->tierHeight;

    if (w->logicalHeight <= w->core.height) maxYAdjust = 0;
    else 				    maxYAdjust = w->logicalHeight - w->core.height;

    if (yAdjust > maxYAdjust) yAdjust = maxYAdjust;

    XtVaSetValues (w->vScrollW, XmNvalue, w->shownScrollYAdjust = yAdjust, 0);

    w->internalYAdjust = yAdjust;
    StartPendingScroll (w);
}

static void VPageDecCallback(Widget unused_sbW, XtPointer ow, XtPointer unused_cbDataP)
{
    VListWidget w = (VListWidget)ow;
    int yAdjust;

    yAdjust = w->internalYAdjust - w->core.height + w->tierHeight;
    if (yAdjust < 0) yAdjust = 0;

    XtVaSetValues (w->vScrollW, XmNvalue, w->shownScrollYAdjust = yAdjust, 0);

    w->internalYAdjust = yAdjust;
    StartPendingScroll (w);
}

static void VValueChangedCallback(Widget unused_sbW, XtPointer ow, XtPointer cbDataOP)
{
    VListWidget w = (VListWidget)ow;
    XmScrollBarCallbackStruct *cbDataP = (XmScrollBarCallbackStruct *)cbDataOP;

    if (w->ignoreScrollValueChanged) return;

    w->shownScrollYAdjust = w->internalYAdjust = cbDataP->value;

    if (!VtkIsRealized (w)) SetPosition (w); else StartPendingScroll (w);
}

#define DRAW_HIST_BAR(lowValue, highValue, coloridx, stippled, vFieldP)							\
    {															\
    if (ValuesToXs (minX, maxX, minValue, maxValue, lowValue, highValue, &leftX, &rightX)) {				\
	SetColors (w, coloridx, lineInfoP->vLine.background);								\
	SetStippled (w, stippled, x, y);										\
	XFillRectangle (VtkDisplay (w), VtkWindow (w), w->normgc, leftX, y + vFieldP->marginHeight,			\
	    rightX - leftX + 1, height - 2 * vFieldP->marginHeight);							\
    }															\
    }

static void DrawHist(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP, 
		     int x, int y, int width, unsigned height, int flip, int clear)
{
    int bar, grid, gridX, leftX, rightX;
    VListGrid *gridP;
    VListBar *barP;
    int minX = x + vFieldP->marginWidth;
    int maxX = x + width - vFieldP->marginWidth - 1;
    int minValue = vFieldP->minValue;
    int maxValue = vFieldP->maxValue;
    VListLine *vLineP = (&lineInfoP->vLine);

    SetStippled (w, 0, 0, 0);
    SetColors (w, vLineP->coloridx, vLineP->background);

    if (flip || clear || (w->currentBackground != w->core.background_pixel) || (w->gridStyle != VListGridStyleBackground))
	XFillRectangle (VtkDisplay (w), VtkWindow (w), w->invgc, x, y, width, height);

    FOR_EACH_GRID (vFieldP, grid, gridP)
	if ((gridP->dashOn) && (gridP->value >= vFieldP->minValue) && (gridP->value <= vFieldP->maxValue)) {
	    gridX = ValueToX (minX, maxX, minValue, maxValue, gridP->value);
	    SetDashes (w, gridP->dashOn, gridP->dashOff, 0);
	    SetColors (w, gridP->coloridx, vLineP->background);
	    XDrawLine (VtkDisplay (w), VtkWindow (w), w->normgc, gridX, y, gridX, y + height - 1);
	}

    if (vLineP->mask & VListLineBarsMask)
	FOR_EACH_BAR (vLineP, bar, barP) DRAW_HIST_BAR (barP->lowValue, barP->highValue, barP->coloridx, barP->stippled, vFieldP)
    else
	DRAW_HIST_BAR (vLineP->lowValue, vLineP->highValue, vLineP->coloridx, vLineP->stippled, vFieldP)

    if (flip) {
        SetColors (w, vLineP->coloridx, vLineP->background);
	SetStippled (w, vLineP->stippled, x, y);
        XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, x, y, width, height);
    }
}

static void DrawText(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP, 
		     int x, int y, int width, int height, int flip, int clear)
{
    int segment, bodyX, bodyY, segmentCnt, maxAscent, maxDescent, chunkX, chunkY;
    int chunkWidth, chunkHeight, leftMarginWidth, rightMarginWidth;
    int  rightMarginX;
    SegmentInfo *segmentInfoP;

    bodyX = ComputeBodyX (x, width,  lineInfoP->bodyWidth,  vFieldP->marginWidth,  vFieldP->alignment);
    bodyY = ComputeBodyY (y, height, lineInfoP->bodyHeight, vFieldP->marginHeight, vFieldP->alignment);

    SetColors (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);

    if (clear || (w->currentBackground != w->core.background_pixel) || (w->gridStyle != VListGridStyleBackground)) {
	SetStippled (w, 0, 0, 0);
	XFillRectangle (VtkDisplay (w), VtkWindow (w), w->invgc, x, y, width, height);
    }

    SetStippled (w, lineInfoP->vLine.stippled, x, y);

    if (flip) XFillRectangle (VtkDisplay (w), VtkWindow (w), w->normgc, x, y, width, height);

    if (!lineInfoP->vLine.segmentCount) {
	if (lineInfoP->textStrlen) {
	    SetFont (w, lineInfoP->vLine.fontidx);
	    XDrawString (VtkDisplay (w), VtkWindow (w), (flip ? w->invgc : w->normgc),
		bodyX, bodyY + lineInfoP->maxAscent, lineInfoP->vLine.textP, lineInfoP->textStrlen);
	}

        return;
    }

    if (flip) {
	/* Flip all internal segment areas.  Do this before drawing ANY text to avoid problems when text is kerned. */

	chunkY = bodyY;
	segment = 0;
	while ((segmentCnt = GetChunkGeometry (lineInfoP, segment, &chunkWidth, &maxAscent, &maxDescent))) {
            GetChunkLayout (bodyX, lineInfoP->bodyWidth, maxAscent, maxDescent, chunkWidth, vFieldP->alignment,
                &chunkX, &chunkHeight, &leftMarginWidth, &rightMarginX, &rightMarginWidth);

	    for (segmentInfoP = SegmentToSegmentInfo (lineInfoP, segment); segmentCnt; segmentInfoP++, segment++, segmentCnt--)
		if (segmentInfoP->bodyWidth) {
	    	    SetColors   (w, segmentInfoP->vSegment.coloridx, lineInfoP->vLine.background);

	    	    XFillRectangle (VtkDisplay (w), VtkWindow (w), w->normgc, chunkX, chunkY, segmentInfoP->bodyWidth,
			chunkHeight);
		    chunkX += segmentInfoP->bodyWidth;
		}

	    chunkY += chunkHeight;
	}
    }

    chunkY = bodyY;
    segment = 0;
    while ((segmentCnt = GetChunkGeometry (lineInfoP, segment, &chunkWidth, &maxAscent, &maxDescent))) {
        GetChunkLayout (bodyX, lineInfoP->bodyWidth, maxAscent, maxDescent, chunkWidth, vFieldP->alignment,
	    &chunkX, &chunkHeight, &leftMarginWidth, &rightMarginX, &rightMarginWidth);

	for (segmentInfoP = SegmentToSegmentInfo (lineInfoP, segment); segmentCnt; segmentInfoP++, segment++, segmentCnt--)
	    if (segmentInfoP->bodyWidth) {
	        SetColors   (w, segmentInfoP->vSegment.coloridx, lineInfoP->vLine.background);
	        SetStippled (w, segmentInfoP->vSegment.stippled, x, y);

		if (segmentInfoP->vSegment.type == VListSegmentTypeText) {
	            SetFont (w, segmentInfoP->vSegment.fontidx);
	            XDrawString (VtkDisplay (w), VtkWindow (w), (flip ? w->invgc : w->normgc), chunkX, chunkY + maxAscent,
		        lineInfoP->vLine.textP + segmentInfoP->vSegment.textPosition, segmentInfoP->vSegment.textStrlen);
		} else {
		    if (segmentInfoP->vSegment.icon) {
		        XSetGraphicsExposures (VtkDisplay (w), w->normgc, 0);
		        XCopyArea (VtkDisplay (w), segmentInfoP->vSegment.icon, VtkWindow (w), w->normgc, 0, 0,
			    segmentInfoP->bodyWidth, segmentInfoP->bodyHeight, chunkX, chunkY);
		        XSetGraphicsExposures (VtkDisplay (w), w->normgc, 1);
		    }
		}
		chunkX += segmentInfoP->bodyWidth;
	    }

	chunkY += chunkHeight;
    }
}

static void DrawIcon(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP,
	int x, int y, int width, int height, int flip, int clear)
{
    int bodyX, bodyY;

    SetColors (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);

    if (clear || (w->currentBackground != w->core.background_pixel) || (w->gridStyle != VListGridStyleBackground)) {
	SetStippled (w, 0, 0, 0);
	XFillRectangle (VtkDisplay (w), VtkWindow (w), w->invgc, x, y, width, height);
    }

    SetStippled (w, lineInfoP->vLine.stippled, x, y);

    if (flip) XFillRectangle (VtkDisplay (w), VtkWindow (w), w->normgc, x, y, width, height);

    if (lineInfoP->vLine.icon) {
	bodyX = ComputeBodyX (x, width,  lineInfoP->bodyWidth,  vFieldP->marginWidth,  vFieldP->alignment);
	bodyY = ComputeBodyY (y, height, lineInfoP->bodyHeight, vFieldP->marginHeight, vFieldP->alignment);

        XSetGraphicsExposures (VtkDisplay (w), w->normgc, 0);
	XCopyArea (VtkDisplay (w), lineInfoP->vLine.icon, VtkWindow (w), w->normgc, 0, 0,
	    lineInfoP->bodyWidth, lineInfoP->bodyHeight, bodyX, bodyY);
        XSetGraphicsExposures (VtkDisplay (w), w->normgc, 1);

	if (lineInfoP->vLine.stippled)
	    XFillRectangle (VtkDisplay (w), VtkWindow (w), w->invgc, bodyX, bodyY, lineInfoP->bodyWidth, lineInfoP->bodyHeight);
    }
}

static void Draw(VListWidget w, int line, int clear, int checkVisibility)
{
    VListField *vFieldP;
    LineInfo *lineInfoP = LineToLineInfo (w, line);
    LineInfo *firstLineInfoP;
    int column, tier, x, y, flip, field;
    int width, height;

    lineInfoP->expose = 0;

    if (!w->visible)       return;
    if (!w->layoutIsValid) return;

    LineToColumnTier (w, line, &column, &tier);
    ColumnTierToCell (w, column, tier, &x, &y, &width, &height);

    if (checkVisibility && (!CellIsVisible (w, x, y, width, height))) return;

    field   = LineToField (w, line);
    vFieldP = FieldToVField (w, field);

    if (w->selectUnit == VListSelectUnitField) flip = lineInfoP->vLine.select;
    else {
	firstLineInfoP = LineToLineInfo (w, LineToFirstLineInRecord (w, line));
	flip = firstLineInfoP->vLine.select;
    }

    switch (lineInfoP->vLine.type) {
	case VListLineTypeHist : DrawHist (w, lineInfoP, vFieldP, x, y, width, height, flip, clear);	break;
	case VListLineTypeText : DrawText (w, lineInfoP, vFieldP, x, y, width, height, flip, clear);	break;
	case VListLineTypeIcon : DrawIcon (w, lineInfoP, vFieldP, x, y, width, height, flip, clear);	break;
    }
}

static void FillMargins(VListWidget w, GC gc, int x, int y, int width, int height, int bodyX, int bodyY, LineInfo *lineInfoP)
{
    int leftMarginWidth, rightMarginWidth, topMarginHeight, bottomMarginHeight;

    leftMarginWidth    = bodyX - x;
    topMarginHeight    = bodyY - y;

    rightMarginWidth   = width  - lineInfoP->bodyWidth  - leftMarginWidth;
    bottomMarginHeight = height - lineInfoP->bodyHeight - topMarginHeight;

    if (leftMarginWidth) XFillRectangle (VtkDisplay (w), VtkWindow (w), gc,
	x, y, leftMarginWidth, height);

    if (rightMarginWidth) XFillRectangle (VtkDisplay (w), VtkWindow (w), gc,
	x + width - rightMarginWidth, y, rightMarginWidth, height);

    if (topMarginHeight) XFillRectangle (VtkDisplay (w), VtkWindow (w), gc,
	x + leftMarginWidth, y, width - leftMarginWidth - rightMarginWidth, topMarginHeight);

    if (bottomMarginHeight) XFillRectangle (VtkDisplay (w), VtkWindow (w), gc,
	x + leftMarginWidth, y + height - bottomMarginHeight, width - leftMarginWidth - rightMarginWidth, bottomMarginHeight);
}

static void FlipHist(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP, int x, int y, int width, int height)
{
    SetStippled (w, lineInfoP->vLine.stippled, x, y);
    SetColors   (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);
    XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, x, y, width, height);
}

static void FlipText(VListWidget w, LineInfo *lineInfoP, VListField *vFieldP, int x, int y, int width, int height)
{
    int bodyX, bodyY, segment, segmentCnt, maxAscent, maxDescent, chunkX, chunkY, rightMarginX;
    int chunkWidth, chunkHeight, leftMarginWidth, rightMarginWidth;
    SegmentInfo *segmentInfoP;

    SetStippled (w, lineInfoP->vLine.stippled, x, y);
    SetColors   (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);

    if (!lineInfoP->vLine.segmentCount) {
        XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, x, y, width, height);
	return;
    }

    /* Multi-segment */

    bodyX = ComputeBodyX (x, width,  lineInfoP->bodyWidth,  vFieldP->marginWidth,  vFieldP->alignment);
    bodyY = ComputeBodyY (y, height, lineInfoP->bodyHeight, vFieldP->marginHeight, vFieldP->alignment);

    FillMargins (w, w->flipgc, x, y, width, height, bodyX, bodyY, lineInfoP);

    chunkY = bodyY;
    segment = 0;
    while ((segmentCnt = GetChunkGeometry (lineInfoP, segment, &chunkWidth, &maxAscent, &maxDescent))) {
	GetChunkLayout (bodyX, lineInfoP->bodyWidth, maxAscent, maxDescent, chunkWidth, vFieldP->alignment,
	    &chunkX, &chunkHeight, &leftMarginWidth, &rightMarginX, &rightMarginWidth);

	if (chunkHeight) {
	    if (leftMarginWidth) {
		SetColors (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);
		XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, bodyX,        chunkY, leftMarginWidth,  chunkHeight);
	    }

	    if (rightMarginWidth) {
	        SetColors (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);
		XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, rightMarginX, chunkY, rightMarginWidth, chunkHeight);
	    }
	}

	for (segmentInfoP = SegmentToSegmentInfo (lineInfoP, segment); segmentCnt; segmentInfoP++, segment++, segmentCnt--)
	    if (segmentInfoP->bodyWidth) {
		SetColors   (w, segmentInfoP->vSegment.coloridx, lineInfoP->vLine.background);
		XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, chunkX, chunkY, segmentInfoP->bodyWidth, chunkHeight);
		chunkX += segmentInfoP->bodyWidth;
	    }

	chunkY += chunkHeight;
    }
}

static void FlipIcon (VListWidget w, LineInfo *lineInfoP, VListField *vFieldP, int x, int y, int width, int height)
{
    int bodyX, bodyY;

    SetStippled (w, lineInfoP->vLine.stippled, x, y);
    SetColors (w, lineInfoP->vLine.coloridx, lineInfoP->vLine.background);

    if (!lineInfoP->vLine.icon) {
	XFillRectangle (VtkDisplay (w), VtkWindow (w), w->flipgc, x, y, width, height);
	return;
    }

    bodyX = ComputeBodyX (x, width,  lineInfoP->bodyWidth,  vFieldP->marginWidth,  vFieldP->alignment);
    bodyY = ComputeBodyY (y, height, lineInfoP->bodyHeight, vFieldP->marginHeight, vFieldP->alignment);

    FillMargins (w, w->flipgc, x, y, width, height, bodyX, bodyY, lineInfoP);
}

static void Flip(VListWidget w, int line)
{
    int column, tier, x, y, field;
    int width, height;
    LineInfo *lineInfoP;
    VListField *vFieldP;

    if (!w->visible)       return;
    if (!w->layoutIsValid) return;

    LineToColumnTier (w, line, &column, &tier);
    ColumnTierToCell (w, column, tier, &x, &y, &width, &height);

    if (!CellIsVisible (w, x, y, width, height)) return;

    if (w->scrollIsActive || w->activeGropCnt) {	/* if we're busy scrolling... */
	Draw (w, line, 1, 1);				/* previous contents are undefined */
	return;
    }

    lineInfoP  = LineToLineInfo (w, line);
    field      = LineToField (w, line);
    vFieldP    = FieldToVField (w, field);

    switch (lineInfoP->vLine.type) {
	case VListLineTypeHist : FlipHist (w, lineInfoP, vFieldP, x, y, width, height);	break;
	case VListLineTypeText : FlipText (w, lineInfoP, vFieldP, x, y, width, height);	break;
	case VListLineTypeIcon : FlipIcon (w, lineInfoP, vFieldP, x, y, width, height);	break;
    }
}

static int AddCellHeightContribution(VListWidget w, int height)
{
    if (height > w->cellHeight) {
	w->cellHeight = height;
	w->cellHeightCnt = 1;
	return (1);
    }

    if (height == w->cellHeight) w->cellHeightCnt++;

    return (0);
}

static int RemoveCellHeightContribution(VListWidget w, int height)
{
    if (w->fieldResizePolicy == VListResizePolicyGrowOnly) return (0);

    if (height < w->cellHeight) return (0);
    if (!(--w->cellHeightCnt)) return (1);
    return (0);
}

static int ChangeCellHeightContribution(VListWidget w, int oldHeight, int newHeight)
{
    if (oldHeight == newHeight) return (0);

    if (newHeight > w->cellHeight) {					/* new max height */
	w->cellHeight = newHeight;					/* save new max height */
	w->cellHeightCnt = 1;						/* single reference count */
	return (1);							/* change needed */
    }

    if (newHeight == w->cellHeight) {					/* same as current max height */
	w->cellHeightCnt++;						/* increment reference count */
	return (0);							/* no change needed */
    }

    if (w->fieldResizePolicy == VListResizePolicyGrowOnly) return (0);

    if (oldHeight < w->cellHeight) return (0);				/* was smaller, is still smaller */
    if (!(--w->cellHeightCnt)) return (1);				/* was equal, decrement ref count, zap if last one */
    return (0);								/* was equal but others remain */
}

static void ComputeCellHeight(VListWidget w, int minHeight)
{
    int field, line;
    LineInfo *lineInfoP;
    VListField *vFieldP;

    w->cellHeight = w->cellHeightCnt = 0;

    AddCellHeightContribution (w, minHeight);		/* at least this high */
    AddCellHeightContribution (w, w->rowHeight);	/* user-specified minimum height */

    FOR_EACH_FIELD (w, field, vFieldP)
	FOR_EACH_LINE_L (w, line, lineInfoP, field, w->fieldCnt)
	    AddCellHeightContribution (w, lineInfoP->bodyHeight + 2 * vFieldP->marginHeight);

    w->tierHeight = w->cellHeight + w->rowSpacingHeight;
}

static int AddColumnWidthContribution(VListWidget w, int column, int width, VListField *vFieldP)
{
    ColumnInfo *columnInfoP = ColumnToColumnInfo (w, column);

    if (width > columnInfoP->width) {
	columnInfoP->width = width;
	columnInfoP->widthCnt = 1;
	return (1);
    }

    if (width == columnInfoP->width) columnInfoP->widthCnt++;

    return (0);
}

static int RemoveColumnWidthContribution(VListWidget w, int column, int width, VListField *vFieldP)
{
    ColumnInfo *columnInfoP;

    if (vFieldP->resizePolicy == VListResizePolicyGrowOnly) return (0);

    columnInfoP = ColumnToColumnInfo (w, column);

    if (width < columnInfoP->width) return (0);
    if (!(--columnInfoP->widthCnt)) return (1);
    return (0);
}

static int ChangeColumnWidthContribution(VListWidget w, int column, int oldWidth, int newWidth, VListField *vFieldP)
{
    ColumnInfo *columnInfoP;

    if (oldWidth == newWidth) return (0);

    columnInfoP = ColumnToColumnInfo (w, column);

    if (newWidth > columnInfoP->width) {				/* new max width */
	columnInfoP->width = newWidth;					/* save new max width */
	columnInfoP->widthCnt = 1;					/* single reference count */
	return (1);							/* column change needed */
    }

    if (newWidth == columnInfoP->width) {				/* same as current max width */
	columnInfoP->widthCnt++;					/* increment reference count */
	return (0);							/* no column change needed */
    }

    if (vFieldP->resizePolicy == VListResizePolicyGrowOnly) return (0);

    if (oldWidth < columnInfoP->width) return (0);			/* was smaller, is still smaller */
    if (!(--columnInfoP->widthCnt)) return (1);				/* was equal, decrement ref count, zap if last one */
    return (0);								/* was equal but others remain */
}

static void ComputeColumnWidth(VListWidget w, int column)
{
    int group, field, label;
    int line, lastLine;
    ColumnInfo *columnInfoP = ColumnToColumnInfo (w, column);
    LineInfo *lineInfoP;
    LabelInfo *labelInfoP;
    VListLabel *vLabelP;
    VListField *vFieldP;

    group   = ColumnToGroup (w, column);
    field   = ColumnGroupToField (w, column, group);
    vFieldP = FieldToVField (w, field);

    columnInfoP->width = columnInfoP->widthCnt = 0;

    AddColumnWidthContribution (w, column, vFieldP->width, vFieldP);
    
    GroupFieldToLines (w, group, field, &line, &lastLine);
    for (lineInfoP = LineToLineInfo (w, line); line <= lastLine; line += w->fieldCnt, lineInfoP += w->fieldCnt)
	AddColumnWidthContribution (w, column, lineInfoP->bodyWidth + 2 * vFieldP->marginWidth, vFieldP);

    FOR_EACH_LABEL (w, label, labelInfoP, vLabelP) if ((vLabelP->firstField <= field) && (vLabelP->lastField >= field))
	AddColumnWidthContribution (w, column, labelInfoP->widthPerColumn, vFieldP);
}

#define TryLayoutCheckNone		0
#define TryLayoutCheckAllVisible	1
#define TryLayoutCheckTwoVisible	2

static int TryLayout(VListWidget w, int check, int width, int height)
{
    int column, x, group, field, adjust;
    int minHeight, padWidth, extraHeight;
    ColumnInfo *columnInfoP, *previousColumnInfoP;
    VListField *vFieldP;

    if (height < (minHeight = (2 * w->marginHeight + w->tierHeight))) w->logicalHeight = minHeight;
    else 							      w->logicalHeight = height;

    w->tierCnt = (w->logicalHeight - 2 * w->marginHeight) / w->tierHeight;

    if (!w->rowCnt) w->groupCnt = 1;
    else 	    w->groupCnt = (w->rowCnt - 1) / w->tierCnt + 1;

    w->columnCnt = w->groupCnt * w->fieldCnt;
    w->columnInfosP = (ColumnInfo *)XtRealloc ((char *)w->columnInfosP, w->columnCnt * sizeof (ColumnInfo));

    if (w->groupCnt == 1) w->topMarginHeight = w->marginHeight;
    else 		  w->topMarginHeight = (w->logicalHeight - w->tierCnt * w->tierHeight + w->rowSpacingHeight + 1) / 2;

    x = w->marginWidth;

    for (column = 0, columnInfoP = ColumnToColumnInfo (w, 0); column < w->columnCnt; column++, columnInfoP++) {
        group   = ColumnToGroup (w, column);
	field   = ColumnGroupToField (w, column, group);
	vFieldP = FieldToVField (w, field);

	columnInfoP->x = x;
	ComputeColumnWidth (w, column);
	x += columnInfoP->width;

	if (field != (w->fieldCnt - 1))      x += w->fieldSpacingWidth;
	else if (group != (w->groupCnt - 1)) x += (2 * w->marginWidth);

	if (check == TryLayoutCheckAllVisible) {
	    if ((x + w->marginWidth) > width) return (0);
	} else if (check == TryLayoutCheckTwoVisible) {
	    if ((field == (w->fieldCnt - 1)) && group) {
	        previousColumnInfoP = ColumnToColumnInfo (w, (column - w->fieldCnt * 2 + 1));
		if ((x - previousColumnInfoP->x + 2 * w->marginWidth) > width) return (0);
	    }
	}
    }

    w->logicalWidth = x + w->marginWidth;

    if (w->spaceGroupsEqually && w->groupCnt && (w->logicalWidth < width)) {
	padWidth = (width - w->logicalWidth) / w->groupCnt;
	adjust = padWidth / 2;

	FOR_EACH_GROUP (w, group) {
	    FOR_EACH_FIELD (w, field, vFieldP) {
	        columnInfoP = ColumnToColumnInfo (w, GroupFieldToColumn (w, group, field));
		columnInfoP->x += adjust;
	    }

	    adjust += padWidth;
	}
    }

    if (w->spaceTiersEqually && (w->groupCnt == 1) && w->rowCnt
      && ((extraHeight = height - (w->rowCnt * w->tierHeight + 2 * w->marginHeight - w->rowSpacingHeight)) > 0)) {
	w->cellHeight += (extraHeight / w->rowCnt);
	w->tierHeight = w->cellHeight + w->rowSpacingHeight;
    }

    return (1);
}

static void DoColumnLayout(VListWidget w)
{
    int width, height;
    Widget parentW = w->core.parent;

    width = parentW->core.width;
    if (w->vScrollPolicy == VListScrollBarPolicyAlways) width -= (w->vScrollW->core.width + w->vScrollW->core.border_width);

    if ((w->columnStyle == VListColumnStyleMultiple) || (w->columnStyle == VListColumnStyleBestFit)) {
	height = parentW->core.height;
	if (w->headerHeight) height -= (w->headerHeight + w->core.border_width);

	if (w->hScrollPolicy == VListScrollBarPolicyNever) {
	    TryLayout (w, TryLayoutCheckNone, width, height);
	    return;
	}

	if (w->hScrollPolicy == VListScrollBarPolicyAsNeeded) if (TryLayout (w, TryLayoutCheckAllVisible, width, height)) return;

	height -= (w->hScrollW->core.height + w->hScrollW->core.border_width);

	if (w->columnStyle == VListColumnStyleMultiple) {
            TryLayout (w, TryLayoutCheckNone, width, height);
	    return;
        }

	if (TryLayout (w, TryLayoutCheckTwoVisible, width, height)) return;
    }

    if ((!w->allowDeadspace) || (w->columnStyle != VListColumnStyleSingle)) {
	height = w->rowCnt * w->tierHeight + 2 * w->marginHeight;
    } else {
	height = (w->rowCnt - 1) * w->tierHeight + parentW->core.height;
	if (w->headerHeight) height -= (w->headerHeight + w->core.border_width);
	if (w->hScrollPolicy == VListScrollBarPolicyAlways) height -= (w->hScrollW->core.height + w->hScrollW->core.border_width);
    }

    TryLayout (w, TryLayoutCheckNone, width, height);
}

static void ApplyLayoutPolicy(VListWidget w, int *xP, int *yP, int *widthP, int *heightP, int *hScrollP, int *vScrollP)
{
    int x, y, width, height, hScroll, vScroll, temp;

    hScroll = vScroll = x = y = 0;
    width  = w->core.parent->core.width;
    height = w->core.parent->core.height;

    if (w->headerHeight) {
	temp = w->headerHeight + w->core.border_width;
	y += temp;
	if ((height -= temp) <= 0) height = 1;
    }

    if ((w->hScrollPolicy == VListScrollBarPolicyAlways)
      || ((w->hScrollPolicy == VListScrollBarPolicyAsNeeded) && (w->logicalWidth > width))) {
	temp = w->hScrollW->core.height + w->hScrollW->core.border_width;
	if ((height -= temp) <= 0) height = 1;
	hScroll = 1;
    }

    if ((w->vScrollPolicy == VListScrollBarPolicyAlways)
      || ((w->vScrollPolicy == VListScrollBarPolicyAsNeeded) && (w->logicalHeight > height))) {
	temp = w->vScrollW->core.width + w->vScrollW->core.border_width;
	if (w->vScrollPosition == VListScrollBarPositionLeft) x += temp;
	if ((width -= temp) <= 0) width = 1;
	vScroll = 1;

	if ((!hScroll) && (w->hScrollPolicy == VListScrollBarPolicyAsNeeded) && (w->logicalWidth > width)) {
	    temp = w->hScrollW->core.height + w->hScrollW->core.border_width;
	    if ((height -= temp) <= 0) height = 1;
	    hScroll = 1;
	}
    }

    *hScrollP 	= hScroll;
    *vScrollP 	= vScroll;
    *xP 	= x;
    *yP 	= y;
    *widthP 	= width;
    *heightP 	= height;
}

static void ArrangeWidgets(VListWidget w)
{
    int x, y, needHScroll, needVScroll;
    int newX, newY;
    int width, height;
    Widget vHeaderW = w->vHeaderW;
    Widget hScrollW = w->hScrollW;
    Widget vScrollW = w->vScrollW;

    ApplyLayoutPolicy (w, &x, &y, &width, &height, &needHScroll, &needVScroll);

    if ((!w->headerHeight) && vHeaderW && XtIsManaged (vHeaderW)) XtUnmanageChild (vHeaderW);
    if ((!needHScroll)     && hScrollW && XtIsManaged (hScrollW)) XtUnmanageChild (hScrollW);
    if ((!needVScroll)     && vScrollW && XtIsManaged (vScrollW)) XtUnmanageChild (vScrollW);

    newX = x - w->core.border_width;
    newY = y - w->core.border_width;

    if ((w->core.x != newX) || (w->core.y != newY) || (w->core.width != width) || (w->core.height != height))
	XtVaSetValues ((Widget)w,
	    XtNx,      newX,
	    XtNy,      newY,
	    XtNwidth,  width,
	    XtNheight, height,
	    0);

    if (w->headerHeight) {
	newX = x - vHeaderW->core.border_width;
	newY = (-vHeaderW->core.border_width);

        if ((vHeaderW->core.x != newX) || (vHeaderW->core.y != newY)
    	  || (vHeaderW->core.width != width) || (vHeaderW->core.height != w->headerHeight))
	    XtVaSetValues (vHeaderW,
	        XtNx,      newX,
	        XtNy,      newY,
	        XtNwidth,  width,
	        XtNheight, w->headerHeight,
		0);

	if (!XtIsManaged (vHeaderW)) XtManageChild (vHeaderW);
    }

    if (needHScroll) {
	int logicalWidth   = w->logicalWidth;
	int scrollBarValue = w->requestedXAdjust;

	if (logicalWidth < width) 		     logicalWidth = width;
	if ((scrollBarValue + width) > logicalWidth) scrollBarValue = logicalWidth - width;
	if (scrollBarValue < 0) 		     scrollBarValue = 0;

	/* Do separate XtSetValues to get around a scroll bar widget bug. */

	newX = x - hScrollW->core.border_width;
	newY = y + height;

	w->ignoreScrollValueChanged = 1;

	if ((hScrollW->core.x != newX) || (hScrollW->core.y != newY))
	    XtVaSetValues (hScrollW,
	        XtNx, newX,
	        XtNy, newY,
	        0);

    	XtVaSetValues (hScrollW,
	    XtNwidth,     	width,
	    XmNsliderSize,    	width,
	    XmNmaximum, 	logicalWidth,
	    XmNvalue,    	scrollBarValue,
	    XmNincrement, 	w->charWidth,
	    0);

	w->shownScrollXAdjust = scrollBarValue;

	w->ignoreScrollValueChanged = 0;

	if (!XtIsManaged (hScrollW)) XtManageChild (hScrollW);
    }

    if (needVScroll) {
        int logicalHeight  = w->logicalHeight;
	int scrollBarValue = w->requestedYAdjust;

	if (logicalHeight < height) 		       logicalHeight = height;
	if ((scrollBarValue + height) > logicalHeight) scrollBarValue = logicalHeight - height;
	if (scrollBarValue < 0) 		       scrollBarValue = 0;

	if (w->vScrollPosition == VListScrollBarPositionRight) x = width;
	else 						       x = (-vScrollW->core.border_width);

	/* Do separate XtSetValues to get around a scroll bar widget bug. */

	newY = y - vScrollW->core.border_width;

	w->ignoreScrollValueChanged = 1;

        if ((vScrollW->core.x != x) || (vScrollW->core.y != newY))
	    XtVaSetValues (vScrollW,
	        XtNx, 	x,
	        XtNy, 	newY,
		0);

	XtVaSetValues (vScrollW,
	    XtNheight,    	height,
	    XmNsliderSize,    	height,
	    XmNmaximum, 	logicalHeight,
	    XmNvalue,    	scrollBarValue,
	    XmNincrement, 	w->tierHeight,
	    0);

	w->shownScrollYAdjust = scrollBarValue;

	w->ignoreScrollValueChanged = 0;

	if (!XtIsManaged (vScrollW)) XtManageChild (vScrollW);
    }
} 

void VListMakeLineVisible(Widget ow, Opaque closure, int visible)
{
    VListWidget w = (VListWidget)ow;
    int line, firstVY, lastVYP1, yAdjust, firstVX, lastVXP1, xAdjust;
    int column, tier, x, y;
    int width, height;

    if ((!w->layoutIsValid) || w->deferMakeLineVisible) {
	w->visibleClosure = closure;
	w->visiblePosition = visible;
	return;
    }

    if ((line = ClosureToLine (w, closure)) == (-1)) return;

    LineToColumnTier (w, line, &column, &tier);
    ColumnTierToCell (w, column, tier, &x, &y, &width, &height);

    firstVX  = x + w->requestedXAdjust - w->marginWidth;
    lastVXP1 = x + w->requestedXAdjust + width + w->marginWidth;

    firstVY  = y + w->requestedYAdjust - w->marginHeight;
    lastVYP1 = y + w->requestedYAdjust + height + w->marginHeight;

    xAdjust = w->internalXAdjust;
    yAdjust = w->internalYAdjust;

    switch (visible) {
      case VListVisibleAtRight		:
      case VListVisibleAtTopRight	:
      case VListVisibleAtBottomRight	: xAdjust = lastVXP1 - w->core.width;		break;

      case VListVisibleAtLeft		:
      case VListVisibleAtTopLeft	:
      case VListVisibleAtBottomLeft	: xAdjust = firstVX;				break;
    }

    switch (visible) {
      case VListVisibleAtBottom		:
      case VListVisibleAtBottomLeft	:
      case VListVisibleAtBottomRight	: yAdjust = lastVYP1 - w->core.height;		break;

      case VListVisibleAtTop		:
      case VListVisibleAtTopLeft	:
      case VListVisibleAtTopRight	: yAdjust = firstVY;				break;
    }


    if (visible == VListVisibleInWindow) {
	if (lastVXP1 > (xAdjust + w->core.width))    xAdjust = lastVXP1 - w->core.width;
	if (lastVYP1 > (yAdjust + w->core.height))   yAdjust = lastVYP1 - w->core.height;

        if (firstVX < xAdjust) 			     xAdjust = firstVX;
        if (firstVY < yAdjust) 			     yAdjust = firstVY;
    }

    SetInternalXYAdjust (w, xAdjust, yAdjust);
    StartPendingScroll (w);
}

static void ScrollButtonPressHandler(Widget scrollW, XtPointer ow, XEvent *eventOP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if ((eventP->button != Button1)
      || (eventP->state & (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))) return;

    w->deferMakeLineVisible = 1;
}

static void ScrollButtonReleaseHandler(Widget scrollW, XtPointer ow, XEvent *eventOP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (!w->deferMakeLineVisible) return;

    w->deferMakeLineVisible = 0;

    if (w->visibleClosure) {
	VListMakeLineVisible ((Widget)w, w->visibleClosure, w->visiblePosition);
	w->visibleClosure = 0;
    }
}

static void ComputeLayout(VListWidget w)
{
    ComputeCellHeight (w, 1);
    DoColumnLayout (w);
    ArrangeWidgets (w);
    SetHeaderLabels (w);
    SetInternalXYAdjust (w, w->internalXAdjust, w->internalYAdjust);
    SetPosition (w);
    w->layoutIsValid = 1;
}

static void Redraw(VListWidget w)
{
    SetPosition (w);

    if (!w->visible) return;

    w->redrawSerial = XNextRequest (VtkDisplay (w));

    XClearArea (VtkDisplay (w), VtkWindow (w), 0, 0, w->core.width, w->core.height, 1);
}

void VListEndBatchUpdate(Widget ow)
{
    VListWidget w = (VListWidget)ow;

    if (--w->batchUpdate) return;	/* still batching? */
    if (!VtkIsRealized (w)) return;	/* not real yet */

    if (!w->layoutIsValid) ComputeLayout (w);

    if (w->visibleClosure) {
	VListMakeLineVisible ((Widget)w, w->visibleClosure, w->visiblePosition);
	w->visibleClosure = 0;
    }

    Redraw (w);
}

void VListStartBatchUpdate(Widget ow)
{
    VListWidget w = (VListWidget)ow;

    w->batchUpdate++;
    w->scrollIsActive = w->activeGropCnt = w->layoutIsValid = 0;
}

static void StartUpdateSweep(VListWidget w)
{
    XEvent event;			/* allocate full event buffer */

    if ((!VtkIsRealized (w)) || w->batchUpdate) return;

    VListStartBatchUpdate ((Widget)w);

    event.xclient.type 	  	= ClientMessage;
    event.xclient.display 	= VtkDisplay (w);
    event.xclient.window 	= VtkWindow (w);
    event.xclient.message_type 	= 0;			/*** should be an atom (sjk) */
    event.xclient.format 	= 32;

    XSendEvent (VtkDisplay (w), VtkWindow (w), 0, 0, &event);
}

static void ResizeCallback(Widget unused_pW, XtPointer ow, XtPointer unused_cbDataP)
{
    VListWidget w = (VListWidget)ow;

    StartUpdateSweep (w);
}

static void DrawExposedLines(VListWidget w)
{
    int column, tier, line, firstColumn, lastColumn, firstTier, lastTier;
    LineInfo *lineInfoP;

    if (!w->anyAreExposed) return;	/* nothing to do */
    if (w->scrollIsActive) return;
    if (w->activeGropCnt)  return;

    w->anyAreExposed = 0;

    /* Loop through visible lines, redrawing requested ones. */

    if (!RectangleToLines (w, w->requestedXAdjust, w->requestedYAdjust, w->core.width, w->core.height,
      &firstColumn, &lastColumn, &firstTier, &lastTier)) return;

    for (column = firstColumn; column <= lastColumn; column++)
        for (tier = firstTier; tier <= lastTier; tier++)
	    if ((line = ColumnTierToLine (w, column, tier)) != (-1)) {
		lineInfoP = LineToLineInfo (w, line);
    		if (lineInfoP->expose) Draw (w, line, 0, 0);
    	    }
}

static void SaveExposeRegion(VListWidget w, int vX, int vY, int width, int height)
{
    int firstColumn, lastColumn, firstTier, lastTier, column, tier, line;
    LineInfo *lineInfoP;

    if (!w->layoutIsValid) return;

    if (!RectangleToLines (w, vX, vY, width, height, &firstColumn, &lastColumn, &firstTier, &lastTier)) return;

    for (column = firstColumn; column <= lastColumn; column++)
        for (tier = firstTier; tier <= lastTier; tier++)
    	    if ((line = ColumnTierToLine (w, column, tier)) != (-1)) {
	        lineInfoP = LineToLineInfo (w, line);
	        lineInfoP->expose = 1;
	    }

    w->anyAreExposed = 1;
}

static void DrawExposeRegion(VListWidget w, int vX, int vY, int width, int height)
{
    int firstColumn, lastColumn, firstTier, lastTier, column, tier, line;

    if (!RectangleToLines (w, vX, vY, width, height, &firstColumn, &lastColumn, &firstTier, &lastTier)) return;

    for (column = firstColumn; column <= lastColumn; column++)
        for (tier = firstTier; tier <= lastTier; tier++)
    	    if ((line = ColumnTierToLine (w, column, tier)) != (-1)) Draw (w, line, 0, 0);
}

static void DoExpose(Widget ow, XEvent *eventOP, Region unused_region)
{
    VListWidget w = (VListWidget)ow;
    XExposeEvent *eventP = (XExposeEvent *)eventOP;

    w->visible = 1;

    if (eventP->serial < w->redrawSerial) return;

    if (!w->layoutIsValid) return;

    XClearArea (VtkDisplay (w), VtkWindow (w),
	eventP->x + w->externalXAdjust - w->requestedXAdjust,
	eventP->y + w->externalYAdjust - w->requestedYAdjust,
	eventP->width, eventP->height, 0);

    eventP->y      -= (w->tierHeight * w->activeGropCnt);
    eventP->height += (w->tierHeight * w->activeGropCnt * 2);

    SaveExposeRegion (w, eventP->x + w->externalXAdjust, eventP->y + w->externalYAdjust, eventP->width, eventP->height);

    if (eventP->count) return;	/* more expose events are coming */

    DrawExposedLines (w);
}

static void HandleNonmaskableEvent(Widget ow, XtPointer unused_closure, XEvent *eventP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    int type = eventP->xany.type;
    int gropIsComplete = 0, scrollIsComplete = 0;

    if (type == ClientMessage) {	/* update sweep complete */
        /*** should check message type (sjk) */
	VListEndBatchUpdate ((Widget)w);
	return;
    }

    if (!w->layoutIsValid) return;

    if (type == GraphicsExpose) {
	eventP->xgraphicsexpose.y      -= (w->activeGropCnt * w->tierHeight);
	eventP->xgraphicsexpose.height += (w->activeGropCnt * w->tierHeight * 2);

	SaveExposeRegion (w,
	    eventP->xgraphicsexpose.x + w->requestedXAdjust,
	    eventP->xgraphicsexpose.y + w->requestedYAdjust,
	    eventP->xgraphicsexpose.width,
	    eventP->xgraphicsexpose.height);

        if (eventP->xgraphicsexpose.count) return;		/* don't get too excited yet - there are more to come */
    }

    /* Handle completion of a graphics operation. */

    if (type == NoExpose)
        if (w->scrollIsActive && (eventP->xnoexpose.serial       >= w->scrollSerial)) 	scrollIsComplete = 1;
        else										gropIsComplete   = 1;
    else if (type == GraphicsExpose)
        if (w->scrollIsActive && (eventP->xgraphicsexpose.serial >= w->scrollSerial))	scrollIsComplete = 1;
        else										gropIsComplete   = 1;

    if (scrollIsComplete) {
        w->scrollIsActive = 0;
	w->externalXAdjust = w->requestedXAdjust;
	w->externalYAdjust = w->requestedYAdjust;

	DrawExposedLines (w);

	StartPendingScroll (w);
    } else if (gropIsComplete && w->activeGropCnt) {
	w->activeGropCnt--;
	DrawExposedLines (w);
    }
}

static void SelectLine(VListWidget w, int line, int select, LineInfo *lineInfoP)
{
    int otherLine;

    if (lineInfoP->vLine.select == select) return;
    if (!lineInfoP->vLine.sensitive)       return;

    if ((lineInfoP->vLine.select = select)) w->selectionCnt++;
    else 				    w->selectionCnt--;
    
    if (w->selectUnit == VListSelectUnitField) Flip (w, line);
    else for (otherLine = line; otherLine < (line + w->fieldCnt); otherLine++) Flip (w, otherLine);
}

static void DeselectAllLines(VListWidget w)
{
    int line, increment;

    if (!w->selectionCnt) return;		/* quick check */

    if (w->selectUnit == VListSelectUnitField) increment = 1;
    else 				       increment = w->fieldCnt;

    for (line = 0; w->selectionCnt; line += increment) SelectLine (w, line, 0, LineToLineInfo (w, line));
}

static void SelectAllLines(VListWidget w)
{
    int line, increment;
    LineInfo *lineInfoP;

    if (w->selectUnit == VListSelectUnitField) increment = 1;
    else                                       increment = w->fieldCnt;

    FOR_EACH_LINE_L (w, line, lineInfoP, 0, increment) SelectLine (w, line, 1, lineInfoP);
}

static void CallSelectCallbacks(VListWidget w, XEvent *eventP, int confirm)
{
    VListSelectCallbackStruct cbData;
    int line;
    LineInfo *lineInfoP;
    VListSelectChange *selectChangeP;
	
    cbData.reason = (confirm ? VListCRSelectConfirm : VListCRSelect);
    cbData.event = eventP;

    ColumnTierToFieldRow (w, w->upColumn, w->upTier, &cbData.field, &cbData.row);

    if ((line = ColumnTierToLine (w, w->upColumn, w->upTier)) != (-1)) {
	lineInfoP = LineToLineInfo (w, line);
	cbData.vLine = lineInfoP->vLine;
    } else cbData.vLine.closure = 0;

    if (!confirm) {
	cbData.selectChangesP = w->selectChangesP =
	    (VListSelectChange *)XtRealloc ((char *)w->selectChangesP, w->lineCnt * sizeof (VListSelectChange));

	selectChangeP = w->selectChangesP;
	FOR_EACH_LINE (w, line, lineInfoP) {
	    selectChangeP->closure   = lineInfoP->vLine.closure;
	    selectChangeP->oldSelect = lineInfoP->undoSelect;
	    selectChangeP->newSelect = lineInfoP->vLine.select;
	    selectChangeP++;
	}
    } else cbData.selectChangesP = 0;
    
    cbData.lineCount      = w->lineCnt;
    cbData.selectionCount = w->selectionCnt;

    XtCallCallbacks ((Widget)w, (confirm ? VListNselectConfirmCallback : VListNselectCallback), &cbData);
}

static void NewSelectColumnTier(VListWidget w, int newColumn, int newTier)
{
    int newLine, mouseLine, field, row, line, fieldIncrement;
    int downField, downRow, mouseField, mouseRow, newField, newRow;
    int oldFirstField, oldLastField, oldFirstRow, oldLastRow;
    int newFirstField, newLastField, newFirstRow, newLastRow;
    LineInfo *lineInfoP;

    if ((newColumn == w->mouseColumn) && (newTier == w->mouseTier)) return;

    if ((w->selectStyle == VListSelectStyleNone) || w->selectionIsLocked) return;

    if (w->selectStyle == VListSelectStyleSingle) {
        mouseLine = ColumnTierToLine (w, w->mouseColumn, w->mouseTier);
	newLine   = ColumnTierToLine (w, newColumn,      newTier);

	if (mouseLine != newLine) {
	    if (mouseLine != (-1)) SelectLine (w, mouseLine, 0, LineToLineInfo (w, mouseLine));
	    if (newLine   != (-1)) SelectLine (w, newLine,   1, LineToLineInfo (w, newLine));
	}
    } else {
	ColumnTierToFieldRow (w, w->downColumn,  w->downTier,  &downField,  &downRow);
	ColumnTierToFieldRow (w, w->mouseColumn, w->mouseTier, &mouseField, &mouseRow);
	ColumnTierToFieldRow (w, newColumn,      newTier,      &newField,   &newRow);

	MinMax (downField, mouseField, &oldFirstField, &oldLastField);
	MinMax (downRow,   mouseRow,   &oldFirstRow,   &oldLastRow);
	MinMax (downField, newField,   &newFirstField, &newLastField);
	MinMax (downRow,   newRow,     &newFirstRow,   &newLastRow);

	/* Go through the old select range, resetting any lines that are not included in the new select range. */

	if (w->selectUnit == VListSelectUnitField) fieldIncrement = 1;
    	else fieldIncrement = w->fieldCnt;

 	if (oldFirstField == (-1)) oldFirstField = 0;
	if (newFirstField == (-1)) newFirstField = 0;

	for (row = oldFirstRow; row <= oldLastRow; row++)
	    for (field = oldFirstField; field <= oldLastField; field += fieldIncrement)
		if ((field < newFirstField) || (field > newLastField) || (row < newFirstRow) || (row > newLastRow)) {
		    line = FieldRowToLine (w, field, row);
		    if (line != (-1)) {
			lineInfoP = LineToLineInfo (w, line);
			SelectLine (w, line, lineInfoP->undragSelect, lineInfoP);
		    }
		}

	/* Go through the new select range, setting any lines that are not included in the old select range. */

	for (row = newFirstRow; row <= newLastRow; row++)
	    for (field = newFirstField; field <= newLastField; field += fieldIncrement)
		if ((field < oldFirstField) || (field > oldLastField) || (row < oldFirstRow) || (row > oldLastRow)) {
		    line = FieldRowToLine (w, field, row);
		    if (line != (-1)) SelectLine (w, line, w->selectParity, LineToLineInfo (w, line));
		}
    }	

    w->mouseColumn = newColumn;
    w->mouseTier   = newTier;
}

static void CancelSelect(VListWidget w)
{
    int line;
    LineInfo *lineInfoP;

    w->selectIsActive = w->upTime = 0;

    VListSetAutoScrollPosition ((Widget)w, 0, 0);

    if (!w->selectionIsLocked)
        FOR_EACH_LINE (w, line, lineInfoP)
	    if ((lineInfoP->vLine.select != lineInfoP->undoSelect)) SelectLine (w, line, lineInfoP->undoSelect, lineInfoP);

    SetInternalXYAdjust (w, w->undoXAdjust, w->undoYAdjust);
    StartPendingScroll (w);
}

static void StartSelect(VListWidget w, XButtonEvent *eventP)
{
    int line, field, row;
    LineInfo *lineInfoP;

    w->selectIsActive = 1;

    XYToColumnTier (w, eventP->x, eventP->y, &w->downColumn, &w->downTier);

    /* If the ButtonPress came soon enough after the last ButtonRelease, and we're still over the same cell, and the widget does
     * have at least one selectConfirm callback declared, then prime the doubleClick action. */

    if ((eventP->time <= (w->upTime + w->doubleClickDelay)) && (w->downColumn == w->upColumn) && (w->downTier == w->upTier)
      && (XtHasCallbacks ((Widget)w, VListNselectConfirmCallback) == XtCallbackHasSome)) {
	w->doubleClickIsPending = 1;
	return;
    }

    w->doubleClickIsPending = 0;

    /* Save state for chorded cancel. */

    FOR_EACH_LINE (w, line, lineInfoP) lineInfoP->undoSelect = lineInfoP->vLine.select;

    w->undoXAdjust = w->internalXAdjust;
    w->undoYAdjust = w->internalYAdjust;

    /* If the select style dictates, clear the selection. */

    if ((w->selectStyle == VListSelectStyleSingle) ||
      ((w->selectStyle == VListSelectStyleExtend) && (!(eventP->state & ShiftMask))))
	if (!w->selectionIsLocked) DeselectAllLines (w);

    /* If range select is possible, make a snapshot of the selection state so we can backtrack. */

    if ((w->selectStyle != VListSelectStyleSingle) && (w->selectStyle != VListSelectStyleNone))
        FOR_EACH_LINE (w, line, lineInfoP) lineInfoP->undragSelect = lineInfoP->vLine.select;

    /* Determine the select parity, and flip the target line's selection. */

    if ((w->selectStyle != VListSelectStyleNone) && (!w->selectionIsLocked)) {
	if (w->selectStyle != VListSelectStyleSingle) {
	    ColumnTierToFieldRow ( w, w->downColumn, w->downTier, &field, &row);
	    line = FieldRowToLine (w, field, row);
	} else line = ColumnTierToLine (w, w->downColumn, w->downTier);

	if (line == (-1)) w->selectParity = 1;
	else {
	    lineInfoP = LineToLineInfo (w, line);
	    w->selectParity = (1 - lineInfoP->vLine.select);
	    SelectLine (w, line, w->selectParity, lineInfoP);
	}
    }

    w->mouseColumn = w->downColumn;
    w->mouseTier   = w->downTier;
}

static void SelectMotionHandler(VListWidget w, XMotionEvent *eventP)
{
    int newColumn, newTier;

    if (w->doubleClickIsPending) return;

    XYToColumnTier (w, eventP->x, eventP->y, &newColumn, &newTier);
    NewSelectColumnTier (w, newColumn, newTier);
    VListSetAutoScrollPosition ((Widget)w, eventP->x, eventP->y);
}

static void SelectReleaseHandler(VListWidget w, XButtonEvent *eventP)
{
    int x, y;

    w->selectIsActive = 0;			/* done with list grab */

    VListSetAutoScrollPosition ((Widget)w, 0, 0);

    /* If the select style supports drag select, and we're outside the window, treat the upclick as though it were just
     * inside the window to prevent unseen lines from being selected. */

    x = eventP->x;
    y = eventP->y;

    if ((w->selectStyle != VListSelectStyleSingle) && (!w->doubleClickIsPending)) {
	if (x < 0) x = 0;
	else if (x >= w->core.width) x = w->core.width - 1;
    
	if (y < 0) y = 0;
	else if (y >= w->core.height) y = w->core.height - 1;
    }

    XYToColumnTier (w, x, y, &w->upColumn, &w->upTier);

    if (w->doubleClickIsPending) {
	w->upTime = 0;			/* don't want tripleClick problem */

	/* If the user didn't drag off the entry, then call the SelectConfirm callbacks. */

	if ((w->upColumn == w->downColumn) && (w->upTier == w->downTier)) CallSelectCallbacks (w, (XEvent *)eventP, 1);
    } else {
	w->upTime = eventP->time;	/* to test against next Down time */

	NewSelectColumnTier (w, w->upColumn, w->upTier);
	CallSelectCallbacks (w, (XEvent *)eventP, 0);
    }
}

static void CallDragArmCallbacks(VListWidget w, XEvent *eventP)
{
    VListDragArmCallbackStruct cbData;
    int line;
    LineInfo *lineInfoP;
	
    cbData.reason = VListCRDragArm;
    cbData.event  = eventP;

    ColumnTierToFieldRow (w, w->downColumn, w->downTier, &cbData.field, &cbData.row);

    if ((line = ColumnTierToLine (w, w->downColumn, w->downTier)) != (-1)) {
	lineInfoP = LineToLineInfo (w, line);
	cbData.vLine = lineInfoP->vLine;
    } else cbData.vLine.closure = 0;

    /* Copy into VListDragDropCallbackStruct for later use. */

    w->dragDropCallbackStruct.field = cbData.field;
    w->dragDropCallbackStruct.row   = cbData.row;
    w->dragDropCallbackStruct.vLine = cbData.vLine;

    XtCallCallbacks ((Widget)w, VListNdragArmCallback, &cbData);
}

static void CallDragDropCallbacks(VListWidget w, XButtonEvent *eventP)
{
    VListDragDropCallbackStruct *cbDataP = (&w->dragDropCallbackStruct);
    Window dragParentWindow = VtkWindow (VtkParent (w->dragW));
    int column, tier, line;
    LineInfo *lineInfoP;
    int dragParentRelativeX, dragParentRelativeY;
    Window dummyChildWindow;
    VListWidget dropW;

    cbDataP->reason 	= VListCRDragDrop;
    cbDataP->event  	= (XEvent *)eventP;

    XTranslateCoordinates (eventP->display, RootWindowOfScreen (XtScreen (w)), dragParentWindow,
	eventP->x_root, eventP->y_root, &dragParentRelativeX, &dragParentRelativeY, &dummyChildWindow);

    cbDataP->dropWindow = VtkGetSubwindow (eventP->display, dragParentWindow, dragParentRelativeX, dragParentRelativeY, 0,
	&cbDataP->dropX, &cbDataP->dropY);

    cbDataP->dropWidget = XtWindowToWidget (eventP->display, cbDataP->dropWindow);

    cbDataP->dropVLine.closure = 0;		/* assume target VLine is unknown */

    if (cbDataP->dropWidget && XtIsSubclass (cbDataP->dropWidget, vlistwidgetclass)) {
	dropW = (VListWidget)cbDataP->dropWidget;

        cbDataP->dropWidgetIsVList = 1;

	XYToColumnTier (dropW, cbDataP->dropX, cbDataP->dropY, &column, &tier);
	ColumnTierToFieldRow (dropW, column, tier, &cbDataP->dropField, &cbDataP->dropRow);

	if ((line = ColumnTierToLine (dropW, column, tier)) != (-1)) {
	    lineInfoP = LineToLineInfo (dropW, line);
	    cbDataP->dropVLine = lineInfoP->vLine;
	} else cbDataP->dropVLine.closure = 0;

	cbDataP->dropVListFieldCount = dropW->fieldCnt;
	cbDataP->dropVListRowCount   = dropW->rowCnt;
    } else cbDataP->dropWidgetIsVList = 0;

    XtCallCallbacks ((Widget)w, VListNdragDropCallback, cbDataP);
}

static void CancelDrag(VListWidget w)
{
    w->dragIsActive = 0;

    XUnmapWindow (VtkDisplay (w->dragW), VtkWindow (w->dragW));

    VListSetAutoScrollPosition ((Widget)w, 0, 0);
    SetInternalXYAdjust (w, w->undoXAdjust, w->undoYAdjust);
    StartPendingScroll (w);
}

static void StartDrag(VListWidget w, XButtonEvent *eventP)
{
    Widget dragW;
    Window dragParentWindow, dummyChildWindow;
    XWindowChanges changes;

    XYToColumnTier (w, eventP->x, eventP->y, &w->downColumn, &w->downTier);

    CallDragArmCallbacks (w, (XEvent *)eventP);

    if (!(dragW = w->dragW)) return;

    /* Create the drag widget's window without ever mapping it and leave it unmanaged. */

    if (!VtkIsRealized (dragW)) {
	XtVaSetValues (dragW, XtNmappedWhenManaged, 0, 0);
	XtRealizeWidget (dragW);
	XtManageChild (dragW);
	XtUnmanageChild (dragW);
	XtVaSetValues (dragW, XtNmappedWhenManaged, 1, 0);

	/* If a child of the root, override redirect to avoid window manager interference. */
    
	if (VtkWindow (VtkParent (dragW)) == RootWindowOfScreen (XtScreen (dragW))) {
	    XSetWindowAttributes attrs;
	    attrs.override_redirect = 1;
	    XChangeWindowAttributes (VtkDisplay (dragW), VtkWindow (dragW), CWOverrideRedirect, &attrs);
	}
    }
    
    dragParentWindow = VtkWindow (VtkParent (dragW));

    if (eventP->window == dragParentWindow) w->dragAdjustX = w->dragAdjustY = 0;
    else XTranslateCoordinates (eventP->display, eventP->window, dragParentWindow, 0, 0, &w->dragAdjustX, &w->dragAdjustY,
	&dummyChildWindow);

    w->dragX = dragW->core.width  / 2 + dragW->core.border_width;
    w->dragY = dragW->core.height / 2 + dragW->core.border_width;

    changes.x = dragW->core.x = eventP->x - w->dragX + w->dragAdjustX;
    changes.y = dragW->core.y = eventP->y - w->dragY + w->dragAdjustY;

    XConfigureWindow (VtkDisplay (dragW), VtkWindow (dragW), CWX | CWY, &changes);
    XMapRaised       (VtkDisplay (dragW), VtkWindow (dragW));

    w->dragIsActive = 1;
}

static void DragMotionHandler(VListWidget w, XMotionEvent *eventP)
{
    XEvent event;
    XWindowChanges changes;
    int changeMask = 0;
    int dragMotionMask;

    /* Poor-man's motion compression.  Scan event queue until we have the most recent motion event. */

    switch (w->dragButton) {
	case VListButton1	: dragMotionMask = Button1MotionMask;		break;
	case VListButton2	: dragMotionMask = Button2MotionMask;		break;
	case VListButton3	: dragMotionMask = Button3MotionMask;		break;
	case VListButton4	: dragMotionMask = Button4MotionMask;		break;
	case VListButton5	: dragMotionMask = Button5MotionMask;		break;
    }
                
    event.xmotion = *eventP;
    while (XCheckWindowEvent (eventP->display, eventP->window, dragMotionMask, &event) == True) ;

    if (VtkParent (w->dragW) == (Widget)w) VListSetAutoScrollPosition ((Widget)w, event.xmotion.x, event.xmotion.y);

    /* If the pointer has moved, update the widget location.  Bypass XtSetValues because it causes the widget to redraw itself! */

    if (w->dragW->core.x != (changes.x = event.xmotion.x - w->dragX + w->dragAdjustX)) changeMask |= CWX;
    if (w->dragW->core.y != (changes.y = event.xmotion.y - w->dragY + w->dragAdjustY)) changeMask |= CWY;

    if (changeMask) {
        XConfigureWindow (VtkDisplay (w->dragW), VtkWindow (w->dragW), changeMask, &changes);
        w->dragW->core.x = changes.x;
        w->dragW->core.y = changes.y;
    }

    /* Synch with the server, to help motion compression do its thing. */

    XSync (eventP->display, 0);
}

static void DragReleaseHandler(VListWidget w, XButtonEvent *eventP)
{
    w->dragIsActive = 0;			/* done with list grab */

    XUnmapWindow (VtkDisplay (w->dragW), VtkWindow (w->dragW));

    VListSetAutoScrollPosition ((Widget)w, 0, 0);

    CallDragDropCallbacks (w, eventP);
}

static void ButtonPressHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    /* Chorded cancel. */

    if (w->selectIsActive) 			{CancelSelect (w);		return;}
    if (w->dragIsActive) 			{CancelDrag   (w);		return;}

    /* If this is not a simple un-chorded Button event, ignore it. */

    if (eventP->state & (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask)) return;

    if ((eventP->button == w->selectButton) || (w->selectButton == AnyButton)) {StartSelect  (w, eventP);	return;}
    if ((eventP->button == w->dragButton)   || (w->dragButton   == AnyButton)) {StartDrag    (w, eventP); 	return;}
}

static void PointerMotionHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    XMotionEvent *eventP = (XMotionEvent *)eventOP;

    if (w->selectIsActive) 	{SelectMotionHandler (w, eventP); 	return;}
    if (w->dragIsActive)	{DragMotionHandler   (w, eventP); 	return;};
}

static void ButtonReleaseHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    VListWidget w = (VListWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (w->selectIsActive) 	{SelectReleaseHandler (w, eventP);	return;}
    if (w->dragIsActive)	{DragReleaseHandler   (w, eventP);	return;}
}

static void DoInitialize(Widget unused_requestOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VListWidget w = (VListWidget)ow;

    VtkSetFieldsToZero ((char *)&w->beginZeroInit, (char *)&w->endZeroInit);

    w->currentFontidx = (-1);

    CopyColors (w);
    CopyFonts (w);
    CopyFields (w);
    CopyLabels (w);
}

static void SniffOutSlaves(VListWidget w)
{
    Widget vFrameW = VtkParent (w);

    if (!w->hScrollW) 
	if ((w->hScrollW = (Widget)VFrameGetScrollBarWidget (vFrameW, XmHORIZONTAL))) XtUnmanageChild (w->hScrollW);

    if (!w->vScrollW)
	if ((w->vScrollW = (Widget)VFrameGetScrollBarWidget (vFrameW, XmVERTICAL)))   XtUnmanageChild (w->vScrollW);

    if (!w->vHeaderW)
	if ((w->vHeaderW = (Widget)VtkGetChildByClass (vFrameW, vheaderwidgetclass))) XtUnmanageChild (w->vHeaderW);
}

static void RealizeHandlersAndCallbacks(VListWidget w)
{
    Widget hScrollW, vScrollW;

    if ((hScrollW = w->hScrollW)) {
	XtAddCallback (hScrollW, XmNpageIncrementCallback, 	HPageIncCallback, 	w);
	XtAddCallback (hScrollW, XmNpageDecrementCallback, 	HPageDecCallback, 	w);
	XtAddCallback (hScrollW, XmNdragCallback, 	   	HValueChangedCallback, 	w);
	XtAddCallback (hScrollW, XmNvalueChangedCallback, 	HValueChangedCallback, 	w);

        XtVaSetValues (hScrollW,
	    XmNminimum, 	0,
	    XmNincrement, 	1,
	    XmNpageIncrement,  	1,
	    0);

	XtAddRawEventHandler (hScrollW, ButtonPressMask,   0, ScrollButtonPressHandler,   w);
	XtAddRawEventHandler (hScrollW, ButtonReleaseMask, 0, ScrollButtonReleaseHandler, w);
    } else w->hScrollPolicy = VListScrollBarPolicyNever;

    if ((vScrollW = w->vScrollW)) {
	XtAddCallback (vScrollW, XmNpageIncrementCallback, 	VPageIncCallback, 	w);
	XtAddCallback (vScrollW, XmNpageDecrementCallback, 	VPageDecCallback, 	w);
	XtAddCallback (vScrollW, XmNdragCallback, 	   	VValueChangedCallback, 	w);
	XtAddCallback (vScrollW, XmNvalueChangedCallback, 	VValueChangedCallback, 	w);
    
	XtVaSetValues (vScrollW,
	    XmNminimum, 	0,
	    XmNincrement, 	1,
	    XmNpageIncrement,  	1,
	    0);

	XtAddRawEventHandler (vScrollW, ButtonPressMask,   0, ScrollButtonPressHandler,   w);
	XtAddRawEventHandler (vScrollW, ButtonReleaseMask, 0, ScrollButtonReleaseHandler, w);
    } else w->vScrollPolicy = VListScrollBarPolicyNever;

    /* Trap resize events on our parent (VFrame) widget. */

    XtAddCallback (VtkParent (w), VFrameNresizeCallback, ResizeCallback, w);

    /* Register an event handler for nonmaskable events. */

    XtAddRawEventHandler ((Widget)w, 0, 1, HandleNonmaskableEvent, 0);

    /* Register raw event handlers for the events that we will receive as a result of our passive grab on Button. */

    XtAddRawEventHandler ((Widget)w, ButtonPressMask,   0, ButtonPressHandler,   0);
    XtAddRawEventHandler ((Widget)w, PointerMotionMask, 0, PointerMotionHandler, 0);
    XtAddRawEventHandler ((Widget)w, ButtonReleaseMask, 0, ButtonReleaseHandler, 0);
}

static void RealizeGCs(VListWidget w)
{
    XGCValues values;

    /* The GCs must be private (allocated with XCreateGC) rather than shared (allocated with XtGetGC) because they are
     * not read-only.  Their colors and fonts are changed dynamically.  Start with foreground and background equal to zero,
     * since that is the initial setting of w->currentForeground and w->currentBackground. */

    values.background = 0;
    values.foreground = 0;
    values.function = GXcopy;
    values.line_style = LineOnOffDash;
    w->normgc = XCreateGC (VtkDisplay (w), VtkWindow (w), GCBackground | GCForeground | GCFunction | GCLineStyle, &values);

    values.background = 0;
    values.foreground = 0;
    values.function = GXcopy;
    w->invgc = XCreateGC (VtkDisplay (w), VtkWindow (w), GCBackground | GCForeground | GCFunction, &values);

    values.function = GXinvert;
    values.plane_mask = 0;
    w->flipgc = XCreateGC (VtkDisplay (w), VtkWindow (w), GCPlaneMask | GCFunction, &values);
}

static void RealizeGrabs(VListWidget w)
{
    /* Establish a passive grab for selection. */

    XGrabButton (
	VtkDisplay (w),							/* display */
	w->selectButton,						/* button_grab */
	AnyModifier,							/* modifiers */
	VtkWindow (w),							/* window */
	0,								/* owner_events */
	(ButtonPressMask | ButtonReleaseMask | ButtonMotionMask),	/* event_mask */
	GrabModeAsync,							/* pointer_mode */
	GrabModeAsync,							/* keyboard_mode */
	None,								/* confine_to */
	None);								/* cursor */

    /* Establish a passive grab for dragging.  */

    if ((w->dragButton != w->selectButton) && (w->selectButton != AnyButton))
	XGrabButton (
	    VtkDisplay (w),							/* display */
	    w->dragButton,							/* button_grab */
	    AnyModifier,							/* modifiers */
	    VtkWindow (w),							/* window */
	    0,									/* owner_events */
	    (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask),		/* event_mask */
	    GrabModeAsync,							/* pointer_mode */
	    GrabModeAsync,							/* keyboard_mode */
	    None,								/* confine_to */
	    None);								/* cursor */
}

static void SetWindowBackground(VListWidget w)
{
    switch (w->gridStyle) {
      case VListGridStylePixmap :     XSetWindowBackgroundPixmap (VtkDisplay (w), VtkWindow (w), w->gridPixmap);
					break;
      case VListGridStyleForeground : XSetWindowBackground       (VtkDisplay (w), VtkWindow (w), w->foreground);
					break;
      case VListGridStyleBackground : XSetWindowBackground       (VtkDisplay (w), VtkWindow (w), w->core.background_pixel);
					break;
    }
}

static void DoRealize(Widget ow, XtValueMask *maskP, XSetWindowAttributes *attributesP)
{
    VListWidget w = (VListWidget)ow;

    SniffOutSlaves (w);
    ComputeLabelInfo (w);

    /* Give a BitGravity to minimize repaints during resize. */

    *maskP |= CWBitGravity;
    attributesP->bit_gravity = NorthWestGravity;

    if ((!w->core.width) || (!w->core.height)) {
	w->core.width  = w->core.parent->core.width;
    	w->core.height = w->core.parent->core.height;
	w->core.x      = (-w->core.border_width);
	w->core.y      = (-w->core.border_width);
    }

    XtCreateWindow ((Widget)w, InputOutput, CopyFromParent, *maskP, attributesP);

    if (w->gridStyle != VListGridStyleBackground) SetWindowBackground (w);

    RealizeGCs (w);
    RealizeGrabs (w);
    RealizeHandlersAndCallbacks (w);

    StartUpdateSweep (w);
}

#define Changed(f) (w->f != oldW->f)

static Boolean DoSetValues(Widget oldOW, Widget unused_refOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VListWidget oldW = (VListWidget)oldOW;
    VListWidget w = (VListWidget)ow;

    if (Changed (fieldsP) || Changed (fieldCnt)) {
	FreeFields (oldW);
	CopyFields (w);
	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    if (Changed (labelsP) || Changed (labelCnt)) {
	FreeLabels (oldW);
	CopyLabels (w);
	if (VtkIsRealized (w)) ComputeLabelInfo (w);
	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    if (Changed (selectStyle) || Changed (selectUnit)) {
	DeselectAllLines (oldW);
	w->selectionCnt = 0;
    }

    if  (Changed (columnStyle)		|| Changed (spaceGroupsEqually)		|| Changed (spaceTiersEqually)
      || Changed (fieldSpacingWidth)	|| Changed (hScrollPolicy) 		|| Changed (allowDeadspace)
      || Changed (marginWidth)		|| Changed (marginHeight)
      || Changed (rowHeight) 		|| Changed (rowSpacingHeight)
      || Changed (vScrollPolicy)	|| Changed (vScrollPosition)
	) if (w->layoutIsValid) StartUpdateSweep (w);

    if (Changed (core.background_pixel)) {
	if (VtkIsRealized (w)) {
	    XSetBackground (XtDisplay (w), w->normgc, w->core.background_pixel);
	    XSetForeground (XtDisplay (w), w->invgc,  w->core.background_pixel);
	    XSetPlaneMask  (XtDisplay (w), w->flipgc, w->core.background_pixel);
	}

	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    if (Changed (gridStyle) || Changed (gridPixmap)) {
	SetWindowBackground (w);
	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    if (Changed (colorList)) {
	FreeColors (oldW);
	CopyColors (w);
	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    if (Changed (fontTableP)) {
	FreeFonts (oldW);
	CopyFonts (w);
	if (w->layoutIsValid) StartUpdateSweep (w);
    }

    return (0);
}

static void DoDestroy(Widget ow)
{
    VListWidget w = (VListWidget)ow;

    FreeLines (w);

    if (w->labelsP) 	FreeLabels 	(w);
    if (w->colorTableP) FreeColors 	(w);
    if (w->fontTableP)	FreeFonts 	(w);
}

#define XY(n, c, r, t, f, dr, d)	{n, c, r, sizeof(t), XtOffset(VListWidget, f), dr, (char *)d},
#define XX(n, c, r, t, f, d)     	XY (n, c, r, t, f, XtRImmediate, d)
#define XI(n, f, d) 		 	XX (n, XtCValue, XtRInt, int, f, d)

#define DEFAULT_DRAG_BUTTON	VListButton2

static XtResource resources[] = {

XI (VListNalignment, 			alignment, 		VListAlignmentBeginning)
XI (VListNallowDeadspace,		allowDeadspace,		0)
XI (VListNautoScrollRate, 	  	autoScrollRate, 	15)
XI (VListNcolumnStyle, 		  	columnStyle, 		VListColumnStyleBestFit)
XI (VListNdoubleClickDelay, 	  	doubleClickDelay, 	250)
XI (VListNdragButton,			dragButton,		DEFAULT_DRAG_BUTTON)
XI (VListNfieldResizePolicy,		fieldResizePolicy,	VListResizePolicyAny)
XI (VListNfieldCount, 		  	fieldCnt, 		1)
XI (VListNfieldSpacingWidth, 	  	fieldSpacingWidth, 	0)
XI (VListNgridDashOff, 		  	gridDashOff, 		2)
XI (VListNgridDashOn, 		  	gridDashOn, 		2)
XI (VListNgridStyle,			gridStyle,		VListGridStyleBackground)
XI (VListNhistHeight, 		  	histHeight, 		10)
XI (VListNhistMarginHeight, 	  	histMarginHeight, 	4)
XI (VListNhistMarginWidth, 	  	histMarginWidth, 	4)
XI (VListNhistWidth, 		  	histWidth, 		100)
XI (VListNhScrollBarPolicy, 	  	hScrollPolicy, 		VListScrollBarPolicyAsNeeded)
XI (VListNiconMarginHeight, 	  	iconMarginHeight, 	4)
XI (VListNiconMarginWidth, 	  	iconMarginWidth, 	4)
XI (VListNlabelCount, 		  	labelCnt, 		0)
XI (VListNlabelPadOption, 	  	labelPadOption, 	VListLabelPadWithDashes)
XI (VListNmarginHeight, 	  	marginHeight, 		6)
XI (VListNmarginWidth, 		  	marginWidth, 		8)
XI (VListNrowHeight, 		  	rowHeight, 		0)
XI (VListNrowSpacingHeight, 	  	rowSpacingHeight, 	0)
XI (VListNselectButton,			selectButton, 		VListButton1)
XI (VListNselectStyle, 		  	selectStyle, 		VListSelectStyleSingle)
XI (VListNselectUnit, 		  	selectUnit, 		VListSelectUnitRow)
XI (VListNselectionIsLocked,		selectionIsLocked,	0)
XI (VListNspaceGroupsEqually,		spaceGroupsEqually,	0)
XI (VListNspaceTiersEqually,		spaceTiersEqually,	0)
XI (VListNtextMarginHeight, 	  	textMarginHeight, 	0)
XI (VListNtextMarginWidth, 	  	textMarginWidth, 	4)
XI (VListNvScrollBarPolicy, 	  	vScrollPolicy, 		VListScrollBarPolicyAsNeeded)
XI (VListNvScrollBarPosition, 	  	vScrollPosition, 	VListScrollBarPositionRight)

XX (VListNuserData,   		 XtCValue,      XtRPointer,     XtPointer,      userData,       0)
XX (VListNcolorList, 		 XtCValue, 	XtRPixmap, 	Pixmap,  	colorList, 	0)
XX (VListNfields, 		 XtCValue, 	XtRPointer, 	VListField *,  	fieldsP, 	0)
XX (VListNgridPixmap, 		 XtCValue, 	XtRPixmap, 	Pixmap,  	gridPixmap, 	0)
XX (VListNlabels, 		 XtCValue, 	XtRPointer, 	VListLabel *,  	labelsP, 	0)

XX (VListNdragWidget, 		 XtCValue, 	XtRWindow, 	Widget,  	dragW, 		0)
XX (VListNheaderWidget, 	 XtCValue, 	XtRWindow, 	Widget,  	vHeaderW, 	0)
XX (VListNhScrollBarWidget, 	 XtCValue, 	XtRWindow, 	Widget,  	hScrollW, 	0)
XX (VListNvScrollBarWidget, 	 XtCValue, 	XtRWindow, 	Widget,  	vScrollW, 	0)

XY (VListNfontTable, 		 XmCFontList, 	XmRFontList, 	struct _XmFontListRec *,  	fontTableP, 	XtRString, 0)
XY (VListNforeground, 		 XtCForeground, XtRPixel, 	Pixel,  	foreground, 	XtRString, XtDefaultForeground)

XY (VListNdragArmCallback, 	 XtCValue, 	XtRCallback, 	XtCallbackList, dragArmCallback, 	XtRCallback, 0)
XY (VListNdragDropCallback, 	 XtCValue, 	XtRCallback, 	XtCallbackList, dragDropCallback, 	XtRCallback, 0)
XY (VListNselectCallback, 	 XtCValue, 	XtRCallback, 	XtCallbackList, selectCallback, 	XtRCallback, 0)
XY (VListNselectConfirmCallback, XtCValue, 	XtRCallback, 	XtCallbackList, selectConfirmCallback, 	XtRCallback, 0)
};

#undef XX
#undef XY
#undef XI

externaldef(vlistwidgetclassrec) VListClassRec vlistwidgetclassrec = {

    {/* core_class fields	*/
	/* superclass		*/	(WidgetClass)&compositeClassRec,
	/* class_name	  	*/	"VList",
	/* widget_size	  	*/	sizeof(VListWidgetRec),
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
	/* resize		*/	0,
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
    },
    {/* composite class fields	*/
	/* geometry_manager	*/	VtkPermissiveGeometryManager,
	/* change_managed	*/	(XtWidgetProc)VtkNullRoutine,
	/* insert_child		*/	(XtWidgetProc)_XtInherit,
	/* delete_child		*/	(XtWidgetProc)_XtInherit,
	/* extension		*/	0
    }
};

externaldef(vlistwidgetclass) WidgetClass vlistwidgetclass = (WidgetClass)&vlistwidgetclassrec;

Widget VListCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
{
    return (XtCreateWidget (nameP, vlistwidgetclass, pW, argsP, argCnt));
}

void VListInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "VList", "VListCreate", VListCreate, vlistwidgetclass);
}

void VListSelectAll(Widget ow, int select)
{
    VListWidget w = (VListWidget)ow;

    if (select) SelectAllLines (w);
    else DeselectAllLines (w);
}

Opaque VListGetFirstSelectedLine(Widget ow)
{
    VListWidget w = (VListWidget)ow;
    int line;
    LineInfo *lineInfoP;

    FOR_EACH_LINE (w, line, lineInfoP) if (lineInfoP->vLine.select) return (lineInfoP->vLine.closure);

    return (0);
}

int VListGetAllSelectedLines(Widget ow, Opaque **closurePP)
{
    VListWidget w = (VListWidget)ow;
    Opaque *closureP;
    int remCnt;
    LineInfo *lineInfoP;

    if (!closurePP) return (w->selectionCnt);		/* simply return number of selected entries */

    if (!w->selectionCnt) closureP = (Opaque *)XtMalloc (1);
    else 		  closureP = (Opaque *)XtMalloc (w->selectionCnt * sizeof (Opaque));

    *closurePP = closureP;

    for (remCnt = w->selectionCnt, lineInfoP = w->lineInfosP; remCnt; lineInfoP++)
	if (lineInfoP->vLine.select) {
	    *(closureP++) = lineInfoP->vLine.closure;
	    remCnt--;
	}

    return (w->selectionCnt);
}

static int GetLines(VListWidget w, int firstLine, int increment, Opaque **closurePP)
{
    Opaque *closureP;
    int line;
    LineInfo *lineInfoP;

    if (!w->lineCnt) closureP = (Opaque *)XtMalloc (1);
    else             closureP = (Opaque *)XtMalloc (w->lineCnt / increment * sizeof (Opaque));

    *closurePP = closureP;

    FOR_EACH_LINE_L (w, line, lineInfoP, firstLine, increment) *(closureP++) = lineInfoP->vLine.closure;

    return (w->lineCnt);
}

int VListGetAllLines(Widget ow, Opaque **closurePP)
{
    VListWidget w = (VListWidget)ow;

    return (GetLines (w, 0, 1, closurePP));
}

int VListGetFieldLines(Widget ow, int field, Opaque **closurePP)
{
    VListWidget w = (VListWidget)ow;

    return (GetLines (w, field, w->fieldCnt, closurePP));
}

int VListGetLineCount(Widget ow)
{
    VListWidget w = (VListWidget)ow;

    return (w->lineCnt);
}

int VListGetLineInfo(Widget ow, Opaque closure, VListLine *vLineP, int *fieldP, int *rowP)
{
    VListWidget w = (VListWidget)ow;
    int line;
    LineInfo *lineInfoP;

    if ((line = ClosureToLine (w, closure)) == (-1)) return (0);

    if (vLineP) {
	lineInfoP = LineToLineInfo (w, line);
    	*vLineP = lineInfoP->vLine;
    }

    if (fieldP || rowP) {
	if (fieldP) *fieldP = LineToField (w, line);
	if (rowP)   *rowP   = LineToRow   (w, line);
    }

    return (1);
}

Opaque VListFieldRowToLine(Widget ow, int field, int row)
{
    VListWidget w = (VListWidget)ow;
    int line;
    LineInfo *lineInfoP;

    if ((line = FieldRowToLine (w, field, row)) == (-1)) return (0);
    lineInfoP = LineToLineInfo (w, line);
    return (lineInfoP->vLine.closure);    
}

Opaque VListXYToLine(Widget ow, int x, int y)
{
    VListWidget w = (VListWidget)ow;
    int line, column, tier;
    LineInfo *lineInfoP;

    XYToColumnTier (w, x, y, &column, &tier);
    if ((line = ColumnTierToLine (w, column, tier)) == (-1)) return (0);
    lineInfoP = LineToLineInfo (w, line);
    return (lineInfoP->vLine.closure);
}

Opaque VListAddLine(Widget ow, VListLine *vLinesP, int row)
{
    VListWidget w = (VListWidget)ow;
    VListLine *vLineP;
    LineInfo *lineInfoP, *firstNewLineInfoP;
    VListField *vFieldP;
    int firstNewLine, field, line, insertAtEnd, newLineCnt, selectCnt, firstVY, lastVYP1, x, fromY, width, height, toY;

    if (w->layoutIsValid && (w->groupCnt != 1)) StartUpdateSweep (w);

    /* Determine how many of the new items are selected. */

    selectCnt = 0;
    for (field = 0, vLineP = vLinesP; field < w->fieldCnt; field++, vLineP++)
	if ((vLineP->mask & VListLineSelectMask) && vLineP->select) selectCnt++;

    /* If any of the new lines are selected and we're doing single select style, then clear the previous selection. */

    if (selectCnt && (w->selectStyle == VListSelectStyleSingle)) DeselectAllLines (w);

    /* Extend the LineInfo array if necessary. */

    newLineCnt = w->lineCnt + w->fieldCnt;

    if (newLineCnt > w->maxLineCnt) {
	w->maxLineCnt = (newLineCnt * 3 / 2 + 8);	/* somewhat arbitrary */
	w->lineInfosP = (LineInfo *)XtRealloc ((char *)w->lineInfosP, w->maxLineCnt * sizeof (LineInfo));
    }

    /* Find the insertion point, shuffle and initialize. */

    if ((row < 0) || ((firstNewLine = (row * w->fieldCnt)) >= w->lineCnt)) {
	firstNewLine = w->lineCnt;
	insertAtEnd = 1;
    } else insertAtEnd = 0;

    w->lineCnt += w->fieldCnt;
    w->rowCnt++;
    w->selectionCnt += selectCnt;

    for (line = (w->lineCnt - 1); line >= (firstNewLine + w->fieldCnt); line--)
	w->lineInfosP[line] = w->lineInfosP[line - w->fieldCnt];

    lineInfoP = LineToLineInfo (w, firstNewLine);
    firstNewLineInfoP = lineInfoP;

    vLineP = vLinesP;
    FOR_EACH_LINE_IN_RECORD (w, line, lineInfoP, field, vFieldP, firstNewLine) {
	lineInfoP->vLine = (*(vLineP++));
	SupplyLineDefaults (w, &lineInfoP->vLine, vFieldP);
        if (lineInfoP->vLine.textP) lineInfoP->vLine.textP = XtNewString (lineInfoP->vLine.textP);

	lineInfoP->expose = 0;
	lineInfoP->segmentInfosP = 0;

	if (lineInfoP->vLine.segmentCount) {
	    CopySegments (lineInfoP);
	    SupplySegmentDefaults (w, lineInfoP);
	}

        if (lineInfoP->vLine.barCount) {
	    CopyBars (lineInfoP);
	    SupplyBarDefaults (w, &lineInfoP->vLine);
	}

	ComputeCellInfo (w, vFieldP, lineInfoP);

	if (w->layoutIsValid)
	    if (AddColumnWidthContribution (w, field, lineInfoP->bodyWidth  + 2 * vFieldP->marginWidth, vFieldP))
		StartUpdateSweep (w);

	if (w->layoutIsValid)
	    if (AddCellHeightContribution (w,         lineInfoP->bodyHeight + 2 * vFieldP->marginHeight))
		StartUpdateSweep (w);
    }

    if (!w->layoutIsValid) return (firstNewLineInfoP->vLine.closure);

    w->tierCnt = w->rowCnt;
    ComputeLogicalHeight (w);
    ArrangeWidgets (w);

    if (!insertAtEnd) {
	firstVY  = w->marginHeight + row * w->tierHeight;
	lastVYP1 = firstVY + w->tierHeight;
	x        = 0;
	fromY    = firstVY - w->requestedYAdjust;
	width    = w->core.width;
	height   = (w->rowCnt - row - 1) * w->tierHeight + w->marginHeight;
	toY      = lastVYP1 - w->requestedYAdjust;

	if (fromY <= 0) {
	    w->internalYAdjust  += w->tierHeight;
	    w->requestedYAdjust += w->tierHeight;
	    w->externalYAdjust  += w->tierHeight;
        } else if (fromY < w->core.height) {
	    SetStippled (w, 0, 0, 0);
	    if (height > w->core.height) height = w->core.height;
	    XCopyArea (VtkDisplay (w), VtkWindow (w), VtkWindow (w), w->normgc, x, fromY, width, height, x, toY);
	    w->activeGropCnt++;
	}
    }

    for (line = firstNewLine; line < (firstNewLine + w->fieldCnt); line++) Draw (w, line, 1, 1);

    return (firstNewLineInfoP->vLine.closure);
}

static void ChangeLine(VListWidget w, int line, VListLine *vLineP)
{
    int field, selectLine, column, tier, oldBodyWidth, oldBodyHeight;
    int redraw = 0, recomputeCellInfo = 0, resupplySegmentDefaults = 0, resupplyBarDefaults = 0;
    int flipSelect = 0;
    LineInfo *lineInfoP, *selectLineInfoP;
    VListField *vFieldP;

    lineInfoP  = LineToLineInfo (w, line);
    field      = LineToField (w, line);
    vFieldP    = FieldToVField (w, field);

    if ((vLineP->mask & VListLineClosureMask) && vLineP->closure) lineInfoP->vLine.closure = vLineP->closure;

    if ((vLineP->mask & VListLineTextMask) && ((!lineInfoP->vLine.textP) || strcmp (vLineP->textP, lineInfoP->vLine.textP))) {
	XtFree ((char *)lineInfoP->vLine.textP);
        lineInfoP->vLine.textP = XtNewString (vLineP->textP);
	recomputeCellInfo = redraw = 1;
    }

    if (vLineP->mask & VListLineSegmentsMask) {
	lineInfoP->vLine.segmentsP    = vLineP->segmentsP;
	lineInfoP->vLine.segmentCount = vLineP->segmentCount;
        CopySegments (lineInfoP);
	resupplySegmentDefaults = recomputeCellInfo = redraw = 1;
    }

    if (vLineP->mask & VListLineBarsMask) {
	XtFree ((char *)lineInfoP->vLine.barsP);
	lineInfoP->vLine.barsP        = vLineP->barsP;
	lineInfoP->vLine.barCount     = vLineP->barCount;
	CopyBars (lineInfoP);
	resupplyBarDefaults = redraw = 1;
    }

    if ((vLineP->mask & VListLineFontidxMask) && (vLineP->fontidx != lineInfoP->vLine.fontidx)) {
	lineInfoP->vLine.fontidx = vLineP->fontidx;
	resupplySegmentDefaults = recomputeCellInfo = redraw = 1;
    }

    if ((vLineP->mask & VListLineColoridxMask) && (vLineP->coloridx != lineInfoP->vLine.coloridx)) {
	lineInfoP->vLine.coloridx = vLineP->coloridx;
	resupplySegmentDefaults = resupplyBarDefaults = redraw = 1;
    }

    if ((vLineP->mask & VListLineBackgroundMask) && (vLineP->background != lineInfoP->vLine.background)) {
	lineInfoP->vLine.background = vLineP->background;
	resupplySegmentDefaults = resupplyBarDefaults = redraw = 1;
    }

    if (vLineP->mask & VListLineSelectMask) {
	if (w->selectUnit == VListSelectUnitField) {
	    selectLine      = line;
	    selectLineInfoP = lineInfoP;
	} else {
	    selectLine      = LineToFirstLineInRecord (w, line);
            selectLineInfoP = LineToLineInfo (w, selectLine);
	}

        if (vLineP->select != selectLineInfoP->vLine.select) {
	    if (vLineP->select && (w->selectStyle == VListSelectStyleSingle)) DeselectAllLines (w);
    
	    if ((selectLineInfoP->vLine.select = vLineP->select)) w->selectionCnt++;
	    else 					          w->selectionCnt--;
    
	    flipSelect = 1;
        }
    }

    if ((vLineP->mask & VListLineSensitiveMask) && (vLineP->sensitive != lineInfoP->vLine.sensitive))
	lineInfoP->vLine.sensitive = vLineP->sensitive;

    if ((vLineP->mask & VListLineStippledMask) && (vLineP->stippled != lineInfoP->vLine.stippled)) {
	lineInfoP->vLine.stippled = vLineP->stippled;
	resupplySegmentDefaults = resupplyBarDefaults = redraw = 1;
    }

    if ((vLineP->mask & VListLineLowValueMask) && (vLineP->lowValue != lineInfoP->vLine.lowValue)) {
	lineInfoP->vLine.lowValue = vLineP->lowValue;
	resupplyBarDefaults = redraw = 1;
    }

    if ((vLineP->mask & VListLineHighValueMask) && (vLineP->highValue != lineInfoP->vLine.highValue)) {
	lineInfoP->vLine.highValue = vLineP->highValue;
	resupplyBarDefaults = redraw = 1;
    }

    if (vLineP->mask & VListLineIconMask) {
	lineInfoP->vLine.icon = vLineP->icon;
	recomputeCellInfo = redraw = 1;
    }

    if (resupplySegmentDefaults) SupplySegmentDefaults (w, lineInfoP);
    if (resupplyBarDefaults)     SupplyBarDefaults     (w, &lineInfoP->vLine);

    if (recomputeCellInfo) {
	oldBodyWidth  = lineInfoP->bodyWidth;
	oldBodyHeight = lineInfoP->bodyHeight;

	ComputeCellInfo (w, vFieldP, lineInfoP);

	if (w->layoutIsValid && (oldBodyWidth != lineInfoP->bodyWidth)) {
	    LineToColumnTier (w, line, &column, &tier);
	    if (ChangeColumnWidthContribution (w, column,
		oldBodyWidth 	     + 2 * vFieldP->marginWidth,
		lineInfoP->bodyWidth + 2 * vFieldP->marginWidth, vFieldP)) StartUpdateSweep (w);
	}

	if (w->layoutIsValid && (oldBodyHeight != lineInfoP->bodyHeight))
	    if (ChangeCellHeightContribution (w,
		oldBodyHeight 	      + 2 * vFieldP->marginHeight,
		lineInfoP->bodyHeight + 2 * vFieldP->marginHeight)) 		StartUpdateSweep (w);
    }

    if (!w->layoutIsValid) return;

    if (redraw) 	 Draw (w, line, 1, 1);
    else if (flipSelect) Flip (w, line);

    if (flipSelect && (w->selectUnit == VListSelectUnitRow)) {
	int lastLineP1 = selectLine + w->fieldCnt;

	for (; selectLine < lastLineP1; selectLine++) if (selectLine != line) Flip (w, selectLine);
    }
}

void VListChangeLine(Widget ow, Opaque closure, VListLine *vLineP)
{
    VListWidget w = (VListWidget)ow;
    int line;

    if ((line = ClosureToLine (w, closure)) == (-1)) return;

    ChangeLine (w, line, vLineP);
}

void VListChangeCell(Widget ow, int field, int row, VListLine *vLineP)
{
    VListWidget w = (VListWidget)ow;
    int line;

    if ((line = FieldRowToLine (w, field, row)) == (-1)) return;

    ChangeLine (w, line, vLineP);
}


void VListChangeLineBars(Widget ow, Opaque closure, VListBar *newBarsP, int newBarCount)
{
    VListLine vLine;

    vLine.mask = VListLineBarsMask;
    vLine.barsP = newBarsP;
    vLine.barCount = newBarCount;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineClosure(Widget ow, Opaque closure, Opaque newClosure)
{
    VListLine vLine;

    vLine.mask = VListLineClosureMask;
    vLine.closure = newClosure;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineText(Widget ow, Opaque closure, char *newTextP)
{
    VListLine vLine;

    vLine.mask = VListLineTextMask;
    vLine.textP = newTextP;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineFontidx(Widget ow, Opaque closure, int newFontidx)
{
    VListLine vLine;

    vLine.mask = VListLineFontidxMask;
    vLine.fontidx = newFontidx;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineColoridx(Widget ow, Opaque closure, int newColoridx)
{
    VListLine vLine;

    vLine.mask = VListLineColoridxMask;
    vLine.coloridx = newColoridx;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineSelect(Widget ow, Opaque closure, int newSelect)
{
    VListLine vLine;

    vLine.mask = VListLineSelectMask;
    vLine.select = newSelect;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineSensitive(Widget ow, Opaque closure, int newSensitive)
{
    VListLine vLine;

    vLine.mask = VListLineSensitiveMask;
    vLine.sensitive = newSensitive;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineSegments(Widget ow, Opaque closure, VListSegment *newSegmentsP, int newSegmentCount)
{
    VListLine vLine;

    vLine.mask = VListLineSegmentsMask;
    vLine.segmentsP = newSegmentsP;
    vLine.segmentCount = newSegmentCount;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineStippled(Widget ow, Opaque closure, int newStippled)
{
    VListLine vLine;

    vLine.mask = VListLineStippledMask;
    vLine.stippled = newStippled;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineValues(Widget ow, Opaque closure, int newLowValue, int newHighValue)
{
    VListLine vLine;

    vLine.mask = VListLineLowValueMask | VListLineHighValueMask;
    vLine.lowValue = newLowValue;
    vLine.highValue = newHighValue;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineIcon(Widget ow, Opaque closure, Pixmap newIcon)
{
    VListLine vLine;

    vLine.mask = VListLineIconMask;
    vLine.icon = newIcon;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeLineBackground(Widget ow, Opaque closure, int newBackground)
{
    VListLine vLine;

    vLine.mask = VListLineBackgroundMask;
    vLine.background = newBackground;
    VListChangeLine (ow, closure, &vLine);
}

void VListChangeCellBars(Widget ow, int field, int row, VListBar *newBarsP, int newBarCount)
{
    VListLine vLine;

    vLine.mask = VListLineBarsMask;
    vLine.barsP = newBarsP;
    vLine.barCount = newBarCount;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellClosure(Widget ow, int field, int row, Opaque newClosure)
{
    VListLine vLine;

    vLine.mask = VListLineClosureMask;
    vLine.closure = newClosure;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellText(Widget ow, int field, int row, char *newTextP)
{
    VListLine vLine;

    vLine.mask = VListLineTextMask;
    vLine.textP = newTextP;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellFontidx(Widget ow, int field, int row, int newFontidx)
{
    VListLine vLine;

    vLine.mask = VListLineFontidxMask;
    vLine.fontidx = newFontidx;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellColoridx(Widget ow, int field, int row, int newColoridx)
{
    VListLine vLine;

    vLine.mask = VListLineColoridxMask;
    vLine.coloridx = newColoridx;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellSelect(Widget ow, int field, int row, int newSelect)
{
    VListLine vLine;

    vLine.mask = VListLineSelectMask;
    vLine.select = newSelect;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellSensitive(Widget ow, int field, int row, int newSensitive)
{
    VListLine vLine;

    vLine.mask = VListLineSensitiveMask;
    vLine.sensitive = newSensitive;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellSegments(Widget ow, int field, int row, VListSegment *newSegmentsP, int newSegmentCount)
{
    VListLine vLine;

    vLine.mask = VListLineSegmentsMask;
    vLine.segmentsP = newSegmentsP;
    vLine.segmentCount = newSegmentCount;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellStippled(Widget ow, int field, int row, int newStippled)
{
    VListLine vLine;

    vLine.mask = VListLineStippledMask;
    vLine.stippled = newStippled;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellValues(Widget ow, int field, int row, int newLowValue, int newHighValue)
{
    VListLine vLine;

    vLine.mask = VListLineLowValueMask | VListLineHighValueMask;
    vLine.lowValue = newLowValue;
    vLine.highValue = newHighValue;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellIcon(Widget ow, int field, int row, Pixmap newIcon)
{
    VListLine vLine;

    vLine.mask = VListLineIconMask;
    vLine.icon = newIcon;
    VListChangeCell (ow, field, row, &vLine);
}

void VListChangeCellBackground(Widget ow, int field, int row, int newBackground)
{
    VListLine vLine;

    vLine.mask = VListLineBackgroundMask;
    vLine.background = newBackground;
    VListChangeCell (ow, field, row, &vLine);
}

void VListMoveLine(Widget ow, Opaque closure, int up)
{
    VListWidget w = (VListWidget)ow;
    LineInfo lineInfo, *lineInfoP, *otherLineInfoP;
    int field, firstLine, otherFirstLine, line;
    VListField *vFieldP;

    if ((line = ClosureToLine (w, closure)) == (-1)) return;

    firstLine = LineToFirstLineInRecord (w, line);

    if (up) otherFirstLine = firstLine - w->fieldCnt;
    else    otherFirstLine = firstLine + w->fieldCnt;

    if (otherFirstLine < 0)           return;	/* already at top */
    if (otherFirstLine >= w->lineCnt) return;	/* already at bottom */

    otherLineInfoP = LineToLineInfo (w, otherFirstLine);
    FOR_EACH_LINE_IN_RECORD (w, line, lineInfoP, field, vFieldP, firstLine) {
        lineInfo        = (*lineInfoP);		/* copy to temp */
        *lineInfoP      = (*otherLineInfoP);	/* copy other */
        *otherLineInfoP = lineInfo;		/* and restore */

	otherLineInfoP++;
    }

    /* If the row is changing groups, invalidate the layout. */

    if (w->layoutIsValid && (firstLine / w->fieldCnt / w->tierCnt) != (otherFirstLine / w->fieldCnt / w->tierCnt))
	StartUpdateSweep (w);

    if (!w->layoutIsValid) return;
	
    for (field = 0, line = firstLine;      field < w->fieldCnt; field++, line++) Draw (w, line, 1, 1);
    for (field = 0, line = otherFirstLine; field < w->fieldCnt; field++, line++) Draw (w, line, 1, 1);
}	

static void DeleteRecord(VListWidget w, int firstLine)
{
    int line, firstVY, lastVYP1, row, fromY, toY, height;
    int field;
    LineInfo *lineInfoP, *otherLineInfoP;
    VListField *vFieldP;

    if (w->layoutIsValid && (w->groupCnt != 1)) StartUpdateSweep (w);

    FOR_EACH_LINE_IN_RECORD (w, line, lineInfoP, field, vFieldP, firstLine) {
	if (lineInfoP->vLine.select) w->selectionCnt--;

	XtFree ((char *)lineInfoP->vLine.textP);
	XtFree ((char *)lineInfoP->segmentInfosP);
	XtFree ((char *)lineInfoP->vLine.barsP);

	if (w->layoutIsValid) 
	    if (RemoveColumnWidthContribution (w, field, lineInfoP->bodyWidth  + 2 * vFieldP->marginWidth, vFieldP))
		StartUpdateSweep (w);

	if (w->layoutIsValid)
	    if (RemoveCellHeightContribution (w,         lineInfoP->bodyHeight + 2 * vFieldP->marginHeight))
		StartUpdateSweep (w);
    }	

    /* Collapse the row from the array. */

    w->lineCnt -= w->fieldCnt;
    w->rowCnt--;

    for (line = firstLine, lineInfoP = LineToLineInfo (w, line), otherLineInfoP = LineToLineInfo (w, line + w->fieldCnt);
      line < w->lineCnt; line++, lineInfoP++, otherLineInfoP++) *lineInfoP = (*otherLineInfoP);

    if (!w->layoutIsValid) return;

    row      = LineToRow (w, firstLine);
    firstVY  = w->marginHeight + row * w->tierHeight;
    lastVYP1 = firstVY + w->tierHeight;

    w->tierCnt = w->rowCnt;
    ComputeLogicalHeight (w);
    ArrangeWidgets (w);

    /* Removed row is above top of window? */

    if (w->internalYAdjust >= lastVYP1) {
	w->internalYAdjust  -= w->tierHeight;
	w->requestedYAdjust -= w->tierHeight;
	w->externalYAdjust  -= w->tierHeight;

	SetInternalXYAdjust (w, w->internalXAdjust, w->internalYAdjust);	/***** may not be necessary */
	StartPendingScroll (w);							/***** may not be necessary */
	return;
    }

    /* Removed row is below bottom of window? */

    if ((w->internalYAdjust + w->core.height) <= firstVY) return;

    /* Removed row is visible.  Fix window contents. */

    toY    = firstVY - w->requestedYAdjust;
    fromY  = toY + w->tierHeight;
    height = w->core.height - fromY;

    /* If anything visible follows the row being removed, scroll it up to fill the gap. */

    if (height > 0) {
        SetStippled (w, 0, 0, 0);
        XCopyArea (VtkDisplay (w), VtkWindow (w), VtkWindow (w), w->normgc, 0, fromY, w->core.width, height, 0, toY);
	w->activeGropCnt++;
    }

    /* Clear the gap left at the end of the window and redraw lines in that area. */

    XClearArea (VtkDisplay (w), VtkWindow (w), 0, w->core.height - w->tierHeight, w->core.width, w->tierHeight, 0);

    DrawExposeRegion (w, 0, firstVY + height, w->logicalWidth, w->tierHeight);

    /* Possible scroll to eliminate trailing deadspace. */

    SetInternalXYAdjust (w, w->internalXAdjust, w->internalYAdjust);
    StartPendingScroll (w);
}

void VListRemoveLine(Widget ow, Opaque closure)
{
    VListWidget w = (VListWidget)ow;
    int line;

    if ((line = ClosureToLine (w, closure)) == (-1)) return;

    DeleteRecord (w, LineToFirstLineInRecord (w, line));
}

void VListRemoveRow(Widget ow, int row)
{
    VListWidget w = (VListWidget)ow;
    int firstLine;

    if ((firstLine = RowToFirstLine (w, row)) == (-1)) return;

    DeleteRecord (w, firstLine);
}

void VListRemoveAllLines(Widget ow)
{
    VListWidget w = (VListWidget)ow;

    FreeLines (w);

    w->anyAreExposed = w->lineCnt = w->rowCnt = w->selectionCnt = w->layoutIsValid = 0;

    SetInternalXYAdjust (w, 0, 0);
    SetPosition (w);
    StartUpdateSweep (w);
}

int VListComputePreferredWidth(Widget ow)
{
    VListWidget w = (VListWidget)ow;
    int width, height;

    /* If we are not in single column style, or we always want the
     * horizontal scrollbar anyway, just return the current size. */

    if ((w->columnStyle != VListColumnStyleSingle) || (w->hScrollPolicy == VListScrollBarPolicyAlways))
	return (w->core.parent->core.width);

    ComputeCellHeight (w, 1);
    DoColumnLayout (w);

    width = w->logicalWidth;
    height = w->core.parent->core.height;

    if (w->headerHeight) height -= (w->headerHeight + w->core.border_width);

    if (w->vScrollW && ((w->vScrollPolicy == VListScrollBarPolicyAlways)
      || ((w->vScrollPolicy == VListScrollBarPolicyAsNeeded) && (w->logicalHeight > height))))
	width += (w->vScrollW->core.width + w->vScrollW->core.border_width);

    return (width);
}

int VListLineToCell(Widget ow, Opaque closure, int *xP, int *yP, int *widthP, int *heightP)
{
    VListWidget w = (VListWidget)ow;
    int line, column, tier;

    /* Given the cell identified by closure, return the x, y, width and height of that cell. */

    if (!w->layoutIsValid) return (0);						/* can't compute it yet */
    if ((line = ClosureToLine (w, closure)) == (-1)) return (0); 		/* no such line */

    LineToColumnTier (w, line, &column, &tier);
    ColumnTierToCell (w, column, tier, xP, yP, widthP, heightP);

    return (1);
}
