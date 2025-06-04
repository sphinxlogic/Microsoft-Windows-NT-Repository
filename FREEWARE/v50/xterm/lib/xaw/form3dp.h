/* $XConsortium: FormP.h,v 1.20 91/05/02 16:20:29 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987 */


/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * this code was modified in 12/93 by Johannes Plass,
 * plass@dipmza.physik.uni-mainz.de
 *
*/  

/* Form widget private definitions */

#ifndef _XawForm3dP_h
#define _XawForm3dP_h

#ifdef VMS
#   ifdef  _FORM3D_LOCAL_ /*###jp### 23.1.95 */
#      include "Form3d.h"
#   else
#      include <XAW_DIRECTORY/Form3d.h>
#   endif
#   include <X11_DIRECTORY/ConstrainP.h>
#else
#   include "Form3d.h"
#   include <X11/ConstrainP.h>
#endif

#define XtREdgeType "EdgeType"

typedef enum {LayoutPending, LayoutInProgress, LayoutDone} LayoutState;
#define XtInheritLayout ((Boolean (*)())_XtInherit)
#define XtInheritForm3dShadowDraw ((void (*)())_XtInherit)

typedef struct {
    Boolean	(*layout)(/* Form3dWidget, Dimension, Dimension */);
} Form3dClassPart;

/*
 * Layout(
 *	FormWidget w	- the widget whose children are to be configured
 *	Dimension w, h	- bounding box of layout to be calculated
 *
 *  Stores preferred geometry in w->form.preferred_{width,height}.
 *  If w->form.resize_in_layout is True, then a geometry request
 *  may be made for the preferred bounding box if necessary.
 *
 *  Returns True if a geometry request was granted, False otherwise.
 */

typedef struct {
    void (*shadowdraw)();
  } ThreeDForm3dClassPart;   


typedef struct _Form3dClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    Form3dClassPart	form_class;
    ThreeDForm3dClassPart	threeD_class;
} Form3dClassRec;

extern Form3dClassRec form3dClassRec;

typedef struct _FormPart {
    /* resources */
    int		default_spacing;    /* default distance between children */
    /* private state */
    Dimension	old_width, old_height; /* last known dimensions		 */
    int		no_refigure;	    /* no re-layout while > 0		 */
    Boolean	needs_relayout;	    /* next time no_refigure == 0	 */
    Boolean	resize_in_layout;   /* should layout() do geom request?  */
    Dimension	preferred_width, preferred_height; /* cached from layout */
    Boolean     resize_is_no_op;    /* Causes resize to take not action. */
} Form3dPart;

typedef struct _ThreeDForm3dPart {
    Dimension	shadow_width;
    Pixel	top_shadow_pixel;
    Pixel	bot_shadow_pixel;
    Pixmap	top_shadow_pxmap;
    Pixmap	bot_shadow_pxmap;
    int		top_shadow_contrast;
    int		bot_shadow_contrast;
    GC		top_shadow_GC;
    GC		bot_shadow_GC;
    XtPointer	user_data;
    Boolean	be_nice_to_cmap;
  } ThreeDForm3dPart;      

typedef struct _Form3dRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    Form3dPart		form;
    ThreeDForm3dPart	threeD;
} Form3dRec;

typedef struct _Form3dConstraintsPart {
    XtEdgeType	top, bottom,	/* where to drag edge on resize		*/
		left, right;
    int		dx;		/* desired horiz offset			*/
    int		dy;		/* desired vertical offset		*/
    Widget	horiz_base;	/* measure dx from here if non-null	*/
    Widget	vert_base;	/* measure dy from here if non-null	*/
    Boolean	allow_resize;	/* TRUE if child may request resize	*/
    short	virtual_width, virtual_height;
    Position new_x, new_y;
    LayoutState	layout_state;	/* temporary layout state		*/
    Boolean	deferred_resize; /* was resized while no_refigure is set */
} Form3dConstraintsPart;


typedef struct _Form3dConstraintsRec {
    Form3dConstraintsPart	form;
} Form3dConstraintsRec, *Form3dConstraints;

#endif /* _XawFormP_h */
