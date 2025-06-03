/* tlist.h - TList widget C-language include file

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains C-language declarations needed to use the TList widget.
Include this file into any C module that directly uses a TList widget.

MODIFICATION HISTORY:

03-Sep-1993 (sjk/nfc) VToolkit V2.

19-Mar-1992 (sjk) Conditionalize #pragma for VMS only.

06-Nov-1991 (sjk) Cleanup.

09-Apr-1991 (sjk) Add TListNcolorNameList resource.

03-Apr-1991 (sjk) Add TListNcolorList, TListNfontTable and TListNscrollDelay resources.
    Get rid of obsolete TListNfont, TListNboldFont, TListNscrollBarWidth and TListNscrollBarSepWidth resources.

16-Feb-1990 (sjk) Add TListNwordWrap resource.

11-Oct-1989 (sjk) Initial entry.
*/

#ifndef _TList_h
#define _TList_h

/* Widget class */

external WidgetClass		tlistwidgetclass;

/* Resource names */

#define TListNcolorList		"colorList"
#define TListNcolorNameList	"colorNameList"
#define TListNfontTable		"fontTable"
#define TListNforeground	"foreground"
#define TListNmarginWidth	"marginWidth"
#define TListNmarginHeight	"marginHeight"
#define TListNmaxLineCnt	"maxLineCnt"
#define TListNscrollDelay	"scrollDelay"
#define TListNscrollFactor	"scrollFactor"
#define TListNwordWrap		"wordWrap"

void TListAddLine(Widget, char *, int, int);
void TListInitializeForMRM(void);
void TListReset(Widget);
Widget TListCreate(Widget, char *, Arg *, int);

#define TListInitializeDRM	TListInitializeForMRM

#endif /* _TList_h */
