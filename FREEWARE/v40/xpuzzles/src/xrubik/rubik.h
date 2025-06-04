/*
# X-BASED RUBIK'S CUBE(tm)
#
#  Rubik.h
#
###
#
#  Copyright (c) 1994 - 95	David Albert Bagley, bagleyd@source.asset.com
#
#                   All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "playable",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
*/

/* Public header file for Rubik */

#ifndef _XtRubik_h
#define _XtRubik_h

/***********************************************************************
 *
 * Rubik Widget
 *
 ***********************************************************************/

#define XtNselectCallback "selectCallback"
#define XtNsize "size"
#define XtNorient "orient"
#define XtNmono "mono"
#define XtNface "face"
#define XtNi "i"
#define XtNj "j"
#define XtNdirection "direction"
#define XtNpractice "practice"
#define XtNstart "start"
#define XtNfaceColor0 "faceColor0"
#define XtNfaceColor1 "faceColor1"
#define XtNfaceColor2 "faceColor2"
#define XtNfaceColor3 "faceColor3"
#define XtNfaceColor4 "faceColor4"
#define XtNfaceColor5 "faceColor5"
#define XtCSize "Size"
#define XtCOrient "Orient"
#define XtCMono "Mono"
#define XtCFace "Face"
#define XtCI "I"
#define XtCJ "J"
#define XtCDirection "Direction"

#define RUBIK_RESTORE (-3)
#define RUBIK_RESET (-2)
#define RUBIK_IGNORE (-1)
#define RUBIK_MOVED 0
#define RUBIK_CONTROL 1
#define RUBIK_SOLVED 2
#define RUBIK_PRACTICE 3
#define RUBIK_RANDOMIZE 4
#define RUBIK_DEC 5
#define RUBIK_ORIENT 6
#define RUBIK_INC 7
#define RUBIK_COMPUTED 8
#define RUBIK_UNDO 9

typedef struct _RubikClassRec *RubikWidgetClass;
typedef struct _RubikRec *RubikWidget;

extern WidgetClass rubikWidgetClass;

typedef struct {
  XEvent *event;
  int reason;
  int face, i, j, direction;
} rubikCallbackStruct;

#endif _XtRubik_h
/* DON'T ADD STUFF AFTER THIS #endif */
