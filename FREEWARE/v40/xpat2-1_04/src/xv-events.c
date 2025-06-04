/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xv-events.c			     */
/*									     */
/*	event handlers for the Xview interface				     */
/*	written by Brandon S. Allbery					     */
/*	based on code by Heiko Eissfeldt and Michael Bischoff		     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/

#ifdef useXview

#include "X-pat.h"

void
Force_Resize(XSize_t w, XSize_t h)
{
    xv_set(canvas,
	   XV_HEIGHT, h + 2,
	   XV_WIDTH, w + 2,
	   0);
    frame_pack_all(mainwindow);
}

void
table_events(Xv_Window w, Event *ev)
{
    switch (event_xevent_type(ev))
    {
    case KeyPress:
	key_press(&event_xevent(ev)->xkey);
	break;
    case MotionNotify:
	mouse_motion(&event_xevent(ev)->xmotion);
	break;
    case ButtonRelease:
	button_release(&event_xevent(ev)->xbutton);
	break;
    case ButtonPress:
	bp_event = ev;		/* forwarded to X-events:menu_show() */
	button_press(&event_xevent(ev)->xbutton);
	break;
    }
}

void
table_resize(Canvas c, int w, int h)
{
    resize_event(w, h);
}

void
table_repaint(Canvas c, Xv_Window w, Rectlist *rl)
{
    XExposeEvent ev;		/* forged from whole cloth... */
    struct rectnode *r;

    for (r = rl->rl_head; r; r = (r == rl->rl_tail? 0: r->rn_next))
    {
	ev.y = rl->rl_y + r->rn_rect.r_top;
	ev.x = rl->rl_x + r->rn_rect.r_left;
	ev.height = r->rn_rect.r_height;
	ev.width = r->rn_rect.r_width;
	ev.count = (r != rl->rl_tail);
	redraw_table(&ev);
    }
}

/*
 * XView lacks a function to match the sizes of components of a frame, so we
 * fake it.
 *
 * NB: this doesn't work, because WIN_FIT_WIDTH changes XV_WIDTH permanently.
 * (In effect, it becomes a hidden extension to the window it affects, and
 * subsequent calls appear to be additive.)
 */

void
frame_pack_all(Frame frame)
{
    Xv_Window win;
    int n, w, maxw;

    maxw = 0;
    /* I think 1 is the footer. */
    for (n = 2; (win = (Xv_Window) xv_get(frame, FRAME_NTH_SUBWINDOW, n)); n++)
    {
	window_fit(win);
	if ((w = xv_get(win, XV_WIDTH)) > maxw)
	    maxw = w;
    }
#if 0
    for (n = 2; (win = (Xv_Window) xv_get(frame, FRAME_NTH_SUBWINDOW, n)); n++)
	xv_set(win, WIN_FIT_WIDTH, maxw - xv_get(win, XV_WIDTH), 0);
#endif
    for (n = 2; (win = (Xv_Window) xv_get(frame, FRAME_NTH_SUBWINDOW, n)); n++)
	xv_set(win, XV_WIDTH, maxw, 0);
    window_fit(frame);
}

#endif
