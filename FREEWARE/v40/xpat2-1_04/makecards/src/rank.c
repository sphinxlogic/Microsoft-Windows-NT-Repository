#include "config.h"

/* do not change anything below this, unless you know what you're doing */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xpm.h>

#include "largeclub.bm"
#include "largespade.bm"
#include "largeheart.bm"
#include "largediamond.bm"
#include "largerank.bm"

#define RANK_WIDTH 9
#define RANK_HEIGHT 14

static unsigned char grey[90][90];
static int size[] = { 0, 41, 21, 15, 11 };

static void bitmap_to_grey(char *bm, int w, int h, int scale, int scanline) {
    int i, j;
    char *b, *bb;
    unsigned char *g, *gg;
    int scancol, scancol0;

    memset(grey, 0, sizeof(grey));
    b = bm;
    if (scanline == -1) {
	scanline = 0;
	scancol0 = 0;
	g = grey[0];
    } else {
	i = (scale * size[scale/2] - w) / 2;
	scancol0 = i % scale;
	g = grey[(82-h)/scale] + i / scale;
    }

    /* make a picture of bitmap, offsetting it (scanline, scancol0) */
    --scanline;
    for (i = 0; i < h; ++i) {
	int c;
	/* start a new scanline */
	if (++scanline >= scale) {
	    scanline -= scale;
	    g += 90;
	}
	gg = g;		/* restart at this line */
	bb = b;
	scancol = scancol0 - 1;
	/* first, skip xoffset pixel */
	for (j = 0; j < w; ++j) {
	    if (++scancol >= scale) {
		++gg;
		scancol -= scale;
	    }
	    if (!(j % 8))
		c = *bb++;
	    /* c has bit in pos 0 */
	    if (c & 1)
		++*gg;
	    c >>= 1;
	}
	b += (w + 7) >> 3;
    }
}

struct longcolor {
    long red, green, blue;
};

static Display *dpy;
static unsigned int screen;
static Drawable w;
static GC gc;
static unsigned long black_ramp[NUM_BLACK_COLORS+1],
    red_ramp[NUM_RED_COLORS+1];

static void make_ramp(int numcolors, unsigned long *xcolors,
	      struct longcolor bg, struct longcolor fg) {
    XColor color;
    Colormap cmap;
    int i;

    cmap = XDefaultColormap(dpy, screen);
    color.flags = DoRed|DoGreen|DoBlue;

    for (i = 0; i <= numcolors; ++i) {
	color.red   = ((numcolors-i) * bg.red   + i * fg.red)   / numcolors;
	color.green = ((numcolors-i) * bg.green + i * fg.green) / numcolors;
	color.blue  = ((numcolors-i) * bg.blue  + i * fg.blue)  / numcolors;
	/* printf("r/g/b = %04x/%04x/%04x yields ", color.red,
	     color.green, color.blue); */
	if (!XAllocColor(dpy, cmap, &color)) {
	    fprintf(stderr, "Error! Cannot allocate color cell!\n");
	    exit(1);
	}
	/* printf("%04x/%04x/%04x\n", color.red, color.green, color.blue); */
	xcolors[i] = color.pixel;
    }
}

static void connect(int view, int width, int height, char *name) {
    Window root;
    Pixmap p;

    if(!(dpy = XOpenDisplay(NULL))) {
	fprintf( stderr, "cannot open display\n");
	exit(EXIT_FAILURE);
    }
    screen = XDefaultScreen(dpy);
    gc = XDefaultGC(dpy, screen);
    root = RootWindow(dpy, screen);

    if (!view) {
	p = XCreatePixmap(dpy, root, width, height, DefaultDepth(dpy, screen));
	w = p;
    } else {
	Window win;
	XGCValues gcv;
	unsigned long w_mask, gc_mask;
	XSizeHints xsh;
	XWMHints xwmh = {
	    (InputHint|StateHint),
	    True,
	    NormalState,
	    0,0,0,0,0,0 };
	
	w_mask = CWColormap|CWBackPixel;
	xsh.flags = PPosition | PSize;
	xsh.width = width;
	xsh.height = height;
	xsh.x = 100;
	xsh.y = 200;
	
	win = XCreateSimpleWindow(dpy, root, xsh.x,xsh.y,xsh.width,
				  xsh.height, 0,
				  WhitePixel(dpy, screen),
				  BlackPixel(dpy, screen));
	XSetStandardProperties(dpy, win, name, name, None, NULL, 0, &xsh);
	XSetWMHints(dpy, win, &xwmh);
	XSelectInput(dpy, win, ExposureMask|ButtonPressMask);
	XMapRaised(dpy, win);
	w = win;
	gcv.function = GXcopy;
	gcv.plane_mask = AllPlanes;
	gcv.line_width = 0;
	gcv.line_style = LineSolid;
	gcv.join_style = JoinMiter;
	gcv.fill_style = FillSolid;
	gc_mask = GCFunction|GCPlaneMask|GCLineWidth|GCLineStyle|GCJoinStyle|GCFillStyle;
	XChangeGC(dpy, gc, gc_mask, &gcv);
    }
}

static void wait_for_event(void) {
    XEvent event;
    do {
	XNextEvent(dpy, &event);
	if (event.type == ButtonPress)
	    exit(0);
    } while (event.type != Expose || event.xexpose.count != 0);

    /* skip coming events */
    while(XCheckTypedEvent(dpy, Expose, &event))
	;
}




static void paintsuit(char *bits, int width, int height, int y,
		      int scale, int suit) {
    int i, j, this;
    this = size[scale];
    bitmap_to_grey(bits, width, height, 2*scale, scale*this - 41);
    for (i = 0; i < this; ++i)
	for (j = 0; j < this; ++j) {
	    if (suit < 2)
		XSetForeground(dpy, gc, black_ramp[(((int)grey[j][i]
		   * NUM_BLACK_COLORS / scale / scale) + 2) / 4]);
	    else
		XSetForeground(dpy, gc, red_ramp[(((int)grey[j][i]
		   * NUM_RED_COLORS / scale / scale) + 2) / 4]);
	    /* single pixels: (O'Reilly Xlib PM 3rd ed., p. 738) */
	    XFillRectangle(dpy, w, gc, suit*this+i,       y+j,        1, 1);
	    if (scale > 1)
	    XFillRectangle(dpy, w, gc, (suit+5)*this-i-1, y+this-1-j, 1, 1);
	}
}

static void paint(int width, int height) {
    int scale, y;

    XSetForeground(dpy, gc, black_ramp[0]);
    XFillRectangle(dpy, w, gc, 0, 0, width, height);

    y = 0;
    for (scale = 1; scale <= 4; ++scale) {
	paintsuit(largeclub_bits, largeclub_width, largeclub_height,
		  y, scale, 0);
	paintsuit(largespade_bits, largespade_width, largespade_height,
		  y, scale, 1);
	paintsuit(largeheart_bits, largeheart_width, largeheart_height,
		  y, scale, 2);
	paintsuit(largediamond_bits, largediamond_width, largediamond_height,
		  y, scale, 3);
	y += size[scale];
    }
}

static void paintrank(int width, int height) {
    int i, j, scale;
    XSetForeground(dpy, gc, black_ramp[0]);
    XFillRectangle(dpy, w, gc, 0, 0, width, height);
    scale = largerank_width / 9 / 3;
    if ((scale & 1) ||
	scale * 9 * 3 != largerank_width ||
	scale * 14 * 5 != largerank_height)
	fprintf(stderr, "Please check your scale. I'm getting weird results\n");
    scale /= 2;
    bitmap_to_grey(largerank_bits, largerank_width, largerank_height, 2*scale, -1);
    for (i = 0; i < 3*RANK_WIDTH; ++i)
	for (j = 0; j < 5*RANK_HEIGHT; ++j) {
	    XSetForeground(dpy, gc, black_ramp[(((int)grey[j][i] * NUM_BLACK_COLORS
		/ scale / scale) + 2) / 4]);
	    XFillRectangle(dpy, w, gc, i,                j,                 1, 1);
	    XFillRectangle(dpy, w, gc, 9*RANK_WIDTH-i-1, 5*RANK_HEIGHT-1-j, 1, 1);
	    XSetForeground(dpy, gc, red_ramp[(((int)grey[j][i] * NUM_RED_COLORS
	        / scale / scale) + 2) / 4]);
	    XFillRectangle(dpy, w, gc, 3*RANK_WIDTH+i,   j,                 1, 1);
	    XFillRectangle(dpy, w, gc,12*RANK_WIDTH-i-1, 5*RANK_HEIGHT-1-j, 1, 1);
	}
}

static void writefile(const char *filename) {
    static int is_written = 0;
    if (!is_written) {
	is_written = 1;
	if (XpmWriteFileFromPixmap(dpy, filename, w, 0, NULL) != XpmSuccess)
	    fprintf(stderr, "error writing xpm file %s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    char *p;
    static struct longcolor red = RED_COLOR, black = BLACK_COLOR,
       bg = WHITE_COLOR;
    int view = 0, do_rank = 0;
    int width = 168, height = 88;

    p = strrchr(argv[0], '/');
    p = p ? p+1 : argv[0];
    if (!strcmp(p, "rank")) {
	do_rank = 1;
	width = 12 * RANK_WIDTH;
	height = 5 * RANK_HEIGHT;
    }
    if (argc == 2 && !strcmp(argv[1], "-v"))
	view = 1;
    else if (argc != 1) {
	fprintf(stderr, "usage: %s [-v]\n", p);
	exit(1);
    }

    connect(view, width, height, p);
    make_ramp(NUM_BLACK_COLORS, black_ramp, bg, black);
    make_ramp(NUM_RED_COLORS, red_ramp, bg, red);
    do {
	if (view)
	    wait_for_event();
	if (do_rank) {
	    paintrank(width, height);
	    writefile("Ranks.xpm");
	} else {
	    paint(width, height);
	    writefile("Suits.xpm");
	}
    } while (view);
    return 0;
}
