/*
 * Copyright(c) 1992 Bell Communications Research, Inc. (Bellcore)
 *                        All rights reserved
 * Permission to use, copy, modify and distribute this material for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Bellcore not be used in advertising
 * or publicity pertaining to this material without the specific,
 * prior written permission of an authorized representative of
 * Bellcore.
 *
 * BELLCORE MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, EX-
 * PRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST IN-
 * FRINGEMENT OF PATENTS OR OTHER INTELLECTUAL PROPERTY RIGHTS.  THE
 * SOFTWARE IS PROVIDED "AS IS", AND IN NO EVENT SHALL BELLCORE OR
 * ANY OF ITS AFFILIATES BE LIABLE FOR ANY DAMAGES, INCLUDING ANY
 * LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES RELAT-
 * ING TO THE SOFTWARE.
 *
 * MatrixWidget Author: Andrew Wason, Bellcore, aw@bae.bellcore.com
 *
 * Revisions:
 *	1/93:  Added textBackground resource (create/set/get).
 *	1/93:  Added public function XbaeMatrixGetEventRowColumn().
 *	2/93:  Added verticalScrollBar and horizontalScrollBar resources (get only).
 *	8/94:  Added XbaeMatrixGetCurrentCell().
 *      9/94:  Added prev_text to XbaeMatrixModifyVerifyCallbackStruct.
 *      9/94:  Added selectScrollVisible resource
 *	2/95:  Added XbaeMatrixRefresh()
 */

#ifndef _Xbae_Matrix_h
#define _Xbae_Matrix_h

#ifndef lint
static char *matrix_h_id = "@(#)Matrix.h	4.1 3/19/95";
#endif /* lint */

/*
 * Matrix Widget public include file
 */

#include <Xm/Xm.h>
#include <X11/Core.h>


/* Resources:
 * Name			Class			RepType		Default Value
 * ----			-----			-------		-------------
 * altRowCount		AltRowCount		int		1
 * boldLabels		BoldLabels		Boolean		False
 * cellHighlightThickness HighlightThickness	HorizontalDimension 2
 * cellMarginHeight	MarginHeight		VerticalDimension   5
 * cellMarginWidth	MarginWidth		HorizontalDimension 5
 * cells		Cells			StringTable	NULL
 * cellShadowThickness	ShadowThickness		HorizontalDimension 2
 * colors		Colors			PixelTable	NULL
 * columnAlignments	Alignments		AlignmentArray	dynamic
 * columnLabelAlignments Alignments		AlignmentArray	dynamic
 * columnLabels		Labels			StringArray	NULL
 * columnMaxLengths	ColumnMaxLengths	MaxLengthArray	NULL
 * columnWidths		ColumnWidths		WidthArray	NULL
 * columns		Columns			int		0
 * drawCellCallback	Callback		Callback	NULL
 * enterCellCallback	Callback		Callback	NULL
 * evenRowBackground	Background		Pixel		dynamic
 * fixedColumns		FixedColumns		Dimension	0
 * fixedRows		FixedRows		Dimension	0
 * fontList		FontList		FontList	fixed
 * horizonalScrollBar	HorizonalScrollBar	Window		NULL (get only)
 * leaveCellCallback	Callback		Callback	NULL
 * modifyVerifyCallback	Callback		Callback	NULL
 * oddRowBackground	Background		PixelTable	NULL
 * rowLabelAlignment	Alignment		Alignment	XmALIGNMENT_END
 * rowLabelWidth	RowLabelWidth		Short		dynamic
 * rowLabels		Labels			StringArray	NULL
 * rows			Rows			int		0
 * selectCellCallback	Callback		Callback	NULL
 * selectedCells	SelectedCells		BooleanTable	dynamic
 * selectScrollVisible	SelectScrollVisible	Boolean		True
 * space		Space			Dimension	6
 * textBackground	Backgound		Pixel   	dynamic
 * textTranslations	Translations		TranslationTable dynamic
 * topRow		TopRow			int		0
 * traverseCellCallback	Callback		Callback	NULL
 * verticalScrollBar	VerticalScrollBar	Window		NULL (get only)
 * visibleColumns	VisibleColumns		Dimension	0
 * visibleRows		VisibleRows		Dimension	0
 */

#define XmNaltRowCount "altRowCount"
#define XmNboldLabels "boldLabels"
#define XmNcellHighlightThickness "cellHighlightThickness"
#define XmNcellMarginHeight "cellMarginHeight"
#define XmNcellMarginWidth "cellMarginWidth"
#define XmNcellShadowThickness "cellShadowThickness"
#define XmNcells "cells"
#define XmNcolors "colors"
#define XmNcolumnAlignments "columnAlignments"
#define XmNcolumnLabelAlignments "columnLabelAlignments"
#define XmNcolumnLabels "columnLabels"
#define XmNcolumnMaxLengths "columnMaxLengths"
#define XmNcolumnWidths "columnWidths"
#define XmNeditVerifyCallback "editVerifyCallback"
#define XmNdrawCellCallback "drawCellCallback"
#define XmNenterCellCallback "enterCellCallback"
#define XmNevenRowBackground "evenRowBackground"
#define XmNfixedColumns "fixedColumns"
#define XmNfixedRows "fixedRows"
#define XmNleaveCellCallback "leaveCellCallback"
#define XmNoddRowBackground "oddRowBackground"
#define XmNrowLabelAlignment "rowLabelAlignment"
#define XmNrowLabelWidth "rowLabelWidth"
#define XmNrowLabels "rowLabels"
#define XmNselectedCells "selectedCells"
#define XmNselectCellCallback "selectCellCallback"
#define XmNselectScrollVisible "selectScrollVisible"
#define XmNtextBackground "textBackground"
#define XmNtopRow "topRow"
#define XmNtraverseCellCallback "traverseCellCallback"
#define XmNvisibleColumns "visibleColumns"
#define XmNvisibleRows "visibleRows"

#define XmCAlignments "Alignments"
#define XmCAltRowCount "AltRowCount"
#define XmCBoldLabels "BoldLabels"
#define XmCCells "Cells"
#define XmCColors "Colors"
#define XmCColumnMaxLengths "ColumnMaxLengths"
#define XmCColumnWidths "ColumnWidths"
#define XmCFixedColumns "FixedColumns"
#define XmCFixedRows "FixedRows"
#define XmCLabels "Labels"
#define XmCRowLabelWidth "RowLabelWidth"
#define XmCSelectedCells "SelectedCells"
#define XmCSelectScrollVisible "SelectScrollVisible"
#define XmCTextBackground "TextBackground"
#define XmCTopRow "TopRow"
#define XmCVisibleColumns "VisibleColumns"
#define XmCVisibleRows "VisibleRows"

#if XmVersion == 1001
#define XmRStringArray "StringArray"
#endif

#define XmRWidthArray "WidthArray"
#define XmRMaxLengthArray "MaxLengthArray"
#define XmRAlignmentArray "AlignmentArray"
#define XmRPixelTable "PixelTable"
#define XmRBooleanTable "BooleanTable"


/* Class record constants */

externalref WidgetClass xbaeMatrixWidgetClass;

typedef struct _XbaeMatrixClassRec *XbaeMatrixWidgetClass;
typedef struct _XbaeMatrixRec *XbaeMatrixWidget;

/*
 * A few definitions we like to use, but those with R4 won't have.
 * From Xfuncproto.h.
 */

#ifndef XlibSpecificationRelease
# if defined (__cplusplus) || defined(c_plusplus) || (__STDC__ >= 1)
#   define _Xconst const
# else
#   define _Xconst
# endif
# ifndef _XFUNCPROTOBEGIN
#   ifdef __cplusplus                      /* for C++ V2.0 */
#     define _XFUNCPROTOBEGIN extern "C" {   /* do not leave open across includes */
#     define _XFUNCPROTOEND }
#   else
#     define _XFUNCPROTOBEGIN
#     define _XFUNCPROTOEND
#   endif
# endif /* _XFUNCPROTOBEGIN */
#endif

/*
 * External interfaces to class methods
 */
_XFUNCPROTOBEGIN

extern void XbaeMatrixSetCell(
#if NeedFunctionPrototypes
			      Widget		/* w */,
			      int		/* row */,
			      int		/* column */,
			      _Xconst String	/* value */
#endif
			      );

extern void XbaeMatrixEditCell(
#if NeedFunctionPrototypes
			       Widget	/* w */,
			       int	/* row */,
			       int	/* column */
#endif
			       );

extern void XbaeMatrixSelectCell(
#if NeedFunctionPrototypes
				 Widget	/* w */,
				 int	/* row */,
				 int	/* column */
#endif
				 );

extern void XbaeMatrixSelectRow(
#if NeedFunctionPrototypes
				Widget	/* w */,
				int	/* row */
#endif
				);

extern void XbaeMatrixSelectColumn(
#if NeedFunctionPrototypes
				   Widget	/* w */,
				   int		/* column */
#endif
				 );

extern void XbaeMatrixDeselectAll(
#if NeedFunctionPrototypes
				 Widget	/* w */
#endif
				 );

extern void XbaeMatrixDeselectCell(
#if NeedFunctionPrototypes
				   Widget/* w */,
				   int	/* row */,
				   int	/* column */
#endif
				 );

extern void XbaeMatrixDeselectRow(
#if NeedFunctionPrototypes
				   Widget/* w */,
				   int	/* row */
#endif
				 );

extern void XbaeMatrixDeselectColumn(
#if NeedFunctionPrototypes
				   Widget/* w */,
				   int	/* column */
#endif
				 );

extern String XbaeMatrixGetCell(
#if NeedFunctionPrototypes
				Widget	/* w */,
				int	/* row */,
				int	/* column */
#endif
				);

extern Boolean XbaeMatrixCommitEdit(
#if NeedFunctionPrototypes
				    Widget	/* w */,
				    Boolean	/* unmap */
#endif
				    );

extern void XbaeMatrixCancelEdit(
#if NeedFunctionPrototypes
				 Widget		/* w */,
				 Boolean	/* unmap */
#endif
				 );


extern void XbaeMatrixAddRows(
#if NeedFunctionPrototypes
			      Widget	/* w */,
			      int	/* position */,
			      String *	/* rows */,
			      String *	/* labels */,
			      Pixel *	/* colors */,
			      int	/* num_colors */
#endif
			      );

extern void XbaeMatrixDeleteRows(
#if NeedFunctionPrototypes
				 Widget	/* w */,
				 int	/* position */,
				 int	/* num_rows */
#endif
				 );

extern void XbaeMatrixAddColumns(
#if NeedFunctionPrototypes
				 Widget		/* w */,
				 int		/* position */,
				 String *	/* columns */,
				 String *	/* labels */,
				 short *	/* widths */,
				 int *		/* max_lengths */,
				 unsigned char * /* alignments */,
				 unsigned char * /* label_alignments */,
				 Pixel *	/* colors */,
				 int		/* num_columns */
#endif
				 );

extern void XbaeMatrixDeleteColumns(
#if NeedFunctionPrototypes
				    Widget	/* w */,
				    int		/* position */,
				    int		/* num_columns */
#endif
				    );

extern void XbaeMatrixSetRowColors(
#if NeedFunctionPrototypes
				   Widget	/* w */,
				   int		/* position */,
				   Pixel *	/* colors */,
				   int		/* num_colors */
#endif
				   );

extern void XbaeMatrixSetColumnColors(
#if NeedFunctionPrototypes
				      Widget	/* w */,
				      int	/* position */,
				      Pixel *	/* colors */,
				      int	/* num_colors */
#endif
				      );

extern void XbaeMatrixSetCellColor(
#if NeedFunctionPrototypes
				      Widget	/* w */,
				      int	/* row */,
				      int	/* column */,
				      Pixel	/* color */
#endif
				      );

extern int 
XbaeMatrixGetEventRowColumn(
#if NeedFunctionPrototypes
					Widget	 /* w */,
					XEvent * /* event */,
					int *	 /* row */,
					int *	 /* column */
#endif
					);

extern void 
XbaeMatrixGetCurrentCell(
#if NeedFunctionPrototypes
					Widget	 /* w */,
					int *	 /* row */,
					int *	 /* column */
#endif
					);

extern void 
XbaeMatrixRefresh(
#if NeedFunctionPrototypes
					Widget	 /* w */
#endif
					);
_XFUNCPROTOEND


#ifndef XBAE_NO_EXTRA_TYPES
typedef unsigned char	Alignment;
typedef Alignment *	AlignmentArray;
typedef String *	StringTable;
typedef short 		Width;
typedef Width *		WidthArray;
typedef int 		MaxLength;
typedef MaxLength *	MaxLengthArray;
#endif

/*
 * Callback reasons.  Try to stay out of range of the Motif XmCR_* reasons.
 */
typedef enum _XbaeReasonType {
    XbaeModifyVerifyReason = 102,
    XbaeEnterCellReason,
    XbaeLeaveCellReason,
    XbaeTraverseCellReason,
    XbaeSelectCellReason,
    XbaeDrawCellReason
} XbaeReasonType;

/*
 * DrawCell types.
 */
typedef enum {
    XbaeString=1,
    XbaePixmap
} XbaeCellType;

/*
 * Struct passed to modifyVerifyCallback
 */
typedef struct _XbaeMatrixModifyVerifyCallbackStruct {
    XbaeReasonType reason;
    int row, column;
    XmTextVerifyCallbackStruct *verify;
    _Xconst char *prev_text;
} XbaeMatrixModifyVerifyCallbackStruct;

/*
 * Struct passed to drawCellCallback
 */
typedef struct {
    XbaeReasonType reason;
    int row, column;
    int width, height;
    XbaeCellType type;
    String string;
    Pixmap pixmap;
} XbaeMatrixDrawCellCallbackStruct;

/*
 * Struct passed to enterCellCallback
 */
typedef struct _XbaeMatrixEnterCellCallbackStruct {
    XbaeReasonType reason;
    int row, column;
    Boolean doit;
} XbaeMatrixEnterCellCallbackStruct;

/*
 * Struct passed to leaveCellCallback
 */
typedef struct _XbaeMatrixLeaveCellCallbackStruct {
    XbaeReasonType reason;
    int row, column;
    String value;
    Boolean doit;
} XbaeMatrixLeaveCellCallbackStruct;

/*
 * Struct passed to traverseCellCallback
 */
typedef struct _XbaeMatrixTraverseCellCallbackStruct {
    XbaeReasonType reason;
    int row, column;
    int next_row, next_column;
    int fixed_rows, fixed_columns;
    int num_rows, num_columns;
    String param;
    XrmQuark qparam;
} XbaeMatrixTraverseCellCallbackStruct;

/*
 * Struct passed to selectCellCallback
 */
typedef struct _XbaeMatrixSelectCellCallbackStruct {
    XbaeReasonType reason;
    int row, column;
    Boolean **selected_cells;
    String **cells;
    Cardinal num_params;
    String *params;
    XEvent *event;
} XbaeMatrixSelectCellCallbackStruct;

/* provide clean-up for those with R4 */
#ifndef XlibSpecificationRelease
# undef _Xconst
# undef _XFUNCPROTOBEGIN
# undef _XFUNCPROTOEND
#endif

#endif /* _Xbae_Matrix_h */
/* DON'T ADD STUFF AFTER THIS #endif */

