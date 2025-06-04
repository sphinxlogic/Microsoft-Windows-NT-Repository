/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-gfx2.c				     */
/*									     */
/*	Alternate graphics interface for colour servers			     */
/*	Requires the xpm library.					     */
/*	written by Michael Bischoff (mbi@mo.math.nat.tu-bs.de)		     */
/*	March-1994							     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
/* #define SAVE_IMAGES */
#define ALLATONCE
/* #define MIRRORING */

#include "X-pat.h"
#include <time.h>
#ifndef VMS
#include <xpm.h>

#ifndef NO_ROUND_CARDS
#include <X11/Xmu/Drawing.h>
#endif
#else
#include "xpm.h"
#ifndef NO_ROUND_CARDS
#include <Xmu/Drawing.h>
#endif
#endif /* VMS */

/* with this module, cards are built the following way:
   1) If a complete card image does exist (filename: "(rank).(suit).xpm"),
      this image is read and used as the card pixmap. A rounded rectangle
      is drawn as a border. If the image does not exist, the card is
      constructed as follows.
   2) a white pixmap of full cardsize is created. The width and height
      are taken from the Cards.conf file (1st line) and default to 79x123.
   3) The default card background, taken from Background.xpm, is centered
      in the pixmap.
   4) Rank and small suit symbols are drawn into the pixmap.
   5) If a center picture does exist (filename "(rank).(suit).pic.xpm"),
      it is centered in the card. Otherwise, the card is filled with suit 
      symbols.
   6) A rounded rectangle is drawn as card border.

   For pictures (Jack to King), the procedure in 5) is slightly different,
   as the center pictures have to be mirrored and drawn twice.


   The cards may be configured by the Cards.conf file. This file is optional
   and may end after any line. The values not read from the file take their
   default values then.

   line | entries | comment, [default]
   -----+---------+--------------------------------------------------------
      1 |   2     | width, height of the cards	[79x123]
      2 |   2     | number of cardbacks, jokers [1, 1]
      3 |   1     | flags, bits are ored together, see below [0]
      4 |   2     | x, y distance of the border lines for pictures [6, 7]
      5 |   2     | Position of the medium-sized suit symbols for pictures
        |         |   (0 = omit them)			[11, computed]
      6 |   2     | x, y coordinate of the rank symbols [4,6]
      7 |   2     | x, y coordinate of the small suit symbol below the rank
        |         |   symbol				[3, computed]
      8 |   2     | xdelta, ydelta amount in pixels to shift large symbols
        |         |   closer together                   [0,0]
      9 |   3     | x0,x1,x2 the three possible x-coordinates of the large
        |         |   suit symbols			[computed]
     10 |   9     | y0..y8 the nine possible y-coordinates of the large
        |         |   suit symbols			[computed]
   -----+---------+---------------------------------------------------------
   Use of the lines 9 and 10 is intended for emergency cases only, as line
   8 should allow all configurations with equidistant suit symbols.
*/

/* for flags: */
#define TWORANKS	1	/* display two rank symbols? */
#define PRINT		2	/* print x(i), y(i) (Debugging) */
#define PIC_BG		4	/* use same background for pictures? */
#define NOCENTERLINE	8	/* place picture one lower and don't draw black center line */

#define RANKWIDTH	9
#define RANKHEIGHT	14
#define SUITSIZE0	41
#define SUITSIZE1	21
#define SUITSIZE2	15
#define SUITSIZE3	11
#define SUITOFF0	(0)
#define SUITOFF1	(SUITSIZE0)
#define SUITOFF2	(SUITSIZE0+SUITSIZE1)
#define SUITOFF3	(SUITSIZE0+SUITSIZE1+SUITSIZE2)

struct cardconfig {
    int width, height;
    int flags;
    int ydelta;		/* odd integer! */
    int Ox, Oy;		/* distance of the border for pictures */
    int Rx, Ry;		/* position of the rank symbol */
    int Sx, Sy;		/* position of the small suit symbol */
    int Mx, My;		/* position of the medium suit symbol */
    int x0, x1, x2;	/* x-positions of the large suit symbols */
    int y0, y1, y2, y3, y4, y5, y6, y7, y8;	/* y-positions */
    int numbacks, numjokers;
} cc;


static void read_cardconfig(const char *filename) {
    FILE *fp;
    int tmp[9];
    if (!(fp = fopen(filename, "r")))
	goto stdconfig;

    if (fscanf(fp, "%d %d", &CARD_WIDTH, &CARD_HEIGHT) != 2 ||
	CARD_WIDTH < 49 || CARD_WIDTH > 123 ||
	CARD_HEIGHT < 95 || CARD_HEIGHT > 200) {
	fclose(fp);
	fp = NULL;
	goto stdconfig;
    } else
	goto havesize;

 stdconfig:
    CARD_WIDTH = 79;
    CARD_HEIGHT = 123;
 havesize:
    cc.numbacks = cc.numjokers = 1;
    cc.ydelta = (((CARD_HEIGHT - 2 - 4 * SUITSIZE1) / 7) | 1);

    cc.x0 = (CARD_WIDTH - 1 * SUITSIZE1) / 2 - 2 * CARD_WIDTH / 9 + 1;
    cc.x1 = (CARD_WIDTH - 1 * SUITSIZE1) / 2;
    cc.x2 = (CARD_WIDTH - 1 * SUITSIZE1) / 2 + 2 * CARD_WIDTH / 9 - 1;

    cc.y0 = (CARD_HEIGHT - 3 * cc.ydelta - 4 * SUITSIZE1) / 2;
    cc.y1 = (CARD_HEIGHT - 2 * cc.ydelta - 3 * SUITSIZE1) / 2;
    cc.y2 = (CARD_HEIGHT - 1 * cc.ydelta - 2 * SUITSIZE1) / 2;
    cc.y3 = (CARD_HEIGHT - 0 * cc.ydelta - 1 * SUITSIZE1) / 2;
    cc.y4 = (CARD_HEIGHT + 1 * cc.ydelta - 0 * SUITSIZE1) / 2;
    cc.y5 = (CARD_HEIGHT + 2 * cc.ydelta + 1 * SUITSIZE1) / 2;
    cc.y6 = (CARD_HEIGHT + 3 * cc.ydelta + 2 * SUITSIZE1) / 2;
    cc.y7 = (cc.y0 + cc.y3) / 2;
    cc.y8 = (cc.y6 + cc.y3) / 2;

    cc.flags = 0;
    cc.Mx = 11;
    cc.My = cc.y0 + 3;
    cc.Ox = 6;
    cc.Oy = 7;
    cc.Rx = 4;
    cc.Ry = 6;
    cc.Sx = 3;
    cc.Sy = cc.y0 + SUITSIZE1 - SUITSIZE3;
    if (fp) {	/* Cards.Conf does exist */
	/* possibility to override the defaults */
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2)
	    cc.numbacks = tmp[0], cc.numjokers = tmp[1];
	if (fscanf(fp, "%d", tmp) == 1)
	    cc.flags = tmp[0];
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2)
	    cc.Ox = tmp[0],	cc.Oy = tmp[1];
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2)
	    cc.Mx = tmp[0],	cc.My = tmp[1];
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2)
	    cc.Rx = tmp[0],	cc.Ry = tmp[1];
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2)
	    cc.Sx = tmp[0],	cc.Sy = tmp[1];
	if (fscanf(fp, "%d %d", tmp, tmp+1) == 2) {	/* xdelta and ydelta */
	    cc.x0 += tmp[0], cc.x2 -= tmp[0],
	    cc.y0 += 3 * tmp[1];
	    cc.y1 += 2 * tmp[1];
	    cc.y2 += 1 * tmp[1];
	    cc.y4 -= 1 * tmp[1];
	    cc.y5 -= 2 * tmp[1];
	    cc.y6 -= 3 * tmp[1];
	    tmp[1] = tmp[1] * 3 / 2;
	    cc.y7 += tmp[1];
	    cc.y8 -= tmp[1];
	}
	if (fscanf(fp, "%d %d %d", tmp, tmp+1, tmp+2) == 3)
	    cc.x0 = tmp[0],	cc.x1 = tmp[1], cc.x2 = tmp[2];
	if (fscanf(fp, "%d %d %d %d %d %d %d %d %d", tmp, tmp+1, tmp+2, tmp+3,
		   tmp+4, tmp+5, tmp+6, tmp+7, tmp+8) == 9)
	    cc.y0 = tmp[0],	cc.y1 = tmp[1], cc.y2 = tmp[2],
	    cc.y3 = tmp[3],	cc.y4 = tmp[4], cc.y5 = tmp[5],
	    cc.y6 = tmp[6],	cc.y7 = tmp[7], cc.y8 = tmp[8];
	fclose(fp);
    }
    if (cc.flags & PRINT)
	printf("y = %d %d %d  %d %d %d  %d %d %d, x = %d %d %d\n",
	   cc.y0, cc.y1, cc.y2,
	   cc.y3, cc.y4, cc.y5,
	   cc.y6, cc.y7, cc.y8,
	   cc.x0, cc.x1, cc.x2);
    return;

}

#define MAXJOKERS 4

static Pixmap card_map[60+MAXJOKERS], card_clip;

/* we use a 2-phase algorithm, a combination of two mirrors */
/* the amount of data copied is greater, but there are far less calls to XCopyArea */
static void MirrorCard(Drawable p, int w, int h) {
    int x, y, H;
    H = h/2;
    /* first, build a left-right mirror */
    for (x = 0; x < w; ++x)
	XCopyArea(dpy, p, p, whitegc, x, 1, 1, H-1, w-1-x, H+2);
    /* now mirror the lower half of the card upside-down */
    for (y = 0; y < H/2; ++y) {
	XCopyArea(dpy, p, p, whitegc, 0, h-1-y, w, 1, 0, H+1+y);
	XCopyArea(dpy, p, p, whitegc, 0, H+2+y, w, 1, 0, h-1-y);
    }
    /* shift one scan line */
    XCopyArea(dpy, p, p, whitegc, 0, H+2+H/2, w, H/2, 0, H+1+H/2);
    
    /* restore the bottom line which was overwritten */
    XCopyArea(dpy, p, p, whitegc, 0, 0, w, 1, 0, h-1);
}

static const char *xpmdir;
static int mem_option;
static GC cardbackgc;		/* gc in use when drawing cardbacks */
static GC suitgc, rankgc;	/* gc in use when drawing suit and rank symbols */
/* these have fixed clipmasks */

#include "logo.bm"

static void make_GCs(void) {
    XGCValues gcv;
    long gcflags;
    Pixmap tmpmap;
    Pixmap logomap;

    /* make GC for cardbacks */
    tmpmap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
	logo_bits, logo_width, logo_height);
    logomap = XCreatePixmap(dpy, RootWindow(dpy, screen),
	logo_width, logo_height, DefaultDepth(dpy, screen));
    
    gcv.foreground = BlackPixel(dpy, screen);
    gcv.background = WhitePixel(dpy, screen);
    gcv.graphics_exposures = True;
    gcflags = GCForeground | GCBackground | GCGraphicsExposures;
    
    cardbackgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    suitgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    rankgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}


static char *xpmname(const char *s, const char *ext) {
    static char buff[200];
    sprintf(buff, "%s/%s.%s", xpmdir, s, ext);
    return buff;
}


void init_cards(const char *cardset, int rx, int ry,
    unsigned long red, unsigned long cbcolor, int mem, const char *xpm) {
    int i;

    for (i = 0; i < 58; ++i)
	card_map[i] = 0;

    mem_option = mem;
    make_GCs();
#ifdef NO_ROUND_CARDS
    rx = ry = 0;
#endif
    if (!xpm) {		/* we definitely need an xpmdir for this module */
	char *s;
	s = malloc(strlen(LIBDIR) + 10);
	sprintf(s, "%s/default", LIBDIR);
	xpmdir = s;
    } else if (*xpm != '.' && *xpm != '/') {	/* supply path */
	char *s;
	s = malloc(strlen(LIBDIR) + strlen(xpm) + 2);
	sprintf(s, "%s/%s", LIBDIR, xpm);
	xpmdir = s;
    }
    
    card.cardset = cardset;
    /* build internal cardset */
    read_cardconfig(xpmname("Cards", "conf"));


    /* sanity checks: */
    if (2 * rx >= (int)CARD_WIDTH)
	rx = CARD_WIDTH / 2;
    if (2 * ry >= (int)CARD_HEIGHT)
	ry = CARD_HEIGHT / 2;
    /* assign values */
    if (rx >= 0)
	card.rx = rx;
    if (ry >= 0)
	card.ry = ry;
    

    ROUND_W = ROUND_H = 9;
    STD_DELTA = cc.ydelta + SUITSIZE1;

    /* adapt arrows to match cardset sizes */
    graphic.aw = graphic.ah = STD_DELTA / 6;
    graphic.ya_h = graphic.xa_w = graphic.aw * 2;
    graphic.xa_h = graphic.ya_w = graphic.aw * 4;

    /* finally, compute a clipping mask for the cards */
    /* apply this clip mask to cardbackgc */
#ifndef NO_ROUND_CARDS
#ifndef STATIC_CLIPMAPS
    if (ROUND_W) {
	card_clip = XCreatePixmap(dpy, RootWindow(dpy, screen),
				    CARD_WIDTH+1, CARD_HEIGHT+1, 1);
	{    XGCValues	gcv;
	     long gcflags, f, b;
	     GC clipgc;
	     
	     b = 0;
	     f = 1;

	     /* first, clear the clipmap */
	     /* (is it possible to do it easier?) */
	     gcv.foreground = b;
	     gcv.background = f;
	     gcv.graphics_exposures = False;
	     gcflags = GCForeground | GCBackground | GCGraphicsExposures;
	     clipgc = XCreateGC(dpy, card_clip, gcflags, &gcv);
	     XFillRectangle(dpy, card_clip, clipgc, 0, 0, 
			    CARD_WIDTH, CARD_HEIGHT);
	     XFreeGC(dpy, clipgc);
	     
	     /* make gc for clipgc */
	     gcv.foreground = f;
	     gcv.background = b;
	     gcv.graphics_exposures = False;
	     gcflags = GCForeground | GCBackground | GCGraphicsExposures;
	     clipgc = XCreateGC(dpy, card_clip, gcflags, &gcv);
	     XmuFillRoundedRectangle(dpy, card_clip, clipgc, 0, 0, 
		     CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	     XmuDrawRoundedRectangle(dpy, card_clip, clipgc, 0, 0, 
		     CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	     XFreeGC(dpy, clipgc);
	 }
#ifdef DEBUG
	XWriteBitmapFile(dpy, "clip.mask.bm", card_clip, CARD_WIDTH, CARD_HEIGHT, -1, -1);
#endif
	/* fill the background */
	XSetClipMask(dpy, cardbackgc, card_clip);
    }
#else
    switch (ROUND_H) {
    case 3:
    case 4:
    case 5:
	card_clip = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
	    clip4_bits, clip4_width, clip4_height);
	XSetClipMask(dpy, cardbackgc, card_clip);
	break;
    case 6:
    case 7:
    case 8:
    case 9:
	card_clip = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
            clip7_bits, clip7_width, clip7_height);
	XSetClipMask(dpy, cardbackgc, card_clip);
	break;
    }
#endif
#endif
}

#ifdef SAVE_IMAGES
static void write_xpmfile(char *filename, Pixmap pict, Pixmap clip) {
    if (XpmWriteFileFromPixmap(dpy, filename, pict, clip, NULL) != XpmSuccess)
        fprintf(stderr, "error writing xpm file %s\n", filename);
}
#endif

static struct picture {
    int flag;
    int w, h;
    Pixmap map;
    Pixmap clip;
} big = { 0 }, bg = { 0 }, p_rank = { 0 }, p_suit = { 0 };


static int read_xpmfile(const char *name, struct picture *p) {
    XpmAttributes attribs;
    attribs.valuemask = 0;
    switch (XpmReadFileToPixmap(dpy, table, xpmname(name, "xpm"), &p->map, &p->clip, &attribs)) {
    case XpmSuccess:
	break;	/* yeah! */
    case XpmColorError:
    case XpmColorFailed:
	fprintf(stderr, "xpm: Not enough colors.\n");
	return p->flag = 0;
    case XpmFileInvalid:
	fprintf(stderr, "xpm: Invalid file.\n");
	return p->flag = 0;
    case XpmNoMemory:
	fprintf(stderr, "xpm: Out of memory.\n");
	return p->flag = 0;
    case XpmOpenFailed:
    default:
	/* maybe not serious */
	return p->flag = 0;
    }
    p->w = attribs.width;
    p->h = attribs.height;
    return p->flag = 1;
}

static void free_picture(struct picture *p) {
    XFreePixmap(dpy, p->map);
    if (p->clip) XFreePixmap(dpy, p->clip);
    p->flag = 0;
}

static int overlap_picture(const char *name, int two, Pixmap d) {
    struct picture p;
    int x, y;

    if (!read_xpmfile(name, &p))
	return 0;

    if (two) {
	x = (CARD_WIDTH-p.w) / 2;
	y = CARD_HEIGHT/2 - p.h;
	if (cc.flags & NOCENTERLINE)
	    ++y;
    } else {
	x = (CARD_WIDTH-p.w) / 2;
	y = (CARD_HEIGHT-p.h) / 2;
    }
    if (p.clip) {
	XSetClipMask(dpy, whitegc, p.clip);
	XSetClipOrigin(dpy, whitegc, x, y);
	XCopyArea(dpy, p.map, d, whitegc, 0, 0, p.w, p.h, x, y);
	XSetClipMask(dpy, whitegc, None);
    } else
	XCopyArea(dpy, p.map, d, whitegc, 0, 0, p.w, p.h, x, y);
    
    free_picture(&p);
    return 1;
}



/* suitflags: */
/*  0 3 0
      4
    1 5 1	 5 also 6 for asymmetric card (7) 
 -- 2 7 2 -- */

/* rank symbols:
   american: J, Q, K
   german:   B, D, K
   french:   V, D, R
   old german: O, U, K
*/



#define PAINT_AT(xx, yy) {					\
    XSetClipOrigin(dpy, suitgc, (xx)-x, (yy)-SUITSIZE0);	\
    XCopyArea(dpy, p_suit.map, d, suitgc, x,			\
        SUITSIZE0, SUITSIZE1, SUITSIZE1, xx, yy);		\
}
#define PAINT_RV(xx, yy) { 					\
    XSetClipOrigin(dpy, suitgc, (xx)-x, (yy)-SUITSIZE0);	\
    XCopyArea(dpy, p_suit.map, d, suitgc, x,			\
        SUITSIZE0, SUITSIZE1, SUITSIZE1, xx, yy);		\
}



/* paint a (nearly) symmetric card */
static void paint_large_1(Rank rank, Suit suit, Drawable d) {
    static const unsigned char suitflags[] = {  /* 25 => 03 for different 8 */
      0x80, 0x08, 0x88, 0x01, 0x81, 0x05, 0x45, 0x25, 0x83, 0x13, 0, 0, 0, 0 };
    int x, y, cx, cy, dl;
    char name[32];

#ifndef MIRRORING
    if (cc.Rx) {
	/* Draw the lower rank symbol(s). */
	x = (8 + 3 * (suit/2) - (rank / 5)) * RANKWIDTH;
	y = (4 - rank % 5) * RANKHEIGHT;
	dl = CARD_WIDTH-RANKWIDTH;
	cx = dl-cc.Rx;
	cy = CARD_HEIGHT-cc.Ry-RANKHEIGHT;
	if (p_rank.clip) XSetClipOrigin(dpy, rankgc, cx-x, cy-y);
	XCopyArea(dpy, p_rank.map, d, rankgc, x, y, RANKWIDTH, RANKHEIGHT, cx, cy);
	if (cc.flags & TWORANKS) {
	    cx = dl - cx;
	    if (p_rank.clip) XSetClipOrigin(dpy, rankgc, cx-x, cy-y);
	    XCopyArea(dpy, p_rank.map, d, rankgc, x, y, RANKWIDTH, RANKHEIGHT, cx, cy);
	}
    }
    if (cc.Sx) {
	/* Draw the lower small suit symbol(s). */
	x = (suit+4) * SUITSIZE3;
	y = SUITOFF3;
	dl = CARD_WIDTH-SUITSIZE3;
	cx = dl-cc.Sx;
	cy = CARD_HEIGHT-cc.Sy-SUITSIZE3;
	XSetClipOrigin(dpy, suitgc, cx-x, cy-y);
	XCopyArea(dpy, p_suit.map, d, suitgc, x, y,
		  SUITSIZE3, SUITSIZE3, cx, cy);
	if (cc.flags & TWORANKS) {
	    cx = dl - cx;
	    XSetClipOrigin(dpy, suitgc, cx-x, cy-y);
	    XCopyArea(dpy, p_suit.map, d, suitgc, x, y,
		      SUITSIZE3, SUITSIZE3, cx, cy);
	}
    }
#endif

    /* load picture instead of standard symbols? */
    sprintf(name, "%s.%s.pic", US_rank_name[rank], US_suit_name[suit]);
    if (overlap_picture(name, 0, d))
	return;

    /* OK, draw the suit symbols */
    /* first, all symbols which are not mirrored */
    x = SUITSIZE1 * suit;
    if (suitflags[rank] & 0x01) {
	PAINT_AT(cc.x0, cc.y0);
	PAINT_AT(cc.x2, cc.y0);
    }
    if (suitflags[rank] & 0x02) {
	PAINT_AT(cc.x0, cc.y2);
	PAINT_AT(cc.x2, cc.y2);
    }
    if (suitflags[rank] & 0x08) {
	PAINT_AT(cc.x1, cc.y0);
    }
    if (suitflags[rank] & 0x10) {
	PAINT_AT(cc.x1, cc.y1);
    }
    if (suitflags[rank] & 0x20) {
	PAINT_AT(cc.x1, cc.y7);
    }
#ifdef MIRRORING
    MirrorCard(d, CARD_WIDTH, CARD_HEIGHT);
#endif
    if (suitflags[rank] & 0x04) {
	PAINT_AT(cc.x0, cc.y3);
	PAINT_AT(cc.x2, cc.y3);
    }
    if (suitflags[rank] & 0x40) {
	PAINT_AT(cc.x1, cc.y7);
    }
    if (suitflags[rank] & 0x80) {
	PAINT_AT(cc.x1, cc.y3);
    }
#ifdef MIRRORING
    return;
#endif
    /* draw all symbols which are upside-down */
    x = SUITSIZE1 * (suit+4);
    if (suitflags[rank] & 0x01) {
	PAINT_RV(cc.x0, cc.y6);
	PAINT_RV(cc.x2, cc.y6);
    }
    if (suitflags[rank] & 0x02) {
	PAINT_RV(cc.x0, cc.y4);
	PAINT_RV(cc.x2, cc.y4);
    }
    if (suitflags[rank] & 0x08) {
	PAINT_RV(cc.x1, cc.y6);
    }
    if (suitflags[rank] & 0x10) {
	PAINT_RV(cc.x1, cc.y5);
    }
    if (suitflags[rank] & 0x20) {
	PAINT_RV(cc.x1, cc.y8);
    }
}

/* paint a half card and mirror it */
static void paint_large_2(Rank rank, Suit suit, Drawable d) {
    int x, y;
    if (cc.Mx > 0) {
	x = cc.Mx;
	y = cc.My;
	/* if (suit != 2)
	   --y; */
	XSetClipOrigin(dpy, suitgc, x-suit*SUITSIZE2, y-SUITOFF2);
	XCopyArea(dpy, p_suit.map, d, suitgc, SUITSIZE2 * suit, SUITOFF2, SUITSIZE2, SUITSIZE2, x, y);
    }
    /* horizontal line in the midst of the card: */
    if (!(cc.flags & NOCENTERLINE))
	XDrawLine(dpy, d, blackgc, cc.Ox, CARD_HEIGHT/2, CARD_WIDTH-1-cc.Ox, CARD_HEIGHT/2);
    if (cc.Oy)
	XDrawLine(dpy, d, blackgc, 4 + RANKWIDTH + 1, cc.Oy, CARD_WIDTH - 1 - 4 - RANKWIDTH - 1, cc.Oy);
    if (cc.Ox) {
	XDrawLine(dpy, d, blackgc, cc.Ox,              cc.y7+1, cc.Ox,              CARD_HEIGHT/2);
	XDrawLine(dpy, d, blackgc, CARD_WIDTH-1-cc.Ox, cc.y7+1, CARD_WIDTH-1-cc.Ox, CARD_HEIGHT/2);
    }
    {   /* load a card image */
	if (big.flag == 1) {
	    /* have it! */
	    x = (CARD_WIDTH-big.w) / 2;
	    y = CARD_HEIGHT/2 - big.h;
	    if (cc.flags & NOCENTERLINE)
		++y;
	    XSetClipMask(dpy, whitegc, big.clip);
	    XSetClipOrigin(dpy, whitegc, x-big.w*suit, y-big.h*(rank-Jack));
	    XCopyArea(dpy, big.map, d, whitegc, big.w*suit, big.h*(rank-Jack), big.w, big.h, x, y);
	    XSetClipMask(dpy, whitegc, None);
	} else {
	    char name[32];
	    sprintf(name, "%s.%s.pic", US_rank_name[rank], US_suit_name[suit]);
	    overlap_picture(name, 2, d);
	}
    }
    MirrorCard(d, CARD_WIDTH, CARD_HEIGHT);
}

static void read_images(void) {    
    static int done = 0;
    if (done)
	return;
    done = 1;

    if (read_xpmfile("Pictures", &big)) {
	big.w /= 4;
	big.h /= 3;
    }
    read_xpmfile("Background", &bg);
    if (!read_xpmfile("Suits", &p_suit)) {
	fprintf(stderr, "cannot read suits\n");
	exit(1);
    }
    if (!read_xpmfile("Ranks", &p_rank)) {
	fprintf(stderr, "cannot read ranks\n");
	exit(1);
    }
    XSetClipMask(dpy, suitgc, p_suit.clip);
    if (p_rank.clip) XSetClipMask(dpy, rankgc, p_rank.clip);
}


static void paint_large_card(Rank rank, Suit suit, Drawable d) {
    int x, y;
    /* make sure we have all pixmaps */

    if (rank > King) {	/* is a suit symbol for empty stack */
	x = (CARD_WIDTH-SUITSIZE0)/2;
	y = (CARD_HEIGHT-SUITSIZE0)/2;
	XSetClipOrigin(dpy, suitgc, x-suit*SUITSIZE0, y);
	XCopyArea(dpy, p_suit.map, d, suitgc, SUITSIZE0 * suit, 0,
		  SUITSIZE0, SUITSIZE0, x, y);
	return;
    }

    if (cc.Rx) {
	/* Draw the upper rank symbol(s). */
	x = 3 * (suit/2) * RANKWIDTH + (rank / 5) * RANKWIDTH;
	y = (rank % 5) * RANKHEIGHT;
	if (p_rank.clip) XSetClipOrigin(dpy, rankgc, cc.Rx-x, cc.Ry-y);
	XCopyArea(dpy, p_rank.map, d, rankgc, x, y,
		  RANKWIDTH, RANKHEIGHT, cc.Rx, cc.Ry);
	if (cc.flags & TWORANKS) {
	    if (p_rank.clip) XSetClipOrigin(dpy, rankgc, CARD_WIDTH-RANKWIDTH-cc.Rx-x, cc.Ry-y);
	    XCopyArea(dpy, p_rank.map, d, rankgc, x, y,
		      RANKWIDTH, RANKHEIGHT, CARD_WIDTH-RANKWIDTH-cc.Rx, cc.Ry);
	}
    }
    if (cc.Sx) {
	/* Draw the upper small suit symbol(s). */
	x = cc.Sx;
	y = cc.Sy;
	XSetClipOrigin(dpy, suitgc, x-suit*SUITSIZE3, y-SUITOFF3);
	XCopyArea(dpy, p_suit.map, d, suitgc, suit * SUITSIZE3, SUITOFF3,
		  SUITSIZE3, SUITSIZE3, x, y);
	if (cc.flags & TWORANKS) {
	    x = CARD_WIDTH-SUITSIZE3 - cc.Sx;
	    XSetClipOrigin(dpy, suitgc, x-suit*SUITSIZE3, y-SUITOFF3);
	    XCopyArea(dpy, p_suit.map, d, suitgc, suit * SUITSIZE3, SUITOFF3,
		      SUITSIZE3, SUITSIZE3, x, y);
	}
    }
    if (rank >= Jack)
	paint_large_2(rank, suit, d);
    else
	paint_large_1(rank, suit, d);
}

static Pixmap start_card(int c) {
    Pixmap d;
    d = XCreatePixmap(dpy, table, CARD_WIDTH, CARD_HEIGHT, DefaultDepth(dpy, screen));
    XFillRectangle(dpy, d, whitegc, 0, 0, CARD_WIDTH-1, CARD_HEIGHT-1);

    /* draw the background, if non-picture */
    if (bg.flag == 1 && ((cc.flags & PIC_BG) || !(RANK(c) >= Jack && RANK(c) <= King))) {
	int x, y;
	x = (CARD_WIDTH-bg.w)/2;
	y = (CARD_HEIGHT-bg.h)/2;
	if (bg.clip) {   
	    XSetClipMask(dpy, whitegc, bg.clip);
	    XSetClipOrigin(dpy, whitegc, x, y);
	    XCopyArea(dpy, bg.map, d, whitegc, 0, 0, bg.w, bg.h, x, y);
	    XSetClipMask(dpy, whitegc, None);
	} else
	    XCopyArea(dpy, bg.map, d, whitegc, 0, 0, bg.w, bg.h, x, y);
    }
    return d;
}

static void finish_card(int c, Pixmap d) {
    /* 6) A rounded rectangle is drawn as card border. */
#ifndef NO_ROUND_CARDS
	if (ROUND_W)
	    XmuDrawRoundedRectangle(dpy, d,
	        blackgc, 0, 0, CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	else
#endif
	    XDrawRectangle(dpy, d, blackgc, 0, 0, CARD_WIDTH-1, CARD_HEIGHT-1);
}


static void get_card(int c) {
    /* try to load card image from xpm file */
    char name[200];
    read_images();
    if (c >= 52) {
	if (c == CARDBACK) {
	    sprintf(name, "Cardback%d", 1 +
		    (int)(game.seed & 0xfff) % cc.numbacks);
	} else if (c >= JOKER) {
	    sprintf(name, "Joker%d", 1 + ((c - JOKER) + (int)((game.seed / cc.numbacks) & 0xfff))
		    % cc.numjokers);	/* use all different jokers, starting at rnd position */
	} else
	    *name = '\0';
    } else
	sprintf(name, "%s.%s", US_rank_name[RANK(c)],
		US_suit_name[SUIT(c)]);

    card_map[c] = start_card(c);
    if (*name && overlap_picture(name, 0, card_map[c])) {
	/* has map, just draw a rounded rectangle as border around it */
	finish_card(c, card_map[c]);
	return;
    }
    if (c >= 56) {
	fprintf(stderr, "cannot read %s\n", name);
	exit(1);
    }

    /* OK, we have to create it ourselves. */
    /* 2) a white pixmap of full cardsize is created. The width and height
          are taken from the Cards.conf file (1st line) and default to 79x123. */
    /* basic initialisation */
    paint_large_card(RANK(c), SUIT(c), card_map[c]);
    finish_card(c, card_map[c]);

#ifdef SAVE_IMAGES
    if (*name)
	write_xpmfile(name, card_map[c], 0);
#endif
}

void PaintCard(int x, int y, int c, int delta) {
    
#ifdef ALLATONCE
    static int loaded = 0;
    if (!loaded) {
	int i;
	loaded = 1;
	/* show_message(TXT_LOADING);
	XSync(dpy, 0); */
	/* read all the cards we need in any case. Do not read */
	/* Cardbacks or Jokers, they are possibly not needed */
	/* printf("before read_images: %ld\n", time(NULL)); */
	read_images();
	for (i = 0; i < 56; ++i) {
	    /* if (!(i&3)) printf("before card %d: %ld\n", i, time(NULL)); */
	    get_card(i);
	}
	/* printf("after cards are contructed: %ld\n", time(NULL)); */
	/* show_message(TXT_WELCOME, VERSION); */
	if (big.map)
	    free_picture(&big);
	free_picture(&p_rank);
	free_picture(&p_suit);
    }
#endif


    if (c == NOTHING)
	/* return; */
	goto outline;	/* this is better */
    /* JOKERs not yet fully implemented */
    if (IS_JOKER(c))
	c = JOKER + (c-JOKER) % MAXJOKERS;
    if (c != OUTLINE) {
	if (!card_map[c])
	    /* must make cardmap */
	    get_card(c);
	delta += ROUND_H;
	if (delta == ROUND_H || delta > CARD_HEIGHT)
	    delta = CARD_HEIGHT;
	XSetClipOrigin(dpy, cardbackgc, x, y);
	XCopyArea(dpy, card_map[c], table, cardbackgc, 0, 0, CARD_WIDTH, delta, x, y);
	return;
    }
 outline:
    /* if (c == OUTLINE) */
    /* to be sure the place is clear: */
    XClearArea(dpy, table, x, y, CARD_WIDTH, CARD_HEIGHT, False);
#ifndef NO_ROUND_CARDS
    if (ROUND_W)
	XmuDrawRoundedRectangle(dpy, table, blackgc, x, y,
	   CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
    else
#endif
	XDrawRectangle(dpy, table, blackgc, x, y, CARD_WIDTH-1, CARD_HEIGHT-1);
}
