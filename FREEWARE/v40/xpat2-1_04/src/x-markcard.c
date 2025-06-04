/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-markcard.c			     */
/*									     */
/*	Routines for marking selected cards				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

#ifndef NO_ROUND_CARDS
#ifndef VMS
#include	<X11/Xmu/Drawing.h>
#else
#include	<Xmu/Drawing.h>
#endif
#endif


#include "stipple.bm"

/* game.for set-mark / clear-mark */

static GC dashedlinegc;
static GC whitelinegc;
static GC stipplegc;
static int LINEWIDTH = 3;
static Pixmap mark_storage_x = 0;
static int mark_height = 0, mark_width = 0;
static Pixmap mark_storage_y;

static int rem_x, rem_y;
static XSize_t rem_w, rem_h;	   /* h = 0: only horizontal bars saved */

void init_mark(unsigned long pixel, int markwidth) {
    /* make GCs for dashed lines or colored lines (to mark cards) */
    XGCValues gcv;
    long gcflags;
    Pixmap stipplemap;

    if (2 * markwidth > CARD_WIDTH)
	markwidth = (CARD_WIDTH+1) >> 1;
    LINEWIDTH = markwidth;
    /* make own gcs for dashed lines */
    gcv.background = WhitePixel(dpy, screen);
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    if (graphic.is_color && pixel != BlackPixel(dpy, screen)) {
	gcv.foreground = pixel;
	dashedlinegc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
	XSetLineAttributes(dpy, dashedlinegc, LINEWIDTH, LineSolid, CapButt, JoinMiter);
    } else {
	gcv.foreground = BlackPixel(dpy, screen);
	dashedlinegc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
	XSetLineAttributes(dpy, dashedlinegc, LINEWIDTH, LineOnOffDash, CapButt, JoinMiter);
    }
    gcv.foreground = WhitePixel(dpy, screen);
    gcv.background = BlackPixel(dpy, screen);
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    whitelinegc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);

    XSetLineAttributes(dpy, whitelinegc, LINEWIDTH, LineSolid, CapButt, JoinMiter);
    stipplemap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
				       stipple_bits, stipple_width, stipple_height);
    gcflags = GCForeground | /* GCBackground | */ GCGraphicsExposures |
	GCFillStyle | GCStipple;
    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcv.stipple = stipplemap;
    gcv.fill_style = FillStippled;
    stipplegc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}

static void set_mark(int x, int y, XSize_t W, XSize_t h) {
    if (h > mark_height) {	/* make sure there's enough space */
	XFreePixmap(dpy, mark_storage_y);
	mark_height = h;
        mark_storage_y = XCreatePixmap(dpy, table, 2 * LINEWIDTH, mark_height,
	    DefaultDepth(dpy, screen));
    }
    if (W > mark_width) {
	XFreePixmap(dpy, mark_storage_x);
	mark_width = W;
        mark_storage_x = XCreatePixmap(dpy, table, W, 2 * LINEWIDTH,
	    DefaultDepth(dpy, screen));
    }

    if (2 * LINEWIDTH >= W) {	/* rectangle operation */
	rem_h = h;
	rem_w = W;
	rem_x = x;
	rem_y = y;
	XCopyArea(dpy, table, mark_storage_y, whitegc, x, y,
	    rem_w, rem_h, 0, 0);
#ifndef NO_ROUND_CARDS
	if (ROUND_W)
	    XmuFillRoundedRectangle(dpy, table, stipplegc, x, y,
		rem_w, rem_h, ROUND_W, ROUND_H);
	else
#endif
	    XFillRectangle(dpy, table, stipplegc, x, y, rem_w, rem_h);
	XFlush(dpy);
	return;
    }

    rem_h = h - 2 * LINEWIDTH;
    if (h <= 0)		/* do not mark thin areas */
	return;
    rem_w = W;
    rem_x = x;
    rem_y = y;
    
    XCopyArea(dpy, table, mark_storage_x, whitegc, x, y,
	      rem_w, LINEWIDTH, 0, 0);
    XCopyArea(dpy, table, mark_storage_x, whitegc, x, y+h-LINEWIDTH,
	      rem_w, LINEWIDTH, 0, LINEWIDTH);
    if (rem_h > 0) {
	XCopyArea(dpy, table, mark_storage_y, whitegc, x, y+LINEWIDTH,
		  LINEWIDTH, rem_h, 0, 0);
	XCopyArea(dpy, table, mark_storage_y, whitegc, x+W-LINEWIDTH, y+LINEWIDTH,
		  LINEWIDTH, rem_h, LINEWIDTH, 0);
    }
    if (!ROUND_H) {		/* this variant does not look very pretty for round cards */
	XPoint r[5];
	r[0].x = r[3].x = r[4].x = x + LINEWIDTH/2;
	r[1].x = r[2].x = x + W - (LINEWIDTH+1)/2;
	r[0].y = r[1].y = r[4].y = y + LINEWIDTH/2;
	r[2].y = r[3].y = y + h - (LINEWIDTH+1)/2;
	/* XDrawLines(dpy, table, whitelinegc,  r, 5, CoordModeOrigin); */
	XDrawLines(dpy, table, dashedlinegc, r, 5, CoordModeOrigin);
    } else {
	int d, e, i, w, o;
	static GC *gc1tab[2] = { &dashedlinegc, &whitelinegc };
	
	h -= (LINEWIDTH+1)/2;	/* correct values for DrawLine */
	w = W - (LINEWIDTH+1)/2;
	d = ROUND_W;
	e = ROUND_H;
	o = LINEWIDTH / 2;
	for (i = 0; i >= 0; --i) {
	    XDrawLine(dpy, table, *gc1tab[i], x+o+d, y+o,   x+w-d, y+o); 
	    XDrawLine(dpy, table, *gc1tab[i], x+o+d, y+h,   x+w-d, y+h); 
	    XDrawLine(dpy, table, *gc1tab[i], x+o,   y+o+e, x+o,   y+h-e); 
	    XDrawLine(dpy, table, *gc1tab[i], x+w,   y+o+e, x+w,   y+h-e); 
	}
    }
    XFlush(dpy);
}

static void clear_mark(void) {
    if (2 * LINEWIDTH >= rem_w) {	/* rectangle operation */
	XCopyArea(dpy, mark_storage_y, table, whitegc, 0, 0,
	    rem_w, rem_h, rem_x, rem_y);
    } else {
	XCopyArea(dpy, mark_storage_x, table, whitegc, 0, 0,
	    rem_w, LINEWIDTH, rem_x, rem_y);
	XCopyArea(dpy, mark_storage_x, table, whitegc, 0, LINEWIDTH,
            rem_w, LINEWIDTH, rem_x, rem_y+rem_h+LINEWIDTH);
	if (rem_h > 0) {
	    XCopyArea(dpy, mark_storage_y, table, whitegc, 0, 0,
	        LINEWIDTH, rem_h, rem_x, rem_y+LINEWIDTH);
	    XCopyArea(dpy, mark_storage_y, table, whitegc, LINEWIDTH, 0,
	        LINEWIDTH, rem_h, rem_x+CARD_WIDTH-LINEWIDTH, rem_y+LINEWIDTH);
	}
    }
    XFlush(dpy);
}

/* pile is completely drawn and unmarked. */
/* check, if pile has to be marked.       */
/* if so, do it                           */

void show_mark(boolean on) {
    if (!game.graphic)
	return;
    if (!mark_storage_x) {
	mark_storage_x = XCreatePixmap(dpy, table, CARD_WIDTH, 2 * LINEWIDTH,
				   DefaultDepth(dpy, screen));
	mark_storage_y = XCreatePixmap(dpy, table, 2 * LINEWIDTH, graphic.height,
				   DefaultDepth(dpy, screen));
	mark_height = graphic.height;
	mark_width = CARD_WIDTH;
    }
    if (on) {
	struct pile *p;
	int f, l;
	int pile;
 	if (game.srcind < 0)
	    return;
	p = graphic.pile + (pile = getpile(game.srcind));
	f = game.srcind - INDEX_OF_FIRST_CARD(pile);
	l = CARDS_ON_PILE(pile)-1;
	set_mark(p->x + f*p->xdelta, p->y + f * p->delta,
		 (l-f)*p->xdelta + CARD_WIDTH, (l-f) * p->delta + CARD_HEIGHT);
    } else {
	clear_mark();
    }
}
