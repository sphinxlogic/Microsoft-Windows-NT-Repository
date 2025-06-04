/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xlib-windows.c			     */
/*									     */
/*	windows creation for the Xlib interface				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	based on the windows.c module from Spider by David Lemke	     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useXlib

#include "X-pat.h"

#include "pat.bm"
#include "gray1.bm"

void init_fonts(const char *statusfont, const char *buttonfont)
{   static const char *buttonfonttable[] = {
	NULL,
	"-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1",
	"-misc-fixed-bold-r-normal--15-140-75-75-c-90-iso8859-1",
	"-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso8859-1",
	"-misc-fixed-bold*",
	"fixed"
	};
    int i;
    XGCValues gcv;
    long gcflags;

    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcv.graphics_exposures = False;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    
    buttonfonttable[0] = buttonfont;
    
    /* add on to blackgc */
    if ((graphic.message_font = XLoadQueryFont(dpy, statusfont)) == NULL) {
	(void) fprintf(stderr,"can't get font %s\n", statusfont);
	exit(0);
    }
    graphic.message_height = graphic.message_font->ascent + graphic.message_font->descent + 2 * TABLE_BW;
    gcv.font = graphic.message_font->fid;
    gcflags |= GCFont;
    
    graphic.textgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);

    for (i = 0; i < sizeof(buttonfonttable) / sizeof(buttonfonttable[0]); ++i) {
	if (!buttonfonttable[i])
	    continue;	/* no user font */
	if ((button.font = XLoadQueryFont(dpy, buttonfonttable[i])))
	    break;
	fprintf(stderr,"warning: can't get font %s\n", buttonfonttable[i]);
    }
    if (!button.font) {
	fprintf(stderr, "unable to get button font. terminating\n");
	exit(EXIT_FAILURE);
    }
    
    button.fontheight = button.font->ascent + button.font->descent;
    
    gcv.font = button.font->fid;
    gcflags |= GCFont;
    button.gc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}


void init_windows(int ac, char **av, const char *geom,
    int xgap, int ygap, int bradx, int brady, int bx, int by, int autolayout, const char *bkcolor)
{
    unsigned long borderpixel = blackpixel;
    XSetWindowAttributes winattr;
    long winmask;
    XSizeHints xsh;
    int x, y;
    Pixmap icon_map;

    graphic.xgap = (XSize_t)xgap;
    graphic.ygap = (XSize_t)ygap;
    graphic.autolayout = autolayout;

    button.bx = (XSize_t)bx;
    button.by = (XSize_t)by;
    if (button.fontheight + 2 * button.by < 2 * brady)
	brady = button.fontheight/2 + button.by;
    B_ROUND_W = bradx;
    B_ROUND_H = brady;

    /* compute minimum size needed */
    generic_minwindow((XSize_t *)&xsh.min_width, (XSize_t *)&xsh.min_height);
    graphic.width = xsh.min_width;
    graphic.height = xsh.min_height;
    graphic.min_width = graphic.width;
    graphic.min_height = graphic.height;

    xsh.flags = PPosition | PSize | PMinSize;
    x = y = 0;

    if (geom) {
	int flags = XParseGeometry(geom, &x, &y, &graphic.width, &graphic.height);
		
	/* don't let it start too short */
	if (flags & HeightValue && graphic.height < xsh.min_height)
	    graphic.height = xsh.min_height;
	/* don't let it start too narrow */
	if (flags & WidthValue && graphic.width < xsh.min_width)
	    graphic.width = xsh.min_width;

	if (flags & (WidthValue | HeightValue))
	    xsh.flags |= USSize;
	if (flags & (XValue | YValue))
	    xsh.flags |= USPosition;

	if (flags & XValue && flags & XNegative)
	    x = DisplayWidth(dpy, screen) - (graphic.width + x);
	if (flags & YValue && flags & YNegative)
	    y = DisplayHeight(dpy, screen) - (graphic.height + y);
    }

    /* with internal cards, we're fast enough to forget the backing store */
    /* (we already use a lot of memory to store the cards) */
    /* in the case NO_MEM, we don't have the images ready, but using */
    /* the backing store in this case would be a contradiction */
    {
	winattr.backing_store = card.cardset ? WhenMapped : NotUseful;
	winattr.border_pixel = blackpixel;
	winattr.bit_gravity = NorthWestGravity;
	winattr.event_mask = KeyPressMask | ExposureMask | ButtonPressMask | 
	    ButtonReleaseMask | StructureNotifyMask | Button3MotionMask;
	winmask = CWBorderPixel | CWEventMask | CWBackingStore | CWBitGravity;
	
	if (graphic.is_color)	{
	    XColor color;
	    Colormap cmap;
	    cmap = DefaultColormap(dpy, screen);
	    color.flags = DoRed | DoGreen | DoBlue;
	    XAllocNamedColor(dpy, cmap, bkcolor, &color, &color);
	    winattr.background_pixel = color.pixel;
	    winmask |= CWBackPixel;
	} else	{
	    winattr.background_pixmap = XCreateBitmapFromData(dpy,
	      RootWindow(dpy, screen), gray1_bits, gray1_width, gray1_height);
	    winmask |= CWBackPixmap;
	}
	table = XCreateWindow(dpy, RootWindow(dpy, screen), 
	    x, y, graphic.width, graphic.height, TABLE_BW,
	    CopyFromParent, CopyFromParent, CopyFromParent, winmask, &winattr);
    }

    xsh.x = x;
    xsh.y = y;

    xsh.width = graphic.width;
    xsh.height = graphic.height;

    icon_map = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
	pat_bits, pat_width, pat_height);

    XSetStandardProperties(dpy, table, "Patience", "Patience", icon_map,
	av, ac, &xsh);

    {   XWMHints xwmh;

	xwmh.flags = InputHint | IconPixmapHint;
	xwmh.input = True;
	xwmh.icon_pixmap = icon_map;
	XSetWMHints(dpy, table, &xwmh);
    }
    XStoreName(dpy, table, rules.longname ? rules.longname : rules.shortname);
    XSetIconName(dpy, table, rules.shortname);

    /* calculate size and position of confirm window */
    {   int w;
	long dummy;
	x = graphic.xgap;
	y = graphic.ygap + 3 * button.by + 2 * button.fontheight;
	(void)arrange_buttons(confirmbuttons, &x, &y, 32767, &dummy);
	w = XTextWidth(button.font, "Please confirm<space>", 21);
	if (x < w)
	    x = w;
	confirm.w = x;
	confirm.h = y;
	confirm.x = (graphic.width - x) / 2;
	confirm.y = (graphic.height - y) / 2;
    }
    {
	confirm.win = XCreateSimpleWindow(dpy, table, confirm.x, confirm.y,
 	    confirm.w, confirm.h, TABLE_BW, borderpixel, whitepixel);
	winattr.event_mask = ExposureMask | ButtonPressMask;
	winmask = CWEventMask;
	XChangeWindowAttributes(dpy, confirm.win, winmask, &winattr);

	finished_win = XCreateSimpleWindow(dpy, table, 0, 0,
 	    FINISHED_W, FINISHED_H, TABLE_BW, borderpixel, whitepixel);
	winattr.event_mask = ExposureMask;  /* shall receive exposure event */
	winmask = CWEventMask;
	XChangeWindowAttributes(dpy, finished_win, winmask, &winattr);
    }
    XMapWindow(dpy, table);
}
#endif
