#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#ifdef X11_MOUSEDRIVER
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
#define fontwidth __X_fontwidth
#define fontheigth __X_fontheight
extern Display *dp;
extern Window wi;
extern Pixmap pi;
extern XSetWindowAttributes attr;
extern int fontheight;
extern int fontwidth;


static int X_init(aa_context * c, int mode)
{
    if (dp == NULL)
	return 0;
    attr.event_mask |= ButtonPressMask | ButtonReleaseMask |
	(mode & AA_MOUSEMOVEMASK ? PointerMotionMask : 0);
    /*|(mode&AA_PRESSEDMOVEMAKS?ButtonMotionMask:0); */
    XSelectInput(dp, wi, attr.event_mask);
    return 1;
}
static void X_uninit(aa_context * c)
{
    attr.event_mask &= ~(ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask);
    XSelectInput(dp, wi, attr.event_mask);
}

int __X_mousex, __X_mousey, __X_buttons;
static void X_getmouse(aa_context * c, int *x, int *y, int *b)
{
    *x = __X_mousex / fontwidth;
    *y = __X_mousey / fontheight;
    *b = 0;
    if (__X_buttons & Button1Mask)
	*b |= AA_BUTTON1;
    if (__X_buttons & Button2Mask)
	*b |= AA_BUTTON2;
    if (__X_buttons & Button3Mask)
	*b |= AA_BUTTON3;
#if 0
    if (__X_buttons & 256)
	*b |= AA_BUTTON1;
    if (__X_buttons & 512)
	*b |= AA_BUTTON2;
    if (__X_buttons & 1024)
	*b |= AA_BUTTON3;
#endif
}
struct aa_mousedriver mouse_X11_d =
{
    "X11", "X11 mouse driver 1.0",
    AA_MOUSEALLMASK,
    X_init,
    X_uninit,
    X_getmouse,
};
#endif
