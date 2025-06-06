#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include "config.h"
#ifdef GPM_MOUSEDRIVER
#include <gpm.h>
#include <ncurses.h>
#include "aalib.h"
#if 0
extern int __curses_is_up;
extern int __curses_keyboard;
#endif
static Gpm_Connect conn;
int __curses_usegpm;
static int mousex, mousey, mousebuttons;
static int user_handler(Gpm_Event * event, void *data)
{
    mousex = event->x;
    mousey = event->y;
    mousebuttons = event->buttons;
    if (event->type &= GPM_UP)
	mousebuttons = 0;
    return KEY_MOUSE;
}

static int gpm_init(struct aa_context *context, int mode)
{
    /*conn.eventMask=~0; */
    conn.eventMask = (mode & AA_MOUSEMOVEMASK ? GPM_MOVE | GPM_DRAG : 0) | GPM_DOWN | GPM_UP;
    conn.defaultMask = 0;
    conn.maxMod = ~0;
    conn.minMod = 0;
    if (Gpm_Open(&conn, 0) < 0)
	return 0;
    __curses_usegpm = 1;
    gpm_handler = user_handler;
    gpm_visiblepointer = 1;
    gpm_hflag = 1;
    return 1;
}
static void gpm_uninit(aa_context * c)
{
    __curses_usegpm = 0;
    Gpm_Close();
}

static void gpm_mouse(aa_context * c, int *x, int *y, int *b)
{
    *x = mousex;
    *y = mousey;
    *b = 0;
    if (mousebuttons & 4)
	*b |= AA_BUTTON1;
    if (mousebuttons & 2)
	*b |= AA_BUTTON2;
    if (mousebuttons & 1)
	*b |= AA_BUTTON3;
}

struct aa_mousedriver mouse_gpm_d =
{
    "gpm", "Gpm mouse driver 1.0",
    AA_MOUSEALLMASK,
    gpm_init,
    gpm_uninit,
    gpm_mouse
};
#endif
