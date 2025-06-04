/*
 * Routine to let user select a window using the mouse
 *
 * Borrowed from the DSIMPLE.C routine Select_Window().
 * Modified to work with DECwindows.
 *
 * Calling sequence:
 *	Choose_Window(display, subwindow_flag);
 * where:
 *	display		= display pointer
 *	subwindow_flag	= TRUE	- Translate coordinates to a subwindow ID
 *			  FALSE	- Return parent window ID
 *
 */

#include <stdio.h>
#ifdef VMS
#include <decw$include/Xlib.h>
#include <decw$include/cursorfont.h>
#include "VMSUTIL.H"
#else
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif /* vax11c */

/*
	NOTE: As with almost all applications,
	this code does not work on displays with
	more than one screen because it uses
	DefaultScreen(dpy).
*/

Window Choose_Window(dpy, subwindow_flag)
Display *dpy;
int subwindow_flag;
{
  int status;
  Cursor cursor;
  XEvent event;
  Window target_win = None;
  int buttons = 0;

#ifdef vax11c
  Window root;
  Window child;
  int child_x, child_y;
  extern Window pseudo_root();
  /* 
    For VMS we have to use the "pseudoroot" because 
    of the window manager 
  */
  root = pseudo_root(dpy, DefaultScreen(dpy));
#else
  Window root;
  root = RootWindow(dpy, DefaultScreen(dpy));
#endif


  /* Make the target cursor */
  cursor = XCreateFontCursor(dpy, XC_crosshair);

  /* Grab the pointer using target cursor, letting it room all over */
  status = XGrabPointer(dpy, root, False,
			ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			GrabModeAsync, None, cursor, CurrentTime);
#ifdef vax11c
  if (status != GrabSuccess) {
	fprintf(stderr, "Can't grab the mouse.");
	return((Window) 0);
  }
#else
  if (status != GrabSuccess) Fatal_Error("Can't grab the mouse.");
#endif /* vax11c */

  /* Let the user select a window... */
  while ((target_win == None) || (buttons != 0)) {
    /* allow one more event */
    XAllowEvents(dpy, SyncPointer, CurrentTime);

    XWindowEvent(dpy, root,
		 ButtonPressMask|ButtonReleaseMask, &event);

    switch (event.type) {
    case ButtonPress:
      if (target_win == None) {
	target_win = event.xbutton.subwindow; /* window selected */

#ifdef vax11c
	if (target_win == None)
	    target_win = event.xbutton.window;
	else
	{
	    /* Try to determine if the pointer is inside a child */

	    /*
		Not everybody wants to select a subwindow.
		For example, if the user wants to see what
		the window ID is of a top-level shell widget,
		then this routine must NOT look for a child.
	    */
	    if (subwindow_flag)
	    {
		child = None;
		if (XTranslateCoordinates(dpy, root, target_win,
			event.xbutton.x, event.xbutton.y,
			&child_x, &child_y, &child))
		{
		    if (child != None)
			target_win = child;
		}
	    }
	}
#else
	if (target_win == None)
	    target_win = root;
#endif

      }
      buttons++;
      break;
    case ButtonRelease:
      if (buttons > 0) /* there may have been some down before we started */
	buttons--;
       break;
    }
  } 

  XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */

  return(target_win);

} /*** End choose_window() ***/

