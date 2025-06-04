/* SMPAUSEWINDOW.C - Routine to find the Session Manager pause window for   */
/*                   SSB DECwindows MOTIF on VMS.                           */

#include <decw$include/Xatom.h>
#include <decw$include/Xlib.h>

static int SmPauseHandler(dpy, e)
    Display *dpy;
    XErrorEvent *e;
{
    /* ignore errors from GetWindowProperty */
    
    return 0;
}


Window SmPauseWindow(dpy, root)
    Display *dpy;
    Window root;
{
    char *pause_window = "_DEC_SM_PAUSE_WINDOW";
    Atom pause_window_atom; 
    Window *wid;
    Window retval;
    Atom type_returned;
    int format_returned, status;
    unsigned long num_items_returned, bytes_remaining;

    /* enable handler to trap errors (especially BadAtom) */
    
    XSetErrorHandler(&SmPauseHandler);

    pause_window_atom = XInternAtom (dpy, pause_window, True);

    status = XGetWindowProperty (dpy, root, pause_window_atom, 0, 1, False,
				 XA_WINDOW, &type_returned, &format_returned,
				 &num_items_returned, &bytes_remaining, &wid);

    /* disable the handler */
    
    XSetErrorHandler(0);

    if ((status != 0) || (type_returned == None))
	return 0;	/* XGetWindowProperty failed */

    retval = *wid;
    XFree(wid);
    return retval;	/* Success */
}
