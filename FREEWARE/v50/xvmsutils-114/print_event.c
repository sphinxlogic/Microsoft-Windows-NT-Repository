/*
 * xev - event diagnostics
 *
 * $XConsortium: xev.c,v 1.8 88/10/09 15:44:28 rws Exp $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <ctype.h>


static char *Yes = "YES";
static char *No = "NO";
static char *Unknown = "unknown";



static void prologue (eventp, event_name)
XEvent *eventp;
char *event_name;
{
    XAnyEvent *e = (XAnyEvent *) eventp;

    printf ("\n%s event, serial %ld, synthetic %s, window 0x%lx,\n",
	    event_name, e->serial, e->send_event ? Yes : No, e->window);
    return;
}


static void do_KeyPress (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XKeyEvent *e = (XKeyEvent *) eventp;
    KeySym ks;
    char *ksname;
    int nbytes;
    int keycode;
    char str[256+1];

    nbytes = XLookupString ((XKeyEvent *) eventp, str, 256, &ks, NULL);
    if (ks == NoSymbol)
	ksname = "NoSymbol";
    else if (!(ksname = XKeysymToString (ks)))
	ksname = "(no name)";
    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, keycode %u (keysym 0x%x, %s), same_screen %s,\n",
	    e->state, e->keycode, ks, ksname, e->same_screen ? Yes : No);
    if (nbytes < 0) nbytes = 0;
    if (nbytes > 256) nbytes = 256;
    str[nbytes] = '\0';
    printf ("    XLookupString gives %d characters:  \"%s\"\n", nbytes, str);

    return;
}


static void do_KeyRelease (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    do_KeyPress (dpy, eventp);		/* since it has the same info */
    return;
}


static void do_ButtonPress (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XButtonEvent *e = (XButtonEvent *) eventp;

    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, button %u, same_screen %s\n",
	    e->state, e->button, e->same_screen ? Yes : No);

    return;
}


static void do_ButtonRelease (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    do_ButtonPress (dpy, eventp);	/* since it has the same info */
    return;
}


static void do_MotionNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XMotionEvent *e = (XMotionEvent *) eventp;

    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, is_hint %u, same_screen %s\n",
	    e->state, e->is_hint, e->same_screen ? Yes : No);

    return;
}


static void do_EnterNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XCrossingEvent *e = (XCrossingEvent *) eventp;
    char *mode, *detail;
    char dmode[10], ddetail[10];

    switch (e->mode) {
      case NotifyNormal:  mode = "NotifyNormal"; break;
      case NotifyGrab:  mode = "NotifyGrab"; break;
      case NotifyUngrab:  mode = "NotifyUngrab"; break;
      case NotifyWhileGrabbed:  mode = "NotifyWhileGrabbed"; break;
      default:  mode = dmode, sprintf (dmode, "%u", e->mode); break;
    }

    switch (e->detail) {
      case NotifyAncestor:  detail = "NotifyAncestor"; break;
      case NotifyVirtual:  detail = "NotifyVirtual"; break;
      case NotifyInferior:  detail = "NotifyInferior"; break;
      case NotifyNonlinear:  detail = "NotifyNonlinear"; break;
      case NotifyNonlinearVirtual:  detail = "NotifyNonlinearVirtual"; break;
      case NotifyPointer:  detail = "NotifyPointer"; break;
      case NotifyPointerRoot:  detail = "NotifyPointerRoot"; break;
      case NotifyDetailNone:  detail = "NotifyDetailNone"; break;
      default:  detail = ddetail; sprintf (ddetail, "%u", e->detail); break;
    }

    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    mode %s, detail %s, same_screen %s,\n",
	    mode, detail, e->same_screen ? Yes : No);
    printf ("    focus %s, state %u\n", e->focus ? Yes : No, e->state);

    return;
}


static void do_LeaveNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    do_EnterNotify (dpy, eventp);	/* since it has same information */
    return;
}


static void do_FocusIn (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XFocusChangeEvent *e = (XFocusChangeEvent *) eventp;
    char *mode, *detail;
    char dmode[10], ddetail[10];

    switch (e->mode) {
      case NotifyNormal:  mode = "NotifyNormal"; break;
      case NotifyGrab:  mode = "NotifyGrab"; break;
      case NotifyUngrab:  mode = "NotifyUngrab"; break;
      case NotifyWhileGrabbed:  mode = "NotifyWhileGrabbed"; break;
      default:  mode = dmode, sprintf (dmode, "%u", e->mode); break;
    }

    switch (e->detail) {
      case NotifyAncestor:  detail = "NotifyAncestor"; break;
      case NotifyVirtual:  detail = "NotifyVirtual"; break;
      case NotifyInferior:  detail = "NotifyInferior"; break;
      case NotifyNonlinear:  detail = "NotifyNonlinear"; break;
      case NotifyNonlinearVirtual:  detail = "NotifyNonlinearVirtual"; break;
      case NotifyPointer:  detail = "NotifyPointer"; break;
      case NotifyPointerRoot:  detail = "NotifyPointerRoot"; break;
      case NotifyDetailNone:  detail = "NotifyDetailNone"; break;
      default:  detail = ddetail; sprintf (ddetail, "%u", e->detail); break;
    }

    printf ("    mode %s, detail %s\n", mode, detail);
    return;
}


static void do_FocusOut (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    do_FocusIn (dpy, eventp);		/* since it has same information */
    return;
}


static void do_KeymapNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XKeymapEvent *e = (XKeymapEvent *) eventp;
    int i, j;

    printf ("    keys:  ");
    for (i = 0; i < 32; i++) {
	if (i == 16) printf ("\n           ");
	printf ("%-3u ", (unsigned int) e->key_vector[i]);
    }
    printf ("\n");
    return;
}


static void do_Expose (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XExposeEvent *e = (XExposeEvent *) eventp;

    printf ("    (%d,%d), width %d, height %d, count %d\n",
	    e->x, e->y, e->width, e->height, e->count);
    return;
}


static void do_GraphicsExpose (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XGraphicsExposeEvent *e = (XGraphicsExposeEvent *) eventp;
    char *m;
    char mdummy[10];

    switch (e->major_code) {
      case X_CopyArea:  m = "CopyArea";  break;
      case X_CopyPlane:  m = "CopyPlane";  break;
      default:  m = mdummy; sprintf (mdummy, "%d", e->major_code); break;
    }

    printf ("    (%d,%d), width %d, height %d, count %d,\n",
	    e->x, e->y, e->width, e->height, e->count);
    printf ("    major %s, minor %d\n", m, e->minor_code);
    return;
}


static void do_NoExpose (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XNoExposeEvent *e = (XNoExposeEvent *) eventp;
    char *m;
    char mdummy[10];

    switch (e->major_code) {
      case X_CopyArea:  m = "CopyArea";  break;
      case X_CopyPlane:  m = "CopyPlane";  break;
      default:  m = mdummy; sprintf (mdummy, "%d", e->major_code); break;
    }

    printf ("    major %s, minor %d\n", m, e->minor_code);
    return;
}


static void do_VisibilityNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XVisibilityEvent *e = (XVisibilityEvent *) eventp;
    char *v;
    char vdummy[10];

    switch (e->state) {
      case VisibilityUnobscured:  v = "VisibilityUnobscured"; break;
      case VisibilityPartiallyObscured:  v = "VisibilityPartiallyObscured"; break;
      case VisibilityFullyObscured:  v = "VisibilityFullyObscured"; break;
      default:  v = vdummy; sprintf (vdummy, "%d", e->state); break;
    }

    printf ("    state %s\n", v);
    return;
}


static void do_CreateNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XCreateWindowEvent *e = (XCreateWindowEvent *) eventp;

    printf ("    parent 0x%lx, window 0x%lx, (%d,%d), width %d, height %d\n",
	    e->parent, e->window, e->x, e->y, e->width, e->height);
    printf ("border_width %d, override %s\n",
	    e->border_width, e->override_redirect ? Yes : No);
    return;
}


static void do_DestroyNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XDestroyWindowEvent *e = (XDestroyWindowEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx\n", e->event, e->window);
    return;
}


static void do_UnmapNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XUnmapEvent *e = (XUnmapEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx, from_configure %s\n",
	    e->event, e->window, e->from_configure ? Yes : No);
    return;
}


static void do_MapNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XMapEvent *e = (XMapEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx, override %s\n",
	    e->event, e->window, e->override_redirect ? Yes : No);
    return;
}


static void do_MapRequest (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XMapRequestEvent *e = (XMapRequestEvent *) eventp;

    printf ("    parent 0x%lx, window 0x%lx\n", e->parent, e->window);
    return;
}


static void do_ReparentNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XReparentEvent *e = (XReparentEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx, parent 0x%lx,\n",
	    e->event, e->window, e->parent);
    printf ("    (%d,%d), override %s\n", e->x, e->y, 
	    e->override_redirect ? Yes : No);
    return;
}


static void do_ConfigureNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XConfigureEvent *e = (XConfigureEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx, (%d,%d), width %d, height %d,\n",
	    e->event, e->window, e->x, e->y, e->width, e->height);
    printf ("    border_width %d, above 0x%lx, override %s\n",
	    e->border_width, e->above, e->override_redirect ? Yes : No);
    return;
}


static void do_ConfigureRequest (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XConfigureRequestEvent *e = (XConfigureRequestEvent *) eventp;
    char *detail;
    char ddummy[10];

    switch (e->detail) {
      case Above:  detail = "Above";  break;
      case Below:  detail = "Below";  break;
      case TopIf:  detail = "TopIf";  break;
      case BottomIf:  detail = "BottomIf"; break;
      case Opposite:  detail = "Opposite"; break;
      default:  detail = ddummy; sprintf (ddummy, "%d", e->detail); break;
    }

    printf ("    parent 0x%lx, window 0x%lx, (%d,%d), width %d, height %d,\n",
	    e->parent, e->window, e->x, e->y, e->width, e->height);
    printf ("    border_width %d, above 0x%lx, detail %s, value 0x%lx\n",
	    e->border_width, e->above, detail, e->value_mask);
    return;
}


static void do_GravityNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XGravityEvent *e = (XGravityEvent *) eventp;

    printf ("    event 0x%lx, window 0x%lx, (%d,%d)\n",
	    e->event, e->window, e->x, e->y);
    return;
}


static void do_ResizeRequest (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XResizeRequestEvent *e = (XResizeRequestEvent *) eventp;

    printf ("    width %d, height %d\n", e->width, e->height);
    return;
}


static void do_CirculateNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XCirculateEvent *e = (XCirculateEvent *) eventp;
    char *p;
    char pdummy[10];

    switch (e->place) {
      case PlaceOnTop:  p = "PlaceOnTop"; break;
      case PlaceOnBottom:  p = "PlaceOnBottom"; break;
      default:  p = pdummy; sprintf (pdummy, "%d", e->place); break;
    }

    printf ("    event 0x%lx, window 0x%lx, place %s\n",
	    e->event, e->window, p);
    return;
}


static void do_CirculateRequest (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XCirculateRequestEvent *e = (XCirculateRequestEvent *) eventp;
    char *p;
    char pdummy[10];

    switch (e->place) {
      case PlaceOnTop:  p = "PlaceOnTop"; break;
      case PlaceOnBottom:  p = "PlaceOnBottom"; break;
      default:  p = pdummy; sprintf (pdummy, "%d", e->place); break;
    }

    printf ("    parent 0x%lx, window 0x%lx, place %s\n",
	    e->parent, e->window, p);
    return;
}


static void do_PropertyNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XPropertyEvent *e = (XPropertyEvent *) eventp;
    char *aname = XGetAtomName (dpy, e->atom);
    char *s;
    char sdummy[10];

    switch (e->state) {
      case PropertyNewValue:  s = "PropertyNewValue"; break;
      case PropertyDelete:  s = "PropertyDelete"; break;
      default:  s = sdummy; sprintf (sdummy, "%d", e->state); break;
    }

    printf ("    atom 0x%lx (%s), time %lu, state %s\n",
	   e->atom, aname ? aname : Unknown, e->time,  s);

    if (aname) XFree (aname);
    return;
}


static void do_SelectionClear (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XSelectionClearEvent *e = (XSelectionClearEvent *) eventp;
    char *sname = XGetAtomName (dpy, e->selection);

    printf ("    selection 0x%lx (%s), time %lu\n",
	    e->selection, sname ? sname : Unknown, e->time);

    if (sname) XFree (sname);
    return;
}


static void do_SelectionRequest (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XSelectionRequestEvent *e = (XSelectionRequestEvent *) eventp;
    char *sname = XGetAtomName (dpy, e->selection);
    char *tname = XGetAtomName (dpy, e->target);
    char *pname = XGetAtomName (dpy, e->property);

    printf ("    owner 0x%lx, requestor 0x%lx, selection 0x%lx (%s),\n",
	    e->owner, e->requestor, e->selection, sname ? sname : Unknown);
    printf ("    target 0x%lx (%s), property 0x%lx (%s), time %lu\n",
	    e->target, tname ? tname : Unknown, e->property,
	    pname ? pname : Unknown, e->time);

    if (sname) XFree (sname);
    if (tname) XFree (tname);
    if (pname) XFree (pname);

    return;
}


static void do_SelectionNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XSelectionEvent *e = (XSelectionEvent *) eventp;
    char *sname = XGetAtomName (dpy, e->selection);
    char *tname = XGetAtomName (dpy, e->target);
    char *pname = XGetAtomName (dpy, e->property);

    printf ("    selection 0x%lx (%s), target 0x%lx (%s),\n",
	    e->selection, sname ? sname : Unknown, e->target,
	    tname ? tname : Unknown);
    printf ("    property 0x%lx (%s), time %lu\n",
	    e->property, pname ? pname : Unknown, e->time);

    if (sname) XFree (sname);
    if (tname) XFree (tname);
    if (pname) XFree (pname);

    return;
}


static void do_ColormapNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XColormapEvent *e = (XColormapEvent *) eventp;
    char *s;
    char sdummy[10];

    switch (e->state) {
      case ColormapInstalled:  s = "ColormapInstalled"; break;
      case ColormapUninstalled:  s = "ColormapUninstalled"; break;
      default:  s = sdummy; sprintf (sdummy, "%d", e->state); break;
    }

    printf ("    colormap 0x%lx, new %s, state %s\n",
	    e->colormap, e->new ? Yes : No, s);
    return;
}


static void do_ClientMessage (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XClientMessageEvent *e = (XClientMessageEvent *) eventp;
    char *mname = XGetAtomName (dpy, e->message_type);

    printf ("    message_type 0x%lx (%s), format %d\n",
	    e->message_type, mname ? mname : Unknown, e->format);

    if (mname) XFree (mname);
    return;
}


static void do_MappingNotify (dpy, eventp)
Display *dpy;
XEvent *eventp;
{
    XMappingEvent *e = (XMappingEvent *) eventp;
    char *r;
    char rdummy[10];

    switch (e->request) {
      case MappingModifier:  r = "MappingModifier"; break;
      case MappingKeyboard:  r = "MappingKeyboard"; break;
      case MappingPointer:  r = "MappingPointer"; break;
      default:  r = rdummy; sprintf (rdummy, "%d", e->request); break;
    }

    printf ("    request %s, first_keycode %d, count %d\n",
	    r, e->first_keycode, e->count);
    return;
}


print_X_event(dpy, event)
Display *dpy;
XEvent *event;
{

    switch (event->type)
    {
	case KeyPress:
	    prologue (event, "KeyPress");
	    do_KeyPress (dpy, event);
	    break;
	case KeyRelease:
	    prologue (event, "KeyRelease");
	    do_KeyRelease (dpy, event);
	    break;
	case ButtonPress:
	    prologue (event, "ButtonPress");
	    do_ButtonPress (dpy, event);
	    break;
	case ButtonRelease:
	    prologue (event, "ButtonRelease");
	    do_ButtonRelease (dpy, event);
	    break;
	case MotionNotify:
	    prologue (event, "MotionNotify");
	    do_MotionNotify (dpy, event);
	    break;
	case EnterNotify:
	    prologue (event, "EnterNotify");
	    do_EnterNotify (dpy, event);
	    break;
	case LeaveNotify:
	    prologue (event, "LeaveNotify");
	    do_LeaveNotify (dpy, event);
	    break;
	case FocusIn:
	    prologue (event, "FocusIn");
	    do_FocusIn (dpy, event);
	    break;
	case FocusOut:
	    prologue (event, "FocusOut");
	    do_FocusOut (dpy, event);
	    break;
	case KeymapNotify:
	    prologue (event, "KeymapNotify");
	    do_KeymapNotify (dpy, event);
	    break;
	case Expose:
	    prologue (event, "Expose");
	    do_Expose (dpy, event);
	    break;
	case GraphicsExpose:
	    prologue (event, "GraphicsExpose");
	    do_GraphicsExpose (dpy, event);
	    break;
	case NoExpose:
	    prologue (event, "NoExpose");
	    do_NoExpose (dpy, event);
	    break;
	case VisibilityNotify:
	    prologue (event, "VisibilityNotify");
	    do_VisibilityNotify (dpy, event);
	    break;
	case CreateNotify:
	    prologue (event, "CreateNotify");
	    do_CreateNotify (dpy, event);
	    break;
	case DestroyNotify:
	    prologue (event, "DestroyNotify");
	    do_DestroyNotify (dpy, event);
	    break;
	case UnmapNotify:
	    prologue (event, "UnmapNotify");
	    do_UnmapNotify (dpy, event);
	    break;
	case MapNotify:
	    prologue (event, "MapNotify");
	    do_MapNotify (dpy, event);
	    break;
	case MapRequest:
	    prologue (event, "MapRequest");
	    do_MapRequest (dpy, event);
	    break;
	case ReparentNotify:
	    prologue (event, "ReparentNotify");
	    do_ReparentNotify (dpy, event);
	    break;
	case ConfigureNotify:
	    prologue (event, "ConfigureNotify");
	    do_ConfigureNotify (dpy, event);
	    break;
	case ConfigureRequest:
	    prologue (event, "ConfigureRequest");
	    do_ConfigureRequest (dpy, event);
	    break;
	case GravityNotify:
	    prologue (event, "GravityNotify");
	    do_GravityNotify (dpy, event);
	    break;
	case ResizeRequest:
	    prologue (event, "ResizeRequest");
	    do_ResizeRequest (dpy, event);
	    break;
	case CirculateNotify:
	    prologue (event, "CirculateNotify");
	    do_CirculateNotify (dpy, event);
	    break;
	case CirculateRequest:
	    prologue (event, "CirculateRequest");
	    do_CirculateRequest (dpy, event);
	    break;
	case PropertyNotify:
	    prologue (event, "PropertyNotify");
	    do_PropertyNotify (dpy, event);
	    break;
	case SelectionClear:
	    prologue (event, "SelectionClear");
	    do_SelectionClear (dpy, event);
	    break;
	case SelectionRequest:
	    prologue (event, "SelectionRequest");
	    do_SelectionRequest (dpy, event);
	    break;
	case SelectionNotify:
	    prologue (event, "SelectionNotify");
	    do_SelectionNotify (dpy, event);
	    break;
	case ColormapNotify:
	    prologue (event, "ColormapNotify");
	    do_ColormapNotify (dpy, event);
	    break;
	case ClientMessage:
	    prologue (event, "ClientMessage");
	    do_ClientMessage (dpy, event);
	    break;
	case MappingNotify:
	    prologue (event, "MappingNotify");
	    do_MappingNotify (dpy, event);
	    break;
	default:
	    printf ("Unknown event type %d\n", event->type);
	    break;
    }

}

