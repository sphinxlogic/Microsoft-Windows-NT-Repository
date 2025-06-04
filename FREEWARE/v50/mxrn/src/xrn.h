#ifndef XRN_H
#define XRN_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/xrn.h,v 1.5 1993/01/11 02:15:51 ricks Exp $
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

#ifndef MOTIF
#define XRN_VERSION "6.18-32A (for XUI)"
#define TITLE "dxrn 6.18-32A"
#define UNREADTITLE "dxrn 6.18-32A (New News)"
#else
#define XRN_VERSION "6.18-32A (for Motif)"
#define TITLE "mxrn 6.18-32A"
#define UNREADTITLE "mxrn 6.18-32A (New News)"
#endif

#ifndef VMS
#include <X11/Intrinsic.h>
#else
#include <decw$include/Intrinsic.h>
#endif

/*
 * xrn.h: set up the main screens
 *
 */

/* global variables that represent the widgets that are dynamically changed */

extern Widget TopLevel;
extern Widget MainWindow;	/* Main window widget */
extern Widget Frame;
extern Widget MenuBar;		/* Top menu bar */
extern Widget TopButtonBox;	/* button box containing the command buttons */
extern Widget BottomButtonBox;	/* button box containing the article specific buttons */
extern Widget TopInfoLine;      /* top button info line                      */
extern Widget BottomInfoLine;   /* bottom button info line                   */
extern Widget TopList;		/* scrollable list window                    */
extern Widget ArticleText;
extern Widget PopupMenu;	/* the Popup */
extern Widget ArtTopLevel;	/* Article window */
extern Widget ArtWindow;	/* Article window paned widget */
extern Widget ArtMainWindow;	/* Article Main window widget */
extern Widget ComposeTopLevel;	/* composition frame */
extern Widget ComposeText;	/* composition article */
extern Widget HeaderText;	/* composition header */
#ifdef TITLEBAR
extern Widget TitleBar;
#endif

extern int XRNState;

extern int inCommand;		/* executing a button function		     */

#define XRN_X_UP    0x01
#define XRN_NEWS_UP 0x10

#define LABEL_SIZE 512

#endif /* XRN_H */
