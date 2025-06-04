/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-events.c			     */
/*									     */
/*	Event handlers for the X interface				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

Display *dpy;
int screen;
unsigned long blackpixel;
unsigned long whitepixel;
GC blackgc;
GC whitegc;

#ifdef useXlib
static int intersect(int x1, int w1, int x2, int w2) {
    if (x1 < x2)
	return x1+w1 > x2;
    else
	return x2+w2 > x1;
}
#endif

/* events for Xlib AND Xaw interface */
/* event entry points are: key_press, button_press, button_release,
   mouse_motion (only called with button 3 pressed) redraw_table */


void key_press(XKeyPressedEvent *xev) {
    char str[32];
    int num;

#define	get_name_field()	get_selection()

    num = XLookupString(xev, str, 31, NULL, NULL);
    if (num == 0)
	return;
    str[num] = '\0';		/* NULL to terminate it */

    show_exposed_card(False);
    key_pressed(str);
}

/* SPEEDUP does not work yet */
#ifdef SPEEDUP
static int pile_is_drawn[MAXPILES] = { 0, 0 };
static int buttons_are_drawn = 0;
#endif

extern int table_clear;

void redraw_table(XExposeEvent *xev) {
    int i;
    table_clear = 0;
#ifdef useXlib
    if (intersect(0, graphic.buttons_height, xev->y, xev->height))
#ifdef SPEEDUP
	if (!buttons_are_drawn++)
#endif
	    redraw_buttons(xev->x, xev->y, xev->width, xev->height, table, button.num, button.b);
    if (intersect(graphic.buttons_height, graphic.message_height, xev->y, xev->height))
	show_message(NULL);
    if (xev->count && xev->y + xev->height < graphic.yoff)	/* else may miss the last event */
	return;
#endif

#ifdef LABER
    printf("request for expose %d,%d of size %d,%d, cnt %d\n", xev->x, xev->y, xev->width, xev->height, xev->count);
#endif

    for (i = 0; i < game.numpiles; ++i) {
	struct pile *p;
#ifdef SPEEDUP
	if (pile_is_drawn[i]++)
	    continue;
#endif
	p = graphic.pile + i;
	if (NOT_DISPLAYED(p) ||
	    xev->y >= p->y + p->ytotal || xev->y + xev->height <= p->y ||
	    xev->x >= p->x + p->xtotal || xev->x + xev->width <= p->x)
	    continue;	/* this pile is not affected */
#ifdef LABER
	printf("request for update pile %d of pos %d,%d, size %d,%d\n", i,
	       p->x, p->y, p->xtotal, p->ytotal);
#endif
	draw_pileupdate(i, 0);
    }
    if (!xev->count) {
	show_arrow(2);				/* Update hint arrow */
    }
#ifdef LABER
    if (!xev->count)
	printf("         => last expose-table call\n");
#endif
#ifdef SPEEDUP
    if (xev->count)
	return;
    /* was last event, unmark piles */
    for (i = 0; i < MAXPILES; ++i)
	pile_is_drawn[i] = 0;
    buttons_are_drawn = 0;
#endif
}


/* the pointer is somewhere in the area of pile i */
/* check, if a card has to be exposed */

static void expose_card(Pileindex i, int y) {
    struct pile *p;
    Cardindex ind;

    if (EMPTY(i))
	return;
    p = graphic.pile + i;
    ind = game.ind[i];
    while (ind != INDEX_OF_LAST_CARD(i) && y >=	p->y + graphic.cardy[ind+1])
        ++ind;
    if (ind == INDEX_OF_LAST_CARD(i))   /* the most bottom card don't has to */
        ind = -1;                       /* be put on foreground */
    if (graphic.zoomed_card != ind) {   /* change of state */
	show_exposed_card(False);	/* hide it */
        /* graphic.zoomed_card is now -1 */
        if (ind >= 0) {
  	    graphic.zoomed_card = ind;
	    show_exposed_card(True);
        }
    }
}

void mouse_motion(XPointerMovedEvent *xev) {
    Pileindex i;
    /* find new pile */
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	struct pile *p;
	p = graphic.pile+i;
	if (xev->x >= p->x && xev->x < p->x + p->xmaxwidth &&
	    xev->y >= p->y && xev->y < p->y + p->ymaxheight) {
	    /* yeah, a slot is hit */
	    /* find out which card is the target */
	    expose_card(i, xev->y);
	    return;
        }
    }
    /* pointer moved out of scope: */
    show_exposed_card(False);
}

void button_release(XButtonPressedEvent *xev) {
    show_exposed_card(False);
}

void button_press(XButtonPressedEvent *xev) {
    Pileindex i;

    show_exposed_card(False);
#ifdef useXlib
    {	void (*func)(void);
	func = check_button_list(button.b, button.num, xev); 

	/* check for hit button */
	if (func) {
	    (*func)();
	    return;
	}
    }
#endif

    for (i = 0; i < game.numpiles; ++i) {
	struct pile *p;
	p = graphic.pile+i;
	if (NOT_DISPLAYED(p))
	    continue;
	if (xev->x >= p->x && xev->x < p->x + p->xmaxwidth &&
	    xev->y >= p->y && xev->y < p->y + p->ymaxheight) {
	    Cardindex cardi;
	    if EMPTY(i)
		cardi = -1;
	    else {
		cardi = INDEX_OF_LAST_CARD(i);
		if (game.piletype[i] == Slot)
		    /* possibly a different card */
		    while (cardi != INDEX_OF_FIRST_CARD(i) &&
			   graphic.cardy[cardi] > xev->y - graphic.pile[i].y)
			--cardi;
	    }
	    switch (xev->button) {
	    case Button1:            /* quick move */
		button_pressed(i, cardi, 1);
		break;
	    case Button2:            /* select / deselect */
		button_pressed(i, cardi, 2);
		break;
	    case Button3:
#ifdef BUTTON_3_DRAGS_CARD
		button_pressed(i, cardi, 3);
#else
#ifndef useXview
		if (game.piletype[i] == Slot)
		    expose_card(i, xev->y);
#else
		if (game.piletype[i] == Slot &&
		    xev->y < graphic.pile[i].y + graphic.cardy[cardi] +
			CARD_HEIGHT)
		    expose_card(i, xev->y);
		else
		{
		    menu_show(cmenu, canvas, bp_event, 0);
		    return;
		}
#endif
		break;
#endif
	    }
	    return;
	}
    }
#ifdef useXview
    if (xev->button == Button3)
    {
	menu_show(cmenu, canvas, bp_event, 0);
	return;
    }
#endif
    button_pressed(-1, -1, -1);		/* cancel action */
}

/*****************************************************************************/
/*									     */
/*	Functions for resize events and resize requests			     */
/*									     */
/*****************************************************************************/

/* 1) hard resizes (i.e. forcing the outer window to change size) */
/*    I think these are not liked in the Xaw community */

void cmd_MinWindow(void) {
    XSize_t w, h;
    w = graphic.min_width;
    h = graphic.min_height;
    Force_Resize(w, h);
}

void cmd_PreferredWindow(void) {
    XSize_t w, h;
    w = graphic.preferred_width;
    h = graphic.preferred_height;
    Force_Resize(w, h);
}

/* event handler function. This function is called by the Widget in response
   to a request from us. In Xaw, this is a resize of the logical area, i.e.
   of the virtual size of the tableau. */

void resize_event(XSize_t w, XSize_t h) {
#ifdef LABER
    printf("resize event to (%d,%d) called\n", w, h);
#endif
    if (game.graphic)
	cmd_ResetHints();
    if (graphic.height == h && graphic.width == w)
	return;		/* no change of size */

    /* in xlib, we must clear the new area by hand; there may be illegal data
       left in the server. This applies to Xaw as well */
    {   XExposeEvent xev;
	xev.count = -1;
        if (game.graphic) {
	    if (graphic.height < h) {
		/* window is greater now */
		XClearArea(dpy, table, 0, graphic.height, graphic.width, h - graphic.height, True);
		++xev.count;
	    }
	    if (graphic.width < w) {
		/* window is greater now */
		XClearArea(dpy, table, graphic.width, 0, w - graphic.width, h, True);
		++xev.count;
	    }
	    if (xev.count >= 0) {
		/* generate synthetic expose events for the new area */
		/* this must be done before we possibly change the layout */
		if (graphic.height < h) {
		    /* window is greater now */
		    xev.x = 0;
		    xev.y = graphic.height;
		    xev.width = graphic.width;
		    xev.height = h - graphic.height;
		    redraw_table(&xev);
		    --xev.count;
		}
		if (graphic.width < w) {
		    /* window is greater now */
		    xev.x = graphic.width;
		    xev.y = 0;
		    xev.width = w - graphic.width;
		    xev.height = h;
		    redraw_table(&xev);
		}
	    }
	}
    }
    graphic.height = h;
    graphic.width = w;

    if (!game.graphic)
	return;
#ifdef useXlib
    confirm.x = (graphic.width - confirm.w) / 2;
    confirm.y = (graphic.height - confirm.h) / 2;
    XMoveWindow(dpy, confirm.win, confirm.x, confirm.y);
#endif

    if (graphic.autolayout) {
	cmd_Layout();	/* change everything */
    } else {
	Pileindex i;
	/* fix piles    THIS IS WRONG FOR 2-ROW LAYOUTS!!!! FIX THIS! */
	for (i = 0; i < rules.numslots; ++i)
	    graphic.pile[SLOT(i)].ymaxheight = graphic.height - graphic.pile[SLOT(i)].y;
	
	for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	    if (pile_resize(i))
		draw_pileupdate(i, 0);
    }
    
}
