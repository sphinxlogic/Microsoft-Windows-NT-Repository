/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
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
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Xos.h>
#include "xpaint.h"
#include "Paint.h"

#define	DELAY	500

typedef struct {
	XtIntervalId	id;
	Boolean		state;
	int		startX, startY, curX, curY, height;
	XFontStruct	*fi;
	int		insertX, insertY, insertSX, insertSY, insertH;
	Widget		w;
	char		*sptr;
	char		str[512];
} LocalInfo;

static void	cursor(LocalInfo *l, Boolean flag)
{
	if (l->w == None)
		return;
	if (flag) {
		XDrawLine(XtDisplay(l->w), XtWindow(l->w), Global.gcx,
				l->insertX, l->insertY, l->insertX, l->insertY + l->insertH);
		l->state = !l->state;
	} else if (l->state) {
		XDrawLine(XtDisplay(l->w), XtWindow(l->w), Global.gcx,
				l->insertX, l->insertY, l->insertX, l->insertY + l->insertH);
		l->state = False;
	}
}

static void	flash(LocalInfo *l)
{
	cursor(l, True);
	l->id = XtAppAddTimeOut(Global.appContext, DELAY, (XtTimerCallbackProc)flash, l);
}

static void	press(Widget w, LocalInfo *l, XButtonEvent *event, OpInfo *info) 
{
	cursor(l, False);

	XtVaGetValues(w, XtNfont, &l->fi, NULL);
	if (l->fi == NULL)
		return;

	l->w      = w;
	l->height = l->fi->ascent + l->fi->descent;
	l->startX = l->curX = info->x;
	l->startY = l->curY = info->y;
	l->curY   = l->curY - l->height / 2;

	if (info->surface == opWindow) {
		l->insertH  = l->height * info->zoom;
		l->insertY  = event->y - l->insertH / 2;
		l->insertX  = event->x;
		l->insertSX = event->x;
	}

	l->sptr = l->str;

	if (l->id == (XtIntervalId)NULL)
		flash(l);

	UndoStartPoint(w, info, l->curX, l->curY);
}

static void	key(Widget w, LocalInfo *l, XKeyEvent *event, OpInfo *info) 
{
	char		buf[21];
	KeySym		keysym;
	int		len, i, width;
	XRectangle	rect;

	if (l->w == None)
		return;

	if ((len = XLookupString(event, buf, sizeof(buf) - 1, &keysym, NULL)) == 0)
		return;

	cursor(l, False);

	for (i = 0; i < len; i++) {
		if (buf[i] == '\n' || buf[i] == '\r') {
			l->curX     = l->startX;
			l->curY    += l->height + 2;
			l->insertX  = l->insertSX;
			l->insertY += l->insertH + 2 * info->zoom;

			*l->sptr++ = '\n';
		} else if (buf[i] == 0x08 || buf[i] == 0x7f) {
			if (l->sptr != l->str) {
				l->sptr--;
				if (*l->sptr == '\n') {
					char	*cp;
					*l->sptr = '\0';
					if ((cp = strrchr(l->str, '\n')) == NULL)
						cp = l->str;
					else
						cp++;
					width = XTextWidth(l->fi, cp, strlen(cp));
					l->curX = l->startX + width;
					l->curY    -= l->height + 2;
					l->insertX = (l->startX + width) * info->zoom;
					l->insertY -= l->insertH + 2 * info->zoom;
				} else {
					width = XTextWidth(l->fi, l->sptr, 1);
					l->curX -= width;
					l->insertX -= width * info->zoom;

					/*
					**  Now undraw the character.
					*/
					XYtoRECT(l->curX, l->curY, l->curX + width, l->curY + l->height, &rect);
					PwUpdateFromLast(w, &rect);
				}
			}
		} else {
			if (!info->isFat)
				XDrawString(XtDisplay(w), XtWindow(w), info->filled_gc, 
					l->curX, l->curY + l->fi->ascent, &buf[i], 1);
			XDrawString(XtDisplay(w), info->drawable, info->filled_gc, 
					l->curX, l->curY + l->fi->ascent, &buf[i], 1);
			width = XTextWidth(l->fi, &buf[i], 1);

			XYtoRECT(l->curX, l->curY, l->curX + width, l->curY + l->height, &rect);
			PwUpdate(w, &rect, False);

			l->curX    += width;
			l->insertX += width * info->zoom;

			UndoGrow(w, l->curX, l->curY + l->height);
			UndoGrow(w, l->curX, l->curY + l->height);

			*l->sptr++ = buf[i];
		}
	}
}

/*
**  Those public functions
*/
void *FontAdd(Widget w)
{
	LocalInfo	*l = (LocalInfo*)XtMalloc(sizeof(LocalInfo));
	Cursor		c;

	l->id = (XtIntervalId)NULL;
	l->w  = None;

	OpAddEventHandler(w, opPixmap|opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpAddEventHandler(w, opPixmap, KeyPressMask, FALSE, (OpEventProc)key, l);

	c = XCreateFontCursor(XtDisplay(w), XC_xterm);
	XDefineCursor(XtDisplay(w), XtWindow(w), c);

	return l;
}
void FontRemove(Widget w, LocalInfo *l)
{
	OpRemoveEventHandler(w, opPixmap|opWindow, ButtonPressMask, FALSE, (OpEventProc)press, l);
	OpRemoveEventHandler(w, opPixmap, KeyPressMask, FALSE, (OpEventProc)key, l);

	if (l->id != (XtIntervalId)NULL)
		XtRemoveTimeOut(l->id);

	cursor(l, False);

	XtFree((XtPointer)l);
}
