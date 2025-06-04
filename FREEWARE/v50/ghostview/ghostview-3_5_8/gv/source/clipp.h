/*
**
** ClipP.h
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


#ifndef _XawClipP_h
#define _XawClipP_h

#include INC_X11(CompositeP.h)
#include "Clip.h"

typedef struct {			/* new fields in widget class */
    int dummy;
} ClipClassPart;

typedef struct _ClipClassRec {	/* Clip widget class */
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ClipClassPart clip_class;
} ClipClassRec;


typedef struct {			/* new fields in widget */
    /* resources... */
    XtCallbackList report_callbacks;	/* callback/Callback */
#if 0
    int child_x;
    int child_y;
#endif
    /* private data... */
    Dimension     child_width_nat;
    Dimension     child_height_nat;
    Dimension     child_border_nat; 
    Position      child_x_nat;
    Position      child_y_nat;
} ClipPart;

typedef struct _ClipRec {
    CorePart core;
    CompositePart composite;
    ClipPart clip;
} ClipRec;


/*
 * external declarations
 */


extern ClipClassRec clipClassRec;

#endif /* _XawClipP_h */
