/* $XFree86: mit/lib/Xt/Core.c,v 1.4 1993/03/20 03:40:44 dawes Exp $ */
/* $XConsortium: Core.c,v 1.55 91/06/11 20:25:04 converse Exp $ */

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

#define _XT_CORE_C

#include "IntrinsicP.h"
#include "EventI.h"
#include "TranslateI.h"
#include "ResourceI.h"
#include "RectObj.h"
#include "RectObjP.h"
#include "StringDefs.h"

/******************************************************************
 *
 * CoreWidget Resources
 *
 ******************************************************************/

#ifndef SVR3SHLIB
externaldef(xtinherittranslations) int _XtInheritTranslations = 0;
#else
externalref int _XtInheritTranslations;
#endif
extern String XtCXtToolkitError; /* from IntrinsicI.h */
static void XtCopyScreen();

static XtResource resources[] = {
    {XtNscreen, XtCScreen, XtRScreen, sizeof(Screen*),
      XtOffsetOf(CoreRec,core.screen), XtRCallProc, (XtPointer)XtCopyScreen},
/*_XtCopyFromParent does not work for screen because the Display
parameter is not passed through to the XtRCallProc routines */
    {XtNdepth, XtCDepth, XtRInt,sizeof(int),
         XtOffsetOf(CoreRec,core.depth),
	 XtRCallProc, (XtPointer)_XtCopyFromParent},
    {XtNcolormap, XtCColormap, XtRColormap, sizeof(Colormap),
	 XtOffsetOf(CoreRec,core.colormap),
	 XtRCallProc,(XtPointer)_XtCopyFromParent},
    {XtNbackground, XtCBackground, XtRPixel,sizeof(Pixel),
         XtOffsetOf(CoreRec,core.background_pixel),
	 XtRString, (XtPointer)"XtDefaultBackground"},
    {XtNbackgroundPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
         XtOffsetOf(CoreRec,core.background_pixmap),
	 XtRImmediate, (XtPointer)XtUnspecifiedPixmap},
    {XtNborderColor, XtCBorderColor, XtRPixel,sizeof(Pixel),
         XtOffsetOf(CoreRec,core.border_pixel),
         XtRString,(XtPointer)"XtDefaultForeground"},
    {XtNborderPixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
         XtOffsetOf(CoreRec,core.border_pixmap),
	 XtRImmediate, (XtPointer)XtUnspecifiedPixmap},
    {XtNmappedWhenManaged, XtCMappedWhenManaged, XtRBoolean, sizeof(Boolean),
         XtOffsetOf(CoreRec,core.mapped_when_managed),
	 XtRImmediate, (XtPointer)True},
    {XtNtranslations, XtCTranslations, XtRTranslationTable,
        sizeof(XtTranslations), XtOffsetOf(CoreRec,core.tm.translations),
        XtRTranslationTable, (XtPointer)NULL},
    {XtNaccelerators, XtCAccelerators, XtRAcceleratorTable,
        sizeof(XtTranslations), XtOffsetOf(CoreRec,core.accelerators),
        XtRTranslationTable, (XtPointer)NULL}
    };

static void CoreInitialize();
static void CoreClassPartInitialize();
static void CoreDestroy();
static void CoreRealize();
static Boolean CoreSetValues();
static void CoreSetValuesAlmost();

#ifndef SVR3SHLIB
static
#else
#define unNamedObjClassRec _libXtCore_unNamedObjClassRec
#endif
RectObjClassRec unNamedObjClassRec = {
  {
#ifndef SVR3SHLIB
    /* superclass	  */	(WidgetClass)&rectObjClassRec,
#else
    /* superclass         */    NULL,	/* binded on runtime */
#endif
    /* class_name	  */	"UnNamedObj",
    /* widget_size	  */	0,
    /* class_initialize   */    NULL,
    /* class_part_initialize*/	NULL,
    /* class_inited       */	FALSE,
    /* initialize	  */	NULL,
    /* initialize_hook    */	NULL,		
    /* realize		  */	(XtProc)XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	NULL,
    /* num_resources	  */	0,
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	FALSE,
    /* compress_exposure  */	FALSE,
    /* compress_enterleave*/ 	FALSE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	NULL,
    /* expose		  */	NULL,
    /* set_values	  */	NULL,
    /* set_values_hook    */	NULL,			
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook    */	NULL,			
    /* accept_focus	  */	NULL,
    /* version		  */	XtVersion,
    /* callback_offsets   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry	    */  NULL,
    /* display_accelerator  */	NULL,
    /* extension	    */  NULL
  }
};


externaldef(widgetclassrec) WidgetClassRec widgetClassRec = {
{
    /* superclass         */    (WidgetClass)&unNamedObjClassRec,
    /* class_name         */    "Core",
    /* widget_size        */    sizeof(WidgetRec),
    /* class_initialize   */    NULL,
    /* class_part_initialize*/  CoreClassPartInitialize,
    /* class_inited       */    FALSE,
    /* initialize         */    CoreInitialize,
    /* initialize_hook    */    NULL,
    /* realize            */    CoreRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    FALSE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    FALSE,
    /* visible_interest   */    FALSE,
    /* destroy            */    CoreDestroy,
    /* resize             */    NULL,
    /* expose             */    NULL,
    /* set_values         */    CoreSetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    CoreSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_offsets   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry       */  NULL,
    /* display_accelerator  */  NULL,
    /* extension            */  NULL
  }
};
#ifndef SVR3SHLIB
externaldef (WidgetClass) WidgetClass widgetClass = &widgetClassRec;

externaldef (WidgetClass) WidgetClass coreWidgetClass = &widgetClassRec;
#else
externalref WidgetClass widgetClass;
externalref WidgetClass coreWidgetClass;
#define _XtInherit  (*_libXt__XtInherit)
#endif


/*ARGSUSED*/
static void XtCopyScreen(widget, offset, value)
    Widget      widget;
    int		offset;
    XrmValue    *value;
{
    value->addr = (XPointer)(&widget->core.screen);
}

/*
 * Start of Core methods
 */

static void CoreClassPartInitialize(wc)
    register WidgetClass wc;
{
    /* We don't need to check for null super since we'll get to object
       eventually, and it had better define them!  */

    register WidgetClass super = wc->core_class.superclass;

    if (wc->core_class.realize == XtInheritRealize) {
	wc->core_class.realize = super->core_class.realize;
    }

    if (wc->core_class.accept_focus == XtInheritAcceptFocus) {
	wc->core_class.accept_focus = super->core_class.accept_focus;
    }

    if (wc->core_class.display_accelerator == XtInheritDisplayAccelerator) {
	wc->core_class.display_accelerator = 
		super->core_class.display_accelerator;
    }

    if (wc->core_class.tm_table == (char *) XtInheritTranslations) {
	wc->core_class.tm_table =
		wc->core_class.superclass->core_class.tm_table;
    } else if (wc->core_class.tm_table != NULL) {
	wc->core_class.tm_table =
	      (String)XtParseTranslationTable(wc->core_class.tm_table);
    }

    if (wc->core_class.actions != NULL) {
	Boolean inPlace;

	if (wc->core_class.version == XtVersionDontCheck)
	    inPlace = True;
	else  
	    inPlace = (wc->core_class.version < XtVersion) ? False : True;

	/* Compile the action table into a more efficient form */
        wc->core_class.actions = (XtActionList) _XtInitializeActionData(
	    wc->core_class.actions, wc->core_class.num_actions, inPlace);
    }
}
/* ARGSUSED */
static void CoreInitialize(requested_widget, new_widget, args, num_args)
    Widget   requested_widget;
    register Widget new_widget;
    ArgList args;
    Cardinal *num_args;
{
    XtTranslations save1, save2;
    new_widget->core.window = (Window) NULL;
    new_widget->core.visible = TRUE;
    new_widget->core.event_table = NULL;
    new_widget->core.popup_list = NULL;
    new_widget->core.num_popups = 0;
    new_widget->core.tm.proc_table = NULL;
    new_widget->core.tm.lastEventTime = 0;
    /* magic semi-resource fetched by GetResources */
    save1 = (XtTranslations)new_widget->core.tm.current_state;
    new_widget->core.tm.current_state = NULL;
    save2 = new_widget->core.tm.translations;
    new_widget->core.tm.translations =
	(XtTranslations)new_widget->core.widget_class->core_class.tm_table;
    if (save1)
	_XtMergeTranslations(new_widget, save1, save1->operation);
    if (save2)
	_XtMergeTranslations(new_widget, save2, save2->operation);
}

static void CoreRealize(widget, value_mask, attributes)
    Widget		 widget;
    XtValueMask		 *value_mask;
    XSetWindowAttributes *attributes;
{
    XtCreateWindow(widget, (unsigned int) InputOutput,
	(Visual *) CopyFromParent, *value_mask, attributes);
} /* CoreRealize */

static void CoreDestroy (widget)
     Widget    widget;
{
    _XtFreeEventTable(&widget->core.event_table);
    _XtDestroyTMData(widget);
    _XtUnregisterWindow(widget->core.window, widget);

    if (widget->core.popup_list != NULL)
        XtFree((char *)widget->core.popup_list);

} /* CoreDestroy */

/* ARGSUSED */
static Boolean CoreSetValues(old, reference, new, args, num_args)
    Widget old, reference, new;
    ArgList args;
    Cardinal *num_args;
{
    Boolean redisplay;
    Mask    window_mask;
    XSetWindowAttributes attributes;
    XtTranslations save;

    redisplay = FALSE;
    if  (old->core.tm.translations != new->core.tm.translations) {
	save = new->core.tm.translations;
	new->core.tm.translations = old->core.tm.translations;
	_XtMergeTranslations(new, save, XtTableReplace);
    }       

    /* Check everything that depends upon window being realized */
    if (XtIsRealized(old)) {
	window_mask = 0;
	/* Check window attributes */
	if (old->core.background_pixel != new->core.background_pixel
	    && new->core.background_pixmap == XtUnspecifiedPixmap) {
	   attributes.background_pixel  = new->core.background_pixel;
	   window_mask |= CWBackPixel;
	   redisplay = TRUE;
	}	
	if (old->core.background_pixmap != new->core.background_pixmap) {
	   if (new->core.background_pixmap == XtUnspecifiedPixmap) {
	       window_mask |= CWBackPixel;
	       attributes.background_pixel  = new->core.background_pixel;
	   }
	   else {
	       attributes.background_pixmap = new->core.background_pixmap;
	       window_mask &= ~CWBackPixel;
	       window_mask |= CWBackPixmap;
	   }
	   redisplay = TRUE;
	}	
	if (old->core.border_pixel != new->core.border_pixel
	    && new->core.border_pixmap == XtUnspecifiedPixmap) {
	   attributes.border_pixel  = new->core.border_pixel;
	   window_mask |= CWBorderPixel;
       }
	if (old->core.border_pixmap != new->core.border_pixmap) {
	   if (new->core.border_pixmap == XtUnspecifiedPixmap) {
	       window_mask |= CWBorderPixel;
	       attributes.border_pixel  = new->core.border_pixel;
	   }
	   else {
	       attributes.border_pixmap = new->core.border_pixmap;
	       window_mask &= ~CWBorderPixel;
	       window_mask |= CWBorderPixmap;
	   }
       }
	if (old->core.depth != new->core.depth) {
	   XtAppWarningMsg(XtWidgetToApplicationContext(old),
		    "invalidDepth","setValues",XtCXtToolkitError,
               "Can't change widget depth", (String *)NULL, (Cardinal *)NULL);
	   new->core.depth = old->core.depth;
	}
	if (old->core.colormap != new->core.colormap) {
	    window_mask |= CWColormap;
	    attributes.colormap = new->core.colormap;
	}
	if (window_mask != 0) {
	    /* Actually change X window attributes */
	    XChangeWindowAttributes(
		XtDisplay(new), XtWindow(new), window_mask, &attributes);
	}

	if (old->core.mapped_when_managed != new->core.mapped_when_managed) {
	    Boolean mapped_when_managed = new->core.mapped_when_managed;
	    new->core.mapped_when_managed = !mapped_when_managed;
	    XtSetMappedWhenManaged(new, mapped_when_managed);
	} 
    } /* if realized */

    return redisplay;
} /* CoreSetValues */

/*ARGSUSED*/
static void CoreSetValuesAlmost(old, new, request, reply)
    Widget		old;
    Widget		new;
    XtWidgetGeometry    *request;
    XtWidgetGeometry    *reply;
{
    *request = *reply;
}
