/* $XFree86: mit/lib/Xt/Constraint.c,v 1.4 1993/03/20 03:40:41 dawes Exp $ */
/* $XConsortium: Constraint.c,v 1.13 91/06/10 15:08:01 converse Exp $ */

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

#define CONSTRAINT
#include "IntrinsicI.h"
#include "StringDefs.h"

static void ConstraintPartInitialize();
externaldef(constraintclassrec) ConstraintClassRec constraintClassRec = {
  { /******* CorePart *******/
#ifndef SVR3SHLIB
    /* superclass	    */	(WidgetClass) &compositeClassRec,
#else
    /* superclass           */  NULL,	/* binded on runtime */
#endif
    /* class_name	    */	"Constraint",
    /* widget_size	    */	sizeof(ConstraintRec),
    /* class_initialize     */  NULL,
    /* class_part_initialize*/	ConstraintPartInitialize,
    /* class_inited	    */	FALSE,
    /* initialize	    */	NULL,
    /* initialize_hook      */	NULL,		
    /* realize		    */	XtInheritRealize,
    /* actions		    */	NULL,
    /* num_actions	    */	0,
    /* resources	    */	NULL,
    /* num_resources	    */	0,
    /* xrm_class	    */	NULLQUARK,
    /* compress_motion      */	FALSE,
    /* compress_exposure    */	TRUE,
    /* compress_enterleave  */  FALSE,
    /* visible_interest     */	FALSE,
    /* destroy		    */	NULL,
    /* resize		    */	NULL,
    /* expose		    */	NULL,
    /* set_values	    */	NULL,
    /* set_values_hook      */	NULL,			
    /* set_values_almost    */	XtInheritSetValuesAlmost,  
    /* get_values_hook      */	NULL,			
    /* accept_focus	    */	NULL,
    /* version		    */	XtVersion,
    /* callback_offsets     */  NULL,
    /* tm_table		    */  NULL,
    /* query_geometry	    */  NULL,
    /* display_accelerator  */  NULL,
    /* extension	    */  NULL
  },{ /**** CompositePart *****/
    /* geometry_handler     */  NULL,
    /* change_managed       */  NULL,
    /* insert_child	    */  XtInheritInsertChild,
    /* delete_child	    */  XtInheritDeleteChild,
    /* extension	    */  NULL
  },{ /**** ConstraintPart ****/
    /* resources	    */  NULL,
    /* num_resources	    */  0,
    /* constraint_size      */  0,
    /* initialize	    */  NULL,
    /* destroy		    */  NULL,
    /* set_values	    */  NULL,
    /* extension	    */  NULL
  }
};
#ifndef SVR3SHLIB
externaldef(constraintwidgetclass) WidgetClass constraintWidgetClass =
	(WidgetClass) &constraintClassRec;
#else
externalref WidgetClass constraintWidgetClass;
#define _XtInherit  (*_libXt__XtInherit)
#endif


static void ConstraintPartInitialize(wc)
    WidgetClass wc;
{
    ConstraintWidgetClass cwc = (ConstraintWidgetClass)wc;
    if (cwc->constraint_class.resources)
	_XtCompileResourceList(cwc->constraint_class.resources,
			       cwc->constraint_class.num_resources);

    _XtConstraintResDependencies((ConstraintWidgetClass)wc);
}
