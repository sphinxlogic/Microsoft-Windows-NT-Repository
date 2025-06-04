/*
 * $XConsortium: Layout.h,v 1.2 92/01/22 18:03:05 keith Exp $
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

#ifndef _Layout_h
#define _Layout_h

#ifdef VMS
#   include <X11_DIRECTORY/Constraint.h>
#   include <X11_DIRECTORY/Xfuncproto.h>
#else
#   include <X11/Constraint.h>
#   include <X11/Xfuncproto.h>
#endif

/****************************************************************
 *
 * Layout Widget (SubClass of CompositeClass)
 *
 ****************************************************************/

/* RESOURCES:

 Name		         Class		   RepType	    Default Value
 ----		         -----		   -------	    -------------
 background	         Background	   Pixel	    XtDefaultBackground
 border		         BorderColor       Pixel	    XtDefaultForeground
 borderWidth	         BorderWidth       Dimension	    1
 cursor		         Cursor	           Cursor	    None
 destroyCallback         Callback	   Pointer	    NULL
 height		         Height	           Dimension	    0
 mappedWhenManaged       MappedWhenManaged Boolean	    True
 sensitive	         Sensitive	   Boolean	    True
 width		         Width	           Dimension	    0
 x		         Position	   Position	    0
 y		         Position	   Position    	    0
 layout			 Layout		   Layout	    NULL
 debug			 Boolean	   Boolean	    False
 resize			 Boolean	   Boolean	    True ###jp### 11/94
 maximumWidth	         MaximumWidth	   Dimension	    0    ###jp### 11/94
 maximumHeight		 MaximumHeight	   Dimension	    0    ###jp### 11/94
 minimumWidth	         MinimumWidth	   Dimension	    0    ###jp### 11/94
 minimumHeight		 MinimumHeight	   Dimension	    0    ###jp### 11/94

*/

/*
     CONSTRAINT RESOURCES: ###jp### 11/94

 Name                    Class             RepType          Default Value
 ----                    -----             -------          -------------
 resizable               Boolean           Boolean          True

*/
  
/* New Fields */
#define XtNlayout		"layout"
#define XtCLayout		"Layout"
#define XtRLayout		"Layout"
#define XtNdebug		"debug"
#define XtNresizable		"resizable"
#define XtNresizeWidth		"resizeWidth"
#define XtNresizeHeight		"resizeHeight"
#define XtNconditionedResize	"conditionedResize"
#define XtNmaximumWidth		"maximumWidth"
#define XtCMaximumWidth		"MaximumWidth"
#define XtNmaximumHeight	"maximumHeight"
#define XtCMaximumHeight	"MaximumHeight"
#define XtNminimumWidth		"minimumWidth"
#define XtCMinimumWidth		"MinimumWidth"
#define XtNminimumHeight	"minimumHeight"
#define XtCMinimumHeight	"MinimumHeight"

/* Class record constant */

#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_S.H>
#else
#   define _EXTERN_ extern
#endif 

_EXTERN_ WidgetClass layoutWidgetClass;

#undef _EXTERN_
#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_E.H>
#endif

typedef struct _LayoutClassRec	*LayoutWidgetClass;
typedef struct _LayoutRec	*LayoutWidget;

_XFUNCPROTOBEGIN

extern void LayoutWidgetGetNaturalSize (
#if NeedFunctionPrototypes
    LayoutWidget	/* l */         ,
    Dimension*	        /* widthp */    ,
    Dimension*          /* heightp */
#endif
);

extern void LayoutWidgetAllowResize (
#if NeedFunctionPrototypes
    LayoutWidget	   /* l */      ,
#   if NeedWidePrototypes
       int                 /* wflag */  ,
       int                 /* hflag */
#   else
       Boolean             /* wflag */  ,
       Boolean             /* hflag */
#   endif
#endif
);

extern void LayoutWidgetSetSizeBounds (
#if NeedFunctionPrototypes
    LayoutWidget	/* l */         ,
    Dimension*	        /* minw_p */    ,
    Dimension*          /* minh_p */    ,
    Dimension*	        /* maxw_p */    ,
    Dimension*          /* maxh_p */
#endif
);

extern void LayoutWidgetGetSizeBounds (
#if NeedFunctionPrototypes
    LayoutWidget	/* l */         ,
    Dimension*	        /* minw_p */    ,
    Dimension*          /* minh_p */    ,
    Dimension*	        /* maxw_p */    ,
    Dimension*          /* maxh_p */
#endif
);

_XFUNCPROTOEND   

#endif /* _Layout_h */
