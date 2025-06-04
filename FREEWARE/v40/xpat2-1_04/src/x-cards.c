/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-cards.c			     */
/*									     */
/*	Card drawing interface for X11					     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

struct graphic graphic;
struct card card;
int table_clear = 1;

void win_game(void) {
#if 0
#ifdef useXlib
    XMoveWindow(dpy, finished_win, (graphic.width - FINISHED_W) / 2,
		(graphic.height - FINISHED_H) / 2);
    XMapWindow(dpy, finished_win);
    redraw_finwin((XExposeEvent *)0);	/* must unmap it later! */
#endif
#endif
#ifdef useXview
    xv_set(xv_default_server, SERVER_SYNC, FALSE, 0);
#else
    XSync(dpy, 0);	/* show all requests and let it be for now */
#endif
}


void init_gfx(void) {
    XGCValues gcv;
    long gcflags;

    blackpixel = BlackPixel(dpy, screen);
    whitepixel = WhitePixel(dpy, screen);
    
    
    /* make gc for white */
    gcv.foreground = WhitePixel(dpy, screen);
    gcv.background = BlackPixel(dpy, screen);
    gcv.graphics_exposures = False;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    
    whitegc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    
    /* make gc for black */
    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    
    blackgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}


/* this is a helper routine only called by do_move(): */
/* it transfers card game.which is unknown to the generic part */
/* these transfers do not necessarily affect the appearance on the screen, */
/* they are needed due to the design of the game.structures as arrays */
/* instead of linked lists */


void move_card_data(Cardindex dst, Cardindex src) {
    game.cards[dst] = game.cards[src];
    game.visible[dst] = game.visible[src];
    graphic.cardy[dst] = graphic.cardy[src];
}


void refresh_screen(void) {
#ifdef LABER
    printf("refresh_screen called\n");
#endif
    if (table_clear) {
#ifdef LABER
	printf("(ignored)\n");
#endif
	return;
    }
    table_clear = 1;
    XClearArea(dpy, table, 0, 0, 0, 0, True);	/* force redraw */
}

/* all card drawings go through this routine:			     */
/* inform the graphics interface that cards are added (delta > 0)    */
/* or removed (delta < 0) from the specified pile. if delta = 0,     */
/* then redrawing of the complete pile is necessary (possibly caused */
/* by "new game" commands)					     */
/* if cards on the pile are marked (selected), the mark is refreshed */

void draw_pileupdate(int pile, int delta) {
    struct pile *p = graphic.pile + pile;

    if (!game.graphic) {        /* this is off when replaying saved games */
	/* only log changes */
	if (game.pile_changed[pile] == PILE_UNCHANGED)
	    game.pile_changed[pile] = delta;
	else
	    if ((delta > 0 && game.pile_changed[pile] > 0) ||
		(delta < 0 && game.pile_changed[pile] < 0))
		game.pile_changed[pile] += delta;
	    else
		game.pile_changed[pile] = 0;
        return;
    }
    if (game.disable[pile])	/* don't draw THIS pile */
	return;
    if (NOT_DISPLAYED(p))
	return;			/* draw nothing */
    if (EMPTY(pile)) {		/* draw empty pile */
	int c;
	XClearArea(dpy, table, p->x, p->y, p->xmaxwidth, p->ymaxheight, False);
	switch (game.piletype[pile]) {
	case Stack:
	    c = SUITSYMBOL + SUIT(pile);
	    break;
	default:
	    c = OUTLINE;
	    break;
	}
	PaintCard(p->x, p->y, c, 0);
	return;
    }
    if (!p->delta && !p->xdelta) {	/* only topmost card is visible */
	Cardindex c;
	c = INDEX_OF_LAST_CARD(pile);
	PaintCard(p->x, p->y, game.visible[c] ? game.cards[c] : CARDBACK, 0);
    } else {

	if (pile_resize(pile))	/* delta changed => complete redraw */
	    delta = 0;
	
	if (delta < 0) {	/* cards were removed. Clear area and redraw top */
	    int x, y;
	    if (p->xdelta) {
		x = (CARDS_ON_PILE(pile)-1) * p->xdelta + CARD_WIDTH - ROUND_W;
		y = (CARDS_ON_PILE(pile)-1) * p->delta;
		XClearArea(dpy, table, p->x+x, p->y+y, p->xmaxwidth-x, p->ymaxheight-y, False);
	    }
	    if (p->delta) {
		x = (CARDS_ON_PILE(pile)-1) * p->xdelta;
		y = (CARDS_ON_PILE(pile)-1) * p->delta + CARD_HEIGHT - ROUND_H;
		XClearArea(dpy, table, p->x+x, p->y+y, p->xmaxwidth-x, p->ymaxheight-y, False);
	    }
	    delta = 1;	/* only last card is to be drawn new */
	} else if (!delta) {	/* draw complete pile */
	    XClearArea(dpy, table, p->x, p->y, p->xmaxwidth, p->ymaxheight, False);
	    delta = CARDS_ON_PILE(pile);
	}
	/* (delta > 0) */
	/* draw last cards */
	{   Cardindex c = INDEX_OF_LAST_CARD(pile) + 1 - delta;
	    int i = c - INDEX_OF_FIRST_CARD(pile); 
	    if (!p->xdelta) {
		/* may use card clipping optimisation */
		while (delta > 1) {
		    /* not the last card */
		    --delta;
		    PaintCard(p->x + p->xdelta*i, p->y + p->delta*i,
			      (game.visible[c] ? game.cards[c] : CARDBACK), p->delta);
		    ++c;
		    ++i;
		}
	    }
	    while (--delta >= 0) {
		PaintCard(p->x + p->xdelta*i, p->y + p->delta*i,
			  (game.visible[c] ? game.cards[c] : CARDBACK), 0);
		++c;
		++i;
	    }
	}
    }
    if (game.srcind >= 0 && getpile(game.srcind) == pile)
	show_mark(True);
}


