/***********************************************************

 $XConsortium: SimpleP.h,v 1.10 90/03/05 17:47:33 kit Exp $ 

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

#ifndef _SimpleP_h
#define _SimpleP_h

#ifdef VMS
#   include <XAW_DIRECTORY/Simple.h>
#   include <X11_DIRECTORY/CoreP.h>
#else
#   include <X11/Xaw3d/Simple.h>
#   include <X11/CoreP.h>
#endif

typedef struct {
    Boolean	(*change_sensitive)(/* widget */);
} SimpleClassPart;

#define XtInheritChangeSensitive ((Boolean (*)())_XtInherit)

typedef struct _SimpleClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
} SimpleClassRec;

#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_S.H>
#else
#   define _EXTERN_ extern
#endif 

_EXTERN_ SimpleClassRec simpleClassRec;

#undef _EXTERN_
#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_E.H>
#endif

typedef struct {
    /* resources */
    Cursor	cursor;
    Pixmap	insensitive_border;
    String      cursor_name;	/* cursor specified by name. */

    Pixel       pointer_fg, pointer_bg;	/* Pointer colors. */
    /* private state */
} SimplePart;

typedef struct _SimpleRec {
    CorePart	core;
    SimplePart	simple;
} SimpleRec;

#endif /* _SimpleP_h */
