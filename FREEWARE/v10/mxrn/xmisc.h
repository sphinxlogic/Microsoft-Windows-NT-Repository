#ifndef XMISC_H
#define XMISC_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xmisc.h,v 1.5 1993/01/11 02:15:47 ricks Exp $
 */

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * xmisc.h: routines for handling miscellaneous x functions
 *
 */

/* create the icon and the handlers for it */
extern void xmIconCreate();

/* set icon name and pixmap */
typedef enum {InitIcon, ReadIcon, UnreadIcon, BusyIcon, NewGroupsIcon,
	      LostIcon, PrevIcon} IconType;
extern void xmSetIconAndName _ARGUMENTS((IconType));

extern void unbusyCursor();
extern void busyCursor();

/* suitable for call back use */
extern void CBunbusyCursor _ARGUMENTS((Widget, caddr_t, caddr_t));
extern void CBbusyCursor _ARGUMENTS((Widget, caddr_t, caddr_t));

/* find out the number of lines in the article display */
extern int articleLines();

/* Word wrap a motif widget */
#if defined(MOTIF)
extern char * xmWrapTextWidget _ARGUMENTS((Widget));
#endif

#endif /* XMISC_H */
