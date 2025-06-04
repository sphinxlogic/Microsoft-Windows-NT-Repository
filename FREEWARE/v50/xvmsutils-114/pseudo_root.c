/*
	pseudo_root - Return the Window ID of the Pseudo Root

	NOTE: This routine is required for DECwindows
	because the true root window is hidden behind
	a pseudo-root window created by the window manager.

	Calling Sequence:
		root_win = pseudo_root(display, screen);
	where:
		display must already be opened and
		screen is usually DefaultScreen(display).

*/

#include <X11/Xlib.h>

#ifdef NULL
#undef NULL
#endif
#define	NULL	0L

static void chase_root();

static int done;
static Window last_win;
static int root_x, root_y, root_width, root_height;
static int root_bw, root_depth;


Window pseudo_root(dpy, screen)
Display *dpy;
int screen;
{
    Window root, win;

    /* Start at the real root */
    root = RootWindow(dpy, screen);

    /* Get the geometry of the root */
    if (XGetGeometry(dpy, root, &win, &root_x, &root_y, 
			(unsigned int *) &root_width, 
                        (unsigned int *) &root_height,
			(unsigned int *) &root_bw, 
                        (unsigned int *) &root_depth))
    {
	/* Set up for the tree walk */
	done = False;
	last_win = root;

	/* Run down the tree to find the pseudo root */
	chase_root(dpy, root);
	return last_win;
    }
    else
	/* Classic case of "this should never happen" */
	return root;

} /*** End pseudo_root() ***/


/*
	chase_root - Internal to this module

	This is a recursive routine for descending the window tree.

	It looks for the first window which does NOT overlay the root,
	then returns with the ID of the last window it saw in the
	global variable last_win.

	NOTE: The parameters of the root window must be set up before
	calling chase_root().
*/

static void chase_root (dpy, w)
Display *dpy;
Window w;
{
    Window root, parent;
    unsigned int nchildren;
    Window *children = NULL;
    Status status;
    int n;
    int x, y, rx, ry;
    unsigned int width, height, bw, depth;

    /* Insurance */
    if (done)
	return;

    if (XGetGeometry(dpy, w, &root, &x, &y, &width, &height, &bw, &depth))
    {
	/*
	    If this window does not exactly overlay the root
	    then we have gone one too far, i.e., we have finished.
	*/
	if ( (x != root_x) ||
	     (y != root_y) ||
	     (width  != root_width) ||
	     (height != root_height) )
	{
	    done = True;
	    return;
	}

	/* Otherwise, remember where we got up to and continue */
	else
	    last_win = w;
    }

    else
	/* We are in trouble if this happens!!! */
	return;

    if (!XQueryTree (dpy, w, &root, &parent, &children, &nchildren))
	/* Likewise, we hope we never get here!!! */
	return;

    for (n = 0; n < nchildren; n++)
    {
	chase_root (dpy, children[n]);
	if (done)
	    break;
    }
#ifdef VMS
    if (children) XFree ((char *) children);
#else
    if (children) free ((char *) children);
#endif
    return;

} /*** End chase_root() ***/

