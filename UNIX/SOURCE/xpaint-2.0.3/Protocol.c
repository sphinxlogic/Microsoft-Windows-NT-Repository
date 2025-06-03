/* +-------------------------------------------------------------------+ */
/* | Copyright 1993, David Koblas (koblas@netcom.com)                  | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

typedef void	(*func_t)(Widget, void *, XEvent *);

typedef struct {
	Boolean	flag;
	void	*data;
	func_t	func;
} LocalInfo;

static Atom	delwin = None, protocols = None;

static void handler(Widget w, XtPointer lArg, XEvent *eventArg, Boolean *junk)
{
	XClientMessageEvent	*event = (XClientMessageEvent*)eventArg;
	LocalInfo		*l = (LocalInfo *)lArg;
	if (event->type != ClientMessage)
		return;

	if (event->message_type == protocols && event->data.l[0] == delwin)
		l->func(w, l->data, eventArg);
}

static void realize(Widget w, XtPointer ldataArg, XEvent *event, Boolean *junk)
{
	LocalInfo	*ldata = (LocalInfo *)ldataArg;

	if (event->type == MapNotify && !ldata->flag) {
		XtAddRawEventHandler(w, 0, True, handler, ldataArg);
		XChangeProperty(XtDisplay(w), XtWindow(w), protocols, 
				XA_ATOM, 32, PropModeReplace, 
				(unsigned char *) &delwin, 1);
		ldata->flag = True;
	}
}

void AddDestroyCallback(Widget w, func_t func, void *data)
{
	LocalInfo 	*l;

	if (delwin == None)
		delwin = XInternAtom(XtDisplay(w), "WM_DELETE_WINDOW", FALSE);
	if (protocols == None)
		protocols = XInternAtom(XtDisplay(w), "WM_PROTOCOLS", FALSE);

	if (delwin == None || protocols == None)
		return;

	l = XtNew(LocalInfo);
	l->data = data;
	l->func = func;
	l->flag = False;

	if (!XtIsRealized(w)) {
		XtAddEventHandler(w, StructureNotifyMask, False, realize, (XtPointer)l);
	} else {
		XtAddRawEventHandler(w, 0, True, handler, (XtPointer)l);
		XChangeProperty(XtDisplay(w), XtWindow(w), protocols, 
				XA_ATOM, 32, PropModeReplace, 
				(unsigned char *) &delwin, 1);
		l->flag = True;
	}
}
