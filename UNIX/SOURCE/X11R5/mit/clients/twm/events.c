/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**  Portions Copyright 1989 by the Massachusetts Institute of Technology   **/
/**                        Cambridge, Massachusetts                         **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    names of Evans & Sutherland and M.I.T. not be used in advertising    **/
/**    in publicity pertaining to distribution of the  software  without    **/
/**    specific, written prior permission.                                  **/
/**                                                                         **/
/**    EVANS & SUTHERLAND AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD    **/
/**    TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES  OF  MERCHANT-    **/
/**    ABILITY  AND  FITNESS,  IN  NO  EVENT SHALL EVANS & SUTHERLAND OR    **/
/**    M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL  DAM-    **/
/**    AGES OR  ANY DAMAGES WHATSOEVER  RESULTING FROM LOSS OF USE, DATA    **/
/**    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER    **/
/**    TORTIOUS ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE    **/
/**    OR PERFORMANCE OF THIS SOFTWARE.                                     **/
/*****************************************************************************/


/***********************************************************************
 *
 * $XFree86: mit/clients/twm/events.c,v 1.2 1993/03/20 06:05:33 dawes Exp $
 * $XConsortium: events.c,v 1.182 91/07/17 13:59:14 dave Exp $
 *
 * twm event handling
 *
 * 17-Nov-87 Thomas E. LaStrange		File created
 *
 ***********************************************************************/

#include <stdio.h>
#include "twm.h"
#include <X11/Xatom.h>
#include "add_window.h"
#include "menus.h"
#include "events.h"
#include "resize.h"
#include "parse.h"
#include "gram.h"
#include "util.h"
#include "screen.h"
#include "iconmgr.h"
#include "version.h"

extern int iconifybox_width, iconifybox_height;
extern unsigned int mods_used;
extern int menuFromFrameOrWindowOrTitlebar;

#define MAX_X_EVENT 256
event_proc EventHandler[MAX_X_EVENT]; /* event handler jump table */
char *Action;
int Context = C_NO_CONTEXT;	/* current button press context */
TwmWindow *ButtonWindow;	/* button press window structure */
XEvent ButtonEvent;		/* button press event */
XEvent Event;			/* the current event */
TwmWindow *Tmp_win;		/* the current twm window */

/* Used in HandleEnterNotify to remove border highlight from a window 
 * that has not recieved a LeaveNotify event because of a pointer grab 
 */
TwmWindow *UnHighLight_win = NULL;

Window DragWindow;		/* variables used in moving windows */
int origDragX;
int origDragY;
int DragX;
int DragY;
int DragWidth;
int DragHeight;
int CurrentDragX;
int CurrentDragY;

/* Vars to tell if the resize has moved. */
extern int ResizeOrigX;
extern int ResizeOrigY;

static int enter_flag;
static int ColortableThrashing;
static TwmWindow *enter_win, *raise_win;

ScreenInfo *FindScreenInfo();
int ButtonPressed = -1;
int Cancel = FALSE;

void HandleCreateNotify();

void HandleShapeNotify ();
extern int ShapeEventBase, ShapeErrorBase;

void AutoRaiseWindow (tmp)
    TwmWindow *tmp;
{
    XRaiseWindow (dpy, tmp->frame);
    XSync (dpy, 0);
    enter_win = NULL;
    enter_flag = TRUE;
    raise_win = tmp;
}

void SetRaiseWindow (tmp)
    TwmWindow *tmp;
{
    enter_flag = TRUE;
    enter_win = NULL;
    raise_win = tmp;
    XSync (dpy, 0);
}



/***********************************************************************
 *
 *  Procedure:
 *	InitEvents - initialize the event jump table
 *
 ***********************************************************************
 */

void
InitEvents()
{
    int i;


    ResizeWindow = 0;
    DragWindow = 0;
    enter_flag = FALSE;
    enter_win = raise_win = NULL;

    for (i = 0; i < MAX_X_EVENT; i++)
	EventHandler[i] = HandleUnknown;

    EventHandler[Expose] = HandleExpose;
    EventHandler[CreateNotify] = HandleCreateNotify;
    EventHandler[DestroyNotify] = HandleDestroyNotify;
    EventHandler[MapRequest] = HandleMapRequest;
    EventHandler[MapNotify] = HandleMapNotify;
    EventHandler[UnmapNotify] = HandleUnmapNotify;
    EventHandler[MotionNotify] = HandleMotionNotify;
    EventHandler[ButtonRelease] = HandleButtonRelease;
    EventHandler[ButtonPress] = HandleButtonPress;
    EventHandler[EnterNotify] = HandleEnterNotify;
    EventHandler[LeaveNotify] = HandleLeaveNotify;
    EventHandler[ConfigureRequest] = HandleConfigureRequest;
    EventHandler[ClientMessage] = HandleClientMessage;
    EventHandler[PropertyNotify] = HandlePropertyNotify;
    EventHandler[KeyPress] = HandleKeyPress;
    EventHandler[ColormapNotify] = HandleColormapNotify;
    EventHandler[VisibilityNotify] = HandleVisibilityNotify;
    if (HasShape)
	EventHandler[ShapeEventBase+ShapeNotify] = HandleShapeNotify;
}




Time lastTimestamp = CurrentTime;	/* until Xlib does this for us */

Bool StashEventTime (ev)
    register XEvent *ev;
{
    switch (ev->type) {
      case KeyPress:
      case KeyRelease:
	lastTimestamp = ev->xkey.time;
	return True;
      case ButtonPress:
      case ButtonRelease:
	lastTimestamp = ev->xbutton.time;
	return True;
      case MotionNotify:
	lastTimestamp = ev->xmotion.time;
	return True;
      case EnterNotify:
      case LeaveNotify:
	lastTimestamp = ev->xcrossing.time;
	return True;
      case PropertyNotify:
	lastTimestamp = ev->xproperty.time;
	return True;
      case SelectionClear:
	lastTimestamp = ev->xselectionclear.time;
	return True;
      case SelectionRequest:
	lastTimestamp = ev->xselectionrequest.time;
	return True;
      case SelectionNotify:
	lastTimestamp = ev->xselection.time;
	return True;
    }
    return False;
}



/*
 * WindowOfEvent - return the window about which this event is concerned; this
 * window may not be the same as XEvent.xany.window (the first window listed
 * in the structure).
 */
Window WindowOfEvent (e)
    XEvent *e;
{
    /*
     * Each window subfield is marked with whether or not it is the same as
     * XEvent.xany.window or is different (which is the case for some of the
     * notify events).
     */
    switch (e->type) {
      case KeyPress:
      case KeyRelease:  return e->xkey.window;			     /* same */
      case ButtonPress:
      case ButtonRelease:  return e->xbutton.window;		     /* same */
      case MotionNotify:  return e->xmotion.window;		     /* same */
      case EnterNotify:
      case LeaveNotify:  return e->xcrossing.window;		     /* same */
      case FocusIn:
      case FocusOut:  return e->xfocus.window;			     /* same */
      case KeymapNotify:  return e->xkeymap.window;		     /* same */
      case Expose:  return e->xexpose.window;			     /* same */
      case GraphicsExpose:  return e->xgraphicsexpose.drawable;	     /* same */
      case NoExpose:  return e->xnoexpose.drawable;		     /* same */
      case VisibilityNotify:  return e->xvisibility.window;	     /* same */
      case CreateNotify:  return e->xcreatewindow.window;	     /* DIFF */
      case DestroyNotify:  return e->xdestroywindow.window;	     /* DIFF */
      case UnmapNotify:  return e->xunmap.window;		     /* DIFF */
      case MapNotify:  return e->xmap.window;			     /* DIFF */
      case MapRequest:  return e->xmaprequest.window;		     /* DIFF */
      case ReparentNotify:  return e->xreparent.window;		     /* DIFF */
      case ConfigureNotify:  return e->xconfigure.window;	     /* DIFF */
      case ConfigureRequest:  return e->xconfigurerequest.window;    /* DIFF */
      case GravityNotify:  return e->xgravity.window;		     /* DIFF */
      case ResizeRequest:  return e->xresizerequest.window;	     /* same */
      case CirculateNotify:  return e->xcirculate.window;	     /* DIFF */
      case CirculateRequest:  return e->xcirculaterequest.window;    /* DIFF */
      case PropertyNotify:  return e->xproperty.window;		     /* same */
      case SelectionClear:  return e->xselectionclear.window;	     /* same */
      case SelectionRequest: return e->xselectionrequest.requestor;  /* DIFF */
      case SelectionNotify:  return e->xselection.requestor;	     /* same */
      case ColormapNotify:  return e->xcolormap.window;		     /* same */
      case ClientMessage:  return e->xclient.window;		     /* same */
      case MappingNotify:  return None;
    }
    return None;
}



/***********************************************************************
 *
 *  Procedure:
 *	DispatchEvent2 - 
 *      handle a single X event stored in global var Event
 *      this rouitine for is for a call during an f.move
 *
 ***********************************************************************
 */
Bool DispatchEvent2 ()
{
    Window w = Event.xany.window;
    StashEventTime (&Event);

    if (XFindContext (dpy, w, TwmContext, (caddr_t *) &Tmp_win) == XCNOENT)
      Tmp_win = NULL;

    if (XFindContext (dpy, w, ScreenContext, (caddr_t *)&Scr) == XCNOENT) {
	Scr = FindScreenInfo (WindowOfEvent (&Event));
    }

    if (!Scr) return False;

    if (menuFromFrameOrWindowOrTitlebar && Event.type == Expose)
      HandleExpose();

    if (!menuFromFrameOrWindowOrTitlebar && Event.type>= 0 && Event.type < MAX_X_EVENT) {
	(*EventHandler[Event.type])();
    }

    return True;
}

/***********************************************************************
 *
 *  Procedure:
 *	DispatchEvent - handle a single X event stored in global var Event
 *
 ***********************************************************************
 */
Bool DispatchEvent ()
{
    Window w = Event.xany.window;
    StashEventTime (&Event);

    if (XFindContext (dpy, w, TwmContext, (caddr_t *) &Tmp_win) == XCNOENT)
      Tmp_win = NULL;

    if (XFindContext (dpy, w, ScreenContext, (caddr_t *)&Scr) == XCNOENT) {
	Scr = FindScreenInfo (WindowOfEvent (&Event));
    }

    if (!Scr) return False;

    if (Event.type>= 0 && Event.type < MAX_X_EVENT) {
	(*EventHandler[Event.type])();
    }

    return True;
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleEvents - handle X events
 *
 ***********************************************************************
 */

void
HandleEvents()
{
    while (TRUE)
    {
	if (enter_flag && !QLength(dpy)) {
	    if (enter_win && enter_win != raise_win) {
		AutoRaiseWindow (enter_win);  /* sets enter_flag T */
	    } else {
		enter_flag = FALSE;
	    }
	}
	if (ColortableThrashing && !QLength(dpy) && Scr) {
	    InstallWindowColormaps(ColormapNotify, (TwmWindow *) NULL);
	}
	WindowMoved = FALSE;
	XNextEvent(dpy, &Event);
	(void) DispatchEvent ();
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleColormapNotify - colormap notify event handler
 *
 * This procedure handles both a client changing its own colormap, and
 * a client explicitly installing its colormap itself (only the window
 * manager should do that, so we must set it correctly).
 *
 ***********************************************************************
 */

void
HandleColormapNotify()
{
    XColormapEvent *cevent = (XColormapEvent *) &Event;
    ColormapWindow *cwin, **cwins;
    TwmColormap *cmap;
    int lost, won, n, number_cwins;
    extern TwmColormap *CreateTwmColormap();

    if (XFindContext(dpy, cevent->window, ColormapContext, (caddr_t *)&cwin) == XCNOENT)
	return;
    cmap = cwin->colormap;

    if (cevent->new)
    {
	if (XFindContext(dpy, cevent->colormap, ColormapContext,
			 (caddr_t *)&cwin->colormap) == XCNOENT)
	    cwin->colormap = CreateTwmColormap(cevent->colormap);
	else
	    cwin->colormap->refcnt++;

	cmap->refcnt--;

	if (cevent->state == ColormapUninstalled)
	    cmap->state &= ~CM_INSTALLED;
	else
	    cmap->state |= CM_INSTALLED;

	if (cmap->state & CM_INSTALLABLE)
	    InstallWindowColormaps(ColormapNotify, (TwmWindow *) NULL);

	if (cmap->refcnt == 0)
	{
	    XDeleteContext(dpy, cmap->c, ColormapContext);
	    free((char *) cmap);
	}

	return;
    }

    if (cevent->state == ColormapUninstalled &&
	(cmap->state & CM_INSTALLABLE))
    {
	if (!(cmap->state & CM_INSTALLED))
	    return;
	cmap->state &= ~CM_INSTALLED;

	if (!ColortableThrashing)
	{
	    ColortableThrashing = TRUE;
	    XSync(dpy, 0);
	}

	if (cevent->serial >= Scr->cmapInfo.first_req)
	{
	    number_cwins = Scr->cmapInfo.cmaps->number_cwins;

	    /*
	     * Find out which colortables collided.
	     */

	    cwins = Scr->cmapInfo.cmaps->cwins;
	    for (lost = won = -1, n = 0;
		 (lost == -1 || won == -1) && n < number_cwins;
		 n++)
	    {
		if (lost == -1 && cwins[n] == cwin)
		{
		    lost = n;	/* This is the window which lost its colormap */
		    continue;
		}

		if (won == -1 &&
		    cwins[n]->colormap->install_req == cevent->serial)
		{
		    won = n;	/* This is the window whose colormap caused */
		    continue;	/* the de-install of the previous colormap */
		}
	    }

	    /*
	    ** Cases are:
	    ** Both the request and the window were found:
	    **		One of the installs made honoring the WM_COLORMAP
	    **		property caused another of the colormaps to be
	    **		de-installed, just mark the scoreboard.
	    **
	    ** Only the request was found:
	    **		One of the installs made honoring the WM_COLORMAP
	    **		property caused a window not in the WM_COLORMAP
	    **		list to lose its map.  This happens when the map
	    **		it is losing is one which is trying to be installed,
	    **		but is getting getting de-installed by another map
	    **		in this case, we'll get a scoreable event later,
	    **		this one is meaningless.
	    **
	    ** Neither the request nor the window was found:
	    **		Somebody called installcolormap, but it doesn't
	    **		affect the WM_COLORMAP windows.  This case will
	    **		probably never occur.
	    **
	    ** Only the window was found:
	    **		One of the WM_COLORMAP windows lost its colormap
	    **		but it wasn't one of the requests known.  This is
	    **		probably because someone did an "InstallColormap".
	    **		The colormap policy is "enforced" by re-installing
	    **		the colormaps which are believed to be correct.
	    */

	    if (won != -1)
		if (lost != -1)
		{
		    /* lower diagonal index calculation */
		    if (lost > won)
			n = lost*(lost-1)/2 + won;
		    else
			n = won*(won-1)/2 + lost;
		    Scr->cmapInfo.cmaps->scoreboard[n] = 1;
		} else
		{
		    /*
		    ** One of the cwin installs caused one of the cwin
		    ** colormaps to be de-installed, so I'm sure to get an
		    ** UninstallNotify for the cwin I know about later.
		    ** I haven't got it yet, or the test of CM_INSTALLED
		    ** above would have failed.  Turning the CM_INSTALLED
		    ** bit back on makes sure we get back here to score
		    ** the collision.
		    */
		    cmap->state |= CM_INSTALLED;
		}
	    else if (lost != -1)
		InstallWindowColormaps(ColormapNotify, (TwmWindow *) NULL);
	}
    }

    else if (cevent->state == ColormapUninstalled)
	cmap->state &= ~CM_INSTALLED;

    else if (cevent->state == ColormapInstalled)
	cmap->state |= CM_INSTALLED;
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleVisibilityNotify - visibility notify event handler
 *
 * This routine keeps track of visibility events so that colormap
 * installation can keep the maximum number of useful colormaps
 * installed at one time.
 *
 ***********************************************************************
 */

void
HandleVisibilityNotify()
{
    XVisibilityEvent *vevent = (XVisibilityEvent *) &Event;
    ColormapWindow *cwin;
    TwmColormap *cmap;

    if (XFindContext(dpy, vevent->window, ColormapContext, (caddr_t *)&cwin) == XCNOENT)
	return;
    
    /*
     * when Saber complains about retreiving an <int> from an <unsigned int>
     * just type "touch vevent->state" and "cont"
     */
    cmap = cwin->colormap;
    if ((cmap->state & CM_INSTALLABLE) &&
	vevent->state != cwin->visibility &&
	(vevent->state == VisibilityFullyObscured ||
	 cwin->visibility == VisibilityFullyObscured) &&
	cmap->w == cwin->w) {
	cwin->visibility = vevent->state;
	InstallWindowColormaps(VisibilityNotify, (TwmWindow *) NULL);
    } else
	cwin->visibility = vevent->state;
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleKeyPress - key press event handler
 *
 ***********************************************************************
 */

void
HandleKeyPress()
{
    FuncKey *key;
    int len;
    unsigned int modifier;

    if (InfoLines) XUnmapWindow(dpy, Scr->InfoWindow);
    Context = C_NO_CONTEXT;

    if (Event.xany.window == Scr->Root)
	Context = C_ROOT;
    if (Tmp_win)
    {
	if (Event.xany.window == Tmp_win->title_w)
	    Context = C_TITLE;
	if (Event.xany.window == Tmp_win->w)
	    Context = C_WINDOW;
	if (Event.xany.window == Tmp_win->icon_w)
	    Context = C_ICON;
	if (Event.xany.window == Tmp_win->frame)
	    Context = C_FRAME;
	if (Tmp_win->list && Event.xany.window == Tmp_win->list->w)
	    Context = C_ICONMGR;
	if (Tmp_win->list && Event.xany.window == Tmp_win->list->icon)
	    Context = C_ICONMGR;
    }

    modifier = (Event.xkey.state & mods_used);
    for (key = Scr->FuncKeyRoot.next; key != NULL; key = key->next)
    {
	if (key->keycode == Event.xkey.keycode &&
	    key->mods == modifier &&
	    (key->cont == Context || key->cont == C_NAME))
	{
	    /* weed out the functions that don't make sense to execute
	     * from a key press 
	     */
	    if (key->func == F_MOVE || key->func == F_RESIZE)
		return;

	    if (key->cont != C_NAME)
	    {
		ExecuteFunction(key->func, key->action, Event.xany.window,
		    Tmp_win, &Event, Context, FALSE);
		XUngrabPointer(dpy, CurrentTime);
		return;
	    }
	    else
	    {
		int matched = FALSE;
		len = strlen(key->win_name);

		/* try and match the name first */
		for (Tmp_win = Scr->TwmRoot.next; Tmp_win != NULL;
		    Tmp_win = Tmp_win->next)
		{
		    if (!strncmp(key->win_name, Tmp_win->name, len))
		    {
			matched = TRUE;
			ExecuteFunction(key->func, key->action, Tmp_win->frame,
			    Tmp_win, &Event, C_FRAME, FALSE);
			XUngrabPointer(dpy, CurrentTime);
		    }
		}

		/* now try the res_name */
		if (!matched)
		for (Tmp_win = Scr->TwmRoot.next; Tmp_win != NULL;
		    Tmp_win = Tmp_win->next)
		{
		    if (!strncmp(key->win_name, Tmp_win->class.res_name, len))
		    {
			matched = TRUE;
			ExecuteFunction(key->func, key->action, Tmp_win->frame,
			    Tmp_win, &Event, C_FRAME, FALSE);
			XUngrabPointer(dpy, CurrentTime);
		    }
		}

		/* now try the res_class */
		if (!matched)
		for (Tmp_win = Scr->TwmRoot.next; Tmp_win != NULL;
		    Tmp_win = Tmp_win->next)
		{
		    if (!strncmp(key->win_name, Tmp_win->class.res_class, len))
		    {
			matched = TRUE;
			ExecuteFunction(key->func, key->action, Tmp_win->frame,
			    Tmp_win, &Event, C_FRAME, FALSE);
			XUngrabPointer(dpy, CurrentTime);
		    }
		}
		if (matched)
		    return;
	    }
	}
    }

    /* if we get here, no function key was bound to the key.  Send it
     * to the client if it was in a window we know about.
     */
    if (Tmp_win)
    {
        if (Event.xany.window == Tmp_win->icon_w ||
	    Event.xany.window == Tmp_win->frame ||
	    Event.xany.window == Tmp_win->title_w ||
	    (Tmp_win->list && (Event.xany.window == Tmp_win->list->w)))
        {
            Event.xkey.window = Tmp_win->w;
            XSendEvent(dpy, Tmp_win->w, False, KeyPressMask, &Event);
        }
    }

}



static void free_window_names (tmp, nukefull, nukename, nukeicon)
    TwmWindow *tmp;
    Bool nukefull, nukename, nukeicon;
{
/*
 * XXX - are we sure that nobody ever sets these to another constant (check
 * twm windows)?
 */
    if (tmp->name == tmp->full_name) nukefull = False;
    if (tmp->icon_name == tmp->name) nukename = False;

#define isokay(v) ((v) && (v) != NoName)
    if (nukefull && isokay(tmp->full_name)) XFree (tmp->full_name);
    if (nukename && isokay(tmp->name)) XFree (tmp->name);
    if (nukeicon && isokay(tmp->icon_name)) XFree (tmp->icon_name);
#undef isokay
    return;
}



void free_cwins (tmp)
    TwmWindow *tmp;
{
    int i;
    TwmColormap *cmap;

    if (tmp->cmaps.number_cwins) {
	for (i = 0; i < tmp->cmaps.number_cwins; i++) {
	     if (--tmp->cmaps.cwins[i]->refcnt == 0) {
		cmap = tmp->cmaps.cwins[i]->colormap;
		if (--cmap->refcnt == 0) {
		    XDeleteContext(dpy, cmap->c, ColormapContext);
		    free((char *) cmap);
		}
		XDeleteContext(dpy, tmp->cmaps.cwins[i]->w, ColormapContext);
		free((char *) tmp->cmaps.cwins[i]);
	    }
	}
	free((char *) tmp->cmaps.cwins);
	if (tmp->cmaps.number_cwins > 1) {
	    free(tmp->cmaps.scoreboard);
	    tmp->cmaps.scoreboard = NULL;
	}
	tmp->cmaps.number_cwins = 0;
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandlePropertyNotify - property notify event handler
 *
 ***********************************************************************
 */

void
HandlePropertyNotify()
{
    char *prop = NULL;
    Atom actual = None;
    int actual_format;
    unsigned long nitems, bytesafter;
    unsigned long valuemask;		/* mask for create windows */
    XSetWindowAttributes attributes;	/* attributes for create windows */
    Pixmap pm;

    /* watch for standard colormap changes */
    if (Event.xproperty.window == Scr->Root) {
	XStandardColormap *maps = NULL;
	int nmaps;

	switch (Event.xproperty.state) {
	  case PropertyNewValue:
	    if (XGetRGBColormaps (dpy, Scr->Root, &maps, &nmaps, 
				  Event.xproperty.atom)) {
		/* if got one, then replace any existing entry */
		InsertRGBColormap (Event.xproperty.atom, maps, nmaps, True);
	    }
	    return;

	  case PropertyDelete:
	    RemoveRGBColormap (Event.xproperty.atom);
	    return;
	}
    }

    if (!Tmp_win) return;		/* unknown window */

#define MAX_NAME_LEN 200L		/* truncate to this many */
#define MAX_ICON_NAME_LEN 200L		/* ditto */

    switch (Event.xproperty.atom) {
      case XA_WM_NAME:
	if (XGetWindowProperty (dpy, Tmp_win->w, Event.xproperty.atom, 0L, 
				MAX_NAME_LEN, False, XA_STRING, &actual,
				&actual_format, &nitems, &bytesafter,
				(unsigned char **) &prop) != Success ||
	    actual == None)
	  return;
	if (!prop) prop = NoName;
	free_window_names (Tmp_win, True, True, False);

	Tmp_win->full_name = prop;
	Tmp_win->name = prop;

	Tmp_win->name_width = XTextWidth (Scr->TitleBarFont.font,
					  Tmp_win->name,
					  strlen (Tmp_win->name));

	SetupWindow (Tmp_win, Tmp_win->frame_x, Tmp_win->frame_y,
		     Tmp_win->frame_width, Tmp_win->frame_height, -1);

	if (Tmp_win->title_w) XClearArea(dpy, Tmp_win->title_w, 0,0,0,0, True);

	/*
	 * if the icon name is NoName, set the name of the icon to be
	 * the same as the window 
	 */
	if (Tmp_win->icon_name == NoName) {
	    Tmp_win->icon_name = Tmp_win->name;
	    RedoIconName();
	}
	break;

      case XA_WM_ICON_NAME:
	if (XGetWindowProperty (dpy, Tmp_win->w, Event.xproperty.atom, 0, 
				MAX_ICON_NAME_LEN, False, XA_STRING, &actual,
				&actual_format, &nitems, &bytesafter,
				(unsigned char **) &prop) != Success ||
	    actual == None)
	  return;
	if (!prop) prop = NoName;
	free_window_names (Tmp_win, False, False, True);
	Tmp_win->icon_name = prop;

	RedoIconName();
	break;

      case XA_WM_HINTS:
	if (Tmp_win->wmhints) XFree ((char *) Tmp_win->wmhints);
	Tmp_win->wmhints = XGetWMHints(dpy, Event.xany.window);

	if (Tmp_win->wmhints && (Tmp_win->wmhints->flags & WindowGroupHint))
	  Tmp_win->group = Tmp_win->wmhints->window_group;

	if (!Tmp_win->forced && Tmp_win->wmhints &&
	    Tmp_win->wmhints->flags & IconWindowHint) {
	    if (Tmp_win->icon_w) {
	    	int icon_x, icon_y;

		/*
		 * There's already an icon window.
		 * Try to find out where it is; if we succeed, move the new
		 * window to where the old one is.
		 */
		if (XGetGeometry (dpy, Tmp_win->icon_w, &JunkRoot, &icon_x,
		  &icon_y, &JunkWidth, &JunkHeight, &JunkBW, &JunkDepth)) {
		    /*
		     * Move the new icon window to where the old one was.
		     */
		    XMoveWindow(dpy, Tmp_win->wmhints->icon_window, icon_x,
		      icon_y);
		}

		/*
		 * If the window is iconic, map the new icon window.
		 */
		if (Tmp_win->icon)
		    XMapWindow(dpy, Tmp_win->wmhints->icon_window);

		/*
		 * Now, if the old window isn't ours, unmap it, otherwise
		 * just get rid of it completely.
		 */
		if (Tmp_win->icon_not_ours) {
		    if (Tmp_win->icon_w != Tmp_win->wmhints->icon_window)
			XUnmapWindow(dpy, Tmp_win->icon_w);
		} else
		    XDestroyWindow(dpy, Tmp_win->icon_w);

		/*
		 * The new icon window isn't our window, so note that fact
		 * so that we don't treat it as ours.
		 */
		Tmp_win->icon_not_ours = TRUE;

		/*
		 * Now make the new window the icon window for this window,
		 * and set it up to work as such (select for key presses
		 * and button presses/releases, set up the contexts for it,
		 * and define the cursor for it).
		 */
		Tmp_win->icon_w = Tmp_win->wmhints->icon_window;
		XSelectInput (dpy, Tmp_win->icon_w,
		  KeyPressMask | ButtonPressMask | ButtonReleaseMask);
		XSaveContext(dpy, Tmp_win->icon_w, TwmContext, (caddr_t)Tmp_win);
		XSaveContext(dpy, Tmp_win->icon_w, ScreenContext, (caddr_t)Scr);
		XDefineCursor(dpy, Tmp_win->icon_w, Scr->IconCursor);
	    }
	}

	if (Tmp_win->icon_w && !Tmp_win->forced && Tmp_win->wmhints &&
	    (Tmp_win->wmhints->flags & IconPixmapHint)) {
	    if (!XGetGeometry (dpy, Tmp_win->wmhints->icon_pixmap, &JunkRoot,
			       &JunkX, &JunkY, (unsigned int *)&Tmp_win->icon_width, 
			       (unsigned int *)&Tmp_win->icon_height, &JunkBW, &JunkDepth)) {
		return;
	    }

	    pm = XCreatePixmap (dpy, Scr->Root, Tmp_win->icon_width,
				Tmp_win->icon_height, Scr->d_depth);

	    FB(Tmp_win->iconc.fore, Tmp_win->iconc.back);
	    XCopyPlane(dpy, Tmp_win->wmhints->icon_pixmap, pm,
		Scr->NormalGC,
		0,0, Tmp_win->icon_width, Tmp_win->icon_height, 0, 0, 1 );

	    valuemask = CWBackPixmap;
	    attributes.background_pixmap = pm;

	    if (Tmp_win->icon_bm_w)
		XDestroyWindow(dpy, Tmp_win->icon_bm_w);

	    Tmp_win->icon_bm_w =
	      XCreateWindow (dpy, Tmp_win->icon_w, 0, 0,
			     (unsigned int) Tmp_win->icon_width,
			     (unsigned int) Tmp_win->icon_height,
			     (unsigned int) 0, Scr->d_depth,
			     (unsigned int) CopyFromParent, Scr->d_visual,
			     valuemask, &attributes);

	    XFreePixmap (dpy, pm);
	    RedoIconName();
	}
	break;

      case XA_WM_NORMAL_HINTS:
	GetWindowSizeHints (Tmp_win);
	break;

      default:
	if (Event.xproperty.atom == _XA_WM_COLORMAP_WINDOWS) {
	    FetchWmColormapWindows (Tmp_win);	/* frees old data */
	    break;
	} else if (Event.xproperty.atom == _XA_WM_PROTOCOLS) {
	    FetchWmProtocols (Tmp_win);
	    break;
	}
	break;
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	RedoIconName - procedure to re-position the icon window and name
 *
 ***********************************************************************
 */

RedoIconName()
{
    int x, y;

    if (Tmp_win->list)
    {
	/* let the expose event cause the repaint */
	XClearArea(dpy, Tmp_win->list->w, 0,0,0,0, True);

	if (Scr->SortIconMgr)
	    SortIconManager(Tmp_win->list->iconmgr);
    }

    if (Tmp_win->icon_w == 0)
	return;

    if (Tmp_win->icon_not_ours)
	return;

    Tmp_win->icon_w_width = XTextWidth(Scr->IconFont.font,
	Tmp_win->icon_name, strlen(Tmp_win->icon_name));

    Tmp_win->icon_w_width += 6;
    if (Tmp_win->icon_w_width < Tmp_win->icon_width)
    {
	Tmp_win->icon_x = (Tmp_win->icon_width - Tmp_win->icon_w_width)/2;
	Tmp_win->icon_x += 3;
	Tmp_win->icon_w_width = Tmp_win->icon_width;
    }
    else
    {
	Tmp_win->icon_x = 3;
    }

    if (Tmp_win->icon_w_width == Tmp_win->icon_width)
	x = 0;
    else
	x = (Tmp_win->icon_w_width - Tmp_win->icon_width)/2;

    y = 0;

    Tmp_win->icon_w_height = Tmp_win->icon_height + Scr->IconFont.height + 4;
    Tmp_win->icon_y = Tmp_win->icon_height + Scr->IconFont.height;

    XResizeWindow(dpy, Tmp_win->icon_w, Tmp_win->icon_w_width,
	Tmp_win->icon_w_height);
    if (Tmp_win->icon_bm_w)
    {
	XMoveWindow(dpy, Tmp_win->icon_bm_w, x, y);
	XMapWindow(dpy, Tmp_win->icon_bm_w);
    }
    if (Tmp_win->icon)
    {
	XClearArea(dpy, Tmp_win->icon_w, 0, 0, 0, 0, True);
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleClientMessage - client message event handler
 *
 ***********************************************************************
 */

void
HandleClientMessage()
{
    if (Event.xclient.message_type == _XA_WM_CHANGE_STATE)
    {
	if (Tmp_win != NULL)
	{
	    if (Event.xclient.data.l[0] == IconicState && !Tmp_win->icon)
	    {
		XEvent button;

		XQueryPointer( dpy, Scr->Root, &JunkRoot, &JunkChild,
			      &(button.xmotion.x_root),
			      &(button.xmotion.y_root),
			      &JunkX, &JunkY, &JunkMask);

		ExecuteFunction(F_ICONIFY, NULLSTR, Event.xany.window,
		    Tmp_win, &button, FRAME, FALSE);
		XUngrabPointer(dpy, CurrentTime);
	    }
	}
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleExpose - expose event handler
 *
 ***********************************************************************
 */

static void flush_expose();

void
HandleExpose()
{
    MenuRoot *tmp;
    if (XFindContext(dpy, Event.xany.window, MenuContext, (caddr_t *)&tmp) == 0)
    {
	PaintMenu(tmp, &Event);
	return;
    }

    if (Event.xexpose.count != 0)
	return;

    if (Event.xany.window == Scr->InfoWindow && InfoLines)
    {
	int i;
	int height;

	FBF(Scr->DefaultC.fore, Scr->DefaultC.back,
	    Scr->DefaultFont.font->fid);

	height = Scr->DefaultFont.height+2;
	for (i = 0; i < InfoLines; i++)
	{
	    XDrawString(dpy, Scr->InfoWindow, Scr->NormalGC,
		5, (i*height) + Scr->DefaultFont.y, Info[i], strlen(Info[i]));
	}
	flush_expose (Event.xany.window);
    } 
    else if (Tmp_win != NULL)
    {
	if (Event.xany.window == Tmp_win->title_w)
	{
	    FBF(Tmp_win->title.fore, Tmp_win->title.back,
		Scr->TitleBarFont.font->fid);

	    XDrawString (dpy, Tmp_win->title_w, Scr->NormalGC,
			 Scr->TBInfo.titlex, Scr->TitleBarFont.y, 
			 Tmp_win->name, strlen(Tmp_win->name));
	    flush_expose (Event.xany.window);
	}
	else if (Event.xany.window == Tmp_win->icon_w)
	{
	    FBF(Tmp_win->iconc.fore, Tmp_win->iconc.back,
		Scr->IconFont.font->fid);

	    XDrawString (dpy, Tmp_win->icon_w,
		Scr->NormalGC,
		Tmp_win->icon_x, Tmp_win->icon_y,
		Tmp_win->icon_name, strlen(Tmp_win->icon_name));
	    flush_expose (Event.xany.window);
	    return;
	} else if (Tmp_win->titlebuttons) {
	    int i;
	    Window w = Event.xany.window;
	    register TBWindow *tbw;
	    int nb = Scr->TBInfo.nleft + Scr->TBInfo.nright;

	    for (i = 0, tbw = Tmp_win->titlebuttons; i < nb; i++, tbw++) {
		if (w == tbw->window) {
		    register TitleButton *tb = tbw->info;

		    FB(Tmp_win->title.fore, Tmp_win->title.back);
		    XCopyPlane (dpy, tb->bitmap, w, Scr->NormalGC,
				tb->srcx, tb->srcy, tb->width, tb->height,
				tb->dstx, tb->dsty, 1);
		    flush_expose (w);
		    return;
		}
	    }
	}
	if (Tmp_win->list) {
	    if (Event.xany.window == Tmp_win->list->w)
	    {
		FBF(Tmp_win->list->fore, Tmp_win->list->back,
		    Scr->IconManagerFont.font->fid);
		XDrawString (dpy, Event.xany.window, Scr->NormalGC, 
		    iconmgr_textx, Scr->IconManagerFont.y+4,
		    Tmp_win->icon_name, strlen(Tmp_win->icon_name));
		DrawIconManagerBorder(Tmp_win->list);
		flush_expose (Event.xany.window);
		return;
	    }
	    if (Event.xany.window == Tmp_win->list->icon)
	    {
		FB(Tmp_win->list->fore, Tmp_win->list->back);
		XCopyPlane(dpy, Scr->siconifyPm, Tmp_win->list->icon,
		    Scr->NormalGC,
		    0,0, iconifybox_width, iconifybox_height, 0, 0, 1);
		flush_expose (Event.xany.window);
		return;
	    }
	} 
    }
}



static void remove_window_from_ring (tmp)
    TwmWindow *tmp;
{
    TwmWindow *prev = tmp->ring.prev, *next = tmp->ring.next;

    if (enter_win == tmp) {
	enter_flag = FALSE;
	enter_win = NULL;
    }
    if (raise_win == Tmp_win) raise_win = NULL;

    /*
     * 1. Unlink window
     * 2. If window was only thing in ring, null out ring
     * 3. If window was ring leader, set to next (or null)
     */
    if (prev) prev->ring.next = next;
    if (next) next->ring.prev = prev;
    if (Scr->Ring == tmp) 
      Scr->Ring = (next != tmp ? next : (TwmWindow *) NULL);

    if (!Scr->Ring || Scr->RingLeader == tmp) Scr->RingLeader = Scr->Ring;
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleDestroyNotify - DestroyNotify event handler
 *
 ***********************************************************************
 */

void
HandleDestroyNotify()
{
    int i;

    /*
     * Warning, this is also called by HandleUnmapNotify; if it ever needs to
     * look at the event, HandleUnmapNotify will have to mash the UnmapNotify
     * into a DestroyNotify.
     */

    if (Tmp_win == NULL)
	return;

    if (Tmp_win == Scr->Focus)
    {
	FocusOnRoot();
    }
    XDeleteContext(dpy, Tmp_win->w, TwmContext);
    XDeleteContext(dpy, Tmp_win->w, ScreenContext);
    XDeleteContext(dpy, Tmp_win->frame, TwmContext);
    XDeleteContext(dpy, Tmp_win->frame, ScreenContext);
    if (Tmp_win->icon_w)
    {
	XDeleteContext(dpy, Tmp_win->icon_w, TwmContext);
	XDeleteContext(dpy, Tmp_win->icon_w, ScreenContext);
    }
    if (Tmp_win->title_height)
    {
	int nb = Scr->TBInfo.nleft + Scr->TBInfo.nright;
	XDeleteContext(dpy, Tmp_win->title_w, TwmContext);
	XDeleteContext(dpy, Tmp_win->title_w, ScreenContext);
	if (Tmp_win->hilite_w)
	{
	    XDeleteContext(dpy, Tmp_win->hilite_w, TwmContext);
	    XDeleteContext(dpy, Tmp_win->hilite_w, ScreenContext);
	}
	if (Tmp_win->titlebuttons) {
	    for (i = 0; i < nb; i++) {
		XDeleteContext (dpy, Tmp_win->titlebuttons[i].window,
				TwmContext);
		XDeleteContext (dpy, Tmp_win->titlebuttons[i].window,
				ScreenContext);
	    }
        }
    }

    if (Scr->cmapInfo.cmaps == &Tmp_win->cmaps)
	InstallWindowColormaps(DestroyNotify, &Scr->TwmRoot);

    /*
     * TwmWindows contain the following pointers
     * 
     *     1.  full_name
     *     2.  name
     *     3.  icon_name
     *     4.  wmhints
     *     5.  class.res_name
     *     6.  class.res_class
     *     7.  list
     *     8.  iconmgrp
     *     9.  cwins
     *     10. titlebuttons
     *     11. window ring
     */
    if (Tmp_win->gray) XFreePixmap (dpy, Tmp_win->gray);

    XDestroyWindow(dpy, Tmp_win->frame);
    if (Tmp_win->icon_w && !Tmp_win->icon_not_ours) {
	XDestroyWindow(dpy, Tmp_win->icon_w);
	IconDown (Tmp_win);
    }
    RemoveIconManager(Tmp_win);					/* 7 */
    Tmp_win->prev->next = Tmp_win->next;
    if (Tmp_win->next != NULL)
	Tmp_win->next->prev = Tmp_win->prev;
    if (Tmp_win->auto_raise) Scr->NumAutoRaises--;

    free_window_names (Tmp_win, True, True, True);		/* 1, 2, 3 */
    if (Tmp_win->wmhints)					/* 4 */
      XFree ((char *)Tmp_win->wmhints);
    if (Tmp_win->class.res_name && Tmp_win->class.res_name != NoName)  /* 5 */
      XFree ((char *)Tmp_win->class.res_name);
    if (Tmp_win->class.res_class && Tmp_win->class.res_class != NoName) /* 6 */
      XFree ((char *)Tmp_win->class.res_class);
    free_cwins (Tmp_win);				/* 9 */
    if (Tmp_win->titlebuttons)					/* 10 */
      free ((char *) Tmp_win->titlebuttons);
    remove_window_from_ring (Tmp_win);				/* 11 */

    free((char *)Tmp_win);
}



void
HandleCreateNotify()
{
#ifdef DEBUG_EVENTS
    fprintf(stderr, "CreateNotify w = 0x%x\n", Event.xcreatewindow.window);
    fflush(stderr);
    XBell(dpy, 0);
    XSync(dpy, 0);
#endif
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleMapRequest - MapRequest event handler
 *
 ***********************************************************************
 */

void
HandleMapRequest()
{
    int stat;
    int zoom_save;

    Event.xany.window = Event.xmaprequest.window;
    stat = XFindContext(dpy, Event.xany.window, TwmContext, (caddr_t *)&Tmp_win);
    if (stat == XCNOENT)
	Tmp_win = NULL;

    /* If the window has never been mapped before ... */
    if (Tmp_win == NULL)
    {
	/* Add decorations. */
	Tmp_win = AddWindow(Event.xany.window, FALSE, (IconMgr *) NULL);
	if (Tmp_win == NULL)
	    return;
    }
    else
    {
	/*
	 * If the window has been unmapped by the client, it won't be listed
	 * in the icon manager.  Add it again, if requested.
	 */
	if (Tmp_win->list == NULL)
	    (void) AddIconManager (Tmp_win);
    }

    /* If it's not merely iconified, and we have hints, use them. */
    if ((! Tmp_win->icon) &&
	Tmp_win->wmhints && (Tmp_win->wmhints->flags & StateHint))
    {
	int state;
	Window icon;

	/* use WM_STATE if enabled */
	if (!(RestartPreviousState && GetWMState(Tmp_win->w, &state, &icon) &&
	      (state == NormalState || state == IconicState)))
	  state = Tmp_win->wmhints->initial_state;

	switch (state) 
	{
	    case DontCareState:
	    case NormalState:
	    case ZoomState:
	    case InactiveState:
		XMapWindow(dpy, Tmp_win->w);
		XMapWindow(dpy, Tmp_win->frame);
		SetMapStateProp(Tmp_win, NormalState);
		SetRaiseWindow (Tmp_win);
		break;

	    case IconicState:
		zoom_save = Scr->DoZoom;
		Scr->DoZoom = FALSE;
		Iconify(Tmp_win, 0, 0);
		Scr->DoZoom = zoom_save;
		break;
	}
    }
    /* If no hints, or currently an icon, just "deiconify" */
    else
    {
	DeIconify(Tmp_win);
	SetRaiseWindow (Tmp_win);
    }
}



void SimulateMapRequest (w)
    Window w;
{
    Event.xmaprequest.window = w;
    HandleMapRequest ();
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleMapNotify - MapNotify event handler
 *
 ***********************************************************************
 */

void
HandleMapNotify()
{
    if (Tmp_win == NULL)
	return;

    /*
     * Need to do the grab to avoid race condition of having server send
     * MapNotify to client before the frame gets mapped; this is bad because
     * the client would think that the window has a chance of being viewable
     * when it really isn't.
     */
    XGrabServer (dpy);
    if (Tmp_win->icon_w)
	XUnmapWindow(dpy, Tmp_win->icon_w);
    if (Tmp_win->title_w)
	XMapSubwindows(dpy, Tmp_win->title_w);
    XMapSubwindows(dpy, Tmp_win->frame);
    if (Scr->Focus != Tmp_win && Tmp_win->hilite_w)
	XUnmapWindow(dpy, Tmp_win->hilite_w);

    XMapWindow(dpy, Tmp_win->frame);
    XUngrabServer (dpy);
    XFlush (dpy);
    Tmp_win->mapped = TRUE;
    Tmp_win->icon = FALSE;
    Tmp_win->icon_on = FALSE;
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleUnmapNotify - UnmapNotify event handler
 *
 ***********************************************************************
 */

void
HandleUnmapNotify()
{
    int dstx, dsty;
    Window dumwin;

    /*
     * The July 27, 1988 ICCCM spec states that a client wishing to switch
     * to WithdrawnState should send a synthetic UnmapNotify with the
     * event field set to (pseudo-)root, in case the window is already
     * unmapped (which is the case for twm for IconicState).  Unfortunately,
     * we looked for the TwmContext using that field, so try the window
     * field also.
     */
    if (Tmp_win == NULL)
    {
	Event.xany.window = Event.xunmap.window;
	if (XFindContext(dpy, Event.xany.window,
	    TwmContext, (caddr_t *)&Tmp_win) == XCNOENT)
	    Tmp_win = NULL;
    }

    if (Tmp_win == NULL || (!Tmp_win->mapped && !Tmp_win->icon))
	return;

    /*
     * The program may have unmapped the client window, from either
     * NormalState or IconicState.  Handle the transition to WithdrawnState.
     *
     * We need to reparent the window back to the root (so that twm exiting 
     * won't cause it to get mapped) and then throw away all state (pretend 
     * that we've received a DestroyNotify).
     */

    XGrabServer (dpy);
    if (XTranslateCoordinates (dpy, Event.xunmap.window, Tmp_win->attr.root,
			       0, 0, &dstx, &dsty, &dumwin)) {
	XEvent ev;
	Bool reparented = XCheckTypedWindowEvent (dpy, Event.xunmap.window, 
						  ReparentNotify, &ev);
	SetMapStateProp (Tmp_win, WithdrawnState);
	if (reparented) {
	    if (Tmp_win->old_bw) XSetWindowBorderWidth (dpy,
							Event.xunmap.window, 
							Tmp_win->old_bw);
	    if (Tmp_win->wmhints && (Tmp_win->wmhints->flags & IconWindowHint))
	      XUnmapWindow (dpy, Tmp_win->wmhints->icon_window);
	} else {
	    XReparentWindow (dpy, Event.xunmap.window, Tmp_win->attr.root,
			     dstx, dsty);
	    RestoreWithdrawnLocation (Tmp_win);
	}
	XRemoveFromSaveSet (dpy, Event.xunmap.window);
	XSelectInput (dpy, Event.xunmap.window, NoEventMask);
	HandleDestroyNotify ();		/* do not need to mash event before */
    } /* else window no longer exists and we'll get a destroy notify */
    XUngrabServer (dpy);
    XFlush (dpy);
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleMotionNotify - MotionNotify event handler
 *
 ***********************************************************************
 */

void
HandleMotionNotify()
{
    if (ResizeWindow != 0)
    {
	XQueryPointer( dpy, Event.xany.window,
	    &(Event.xmotion.root), &JunkChild,
	    &(Event.xmotion.x_root), &(Event.xmotion.y_root),
	    &(Event.xmotion.x), &(Event.xmotion.y),
	    &JunkMask);

	/* Set WindowMoved appropriately so that f.deltastop will
	   work with resize as well as move. */
	if (abs (Event.xmotion.x - ResizeOrigX) >= Scr->MoveDelta
	    || abs (Event.xmotion.y - ResizeOrigY) >= Scr->MoveDelta)
	  WindowMoved = TRUE;

	XFindContext(dpy, ResizeWindow, TwmContext, (caddr_t *)&Tmp_win);
	DoResize(Event.xmotion.x_root, Event.xmotion.y_root, Tmp_win);
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleButtonRelease - ButtonRelease event handler
 *
 ***********************************************************************
 */
void
HandleButtonRelease()
{
    int xl, xr, yt, yb, w, h;
    unsigned mask;

    if (InfoLines) 		/* delete info box on 2nd button release  */
      if (Context == C_IDENTIFY) {
	XUnmapWindow(dpy, Scr->InfoWindow);
	InfoLines = 0;
	Context = C_NO_CONTEXT;
      }

    if (DragWindow != None)
    {
	MoveOutline(Scr->Root, 0, 0, 0, 0, 0, 0);

	XFindContext(dpy, DragWindow, TwmContext, (caddr_t *)&Tmp_win);
	if (DragWindow == Tmp_win->frame)
	{
	    xl = Event.xbutton.x_root - DragX - Tmp_win->frame_bw;
	    yt = Event.xbutton.y_root - DragY - Tmp_win->frame_bw;
	    w = DragWidth + 2 * Tmp_win->frame_bw;
	    h = DragHeight + 2 * Tmp_win->frame_bw;
	}
	else
	{
	    xl = Event.xbutton.x_root - DragX - Scr->IconBorderWidth;
	    yt = Event.xbutton.y_root - DragY - Scr->IconBorderWidth;
	    w = DragWidth + 2 * Scr->IconBorderWidth;
	    h = DragHeight + 2 * Scr->IconBorderWidth;
	}

	if (ConstMove)
	{
	    if (ConstMoveDir == MOVE_HORIZ)
		yt = ConstMoveY;

	    if (ConstMoveDir == MOVE_VERT)
		xl = ConstMoveX;

	    if (ConstMoveDir == MOVE_NONE)
	    {
		yt = ConstMoveY;
		xl = ConstMoveX;
	    }
	}
	
	if (Scr->DontMoveOff && MoveFunction != F_FORCEMOVE)
	{
	    xr = xl + w;
	    yb = yt + h;

	    if (xl < 0)
		xl = 0;
	    if (xr > Scr->MyDisplayWidth)
		xl = Scr->MyDisplayWidth - w;

	    if (yt < 0)
		yt = 0;
	    if (yb > Scr->MyDisplayHeight)
		yt = Scr->MyDisplayHeight - h;
	}

	CurrentDragX = xl;
	CurrentDragY = yt;
	if (DragWindow == Tmp_win->frame)
	  SetupWindow (Tmp_win, xl, yt,
		       Tmp_win->frame_width, Tmp_win->frame_height, -1);
	else
	    XMoveWindow (dpy, DragWindow, xl, yt);

	if (!Scr->NoRaiseMove && !Scr->OpaqueMove)    /* opaque already did */
	    XRaiseWindow(dpy, DragWindow);

	if (!Scr->OpaqueMove)
	    UninstallRootColormap();
	else
	    XSync(dpy, 0);

	if (Scr->NumAutoRaises) {
	    enter_flag = TRUE;
	    enter_win = NULL;
	    raise_win = ((DragWindow == Tmp_win->frame && !Scr->NoRaiseMove)
			 ? Tmp_win : NULL);
	}

	DragWindow = 0;
	ConstMove = FALSE;
    }

    if (ResizeWindow != 0)
    {
	EndResize();
    }

    if (ActiveMenu != NULL && RootFunction == 0)
    {
	if (ActiveItem != NULL)
	{
	    int func = ActiveItem->func;
	    Action = ActiveItem->action;
	    switch (func) {
	      case F_MOVE:
	      case F_FORCEMOVE:
		ButtonPressed = -1;
		break;
	      case F_CIRCLEUP:
	      case F_CIRCLEDOWN:
	      case F_REFRESH:
	      case F_WARPTOSCREEN:
		PopDownMenu();
		break;
	      default:
		break;
	    }
	    ExecuteFunction(func, Action,
		ButtonWindow ? ButtonWindow->frame : None,
		ButtonWindow, &Event/*&ButtonEvent*/, Context, TRUE);
	    Context = C_NO_CONTEXT;
	    ButtonWindow = NULL;

	    /* if we are not executing a defered command, then take down the
	     * menu
	     */
	    if (RootFunction == 0)
	    {
		PopDownMenu();
	    }
	}
	else
	    PopDownMenu();
    }

    mask = (Button1Mask|Button2Mask|Button3Mask|Button4Mask|Button5Mask);
    switch (Event.xbutton.button)
    {
	case Button1: mask &= ~Button1Mask; break;
	case Button2: mask &= ~Button2Mask; break;
	case Button3: mask &= ~Button3Mask; break;
	case Button4: mask &= ~Button4Mask; break;
	case Button5: mask &= ~Button5Mask; break;
    }

    if (RootFunction != 0 ||
	ResizeWindow != None ||
	DragWindow != None)
	ButtonPressed = -1;

    if (RootFunction == 0 &&
	(Event.xbutton.state & mask) == 0 &&
	DragWindow == None &&
	ResizeWindow == None)
    {
	XUngrabPointer(dpy, CurrentTime);
	XUngrabServer(dpy);
	XFlush(dpy);
	EventHandler[EnterNotify] = HandleEnterNotify;
	EventHandler[LeaveNotify] = HandleLeaveNotify;
	ButtonPressed = -1;
	if (DownIconManager)
	{
	    DownIconManager->down = FALSE;
	    if (Scr->Highlight) DrawIconManagerBorder(DownIconManager);
	    DownIconManager = NULL;
	}
	Cancel = FALSE;
    }
}



static do_menu (menu, w)
    MenuRoot *menu;			/* menu to pop up */
    Window w;				/* invoking window or None */
{
    int x = Event.xbutton.x_root;
    int y = Event.xbutton.y_root;
    Bool center;

    if (!Scr->NoGrabServer)
	XGrabServer(dpy);
    if (w) {
	int h = Scr->TBInfo.width - Scr->TBInfo.border;
	Window child;

	(void) XTranslateCoordinates (dpy, w, Scr->Root, 0, h, &x, &y, &child);
	center = False;
    } else {
	center = True;
    }
    if (PopUpMenu (menu, x, y, center)) {
	UpdateMenu();
    } else {
	XBell (dpy, 0);
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleButtonPress - ButtonPress event handler
 *
 ***********************************************************************
 */
void
HandleButtonPress()
{
    unsigned int modifier;
    Cursor cur;

    /* pop down the menu, if any */
    if (ActiveMenu != NULL)
	PopDownMenu();

    XSync(dpy, 0);			/* XXX - remove? */

    if (ButtonPressed != -1 && !InfoLines) /* want menus if we have info box */
    {
	/* we got another butt press in addition to one still held
	 * down, we need to cancel the operation we were doing
	 */
	Cancel = TRUE;
	CurrentDragX = origDragX;
	CurrentDragY = origDragY;
	if (!menuFromFrameOrWindowOrTitlebar)
	  if (Scr->OpaqueMove && DragWindow != None) {
	    XMoveWindow (dpy, DragWindow, origDragX, origDragY);
	  } else {
	    MoveOutline(Scr->Root, 0, 0, 0, 0, 0, 0);
	  }
	XUnmapWindow(dpy, Scr->SizeWindow);
	if (!Scr->OpaqueMove)
	    UninstallRootColormap();
	ResizeWindow = None;
	DragWindow = None;
	cur = LeftButt;
	if (Event.xbutton.button == Button2)
	    cur = MiddleButt;
	else if (Event.xbutton.button >= Button3)
	    cur = RightButt;

	XGrabPointer(dpy, Scr->Root, True,
	    ButtonReleaseMask | ButtonPressMask,
	    GrabModeAsync, GrabModeAsync,
	    Scr->Root, cur, CurrentTime);

	return;
    }
    else
	ButtonPressed = Event.xbutton.button;

    if (ResizeWindow != None ||
	DragWindow != None  ||
	ActiveMenu != NULL)
	return;

    /* check the title bar buttons */
    if (Tmp_win && Tmp_win->title_height && Tmp_win->titlebuttons)
    {
	register int i;
	register TBWindow *tbw;
	int nb = Scr->TBInfo.nleft + Scr->TBInfo.nright;

	for (i = 0, tbw = Tmp_win->titlebuttons; i < nb; i++, tbw++) {
	    if (Event.xany.window == tbw->window) {
		if (tbw->info->func == F_MENU) {
		    Context = C_TITLE;
		    ButtonEvent = Event;
		    ButtonWindow = Tmp_win;
		    do_menu (tbw->info->menuroot, tbw->window);
		} else {
		    ExecuteFunction (tbw->info->func, tbw->info->action,
				     Event.xany.window, Tmp_win, &Event,
				     C_TITLE, FALSE);
		}
		return;
	    }
	}
    }

    Context = C_NO_CONTEXT;

    if (Event.xany.window == Scr->InfoWindow)
      Context = C_IDENTIFY;

    if (Event.xany.window == Scr->Root)
	Context = C_ROOT;
    if (Tmp_win)
    {
	if (Tmp_win->list && RootFunction != 0 &&
	    (Event.xany.window == Tmp_win->list->w ||
		Event.xany.window == Tmp_win->list->icon))
	{
	    Tmp_win = Tmp_win->list->iconmgr->twm_win;
	    XTranslateCoordinates(dpy, Event.xany.window, Tmp_win->w,
		Event.xbutton.x, Event.xbutton.y, 
		&JunkX, &JunkY, &JunkChild);

	    Event.xbutton.x = JunkX;
	    Event.xbutton.y = JunkY - Tmp_win->title_height;
	    Event.xany.window = Tmp_win->w;
	    Context = C_WINDOW;
	}
	else if (Event.xany.window == Tmp_win->title_w)
	{
	    Context = C_TITLE;
	}
	else if (Event.xany.window == Tmp_win->w) 
	{
	    printf("ERROR! ERROR! ERROR! YOU SHOULD NOT BE HERE!!!\n");
	    Context = C_WINDOW;
	}
	else if (Event.xany.window == Tmp_win->icon_w)
	{
	    Context = C_ICON;
	}
	else if (Event.xany.window == Tmp_win->frame) 
	{
	    /* since we now place a button grab on the frame instead
             * of the window, (see GrabButtons() in add_window.c), we
             * need to figure out where the pointer exactly is before
             * assigning Context.  If the pointer is on the application
             * window we will change the event structure to look as if
             * it came from the application window.
	     */
	    if (Event.xbutton.subwindow == Tmp_win->w) {
	      Event.xbutton.window = Tmp_win->w;
              Event.xbutton.y -= Tmp_win->title_height;
/*****
              Event.xbutton.x -= Tmp_win->frame_bw;
*****/
	      Context = C_WINDOW;
	    }
            else Context = C_FRAME;
	}
	else if (Tmp_win->list &&
	    (Event.xany.window == Tmp_win->list->w ||
		Event.xany.window == Tmp_win->list->icon))
	{
	    Tmp_win->list->down = TRUE;
	    if (Scr->Highlight) DrawIconManagerBorder(Tmp_win->list);
	    DownIconManager = Tmp_win->list;
	    Context = C_ICONMGR;
	}
    }

    /* this section of code checks to see if we were in the middle of
     * a command executed from a menu
     */
    if (RootFunction != 0)
    {
	if (Event.xany.window == Scr->Root)
	{
	    /* if the window was the Root, we don't know for sure it
	     * it was the root.  We must check to see if it happened to be
	     * inside of a client that was getting button press events.
	     */
	    XTranslateCoordinates(dpy, Scr->Root, Scr->Root,
		Event.xbutton.x, 
		Event.xbutton.y, 
		&JunkX, &JunkY, &Event.xany.window);

	    if (Event.xany.window == 0 ||
		(XFindContext(dpy, Event.xany.window, TwmContext,
			      (caddr_t *)&Tmp_win) == XCNOENT))
	    {
		RootFunction = 0;
		XBell(dpy, 0);
		return;
	    }

	    XTranslateCoordinates(dpy, Scr->Root, Event.xany.window,
		Event.xbutton.x, 
		Event.xbutton.y, 
		&JunkX, &JunkY, &JunkChild);

	    Event.xbutton.x = JunkX;
	    Event.xbutton.y = JunkY;
	    Context = C_WINDOW;
	}

	/* make sure we are not trying to move an identify window */
	if (Event.xany.window != Scr->InfoWindow)
	  ExecuteFunction(RootFunction, Action, Event.xany.window,
			  Tmp_win, &Event, Context, FALSE);

	RootFunction = 0;
	return;
    }

    ButtonEvent = Event;
    ButtonWindow = Tmp_win;

    /* if we get to here, we have to execute a function or pop up a 
     * menu
     */
    modifier = (Event.xbutton.state & mods_used);

    if (Context == C_NO_CONTEXT)
	return;

    RootFunction = 0;
    if (Scr->Mouse[Event.xbutton.button][Context][modifier].func == F_MENU)
    {
	do_menu (Scr->Mouse[Event.xbutton.button][Context][modifier].menu,
		 (Window) None);
    }
    else if (Scr->Mouse[Event.xbutton.button][Context][modifier].func != 0)
    {
	Action = Scr->Mouse[Event.xbutton.button][Context][modifier].item ?
	    Scr->Mouse[Event.xbutton.button][Context][modifier].item->action : NULL;
	ExecuteFunction(Scr->Mouse[Event.xbutton.button][Context][modifier].func,
	    Action, Event.xany.window, Tmp_win, &Event, Context, FALSE);
    }
    else if (Scr->DefaultFunction.func != 0)
    {
	if (Scr->DefaultFunction.func == F_MENU)
	{
	    do_menu (Scr->DefaultFunction.menu, (Window) None);
	}
	else
	{
	    Action = Scr->DefaultFunction.item ?
		Scr->DefaultFunction.item->action : NULL;
	    ExecuteFunction(Scr->DefaultFunction.func, Action,
	       Event.xany.window, Tmp_win, &Event, Context, FALSE);
	}
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HENQueueScanner - EnterNotify event q scanner
 *
 *	Looks at the queued events and determines if any matching
 *	LeaveNotify events or EnterEvents deriving from the
 *	termination of a grab are behind this event to allow
 *	skipping of unnecessary processing.
 *
 ***********************************************************************
 */

typedef struct HENScanArgs {
    Window w;		/* Window we are currently entering */
    Bool leaves;	/* Any LeaveNotifies found for this window */
    Bool inferior;	/* Was NotifyInferior the mode for LeaveNotify */
    Bool enters;	/* Any EnterNotify events with NotifyUngrab */
} HENScanArgs;

/* ARGSUSED*/
static Bool
HENQueueScanner(dpy, ev, args)
    Display *dpy;
    XEvent *ev;
    char *args;
{
    if (ev->type == LeaveNotify) {
	if (ev->xcrossing.window == ((HENScanArgs *) args)->w &&
	    ev->xcrossing.mode == NotifyNormal) {
	    ((HENScanArgs *) args)->leaves = True;
	    /*
	     * Only the last event found matters for the Inferior field.
	     */
	    ((HENScanArgs *) args)->inferior =
		(ev->xcrossing.detail == NotifyInferior);
	}
    } else if (ev->type == EnterNotify) {
	if (ev->xcrossing.mode == NotifyUngrab)
	    ((HENScanArgs *) args)->enters = True;
    }

    return (False);
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleEnterNotify - EnterNotify event handler
 *
 ***********************************************************************
 */

void
HandleEnterNotify()
{
    MenuRoot *mr;
    XEnterWindowEvent *ewp = &Event.xcrossing;
    HENScanArgs scanArgs;
    XEvent dummy;
    
    /*
     * Save the id of the window entered.  This will be used to remove
     * border highlight on entering the next application window.
     */
    if (UnHighLight_win && ewp->window != UnHighLight_win->w) {
      SetBorder (UnHighLight_win, False);	/* application window */
      if (UnHighLight_win->list) /* in the icon box */
	NotActiveIconManager(UnHighLight_win->list);
    }
    if (ewp->window == Scr->Root)
      UnHighLight_win = NULL;
    else if (Tmp_win)
      UnHighLight_win = Tmp_win;

    /*
     * if we aren't in the middle of menu processing
     */
    if (!ActiveMenu) {
	/*
	 * We're not interested in pseudo Enter/Leave events generated
	 * from grab initiations.
	 */
	if (ewp->mode == NotifyGrab)
	    return;

	/*
	 * Scan for Leave and Enter Notify events to see if we can avoid some
	 * unnecessary processing.
	 */
	scanArgs.w = ewp->window;
	scanArgs.leaves = scanArgs.enters = False;
	(void) XCheckIfEvent(dpy, &dummy, HENQueueScanner, (char *) &scanArgs);

	/*
	 * if entering root window, restore twm default colormap so that 
	 * titlebars are legible
	 */
	if (ewp->window == Scr->Root) {
	    if (!scanArgs.leaves && !scanArgs.enters)
		InstallWindowColormaps(EnterNotify, &Scr->TwmRoot);
	    return;
	}

	/*
	 * if we have an event for a specific one of our windows
	 */
	if (Tmp_win) {
	    /*
	     * If currently in PointerRoot mode (indicated by FocusRoot), then
	     * focus on this window
	     */
	    if (Scr->FocusRoot && (!scanArgs.leaves || scanArgs.inferior)) {
		if (Tmp_win->list) ActiveIconManager(Tmp_win->list);
		if (Tmp_win->mapped) {
		    /*
		     * unhighlight old focus window
		     */
		    if (Scr->Focus &&
			Scr->Focus != Tmp_win && Tmp_win->hilite_w)
		      XUnmapWindow(dpy, Scr->Focus->hilite_w);

		    /*
		     * If entering the frame or the icon manager, then do 
		     * "window activation things":
		     *
		     *     1.  turn on highlight window (if any)
		     *     2.  install frame colormap
		     *     3.  set frame and highlight window (if any) border
		     *     4.  focus on client window to forward typing
		     *     4a. same as 4 but for icon mgr w/with NoTitlebar on.
		     *     5.  send WM_TAKE_FOCUS if requested
		     */
		    if (ewp->window == Tmp_win->frame ||
			(Tmp_win->list && ewp->window == Tmp_win->list->w)) {
			if (Tmp_win->hilite_w)				/* 1 */
			  XMapWindow (dpy, Tmp_win->hilite_w);
			if (!scanArgs.leaves && !scanArgs.enters)
			    InstallWindowColormaps (EnterNotify,	/* 2 */
						    &Scr->TwmRoot);
			SetBorder (Tmp_win, True);			/* 3 */
			if (Tmp_win->title_w && Scr->TitleFocus &&	/* 4 */
			    Tmp_win->wmhints && Tmp_win->wmhints->input)
			  SetFocus (Tmp_win, ewp->time);
			if (Scr->NoTitlebar && Scr->TitleFocus &&	/*4a */
			    Tmp_win->wmhints && Tmp_win->wmhints->input)
			  SetFocus (Tmp_win, ewp->time);
			if (Tmp_win->protocols & DoesWmTakeFocus)	/* 5 */
			  SendTakeFocusMessage (Tmp_win, ewp->time);
			Scr->Focus = Tmp_win;
		    } else if (ewp->window == Tmp_win->w) {
			/*
			 * If we are entering the application window, install
			 * its colormap(s).
			 */
			if (!scanArgs.leaves || scanArgs.inferior)
			    InstallWindowColormaps(EnterNotify, Tmp_win);
		    }
		}			/* end if Tmp_win->mapped */
		if (Tmp_win->wmhints != NULL &&
			ewp->window == Tmp_win->wmhints->icon_window &&
			(!scanArgs.leaves || scanArgs.inferior))
			    InstallWindowColormaps(EnterNotify, Tmp_win);
	    }				/* end if FocusRoot */
	    /*
	     * If this window is to be autoraised, mark it so
	     */
	    if (Tmp_win->auto_raise) {
		enter_win = Tmp_win;
		if (enter_flag == FALSE) AutoRaiseWindow (Tmp_win);
	    } else if (enter_flag && raise_win == Tmp_win)
	      enter_win = Tmp_win;
	    /*
	     * set ring leader
	     */
	    if (Tmp_win->ring.next && (!enter_flag || raise_win == enter_win))
	      Scr->RingLeader = Tmp_win;
	    XSync (dpy, 0);
	    return;
	}				/* end if Tmp_win */
    }					/* end if !ActiveMenu */

    /*
     * Find the menu that we are dealing with now; punt if unknown
     */
    if (XFindContext (dpy, ewp->window, MenuContext, (caddr_t *)&mr) != XCSUCCESS) return;

    mr->entered = TRUE;
    if (ActiveMenu && mr == ActiveMenu->prev && RootFunction == 0) {
	if (Scr->Shadow) XUnmapWindow (dpy, ActiveMenu->shadow);
	XUnmapWindow (dpy, ActiveMenu->w);
	ActiveMenu->mapped = UNMAPPED;
	UninstallRootColormap ();
	if (ActiveItem) {
	    ActiveItem->state = 0;
	    PaintEntry (ActiveMenu, ActiveItem,  False);
	}
	ActiveItem = NULL;
	ActiveMenu = mr;
	MenuDepth--;
    }
    return;
}



/***********************************************************************
 *
 *  Procedure:
 *	HLNQueueScanner - LeaveNotify event q scanner
 *
 *	Looks at the queued events and determines if any
 *	EnterNotify events are behind this event to allow
 *	skipping of unnecessary processing.
 *
 ***********************************************************************
 */

typedef struct HLNScanArgs {
    Window w;		/* The window getting the LeaveNotify */
    Bool enters;	/* Any EnterNotify event at all */
    Bool matches;	/* Any matching EnterNotify events */
} HLNScanArgs;

/* ARGSUSED*/
static Bool
HLNQueueScanner(dpy, ev, args)
    Display *dpy;
    XEvent *ev;
    char *args;
{
    if (ev->type == EnterNotify && ev->xcrossing.mode != NotifyGrab) {
	((HLNScanArgs *) args)->enters = True;
	if (ev->xcrossing.window == ((HLNScanArgs *) args)->w)
	    ((HLNScanArgs *) args)->matches = True;
    }

    return (False);
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleLeaveNotify - LeaveNotify event handler
 *
 ***********************************************************************
 */

void
HandleLeaveNotify()
{
    HLNScanArgs scanArgs;
    XEvent dummy;

    if (Tmp_win != NULL)
    {
	Bool inicon;

	/*
	 * We're not interested in pseudo Enter/Leave events generated
	 * from grab initiations and terminations.
	 */
	if (Event.xcrossing.mode != NotifyNormal)
	    return;

	inicon = (Tmp_win->list &&
		  Tmp_win->list->w == Event.xcrossing.window);

	if (Scr->RingLeader && Scr->RingLeader == Tmp_win &&
	    (Event.xcrossing.detail != NotifyInferior &&
	     Event.xcrossing.window != Tmp_win->w)) {
	    if (!inicon) {
		if (Tmp_win->mapped) {
		    Tmp_win->ring.cursor_valid = False;
		} else {
		    Tmp_win->ring.cursor_valid = True;
		    Tmp_win->ring.curs_x = (Event.xcrossing.x_root -
					    Tmp_win->frame_x);
		    Tmp_win->ring.curs_y = (Event.xcrossing.y_root -
					    Tmp_win->frame_y);
		}
	    }
	    Scr->RingLeader = (TwmWindow *) NULL;
	}
	if (Scr->FocusRoot) {

	    if (Event.xcrossing.detail != NotifyInferior) {

		/*
		 * Scan for EnterNotify events to see if we can avoid some
		 * unnecessary processing.
		 */
		scanArgs.w = Event.xcrossing.window;
		scanArgs.enters = scanArgs.matches = False;
		(void) XCheckIfEvent(dpy, &dummy, HLNQueueScanner,
				     (char *) &scanArgs);

		if ((Event.xcrossing.window == Tmp_win->frame &&
			!scanArgs.matches) || inicon) {
		    if (Tmp_win->list) NotActiveIconManager(Tmp_win->list);
		    if (Tmp_win->hilite_w)
		      XUnmapWindow (dpy, Tmp_win->hilite_w);
		    SetBorder (Tmp_win, False);
		    if (Scr->TitleFocus ||
			Tmp_win->protocols & DoesWmTakeFocus)
		      SetFocus ((TwmWindow *) NULL, Event.xcrossing.time);
		    Scr->Focus = NULL;
		} else if (Event.xcrossing.window == Tmp_win->w &&
				!scanArgs.enters) {
		    InstallWindowColormaps (LeaveNotify, &Scr->TwmRoot);
		}
	    }
	}
	XSync (dpy, 0);
	return;
    }
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleConfigureRequest - ConfigureRequest event handler
 *
 ***********************************************************************
 */

void
HandleConfigureRequest()
{
    XWindowChanges xwc;
    unsigned long xwcm;
    int x, y, width, height, bw;
    int gravx, gravy;
    XConfigureRequestEvent *cre = &Event.xconfigurerequest;

#ifdef DEBUG_EVENTS
    fprintf(stderr, "ConfigureRequest\n");
    if (cre->value_mask & CWX)
	fprintf(stderr, "  x = %d\n", cre->x);
    if (cre->value_mask & CWY)
	fprintf(stderr, "  y = %d\n", cre->y);
    if (cre->value_mask & CWWidth)
	fprintf(stderr, "  width = %d\n", cre->width);
    if (cre->value_mask & CWHeight)
	fprintf(stderr, "  height = %d\n", cre->height);
    if (cre->value_mask & CWSibling)
	fprintf(stderr, "  above = 0x%x\n", cre->above);
    if (cre->value_mask & CWStackMode)
	fprintf(stderr, "  stack = %d\n", cre->detail);
#endif

    /*
     * Event.xany.window is Event.xconfigurerequest.parent, so Tmp_win will
     * be wrong
     */
    Event.xany.window = cre->window;	/* mash parent field */
    if (XFindContext (dpy, cre->window, TwmContext, (caddr_t *) &Tmp_win) ==
	XCNOENT)
      Tmp_win = NULL;


    /*
     * According to the July 27, 1988 ICCCM draft, we should ignore size and
     * position fields in the WM_NORMAL_HINTS property when we map a window.
     * Instead, we'll read the current geometry.  Therefore, we should respond
     * to configuration requests for windows which have never been mapped.
     */
    if (!Tmp_win || Tmp_win->icon_w == cre->window) {
	xwcm = cre->value_mask & 
	    (CWX | CWY | CWWidth | CWHeight | CWBorderWidth);
	xwc.x = cre->x;
	xwc.y = cre->y;
	xwc.width = cre->width;
	xwc.height = cre->height;
	xwc.border_width = cre->border_width;
	XConfigureWindow(dpy, Event.xany.window, xwcm, &xwc);
	return;
    }

    if ((cre->value_mask & CWStackMode) && Tmp_win->stackmode) {
	TwmWindow *otherwin;

	xwc.sibling = (((cre->value_mask & CWSibling) &&
			(XFindContext (dpy, cre->above, TwmContext,
				       (caddr_t *) &otherwin) == XCSUCCESS))
		       ? otherwin->frame : cre->above);
	xwc.stack_mode = cre->detail;
	XConfigureWindow (dpy, Tmp_win->frame, 
			  cre->value_mask & (CWSibling | CWStackMode), &xwc);
    }


    /* Don't modify frame_XXX fields before calling SetupWindow! */
    x = Tmp_win->frame_x;
    y = Tmp_win->frame_y;
    width = Tmp_win->frame_width;
    height = Tmp_win->frame_height;
    bw = Tmp_win->frame_bw;

    /*
     * Section 4.1.5 of the ICCCM states that the (x,y) coordinates in the
     * configure request are for the upper-left outer corner of the window.
     * This means that we need to adjust for the additional title height as
     * well as for any border width changes that we decide to allow.  The
     * current window gravity is to be used in computing the adjustments, just
     * as when initially locating the window.  Note that if we do decide to 
     * allow border width changes, we will need to send the synthetic 
     * ConfigureNotify event.
     */
    GetGravityOffsets (Tmp_win, &gravx, &gravy);

    if (cre->value_mask & CWBorderWidth) {
	int bwdelta = cre->border_width - Tmp_win->old_bw;  /* posit growth */
	if (bwdelta && Scr->ClientBorderWidth) {  /* if change allowed */
	    x += gravx * bwdelta;	/* change default values only */
	    y += gravy * bwdelta;	/* ditto */
	    bw = cre->border_width;
	    if (Tmp_win->title_height) height += bwdelta;
	    x += (gravx < 0) ? bwdelta : -bwdelta;
	    y += (gravy < 0) ? bwdelta : -bwdelta;
	}
	Tmp_win->old_bw = cre->border_width;  /* for restoring */
    }

    if (cre->value_mask & CWX) {	/* override even if border change */
	x = cre->x - bw;
    }
    if (cre->value_mask & CWY) {
	y = cre->y - ((gravy < 0) ? 0 : Tmp_win->title_height) - bw;
    }

    if (cre->value_mask & CWWidth) {
	width = cre->width;
    }
    if (cre->value_mask & CWHeight) {
	height = cre->height + Tmp_win->title_height;
    }

    if (width != Tmp_win->frame_width || height != Tmp_win->frame_height)
	Tmp_win->zoomed = ZOOM_NONE;

    /*
     * SetupWindow (x,y) are the location of the upper-left outer corner and
     * are passed directly to XMoveResizeWindow (frame).  The (width,height)
     * are the inner size of the frame.  The inner width is the same as the 
     * requested client window width; the inner height is the same as the
     * requested client window height plus any title bar slop.
     */
    SetupWindow (Tmp_win, x, y, width, height, bw);
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleShapeNotify - shape notification event handler
 *
 ***********************************************************************
 */
void
HandleShapeNotify ()
{
    XShapeEvent	    *sev = (XShapeEvent *) &Event;

    if (Tmp_win == NULL)
	return;
    if (sev->kind != ShapeBounding)
	return;
    if (!Tmp_win->wShaped && sev->shaped) {
	XShapeCombineMask (dpy, Tmp_win->frame, ShapeClip, 0, 0, None,
			   ShapeSet);
    }
    Tmp_win->wShaped = sev->shaped;
    SetFrameShape (Tmp_win);
}



/***********************************************************************
 *
 *  Procedure:
 *	HandleUnknown - unknown event handler
 *
 ***********************************************************************
 */

void
HandleUnknown()
{
#ifdef DEBUG_EVENTS
    fprintf(stderr, "type = %d\n", Event.type);
#endif
}



/***********************************************************************
 *
 *  Procedure:
 *	Transient - checks to see if the window is a transient
 *
 *  Returned Value:
 *	TRUE	- window is a transient
 *	FALSE	- window is not a transient
 *
 *  Inputs:
 *	w	- the window to check
 *
 ***********************************************************************
 */

int
Transient(w, propw)
    Window w, *propw;
{
    return (XGetTransientForHint(dpy, w, propw));
}



/***********************************************************************
 *
 *  Procedure:
 *	FindScreenInfo - get ScreenInfo struct associated with a given window
 *
 *  Returned Value:
 *	ScreenInfo struct
 *
 *  Inputs:
 *	w	- the window
 *
 ***********************************************************************
 */

ScreenInfo *
FindScreenInfo(w)
    Window w;
{
    XWindowAttributes attr;
    int scrnum;

    attr.screen = NULL;
    if (XGetWindowAttributes(dpy, w, &attr)) {
	for (scrnum = 0; scrnum < NumScreens; scrnum++) {
	    if (ScreenList[scrnum] != NULL &&
		(ScreenOfDisplay(dpy, ScreenList[scrnum]->screen) ==
		 attr.screen))
	      return ScreenList[scrnum];
	}
    }

    return NULL;
}



static void flush_expose (w)
    Window w;
{
    XEvent dummy;

				/* SUPPRESS 530 */
    while (XCheckTypedWindowEvent (dpy, w, Expose, &dummy)) ;
}



/***********************************************************************
 *
 *  Procedure:
 *	InstallWindowColormaps - install the colormaps for one twm window
 *
 *  Inputs:
 *	type	- type of event that caused the installation
 *	tmp	- for a subset of event types, the address of the
 *		  window structure, whose colormaps are to be installed.
 *
 ***********************************************************************
 */

InstallWindowColormaps (type, tmp)
    int type;
    TwmWindow *tmp;
{
    int i, j, n, number_cwins, state;
    ColormapWindow **cwins, *cwin, **maxcwin = NULL;
    TwmColormap *cmap;
    char *row, *scoreboard;

    switch (type) {
    case EnterNotify:
    case LeaveNotify:
    case DestroyNotify:
    default:
	/* Save the colormap to be loaded for when force loading of
	 * root colormap(s) ends.
	 */
	Scr->cmapInfo.pushed_window = tmp;
	/* Don't load any new colormap if root colormap(s) has been
	 * force loaded.
	 */
	if (Scr->cmapInfo.root_pushes)
	    return;
	/* Don't reload the currend window colormap list.
	 */
	if (Scr->cmapInfo.cmaps == &tmp->cmaps)
	    return;
	if (Scr->cmapInfo.cmaps)
	    for (i = Scr->cmapInfo.cmaps->number_cwins,
		 cwins = Scr->cmapInfo.cmaps->cwins; i-- > 0; cwins++)
		(*cwins)->colormap->state &= ~CM_INSTALLABLE;
	Scr->cmapInfo.cmaps = &tmp->cmaps;
	break;
    
    case PropertyNotify:
    case VisibilityNotify:
    case ColormapNotify:
	break;
    }

    number_cwins = Scr->cmapInfo.cmaps->number_cwins;
    cwins = Scr->cmapInfo.cmaps->cwins;
    scoreboard = Scr->cmapInfo.cmaps->scoreboard;

    ColortableThrashing = FALSE; /* in case installation aborted */

    state = CM_INSTALLED;

      for (i = n = 0; i < number_cwins; i++) {
	cwin = cwins[i];
	cmap = cwin->colormap;
	cmap->state |= CM_INSTALLABLE;
	cmap->state &= ~CM_INSTALL;
	cmap->w = cwin->w;
      }
      for (i = n = 0; i < number_cwins; i++) {
  	cwin = cwins[i];
  	cmap = cwin->colormap;
	if (cwin->visibility != VisibilityFullyObscured &&
	    n < Scr->cmapInfo.maxCmaps) {
	    row = scoreboard + (i*(i-1)/2);
	    for (j = 0; j < i; j++)
		if (row[j] && (cwins[j]->colormap->state & CM_INSTALL))
		    break;
	    if (j != i)
		continue;
	    n++;
	    maxcwin = &cwins[i];
	    state &= (cmap->state & CM_INSTALLED);
	    cmap->state |= CM_INSTALL;
	}
    }

    Scr->cmapInfo.first_req = NextRequest(dpy);

    for ( ; n > 0; maxcwin--) {
	cmap = (*maxcwin)->colormap;
	if (cmap->state & CM_INSTALL) {
	    cmap->state &= ~CM_INSTALL;
	    if (!(state & CM_INSTALLED)) {
		cmap->install_req = NextRequest(dpy);
		XInstallColormap(dpy, cmap->c);
	    }
	    cmap->state |= CM_INSTALLED;
	    n--;
	}
    }
}



/***********************************************************************
 *
 *  Procedures:
 *	<Uni/I>nstallRootColormap - Force (un)loads root colormap(s)
 *
 *	   These matching routines provide a mechanism to insure that
 *	   the root colormap(s) is installed during operations like
 *	   rubber banding or menu display that require colors from
 *	   that colormap.  Calls may be nested arbitrarily deeply,
 *	   as long as there is one UninstallRootColormap call per
 *	   InstallRootColormap call.
 *
 *	   The final UninstallRootColormap will cause the colormap list
 *	   which would otherwise have be loaded to be loaded, unless
 *	   Enter or Leave Notify events are queued, indicating some
 *	   other colormap list would potentially be loaded anyway.
 ***********************************************************************
 */

InstallRootColormap()
{
    TwmWindow *tmp;
    if (Scr->cmapInfo.root_pushes == 0) {
	/*
	 * The saving and restoring of cmapInfo.pushed_window here
	 * is a slimy way to remember the actual pushed list and
	 * not that of the root window.
	 */
	tmp = Scr->cmapInfo.pushed_window;
	InstallWindowColormaps(0, &Scr->TwmRoot);
	Scr->cmapInfo.pushed_window = tmp;
    }
    Scr->cmapInfo.root_pushes++;
}



/* ARGSUSED*/
static Bool
UninstallRootColormapQScanner(dpy, ev, args)
    Display *dpy;
    XEvent *ev;
    char *args;
{
    if (!*args)
	if (ev->type == EnterNotify) {
	    if (ev->xcrossing.mode != NotifyGrab)
		*args = 1;
	} else if (ev->type == LeaveNotify) {
	    if (ev->xcrossing.mode == NotifyNormal)
		*args = 1;
	}

    return (False);
}



UninstallRootColormap()
{
    char args;
    XEvent dummy;

    if (Scr->cmapInfo.root_pushes)
	Scr->cmapInfo.root_pushes--;
    
    if (!Scr->cmapInfo.root_pushes) {
	/*
	 * If we have subsequent Enter or Leave Notify events,
	 * we can skip the reload of pushed colormaps.
	 */
	XSync (dpy, 0);
	args = 0;
	(void) XCheckIfEvent(dpy, &dummy, UninstallRootColormapQScanner, &args);

	if (!args)
	    InstallWindowColormaps(0, Scr->cmapInfo.pushed_window);
    }
}

#ifdef TRACE
dumpevent (e)
    XEvent *e;
{
    char *name = NULL;

    switch (e->type) {
      case KeyPress:  name = "KeyPress"; break;
      case KeyRelease:  name = "KeyRelease"; break;
      case ButtonPress:  name = "ButtonPress"; break;
      case ButtonRelease:  name = "ButtonRelease"; break;
      case MotionNotify:  name = "MotionNotify"; break;
      case EnterNotify:  name = "EnterNotify"; break;
      case LeaveNotify:  name = "LeaveNotify"; break;
      case FocusIn:  name = "FocusIn"; break;
      case FocusOut:  name = "FocusOut"; break;
      case KeymapNotify:  name = "KeymapNotify"; break;
      case Expose:  name = "Expose"; break;
      case GraphicsExpose:  name = "GraphicsExpose"; break;
      case NoExpose:  name = "NoExpose"; break;
      case VisibilityNotify:  name = "VisibilityNotify"; break;
      case CreateNotify:  name = "CreateNotify"; break;
      case DestroyNotify:  name = "DestroyNotify"; break;
      case UnmapNotify:  name = "UnmapNotify"; break;
      case MapNotify:  name = "MapNotify"; break;
      case MapRequest:  name = "MapRequest"; break;
      case ReparentNotify:  name = "ReparentNotify"; break;
      case ConfigureNotify:  name = "ConfigureNotify"; break;
      case ConfigureRequest:  name = "ConfigureRequest"; break;
      case GravityNotify:  name = "GravityNotify"; break;
      case ResizeRequest:  name = "ResizeRequest"; break;
      case CirculateNotify:  name = "CirculateNotify"; break;
      case CirculateRequest:  name = "CirculateRequest"; break;
      case PropertyNotify:  name = "PropertyNotify"; break;
      case SelectionClear:  name = "SelectionClear"; break;
      case SelectionRequest:  name = "SelectionRequest"; break;
      case SelectionNotify:  name = "SelectionNotify"; break;
      case ColormapNotify:  name = "ColormapNotify"; break;
      case ClientMessage:  name = "ClientMessage"; break;
      case MappingNotify:  name = "MappingNotify"; break;
    }

    if (name) {
	printf ("event:  %s, %d remaining\n", name, QLength(dpy));
    } else {
	printf ("unknown event %d, %d remaining\n", e->type, QLength(dpy));
    }
}
#endif /* TRACE */

