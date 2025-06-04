/*
# X-BASED PYRAMINX(tm)
#
#  Pyraminx.h
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

/* Public header file for Pyraminx */

#ifndef _XtPyraminx_h
#define _XtPyraminx_h

/***********************************************************************
 *
 * Pyraminx Widget
 *
 ***********************************************************************/

#define XtNselectCallback "selectCallback"
#define XtNsize "size"
#define XtNmode "mode"
#define XtNorient "orient"
#define XtNsticky "sticky"
#define XtNmono "mono"
#define XtNpractice "practice"
#define XtNstart "start"
#define XtNfaceColor0 "faceColor0"
#define XtNfaceColor1 "faceColor1"
#define XtNfaceColor2 "faceColor2"
#define XtNfaceColor3 "faceColor3"
#define XtCSize "Size"
#define XtCMode "Mode"
#define XtCOrient "Orient"
#define XtCSticky "Sticky"
#define XtCMono "Mono"
#define PYRAMINX_RESTORE (-3)
#define PYRAMINX_RESET (-2)
#define PYRAMINX_IGNORE (-1)
#define PYRAMINX_MOVED 0
#define PYRAMINX_CONTROL 1
#define PYRAMINX_SOLVED 2
#define PYRAMINX_PRACTICE 3
#define PYRAMINX_RANDOMIZE 4
#define PYRAMINX_DEC 5
#define PYRAMINX_ORIENT 6
#define PYRAMINX_INC 7
#define PYRAMINX_PERIOD2 8
#define PYRAMINX_PERIOD3 9
#define PYRAMINX_BOTH 10
#define PYRAMINX_STICKY 11
#define PYRAMINX_COMPUTED 12
#define PYRAMINX_UNDO 13

typedef struct _PyraminxClassRec *PyraminxWidgetClass;
typedef struct _PyraminxRec *PyraminxWidget;

extern WidgetClass pyraminxWidgetClass;

typedef struct {
  XEvent *event;
  int reason;
} pyraminxCallbackStruct;

#endif _XtPyraminx_h
/* DON'T ADD STUFF AFTER THIS #endif */
