/*
 * $XConsortium: AsciiTextP.h,v 1.15 89/07/17 18:09:37 kit Exp $ 
 */

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

/***********************************************************************
 *
 * AsciiText Widget
 *
 ***********************************************************************/

/*
 * AsciiText.c - Private header file for AsciiText Widget.
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

#ifndef _AsciiTextP_h
#define _AsciiTextP_h

#ifdef VMS
#   include <XAW_DIRECTORY/TextP.h>
#   include <XAW_DIRECTORY/AsciiSrc.h> /* no need to get private header. */
#   include <XAW_DIRECTORY/AsciiText.h>
#else
#   include <X11/Xaw3d/TextP.h>
#   include <X11/Xaw3d/AsciiSrc.h> /* no need to get private header. */
#   include <X11/Xaw3d/AsciiText.h>
#endif

typedef struct {int empty;} AsciiClassPart;

typedef struct _AsciiTextClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    TextClassPart	text_class;
    AsciiClassPart	ascii_class;
} AsciiTextClassRec;

#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_S.H>
#else
#   define _EXTERN_ extern
#endif 

_EXTERN_ AsciiTextClassRec asciiTextClassRec;

#undef _EXTERN_
#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_E.H>
#endif 

typedef struct { char foo; /* keep compiler happy. */ } AsciiPart;

typedef struct _AsciiRec {
    CorePart		core;
    SimplePart		simple;
    TextPart		text;
    AsciiPart		ascii;
} AsciiRec;

/************************************************************
 *
 * Ascii String Emulation widget.
 *
 ************************************************************/ 

#ifdef ASCII_STRING

typedef struct {int empty;} AsciiStringClassPart;

typedef struct _AsciiStringClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    TextClassPart	text_class;
    AsciiClassPart	ascii_class;
    AsciiStringClassPart string_class;
} AsciiStringClassRec;

#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_S.H>
#else
#   define _EXTERN_ extern
#endif 

_EXTERN_ AsciiStringClassRec asciiStringClassRec;

#undef _EXTERN_
#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_E.H>
#endif 

typedef struct { char foo; /* keep compiler happy. */ } AsciiStringPart;

typedef struct _AsciiStringRec {
    CorePart		core;
    SimplePart		simple;
    TextPart		text;
    AsciiPart           ascii;
    AsciiStringPart     ascii_str;
} AsciiStringRec;

#endif /* ASCII_STRING */

#ifdef ASCII_DISK

/************************************************************
 *
 * Ascii Disk Emulation widget.
 *
 ************************************************************/ 

typedef struct {int empty;} AsciiDiskClassPart;

typedef struct _AsciiDiskClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    TextClassPart	text_class;
    AsciiClassPart	ascii_class;
    AsciiDiskClassPart	disk_class;
} AsciiDiskClassRec;


#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_S.H>
#else
#   define _EXTERN_ extern
#endif 

_EXTERN_ AsciiDiskClassRec asciiDiskClassRec;

#undef _EXTERN_
#ifdef VMS  /*pragma ###jp### 06/18/95 */
#   include <XAW_DIRECTORY/Xaw3D_VMS_PRAGMA_E.H>
#endif 

typedef struct { char foo; /* keep compiler happy. */ } AsciiDiskPart;

typedef struct _AsciiDiskRec {
    CorePart		core;
    SimplePart		simple;
    TextPart		text;
    AsciiPart           ascii;
    AsciiDiskPart       ascii_disk;
} AsciiDiskRec;
#endif /* ASCII_DISK */

#endif /* _AsciiTextP_h */
