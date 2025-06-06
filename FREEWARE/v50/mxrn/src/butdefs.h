#ifndef BUTDEFS_H
#define BUTDEFS_H

/*
 * $Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/butdefs.h,v 1.3 1993/01/11 02:14:16 ricks Exp $
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
 * butdefs.h: define buttons
 *
 */

#undef lint
#ifdef lint
#define BUTTON(nm)
#else
#if defined(__STDC__) && !defined(UNIXCPP)
#define BUTTON(nm)				\
void nm##Function();				\
static void nm##Action(widget, event, string, count) \
Widget widget;					\
XEvent *event;					\
String *string;					\
Cardinal *count;				\
{						\
    if (inCommand) {				\
	XBell(XtDisplay(TopLevel), 0);		\
	return;					\
    }						\
    inCommand = 1;				\
    removeTimeOut();				\
    busyCursor();				\
    nm##Function(widget, event, string, count);	\
    unbusyCursor();				\
    addTimeOut();				\
    inCommand = 0;				\
    return;					\
}						\
/*ARGSUSED*/                                    \
static void nm##Button(widget, client_data, call_data) \
Widget widget;					\
caddr_t client_data;				\
caddr_t call_data;				\
{						\
    nm##Action(widget, NULL, 0, 0);		\
    return;					\
}						\
externaldef (nm##callbacks) XtCallbackRec nm##Callbacks[] = {	\
    {(XtCallbackProc)nm##Button, NULL},		\
    {NULL, NULL}				\
};						\
static Arg nm##Args[] = {			\
    {XtNname,		(XtArgVal) #nm},	\
    {XmNlabelString,	(XtArgVal) NULL},	\
    {XmNactivateCallback, (XtArgVal) nm##Callbacks},\
    {XmNborderWidth,	(XtArgVal) 1},		\
};
#else
#define BUTTON(nm)				\
void nm/**/Function();				\
static void nm/**/Action(widget, event, string, count) \
Widget widget;					\
XEvent *event;					\
String *string;					\
Cardinal *count;				\
{						\
    if (inCommand) {				\
	XBell(XtDisplay(TopLevel), 0);		\
	return;					\
    }						\
    inCommand = 1;				\
    removeTimeOut();				\
    busyCursor();				\
    nm/**/Function(widget, event, string, count); \
    unbusyCursor();				\
    addTimeOut();				\
    inCommand = 0;				\
    return;					\
}						\
/*ARGSUSED*/					\
static void nm/**/Button(widget, client_data, call_data)	\
Widget widget;					\
caddr_t client_data;				\
caddr_t call_data;				\
{						\
    nm/**/Action(widget, NULL, 0, 0);		\
    return;					\
}						\
externaldef (nm/**/callbacks) XtCallbackRec nm/**/Callbacks[] = {	\
    {(XtCallbackProc)nm/**/Button, NULL},	\
    {NULL, NULL}				\
};						\
static Arg nm/**/Args[] = {			\
    {XtNname,		(XtArgVal) "nm"},	\
    {XmNlabelString,	(XtArgVal) NULL},	\
    {XmNactivateCallback, (XtArgVal) nm/**/Callbacks},\
    {XmNborderWidth,	(XtArgVal) 1},		\
};
#endif
#endif

#undef lint
#ifdef lint
#define BUTTON(nm)
#else
#if defined(__STDC__) && !defined(UNIXCPP)
#define intBUTTON(nm)			\
void nm##Function();				\
/*ARGSUSED*/					\
static void nm##Action(widget, event, string, count) \
Widget widget;					\
XEvent *event;					\
String *string;					\
Cardinal *count;				\
{						\
    nm##Function(widget, event, string, count);	\
    return;					\
}						\
externaldef (nm##callbacks) XtCallbackRec nm##Callbacks[] = {	\
    {(XtCallbackProc)nm##Function, NULL},	\
    {NULL, NULL}				\
};						\
static Arg nm##Args[] = {			\
    {XtNname,		(XtArgVal) #nm},	\
    {XmNlabelString,	(XtArgVal) NULL},	\
    {XmNactivateCallback, (XtArgVal) nm##Callbacks},\
    {XmNborderWidth,	(XtArgVal) 1},		\
};
#else
#define intBUTTON(nm)				\
void nm/**/Function();				\
/*ARGSUSED*/					\
static void nm/**/Action(widget, event, string, count) \
Widget widget;					\
XEvent *event;					\
String *string;					\
Cardinal *count;				\
{						\
    nm/**/Function(widget, event, string, count); \
    return;					\
}						\
externaldef (nm/**/callbacks) XtCallbackRec nm/**/Callbacks[] = {	\
    {(XtCallbackProc)nm/**/Function, NULL},	\
    {NULL, NULL}				\
};						\
static Arg nm/**/Args[] = {			\
    {XtNname,		(XtArgVal) "nm"},	\
    {XmNlabelString, (XtArgVal) NULL},		\
    {XmNactivateCallback, (XtArgVal) nm/**/Callbacks},\
    {XmNborderWidth,	(XtArgVal) 1},		\
};
#endif
#endif

#endif /* BUTDEFS_H */
