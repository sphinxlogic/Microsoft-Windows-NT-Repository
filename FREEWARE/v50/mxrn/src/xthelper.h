#ifndef XTHELPER_H
#define XTHELPER_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xthelper.h,v 1.4 1993/01/11 02:15:57 ricks Exp $
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
 * xthelper.h: routines for simplifying the use of the X toolkit
 *
 */

extern void xthCenterWidgetOverCursor _ARGUMENTS((Widget));
extern void xthCenterWidget _ARGUMENTS((Widget, int, int));
extern void xthHandleAllPendingEvents();
#ifdef MOTIF
extern int xthTextWidth _ARGUMENTS((Widget));
extern void fitFrameToContents _ARGUMENTS((Widget, Widget));
#endif
extern void xthUnmapWidget _ARGUMENTS((Widget));
extern void xthTranslateGeometry ();
#endif /* XTHELPER_H */
