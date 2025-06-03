/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Xw.h
 **
 **   Project:     X Widgets
 **
 **   Description: This include file contains public defines and structures
 **                needed by all XToolkit applications using the X Widget Set.
 **                Included in the file are the definitions for all common
 **                resource types, defines for resource or arg list values,
 **                and the class and instance record definitions for all meta
 **                classes.
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/


#ifndef _XtXw_h
#define _XtXw_h

/************************************************************************
 *
 *  Good 'ole Max and Min from the defunct <X11/Misc.h>
 *
 ************************************************************************/
#define Max(x, y)       (((x) > (y)) ? (x) : (y))
#define Min(x, y)       (((x) > (y)) ? (y) : (x))

/************************************************************************
 *
 *  Resource manager definitions
 *
 ************************************************************************/

#ifndef	XtNtraversalOn
#define XtNtraversalOn          "traversalOn"
#endif
#ifndef	XtCTraversalOn
#define XtCTraversalOn          "TraversalOn"
#endif

#ifndef	XtNtraversalType
#define XtNtraversalType        "traversalType"
#endif
#ifndef	XtCTraversalType
#define XtCTraversalType        "TraversalType"
#endif
#ifndef	XtRTraversalType
#define XtRTraversalType        "TraversalType"
#endif

#ifndef	XtNhighlightStyle
#define XtNhighlightStyle       "highlightStyle"
#endif
#ifndef	XtCHighlightStyle
#define XtCHighlightStyle       "HighlightStyle"
#endif
#ifndef	XtNhighlightTile
#define XtNhighlightTile        "highlightTile"
#endif
#ifndef	XtCHighlightTile
#define XtCHighlightTile        "HighlightTile"
#endif
#ifndef	XtRHighlightStyle
#define XtRHighlightStyle       "HighlightStyle"
#endif
#ifndef	XtNhighlightThickness
#define XtNhighlightThickness   "highlightThickness"
#endif
#ifndef	XtCHighlightThickness
#define XtCHighlightThickness   "HighlightThickness"
#endif
#ifndef	XtNhighlightColor
#define XtNhighlightColor       "highlightColor"
#endif

#ifndef	XtNbackgroundTile
#define XtNbackgroundTile       "backgroundTile"
#endif
#ifndef	XtCBackgroundTile
#define XtCBackgroundTile       "BackgroundTile"
#endif

#ifndef	XtNcursor
#define XtNcursor 		"cursor"
#endif

#ifndef	XtNrecomputeSize
#define XtNrecomputeSize 	"recomputeSize"
#endif
#ifndef	XtCRecomputeSize
#define XtCRecomputeSize 	"RecomputeSize"
#endif

#ifndef	XtNlayout
#define XtNlayout               "layout"
#endif
#ifndef	XtCLayout
#define XtCLayout               "Layout"
#endif
#ifndef	XtRLayout
#define XtRLayout               "Layout"
#endif

#ifndef	XtRTileType
#define XtRTileType              "tileType"
#endif

#ifndef	XtRTranslationTable
#define XtRTranslationTable      "TranslationTable"
#endif
#ifndef	XtNlabelLocation
#define XtNlabelLocation         "labelLocation"
#endif
#ifndef	XtCLabelLocation
#define XtCLabelLocation         "LabelLocation"
#endif
#ifndef	XtRLabelLocation
#define XtRLabelLocation         "LabelLocation"
#endif
#ifndef	XtNsensitiveTile
#define XtNsensitiveTile 	"sensitiveTile"
#endif
#ifndef	XtCSensitiveTile
#define XtCSensitiveTile 	"SensitiveTile"
#endif

#ifndef	XtNcolumns
#define XtNcolumns               "columns"
#endif
#ifndef	XtCColumns
#define XtCColumns               "Columns"
#endif
#ifndef	XtNmode
#define XtNmode                  "mode"
#endif
#ifndef	XtCMode
#define XtCMode                  "Mode"
#endif
#ifndef	XtRRCMode
#define XtRRCMode                "Mode"
#endif
#ifndef	XtNset
#define XtNset                   "set"
#endif
#ifndef	XtCSet
#define XtCSet                   "Set"
#endif

#ifndef	XtNselect
#define XtNselect               "select"
#endif
#ifndef	XtNrelease
#define XtNrelease              "release"
#endif
#ifndef	XtNnextTop
#define XtNnextTop              "nextTop"
#endif

#ifndef	XtNtitleShowing
#define XtNtitleShowing         "titleShowing"
#endif
#ifndef	XtCTitleShowing
#define XtCTitleShowing         "TitleShowing"
#endif
#ifndef	XtNmgrTitleOverride
#define XtNmgrTitleOverride     "mgrTitleOverride"
#endif
#ifndef	XtCTitleOverride
#define XtCTitleOverride        "TitleOverride"
#endif
#ifndef	XtNtitleType
#define XtNtitleType            "titleType"
#endif
#ifndef	XtCTitleType
#define XtCTitleType            "TitleType"
#endif
#ifndef	XtRTitleType
#define XtRTitleType            "TitleType"
#endif
#ifndef	XtNtitleString
#define XtNtitleString          "titleString"
#endif
#ifndef	XtCTitleString
#define XtCTitleString          "TitleString"
#endif
#ifndef	XtNtitleImage
#define XtNtitleImage           "titleImage"
#endif
#ifndef	XtCTitleImage
#define XtCTitleImage           "TitleImage"
#endif
#ifndef	XtNfontColor
#define XtNfontColor            "fontColor"
#endif
#ifndef	XtNmnemonic
#define XtNmnemonic             "mnemonic"
#endif
#ifndef	XtCMnemonic
#define XtCMnemonic             "Mnemonic"
#endif
#ifndef	XtNunderlineTitle
#define XtNunderlineTitle       "underlineTitle"
#endif
#ifndef	XtCUnderlineTitle
#define XtCUnderlineTitle       "UnderlineTitle"
#endif
#ifndef	XtNmgrUnderlineOverride
#define XtNmgrUnderlineOverride  "mgrUnderlineOverride"
#endif
#ifndef	XtCUnderlineOverride
#define XtCUnderlineOverride    "UnderlineOverride"
#endif
#ifndef	XtNunderlinePosition
#define XtNunderlinePosition    "underlinePosition"
#endif
#ifndef	XtCUnderlinePosition
#define XtCUnderlinePosition    "UnderlinePosition"
#endif
#ifndef	XtNattachTo
#define XtNattachTo             "attachTo"
#endif
#ifndef	XtCAttachTo
#define XtCAttachTo             "AttachTo"
#endif
#ifndef	XtNkbdAccelerator
#define XtNkbdAccelerator       "kbdAccelerator"
#endif
#ifndef	XtCKbdAccelerator
#define XtCKbdAccelerator       "KbdAccelerator"
#endif

#ifndef	XtNassociateChildren
#define XtNassociateChildren    "associateChildren"
#endif
#ifndef	XtCAssociateChildren
#define XtCAssociateChildren    "AssociateChildren"
#endif
#ifndef	XtNmenuPost
#define XtNmenuPost             "menuPost"
#endif
#ifndef	XtCMenuPost
#define XtCMenuPost             "MenuPost"
#endif
#ifndef	XtNmenuSelect
#define XtNmenuSelect           "menuSelect"
#endif
#ifndef	XtCMenuSelect
#define XtCMenuSelect           "MenuSelect"
#endif
#ifndef	XtNpostAccelerator
#define XtNpostAccelerator      "postAccelerator"
#endif
#ifndef	XtCPostAccelerator
#define XtCPostAccelerator      "PostAccelerator"
#endif
#ifndef	XtNmenuUnpost
#define XtNmenuUnpost           "menuUnpost"
#endif
#ifndef	XtCMenuUnpost
#define XtCMenuUnpost           "MenuUnpost"
#endif
#ifndef	XtNkbdSelect
#define XtNkbdSelect            "kbdSelect"
#endif
#ifndef	XtCKbdSelect
#define XtCKbdSelect            "KbdSelect"
#endif


/*  Resources for the List Widget */

#ifndef	XtNnumColumns
#define XtNnumColumns 	 	"numColumns"
#endif
#ifndef	XtCNumColumns
#define XtCNumColumns 	 	"NumColumns"
#endif

#ifndef	XtNrowPosition
#define XtNrowPosition 		"rowPosition"
#endif
#ifndef	XtCRowPosition
#define XtCRowPosition 		"RowPosition"
#endif

#ifndef	XtNcolumnPosition
#define XtNcolumnPosition 	"columnPosition"
#endif
#ifndef	XtCColumnPosition
#define XtCColumnPosition 	"ColumnPosition"
#endif

#ifndef	XtNselectionMethod
#define XtNselectionMethod 	"selectionMethod"
#endif
#ifndef	XtCSelectionMethod
#define XtCSelectionMethod 	"SelectionMethod"
#endif
#ifndef	XtRSelectionMethod
#define XtRSelectionMethod 	"SelectionMethod"
#endif

#ifndef	XtNelementHighlight
#define XtNelementHighlight 	"elementHighlight"
#endif
#ifndef	XtCElementHighlight
#define XtCElementHighlight 	"ElementHighlight"
#endif
#ifndef	XtRElementHighlight
#define XtRElementHighlight 	"ElementHighlight"
#endif

#ifndef	XtNselectionBias
#define XtNselectionBias 	"selectionBias"
#endif
#ifndef	XtCSelectionBias
#define XtCSelectionBias 	"SelectionBias"
#endif
#ifndef	XtRSelectionBias
#define XtRSelectionBias 	"SelectionBias"
#endif

#ifndef	XtNselectionStyle
#define XtNselectionStyle 	"selectionStyle"
#endif
#ifndef	XtCSelectionStyle
#define XtCSelectionStyle 	"SelectionStyle"
#endif
#ifndef	XtRSelectionStyle
#define XtRSelectionStyle 	"SelectionStyle"
#endif

#ifndef	XtNcolumnWidth
#define XtNcolumnWidth 		"columnWidth" 
#endif
#ifndef	XtCColumnWidth
#define XtCColumnWidth 		"ColumnWidth"
#endif

#ifndef	XtNelementHeight
#define XtNelementHeight 	"elementHeight"
#endif
#ifndef	XtCElementHeight
#define XtCElementHeight 	"ElementHeight"
#endif

#ifndef	XtNselectedElements
#define XtNselectedElements 	"selectedElements"
#endif
#ifndef	XtCSelectedElements
#define XtCSelectedElements 	"SelectedElements"
#endif
#ifndef	XtNnumSelectedElements
#define XtNnumSelectedElements 	"numSelectedElements"
#endif
#ifndef	XtCNumSelectedElements
#define XtCNumSelectedElements   "NumSelectedElements"
#endif

#ifndef	XtNdestroyMode
#define XtNdestroyMode 		"destroyMode"
#endif
#ifndef	XtCDestroyMode
#define XtCDestroyMode 		"DestroyMode"
#endif
#ifndef	XtRDestroyMode
#define XtRDestroyMode 		"DestroyMode"
#endif

/*  Resource definition for the row column manager  */

#ifndef	XtNlayoutType
#define XtNlayoutType       "layoutType"
#endif
#ifndef	XtCLayoutType
#define XtCLayoutType       "LayoutType"
#endif
#ifndef	XtRLayoutType
#define XtRLayoutType       "LayoutType"
#endif

#ifndef	XtNforceSize
#define XtNforceSize        "forceSize"
#endif
#ifndef	XtCForceSize
#define XtCForceSize        "ForceSize"
#endif

#ifndef	XtNsingleRow
#define XtNsingleRow 	   "singleRow"
#endif
#ifndef	XtCSingleRow
#define XtCSingleRow 	   "SingleRow"
#endif


/*  Resource definition for the menu separator  */

#ifndef	XtNseparatorType
#define XtNseparatorType 	"separatorType"
#endif
#ifndef	XtCSeparatorType
#define XtCSeparatorType 	"SeparatorType"
#endif
#ifndef	XtRSeparatorType
#define XtRSeparatorType 	"SeparatorType"
#endif

/****************
 *
 *  New resource names for Scrolled Window
 *
 ****************/

#ifndef	XtNvsbX
#define XtNvsbX           "vsbX"
#endif
#ifndef	XtNvsbY
#define XtNvsbY           "vsbY"
#endif
#ifndef	XtNvsbWidth
#define XtNvsbWidth 	 "vsbWidth"
#endif
#ifndef	XtNvsbHeight
#define XtNvsbHeight 	 "vsbHeight"
#endif
#ifndef	XtNhsbX
#define XtNhsbX 	 	 "hsbX"
#endif
#ifndef	XtNhsbY
#define XtNhsbY 	 	 "hsbY"
#endif
#ifndef	XtNhsbWidth
#define XtNhsbWidth 	 "hsbWidth"
#endif
#ifndef	XtNhsbHeight
#define XtNhsbHeight 	 "hsbHeight"
#endif
#ifndef	XtNvSliderMin
#define XtNvSliderMin 	 "vSliderMin"
#endif
#ifndef	XtNvSliderMax
#define XtNvSliderMax 	 "vSliderMax"
#endif
#ifndef	XtNvSliderOrigin
#define XtNvSliderOrigin  "vSliderOrigin"
#endif
#ifndef	XtNvSliderExtent
#define XtNvSliderExtent  "vSliderExtent"
#endif
#ifndef	XtNhSliderMin
#define XtNhSliderMin 	 "hSliderMin"
#endif
#ifndef	XtNhSliderMax
#define XtNhSliderMax 	 "hSliderMax"
#endif
#ifndef	XtNhSliderOrigin
#define XtNhSliderOrigin  "hSliderOrigin"
#endif
#ifndef	XtNhSliderExtent
#define XtNhSliderExtent  "hSliderExtent"
#endif

#ifndef	XtNhScrollEvent
#define XtNhScrollEvent 	 "hScrollEvent"
#endif
#ifndef	XtNvScrollEvent
#define XtNvScrollEvent 	 "vScrollEvent"
#endif

#ifndef	XtNvScrollBarWidth
#define XtNvScrollBarWidth 	 "vScrollBarWidth"
#endif
#ifndef	XtNvScrollBarHeight
#define XtNvScrollBarHeight 	 "vScrollBarHeight"
#endif
#ifndef	XtNhScrollBarWidth
#define XtNhScrollBarWidth 	 "hScrollBarWidth"
#endif
#ifndef	XtNhScrollBarHeight
#define XtNhScrollBarHeight 	 "hScrollBarHeight"
#endif

#ifndef	XtNforceVerticalSB
#define XtNforceVerticalSB 	 "forceVerticalSB"
#endif
#ifndef	XtNforceHorizontalSB
#define XtNforceHorizontalSB 	 "forceHorizontalSB"
#endif

#ifndef	XtNinitialX
#define XtNinitialX 	 "initialX"
#endif
#ifndef	XtNinitialY
#define XtNinitialY 	 "initialY"
#endif

#ifndef	XtNborderPad
#define XtNborderPad 	 "borderPad"
#endif

#ifndef	XtCVsbX
#define XtCVsbX 	 	 "VsbX"
#endif
#ifndef	XtCVsbY
#define XtCVsbY 	 	 "VsbY"
#endif
#ifndef	XtCVsbWidth
#define XtCVsbWidth 	 "VsbWidth"
#endif
#ifndef	XtCVsbHeight
#define XtCVsbHeight 	 "VsbHeight"
#endif
#ifndef	XtCHsbX
#define XtCHsbX 	 	 "HsbX"
#endif
#ifndef	XtCHsbY
#define XtCHsbY 	 	 "HsbY"
#endif
#ifndef	XtCHsbWidth
#define XtCHsbWidth 	 "HsbWidth"
#endif
#ifndef	XtCHsbHeight
#define XtCHsbHeight 	 "HsbHeight"
#endif
#ifndef	XtCVSliderMin
#define XtCVSliderMin 	 "VSliderMin"
#endif
#ifndef	XtCVSliderMax
#define XtCVSliderMax 	 "VSliderMax"
#endif
#ifndef	XtCVSliderOrigin
#define XtCVSliderOrigin  "VSliderOrigin"
#endif
#ifndef	XtCVSliderExtent
#define XtCVSliderExtent  "VSliderExtent"
#endif
#ifndef	XtCHSliderMin
#define XtCHSliderMin 	 "HSliderMin"
#endif
#ifndef	XtCHSliderMax
#define XtCHSliderMax 	 "HSliderMax"
#endif
#ifndef	XtCHSliderOrigin
#define XtCHSliderOrigin  "HSliderOrigin"
#endif
#ifndef	XtCHSliderExtent
#define XtCHSliderExtent  "HSliderExtent"
#endif

#ifndef	XtCVScrollBarWidth
#define XtCVScrollBarWidth 	 "VScrollBarWidth"
#endif
#ifndef	XtCVScrollBarHeight
#define XtCVScrollBarHeight 	 "VScrollBarHeight"
#endif
#ifndef	XtCHScrollBarWidth
#define XtCHScrollBarWidth 	 "HScrollBarWidth"
#endif
#ifndef	XtCHScrollBarHeight
#define XtCHScrollBarHeight 	 "HScrollBarHeight"
#endif

#ifndef	XtCForceVerticalSB
#define XtCForceVerticalSB 	 "ForceVerticalSB"
#endif
#ifndef	XtCForceHorizontalSB
#define XtCForceHorizontalSB 	 "ForceHorizontalSB"
#endif

#ifndef	XtCInitialX
#define XtCInitialX 	 "InitialX"
#endif
#ifndef	XtCInitialY
#define XtCInitialY 	 "InitialY"
#endif
#ifndef	XtCBorderPad
#define XtCBorderPad 	 "BorderPad"
#endif

/****************
 *
 *  New resource types for Static Raster
 *
 ****************/

#ifndef	XtNsRimage
#define XtNsRimage       "rasterImage"
#endif
#ifndef	XtCSRimage
#define XtCSRimage       "RasterImage"
#endif

#ifndef	XtNshowSelected
#define XtNshowSelected 	       "showSelected"
#endif
#ifndef	XtCShowSelected
#define XtCShowSelected 	       "ShowSelected"
#endif

/*  List Manager Defines */

#define XwSINGLE		0
#define XwMULTIPLE		1
#define XwBORDER		0
#define XwINVERT		1
#define XwNO_BIAS		0
#define XwROW_BIAS		1
#define XwCOL_BIAS		2
#define XwINSTANT		0
#define XwSTICKY		1
#define XwNO_SHRINK		0
#define XwSHRINK_COLUMN		1
#define XwSHRINK_ALL		2


/****************************************************************
 *
 * TextEdit widget
 *
 ****************************************************************/

#ifndef	XtNdisplayPosition
#define XtNdisplayPosition       "displayPosition"
#endif
#ifndef	XtNinsertPosition
#define XtNinsertPosition 	"insertPosition"
#endif
#ifndef	XtNleftMargin
#define XtNleftMargin 		"leftMargin"
#endif
#ifndef	XtNrightMargin
#define XtNrightMargin 		"rightMargin"
#endif
#ifndef	XtNtopMargin
#define XtNtopMargin 		"topMargin"
#endif
#ifndef	XtNbottomMargin
#define XtNbottomMargin 		"bottomMargin"
#endif
#ifndef	XtNselectionArray
#define XtNselectionArray 	"selectionArray"
#endif
#ifndef	XtNtextSource
#define XtNtextSource 		"textSource"
#endif
#ifndef	XtNselection
#define XtNselection 		"selection"
#endif
#ifndef	XtNmaximumSize
#define XtNmaximumSize 		"maximumSize"
#endif
#ifndef	XtCMaximumSize
#define XtCMaximumSize           "MaximumSize"
#endif

#ifndef	XtNeditType
#define XtNeditType 		"editType"
#endif
#ifndef	XtNfile
#define XtNfile 			"file"
#endif
#ifndef	XtNstring
#define XtNstring 		"string"
#endif
#ifndef	XtNlength
#define XtNlength 		"length"
#endif
#ifndef	XtNfont
#define XtNfont 			"font"
#endif
#ifndef	XtNdiskSrc
#define XtNdiskSrc               "disksrc"
#endif
#ifndef	XtNstringSrc
#define XtNstringSrc             "stringsrc"
#endif
#ifndef	XtNexecute
#define XtNexecute               "execute"
#endif

#ifndef	XtNsourceType
#define XtNsourceType            "sourceType"
#endif
#ifndef	XtCSourceType
#define XtCSourceType            "SourceType"
#endif
#ifndef	XtRSourceType
#define XtRSourceType            "SourceType"
#endif

#ifndef	XtNmotionVerification
#define XtNmotionVerification 	"motionVerification"
#endif
#ifndef	XtNmodifyVerification
#define XtNmodifyVerification 	"modifyVerification"
#endif
#ifndef	XtNleaveVerification
#define XtNleaveVerification 	"leaveVerification"
#endif

#ifndef	XtNwrap
#define XtNwrap 			"wrap"
#endif
#ifndef	XtCWrap
#define XtCWrap 			"Wrap"
#endif
#ifndef	XtRWrap
#define XtRWrap 			"Wrap"
#endif

#ifndef	XtNwrapForm
#define XtNwrapForm 		"wrapForm"
#endif
#ifndef	XtCWrapForm
#define XtCWrapForm 		"WrapForm"
#endif
#ifndef	XtRWrapForm
#define XtRWrapForm 		XtCWrapForm
#endif

#ifndef	XtNwrapBreak
#define XtNwrapBreak 		"wrapBreak"
#endif
#ifndef	XtCWrapBreak
#define XtCWrapBreak 		"WrapBreak"
#endif
#ifndef	XtRWrapBreak
#define XtRWrapBreak 		XtCWrapBreak
#endif

#ifndef	XtNscroll
#define XtNscroll 		"scroll"
#endif
#ifndef	XtCScroll
#define XtCScroll 		"Scroll"
#endif
#ifndef	XtRScroll
#define XtRScroll 		XtCScroll
#endif

#ifndef	XtNgrow
#define XtNgrow 			"grow"
#endif
#ifndef	XtCGrow
#define XtCGrow 			"Grow"
#endif
#ifndef	XtRGrow
#define XtRGrow 			XtCGrow
#endif

#define XwAutoScrollOff		0
#define XwAutoScrollHorizontal	1
#define XwAutoScrollVertical	2
#define XwAutoScrollBoth	3  /* must be bitwise OR of horiz. and vert. */

#define XwGrowOff		0
#define XwGrowHorizontal	1
#define XwGrowVertical		2
#define XwGrowBoth		3  /* must be bitwise OR of horiz. and vert. */

/* Viable Selection Modes For RowCol Manager */

#define XwN_OF_MANY  0
#define XwONE_OF_MANY 1


/*  Defines for the row col manager layout types  */

#define XwREQUESTED_COLUMNS             0
#define XwMAXIMUM_COLUMNS               1
#define XwMAXIMUM_UNALIGNED             2



/* Valid Label Location Settings for Button */

#define XwRIGHT		0
#define XwLEFT		1
#define XwCENTER	2


/* Valid Title Type Values For MenuPane */

#define XwSTRING  0
#define XwIMAGE   1


/* New resource manager types */

#define XrmRImage	      	"Image"
#ifndef	XtRImage
#define XtRImage 	      	XrmRImage
#endif
#ifndef	XtNalignment
#define XtNalignment 	"alignment"
#endif
#ifndef	XtCAlignment
#define XtCAlignment 	"Alignment"
#endif
#ifndef	XtRAlignment
#define XtRAlignment 	"Alignment"
#endif
#ifndef	XtNlineSpace
#define XtNlineSpace  	"lineSpace"
#endif
#ifndef	XtCLineSpace
#define XtCLineSpace  	"LineSpace"
#endif
#ifndef	XtNgravity
#define XtNgravity 		"gravity"
#endif
#ifndef	XtCGravity
#define XtCGravity 		"Gravity"
#endif
#ifndef	XtRGravity
#define XtRGravity 		"Gravity"
#endif

typedef enum {
	XwALIGN_NONE,
	XwALIGN_LEFT,
	XwALIGN_CENTER,
	XwALIGN_RIGHT
} XwAlignment;

typedef enum {
	XwUNKNOWN,
	XwPULLDOWN,
	XwTITLE,
	XwWORK_SPACE
} XwWidgetType;



/*  Drawing types for the menu separator.  */

#define XwNO_LINE               0
#define XwSINGLE_LINE           1
#define XwDOUBLE_LINE           2
#define XwSINGLE_DASHED_LINE    3
#define XwDOUBLE_DASHED_LINE    4


/*  Defines for the valuator background and slider visual types */

#define XwSOLID		0
#define XwPATTERN	1
#define XwTRANSPARENT	2

#define XwHORIZONTAL	0
#define XwVERTICAL	1


/*  Define the new resource types used by Valuator  */

#ifndef	XtNsliderMin
#define XtNsliderMin 		"sliderMin"
#endif
#ifndef	XtCSliderMin
#define XtCSliderMin 		"SliderMin"
#endif
#ifndef	XtNsliderMax
#define XtNsliderMax 		"sliderMax"
#endif
#ifndef	XtCSliderMax
#define XtCSliderMax 		"SliderMax"
#endif
#ifndef	XtNsliderOrigin
#define XtNsliderOrigin 		"sliderOrigin"
#endif
#ifndef	XtCSliderOrigin
#define XtCSliderOrigin 		"SliderOrigin"
#endif
#ifndef	XtNsliderExtent
#define XtNsliderExtent 		"sliderExtent"
#endif
#ifndef	XtCSliderExtent
#define XtCSliderExtent 		"SliderExtent"
#endif
#ifndef	XtNsliderColor
#define XtNsliderColor 		"sliderColor"
#endif

#ifndef	XtNslideOrientation
#define XtNslideOrientation      "slideOrientation"
#endif
#ifndef	XtCSlideOrientation
#define XtCSlideOrientation      "SlideOrientation"
#endif
#ifndef	XtRSlideOrientation
#define XtRSlideOrientation      "slideOrientation"
#endif

#ifndef	XtNslideMoveType
#define XtNslideMoveType         "slideMoveType"
#endif
#ifndef	XtCSlideMoveType
#define XtCSlideMoveType         "SlideMoveType"
#endif
#ifndef	XtRSlideMoveType
#define XtRSlideMoveType         "slideMoveType"
#endif

#ifndef	XtNslideAreaTile
#define XtNslideAreaTile         "slideAreaTile"
#endif
#ifndef	XtCSlideAreaTile
#define XtCSlideAreaTile         "SlideAreaTile"
#endif

#ifndef	XtNsliderMoved
#define XtNsliderMoved 		"sliderMoved"
#endif
#ifndef	XtNareaSelected
#define XtNareaSelected 		"areaSelected"
#endif

#ifndef	XtNsliderTile
#define XtNsliderTile 	"sliderTile"
#endif
#ifndef	XtCSliderTile
#define XtCSliderTile 	"SliderTile"
#endif

#ifndef	XtNsliderReleased
#define XtNsliderReleased 	"sliderReleased"
#endif



/*  Form resource definitions  */

#ifndef	XtNxRefName
#define XtNxRefName 		"xRefName"
#endif
#ifndef	XtCXRefName
#define XtCXRefName 		"XRefName"
#endif
#ifndef	XtNxRefWidget
#define XtNxRefWidget 		"xRefWidget"
#endif
#ifndef	XtCXRefWidget
#define XtCXRefWidget 		"XRefWidget"
#endif
#ifndef	XtNxOffset
#define XtNxOffset 		"xOffset"
#endif
#ifndef	XtCXOffset
#define XtCXOffset 		"XOffset"
#endif
#ifndef	XtNxAddWidth
#define XtNxAddWidth 		"xAddWidth"
#endif
#ifndef	XtCXAddWidth
#define XtCXAddWidth 		"XAddWidth"
#endif
#ifndef	XtNxVaryOffset
#define XtNxVaryOffset 		"xVaryOffset"
#endif
#ifndef	XtCXVaryOffset
#define XtCXVaryOffset 		"XVaryOffset"
#endif
#ifndef	XtNxResizable
#define XtNxResizable 		"xResizable"
#endif
#ifndef	XtCXResizable
#define XtCXResizable 		"XResizable"
#endif
#ifndef	XtNxAttachRight
#define XtNxAttachRight 		"xAttachRight"
#endif
#ifndef	XtCXAttachRight
#define XtCXAttachRight 		"XAttachRight"
#endif
#ifndef	XtNxAttachOffset
#define XtNxAttachOffset 	"xAttachOffset"
#endif
#ifndef	XtCXAttachOffset
#define XtCXAttachOffset 	"XAttachOffset"
#endif

#ifndef	XtNyRefName
#define XtNyRefName 		"yRefName"
#endif
#ifndef	XtCYRefName
#define XtCYRefName 		"YRefName"
#endif
#ifndef	XtNyRefWidget
#define XtNyRefWidget 		"yRefWidget"
#endif
#ifndef	XtCYRefWidget
#define XtCYRefWidget 		"YRefWidget"
#endif
#ifndef	XtNyOffset
#define XtNyOffset 		"yOffset"
#endif
#ifndef	XtCYOffset
#define XtCYOffset 		"YOffset"
#endif
#ifndef	XtNyAddHeight
#define XtNyAddHeight 		"yAddHeight"
#endif
#ifndef	XtCYAddHeight
#define XtCYAddHeight 		"YAddHeight"
#endif
#ifndef	XtNyVaryOffset
#define XtNyVaryOffset 		"yVaryOffset"
#endif
#ifndef	XtCYVaryOffset
#define XtCYVaryOffset 		"YVaryOffset"
#endif
#ifndef	XtNyResizable
#define XtNyResizable 		"yResizable"
#endif
#ifndef	XtCYResizable
#define XtCYResizable 		"YResizable"
#endif
#ifndef	XtNyAttachBottom
#define XtNyAttachBottom 	"yAttachBottom"
#endif
#ifndef	XtCYAttachBottom
#define XtCYAttachBottom 	"YAttachBottom"
#endif
#ifndef	XtNyAttachOffset
#define XtNyAttachOffset 	"yAttachOffset"
#endif
#ifndef	XtCYAttachOffset
#define XtCYAttachOffset 	"YAttachOffset"
#endif



/*  ImageEdit resource definitions  */

#ifndef	XtNpixelScale
#define XtNpixelScale 		"pixelScale"
#endif
#ifndef	XtCPixelScale
#define XtCPixelScale 		"PixelScale"
#endif

#ifndef	XtNgridThickness
#define XtNgridThickness 	"gridThickness"
#endif
#ifndef	XtCGridThickness
#define XtCGridThickness 	"GridThickness"
#endif

#ifndef	XtNimage
#define XtNimage 		"image"
#endif
#ifndef	XtCImage
#define XtCImage 		"Image"
#endif

#ifndef	XtNdrawColor
#define XtNdrawColor 		"drawColor"
#endif
#ifndef	XtNeraseColor
#define XtNeraseColor 		"eraseColor"
#endif

#ifndef	XtNeraseOn
#define XtNeraseOn 		"eraseOn"
#endif
#ifndef	XtCEraseOn
#define XtCEraseOn 		"EraseOn"
#endif


/*  MenuBtn esource manager definitions  */

#ifndef	XtNlabelType
#define XtNlabelType 	      "labelType"
#endif
#ifndef	XtCLabelType
#define XtCLabelType 	      "LabelType"
#endif
#ifndef	XtRLabelType
#define XtRLabelType 	      "LabelType"
#endif
#ifndef	XtNlabelImage
#define XtNlabelImage          "labelImage"
#endif
#ifndef	XtCLabelImage
#define XtCLabelImage          "LabelImage"
#endif
#ifndef	XtNcascadeImage
#define XtNcascadeImage        "cascadeImage"
#endif
#ifndef	XtCCascadeImage
#define XtCCascadeImage        "CascadeImage"
#endif
#ifndef	XtNmarkImage
#define XtNmarkImage           "markImage"
#endif
#ifndef	XtCMarkImage
#define XtCMarkImage           "MarkImage"
#endif
#ifndef	XtNsetMark
#define XtNsetMark             "setMark"
#endif
#ifndef	XtCSetMark
#define XtCSetMark             "SetMark"
#endif
#ifndef	XtNcascadeOn
#define XtNcascadeOn           "cascadeOn"
#endif
#ifndef	XtCCascadeOn
#define XtCCascadeOn 	      "CascadeOn"
#endif
#ifndef	XtNinvertOnEnter
#define XtNinvertOnEnter 	"invertOnEnter"
#endif
#ifndef	XtCInvertOnEnter
#define XtCInvertOnEnter 	"InvertOnEnter"
#endif
#ifndef	XtNmgrOverrideMnemonic
#define XtNmgrOverrideMnemonic   "mgrOverrideMnemonic"
#endif
#ifndef	XtCMgrOverrideMnemonic
#define XtCMgrOverrideMnemonic   "MgrOverrideMnemonic"
#endif
#ifndef	XtNcascadeSelect
#define XtNcascadeSelect       "cascadeSelect"
#endif
#ifndef	XtNcascadeUnselect
#define XtNcascadeUnselect     "cascadeUnselect"
#endif
#ifndef	XtNmenuMgrId
#define XtNmenuMgrId 	      "menuMgrId"
#endif
#ifndef	XtCMenuMgrId
#define XtCMenuMgrId 	      "MenuMgrId"
#endif


/*  ScrollBar resource definitions  */

#ifndef	XtNscrollbarOrientation
#define XtNscrollbarOrientation 		"scrollbarOrientation"
#endif
#ifndef	XtNselectionColor
#define XtNselectionColor 		"selectionColor"
#endif

#ifndef	XtNinitialDelay
#define XtNinitialDelay 			"initialDelay"
#endif
#ifndef	XtCInitialDelay
#define XtCInitialDelay 			"InitialDelay"
#endif
#ifndef	XtNrepeatRate
#define XtNrepeatRate 			"repeatRate"
#endif
#ifndef	XtCRepeatRate
#define XtCRepeatRate 			"RepeatRate"
#endif
#ifndef	XtNgranularity
#define XtNgranularity 			"granularity"
#endif
#ifndef	XtCGranularity
#define XtCGranularity 			"Granularity"
#endif

/*  Resources for PushButton  */

#ifndef	XtNinvertOnSelect
#define XtNinvertOnSelect 	"invertOnSelect"
#endif
#ifndef	XtCInvertOnSelect
#define XtCInvertOnSelect 	"InvertOnSelect"
#endif
#ifndef	XtNtoggle
#define XtNtoggle                "toggle"
#endif
#ifndef	XtCToggle
#define XtCToggle                "Toggle"
#endif


/*  Resources for Toggle  */

#ifndef	XtNsquare
#define XtNsquare 	"square"
#endif
#ifndef	XtCSquare
#define XtCSquare 	"Square"
#endif
#ifndef	XtNselectColor
#define XtNselectColor 	"selectColor"
#endif

/*  Resources for Vertical Paned Window  */

#ifndef	XtNallowResize
#define XtNallowResize 		"allowResize"
#endif
#ifndef	XtNsashIndent
#define XtNsashIndent 		"sashIndent"
#endif
#ifndef	XtCSashIndent
#define XtCSashIndent            "SashIndent"
#endif
#ifndef	XtNrefigureMode
#define XtNrefigureMode 		"refigureMode"
#endif
#ifndef	XtNpadding
#define XtNpadding               "padding"
#endif
#ifndef	XtCPadding
#define XtCPadding               "Padding"
#endif
#ifndef	XtNmin
#define XtNmin 			"min"
#endif
#ifndef XtCMin
#ifndef	XtCMin
#define XtCMin 			"Min"
#endif
#endif
#ifndef	XtNmax
#define XtNmax 			"max"
#endif
#ifndef XtCMax
#ifndef	XtCMax
#define XtCMax 			"Max"
#endif
#endif
#ifndef	XtNskipAdjust
#define XtNskipAdjust 		"skipAdjust"
#endif
#ifndef	XtNframed
#define XtNframed                "framed"
#endif
#ifndef	XtNborderFrame
#define XtNborderFrame           "borderFrame"
#endif


/*  WorkSpace resources  */

#ifndef	XtNexpose
#define XtNexpose 	"expose"
#endif
#ifndef XtNresize
#ifndef	XtNresize
#define XtNresize 	"resize"
#endif
#endif
#ifndef	XtNkeyDown
#define XtNkeyDown 	"keyDown"
#endif


/***********************************************************************
 *
 * Cascading MenuPane Widget Resources
 *
 ***********************************************************************/

#ifndef	XtRTitlePositionType
#define XtRTitlePositionType     "TitlePositionType"
#endif


/***********************************************************************
 *
 * Popup Menu Manager Widget Resources
 *
 ***********************************************************************/

#ifndef	XtNstickyMenus
#define XtNstickyMenus         "stickyMenus"
#endif
#ifndef	XtCStickyMenus
#define XtCStickyMenus         "StickyMenus"
#endif


/****
 *
 * Menu stuff from hpwm
 *
 ****/
#ifndef	XtNtopShadowColor
#define XtNtopShadowColor 	"topShadowColor"
#endif
#ifndef	XtNbottomShadowColor
#define XtNbottomShadowColor 	"bottomShadowColor"
#endif
#ifndef	XtNbottomShadowTile
#define XtNbottomShadowTile 	"bottomShadowTile"
#endif
#ifndef	XtCBottomShadowTile
#define XtCBottomShadowTile 	"BottomShadowTile"
#endif
#ifndef	XtNtopShadowTile
#define XtNtopShadowTile 	"topShadowTile"
#endif
#ifndef	XtCTopShadowTile
#define XtCTopShadowTile 	"TopShadowTile"
#endif


/***********************************************************************
 *
 * Pulldown Menu Manager Widget Resources
 *
 ***********************************************************************/


#ifndef	XtNallowCascades
#define XtNallowCascades         "allowCascades"
#endif
#ifndef	XtCAllowCascades
#define XtCAllowCascades         "AllowCascades"
#endif
#ifndef	XtNpulldownBarId
#define XtNpulldownBarId         "pulldownBarId"
#endif
#ifndef	XtCPulldownBarId
#define XtCPulldownBarId         "PulldownBarId"
#endif


/***********************************************************************
 *
 * Cascading MenuPane Widget Attribute Definitions
 *
 ***********************************************************************/

#define XwTOP    0x01
#define XwBOTTOM 0x02
#define XwBOTH   XwTOP|XwBOTTOM

/***********************************************************************
 *
 * Static Text resource string definitions
 *
 ***********************************************************************/
#ifndef	XtNstrip
#define XtNstrip 	"strip"
#endif
#ifndef	XtCStrip
#define XtCStrip 	"Strip"
#endif

/***********************************************************************
 *
 * TitleBar resource string definitions
 *
 ***********************************************************************/
#ifndef	XtRTilePix
#define XtRTilePix            	"TilePix"
#endif
#ifndef	XtNtitlePrecedence
#define XtNtitlePrecedence     	"titlePrecedence"
#endif
#ifndef	XtCTitlePrecedence
#define XtCTitlePrecedence     	"TitlePrecedence"
#endif
#ifndef	XtNtitleForeground
#define XtNtitleForeground    	"titleForeground"
#endif
#ifndef	XtNtitleBackground
#define XtNtitleBackground    	"titleBackground"
#endif
#ifndef	XtNtitleRegion
#define XtNtitleRegion       	"titleRegion"
#endif
#ifndef	XtCTitleRegion
#define XtCTitleRegion       	"TitleRegion"
#endif
#ifndef	XtNtitlePosition
#define XtNtitlePosition     	"titlePosition"
#endif
#ifndef	XtCTitlePosition
#define XtCTitlePosition     	"TitlePosition"
#endif
#ifndef	XtNtitleRPadding
#define XtNtitleRPadding     	"titleRPadding"
#endif
#ifndef	XtCTitleRPadding
#define XtCTitleRPadding     	"TitleRPadding"
#endif
#ifndef	XtNtitleLPadding
#define XtNtitleLPadding     	"titleLPadding"
#endif
#ifndef	XtCTitleLPadding
#define XtCTitleLPadding     	"TitleLPadding"
#endif
#ifndef	XtNtitleBorderWidth
#define XtNtitleBorderWidth  	"titleBorderWidth"
#endif
#ifndef	XtCTitleBorderWidth
#define XtCTitleBorderWidth  	"TitleBorderWidth"
#endif
#ifndef	XtNtitleTranslations
#define XtNtitleTranslations 	"titleTranslations"
#endif
#ifndef	XtCTitleTranslations
#define XtCTitleTranslations 	"TitleTranslations"
#endif
#ifndef	XtNtitleHSpace
#define XtNtitleHSpace       	"titleHSpace"
#endif
#ifndef	XtCTitleHSpace
#define XtCTitleHSpace       	"TitleHSpace"
#endif
#ifndef	XtNtitleVSpace
#define XtNtitleVSpace       	"titleVSpace"
#endif
#ifndef	XtCTitleVSpace
#define XtCTitleVSpace       	"TitleVSpace"
#endif
#ifndef	XtNtitleSelect
#define XtNtitleSelect       	"titleSelect"
#endif
#ifndef	XtNtitleRelease
#define XtNtitleRelease       	"titleRelease"
#endif
#ifndef	XtNtitleNextTop
#define XtNtitleNextTop       	"titleNextTop"
#endif
#ifndef	XtNtitlebarTile
#define XtNtitlebarTile      	"titlebarTile"
#endif
#ifndef	XtNenter
#define XtNenter              	"enter"
#endif
#ifndef	XtNleave
#define XtNleave              	"leave"
#endif
#ifndef	XtNregion
#define XtNregion            	"region"
#endif
#ifndef	XtNposition
#define XtNposition          	"position"
#endif
#ifndef	XtNlPadding
#define XtNlPadding          	"lPadding"
#endif
#ifndef	XtCLPadding
#define XtCLPadding          	"LPadding"
#endif
#ifndef	XtNrPadding
#define XtNrPadding          	"rPadding"
#endif
#ifndef	XtCRPadding
#define XtCRPadding          	"RPadding"
#endif
#ifndef	XtNprecedence
#define XtNprecedence        	"precedence"
#endif
#ifndef	XtCPrecedence
#define XtCPrecedence        	"Precedence"
#endif

/***********************************************************************
 *
 * Panel resource string definitions
 *
 ***********************************************************************/
#ifndef	XtNtitleToMenuPad
#define XtNtitleToMenuPad         "titleToMenuPad"
#endif
#ifndef	XtCTitleToMenuPad
#define XtCTitleToMenuPad         "TitleToMenuPad"
#endif
#ifndef	XtNworkSpaceToSiblingPad
#define XtNworkSpaceToSiblingPad  "workSpaceToSiblingPad"
#endif
#ifndef	XtCWorkSpaceToSiblingPad
#define XtCWorkSpaceToSiblingPad  "WorkSpaceToSiblingPad"
#endif
#ifndef	XtNwidgetType
#define XtNwidgetType             "widgetType"
#endif
#ifndef	XtCWidgetType
#define XtCWidgetType             "WidgetType"
#endif
#ifndef	XtRWidgetType
#define XtRWidgetType             "WidgetType"
#endif
#ifndef	XtNtopLevel
#define XtNtopLevel               "topLevel"
#endif
#ifndef	XtCTopLevel
#define XtCTopLevel               "TopLevel"
#endif
#ifndef	XtNdisplayTitle
#define XtNdisplayTitle           "displayTitle"
#endif
#ifndef	XtCDisplayTitle
#define XtCDisplayTitle           "DisplayTitle"
#endif
#ifndef	XtNcausesResize
#define XtNcausesResize           "causesResize" 
#endif
#ifndef	XtCCausesResize
#define XtCCausesResize           "CausesResize" 
#endif

/************************************************************************
 *
 *  Class record constants for Primitive Widget Meta Class
 *
 ************************************************************************/

extern WidgetClass XwprimitiveWidgetClass;

typedef struct _XwPrimitiveClassRec * XwPrimitiveWidgetClass;
typedef struct _XwPrimitiveRec      * XwPrimitiveWidget;


/*  Tile types used for area filling and patterned text placement  */

#define XwFOREGROUND		0
#define XwBACKGROUND		1
#define Xw25_FOREGROUND		2
#define Xw50_FOREGROUND		3
#define Xw75_FOREGROUND		4
#define XwVERTICAL_TILE		5
#define XwHORIZONTAL_TILE	6
#define XwSLANT_RIGHT		7
#define XwSLANT_LEFT		8


/*  Traversal type definitions  */

#define XwHIGHLIGHT_OFF		0
#define XwHIGHLIGHT_ENTER	1
#define XwHIGHLIGHT_TRAVERSAL	2



/*  Defines for the arrow directions  */

#define XwARROW_UP	0
#define XwARROW_DOWN	1
#define XwARROW_LEFT	2
#define XwARROW_RIGHT	3


/*  Define the new resource types used by Arrow  */

#ifndef	XtNarrowDirection
#define XtNarrowDirection 	"arrowDirection"
#endif
#ifndef	XtCArrowDirection
#define XtCArrowDirection 	"ArrowDirection"
#endif
#ifndef	XtRArrowDirection
#define XtRArrowDirection 	"arrowDirection"
#endif



/*  Border highlighting type defines  */

#define XwPATTERN_BORDER   1
#define XwWIDGET_DEFINED   2


/* Manager Layout Info */

#define XwIGNORE           0
#define XwMINIMIZE         1
#define XwMAXIMIZE         2
#define XwSWINDOW          3   /* Special Setting for Scrolled Window */

#ifndef	XtNwidgetType
#define XtNwidgetType             "widgetType"
#endif
#ifndef	XtCWidgetType
#define XtCWidgetType             "WidgetType"
#endif
#ifndef	XtRWidgetType
#define XtRWidgetType             "WidgetType"
#endif

/************************************************************************
 *
 *  Class record constants for Meta Class Widgets
 *
 ************************************************************************/

extern WidgetClass XwmanagerWidgetClass;

typedef struct _XwManagerClassRec * XwManagerWidgetClass;
typedef struct _XwManagerRec      * XwManagerWidget;



extern WidgetClass XwbuttonWidgetClass;

typedef struct _XwButtonClassRec * XwButtonWidgetClass;
typedef struct _XwButtonRec      * XwButtonWidget;


extern WidgetClass XwmenupaneWidgetClass;

typedef struct _XwMenuPaneClassRec *XwMenuPaneWidgetClass;
typedef struct _XwMenuPaneRec      *XwMenuPaneWidget;


extern WidgetClass XwmenumgrWidgetClass;

typedef struct _XwMenuMgrClassRec *XwMenuMgrWidgetClass;
typedef struct _XwMenuMgrRec      *XwMenuMgrWidget;



#endif _XtXw_h
/* DON'T ADD STUFF AFTER THIS #endif */


