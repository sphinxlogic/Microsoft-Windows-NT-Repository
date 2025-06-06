/* $XFree86: mit/lib/Xaw/AsciiText.c,v 1.3 1993/03/27 09:09:37 dawes Exp $ */
/* $XConsortium: AsciiText.c,v 1.44 91/07/12 11:27:23 converse Exp $ */

/*
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

*/

/***********************************************************************
 *
 * AsciiText Widget
 *
 ***********************************************************************/

/*
 * AsciiText.c - Source code for AsciiText Widget.
 *
 * This Widget is intended to be used as a simple front end to the 
 * text widget with an ascii source and ascii sink attached to it.
 *
 * Date:    June 29, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium 
 *          kit@expo.lcs.mit.edu
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/AsciiTextP.h>
#include <X11/Xaw/AsciiSrc.h>
#include <X11/Xaw/AsciiSink.h>

#define TAB_COUNT 32

static void Initialize(), Destroy();

AsciiTextClassRec asciiTextClassRec = {
  { /* core fields */
#ifndef SVR3SHLIB
    /* superclass       */      (WidgetClass) &textClassRec,
#else
    /* superclass       */      NULL,
#endif
    /* class_name       */      "Text",
    /* widget_size      */      sizeof(AsciiRec),
    /* class_initialize */      XawInitializeWidgetSet,
    /* class_part_init  */	NULL,
    /* class_inited     */      FALSE,
    /* initialize       */      Initialize,
    /* initialize_hook  */	NULL,
    /* realize          */      XtInheritRealize,
    /* actions          */      NULL,
    /* num_actions      */      0,
    /* resources        */      NULL,
    /* num_resource     */      0,
    /* xrm_class        */      NULLQUARK,
    /* compress_motion  */      TRUE,
    /* compress_exposure*/      XtExposeGraphicsExpose | XtExposeNoExpose,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      Destroy,
    /* resize           */      XtInheritResize,
    /* expose           */      XtInheritExpose,
    /* set_values       */      NULL,
    /* set_values_hook  */	NULL,
    /* set_values_almost*/	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */      XtInheritAcceptFocus,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      XtInheritTranslations,
    /* query_geometry	*/	XtInheritQueryGeometry
  },
  { /* Simple fields */
    /* change_sensitive	*/	XtInheritChangeSensitive
  },
  { /* text fields */
    /* empty            */      0
  },
  { /* ascii fields */
    /* empty            */      0
  }
};
#ifndef SVR3SHLIB
WidgetClass asciiTextWidgetClass = (WidgetClass)&asciiTextClassRec;
#else
extern WidgetClass asciiTextWidgetClass;
#endif


static void
Initialize(request, new, args, num_args)
Widget request, new;
ArgList args;
Cardinal *num_args;
{
  AsciiWidget w = (AsciiWidget) new;
  int i;
  int tabs[TAB_COUNT], tab;

  /* superclass Initialize can't set the following,
   * as it didn't know the source or sink when it was called */
  if (request->core.height == DEFAULT_TEXT_HEIGHT)
    new->core.height = DEFAULT_TEXT_HEIGHT;

  w->text.source = XtCreateWidget( "textSource", asciiSrcObjectClass,
				  new, args, *num_args );
  w->text.sink = XtCreateWidget( "textSink", asciiSinkObjectClass,
				new, args, *num_args );

  if (w->core.height == DEFAULT_TEXT_HEIGHT)
    w->core.height = VMargins(w) + XawTextSinkMaxHeight(w->text.sink, 1);

  for (i=0, tab=0 ; i < TAB_COUNT ; i++) 
    tabs[i] = (tab += 8);
  
  XawTextSinkSetTabs(w->text.sink, TAB_COUNT, tabs);

  XawTextDisableRedisplay(new);
  XawTextEnableRedisplay(new);
}

static void 
Destroy(w)
Widget w;
{
    if (w == XtParent(((AsciiWidget)w)->text.source))
	XtDestroyWidget( ((AsciiWidget)w)->text.source );

    if (w == XtParent(((AsciiWidget)w)->text.sink))
	XtDestroyWidget( ((AsciiWidget)w)->text.sink );
}

#ifdef ASCII_STRING

/************************************************************
 *
 * Ascii String Compatibility Code.
 *
 ************************************************************/

AsciiStringClassRec asciiStringClassRec = {
  { /* core fields */
    /* superclass       */      (WidgetClass) &asciiTextClassRec,
    /* class_name       */      "Text",
    /* widget_size      */      sizeof(AsciiStringRec),
    /* class_initialize */      NULL,
    /* class_part_init  */	NULL,
    /* class_inited     */      FALSE,
    /* initialize       */      NULL,
    /* initialize_hook  */	NULL,
    /* realize          */      XtInheritRealize,
    /* actions          */      NULL,
    /* num_actions      */      0,
    /* resources        */      NULL,
    /* num_ resource    */      0,
    /* xrm_class        */      NULLQUARK,
    /* compress_motion  */      TRUE,
    /* compress_exposure*/      XtExposeGraphicsExpose,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      NULL,
    /* resize           */      XtInheritResize,
    /* expose           */      XtInheritExpose,
    /* set_values       */      NULL,
    /* set_values_hook  */	NULL,
    /* set_values_almost*/	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */      XtInheritAcceptFocus,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      XtInheritTranslations,
    /* query_geometry	*/	XtInheritQueryGeometry
  },
  { /* Simple fields */
    /* change_sensitive	*/	XtInheritChangeSensitive
  },
  { /* text fields */
    /* empty            */      0
  },
  { /* ascii fields */
    /* empty            */      0
  }
};

WidgetClass asciiStringWidgetClass = (WidgetClass)&asciiStringClassRec;

#endif /* ASCII_STRING */

#ifdef ASCII_DISK

/************************************************************
 *
 * Ascii Disk Compatibility Code.
 *
 ************************************************************/

AsciiDiskClassRec asciiDiskClassRec = {
  { /* core fields */
    /* superclass       */      (WidgetClass) &asciiTextClassRec,
    /* class_name       */      "Text",
    /* widget_size      */      sizeof(AsciiDiskRec),
    /* class_initialize */      NULL,
    /* class_part_init  */	NULL,
    /* class_inited     */      FALSE,
    /* initialize       */      NULL,
    /* initialize_hook  */	NULL,
    /* realize          */      XtInheritRealize,
    /* actions          */      NULL,
    /* num_actions      */      0,
    /* resources        */      NULL,
    /* num_ resource    */      0,
    /* xrm_class        */      NULLQUARK,
    /* compress_motion  */      TRUE,
    /* compress_exposure*/      XtExposeGraphicsExpose,
    /* compress_enterleave*/	TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      NULL,
    /* resize           */      XtInheritResize,
    /* expose           */      XtInheritExpose,
    /* set_values       */      NULL,
    /* set_values_hook  */	NULL,
    /* set_values_almost*/	XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus     */      XtInheritAcceptFocus,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      XtInheritTranslations,
    /* query_geometry	*/	XtInheritQueryGeometry
  },
  { /* Simple fields */
    /* change_sensitive	*/	XtInheritChangeSensitive
  },
  { /* text fields */
    /* empty            */      0
  },
  { /* ascii fields */
    /* empty            */      0
  }
};

WidgetClass asciiDiskWidgetClass = (WidgetClass)&asciiDiskClassRec;

#endif /* ASCII_DISK */












