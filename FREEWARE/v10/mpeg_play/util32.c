#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "video.h"
#include "proto.h"
#include "ui.h"                         /* include user interface */

/*
 * Return a pointer to a full color bit visual on the dpy
 */
Visual *
FindFullColorVisual (dpy, depth)
    Display *dpy;
    int *depth;
{
  XVisualInfo vinfo;
  XVisualInfo *vinfo_ret;
  int numitems, maxdepth;
  
  vinfo.class = TrueColor;
  
  vinfo_ret = XGetVisualInfo(dpy, VisualClassMask, &vinfo, &numitems);
  
  if (numitems == 0) return NULL;

  maxdepth = 0;
  while(numitems > 0) {
    if (vinfo_ret[numitems-1].depth > maxdepth) {
      maxdepth = vinfo_ret[numitems-1 ].depth;
    }
    numitems--;
  }
  XFree(vinfo_ret);

  if (maxdepth < 24) return NULL;

  if (XMatchVisualInfo(dpy, DefaultScreen(dpy), maxdepth, 
		       TrueColor, &vinfo)) {
    *depth = maxdepth;
    return vinfo.visual;
  }
  
  return NULL;
}

Window
CreateFullColorWindow (dpy, x, y, w, h)
    Display *dpy;
    int x, y, w, h;
{
    int depth;
    Visual *visual;
    XSetWindowAttributes xswa;
    Window temp;
    unsigned int mask;
    unsigned int class;
    int screen;

    screen = XDefaultScreen(dpy);
    class = InputOutput;	/* Could be InputOnly */
    visual = FindFullColorVisual (dpy, &depth);
    if (visual == NULL) {
	return 0;
    }
    mask = CWBackPixel | CWColormap | CWBorderPixel;
    xswa.colormap = XCreateColormap(dpy, XRootWindow(dpy, screen),
		    visual, AllocNone);
    xswa.background_pixel = BlackPixel(dpy, DefaultScreen(dpy));
    xswa.border_pixel = WhitePixel(dpy, DefaultScreen(dpy));

    temp = XCreateWindow(dpy, MonitorWindow(), x, y, w, h,
	1, depth, class, visual, mask, &xswa);

    UISetwin(temp);

    return temp;
}

/*
 *  This is a little hack that I'm calling up front to see if the
 *  display is capable of truecolor display.  This allows it to
 *  run in truecolor on a system that supports it, and fall back to
 *  a dither if not.  Rather than needing to know up-front
 *
 */
Bool
CheckFullColorWindow (name)
char *name;
{
    Display *dpy;
    int depth;             
    Visual *visual;

    dpy = XOpenDisplay(name);
    if (!dpy) return FALSE;
    visual = FindFullColorVisual (dpy, &depth);
    XCloseDisplay(dpy);
    if (visual == NULL) {
	return FALSE;
    }
    return TRUE;
}
