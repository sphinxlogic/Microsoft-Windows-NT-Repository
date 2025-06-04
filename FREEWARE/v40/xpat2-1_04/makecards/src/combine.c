#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#ifndef max
#define max(a, b)     ((a) > (b) ? (a) : (b))
#define min(a, b)     ((a) < (b) ? (a) : (b))
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Command.h>

#include <xpm.h>

static const char
    n_ace[] = "Ace", n_deuce[] = "Deuce", n_three[] = "Three",
    n_four[] = "Four", n_five[] = "Five", n_six[] = "Six", n_seven[] = "Seven",
    n_eight[] = "Eight", n_nine[] = "Nine", n_ten[] = "Ten", n_jack[] = "Jack",
    n_queen[] = "Queen", n_king[] = "King",
    n_diamonds[] = "Diamonds", n_hearts[] = "Hearts", n_spades[] = "Spades",
    n_clubs[] = "Clubs";

const char *US_rank_name[] = {
    n_ace, n_deuce, n_three, n_four, n_five, n_six, n_seven, n_eight, n_nine,
    n_ten, n_jack, n_queen, n_king };
const char *US_suit_name[] = {
    n_clubs, n_spades, n_hearts, n_diamonds };


Display	*dpy;
int	screen;
Window	table;

static int read_xpmfile(char *name, Pixmap *image, Pixmap *clip, int *w, int *h) {
    XpmAttributes attribs;
    attribs.valuemask = 0;
    if (XpmReadFileToPixmap(dpy, table, name, image, clip, &attribs) != XpmSuccess)
	return 0;
    if (w) *w = attribs.width;
    if (h) *h = attribs.height;
    return 1;
}

static GC gc, g1, clipgc;
static Pixmap pics[12], clip[12], big, clp;

static void make_GCs(void) {
    XGCValues gcv;
    long gcflags;
    
    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcv.graphics_exposures = False;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    gc = XCreateGC(dpy, big, gcflags, &gcv);
    g1 = XCreateGC(dpy, clp, gcflags, &gcv);
    gcv.foreground = 1;
    gcv.background = 0;
    clipgc = XCreateGC(dpy, clp, gcflags, &gcv);
}

static void action(Widget unused, XtPointer a, XtPointer b) {
    int s, r, w, h, i;
    for (r = 10; r < 13; ++r)
	for (s = 0; s < 4; ++s) {
	    char name[40];
	    i = 4 * (r-10) + s;
	    sprintf(name, "%s.%s.pic.xpm", US_rank_name[r], US_suit_name[s]);
	    if (!read_xpmfile(name, pics+i, clip+i, &w, &h)) {
		fprintf(stderr, "cannot read %s\n", name);
		exit(1);
	    }
	}
    big = XCreatePixmap(dpy, RootWindow(dpy, screen), 4 * w, 3 * h, DefaultDepth(dpy, screen));
    clp = XCreatePixmap(dpy, RootWindow(dpy, screen), 4 * w, 3 * h, 1);
    make_GCs();
    XFillRectangle(dpy, clp, clipgc, 0, 0, 4*w, 3*h);

    for (r = 10; r < 13; ++r)
	for (s = 0; s < 4; ++s) {
	    i = 4 * (r-10) + s;
	    XCopyArea(dpy, pics[i], big, gc, 0, 0, w, h, s * w, (r-10) * h);
	    if (clip[i])
		XCopyArea(dpy, clip[i], clp, g1, 0, 0, w, h, s * w, (r-10) * h);
	}
    if (XpmWriteFileFromPixmap(dpy, "Pictures.xpm", big, clp, NULL) != XpmSuccess) {
        fprintf(stderr, "error writing xpm file Pictures.xpm\n");
	exit(1);
    }
    exit(0);
}

static void action2(Widget unused, XtPointer a, XtPointer b) {
    int s, r, w, h, i;
    if (!read_xpmfile("Pictures.xpm", &big, &clp, &w, &h)) {
	fprintf(stderr, "cannot read Pictures.xpm\n");
	exit(1);
    }
    w /= 4;
    h /= 3;
    make_GCs();

    for (r = 10; r < 13; ++r)
	for (s = 0; s < 4; ++s) {
	    char name[40];
	    i = 4 * (r-10) + s;
	    sprintf(name, "%s.%s.pic.xpm", US_rank_name[r], US_suit_name[s]);
	    pics[i] = XCreatePixmap(dpy, RootWindow(dpy, screen), w, h, DefaultDepth(dpy, screen));
	    clip[i] = XCreatePixmap(dpy, RootWindow(dpy, screen), w, h, 1);
	    XCopyArea(dpy, big, pics[i], gc, s * w, (r-10) * h, w, h, 0, 0);
	    XCopyArea(dpy, clp, clip[i], g1, s * w, (r-10) * h, w, h, 0, 0);
	    if (XpmWriteFileFromPixmap(dpy, name, pics[i], clip[i], NULL) != XpmSuccess) {
		fprintf(stderr, "error writing file %s\n", name);
		exit(1);
	    }
	}
    exit(0);
}


int main(int argc, char *argv[]) {
    XtAppContext app_con;
    Widget toplevel, paned, text1, text2;

    toplevel = XtAppInitialize(&app_con, "Combine", NULL, 0, &argc, argv, NULL, NULL, 0);
    paned = XtCreateManagedWidget("paned", panedWidgetClass, toplevel, NULL, 0);
    text1 = XtCreateManagedWidget("Click to combine", commandWidgetClass, paned, NULL, 0);
    text2 = XtCreateManagedWidget("Click to separate", commandWidgetClass, paned, NULL, 0);
    XtAddCallback(text1, XtNcallback, action, NULL);
    XtAddCallback(text2, XtNcallback, action2, NULL);

    XtRealizeWidget(toplevel);
    dpy = XtDisplay(toplevel);
    screen = DefaultScreen(dpy);
    table  = XtWindow(paned);

    XtAppMainLoop(app_con);	/* does not return */
    return 0;
}
