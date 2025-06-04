#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#ifdef X11_KBDDRIVER
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include "aalib.h"
struct aa_driver X11_d;
#define dp __X_display
#define wi __X_window
#define pi __X_pixmap
#define attr __X_attr
#define screen __X_screen
#define dr (pixmapmode?pi:wi)
extern Display *dp;
extern Window wi;
extern Pixmap pi;
extern XSetWindowAttributes attr;

static int X_init(aa_context * c, int mode)
{
    if (dp == NULL)
	return 0;
    attr.event_mask |= StructureNotifyMask | KeyPressMask | (mode & AA_SENDRELEASE ? KeyReleaseMask : 0);
    XSelectInput(dp, wi, attr.event_mask);
    aa_recommendlowmouse("X11");
    return 1;
}
static void X_uninit(aa_context * c)
{
    attr.event_mask &= ~(KeyPressMask | KeyReleaseMask);
    XSelectInput(dp, wi, attr.event_mask);
}
static int decodekey(XEvent * ev)
{
    KeySym ksym;
    char *name;
    switch (ksym = XLookupKeysym(&ev->xkey, ev->xkey.state)) {
    case XK_Left:
	return (AA_LEFT);
    case XK_Right:
	return (AA_RIGHT);
    case XK_Up:
	return (AA_UP);
    case XK_Down:
	return (AA_DOWN);
    case XK_Escape:
	return (AA_ESC);
    case XK_space:
	return (' ');
    case XK_Return:
#ifdef XK_cr
    case XK_cr:
#endif
	return (13);
    case XK_BackSpace:
    case XK_Delete:
	return (AA_BACKSPACE);
    case XK_division:
	return ('/');
    case XK_question:
	return ('?');
    }
    name = XKeysymToString(ksym);
    if (name == NULL)
	return (AA_UNKNOWN + ksym);
    if (strlen(name) != 1)
	return (AA_UNKNOWN + ksym);
    return (name[0]);

}
extern int __X_getsize(void);
int __X_mousex, __X_mousey, __X_buttons;
static int X_getchar(aa_context * c, int wait)
{
    while (1) {
	XEvent ev;
	if (!wait && !XPending(dp))
	    return AA_NONE;
	XNextEvent(dp, &ev);
	switch (ev.type) {
	case ButtonPress:
	    ev.xbutton.state |= 1 << (ev.xbutton.button + 7);
	    goto skip;
	case ButtonRelease:
	    ev.xbutton.state &= ~(1 << (ev.xbutton.button + 7));
	  skip:;
	case MotionNotify:
	    __X_mousex = ev.xbutton.x;
	    __X_mousey = ev.xbutton.y;
	    __X_buttons = ev.xbutton.state;
	    return (AA_MOUSE);
	case Expose:
	    XSync(dp, 0);
	    XClearWindow(dp, wi);
	    break;
	case ConfigureNotify:
	    if (__X_getsize())
		return (AA_RESIZE);
	    break;
	case KeyPress:
	    return (decodekey(&ev));
	case KeyRelease:
	    return (decodekey(&ev) & AA_RELEASE);

	}
    }
}

struct aa_kbddriver kbd_X11_d =
{
    "X11", "X11 keyboard driver 1.0",
    AA_SENDRELEASE,
    X_init,
    X_uninit,
    X_getchar,
};
#endif
