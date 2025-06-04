/*
**
** Vlist.h
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

#ifndef _Vlist_h_
#define _Vlist_h_

#include "paths.h"
#include INC_XAW(Label.h)

#define XawVlistAll     -13
#define XawVlistEven    -12
#define XawVlistOdd     -11
#define XawVlistCurrent -10
#define XawVlistInvalid  -1
#define XawVlistSet       1
#define XawVlistUnset     2
#define XawVlistToggle    3

#define XtNvlist "vlist"
#define XtCVlist "Vlist"
#define XtNmarkShadowWidth "markShadowWidth"
#define XtNselectedShadowWidth "selectedShadowWidth"
#define XtNhighlightedShadowWidth "highlightedShadowWidth"
#define XtNmarkBackground "markBackground"
#define XtCMarkBackground "MarkBackground"
#define XtNselectedBackground "selectedBackground"
#define XtCSelectedBackground "SelectedBackground"
#define XtNhighlightedBackground "highlightedBackground"
#define XtCHighlightedBackground "HighlightedBackground"
#define XtNhighlightedGeometry "highlightedGeometry"
#define XtCHighlightedGeometry "HighlightedGeometry"
#define XtNselectedGeometry "selectedGeometry"
#define XtCSelectedGeometry "SelectedGeometry"
#define XtNallowMarks "allowMarks"
#define XtCAllowMarks "AllowMarks"

extern WidgetClass vlistWidgetClass;

typedef struct _VlistClassRec   *VlistWidgetClass;
typedef struct _VlistRec        *VlistWidget;

extern int                     VlistHighlighted (
#if NeedFunctionPrototypes
    Widget
#endif
);

extern int                     VlistSelected (
#if NeedFunctionPrototypes
    Widget
#endif
);

extern int                     VlistEntries (
#if NeedFunctionPrototypes
    Widget
#endif
);

extern char*                   VlistVlist (
#if NeedFunctionPrototypes
    Widget
#endif
);

extern int                     VlistEntryOfPosition (
#if NeedFunctionPrototypes
    Widget,
    int
#endif
);

extern void                    VlistPositionOfEntry (
#if NeedFunctionPrototypes
    Widget,
    int,
    int*,
    int*
#endif
);

extern void                     VlistChangeMark (
#if NeedFunctionPrototypes
    Widget,
    int,
    int
#endif
);

extern void                     VlistChangeSelected (
#if NeedFunctionPrototypes
    Widget,
    int,
    int
#endif
);

extern void                     VlistChangeHighlighted (
#if NeedFunctionPrototypes
    Widget,
    int,
    int
#endif
);

#endif /* _Vlist_h_ */
