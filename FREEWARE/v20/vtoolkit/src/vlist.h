/* vlist.h - VList widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains C-language declarations needed to use the VList widget.
Include this file into any C module that directly uses a VList widget.

MODIFICATION HISTORY:

29-Oct-1993 (sjk) Add VListNselectionIsLocked resource.

22-Oct-1993 (sjk) Add VListNgridStyle and VListNgridPixmap resources.

17-Sep-1993 (sjk) Add prototype for VListLineToCell.

14-Sep-1993 (sjk) Add missing prototypes.

28-Aug-1993 (sjk/nfc) Run through the Motif porting tools.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS ony.

03-Jan-1991 (sjk) Add VListLine.type field.

30-Dec-1991 (sjk) Add support for icon segments.

11-Dec-1991 (sjk) Add VListNallowDeadspace resource.

19-Nov-1991 (sjk) Add VListNspaceTiersEqually resource.

15-Nov-1991 (sjk) Add VListNspaceGroupsEqually resource.

14-Nov-1991 (sjk) Add VListAnyButton option.

08-Nov-1991 (sjk) Add VListNdragButton, VListNdragWidget, VListNdragArmCallback, and VListNdragDropCallback resources.

31-Oct-1991 (sjk) Add VListAlignment symbols for two-dimensional options.  Add VListNfieldResizePolicy and VListField.resizePolicy.
	Add VListNtextMarginHeight resource.

29-Oct-1991 (sjk) Add VListSegmentNewLineMask.

28-Oct-1991 (sjk) Add VListNselectButton resource.  Add VListNhistHeight resource.  Add VListField.marginWidth and ~Height.

25-Oct-1991 (sjk) Add VListNiconMarginWidth and VListNiconMarginHeight resources.

29-Aug-1990 (sjk) Add VListLine.background field.

20-Jul-1990 (sjk) Add icon support.  Add VListNrowHeight resource.  Add VListFieldTypeIcon constant.

28-Mar-1990 (sjk) Add VListSelectStyleNone.

27-Mar-1990 (sjk) Add multi-bar histogram support.

06-Mar-1990 (sjk) Add histogram support.  Add selectionCount field to VListSelectCallbackStruct.  Remove VListLinePixelMask and
    VListSegmentPixelMask.

05-Mar-1990 (sjk) Add VListNuserData resource and VListSelectCallbackStruct.selectionCount field.

21-Feb-1990 (sjk) Add VListLinePixelMask and VListSegmentPixelMask.

31-Jan-1990 (sjk) Fix type of VListLine.segmentsP field.

26-Jan-1990 (sjk) Add VListSelectCallbackStruct.selectChangesP and lineCount.

23-Jan-1990 (sjk) Add stippled text support.

19-Jan-1990 (sjk) Add pad option to header labels.

11-Jan-1990 (sjk) Add segmented string support.

03-Jan-1990 (sjk) Add VListAlignmentLeft and ~Right, and remove column and tier fields from VListSelectCallbackStruct.

28-Dec-1989 (sjk) Version X2.0 update.

15-Nov-1988 (sjk) Initial entry.
*/

#ifndef _VList_h
#define _VList_h

/* Widget class */

external WidgetClass vlistwidgetclass;

/* Resource names */

#define VListNalignment			"alignment"
#define VListNallowDeadspace		"allowDeadspace"
#define VListNautoScrollRate		"autoScrollRate"
#define VListNcolorList			"colorList"
#define VListNcolumnStyle		"columnStyle"
#define VListNdoubleClickDelay		"doubleClickDelay"
#define VListNdragArmCallback		"dragArmCallback"
#define VListNdragButton		"dragButton"
#define VListNdragDropCallback		"dragDropCallback"
#define VListNdragWidget		"dragWidget"
#define VListNfieldCount		"fieldCount"
#define VListNfieldResizePolicy		"fieldResizePolicy"
#define VListNfieldSpacingWidth		"fieldSpacingWidth"
#define VListNfields			"fields"
#define VListNfontTable			"fontTable"
#define VListNforeground		"foreground"
#define VListNgridDashOff		"gridDashOff"
#define VListNgridDashOn		"gridDashOn"
#define VListNgridPixmap		"gridPixmap"
#define VListNgridStyle			"gridStyle"
#define VListNheaderWidget		"headerWidget"
#define VListNhistHeight		"histHeight"
#define VListNhistMarginHeight		"histMarginHeight"
#define VListNhistMarginWidth		"histMarginWidth"
#define VListNhistWidth			"histWidth"
#define VListNhScrollBarPolicy		"hScrollBarPolicy"
#define VListNhScrollBarWidget		"hScrollBarWidget"
#define VListNiconMarginHeight		"iconMarginHeight"
#define VListNiconMarginWidth		"iconMarginWidth"
#define VListNlabelCount		"labelCount"
#define VListNlabelPadOption		"labelPadOption"
#define VListNlabels			"labels"
#define VListNmarginHeight		"marginHeight"
#define VListNmarginWidth		"marginWidth"
#define VListNrowHeight			"rowHeight"
#define VListNrowSpacingHeight		"rowSpacingHeight"
#define VListNselectButton		"selectButton"
#define VListNselectCallback		"selectCallback"
#define VListNselectConfirmCallback	"selectConfirmCallback"
#define VListNselectStyle		"selectStyle"
#define VListNselectUnit		"selectUnit"
#define VListNselectionIsLocked		"selectionIsLocked"
#define VListNspaceGroupsEqually	"spaceGroupsEqually"
#define VListNspaceTiersEqually		"spaceTiersEqually"
#define VListNtextMarginHeight		"textMarginHeight"
#define VListNtextMarginWidth		"textMarginWidth"
#define VListNuserData			"userData"
#define VListNvScrollBarPolicy		"vScrollBarPolicy"
#define VListNvScrollBarPosition	"vScrollBarPosition"
#define VListNvScrollBarWidget		"vScrollBarWidget"

/* Values for VListNgridStyle resource */

#define VListGridStyleBackground	1
#define VListGridStyleForeground	2
#define VListGridStylePixmap		3

/* Values for VListNhScrollBarPolicy and VListNvScrollBarPolicy resources */

#define VListScrollBarPolicyNever	1
#define VListScrollBarPolicyAlways	2
#define VListScrollBarPolicyAsNeeded	3

/* Values for VListNvScrollBarPosition resource */

#define VListScrollBarPositionLeft	1
#define VListScrollBarPositionRight	2

/* Values for VListNcolumnStyle resource */

#define VListColumnStyleSingle		1
#define VListColumnStyleMultiple	2
#define VListColumnStyleBestFit		3

/* Values for VListNselectStyle resource */

#define VListSelectStyleSingle		1
#define VListSelectStyleMultiple	2
#define VListSelectStyleExtend		3
#define VListSelectStyleNone		4

/* Values for VListNselectUnit resource */

#define VListSelectUnitField		1
#define VListSelectUnitRow		2

/* Values for VListNfieldResizePolicy resource and VListField.resizePolicy field */

#define VListResizePolicyAny		1
#define VListResizePolicyGrowOnly	2

/* Bar structure definition for VListLine.barsP */

typedef struct {
    int			mask;		/* mask flags */
    int			lowValue;	/* bar low-end value */
    int			highValue;	/* bar high-end value */
    int			coloridx;	/* bar color index or pixel value */
    int			stippled;	/* bar is stippled (1/0) */
} VListBar;

/* Mask flag values for VListBar.mask */

#define VListBarLowValueMask		(1<<0)
#define VListBarHighValueMask		(1<<1)
#define VListBarColoridxMask		(1<<2)
#define VListBarStippledMask		(1<<3)

/* Grid structure definition for VListField.gridsP */

typedef struct {
    int			mask;		/* mask flags */
    char		*textP;		/* ASCIZ label */
    int			row;		/* row on which label is to appear */
    int			value;		/* value */
    int			dashOn;		/* number of pixels on (or zero) */
    int			dashOff;	/* number of pixels off */
    int			coloridx;	/* grid color index or pixel value */
} VListGrid;

/* Mask flag values for VListGrid.mask */

#define VListGridTextMask		(1<<0)
#define VListGridRowMask		(1<<1)
#define VListGridValueMask		(1<<2)
#define VListGridDashMask		(1<<3)
#define VListGridColoridxMask		(1<<4)

/* Field structure definition for VListNfields resource */

typedef struct {
    int			mask;		/* mask flags */
    int			width;		/* field's minimum width in pixels */
    int			alignment;	/* VListAlignment~ */
    int			marginWidth;	/* margin width in pixels */
    int			marginHeight;	/* margin height in pixels */
    int			resizePolicy;	/* VListResizePolicy~ */
    int			type;		/* VListFieldType~ */

    /* VListFieldTypeHist only */
    int			maxValue;	/* maximum value */
    int			minValue;	/* minimum value */
    VListGrid		*gridsP;	/* address of grid array */
    int			gridCount;	/* number of grids */
} VListField;

/* Mask flag values for VListField.mask */

#define VListFieldWidthMask		(1<<0)
#define VListFieldAlignmentMask		(1<<1)
#define VListFieldTypeMask		(1<<2)
#define VListFieldMaxValueMask		(1<<3)
#define VListFieldMinValueMask		(1<<4)
#define VListFieldGridsMask		(1<<5)
#define VListFieldMarginWidthMask	(1<<6)
#define VListFieldMarginHeightMask	(1<<7)
#define VListFieldResizePolicyMask	(1<<8)

/* Values for VListField.type */

#define VListFieldTypeText		0
#define VListFieldTypeHist		1
#define VListFieldTypeIcon		2

/* Label structure definition for VListNlabels resource */

typedef struct {
    int			mask;		/* mask flags */
    char		*textP;		/* pointer to label's ASCIZ text */
    int			row;		/* zero-based row within header area */
    int			alignment;	/* label alignment */
    int			firstField;	/* first field */
    int			lastField;	/* last field */
    int			pad;		/* padding option */
} VListLabel;

/* Mask flag values for VListLabel.mask */

#define VListLabelTextMask		(1<<0)
#define VListLabelRowMask		(1<<1)
#define VListLabelAlignmentMask		(1<<2)
#define VListLabelFirstFieldMask	(1<<3)
#define VListLabelLastFieldMask		(1<<4)
#define VListLabelPadMask		(1<<5)

/* Values for VListNlabelPadOption and VListLabel.pad */

#define VListLabelPadWithBlanks		1
#define VListLabelPadWithDashes		2

/* Values for VListNalignment resource, VListField.alignment, and VListLabel.alignment */

#define VListAlignmentCenter		1
#define VListAlignmentTop		2
#define VListAlignmentBottom		3
#define VListAlignmentLeft		4
#define VListAlignmentRight		5
#define VListAlignmentTopLeft		6
#define VListAlignmentTopRight		7
#define VListAlignmentBottomLeft	8
#define VListAlignmentBottomRight	9

#define VListAlignmentBeginning		VListAlignmentLeft
#define VListAlignmentEnd		VListAlignmentRight

/* Values for VListNselectButton and VListNdragButton resources */

#define VListAnyButton			0
#define VListButton1			1
#define VListButton2			2
#define VListButton3			3
#define VListButton4			4
#define VListButton5			5

/* String segment definition for VListLine.segmentsP */

typedef struct {
    int			mask;		/* mask flags */
    int			type;		/* segment type (VListSegmentType~) */
    int			coloridx;	/* segment's color index or pixel */
    int			stippled;	/* segment is stippled (1/0) */

    /* VListSegmentTypeText only */
    int			textPosition;	/* 0-based offset in VListLine.textP */
    int			textStrlen;	/* number of bytes in text segment */
    int			fontidx;	/* segment's font index */

    /* VListSegmentTypeIcon only */
    Pixmap		icon;		/* icon pixmap */
} VListSegment;

/* Mask flag values for VListSegment.mask */

#define VListSegmentFontidxMask		(1<<0)
#define VListSegmentColoridxMask	(1<<1)
#define VListSegmentStippledMask	(1<<2)
#define VListSegmentNewLineMask		(1<<3)
#define VListSegmentTypeMask		(1<<4)
#define VListSegmentIconMask		(1<<5)

/* Values for VListSegment.type */

#define VListSegmentTypeText		0
#define VListSegmentTypeIcon		2

/* Line item structure definition */

typedef struct {
    /* common fields - all field types */
    int			mask;		/* mask flags */
    Opaque		closure;	/* line item's Opaque closure (ID) */
    int			select;		/* line item is selected (1/0) */
    int			sensitive;	/* line item can be selected (1/0) */
    int			stippled;	/* line item is stippled (1/0) */
    int        		coloridx;       /* line item's foreground color index */
    int			background;	/* line item's background color index */
    int			type;		/* line item's type (VListLineType~) */

    /* VListLineTypeText only */
    char		*textP;		/* pointer to line item's ASCIZ text */
    int        		fontidx;        /* line item's font index */
    VListSegment	*segmentsP;	/* array of VListSegment structures */
    int			segmentCount;	/* count of VListSegment entries */

    /* VListLineTypeHist only */
    int			highValue;	/* histogram high-end value */
    int			lowValue;	/* histogram low-end value */
    VListBar		*barsP;		/* array of VListBar structures */
    int			barCount;	/* count of VListBar entries */

    /* VListLineTypeIcon only */
    Pixmap		icon;		/* icon pixmap */
} VListLine;

/* Mask flag values for VListLine.mask */

#define VListLineClosureMask		(1<<0)
#define VListLineTextMask		(1<<1)
#define VListLineFontidxMask		(1<<2)
#define VListLineColoridxMask		(1<<3)
#define VListLineSelectMask		(1<<4)
#define VListLineSensitiveMask		(1<<5)
#define VListLineSegmentsMask		(1<<6)
#define VListLineStippledMask		(1<<7)
#define VListLineHighValueMask		(1<<8)
#define VListLineLowValueMask		(1<<9)
#define VListLineBarsMask		(1<<10)
#define VListLineIconMask		(1<<11)
#define VListLineBackgroundMask		(1<<12)
#define VListLineTypeMask		(1<<13)

/* Values for VListLine.type */

#define VListLineTypeText		0
#define VListLineTypeHist		1
#define VListLineTypeIcon		2

/* Visible options for VListMakeLineVisible */

#define VListVisibleInWindow		1
#define VListVisibleAtTop		2
#define VListVisibleAtBottom		3
#define VListVisibleAtLeft		4
#define VListVisibleAtRight		5
#define VListVisibleAtTopLeft		6
#define VListVisibleAtTopRight		7
#define VListVisibleAtBottomLeft	8
#define VListVisibleAtBottomRight	9

/* Special values for VListAddLine row */

#define VListAddAtTop			0
#define VListAddAtBottom		(-1)

/* Special values for VListMoveLine direction */

#define VListMoveLineDown		0
#define VListMoveLineUp			1

/* Structure definition for VListSelectCallbackStruct.selectChangesP */

typedef struct {
    Opaque		closure;	/* line item closure */
    int			oldSelect;	/* was previously selected (1/0) */
    int			newSelect;	/* is now selected (1/0) */
} VListSelectChange;

/* Callback reasons for VListSelectCallbackStruct.reason */

#define VListCRSelect			1
#define VListCRSelectConfirm		2
#define VListCRDragArm			3
#define VListCRDragDrop			4

/* Callback structure for Select and SelectConfirm callbacks */

typedef struct {
    int			reason;			/* VListCRSelect or ~SelectConfirm */
    XEvent		*event;			/* event (may be zero) */
    int			field;			/* upclick logical field number */
    int			row;			/* upclick logical row number */
    VListLine		vLine;			/* line info (closure may be zero) */
    VListSelectChange	*selectChangesP;	/* list of old and new select states */
    int			lineCount;		/* number of line items in the list */
    int			selectionCount;		/* (new) number of selected lines */
} VListSelectCallbackStruct;

/* Callback structure for DragArm callback */

typedef struct {
    int			reason;			/* VListCRDragArm */
    XEvent		*event;			/* event (XButtonEvent) */
    int			field;			/* source field */
    int			row;			/* source row */
    VListLine		vLine;			/* source line info (closure may be zero) */
} VListDragArmCallbackStruct;

/* Callback structure for DragDrop callback */

typedef struct {
    int			reason;			/* VListCRDragDrop */
    XEvent		*event;			/* event (XButtonEvent) */
    int			field;			/* source field */
    int			row;			/* source row */
    VListLine		vLine;			/* source line info (closure may be zero) */
    int			dropWindow;		/* target window on which entry was dropped */
    int			dropX, dropY;		/* coordinates of drop within target window */
    Widget		dropWidget;		/* target widget ID on which entry was dropped */
    int			dropWidgetIsVList;	/* target widget is a VList widget instance (1/0) */
    int			dropField;		/* target VList field */
    int			dropRow;		/* target VList row */
    VListLine		dropVLine;		/* target VList line info (closure may be zero) */
    int			dropVListFieldCount;	/* number of fields in target widget */
    int			dropVListRowCount;	/* number of rows in target widget */
} VListDragDropCallbackStruct;

/* global function prototypes */

int VListComputePreferredWidth(Widget);
int VListGetAllSelectedLines(Widget, Opaque **);
int VListGetAllLines(Widget, Opaque **);
int VListGetLineInfo(Widget, Opaque, VListLine *, int *, int *);
int VListGetLineCount(Widget);
Opaque VListAddLine(Widget, VListLine *, int);
Opaque VListGetFirstSelectedLine(Widget);
Opaque VListFieldRowToLine(Widget, int, int);
Opaque VListXYToLine(Widget, int, int);
void VListChangeLine(Widget, Opaque, VListLine *);
void VListChangeLineBackground(Widget, Opaque, int);
void VListChangeLineBars(Widget, Opaque,  VListBar *, int);
void VListChangeLineClosure(Widget, Opaque, Opaque);
void VListChangeLineColoridx(Widget, Opaque, int);
void VListChangeLineFontidx(Widget, Opaque, int);
void VListChangeLineIcon(Widget, Opaque, Pixmap);
void VListChangeLineSelect(Widget, Opaque, int);
void VListChangeLineSegments(Widget, Opaque, VListSegment *, int);
void VListChangeLineSensitive(Widget, Opaque, int);
void VListChangeLineStippled(Widget, Opaque, int);
void VListChangeLineText(Widget, Opaque, char *);
void VListChangeLineValues(Widget, Opaque, int, int);
void VListEndBatchUpdate(Widget);
void VListInitializeForMRM(void);
void VListMakeLineVisible(Widget, Opaque, int);
void VListMoveLine(Widget, Opaque, int);
void VListRemoveAllLines(Widget);
void VListRemoveLine(Widget, Opaque);
void VListSetAutoScrollPosition(Widget, int, int);
void VListStartBatchUpdate(Widget);
void VListSelectAll(Widget, int);
Widget VListCreate(Widget, char *, Arg *, int);
void VListChangeCell(Widget, int, int, VListLine *);
void VListChangeCellBars(Widget, int, int, VListBar *, int);
void VListChangeCellClosure(Widget, int, int, Opaque);
void VListChangeCellText(Widget, int, int, char *);
void VListChangeCellFontidx(Widget, int, int, int);
void VListChangeCellColoridx(Widget, int, int, int);
void VListChangeCellSelect(Widget, int, int, int);
void VListChangeCellSensitive(Widget, int, int, int);
void VListChangeCellSegments(Widget, int, int, VListSegment *, int);
void VListChangeCellStippled(Widget, int, int, int);
void VListChangeCellValues(Widget, int, int, int, int);
void VListChangeCellIcon(Widget, int, int, Pixmap);
void VListChangeCellBackground(Widget, int, int, int );
int VListLineToCell(Widget ow, Opaque closure, int *xP, int *yP, int *widthP, int *heightP);

#define VListInitializeForDRM	VListInitializeForMRM

#endif /* _VList_h */
