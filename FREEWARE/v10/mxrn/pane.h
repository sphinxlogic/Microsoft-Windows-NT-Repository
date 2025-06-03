/*
* $Header: Pane.h,v 1.4 88/02/26 13:50:51 weissman Exp $
*/

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * V1.0a 17-Mar-1988	RDB
 *	fix major bugs and enhancements
 * V1.4	 03-Nov-1988	RDB
 *	updates and revisions
 * V2.0	 04-Apr-1989	RDB
 *	updates and revisions
 * V2.1	 11-Apr-1989	RDB
 *	add leftmullion
 */

#ifndef _DwtPane_h
#define _DwtPane_h

#define NoSharedPaneMask	0L
#define	SharedPaneMask		(1L<<0)
#define	ViewableInPaneMask	(1L<<1)

/***********************************************************************
 *
 * Pane Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* New fields */
#define DwtNmullionSize		"mullionSize"
#define DwtNmin			"min"
#define DwtNmax			"max"
#define DwtNoverrideText	"overrideText"
#define DwtNposition		"position"
#define DwtNsharedFlag		"sharedFlag"

#define DwtCMullionSize		"MullionSize"
#define DwtCMin			"Min"
#define DwtCMax			"Max"
#define DwtCOverrideText	"OverrideText"
#define DwtCPosition		"Position"
#define DwtCSharedFlag		"SharedFlag"

/*
 * New fields for the Pane widget class record 
 */
typedef struct {
    DwtOffsetPtr	paneoffsets;
    int mumble;   /* No new procedures */
} PaneClassPart;
 
/* 
 * Full class record declaration
 */
typedef struct _PaneClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    PaneClassPart	pane_class;
} PaneClassRec, *PaneClass;
 
 
/*
 * New fields for the Pane widget record
 */
typedef struct _PanePart {
    DwtCallbackStruct helpcallback,	/* Help callback */
                      map_callback,     /* about to be mapped */
		      unmap_callback,   /* just been unmapped */                            
		      focus_callback;	/* grabbed input focus */
    unsigned char orientation;		/* Horizontal or Vertical pane. */
    Pixel foreground;		/* Foreground color for mullions, borders, etc. */
    Dimension mullionsize;	/* Size of mullions. same as borderwidth */
    Dimension mullionlength;	/* Actual Size of mullions. */
    Boolean leftmullion;	/* Have we actually left the mullion area */
    short intersub;		/* Number of pixels between subwidgets. */
    WidgetList children;	/* List of subwidgets. */
    Cardinal num_children;	/* Number of subwidgets. */
    int resize_mode;		/* If TRUE, children are allowed to change */
				/* their length to whatever they want, and */
				/* we don't attempt to make things add up to */
				/* the correct total length. */
    GC invgc;			/* GC to use to draw magic borders */
    Widget whichadjust;		/* Which window we're currently dragging. */
    Position origloc;		/* Where the button was originally pressed. */
    Boolean overridetext;	/* Whether to override text bindings. */
    Widget lasthadfocus;	/* The last widget to which we gave focus. */
} PanePart;
 
/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/
 
typedef struct _PaneRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart  constraint;
    PanePart	    pane;
} PaneRec, *PaneWidget;
 
/* 
 * Data to be kept for each child.
 */
typedef struct _ConstraintsRec {
    Dimension min, max;		/* Limits on length of this widget. */
    Dimension dlength;		/* Desired length for this widget. */
    Widget mullion;		/* Widget used for the mullion below this */
				/* widget (if any) */
    Position magicborder;	/* Where the last magic border was drawn. */
    Position position;		/* Where to place this beast */
    Cardinal sharedflag;	/* The position is shared with another widget */
				/* Also is the subwidget viewable */
    Boolean resizable;		/* allow widget to be resized by pane? */

} ConstraintsRec,*Constraints;
 
#ifndef PANE
external PaneClassRec panewidgetclassrec;
external PaneClass panewidgetclass;
#endif

extern Widget DwtPaneCreate(); /* parent,name,arglist,argCount */
    /* Widget parent; */
    /* char   *name; */
    /* Arg    *arglist; */
    /* int    argCount; */

extern Widget DwtHVPane(); /* parent, name, x, y, width, height, orientation, mapcallback, helpcallback */
    /* Widget		parent; */
    /* char		*name; */
    /* Position		x, y; */
    /* Dimension	width, height; */
    /* int		orientation; */
    /* DwtCallBackPtr	mapcallback, helpcallback; */

extern void DwtPaneAddWidget(); /* subwidget, position, min, max, resizeable, sharedflag */
    /* Widget subwidget; */
    /* Cardinal position */
    /* Dimension *min, *max; */
    /* Boolean resizable */
    /* Cardinal	sharedflag */

extern void DwtPaneGetMinMax(); /* subwidget, min, max */
    /* Widget subwidget; */
    /* Dimension *min, *max; */

extern void DwtPaneSetMinMax(); /* subwidget, min, max */
    /* Widget subwidget; */
    /* Dimension min, max; */

extern void DwtPaneSetConstraints(); /* subwidget, min, max, height, resizable */
    /* Widget subwidget; */
    /* Dimension min, max, height; */
    /* Boolean resizable */

extern void DwtPaneSetMin(); /* subwidget, min*/
    /* Widget subwidget; */
    /* Dimension min; */

extern void DwtPaneSetMax(); /* subwidget, max */
    /* Widget subwidget; */
    /* Dimension max; */

extern void DwtPaneAllowResizing(); /* pane, allow */
    /* PaneWidget pane; */
    /* int allowtype; */

extern void DwtPaneMakeViewable(); /* subwidget*/
    /* Widget subwidget; */

extern unsigned int PaneInitializeForDRM();

#endif _DwtPane_h
/* DON'T ADD STUFF AFTER THIS #endif */

/*  DEC/CMS REPLACEMENT HISTORY, Element PANE.H */
/*  *6    11-APR-1989 11:27:31 BRINKLEY "leftmullion added" */
/*  *5     5-APR-1989 09:40:03 BRINKLEY "final standard I hope" */
/*  *4    30-MAR-1989 14:16:55 BRINKLEY "definitions of global variables fixed" */
/*  *3    28-MAR-1989 11:07:53 BRINKLEY "Style Guide Conformity" */
/*  *2     6-JAN-1989 12:35:18 BRINKLEY "new version of pane widget" */
/*  *1    20-DEC-1988 14:51:23 RYAN "Initial elements for V2" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PANE.H */
