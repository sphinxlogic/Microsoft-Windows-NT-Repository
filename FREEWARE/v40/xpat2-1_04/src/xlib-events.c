/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xlib-events.c			     */
/*									     */
/*	Additional event handlers for the Xlib interface.		     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useXlib
#include "X-pat.h"

/* helper functions for Xlib interface */

/* dispatcher for event on table */
static void handle_table_event(XEvent *xev) {
    switch(xev->xany.type) {
     case ButtonPress:	button_press(&xev->xbutton);
			break;
     case ButtonRelease:button_release(&xev->xbutton);
                        break;
     case MotionNotify: mouse_motion(&xev->xmotion);
                        break;
     case KeyPress:	key_press(&xev->xkey);
			break;
     case Expose:	redraw_table(&xev->xexpose);
			break;
     case ConfigureNotify:resize_event(
			       xev->xconfigure.width,
			       xev->xconfigure.height);
			break;
     }
}

void (*check_button_list(struct singlebutton *p, int num,
    XButtonPressedEvent *xev))(void) {
    while (num--) {
	if (xev->x >= p->x && xev->x < p->x + p->w &&
	    xev->y >= p->y && xev->y < p->y + p->h) {
	    switch (xev->button) {
	    case Button1:
		return p->left;
	    case Button2:
		return p->middle;
	    case Button3:
		return p->right;
	    }
	}
	++p;
    }
    return NULL;
}

/* events in confirm window */
static void conf_button_press(XButtonPressedEvent *xev) {
    void (*func)(void);
    func = check_button_list(confirmbuttons, 2, xev);
    if (func)
	(*func)();
}

static void redraw_confirm(XExposeEvent *xev) {
    XFillRectangle(dpy, confirm.win, whitegc, 0, 0, confirm.w-1, confirm.h-1);
    /* draw the text */
    XDrawImageString(dpy, confirm.win, button.gc,
        graphic.xgap, button.by+button.font->ascent, "Please confirm", 14);
    XDrawImageString(dpy, confirm.win, button.gc, graphic.xgap,
        2 * button.by+button.font->ascent + button.fontheight, confirm.text,
        strlen(confirm.text));
    redraw_buttons(0, 0, confirm.w, confirm.h, confirm.win, 2, confirmbuttons);
}

static void handle_confirm_event(XEvent	*xev) {
    switch(xev->xany.type) {
    case ButtonPress:
	conf_button_press(&xev->xbutton);
	break;
    case KeyPress:
	key_press(&xev->xkey); /* go same window */
	break;
    }
}


static void redraw_finwin(XExposeEvent *xev) {
    XFillRectangle(dpy, finished_win, whitegc, 0, 0,
		   FINISHED_W-1, FINISHED_H-1);
    /* draw the text */
    XDrawImageString(dpy, finished_win, button.gc,
		     (FINISHED_W - XTextWidth(button.font, "GONZO!", 6))/2,
		     FINISHED_H * 2 / 3, "GONZO!", 6);
    XFlush(dpy);
}

void handle_expose_event(XExposeEvent *xev) {
    if (xev->window == confirm.win)
    	redraw_confirm(xev);
    else if (xev->window == finished_win)
	redraw_finwin(xev);
}

void event_loop(void) {
    XEvent xev;

    cmd_CancelSelection();
    while (1) {
#if 0
	if (game.ind[FIRST_SLOT] == rules.cards_per_color * rules.numstacks
	    && !game.finished) {
	    /* all cards on the stacks and not yet notified */
	    game.finished = True;
	    XMoveWindow(dpy, finished_win, (graphic.width - FINISHED_W) / 2,
			(graphic.height - FINISHED_H) / 2);
	    XMapWindow(dpy, finished_win);
	    redraw_finwin((XExposeEvent *)0);
	    show_message("You did it!");
	    write_log_file();
	    do_music(&xev);	/* play until next event */
	    XUnmapWindow(dpy, finished_win);
	} else
#endif
	    XNextEvent(dpy, &xev);

	if (xev.xany.window == table)
	    handle_table_event(&xev);
	else if (xev.xany.type == Expose) /* expose events are treated separately */
	    handle_expose_event(&xev.xexpose);
	else if (xev.xany.window == confirm.win)
	    handle_confirm_event(&xev);
    }
}
#endif
