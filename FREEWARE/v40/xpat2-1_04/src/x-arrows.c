/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-arrows.c			     */
/*									     */
/*	Displays hint arrows for the X interface			     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

GC hintgc;
static int rem_sx = -1, rem_sy, rem_dx, rem_dy;
static int size_x, size_y;
static Pixmap arrow_storage_x = 0, arrow_storage_y, arrow_storage_z;

#ifdef useXlib
void init_arrow(const char *arrowcolor, int arrwidth, int arrheight)
{
    /* make GCs for dashed lines or colored lines (to mark cards) */
    XGCValues gcv;
    long gcflags;
    XColor color;
    Colormap cmap;
    
    if (arrwidth >= 0 && arrwidth <= 30)
	graphic.aw = arrwidth;
    if (arrheight >= 0 && arrheight <= 30)
	graphic.ah = arrheight;

    if (graphic.aw <= 0 || graphic.ah <= 0) {
	graphic.aw = graphic.ah = -1;
	return;			/* no arrows wanted */
    }

    /* make own gcs for dashed lines */
    gcv.background = BlackPixel(dpy, screen);
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    if (graphic.is_color && arrowcolor) {
	cmap = DefaultColormap(dpy, screen);
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocNamedColor(dpy, cmap, arrowcolor, &color, &color);
	gcv.foreground = color.pixel;
	hintgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    } else {
	gcv.foreground = WhitePixel(dpy, screen);
	hintgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    }
}
#endif

#ifdef useXview
void init_arrow(unsigned long arrowcolor, int arrwidth, int arrheight)
{
    XGCValues gcv;
    long gcflags;
    
    if (arrwidth >= 0 && arrwidth <= 30)
	graphic.aw = arrwidth;
    if (arrheight >= 0 && arrheight <= 30)
	graphic.ah = arrheight;

    if (graphic.aw <= 0 || graphic.ah <= 0) {
	graphic.aw = graphic.ah = -1;
	return;			/* no arrows wanted */
    }

    gcv.background = BlackPixel(dpy, screen);
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    gcv.foreground = arrowcolor;
    hintgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}
#endif

/* draw an arrow with y-part */
static void polygon_y(int deltax, int deltay, int adx, int ady)
{
    XPoint Poly[10];
    Poly[0].x = rem_dx-deltax;
    Poly[0].y = rem_dy-ady/2;
    Poly[1].x = rem_dx-adx;
    Poly[1].y = rem_dy-ady;
    Poly[2].x = rem_dx;
    Poly[2].y = rem_dy+ady;
    Poly[3].x = rem_dx+adx;
    Poly[3].y = rem_dy-ady;
    Poly[4].x = rem_dx+deltax;
    Poly[4].y = rem_dy-ady/2;
    if (rem_dx-rem_sx > 0) {	/* this is uncritical, since difference is */
        Poly[5].x = rem_dx+deltax;	/* either 0 or at least CARD_WIDTH */
        Poly[5].y = rem_sy+deltay;
        Poly[6].x = rem_sx-deltax;
        Poly[6].y = rem_sy+deltay;
        Poly[7].x = rem_sx-deltax;
        Poly[7].y = rem_sy-deltay;
        Poly[8].x = rem_dx-deltax;
        Poly[8].y = rem_sy-deltay;
        XFillPolygon(dpy, table, hintgc, Poly, 9, Complex, CoordModeOrigin);
	Poly[9] = Poly[0];
	XDrawLines(dpy, table, blackgc, Poly, 10, CoordModeOrigin);
    } else if (rem_dx < rem_sx) {
        Poly[5].x = rem_dx+deltax;
        Poly[5].y = rem_sy-deltay;
        Poly[6].x = rem_sx+deltax;
        Poly[6].y = rem_sy-deltay;
        Poly[7].x = rem_sx+deltax;
        Poly[7].y = rem_sy+deltay;
        Poly[8].x = rem_dx-deltax;
        Poly[8].y = rem_sy+deltay;
        XFillPolygon(dpy, table, hintgc, Poly, 9, Complex, CoordModeOrigin);
	Poly[9] = Poly[0];
	XDrawLines(dpy, table, blackgc, Poly, 10, CoordModeOrigin);
    } else {
        Poly[5].x = rem_sx+deltax;
        Poly[5].y = rem_sy+deltay;
        Poly[6].x = rem_sx-deltax;
        Poly[6].y = rem_sy+deltay;
        XFillPolygon(dpy, table, hintgc, Poly, 7, Complex, CoordModeOrigin);
	Poly[7] = Poly[0];
	XDrawLines(dpy, table, blackgc, Poly, 8, CoordModeOrigin);
    }
}

/* draw an x-only arrow */
static void polygon_x(int deltax, int deltay, int adx, int ady)
{
    XPoint Poly[10];
    /* Pfeil: 1      */
    /*          0    */
    /*             2 */
    /*          4    */
    /*        3      */
    Poly[0].x = rem_dx-adx/2;
    Poly[0].y = rem_sy-deltay;
    Poly[1].x = rem_dx-adx;
    Poly[1].y = rem_sy-ady;
    Poly[2].x = rem_dx+adx;
    Poly[2].y = rem_sy;
    Poly[3].x = rem_dx-adx;
    Poly[3].y = rem_sy+ady;
    Poly[4].x = rem_dx-adx/2;
    Poly[4].y = rem_sy+deltay;
    Poly[5].x = rem_sx-deltax;
    Poly[5].y = rem_sy+deltay;
    Poly[6].x = rem_sx-deltax;
    Poly[6].y = rem_sy-deltay;
    XFillPolygon(dpy, table, hintgc, Poly, 7, Complex, CoordModeOrigin);
    Poly[7] = Poly[0];
    XDrawLines(dpy, table, blackgc, Poly, 8, CoordModeOrigin);
}

#define UPDOWN_MINDIST	(graphic.ya_h + graphic.ah + 1)

static void display_arrow(void)
{
    /* build a polygon i.e. a arrow pointing from source to destination */ 

    if (rem_dy >= rem_sy + UPDOWN_MINDIST)    	/* arrow up */
        polygon_y(graphic.aw,-graphic.ah,graphic.ya_w, graphic.ya_h);
    else if (rem_dy <= rem_sy - UPDOWN_MINDIST)	/* arrow down */
        polygon_y(graphic.aw,graphic.ah,graphic.ya_w,-graphic.ya_h);
    else if (rem_dx >= rem_sx)			/* arrow to the right */
        polygon_x(graphic.aw,graphic.ah,graphic.xa_w,graphic.xa_h);
    else					/* arrow to the left */
        polygon_x(-graphic.aw,graphic.ah,-graphic.xa_w,graphic.xa_h);
}

static void clear_arrow(void)
{   int a, d;
    a = -1;
    if (rem_sx < rem_dx) {
	a = rem_sx; d = rem_dx - rem_sx;
    } else if (rem_sx > rem_dx) {
	a = rem_dx; d = rem_sx - rem_dx;
    } else
	d = a = -1;
    if (a >= 0)
	XCopyArea(dpy, arrow_storage_x, table, whitegc,
		  0, 0, d + 2 * graphic.aw + 1, 2 * graphic.ah + 1,
		  a - graphic.aw, rem_sy - graphic.ah);
    
    if (rem_sy <= rem_dy - UPDOWN_MINDIST) {
	a = rem_sy; d = rem_dy - rem_sy;
    } else if (rem_sy >= rem_dy + UPDOWN_MINDIST) {
	a = rem_dy; d = rem_sy - rem_dy;
    } else {
	d = a = -1;	/* only horizontal part of arrow => y arrow */
	XCopyArea(dpy, arrow_storage_z, table, whitegc,
	  0, 0, 2 * graphic.xa_w + 1, 2 * graphic.xa_h + 1, rem_dx-graphic.xa_w, rem_sy-graphic.xa_h);
    }
    if (a >= 0) {
	XCopyArea(dpy, arrow_storage_y, table, whitegc,
		  0, 0, 2 * graphic.aw + 1, d + 2 * graphic.ah + 1,
		  rem_dx - graphic.aw, a - graphic.ah);
	XCopyArea(dpy, arrow_storage_z, table, whitegc,
	  0, 0, 2 * graphic.ya_w + 1, 2 * graphic.ya_h + 1, rem_dx-graphic.ya_w, rem_dy-graphic.ya_h);
    }
    rem_sx = -1;
}

static void save_background(void)
{
    /* save the background */
    int a, d;
    a = -1;
    if (rem_sx < rem_dx) {
	a = rem_sx; d = rem_dx - rem_sx;
    } else if (rem_sx > rem_dx) {
	a = rem_dx; d = rem_sx - rem_dx;
    } else
	d = a = -1;
    if (a >= 0) {
	int len;
	len = d + 2 * graphic.aw + 1;
	if (len > size_x) {	/* Create a larger Pixmap for saving */
	    XFreePixmap(dpy, arrow_storage_x);
	    arrow_storage_x = XCreatePixmap(dpy, table,
		  size_x = len, 2 * graphic.ah+1, DefaultDepth(dpy, screen));
	}
	XCopyArea(dpy, table, arrow_storage_x, whitegc,
		  a - graphic.aw, rem_sy - graphic.ah,
		  len, 2 * graphic.ah + 1, 0, 0);
    }
    if (rem_sy <= rem_dy - UPDOWN_MINDIST) {
	a = rem_sy; d = rem_dy - rem_sy;
    } else if (rem_sy >= rem_dy + UPDOWN_MINDIST) {
	a = rem_dy; d = rem_sy - rem_dy;
    } else {
	d = a = -1;	/* only horizontal part of arrow */
	XCopyArea(dpy, table, arrow_storage_z, whitegc,
	  rem_dx-graphic.xa_w, rem_sy-graphic.xa_h,  2 * graphic.xa_w + 1, 2 * graphic.xa_h + 1, 0, 0);
    }
    if (a >= 0) {
	int height;
	height = d + 2 * graphic.ah + 1;
	if (size_y < height) {
	    XFreePixmap(dpy, arrow_storage_y);
	    arrow_storage_y = XCreatePixmap(dpy, table,
		2 * graphic.aw+1, size_y = height, DefaultDepth(dpy, screen));
	}
	XCopyArea(dpy, table, arrow_storage_y, whitegc,
		  rem_dx - graphic.aw, a - graphic.ah,
		  2 * graphic.aw + 1, height, 0, 0);
	XCopyArea(dpy, table, arrow_storage_z, whitegc,
	  rem_dx-graphic.ya_w, rem_dy-graphic.ya_h,  2 * graphic.ya_w + 1, 2 * graphic.ya_h + 1, 0, 0);
    }

}

static void compute_rem(void) {
    Pileindex srcpile;
    struct pile *p;

    p = graphic.pile + game.arrow_dstpile;
    srcpile = getpile(game.arrow_srcind);
    rem_sx = graphic.pile[srcpile].x + CARD_WIDTH/2;
    rem_sy = graphic.pile[srcpile].y + CARD_HEIGHT/2;
    if (game.piletype[srcpile] == Slot) {
	int y = graphic.cardy[game.arrow_srcind];
	rem_sy += y;
	if (game.arrow_srcind != INDEX_OF_LAST_CARD(srcpile))
	    rem_sy += (graphic.cardy[game.arrow_srcind+1] - y + 1) / 2
	            - CARD_HEIGHT/2;
    }
    rem_dx = p->x + CARD_WIDTH/2;
    rem_dy = p->y + CARD_HEIGHT/2;
    if (game.piletype[game.arrow_dstpile] == Slot && !EMPTY(game.arrow_dstpile))
	rem_dy += graphic.cardy[INDEX_OF_LAST_CARD(game.arrow_dstpile)];
    if (rem_sy > graphic.height-2)
	rem_sy = graphic.height-2;
    if (rem_dy > graphic.height-2)
	rem_dy = graphic.height-2;
}

void show_arrow(int on) {
    if (graphic.aw <= 0)	/* no arrows active */
	return;

    if (!arrow_storage_x) {
	arrow_storage_x = XCreatePixmap(dpy, table, size_x = graphic.width,
            2 * graphic.ah+1, DefaultDepth(dpy, screen));
	arrow_storage_y = XCreatePixmap(dpy, table, 2 * graphic.aw+1,
	    size_y = graphic.height, DefaultDepth(dpy, screen));
	arrow_storage_z = XCreatePixmap(dpy, table, 2 * max(graphic.xa_w,
	    graphic.ya_w) + 1, 2 * max(graphic.xa_h, graphic.ya_h) + 1, 
	    DefaultDepth(dpy, screen));
    }
    if (on) {		/* calculate new arrow position */
#ifdef DEBUG
	if (on == 2)
	    fprintf(stderr, "arrow(2): srcind = %d\n", game.arrow_srcind);
#endif
	if (game.arrow_srcind < 0)
	    return;
	if (on == 1) {
	    compute_rem();
	    save_background();
	}
	display_arrow();
#ifdef useXview
	xv_set(xv_default_server, SERVER_SYNC, FALSE, 0);
#else
	XFlush(dpy);
#endif
    } else {
	game.arrow_srcind = -1;	/* no valid arrow now */
	if (rem_sx < 0)
	    return;		/* no arrow was visible */
	clear_arrow();
    }
}
