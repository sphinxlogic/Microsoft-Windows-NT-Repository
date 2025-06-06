/*
 * Sun clock.  X11 version by John Mackin.
 *
 * This program was derived from, and is still in part identical with, the
 * Suntools Sun clock program whose author's comment appears immediately
 * below.  Please preserve both notices.
 *
 * The X11R3/4 version of this program was written by John Mackin, at the
 * Basser Department of Computer Science, University of Sydney, Sydney,
 * New South Wales, Australia; <john@cs.su.oz.AU>.  This program, like
 * the one it was derived from, is in the public domain: `Love is the
 * law, love under will.'
 */

/*

	Sun clock

	Designed and implemented by John Walker in November of 1988.

	Version for the Sun Workstation.

    The algorithm used to calculate the position of the Sun is given in
    Chapter 18 of:

    "Astronomical  Formulae for Calculators" by Jean Meeus, Third Edition,
    Richmond: Willmann-Bell, 1985.  This book can be obtained from:

       Willmann-Bell
       P.O. Box 35025
       Richmond, VA  23235
       USA
       Phone: (804) 320-7016

    This program was written by:

       John Walker
       Autodesk, Inc.
       2320 Marinship Way
       Sausalito, CA  94965
       USA
       Fax:   (415) 389-9418
       Voice: (415) 332-2344 Ext. 2829
       Usenet: {sun,well,uunet}!acad!kelvin
	   or: kelvin@acad.uu.net

    This  program is in the public domain: "Do what thou wilt shall be the
    whole of the law".  I'd appreciate  receiving  any  bug  fixes  and/or
    enhancements,  which  I'll  incorporate  in  future  versions  of  the
    program.  Please leave the original attribution information intact	so
    that credit and blame may be properly apportioned.

    Revision history:

	1.0  12/21/89  Initial version.
	      8/24/89  Finally got around to submitting.

    Modification history:

        1.1  11/2/92   Updated by - Ed Smith (smith@spezko.enet.dec.com)
                       - ported to OpenVMS AXP
                       - added window and icon title switches for VMS
                       - added zone and offset for GMT as switches for VMS 
*/


#define	FAILFONT	"fixed"

#define	VERSION		"1.1"
#ifndef BIGFONT
#define BIGFONT "-Adobe-Helvetica-Bold-R-Normal--*-120-*"
/*#define BIGFONT "-*-*-Bold-R-Normal--*-140-*"*/
#endif
#ifndef SMALLFONT
#define SMALLFONT "-Misc-Fixed-Bold-R-Normal--13-120-75-75-C-70-ISO8859-1"
/*#define SMALLFONT "-*-Menu-Medium-R-Normal--*-100-*"*/
#endif 

#include "sunclock.h"

struct sunclock {
	int		s_width;	/* size of pixmap */
	int		s_height;
	Window		s_window;	/* associated window */
	Pixmap		s_pixmap;	/* and pixmap */
	int		s_flags;	/* see below */
	int		s_noon;		/* position of noon */
	short *		s_wtab1;	/* current width table (?) */
	short *		s_wtab;		/* previous width table (?) */
	long		s_increm;	/* increment for fake time */
	long		s_time;		/* time - real or fake, see flags */
	GC		s_gc;		/* GC for writing text into window */
	char *		(*s_tfunc)();	/* function to return the text */
	char		s_text[80];	/* and the current text that's there */
	int		s_textx;	/* where to draw the text */
	int		s_texty;	/* where to draw the text */
	long		s_projtime;	/* last time we projected illumination */
	int		s_timeout;	/* time until next image update */
	struct sunclock * s_next;	/* pointer to next clock context */
};

/*
 * bits in s_flags
 */

#define	S_FAKE		01		/* date is fake, don't use actual time */
#define	S_ANIMATE	02		/* do animation based on increment */
#define	S_DIRTY		04		/* pixmap -> window copy required */
#define	S_ICON		010		/* this is the icon window */

char *				strrchr();
#ifndef VMS
long				time();
#endif

#ifdef VMS
char *                          Zone = "";
int                             Offset = 0, NORMAL_OFFSET, DST_OFFSET; 
#endif

double				jtime();
double				gmst();
char *				salloc();
char *				bigtprint();
char *				smalltprint();
struct sunclock *		makeClockContext();
Bool				evpred();

char *				Wdayname[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
char *				Monname[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
	"Aug", "Sep", "Oct", "Nov", "Dec"
};

struct geom {
	int			mask;
	int			x;
	int			y;
};

char *                          Icon_title = "SunClock";
char *                          Window_title = "SunClock";
char *				Name;
char *				Display_name = "";
Display *			dpy;
int				scr;
unsigned long			Black;
unsigned long			White;
GC				GC_store;
GC				GC_invert;
GC				GC_bigf;
GC				GC_smallf;
XFontStruct *			SmallFont;
XFontStruct *			BigFont;
Pixmap				Mappix;
Pixmap				Iconpix;
Window				Icon;
Window				ClockWin;
struct sunclock *		Current;
int				Iconic = 0;
struct geom			Geom = { 0, 0, 0 };
struct geom			Icongeom = { 0, 0, 0 };

main(argc, argv)
int				argc;
register char **		argv;
{
	char *			p;

	Name = *argv;
#ifndef VMS
	if (p = strrchr(Name, '/'))
		Name = ++p;
#else
	if (p = strrchr(Name, '<')) {
	    Name = ++p;
	    if (p = strchr(Name, '.'))
		*p = '\0';
	} else {
	    if (p = strrchr(Name, ']')) {
		Name = ++p;
		if (p = strchr(Name, '.'))
		    *p = '\0';
	    }
	}
#endif
	parseArgs(argc, argv);

#ifdef VMS

/* For VMS: offset from local time to GMT, use 4 and 5 for EDT/EST. */

        if ((Offset < 1) || (Offset > 24)) usage();
        NORMAL_OFFSET = Offset;
        DST_OFFSET = Offset - 1;

#endif

        dpy = XOpenDisplay(Display_name);
	if (dpy == (Display *)NULL) {
		fprintf(stderr, "%s: can't open display `%s'\n", Display_name);
		exit(1);
	}
	scr = DefaultScreen(dpy);

	getColors();
	getFonts();
	makePixmaps();
	makeWindows();
	makeGCs(ClockWin, Mappix);
	setAllHints(argc, argv);
	makeClockContexts();

	XSelectInput(dpy, ClockWin, ExposureMask);
	XSelectInput(dpy, Icon, ExposureMask);
	XMapWindow(dpy, ClockWin);

	eventLoop();

	/*
	 * eventLoop() never returns, but one day it might, if someone adds a
	 * menu for animation or such with a "quit" option.
	 */

	ByeBye();
	exit(0);
}

parseArgs(argc, argv)
register int			argc;
register char **		argv;
{
	while (--argc > 0) {
		++argv;
		if (strcmp(*argv, "-display") == 0) {
			needMore(argc, argv);
			Display_name = *++argv;
			--argc;
                }
		else if (strcmp(*argv, "-wtitle") == 0) {
			needMore(argc, argv);
			Window_title = *++argv;
			--argc;
                }
		else if (strcmp(*argv, "-ititle") == 0) {
			needMore(argc, argv);
			Icon_title = *++argv;
			--argc;
                }
#ifdef VMS
                else if (strcmp(*argv, "-zone") == 0) {
                        needMore(argc, argv);
                        Zone = *++argv;
                        if (strlen(*argv) > 3) usage();
                        --argc;
                }
                else if (strcmp(*argv, "-offset") == 0) {
                        needMore(argc, argv);
                        Offset = atoi(*++argv);
                        --argc;
                }
#endif
        	else if (strcmp(*argv, "-iconic") == 0)
			Iconic++;
		else if (strcmp(*argv, "-geometry") == 0) {
			needMore(argc, argv);
			getGeom(*++argv, &Geom);
			--argc;
		}
		else if (strcmp(*argv, "-icongeometry") == 0) {
			needMore(argc, argv);
			getGeom(*++argv, &Icongeom);
			--argc;
		}
		else if (strcmp(*argv, "-version") == 0) {
			fprintf(stderr, "%s: version %s patchlevel %d\n",
				Name, VERSION, PATCHLEVEL);
			exit(0);
		}
		else
			usage();
	}
}

needMore(argc, argv)
register int			argc;
register char **		argv;
{
	if (argc == 1) {
		fprintf(stderr, "%s: option `%s' requires an argument\n",
			Name, *argv);
		usage();
	}
}

getGeom(s, g)
register char *			s;
register struct geom *		g;
{
	register int		mask;
	unsigned int		width;
	unsigned int		height;

	mask = XParseGeometry(s, &g->x, &g->y, &width, &height);
	if (mask == 0) {
		fprintf(stderr, "%s: `%s' is a bad geometry specification\n",
			Name, s);
		exit(1);
	}
	if ((mask & WidthValue) || (mask & HeightValue))
		fprintf(stderr, "%s: warning: width/height in geometry `%s' ignored\n",
			Name, s);
	g->mask = mask;
}

/*
 * Free resources.
 */

ByeBye()
{
	XFreeGC(dpy, GC_store);
	XFreeGC(dpy, GC_invert);
	XFreeGC(dpy, GC_bigf);
	XFreeGC(dpy, GC_smallf);
	XFreeFont(dpy, BigFont);
	XFreeFont(dpy, SmallFont);
	XFreePixmap(dpy, Mappix);
	XFreePixmap(dpy, Iconpix);
	XDestroyWindow(dpy, ClockWin);
	XDestroyWindow(dpy, Icon);
	XCloseDisplay(dpy);
}

usage()
{
        fprintf(stderr, " \n");        
#ifdef VMS
	fprintf(stderr, "Usage: %s -offset # [options]\n", Name); 
        fprintf(stderr, " \n");
        fprintf(stderr, " -offset #             : [1mRequired[0m - # of hours from GMT\n");
        fprintf(stderr, "                         eg: EST is 5, PST is 8\n");
        fprintf(stderr, "[-zone aaa]            : Timezone, eg: EST, maximum of 3 characters\n");
#else
        fprintf(stderr, "Usage: %s [options]\n", Name);
        fprintf(stderr, " \n");
#endif
        fprintf(stderr, "[-display dispname]    : Target display\n"); 
        fprintf(stderr, "[-wtitle windowtitle]  : SunClock's window title\n"); 
        fprintf(stderr, "[-geometry +x+y]       : Window geometry\n"); 
        fprintf(stderr, "[-version]             : Displays SunClock's version\n");
        fprintf(stderr, "[-iconic]              : Start up as an icon\n");
        fprintf(stderr, "[-ititle icontitle]    : Icon's title\n");
        fprintf(stderr, "[-icongeometry +x+y]   : Icon geometry\n");
        fprintf(stderr, " \n");
	exit(1);
}

/*
 * Set up stuff the window manager will want to know.  Must be done
 * before mapping window, but after creating it.
 */

setAllHints(argc, argv)
int				argc;
char **				argv;
{
	XClassHint		xch;
	XSizeHints		xsh;
	XWMHints		xwmh;

	xch.res_name = Name;
	xch.res_class = "Sunclock";
	XSetClassHint(dpy, ClockWin, &xch);
	XStoreName(dpy, ClockWin, Window_title);

	XSetCommand(dpy, ClockWin, argv, argc);

	XSetIconName(dpy, ClockWin, Icon_title);

	xsh.flags = PSize | PMinSize | PMaxSize;
	if (Geom.mask & (XValue | YValue)) {
		xsh.x = Geom.x;
		xsh.y = Geom.y;
		xsh.flags |= USPosition;
	}
	xsh.width = xsh.min_width = xsh.max_width = large_map_width;
	xsh.height = xsh.min_height = xsh.max_height = large_map_height;
	XSetNormalHints(dpy, ClockWin, &xsh);

	xwmh.flags = InputHint | StateHint | IconWindowHint;
	if (Icongeom.mask & (XValue | YValue)) {
		xwmh.icon_x = Icongeom.x;
		xwmh.icon_y = Icongeom.y;
		xwmh.flags |= IconPositionHint;
	}
	xwmh.input = False;
	xwmh.initial_state = Iconic ? IconicState : NormalState;
	xwmh.icon_window = Icon;
	XSetWMHints(dpy, ClockWin, &xwmh);
}

makeWindows()
{
	register int		ht;
	XSetWindowAttributes	xswa;
	register int		mask;

	ht = icon_map_height + SmallFont->max_bounds.ascent +
	     SmallFont->max_bounds.descent + 2;
	xswa.background_pixel = White;
	xswa.border_pixel = Black;
	xswa.backing_store = WhenMapped;
	mask = CWBackPixel | CWBorderPixel | CWBackingStore;

	fixGeometry(&Geom, large_map_width, large_map_height);
	ClockWin = XCreateWindow(dpy, RootWindow(dpy, scr), Geom.x, Geom.y,
			      large_map_width, large_map_height, 3, CopyFromParent,
			      InputOutput, CopyFromParent, mask, &xswa);

	fixGeometry(&Icongeom, icon_map_width, ht);
	Icon = XCreateWindow(dpy, RootWindow(dpy, scr), Icongeom.x, Icongeom.y,
			     icon_map_width, ht, 1, CopyFromParent, InputOutput,
			     CopyFromParent, mask, &xswa);

}

fixGeometry(g, w, h)
register struct geom *		g;
register int			w;
register int			h;
{
	if (g->mask & XNegative)
		g->x = DisplayWidth(dpy, scr) - w + g->x;
	if (g->mask & YNegative)
		g->y = DisplayHeight(dpy, scr) - h + g->y;
}

makeGCs(w, p)
register Window			w;
register Pixmap			p;
{
	XGCValues		gcv;

	gcv.foreground = Black;
	gcv.background = White;
	GC_store = XCreateGC(dpy, w, GCForeground | GCBackground, &gcv);
	gcv.function = GXinvert;
	GC_invert = XCreateGC(dpy, p, GCForeground | GCBackground | GCFunction, &gcv);

	gcv.font = BigFont->fid;
	GC_bigf = XCreateGC(dpy, w, GCForeground | GCBackground | GCFont, &gcv);
	gcv.font = SmallFont->fid;
	GC_smallf = XCreateGC(dpy, w, GCForeground | GCBackground | GCFont, &gcv);
}

getColors()
{
	XColor			c;
	XColor			e;
	register Status		s;

	s = XAllocNamedColor(dpy, DefaultColormap(dpy, scr), "Black", &c, &e);
	if (s != (Status)1) {
		fprintf(stderr, "%s: warning: can't allocate color `Black'\n");
		Black = BlackPixel(dpy, scr);
	}
	else
		Black = c.pixel;
	s = XAllocNamedColor(dpy, DefaultColormap(dpy, scr), "White", &c, &e);
	if (s != (Status)1) {
		fprintf(stderr, "%s: can't allocate color `White'\n");
		White = WhitePixel(dpy, scr);
	}
	else
		White = c.pixel;
}

getFonts()
{
	BigFont = XLoadQueryFont(dpy, BIGFONT);
	if (BigFont == (XFontStruct *)NULL) {
		fprintf(stderr, "%s: can't open font `%s', using `%s'\n",
			Name, BIGFONT, FAILFONT);
		BigFont = XLoadQueryFont(dpy, FAILFONT);
		if (BigFont == (XFontStruct *)NULL) {
			fprintf(stderr, "%s: can't open font `%s', giving up\n",
				Name, FAILFONT);
			exit(1);
		}
	}
	SmallFont = XLoadQueryFont(dpy, SMALLFONT);
	if (SmallFont == (XFontStruct *)NULL) {
		fprintf(stderr, "%s: can't open font `%s', using `%s'\n",
			Name, SMALLFONT, FAILFONT);
		SmallFont = XLoadQueryFont(dpy, FAILFONT);
		if (SmallFont == (XFontStruct *)NULL) {
			fprintf(stderr, "%s: can't open font `%s', giving up\n",
				Name, FAILFONT);
			exit(1);
		}
	}
}

makePixmaps()
{
	Mappix = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
				 large_map_bits, large_map_width,
				 large_map_height, 0, 1, 1);

	Iconpix = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
				 icon_map_bits, icon_map_width,
				 icon_map_height, 0, 1, 1);
}

makeClockContexts()
{
	register struct sunclock * s;

	s = makeClockContext(large_map_width, large_map_height, ClockWin, Mappix,
			     GC_bigf, bigtprint, 70,
			     large_map_height - BigFont->max_bounds.descent - 1);
	Current = s;
	s = makeClockContext(icon_map_width, icon_map_height, Icon, Iconpix,
			     GC_smallf, smalltprint, 6,
			     icon_map_height + SmallFont->max_bounds.ascent + 1);
	Current->s_next = s;
	s->s_flags |= S_ICON;
	s->s_next = Current;
}

struct sunclock *
makeClockContext(wid, ht, win, pix, gc, fun, txx, txy)
int				wid;
int				ht;
Window				win;
Pixmap				pix;
GC				gc;
char *				(*fun)();
int				txx;
int				txy;
{
	register struct sunclock * s;

	s = (struct sunclock *)salloc(sizeof (struct sunclock));
	s->s_width = wid;
	s->s_height = ht;
	s->s_window = win;
	s->s_pixmap = pix;
	s->s_flags = S_DIRTY;
	s->s_noon = -1;
	s->s_wtab = (short *)salloc((int)(ht * sizeof (short *)));
	s->s_wtab1 = (short *)salloc((int)(ht * sizeof (short *)));
	s->s_increm = 0L;
	s->s_time = 0L;
	s->s_gc	= gc;
	s->s_tfunc = fun;
	s->s_timeout = 0;
	s->s_projtime = -1L;
	s->s_text[0] = '\0';
	s->s_textx = txx;
	s->s_texty = txy;

	return (s);
}	

/*
 * Someone is sure to wonder why the event loop is coded this way, without
 * using select().  The answer is that this was developed on a System V
 * kernel, which has select() but the call has bugs; so, I was inspired
 * to make it portable to systems without select().  The slight delay in
 * expose event processing that results from using sleep(1) rather than
 * alarm() is a fine payoff for not having to worry about interrupted
 * system calls.
 *
 * I've got to use XCheckIfEvent with a degenerate predicate rather than
 * XCheckMaskEvent with a mask of -1L because the latter won't collect all
 * types of events, notably ClientMessage and Selection events.  Sigh.
 */

eventLoop()
{
	XEvent			ev;

	for (;;) {
		if (XCheckIfEvent(dpy, &ev, evpred, (char *)0))
			switch (ev.type) {
		
			case Expose:
				if (ev.xexpose.count == 0)
					doExpose(ev.xexpose.window);
				break;
			}
		else {
			sleep(1);
			doTimeout();
		}
	}
}

Bool
evpred(d, e, a)
register Display *		d;
register XEvent *		e;
register char *			a;
{
	return (True);
}

/*
 * Got an expose event for window w.  Do the right thing if it's not
 * currently the one we're displaying.
 */

doExpose(w)
register Window			w;
{
	if (w != Current->s_window) {
		Current = Current->s_next;
		if (w != Current->s_window) {
			fprintf(stderr,
				"%s: expose event for unknown window, id = 0x%08lx\n",
				w);
			exit(1);
		}
		setTimeout(Current);
	}
	updimage(Current);
	Current->s_flags |= S_DIRTY;
	showImage(Current);
}

doTimeout()
{
	if (QLength(dpy))
		return;		/* ensure events processed first */
	if (--Current->s_timeout <= 0) {
		updimage(Current);
		showImage(Current);
		setTimeout(Current);
	}
}

setTimeout(s)
register struct sunclock *	s;
{
	long			t;

	if (s->s_flags & S_ICON) {
		time(&t);
		s->s_timeout = 60 - localtime(&t)->tm_sec;
	}
	else
		s->s_timeout = 1;
}

showImage(s)
register struct sunclock *	s;
{
	register char *		p;
	struct tm		lt;
	register struct tm *	gmtp;

	lt = *localtime(&s->s_time);
#ifndef VMS
	gmtp = gmtime(&s->s_time);
#else
	gmtp = localtime(&s->s_time);
#ifdef DST
	gmtp->tm_hour += DST_OFFSET;
#else
	gmtp->tm_hour += NORMAL_OFFSET;
#endif
	if (gmtp->tm_hour > 23) {
	   gmtp->tm_hour -= 24;
	   gmtp->tm_mday++;
	}
	gmtp->tm_isdst = 1;
#endif
	p = (*s->s_tfunc)(&lt, gmtp);

	if (s->s_flags & S_DIRTY) {
		XCopyPlane(dpy, s->s_pixmap, s->s_window, GC_store, 0, 0,
			   s->s_width, s->s_height, 0, 0, 1);
		if (s->s_flags & S_ICON)
			XClearArea(dpy, s->s_window, 0, s->s_height + 1,
				   0, 0, False);
		s->s_flags &= ~S_DIRTY;
	}
	strcpy(s->s_text, p);
	showText(s);
}

showText(s)
register struct sunclock *	s;
{
	XDrawImageString(dpy, s->s_window, s->s_gc, s->s_textx,
			 s->s_texty, s->s_text, strlen(s->s_text));
}

/* --- */
/*  UPDIMAGE  --  Update current displayed image.  */

updimage(s)
register struct sunclock *	s;
{
	register int		i;
	int			xl;
	struct tm *		ct;
	double			jt;
	double			sunra;
	double			sundec;
	double			sunrv;
	double			sunlong;
	double			gt;
	struct tm		lt;
	short *			wtab_swap;

	/* If this is a full repaint of the window, force complete
	   recalculation. */

	if (s->s_noon < 0) {
		s->s_projtime = 0;
		for (i = 0; i < s->s_height; i++) {
			s->s_wtab1[i] = -1;
		}
	}

	if (s->s_flags & S_FAKE) {
		if (s->s_flags & S_ANIMATE)
			s->s_time += s->s_increm;
		if (s->s_time < 0)
			s->s_time = 0;
	} else
		time(&s->s_time);
	lt = *localtime(&s->s_time);

#ifndef VMS
	ct = gmtime(&s->s_time);
#else
	ct = localtime(&s->s_time);
#ifdef DST
	ct->tm_hour += DST_OFFSET;
#else
	ct->tm_hour += NORMAL_OFFSET;
#endif
	if (ct->tm_hour > 23) {
	   ct->tm_hour -= 24;
	   ct->tm_mday++;
	}
	ct->tm_isdst = 1;
#endif

	jt = jtime(ct);
	sunpos(jt, False, &sunra, &sundec, &sunrv, &sunlong);
	gt = gmst(jt);

	/* Projecting the illumination curve  for the current seasonal
           instant is costly.  If we're running in real time, only  do
	   it every PROJINT seconds.  */

	if ((s->s_flags & S_FAKE)
	 || s->s_projtime < 0
	 || (s->s_time - s->s_projtime) > PROJINT) {
		projillum(s->s_wtab, s->s_width, s->s_height, sundec);
		wtab_swap = s->s_wtab;
		s->s_wtab = s->s_wtab1;
		s->s_wtab1 = wtab_swap;
		s->s_projtime = s->s_time;
	}

	sunlong = fixangle(180.0 + (sunra - (gt * 15)));
	xl = sunlong * (s->s_width / 360.0);

	/* If the subsolar point has moved at least one pixel, update
	   the illuminated area on the screen.	*/

	if ((s->s_flags & S_FAKE) || s->s_noon != xl) {
		moveterm(s->s_wtab1, xl, s->s_wtab, s->s_noon, s->s_width,
			 s->s_height, s->s_pixmap);
		s->s_noon = xl;
		s->s_flags |= S_DIRTY;
	}
}

/*  PROJILLUM  --  Project illuminated area on the map.  */

projillum(wtab, xdots, ydots, dec)
short *wtab;
int xdots, ydots;
double dec;
{
	int i, ftf = True, ilon, ilat, lilon, lilat, xt;
	double m, x, y, z, th, lon, lat, s, c;

	/* Clear unoccupied cells in width table */

	for (i = 0; i < ydots; i++)
		wtab[i] = -1;

	/* Build transformation for declination */

	s = sin(-dtr(dec));
	c = cos(-dtr(dec));

	/* Increment over a semicircle of illumination */

	for (th = -(PI / 2); th <= PI / 2 + 0.001;
	    th += PI / TERMINC) {

		/* Transform the point through the declination rotation. */

		x = -s * sin(th);
		y = cos(th);
		z = c * sin(th);

		/* Transform the resulting co-ordinate through the
		   map projection to obtain screen co-ordinates. */

		lon = (y == 0 && x == 0) ? 0.0 : rtd(atan2(y, x));
		lat = rtd(asin(z));

		ilat = ydots - (lat + 90) * (ydots / 180.0);
		ilon = lon * (xdots / 360.0);

		if (ftf) {

			/* First time.  Just save start co-ordinate. */

			lilon = ilon;
			lilat = ilat;
			ftf = False;
		} else {

			/* Trace out the line and set the width table. */

			if (lilat == ilat) {
				wtab[(ydots - 1) - ilat] = ilon == 0 ? 1 : ilon;
			} else {
				m = ((double) (ilon - lilon)) / (ilat - lilat);
				for (i = lilat; i != ilat; i += sgn(ilat - lilat)) {
					xt = lilon + floor((m * (i - lilat)) + 0.5);
					wtab[(ydots - 1) - i] = xt == 0 ? 1 : xt;
				}
			}
			lilon = ilon;
			lilat = ilat;
		}
	}

	/* Now tweak the widths to generate full illumination for
	   the correct pole. */

	if (dec < 0.0) {
		ilat = ydots - 1;
		lilat = -1;
	} else {
		ilat = 0;
		lilat = 1;
	}

	for (i = ilat; i != ydots / 2; i += lilat) {
		if (wtab[i] != -1) {
			while (True) {
				wtab[i] = xdots / 2;
				if (i == ilat)
					break;
				i -= lilat;
			}
			break;
		}
	}
}

/*  XSPAN  --  Complement a span of pixels.  Called with line in which
	       pixels are contained, leftmost pixel in the  line,  and
	       the   number   of   pixels   to	 complement.   Handles
	       wrap-around at the right edge of the screen.  */

xspan(pline, leftp, npix, xdots, p)
register int			pline;
register int			leftp;
register int			npix;
register int			xdots;
register Pixmap			p;
{
	leftp = leftp % xdots;

	if (leftp + npix > xdots) {
		XDrawLine(dpy, p, GC_invert, leftp, pline, xdots - 1, pline);
		XDrawLine(dpy, p, GC_invert, 0, pline,
			  (leftp + npix) - (xdots + 1), pline);
	}
	else
		XDrawLine(dpy, p, GC_invert, leftp, pline,
			  leftp + (npix - 1), pline);
}

/*  MOVETERM  --  Update illuminated portion of the globe.  */

moveterm(wtab, noon, otab, onoon, xdots, ydots, pixmap)
short *wtab, *otab;
int noon, onoon, xdots, ydots;
Pixmap pixmap;
{
	int i, ol, oh, nl, nh;

	for (i = 0; i < ydots; i++) {

		/* If line is off in new width table but is set in
		   the old table, clear it. */

		if (wtab[i] < 0) {
			if (otab[i] >= 0) {
				xspan(i, ((onoon - otab[i]) + xdots) % xdots,
				    otab[i] * 2, xdots, pixmap);
			}
		} else {

			/* Line is on in new width table.  If it was off in
			   the old width table, just draw it. */

			if (otab[i] < 0) {
				xspan(i, ((noon - wtab[i]) + xdots) % xdots,
				    wtab[i] * 2, xdots, pixmap);
			} else {

				/* If both the old and new spans were the entire
				   screen, they're equivalent. */

				if (otab[i] == wtab[i] && wtab[i] == (xdots / 2))
					continue;

				/* The line was on in both the old and new width
				   tables.  We must adjust the difference in the
				   span.  */

				ol =  ((onoon - otab[i]) + xdots) % xdots;
				oh = (ol + otab[i] * 2) - 1;
				nl =  ((noon - wtab[i]) + xdots) % xdots;
				nh = (nl + wtab[i] * 2) - 1;

				/* If spans are disjoint, erase old span and set
				   new span. */

				if (oh < nl || nh < ol) {
					xspan(i, ol, (oh - ol) + 1, xdots, pixmap);
					xspan(i, nl, (nh - nl) + 1, xdots, pixmap);
				} else {
					/* Clear portion(s) of old span that extend
					   beyond end of new span. */
					if (ol < nl) {
						xspan(i, ol, nl - ol, xdots, pixmap);
						ol = nl;
					}
					if (oh > nh) {
						xspan(i, nh + 1, oh - nh, xdots, pixmap);
						oh = nh;
					}
					/* Extend existing (possibly trimmed) span to
					   correct new length. */
					if (nl < ol) {
						xspan(i, nl, ol - nl, xdots, pixmap);
					}
					if (nh > oh) {
						xspan(i, oh + 1, nh - oh, xdots, pixmap);
					}
				}
			}
		}
		otab[i] = wtab[i];
	}
}

char *
salloc(nbytes)
register int			nbytes;
{
	register char *		p;
	char *			malloc();

	p = malloc((unsigned)nbytes);
	if (p == (char *)NULL) {
		fprintf(stderr, "%s: out of memory\n");
		exit(1);
	}
	return (p);
}

char *
bigtprint(ltp, gmtp)
register struct tm *		ltp;
register struct tm *		gmtp;
{
	static char		s[80];
	sprintf(s,
		"%02d:%02d:%02d %s %s %02d %s %02d     %02d:%02d:%02d UTC %s %02d %s %02d",
		ltp->tm_hour, ltp->tm_min,
		ltp->tm_sec,
#ifdef	NEW_CTIME
		ltp->tm_zone,
#else
#ifndef VMS
 		tzname[ltp->tm_isdst],
#else
                Zone,
#endif
#endif
		Wdayname[ltp->tm_wday], ltp->tm_mday,
		Monname[ltp->tm_mon], ltp->tm_year % 100,
		gmtp->tm_hour, gmtp->tm_min,
		gmtp->tm_sec, Wdayname[gmtp->tm_wday], gmtp->tm_mday,
		Monname[gmtp->tm_mon], gmtp->tm_year % 100);

	return (s);
}

char *
smalltprint(ltp, gmtp)
register struct tm *		ltp;
register struct tm *		gmtp;
{
	static char		s[80];

	sprintf(s, "%02d:%02d %s %02d %s %02d", ltp->tm_hour, ltp->tm_min,
		Wdayname[ltp->tm_wday], ltp->tm_mday, Monname[ltp->tm_mon],
		ltp->tm_year % 100);

	return (s);
}
