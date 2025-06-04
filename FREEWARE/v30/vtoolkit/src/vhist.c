/* vhist.c - VHist widget implementation module

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module implements the VHist widget.

MODIFICATION HISTORY:

03-Dec-1993 (sjk) Even more portability work.  Thanks Roy!

11-Oct-1993 (sjk) More portability cleanup.  Thanks Bill!

16-Sep-1993 (sjk) Bugfix typedef for ALPHA/OSF.  Thanks Jean-Paul!

14-Sep-1993 (sjk) Reapply bugfix from 17-Apr-1992 - typo in ComputeLeftRightLabelPosition, alignment VHistAlignmentRight option.

03-Sep-1993 (sjk/nfc) Run through the Motif porting tools, cleaned up, functions changed from old
	declaration style to prototype style, #if... MOTIF conditional removed.

19-Mar-1992 (sjk) Bugfix - don't call CopyFonts in DoRealize.  Do call it in DoSetValues.  Conditionalize #pragma
	for VMS only.

06-Nov-1991 (sjk) Code cleanup for new baselevel.  Change default margin resource values to zero.
	Add VHistNunitType resource.

22-Oct-1991 (tmr) Modified to work with Motif font units and changed name of initialization routine to VHistInitializeForMRM
	when MOTIF is defined.

15-Oct-1991 (tmr) Fixed SetValues for left/right/top/bottom Margin Width(s) and Height(s) resources.

01-Jun-1990 (sjk) Add support for colored bars, colored grid lines,  VHistNuserData resource, per-bar and per-grid closure
	(user-data).  Pass a copy of the VHistBar entry to callbacks (in the callback struct).  Add latent support for
	stippled bars.  Significant code cleanup.  Incompatible user interface change due to the now-required passing
	of a mask in both the VHistBar and VHistGrid structures.

14-May-1990 (sjk) Use portable VTK font routines, motivated by MOTIF BL1.1 port.

20-Apr-1990 (sjk) More portability work.

10-Apr-1990 (sjk) Add MOTIF support.  Improve C-compiler portability.

05-Apr-1990 (sjk) Add VHistOrientationBestFit option.

22-Feb-1990 (sjk) Fix 8-bit character width calculation.

02-Jan-1990 (sjk) Select callbacks.

12-Sep-1989 (sjk) Initial entry.
*/

#include "vtoolkit.h"
#include "vhistp.h"

#define OrientationVertical(w)		(w->actualOrientation == VHistOrientationVertical)

static void CopyLabel(char **labelInfoLabelPP, char **labelPP)
{
    (*labelPP) = (*labelInfoLabelPP) = (char *)VtkRecloneString (*labelInfoLabelPP, *labelPP);
}

static void ComputeLabelWidth(VHistWidget w, LabelInfo *labelInfoP)
{
    labelInfoP->width = VtkComputeTextWidth (w->fontP, labelInfoP->labelP);
}

static void ComputeAllGridLabelWidths(VHistWidget w)
{
    int i;
    GridInfo *gridInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++) {
	ComputeLabelWidth (w, &gridInfoP->lowLabelInfo);
	ComputeLabelWidth (w, &gridInfoP->highLabelInfo);
    }
}

static void ComputeAllBarLabelWidths(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	ComputeLabelWidth (w, &barInfoP->lowLabelInfo);
	ComputeLabelWidth (w, &barInfoP->highLabelInfo);
    }
}

static Pixel ColoridxToColor(VHistWidget w, int coloridx)
{
    if (!w->colorList) return ((Pixel)coloridx);
    else 	       return (w->colorTableP[((unsigned int)coloridx % w->colorCnt)]);
}

static int ValueToY(VHistWidget w, int v)
{
    int vRange = w->maxValue - w->minValue;

    return (w->insideBottomY - (((v - w->minValue) * (w->insideBottomY - w->insideTopY) + (vRange - 1)) / vRange));
}

static int ValuesToYs(VHistWidget w, int lowValue, int highValue, int *bottomYP, int *topYP)
{
    VtkMinMax (lowValue, highValue, &lowValue, &highValue);

    if ((highValue < w->minValue) || (lowValue > w->maxValue)) {
	*bottomYP = (-1);
	*topYP    = (-1);
	return (0);
    }

    if (highValue > w->maxValue) highValue = w->maxValue;
    if (lowValue  < w->minValue) lowValue  = w->minValue;

    *bottomYP = ValueToY (w, lowValue);
    *topYP    = ValueToY (w, highValue);

    return (1);
}

static int ValueToX(VHistWidget w, int v)
{
    int vRange = w->maxValue - w->minValue;

    return (w->insideLeftX + (((v - w->minValue) * (w->insideRightX - w->insideLeftX) + (vRange - 1)) / vRange));
}

static int ValuesToXs(VHistWidget w, int lowValue, int highValue, int *leftXP, int *rightXP)
{
    VtkMinMax (lowValue, highValue, &lowValue, &highValue);

    if ((highValue < w->minValue) || (lowValue > w->maxValue)) {
	*leftXP  = (-1);
	*rightXP = (-1);
	return (0);
    }

    if (highValue > w->maxValue) highValue = w->maxValue;
    if (lowValue  < w->minValue) lowValue  = w->minValue;

    *leftXP  = ValueToX (w, lowValue);
    *rightXP = ValueToX (w, highValue);

    return (1);
}

#define ConvertWidth(f, t)  w->f = XmConvertUnits ((struct _WidgetRec*)w, XmHORIZONTAL, w->unitType, w->t, XmPIXELS)
#define ConvertHeight(f, t) w->f = XmConvertUnits ((struct _WidgetRec*)w, XmVERTICAL,   w->unitType, w->t, XmPIXELS)

static void ConvertFromFontUnits(VHistWidget w)
{
    ConvertWidth (leftMarginWidth, 		leftMarginWidthU);
    ConvertWidth (rightMarginWidth, 		rightMarginWidthU);
    ConvertWidth (leftAxisMarginWidth, 		leftAxisMarginWidthU);
    ConvertWidth (rightAxisMarginWidth, 	rightAxisMarginWidthU);
    ConvertWidth (leftLabelWidth, 		leftLabelWidthU);
    ConvertWidth (rightLabelWidth, 		rightLabelWidthU);

    ConvertHeight (topMarginHeight, 		topMarginHeightU);
    ConvertHeight (bottomMarginHeight, 		bottomMarginHeightU);
    ConvertHeight (topAxisMarginHeight, 	topAxisMarginHeightU);
    ConvertHeight (bottomAxisMarginHeight, 	bottomAxisMarginHeightU);
    ConvertHeight (topLabelHeight, 		topLabelHeightU);
    ConvertHeight (bottomLabelHeight, 		bottomLabelHeightU);
}

static void ComputeAxisPositions(VHistWidget w)
{
    w->totalLeftMarginWidth    = w->leftMarginWidth    + w->leftLabelWidth    + w->leftAxisMarginWidth;
    w->totalRightMarginWidth   = w->rightMarginWidth   + w->rightLabelWidth   + w->rightAxisMarginWidth;
    w->totalTopMarginHeight    = w->topMarginHeight    + w->topLabelHeight    + w->topAxisMarginHeight;
    w->totalBottomMarginHeight = w->bottomMarginHeight + w->bottomLabelHeight + w->bottomAxisMarginHeight;

    w->insideLeftX = w->totalLeftMarginWidth + w->leftAxisThickness;

    w->insideRightX = w->core.width - w->totalRightMarginWidth - w->rightAxisThickness - 1;
    if (w->insideRightX <= w->insideLeftX) w->insideRightX = w->insideLeftX + 1;

    w->insideTopY = w->totalTopMarginHeight + w->topAxisThickness;

    w->insideBottomY = w->core.height - w->totalBottomMarginHeight - w->bottomAxisThickness - 1;
    if (w->insideBottomY <= w->insideTopY) w->insideBottomY = w->insideTopY + 1;

    w->leftLabelX   = w->leftMarginWidth;
    w->rightLabelX  = w->core.width - w->rightMarginWidth - w->rightLabelWidth;
    w->topLabelY    = w->topMarginHeight;
    w->bottomLabelY = w->core.height - w->bottomMarginHeight - w->charHeight;

    w->horizontalAxisLength = w->core.width
	- w->totalLeftMarginWidth  - w->leftAxisThickness
	- w->totalRightMarginWidth - w->rightAxisThickness;
    if (w->horizontalAxisLength <= 0) w->horizontalAxisLength = 1;

    w->verticalAxisLength = w->core.height
	- w->totalTopMarginHeight    - w->topAxisThickness
	- w->totalBottomMarginHeight - w->bottomAxisThickness;
    if (w->verticalAxisLength <= 0) w->verticalAxisLength = 1;
}

static int ComputeBarWidth(VHistWidget w, int axisLength)
{
    return ((axisLength * w->spacingDenominator) /
	(w->barCnt * (w->spacingNumerator + w->spacingDenominator) + w->spacingNumerator));
}

static void ComputeVerticalBarPositions(VHistWidget w)
{
    BarInfo *barInfoP;
    int i, totalBarWidth, barSpacing, x;

    if (!w->barCnt) return;

    w->barWidth = ComputeBarWidth (w, w->horizontalAxisLength);

    totalBarWidth = w->barWidth * w->barCnt;

    barSpacing = (w->horizontalAxisLength - totalBarWidth) / (w->barCnt + 1);

    x = w->totalLeftMarginWidth + w->leftAxisThickness
	+ (w->horizontalAxisLength - totalBarWidth
	- barSpacing * (w->barCnt - 1)) / 2;

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	barInfoP->x = x;
	x += (w->barWidth + barSpacing);
    }
}

static void ComputeHorizontalBarPositions(VHistWidget w)
{
    BarInfo *barInfoP;
    int i, totalBarWidth, barSpacing, y;

    if (!w->barCnt) return;

    w->barWidth = ComputeBarWidth (w, w->verticalAxisLength);

    totalBarWidth = w->barWidth * w->barCnt;

    barSpacing = (w->verticalAxisLength - totalBarWidth) / (w->barCnt + 1);

    y = w->totalTopMarginHeight + w->topAxisThickness
	+ (w->verticalAxisLength - totalBarWidth
	- barSpacing * (w->barCnt - 1)) / 2;

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	barInfoP->y = y;
	y += (w->barWidth + barSpacing);
    }
}

static void ComputeLeftRightLabelPosition(VHistWidget w, int alignment, int x, int y, int width, 
			int thickness, LabelInfo *labelInfoP)
{
    if (!labelInfoP->width) return;

    if (alignment == VHistAlignmentLeft)       labelInfoP->x = x;
    else if (alignment == VHistAlignmentRight) labelInfoP->x = x + width - labelInfoP->width;
    else 				       labelInfoP->x = x + (width - labelInfoP->width) / 2;

    labelInfoP->y = y + (thickness - w->charHeight) / 2;
}

static void ComputeLeftGridLabelPosition(VHistWidget w, GridInfo *gridInfoP)
{
    ComputeLeftRightLabelPosition (w, w->leftLabelAlignment, w->leftLabelX,
	gridInfoP->y, w->leftLabelWidth, 1, &gridInfoP->lowLabelInfo);
}

static void ComputeRightGridLabelPosition(VHistWidget w, GridInfo *gridInfoP)
{
    ComputeLeftRightLabelPosition (w, w->rightLabelAlignment, w->rightLabelX,
	gridInfoP->y, w->rightLabelWidth, 1, &gridInfoP->highLabelInfo);
}

static void ComputeLeftBarLabelPosition(VHistWidget w, BarInfo *barInfoP)
{
    ComputeLeftRightLabelPosition (w, w->leftLabelAlignment, w->leftLabelX,
	barInfoP->y, w->leftLabelWidth, w->barWidth, &barInfoP->lowLabelInfo);
}

static void ComputeRightBarLabelPosition(VHistWidget w, BarInfo *barInfoP)
{
    ComputeLeftRightLabelPosition (w, w->rightLabelAlignment, w->rightLabelX,
	barInfoP->y, w->rightLabelWidth, w->barWidth, &barInfoP->highLabelInfo);
}

static void ComputeTopBottomLabelPosition(VHistWidget w, int x, int y, int thickness, LabelInfo *labelInfoP)
{
    if (!labelInfoP->width) return;

    labelInfoP->x = x + (thickness - labelInfoP->width) / 2;
    labelInfoP->y = y;
}

static void ComputeTopGridLabelPosition(VHistWidget w, GridInfo *gridInfoP)
{
    ComputeTopBottomLabelPosition (w, gridInfoP->x, w->topLabelY, 1, &gridInfoP->lowLabelInfo);
}

static void ComputeBottomGridLabelPosition(VHistWidget w, GridInfo *gridInfoP)
{
    ComputeTopBottomLabelPosition (w, gridInfoP->x, w->bottomLabelY, 1, &gridInfoP->highLabelInfo);
}

static void ComputeTopBarLabelPosition(VHistWidget w, BarInfo *barInfoP)
{
    ComputeTopBottomLabelPosition (w, barInfoP->x, w->topLabelY, w->barWidth, &barInfoP->highLabelInfo);
}

static void ComputeBottomBarLabelPosition(VHistWidget w, BarInfo *barInfoP)
{
    ComputeTopBottomLabelPosition (w, barInfoP->x, w->bottomLabelY, w->barWidth, &barInfoP->lowLabelInfo);
}

static void ComputeActualOrientation(VHistWidget w)
{
    if (w->orientation != VHistOrientationBestFit)	      w->actualOrientation = w->orientation;
    else if (w->verticalAxisLength > w->horizontalAxisLength) w->actualOrientation = VHistOrientationVertical;
    else 						      w->actualOrientation = VHistOrientationHorizontal;
}

static void ComputeHorizontalGridPosition(VHistWidget w, GridInfo *gridInfoP)
{
    gridInfoP->y = ValueToY (w, gridInfoP->vGrid.value);
}

static void ComputeVerticalGridPosition(VHistWidget w, GridInfo *gridInfoP)
{
    gridInfoP->x = ValueToX (w, gridInfoP->vGrid.value);
}

static void ComputeAllVerticalPositions(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;
    GridInfo *gridInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++) {
	ComputeHorizontalGridPosition (w, gridInfoP);
	ComputeLeftGridLabelPosition  (w, gridInfoP);
	ComputeRightGridLabelPosition (w, gridInfoP);
    }

    ComputeVerticalBarPositions (w);

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	ComputeTopBarLabelPosition    (w, barInfoP);
 	ComputeBottomBarLabelPosition (w, barInfoP);
    }
}

static void ComputeAllHorizontalPositions(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;
    GridInfo *gridInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++) {
	ComputeVerticalGridPosition    (w, gridInfoP);
	ComputeTopGridLabelPosition    (w, gridInfoP);
	ComputeBottomGridLabelPosition (w, gridInfoP);
    }

    ComputeHorizontalBarPositions (w);

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	ComputeLeftBarLabelPosition  (w, barInfoP);
	ComputeRightBarLabelPosition (w, barInfoP);
    }
}

static void DrawAxes(VHistWidget w)
{
    if (w->leftAxisThickness)
        XFillRectangle (XtDisplay (w), XtWindow (w), w->normGC, w->totalLeftMarginWidth, w->totalTopMarginHeight,
	    w->leftAxisThickness, w->topAxisThickness + w->verticalAxisLength + w->bottomAxisThickness);

    if (w->rightAxisThickness)
        XFillRectangle (XtDisplay (w), XtWindow (w), w->normGC, w->insideRightX + 1, w->totalTopMarginHeight,
	    w->rightAxisThickness, w->topAxisThickness + w->verticalAxisLength + w->bottomAxisThickness);

    if (w->topAxisThickness)
        XFillRectangle (XtDisplay (w), XtWindow (w), w->normGC, w->totalLeftMarginWidth, w->totalTopMarginHeight,
	    w->leftAxisThickness + w->horizontalAxisLength + w->rightAxisThickness, w->topAxisThickness);

    if (w->bottomAxisThickness)
        XFillRectangle (XtDisplay (w), XtWindow (w), w->normGC, w->totalLeftMarginWidth, w->insideBottomY + 1,
	    w->leftAxisThickness + w->horizontalAxisLength + w->rightAxisThickness, w->bottomAxisThickness);
}

static void DrawLabel(VHistWidget w, LabelInfo *labelInfoP)
{
    if (!labelInfoP->width) return;

    XDrawString (XtDisplay (w), XtWindow (w), w->normGC, labelInfoP->x, labelInfoP->y + w->yFontAdjust, labelInfoP->labelP,
	strlen (labelInfoP->labelP));
}

static void UndrawLabel(VHistWidget w, LabelInfo *labelInfoP)
{
    if (!labelInfoP->width) return;

    XClearArea (XtDisplay (w), XtWindow (w), labelInfoP->x, labelInfoP->y, labelInfoP->width, w->charHeight, 0);
}

static void DrawAllLabels(VHistWidget w)
{
    int i;
    GridInfo *gridInfoP;
    BarInfo *barInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++) {
	DrawLabel (w, &gridInfoP->lowLabelInfo);
	DrawLabel (w, &gridInfoP->highLabelInfo);
    }

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	DrawLabel (w, &barInfoP->lowLabelInfo);
	DrawLabel (w, &barInfoP->highLabelInfo);
    }
}

static int HorizontalGridIsVisible(VHistWidget w, GridInfo *gridInfoP, int topY, int bottomY)
{
    if (!gridInfoP->vGrid.dashOn)  			   return (0);
    if ((gridInfoP->y < topY) || (gridInfoP->y > bottomY)) return (0);
    return (1);
}

static int VerticalGridIsVisible(VHistWidget w, GridInfo *gridInfoP, int leftX, int rightX)
{
    if (!gridInfoP->vGrid.dashOn) 			   return (0);
    if ((gridInfoP->x < leftX) || (gridInfoP->x > rightX)) return (0);
    return (1);
}

static int GridIsVisible(VHistWidget w, GridInfo *gridInfoP)
{
    if (OrientationVertical (w)) return (HorizontalGridIsVisible (w, gridInfoP, w->insideLeftX, w->insideRightX));
    else 			 return (VerticalGridIsVisible   (w, gridInfoP, w->insideTopY,  w->insideBottomY));
}

static void SetGridGC(VHistWidget w, GridInfo *gridInfoP, int dashOffset)
{
    int dashOn = gridInfoP->vGrid.dashOn;
    int dashOff = gridInfoP->vGrid.dashOff;
    Pixel color = ColoridxToColor (w, gridInfoP->vGrid.coloridx);

    char dashList[2];

    dashOffset %= (dashOn + dashOff);

    if ((dashOn == w->gridGCDashOn) && (dashOff == w->gridGCDashOff) && (dashOffset == w->gridGCDashOffset) &&
      (color == w->gridGCColor)) return;

    dashList[0] = dashOn;
    dashList[1] = dashOff;

    XSetDashes (XtDisplay (w), w->gridGC, dashOffset, dashList, 2);

    XSetForeground (XtDisplay (w), w->gridGC, color);

    w->gridGCDashOn     = dashOn;
    w->gridGCDashOff    = dashOff;
    w->gridGCDashOffset = dashOffset;
    w->gridGCColor      = color;
}

static void DrawHorizontalGrid(VHistWidget w, GridInfo *gridInfoP, int leftX, int rightX, int topY, int bottomY)
{
    if (!HorizontalGridIsVisible (w, gridInfoP, topY, bottomY)) return;

    SetGridGC (w, gridInfoP, leftX - w->insideLeftX);

    XDrawLine (XtDisplay (w), XtWindow (w), w->gridGC, leftX, gridInfoP->y, rightX, gridInfoP->y);
}

static void DrawAllHorizontalGrids(VHistWidget w)
{
    int i;
    GridInfo *gridInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++)
	DrawHorizontalGrid (w, gridInfoP, w->insideLeftX, w->insideRightX, w->insideTopY, w->insideBottomY);
}

static void DrawVerticalGrid(VHistWidget w, GridInfo *gridInfoP, int leftX, int rightX, int topY, int bottomY)
{
    if (!VerticalGridIsVisible (w, gridInfoP, leftX, rightX)) return;

    SetGridGC (w, gridInfoP, topY - w->insideTopY);

    XDrawLine (XtDisplay (w), XtWindow (w), w->gridGC, gridInfoP->x, topY, gridInfoP->x, bottomY);
}

static void DrawAllVerticalGrids(VHistWidget w)
{
    int i;
    GridInfo *gridInfoP;

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++)
	DrawVerticalGrid (w, gridInfoP, w->insideLeftX, w->insideRightX, w->insideTopY, w->insideBottomY);
}

static void SetBarGC(VHistWidget w, BarInfo *barInfoP)
{
    Pixel color = ColoridxToColor (w, barInfoP->vBar.coloridx);

    if (color == w->barGCColor) return;

    XSetForeground (XtDisplay (w), w->barGC, color);

    w->barGCColor = color;
}

static void DrawVerticalBar(VHistWidget w, BarInfo *barInfoP, int bottomY, int topY)
{
    SetBarGC (w, barInfoP);

    XFillRectangle (XtDisplay (w), XtWindow (w), w->barGC, barInfoP->x, topY, w->barWidth, bottomY - topY + 1);
}

static void DrawAllVerticalBars(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++)
	if (ValuesToYs (w, barInfoP->vBar.lowValue, barInfoP->vBar.highValue, &barInfoP->bottomY, &barInfoP->topY))
	    DrawVerticalBar (w, barInfoP, barInfoP->bottomY, barInfoP->topY);
}

static void DrawHorizontalBar(VHistWidget w, BarInfo *barInfoP, int leftX, int rightX)
{
    SetBarGC (w, barInfoP);

    XFillRectangle (XtDisplay (w), XtWindow (w), w->barGC, leftX, barInfoP->y, rightX - leftX + 1, w->barWidth);
}

static void DrawAllHorizontalBars(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;

    for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++)
        if (ValuesToXs (w, barInfoP->vBar.lowValue, barInfoP->vBar.highValue, &barInfoP->leftX, &barInfoP->rightX))
	    DrawHorizontalBar (w, barInfoP, barInfoP->leftX, barInfoP->rightX);
}

static void UndrawVerticalBar(VHistWidget w, int x, int bottomY, int topY)
{
    int i;
    GridInfo *gridInfoP;

    XClearArea (XtDisplay (w), XtWindow (w), x, topY, w->barWidth, bottomY - topY + 1, 0);

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++)
	DrawHorizontalGrid (w, gridInfoP, x, x + w->barWidth - 1, topY, bottomY);
}

static void UndrawHorizontalBar(VHistWidget w, int y, int leftX, int rightX)
{
    int i;
    GridInfo *gridInfoP;

    XClearArea (XtDisplay (w), XtWindow (w), leftX, y, rightX - leftX + 1, w->barWidth, 0);

    for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++)
	DrawVerticalGrid (w, gridInfoP, leftX, rightX, y, y + w->barWidth - 1);
}

static void AdjustVerticalBar(VHistWidget w, BarInfo *barInfoP, int oldColoridx)
{
    int bottomY, topY;

    if (!ValuesToYs (w, barInfoP->vBar.lowValue, barInfoP->vBar.highValue, &bottomY, &topY)) {
	if (barInfoP->bottomY != (-1))
	    UndrawVerticalBar (w, barInfoP->x, barInfoP->bottomY, barInfoP->topY);
    } else {
	if ((barInfoP->bottomY != (-1)) && (bottomY < barInfoP->bottomY))
	    UndrawVerticalBar (w, barInfoP->x, barInfoP->bottomY, (bottomY > barInfoP->topY ? bottomY + 1 : barInfoP->topY));

	if ((barInfoP->topY != (-1)) && (topY > barInfoP->topY))
	    UndrawVerticalBar (w, barInfoP->x, (barInfoP->bottomY < topY ? barInfoP->bottomY : topY - 1), barInfoP->topY);

	if (ColoridxToColor (w, oldColoridx) != ColoridxToColor (w, barInfoP->vBar.coloridx))
	    DrawVerticalBar (w, barInfoP, bottomY, topY);
	else {
	    if (topY < barInfoP->topY)
		DrawVerticalBar (w, barInfoP, (bottomY < barInfoP->topY ? bottomY : barInfoP->topY), topY);
    
	    if (bottomY > barInfoP->bottomY)
		DrawVerticalBar (w, barInfoP, bottomY, (topY > barInfoP->bottomY ? topY : barInfoP->bottomY));
        }
    }

    barInfoP->topY    = topY;
    barInfoP->bottomY = bottomY;
}

static void AdjustHorizontalBar(VHistWidget w, BarInfo *barInfoP, int oldColoridx)
{
    int leftX, rightX;

    if (!ValuesToXs (w, barInfoP->vBar.lowValue, barInfoP->vBar.highValue, &leftX, &rightX)) {
	if (barInfoP->leftX != (-1))
	    UndrawHorizontalBar (w, barInfoP->y, barInfoP->leftX, barInfoP->rightX);
    } else {
	if ((barInfoP->leftX != (-1)) && (leftX > barInfoP->leftX))
	    UndrawHorizontalBar (w, barInfoP->y, barInfoP->leftX, (leftX < barInfoP->rightX ? leftX - 1 : barInfoP->rightX));

	if ((barInfoP->rightX != (-1)) && (rightX < barInfoP->rightX))
	    UndrawHorizontalBar (w, barInfoP->y, (barInfoP->leftX > rightX ? barInfoP->leftX : rightX + 1), barInfoP->rightX);

	if (ColoridxToColor (w, oldColoridx) != ColoridxToColor (w, barInfoP->vBar.coloridx))
	    DrawHorizontalBar (w, barInfoP, leftX, rightX);
        else {
	    if (rightX > barInfoP->rightX)
		DrawHorizontalBar (w, barInfoP, (leftX > barInfoP->rightX ? leftX : barInfoP->rightX), rightX);
    
	    if (leftX < barInfoP->leftX)
		DrawHorizontalBar (w, barInfoP, leftX, (rightX < barInfoP->leftX ? rightX : barInfoP->leftX));
        }
    }

    barInfoP->leftX  = leftX;
    barInfoP->rightX = rightX;
}

static int XYIsOverLabel(VHistWidget w, LabelInfo *labelInfoP, int x, int y)
{
    if (labelInfoP->width
      && (x >= labelInfoP->x) && (x < (labelInfoP->x + labelInfoP->width))
      && (y >= labelInfoP->y) && (y < (labelInfoP->y + w->charHeight)))
	return (1);

    return (0);
}

int VHistXYToBar(Widget ow, int x, int y)
{
    VHistWidget w = (VHistWidget)ow;
    int bar;
    BarInfo *barInfoP;

    if (!w->barCnt) return (-1);

    if (OrientationVertical (w)) {
	if ((y > w->insideTopY) && (y < w->insideBottomY))
	    for (bar = 0, barInfoP = w->barInfosP; bar < w->barCnt; bar++, barInfoP++)
	        if ((x >= barInfoP->x) && (x < (barInfoP->x + w->barWidth))) return (bar);
    } else {
	if ((x > w->insideLeftX) && (x < w->insideRightX))
	    for (bar = 0, barInfoP = w->barInfosP; bar < w->barCnt; bar++, barInfoP++)
	        if ((y >= barInfoP->y) && (y < (barInfoP->y + w->barWidth))) return (bar);
    }

    for (bar = 0, barInfoP = w->barInfosP; bar < w->barCnt; bar++, barInfoP++)
	if (XYIsOverLabel (w, &barInfoP->lowLabelInfo, x, y) || XYIsOverLabel (w, &barInfoP->highLabelInfo, x, y))
	    return (bar);

    return (-1);				/* between bars */
}

static void CallSelectCallbacks(VHistWidget w, XButtonEvent *eventP, int confirm)
{
    VHistSelectCallbackStruct cbData;

    cbData.reason = (confirm ? VHistCRSelectConfirm : VHistCRSelect);
    cbData.event  = (XEvent *)eventP;
    cbData.bar    = w->upBar;

    if (w->upBar != -1) cbData.vBar = w->barInfosP[w->upBar].vBar;
    else 		cbData.vBar.closure = 0;

    XtCallCallbacks ((Widget)w, (confirm ? VHistNselectConfirmCallback : VHistNselectCallback), &cbData);    
}

static void ButtonPressHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    VHistWidget w = (VHistWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (w->buttonIsDown) {
	w->buttonIsDown = 0;		/* chorded cancel */
	return;
    }

    /* If this is not a simple un-chorded Button1 event, ignore it. */

    if ((eventP->button != Button1) || (eventP->state & (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask)))
	return;

    w->buttonIsDown = 1;

    w->downBar = VHistXYToBar ((Widget)w, eventP->x, eventP->y);

    /* If the ButtonPress came soon enough after the last ButtonRelease, and we're still over the same bar, and the widget
     * does have at least one selectConfirm callback declared, then prime the doubleClick action. */

    if ((eventP->time <= (w->upTime + w->doubleClickDelay)) && (w->downBar == w->upBar)
      && (XtHasCallbacks ((Widget)w, VHistNselectConfirmCallback) == XtCallbackHasSome)) {
	w->doubleClickIsPending = 1;
	return;
    }

    w->doubleClickIsPending = 0;
}

static void ButtonReleaseHandler(Widget ow, XtPointer unused_closure, XEvent *eventOP, Boolean *unused_continueP)
{
    VHistWidget w = (VHistWidget)ow;
    XButtonEvent *eventP = (XButtonEvent *)eventOP;

    if (!w->buttonIsDown) return;

    w->buttonIsDown = 0;

    w->upBar = VHistXYToBar ((Widget)w, eventP->x, eventP->y);

    if (w->doubleClickIsPending) {
	w->upTime = 0;			/* don't want tripleClick problem */

	/* If the user didn't drag off bar, call SelectConfirm callbacks. */

	if (w->upBar == w->downBar) CallSelectCallbacks (w, eventP, 1);
    } else {
	w->upTime = eventP->time;	/* to test against next Down time */

	CallSelectCallbacks (w, eventP, 0);
    }
}

static void CopyColors(VHistWidget w)
{
    w->colorCnt = VtkCopyColors ((Widget) w, w->colorList, &w->colorTableP);
}

static void CopyFonts(VHistWidget w)
{
    if (!w->fontTableP) w->fontTableP = (XmFontList)VtkGetDefaultFontList ((Widget)w);

    w->fontP = (XFontStruct *)VtkGetFirstFont (w->fontTableP);
    w->charHeight  = w->fontP->max_bounds.descent + w->fontP->max_bounds.ascent;
    w->yFontAdjust = w->fontP->max_bounds.ascent;
}

static void ApplyGridDefaults(VHistWidget w, VHistGrid *gridP)
{
    if (!(gridP->mask & VHistGridValueMask)) gridP->value = w->minValue;

    if ((!(gridP->mask & VHistGridClosureMask)) || (!gridP->closure)) gridP->closure    = (Opaque)(++w->lastUsedClosure);
    if (!(gridP->mask & VHistGridLowLabelMask)) 		      gridP->lowLabelP  = "";
    if (!(gridP->mask & VHistGridHighLabelMask)) 		      gridP->highLabelP = "";

    if (!(gridP->mask & VHistGridDashMask)) {
	gridP->dashOn  = 3;
	gridP->dashOff = 2;
    }

    if (!(gridP->mask & VHistGridColoridxMask))
	if (w->colorCnt) gridP->coloridx = 0;
	else 		 gridP->coloridx = w->foreground;
}

static void ReallocGridInfos(VHistWidget w)
{
    int i;
    GridInfo *gridInfoP;

    if (w->gridCnt > w->gridInfoCnt) {
	w->gridInfosP = (GridInfo *)XtRealloc ((char *)w->gridInfosP, w->gridCnt * sizeof (GridInfo));
    
	for (i = w->gridInfoCnt, gridInfoP = (&w->gridInfosP[i]); i < w->gridCnt; i++, gridInfoP++)
	    VtkSetFieldsToZero ((char*)&gridInfoP->beginZeroInit, (char*)&gridInfoP->endZeroInit);
    
	w->gridInfoCnt = w->gridCnt;
    }
}

static void CopyGrids(VHistWidget w)
{
    int i;
    VHistGrid *gridP;
    GridInfo *gridInfoP;

    ReallocGridInfos (w);

    for (i = 0, gridP = w->gridsP, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridP++, gridInfoP++) {
	gridInfoP->vGrid = (*gridP);

	ApplyGridDefaults (w, &gridInfoP->vGrid);

	CopyLabel (&gridInfoP->lowLabelInfo.labelP,  &gridInfoP->vGrid.lowLabelP);
	CopyLabel (&gridInfoP->highLabelInfo.labelP, &gridInfoP->vGrid.highLabelP);
    }

    w->gridsP = (VHistGrid *)(-1);
}

static void ApplyBarDefaults(VHistWidget w, VHistBar *barP)
{
    if ((!(barP->mask & VHistBarClosureMask)) || (!barP->closure)) barP->closure = (Opaque)(++w->lastUsedClosure);

    if (!(barP->mask & VHistBarLowValueMask))  barP->lowValue   = w->minValue;
    if (!(barP->mask & VHistBarHighValueMask)) barP->highValue  = w->maxValue;
    if (!(barP->mask & VHistBarStippledMask))  barP->stippled   = 0;
    if (!(barP->mask & VHistBarLowLabelMask))  barP->lowLabelP  = "";
    if (!(barP->mask & VHistBarHighLabelMask)) barP->highLabelP = "";

    if (!(barP->mask & VHistBarColoridxMask))
	if (w->colorCnt) barP->coloridx = 0;
	else 		 barP->coloridx = w->barColor;
}

static void ReallocBarInfos(VHistWidget w)
{
    int i;
    BarInfo *barInfoP;

    if (w->barCnt > w->barInfoCnt) {
        w->barInfosP = (BarInfo *)XtRealloc ((char *)w->barInfosP, w->barCnt * sizeof (BarInfo));

        for (i = w->barInfoCnt, barInfoP = (&w->barInfosP[i]); i < w->barCnt; i++, barInfoP++)
            VtkSetFieldsToZero ((char*)&barInfoP->beginZeroInit, (char*)&barInfoP->endZeroInit);

        w->barInfoCnt = w->barCnt;
    }
}

static void CopyBars(VHistWidget w)
{
    int i;
    VHistBar *barP;
    BarInfo *barInfoP;

    ReallocBarInfos (w);

    for (i = 0, barP = w->barsP, barInfoP = w->barInfosP; i < w->barCnt; i++, barP++, barInfoP++) {
        barInfoP->vBar = (*barP);

	ApplyBarDefaults (w, &barInfoP->vBar);

	CopyLabel (&barInfoP->lowLabelInfo.labelP,  &barInfoP->vBar.lowLabelP);
	CopyLabel (&barInfoP->highLabelInfo.labelP, &barInfoP->vBar.highLabelP);
    }

    w->barsP = (VHistBar *)(-1);
}

static void DoInitialize(Widget unused_requestOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VHistWidget w = (VHistWidget)ow;

    VtkSetFieldsToZero ((char*)&w->beginZeroInit, (char*)&w->endZeroInit);
    w->gridGCDashOn = w->gridGCDashOff = (-1);

    CopyColors (w);
    CopyFonts (w);
    CopyBars (w);
    CopyGrids (w);
}

static void RealizeGCs(VHistWidget w)
{
    XGCValues values;

    values.function   = GXcopy;
    values.foreground = w->foreground;
    values.background = w->core.background_pixel;
    values.font       = w->fontP->fid;
    w->normGC = XCreateGC (XtDisplay (w), XtWindow (w), GCForeground | GCBackground | GCFunction | GCFont, &values);

    values.function   = GXcopy;
    values.foreground = 0;
    values.background = w->core.background_pixel;
    values.line_style = LineOnOffDash;
    w->gridGC = XCreateGC (XtDisplay (w), XtWindow (w), GCForeground | GCBackground | GCFunction | GCLineStyle, &values);

    values.function   = GXcopy;
    values.foreground = 0;
    values.background = w->core.background_pixel;
    w->barGC = XCreateGC (XtDisplay (w), XtWindow (w), GCForeground | GCBackground | GCFunction, &values);
}

static void RealizeGrabs(VHistWidget w)
{
    XGrabButton (
	XtDisplay (w),					/* display */
	1,						/* button_grab */
	AnyModifier,					/* modifiers */
	XtWindow (w),					/* window */
	0,						/* owner_events */
	(ButtonPressMask | ButtonReleaseMask),		/* event_mask */
	GrabModeAsync,					/* pointer_mode */
	GrabModeAsync,					/* keyboard_mode */
	None,						/* confine_to */
	None);						/* cursor */

    XtAddRawEventHandler ((Widget)w, ButtonPressMask,   0, ButtonPressHandler,   0);
    XtAddRawEventHandler ((Widget)w, ButtonReleaseMask, 0, ButtonReleaseHandler, 0);
}

static void DoRealize(Widget ow, XtValueMask *maskP, XSetWindowAttributes *attributesP)
{
    VHistWidget w = (VHistWidget)ow;

    if (!w->core.width)  w->core.width = 100;
    if (!w->core.height) w->core.height = 100;

    ConvertFromFontUnits (w);
    ComputeAllGridLabelWidths (w);
    ComputeAllBarLabelWidths (w);
    ComputeAxisPositions (w);
    ComputeActualOrientation (w);

    if (OrientationVertical (w)) ComputeAllVerticalPositions   (w);
    else 			 ComputeAllHorizontalPositions (w);

    XtCreateWindow ((Widget)w, InputOutput, CopyFromParent, *maskP, attributesP);

    RealizeGCs (w);
    RealizeGrabs (w);
}

static void DoExpose(Widget ow, XEvent *eventOP, Region unused_region)
{
    VHistWidget w = (VHistWidget)ow;
    XExposeEvent *eventP = (XExposeEvent *)eventOP;

    XClearArea (XtDisplay (w), XtWindow (w), eventP->x, eventP->y, eventP->width, eventP->height, 0);

    if (eventP->count) return;

    DrawAxes (w);
    DrawAllLabels (w);

    if (OrientationVertical (w)) {
	DrawAllHorizontalGrids (w);
	DrawAllVerticalBars    (w);
    } else {
	DrawAllVerticalGrids  (w);
	DrawAllHorizontalBars (w);
    }
}

static void DoResize(Widget ow)
{
    VHistWidget w = (VHistWidget)ow;

    if (!VtkIsRealized (w)) return;

    ComputeAxisPositions (w);
    ComputeActualOrientation (w);

    if (OrientationVertical (w)) ComputeAllVerticalPositions   (w);
    else 			 ComputeAllHorizontalPositions (w);
}

static int RemoveGrid(VHistWidget w, GridInfo *gridInfoP)
{
    UndrawLabel (w, &gridInfoP->lowLabelInfo);
    UndrawLabel (w, &gridInfoP->highLabelInfo);

    return (GridIsVisible (w, gridInfoP));
}

static int ChangeGrid(VHistWidget w, VHistGrid *gridP, GridInfo *gridInfoP)
{
    int oldPosition, oldVisibility, oldDashOn, oldDashOff, oldColoridx;
    int newPosition, newVisibility, labelChanged, redrawWorkarea;

    if (OrientationVertical (w)) oldPosition = gridInfoP->y;
    else 			 oldPosition = gridInfoP->x;

    oldVisibility = GridIsVisible (w, gridInfoP);

    oldDashOn   = gridInfoP->vGrid.dashOn;
    oldDashOff  = gridInfoP->vGrid.dashOff;
    oldColoridx = gridInfoP->vGrid.coloridx;

    gridInfoP->vGrid = (*gridP);
    ApplyGridDefaults (w, &gridInfoP->vGrid);

    if (OrientationVertical (w)) {
	ComputeHorizontalGridPosition (w, gridInfoP);
	newPosition = gridInfoP->y;
    } else {
	ComputeVerticalGridPosition (w, gridInfoP);
	newPosition = gridInfoP->x;
    }

    newVisibility = GridIsVisible (w, gridInfoP);

    if (oldVisibility && (!newVisibility)) redrawWorkarea = 1;
    else if ((!oldVisibility) && newVisibility) redrawWorkarea = 1;
    else if (oldVisibility && newVisibility && (
      (oldDashOn   != gridInfoP->vGrid.dashOn)   ||
      (oldDashOff  != gridInfoP->vGrid.dashOff)  ||
      (oldColoridx != gridInfoP->vGrid.coloridx) ||
      (oldPosition != newPosition))) redrawWorkarea = 1;

    labelChanged = strcmp (gridInfoP->vGrid.lowLabelP, gridInfoP->lowLabelInfo.labelP);

    if (labelChanged || (oldPosition != newPosition)) UndrawLabel (w, &gridInfoP->lowLabelInfo);

    if (labelChanged) {
	CopyLabel (&gridInfoP->lowLabelInfo.labelP, &gridInfoP->vGrid.lowLabelP);
	ComputeLabelWidth (w, &gridInfoP->lowLabelInfo);
    }

    if (labelChanged || (oldPosition != newPosition)) {
	gridInfoP->lowLabelInfo.redraw = 1;

	if (OrientationVertical (w)) ComputeLeftGridLabelPosition (w, gridInfoP);
	else 			     ComputeTopGridLabelPosition (w, gridInfoP);
    } else gridInfoP->lowLabelInfo.redraw = 0;

    labelChanged = strcmp (gridInfoP->vGrid.highLabelP, gridInfoP->highLabelInfo.labelP);

    if (labelChanged || (oldPosition != newPosition)) UndrawLabel (w, &gridInfoP->highLabelInfo);

    if (labelChanged) {
	CopyLabel (&gridInfoP->highLabelInfo.labelP, &gridInfoP->vGrid.highLabelP);
	ComputeLabelWidth (w, &gridInfoP->highLabelInfo);
    }

    if (labelChanged || (oldPosition != newPosition)) {
	gridInfoP->highLabelInfo.redraw = 1;

	if (OrientationVertical (w)) ComputeRightGridLabelPosition (w, gridInfoP);
	else 			     ComputeBottomGridLabelPosition (w, gridInfoP);
    } else gridInfoP->highLabelInfo.redraw = 0;

    return (redrawWorkarea);
}

static int AddGrid(VHistWidget w, VHistGrid *gridP, GridInfo *gridInfoP)
{
    gridInfoP->vGrid = (*gridP);
    ApplyGridDefaults (w, &gridInfoP->vGrid);

    CopyLabel (&gridInfoP->lowLabelInfo.labelP,  &gridInfoP->vGrid.lowLabelP);
    CopyLabel (&gridInfoP->highLabelInfo.labelP, &gridInfoP->vGrid.highLabelP);

    ComputeLabelWidth (w, &gridInfoP->lowLabelInfo);
    ComputeLabelWidth (w, &gridInfoP->highLabelInfo);
    
    gridInfoP->lowLabelInfo.redraw  = 1;
    gridInfoP->highLabelInfo.redraw = 1;

    if (OrientationVertical (w)) {
	ComputeHorizontalGridPosition (w, gridInfoP);
	ComputeLeftGridLabelPosition  (w, gridInfoP);
	ComputeRightGridLabelPosition (w, gridInfoP);
    } else {
	ComputeVerticalGridPosition    (w, gridInfoP);
	ComputeTopGridLabelPosition    (w, gridInfoP);
	ComputeBottomGridLabelPosition (w, gridInfoP);
    }

    return (GridIsVisible (w, gridInfoP));
}

static void AddBar(VHistWidget w, VHistBar *barP, BarInfo *barInfoP)
{
    barInfoP->vBar = (*barP);
    ApplyBarDefaults (w, &barInfoP->vBar);

    CopyLabel (&barInfoP->lowLabelInfo.labelP,  &barInfoP->vBar.lowLabelP);
    CopyLabel (&barInfoP->highLabelInfo.labelP, &barInfoP->vBar.highLabelP);

    ComputeLabelWidth (w, &barInfoP->lowLabelInfo);
    ComputeLabelWidth (w, &barInfoP->highLabelInfo);

    barInfoP->lowLabelInfo.redraw  = 1;
    barInfoP->highLabelInfo.redraw = 1;
}

static void ChangeBar(VHistWidget w, VHistBar *barP, BarInfo *barInfoP)
{
    int oldColoridx = barInfoP->vBar.coloridx;

    barInfoP->vBar = (*barP);
    ApplyBarDefaults (w, &barInfoP->vBar);

    if (strcmp (barInfoP->vBar.lowLabelP, barInfoP->lowLabelInfo.labelP)) {
	UndrawLabel (w, &barInfoP->lowLabelInfo);
	CopyLabel (&barInfoP->lowLabelInfo.labelP, &barInfoP->vBar.lowLabelP);
	ComputeLabelWidth (w, &barInfoP->lowLabelInfo);
	barInfoP->lowLabelInfo.redraw = 1;
	if (OrientationVertical (w)) ComputeBottomBarLabelPosition (w, barInfoP);
	else 			     ComputeLeftBarLabelPosition   (w, barInfoP);
    } else barInfoP->lowLabelInfo.redraw = 0;

    if (strcmp (barInfoP->vBar.highLabelP, barInfoP->highLabelInfo.labelP)) {
	UndrawLabel (w, &barInfoP->highLabelInfo);
	CopyLabel (&barInfoP->highLabelInfo.labelP, &barInfoP->vBar.highLabelP);
	ComputeLabelWidth (w, &barInfoP->highLabelInfo);
	barInfoP->highLabelInfo.redraw = 1;
	if (OrientationVertical (w)) ComputeTopBarLabelPosition   (w, barInfoP);
	else   			     ComputeRightBarLabelPosition (w, barInfoP);
    } else barInfoP->highLabelInfo.redraw = 0;

    if (OrientationVertical (w)) AdjustVerticalBar   (w, barInfoP, oldColoridx);
    else 			 AdjustHorizontalBar (w, barInfoP, oldColoridx);
}

#define Changed(f) (w->f != oldW->f)

static Boolean DoSetValues(Widget oldOW, Widget unused_refOW, Widget ow, ArgList unused_args, Cardinal *unused_num_argsP)
{
    VHistWidget oldW = (VHistWidget)oldOW;
    VHistWidget w = (VHistWidget)ow;

    int redrawWorkarea, i;
    VHistGrid *gridP;
    GridInfo *gridInfoP;
    VHistBar *barP;
    BarInfo *barInfoP;

    if (!VtkIsRealized (w)) {
	if (Changed (gridsP))     CopyGrids (w);
	if (Changed (barsP))      CopyBars (w);
	if (Changed (fontTableP)) CopyFonts (w);
	return (0);
    }

    if (Changed (orientation) 		||
      Changed (minValue)		|| Changed (maxValue)		 ||
      Changed (spacingDenominator) 	|| Changed (spacingNumerator) 	 ||
      Changed (leftMarginWidthU)	|| Changed (rightMarginWidthU)   ||
      Changed (topMarginHeightU) 	|| Changed (bottomMarginHeightU) ||
      Changed (leftLabelWidthU)		|| Changed (rightLabelWidthU) 	 ||
      Changed (topLabelHeightU)		|| Changed (bottomLabelHeightU)	 ||
      Changed (leftAxisThickness) 	|| Changed (rightAxisThickness)  ||
      Changed (topAxisThickness) 	|| Changed (bottomAxisThickness)) {
	ConvertFromFontUnits (w);
	ComputeAxisPositions (w);
	ComputeActualOrientation (w);

	if (Changed (gridsP)) {
	    CopyGrids (w);
	    ComputeAllGridLabelWidths (w);
	}

	if (Changed (barsP)) {
	    CopyBars (w);
	    ComputeAllBarLabelWidths (w);
	}

        if (OrientationVertical (w)) ComputeAllVerticalPositions   (w);
        else  			     ComputeAllHorizontalPositions (w);

	return (1);
    }

    redrawWorkarea = 0;

    if (Changed (gridsP) || Changed (gridCnt)) {
	for (i = w->gridCnt, gridInfoP = (&w->gridInfosP[i]); i < oldW->gridCnt; i++, gridInfoP++)
	    if (RemoveGrid (w, gridInfoP)) redrawWorkarea = 1;

	for (i = 0, gridP = w->gridsP, gridInfoP = w->gridInfosP; (i < w->gridCnt) && (i < oldW->gridCnt);
	  i++, gridP++, gridInfoP++)
	    if (ChangeGrid (w, gridP, gridInfoP)) redrawWorkarea = 1;

	ReallocGridInfos (w);

	for (i = oldW->gridCnt, gridP = (&w->gridsP[i]), gridInfoP = (&w->gridInfosP[i]); i < w->gridCnt; i++, gridP++, gridInfoP++)
	    if (AddGrid (w, gridP, gridInfoP)) redrawWorkarea = 1;

	for (i = 0, gridInfoP = w->gridInfosP; i < w->gridCnt; i++, gridInfoP++) {
	    if (gridInfoP->lowLabelInfo.redraw)  DrawLabel (w, &gridInfoP->lowLabelInfo);
	    if (gridInfoP->highLabelInfo.redraw) DrawLabel (w, &gridInfoP->highLabelInfo);
	}

	w->gridsP = (VHistGrid *)(-1);
    }

    if (Changed (barCnt)) {
	for (i = 0, barP = w->barsP, barInfoP = w->barInfosP; i < oldW->barCnt;
	  i++, barP++, barInfoP++) {
	    UndrawLabel (w, &barInfoP->lowLabelInfo);
	    UndrawLabel (w, &barInfoP->highLabelInfo);
	}

	ReallocBarInfos (w);

	if (OrientationVertical (w)) {
	    ComputeVerticalBarPositions (w);

	    for (i = 0, barP = w->barsP, barInfoP = w->barInfosP; i < w->barCnt; i++, barP++, barInfoP++) {
	        AddBar (w, barP, barInfoP);
	        ComputeTopBarLabelPosition    (w, barInfoP);
	        ComputeBottomBarLabelPosition (w, barInfoP);
	    }
	} else {
	    ComputeHorizontalBarPositions (w);

	    for (i = 0, barP = w->barsP, barInfoP = w->barInfosP; i < w->barCnt; i++, barP++, barInfoP++) {
	        AddBar (w, barP, barInfoP);
	        ComputeLeftBarLabelPosition  (w, barInfoP);
	        ComputeRightBarLabelPosition (w, barInfoP);
	    }
	}

	redrawWorkarea = 1;

	for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	    if (barInfoP->lowLabelInfo.redraw)  DrawLabel (w, &barInfoP->lowLabelInfo);
	    if (barInfoP->highLabelInfo.redraw) DrawLabel (w, &barInfoP->highLabelInfo);
	}

	w->barsP = (VHistBar *)(-1);
    } else if (Changed (barsP)) {
	for (i = 0, barP = w->barsP, barInfoP = w->barInfosP; i < w->barCnt; i++, barP++, barInfoP++) ChangeBar (w, barP, barInfoP);

	for (i = 0, barInfoP = w->barInfosP; i < w->barCnt; i++, barInfoP++) {
	    if (barInfoP->lowLabelInfo.redraw) 	DrawLabel (w, &barInfoP->lowLabelInfo);
	    if (barInfoP->highLabelInfo.redraw) DrawLabel (w, &barInfoP->highLabelInfo);
	}

	w->barsP = (VHistBar *)(-1);
    }

    if (redrawWorkarea) {
	XClearArea (XtDisplay (w), XtWindow (w), w->insideLeftX, w->insideTopY,
	    w->insideRightX - w->insideLeftX + 1, w->insideBottomY - w->insideTopY + 1, 0);

	if (OrientationVertical (w)) {
	    DrawAllHorizontalGrids (w);
	    DrawAllVerticalBars    (w);
	} else {
	    DrawAllVerticalGrids  (w);
	    DrawAllHorizontalBars (w);
	}
    }

    return (0);
}

static void DoDestroy(Widget unused_ow)
{
    /* to-be-supplied */
}

extern void VtkUnitTypeDefault();

#define XY(n, c, r, t, f, dr, d)        {n, c, r, sizeof(t), XtOffset(VHistWidget, f), dr, (char *)(d)},
#define XX(n, c, r, t, f, d)            XY (n, c, r, t, f, XtRImmediate, d)
#define XI(n, f, d)                     XX (n, XtCValue, XtRInt, int, f, d)

static XtResource resources[] = {

XI (VHistNbarCount,     		barCnt, 			0)
XI (VHistNbottomAxisMarginHeight,	bottomAxisMarginHeightU, 	0)
XI (VHistNbottomAxisThickness,  	bottomAxisThickness, 		2)
XI (VHistNbottomLabelHeight,     	bottomLabelHeightU, 		0)
XI (VHistNbottomMarginHeight,     	bottomMarginHeightU, 		0)
XI (VHistNdoubleClickDelay,     	doubleClickDelay, 		250)
XI (VHistNgridCount,     		gridCnt, 			0)
XI (VHistNleftAxisMarginWidth,     	leftAxisMarginWidthU, 		0)
XI (VHistNleftAxisThickness,    	leftAxisThickness, 		2)
XI (VHistNleftLabelAlignment,   	leftLabelAlignment, 		VHistAlignmentRight)
XI (VHistNleftLabelWidth,     		leftLabelWidthU, 		0)
XI (VHistNleftMarginWidth,     		leftMarginWidthU, 		0)
XI (VHistNmaxValue,     		maxValue, 			100)
XI (VHistNminValue,     		minValue, 			1)
XI (VHistNorientation,     		orientation, 			VHistOrientationVertical)
XI (VHistNrightAxisMarginWidth,     	rightAxisMarginWidthU, 		0)
XI (VHistNrightAxisThickness,   	rightAxisThickness, 		0)
XI (VHistNrightLabelAlignment,  	rightLabelAlignment, 		VHistAlignmentLeft)
XI (VHistNrightLabelWidth,     		rightLabelWidthU, 		0)
XI (VHistNrightMarginWidth,     	rightMarginWidthU, 		0)
XI (VHistNspacingDenominator,   	spacingDenominator, 		100)
XI (VHistNspacingNumerator,     	spacingNumerator, 		50)
XI (VHistNtopAxisMarginHeight,  	topAxisMarginHeightU, 		0)
XI (VHistNtopAxisThickness,     	topAxisThickness, 		0)
XI (VHistNtopLabelHeight,     		topLabelHeightU, 		0)
XI (VHistNtopMarginHeight,     		topMarginHeightU, 		0)

XY (VHistNbarColor,   		 XtCForeground, XtRPixel,     Pixel, 	      barColor,   XtRString, XtDefaultForeground)
XY (VHistNfont, 		 XmCFontList,   XmRFontList,  struct _XmFontListRec*,   fontTableP, XtRString, 0)
XY (VHistNforeground, 		 XtCForeground, XtRPixel,     Pixel,          foreground, XtRString, XtDefaultForeground)

XY (VHistNselectCallback, 	 XtCValue,      XtRCallback,  XtCallbackList, selectCallback,        XtRCallback, 0)
XY (VHistNselectConfirmCallback, XtCValue,      XtRCallback,  XtCallbackList, selectConfirmCallback, XtRCallback, 0)

XX (VHistNbars,     		 XtCValue,	XtRPointer,   VHistBar *,     barsP, 	 (VHistBar *)0)
XX (VHistNgrids,     		 XtCValue,	XtRPointer,   VHistGrid *,    gridsP,	 (VHistGrid *)0)
XX (VHistNuserData,     	 XtCValue,      XtRPointer,   XtPointer,      userData,  (XtPointer)0)

XX (VHistNcolorList,             XtCValue,      XtRPixmap,    Pixmap,         colorList, 0)

XY (VHistNunitType,		 XmCUnitType,   XmRUnitType,  int,	      unitType,   XtRCallProc, VtkUnitTypeDefault)
};

externaldef(vhistwidgetclassrec) VHistClassRec vhistwidgetclassrec = {
    {/* core_class fields	*/
	/* superclass		*/	(WidgetClass)&widgetClassRec,
	/* class_name	  	*/	"VHist",
	/* widget_size	  	*/	sizeof (VHistWidgetRec),
	/* class_initialize   	*/    	0,
	/* class_part_initialize */	0,
	/* class_inited       	*/	0,
	/* initialize	  	*/	DoInitialize,
	/* initialize_hook	*/	0,
	/* realize		*/	DoRealize,
	/* actions		*/    	0,
	/* num_actions	  	*/	0,
	/* resources	  	*/	resources,
	/* num_resources	*/	XtNumber (resources),
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
	/* set_values_almost  	*/	XtInheritSetValuesAlmost,
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

externaldef(vhistwidgetclass) WidgetClass vhistwidgetclass = (WidgetClass)&vhistwidgetclassrec;

Widget VHistCreate(Widget pW, char *nameP, Arg *argsP, int argCnt)
{
    return (XtCreateWidget (nameP, vhistwidgetclass, pW, argsP, argCnt));
}

void VHistInitializeForMRM(void)
{
    MrmRegisterClass (MrmwcUnknown, "VHist", "VHistCreate", VHistCreate, vhistwidgetclass);
}
