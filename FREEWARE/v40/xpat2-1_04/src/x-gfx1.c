/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-gfx1.c				     */
/*									     */
/*	card graphics for the X interface				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	based on the gfx.c module from Spider by Dave Lemke		     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

#ifndef VMS
#ifndef NO_XPM
#include <xpm.h>
#endif
#ifndef NO_ROUND_CARDS
#include <X11/Xmu/Drawing.h>
#endif
#else
#ifndef NO_XPM
#include "xpm.h"
#endif
#ifndef NO_ROUND_CARDS
#include <Xmu/Drawing.h>
#endif
#endif /* VMS */

#ifdef SAVE_IMAGES
#ifdef NO_XPM
#error "Cannot define SAVE_IMAGES with NO_XPM"
#endif
static void write_pixmap(const char *filename, Pixmap pict, Pixmap clip) {
    if (XpmWriteFileFromPixmap(dpy, filename, pict, clip, NULL) != XpmSuccess)
        fprintf(stderr, "error writing xpm file %s\n", filename);
}
#endif

static int read_pixmap(const char *filename, Pixmap *pict) {
#ifndef NO_XPM
    if (XpmReadFileToPixmap(dpy, table, filename, pict, NULL, NULL) == XpmSuccess) {
	/* add nice outline which may vary (-cround option) */
#ifndef NO_ROUND_CARDS
	if (ROUND_W)
	    XmuDrawRoundedRectangle(dpy, *pict,
	        blackgc, 0, 0, CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	else
#endif
	    XDrawRectangle(dpy, *pict, blackgc,
			   0, 0, CARD_WIDTH-1, CARD_HEIGHT-1);
	return 1;
    }
#endif
    *pict = 0;
    return 0;
}



/* card INFO: taken from spider */
/* these are for the large cards */
/* all small card info is variable */

#define FCARD_WIDTH	79
#define FCARD_HEIGHT	123

/* this is the size of the bounding rectangle of the pictures for internal cards */
#define	FACECARD_WIDTH	48
#define	FACECARD_HEIGHT	92

#define	RANK_WIDTH	9
#define	RANK_HEIGHT	14

#define	RANK_LOC_X	4
#define	RANK_LOC_Y	7

#define	SMALL_LOC_X	4
#define	SMALL_LOC_Y	(RANK_HEIGHT + RANK_LOC_Y + 3)

#define	MID_CARD_X	(CARD_WIDTH/2)
#define	MID_CARD_Y	(CARD_HEIGHT/2)

#define	CARD_COL1_X	(3 * CARD_WIDTH/10)
#define	CARD_COL2_X	(CARD_WIDTH/2)
#define	CARD_COL3_X	(7 * CARD_WIDTH/10)

/* 5 diff rows for the two main columns */
/* 1 and 5 are top and bottom, 3 is the middle */
/* 2 & 4 are for the 10 & 9 */
#define	CARD_ROW1_Y	(CARD_HEIGHT/5)
#define	CARD_ROW2_Y	(2 * CARD_HEIGHT/5)
#define	CARD_ROW3_Y	(CARD_HEIGHT/2)
#define	CARD_ROW4_Y	(CARD_HEIGHT - 2 * CARD_HEIGHT/5)
#define	CARD_ROW5_Y	(CARD_HEIGHT - CARD_HEIGHT/5)

/* between 1 & 3, 3 & 5 */
#define	CARD_SEVEN_Y	(7 * CARD_HEIGHT/20)
#define	CARD_EIGHT_Y	(CARD_HEIGHT - 7 * CARD_HEIGHT/20)

/* between rows 1 & 2, 4 & 5 */
#define	CARD_TEN_Y1	(3 * CARD_HEIGHT/10)
#define	CARD_TEN_Y2	(CARD_HEIGHT - 3 * CARD_HEIGHT/10)

/* pip info */
#define	PIP_WIDTH	10
#define	PIP_HEIGHT	10



#define	NUM_RANKS	13
#define	NUM_SUITS	4
#define	CARDS_PER_DECK	(NUM_RANKS * NUM_SUITS)


static GC cardgc;		/* gc in use when drawing cards */
static GC backgc;		/* gc in use when drawing cardbacks */
static GC redgc;		/* gc for red parts of the cards */

/* default cards: */
#include "rank.bm"	/* rank symbol bitmaps 			*/
#include "face.bm"	/* bitmaps for jack, queen and king 	*/
#include "suit.bm"	/* suit bitmaps 			*/
#include "logo.bm"	/* bitmap used for cardbacks 		*/

static struct suit_tab {
    char *bits;
    int w;
    int h;
} suit_tab[] = {
    { club_bits, club_width, club_height },
    { spade_bits, spade_width, spade_height },
    { heart_bits, heart_width, heart_height },
    { diamond_bits, diamond_width, diamond_height }
}, suit_sm_tab[] = {
    { club_sm_bits, club_sm_width, club_sm_height },
    { spade_sm_bits, spade_sm_width, spade_sm_height },
    { heart_sm_bits, heart_sm_width, heart_sm_height },
    { diamond_sm_bits, diamond_sm_width, diamond_sm_height }
}, suit_lg_tab[] = {
    { NULL, club_width, club_height },
    { spade_lg_bits, spade_lg_width, spade_lg_height },
    { NULL, heart_width, heart_height },
    { NULL, diamond_width, diamond_height }
};

static Pixmap rank_map[NUM_RANKS],     rank_r_map[NUM_RANKS];
static Pixmap rank_map_red[NUM_RANKS], rank_r_map_red[NUM_RANKS];
static Pixmap suit_map[NUM_SUITS],     suit_r_map[NUM_SUITS];
static Pixmap suit_sm_map[NUM_SUITS],  suit_sm_r_map[NUM_SUITS];
static Pixmap suit_lg_map[NUM_SUITS];
static Pixmap jack_map[NUM_SUITS], queen_map[NUM_SUITS], king_map[NUM_SUITS];
static Pixmap joker_map;

#include "Joker.bm"

#ifdef STATIC_CLIPMAPS
#include "clip7.bm"
#include "clip4.bm"
#endif

static GC cardbackgc;
#ifndef NO_ROUND_CARDS
static Pixmap cardclipmap;
#endif

static int mem_option;
static const char *xpmdir;
static Pixmap cardpicts[58];

/* clipping rectangles */
static XRectangle cliprects[1] = {{ 0, 0, 0, 0 }};
static int card_is_clipped;	/* optimizer for card drawing */

#include "gray1.bm"

static void make_GCs(unsigned long red, unsigned long cbcolor) {
    XGCValues gcv;
    long gcflags;
    Pixmap tmpmap;
    GC logogc;
    Pixmap logomap;

    gcv.graphics_exposures = False;
    /* make GC for RED */
    if (graphic.is_color)	{
	gcv.foreground = red;
	gcv.background = WhitePixel(dpy, screen);
	gcflags = GCForeground | GCBackground | GCGraphicsExposures;
	redgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    } else	{
	gcv.tile = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
		 gray1_bits, gray1_width, gray1_height);
	gcv.fill_style = FillTiled;
	gcv.foreground = BlackPixel(dpy, screen);
	gcv.background = WhitePixel(dpy, screen);
	
	gcflags = GCTile | GCForeground | GCBackground |
	    GCFillStyle | GCGraphicsExposures;
	redgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
    }

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
    
    if (graphic.is_color)	{
	gcv.foreground = cbcolor;
	gcv.background = WhitePixel(dpy, screen);
	gcflags = GCForeground | GCBackground;
	logogc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
	XCopyPlane(dpy, tmpmap, logomap, logogc, 0, 0, 
		   logo_width, logo_height, 0, 0, 1);
	XFreeGC(dpy, logogc);
    } else	{
	XCopyPlane(dpy, tmpmap, logomap, whitegc, 0, 0, 
		   logo_width, logo_height, 0, 0, 1);
    }
    XFreePixmap(dpy, tmpmap);
    
    gcv.tile = logomap;
    gcv.fill_style = FillTiled;
    gcflags |= GCTile | GCFillStyle | GCGraphicsExposures;
    
    backgc = XCreateGC(dpy, RootWindow(dpy, screen), gcflags, &gcv);
}

#define S(x,y) src[(H-1-(y))*W+(x)]
#define D(x,y) dst[(H-1-(y))*W+(x)]

/* table for left-right mirroring of bitmaps */
static char _reverse_byte[0x100] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};



static void copy_upside_down(char *src, char *dst, int W, int H)
{   int x, y;
    
    W = (W + 7)/8;              /* round up to bytes */
    for (y = 0; y < H; y++)	{
	for (x = 0; x < W; x++)	{
	    D (x, y) = S (x, H - 1 - y);
	}
    }
}

/* copy a bitmap rotated from source to dest */
static void rot_180(char *src, char *dst, int W, int H)
{   int x, y;
    int width = W;
    char *new;
    int bit;
    
    W = (W + 7)/8;              /* round up to bytes */
    for (y = 0; y < H; y++) {
	for (x = 0; x < W; x++) {
	    D (x, y) = _reverse_byte[(unsigned char)(S (W - 1 - x, H - 1 - y))];
	}
    }
    
    /* shift it over (bit correction) */
    new = calloc((size_t)W*H, 1);
    for (y = 0; y < H; y++)	{
	for (x = 0; x < W*8; x++)	{
	    bit = (*(dst + (x + (W*8 - width))/8 + y * W)
		   & (1 << ((x + (W*8 - width)) % 8))) ? 1 : 0;
	    *(new + x/8 + y*W) = (bit << (x%8)) | 
		(*(new + x/8 + y*W) & ~(1 << (x%8)));
	}
    }
    memcpy(dst, new, W*H);
    free(new);
}




/* large cards: */
/*
 * make a 'red' pixmap by setting the clipmask to the desired shape and 
 * pushing 'red' through it
 */

static Pixmap make_color_map(char *bits, int width, int height, Suit suit)
{   Pixmap	tmpmap, newmap;
    static GC	cleargc = (GC) 0;
    XGCValues	xgcv;
    
    if (suit == Spade || suit == Club || graphic.is_color)
	return XCreateBitmapFromData(dpy, RootWindow(dpy, screen), bits, width, height);

    /* get a graphic context, if not yet gotten */
    if (cleargc == (GC) 0) {
	xgcv.function = GXclear;
	cleargc = XCreateGC(dpy, RootWindow(dpy, screen), GCFunction, &xgcv);
    }
    tmpmap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
        bits, width, height);        /* turn data into bitmap */
    
    /* allocate a pixmap */
    newmap = XCreatePixmap(dpy, RootWindow(dpy, screen), width, height, 1);
    
    /* clear red_map to white */
    XFillRectangle(dpy, newmap, cleargc, 0, 0, width, height);
    
    /* use bitmap tmpmap as clipmap for red GC */
    XSetClipMask(dpy, redgc, tmpmap);
    /* color red_map to red using the mask */
    XFillRectangle(dpy, newmap, redgc, 0, 0, width, height);
    XSetClipMask(dpy, redgc, None);    	/* clear clipping mask in the red GC */
    XFreePixmap(dpy, tmpmap);        	/* release bitmap */
  
    return (newmap);
}


#ifdef LOADCARDS
static Pixmap card_bitmaps[68];	/* these hold pixmaps of full depth */
                                /* we have room for 8 jokers */
static int numcards;		/* when reading external cardsets */
static int inset;

/* read a complete set of card bitmaps from a file */
static void read_bitmap(const char *file)
{   FILE *fp;
    int i, w, h, bpl, cardsize;
    char *card_bits;
    char fullname[200];
    struct {			 /* Note: all hi...-fields are currently 0 */
	char type;               /* 0 for cards without and 1 for cards with an frame 	*/
	char hitype;
	char num;                /* # of cards (without jokers) in file 		*/
	char hinum;
	char x;                  /* width of one card 					*/
	char hix;
	char y;                  /* height of one card on screen (size in file can differ) */
	char hiy;
	char rx;                 /* horizontal half axe of rounded corner 		*/
	char hirx;
	char ry;                 /* vertical half axe of rounded corner 		*/
	char hiry;
	char stddelta;           /* fraction of height for displaying 			*/
	char histddelta;
	char numjokers;          /* # of different joker cards 			*/
	char hinumjokers;
	char numcardbacks;       /* # of different cardbacks 				*/
	char hinumcardbacks;
	char fill[14];           /* reserved for future requirements 			*/
    } hdr;

    if (!strchr(file, '/') &&
	strlen(file) + strlen(LIBDIR) + 8 <= sizeof(fullname)) {
	sprintf(fullname, "%s/%s.cards", LIBDIR, file);
	file = fullname;
    }
    fp = fopen(file, "r");
    if (!fp || fread(&hdr, sizeof(hdr), 1, fp) != 1) {
	fprintf(stderr, "cannot read file %s\n", file);
	exit(EXIT_FAILURE);
    }
    /* check the number of cards in the header */
    if (hdr.num < 52 || hdr.num > 57) {
	fprintf(stderr, "Bad value of numcards in file %s\n", file);
	exit(EXIT_FAILURE);
    }
    w = hdr.x; h = hdr.y; numcards = hdr.num;
    CARD_WIDTH = w;
    CARD_HEIGHT = h;
    ROUND_W = hdr.rx;           /* default radii of rounded corners */
    ROUND_H = hdr.ry;
    STD_DELTA = hdr.stddelta;
    bpl = (w + 7) >> 3;         /* bytes per line */
    if (!(card_bits = malloc(bpl * h * numcards))) {
	fprintf(stderr, "Not enough memory for external cardset (file %s)\n",
		file);
	exit(EXIT_FAILURE);
    }

    /* read the card bitmaps */
    fread(card_bits, bpl * h * numcards, 1, fp);
    fclose(fp);

    /* hdr.type & 1 is the number of pixels omitted in the bitmap */
    cardsize = (h - 2 * (inset = (hdr.type & 1))) * bpl;

    /* turn data into bitmaps */
    for (i = 0; i < numcards; i++)	{
	card_bitmaps[i] = XCreateBitmapFromData(dpy, 
		RootWindow(dpy, screen), card_bits+i*cardsize, w-2*inset, h-2*inset);
    }
}
#endif

/* build jack to king bitmaps of the internal cardset by mirroring the first half */
static char *compose_picture(char *tmp, const char *orgbits)
{   int cardsize = 6 * 45; /* omit one scanline */
    memcpy(tmp, orgbits, cardsize);
    rot_180(tmp, tmp+cardsize+6, 47, 45);
    memset(tmp + cardsize, 0xff, 6);            /* add a horizontal line in the middle */
    return tmp;
}

static void build_internal_cardset(void)
{   char new_bits[120/8 * 200];	/* sufficient for the largest bitmap here */
    Rank r;
    int i;

    ROUND_W = ROUND_H = 9;
    STD_DELTA = 25;
    
    /* create bitmaps for rank symbols normal and rotated */
    for (r = Ace; r <= King; r++)	{
	rank_map[(int)r] = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
		 rank_bits[(int)r], rank_width, rank_height);
	rot_180(rank_bits[(int)r], new_bits, rank_width, rank_height);
	rank_r_map[(int)r] = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
		   new_bits, rank_width, rank_height);
	if (graphic.is_color)	{
	    rank_map_red[(int)r] = rank_map[(int)r];
	    rank_r_map_red[(int)r] = rank_r_map[(int)r];
	} else {
	    rank_map_red[(int)r] = make_color_map(
			rank_bits[(int)r], rank_width, rank_height, Heart);
	    rank_r_map_red[(int)r] = make_color_map(
			  new_bits, rank_width, rank_height, Heart);
	}
    }

    for (i = 0; i < 4; ++i) {
	struct suit_tab *bp;
	char *king_bits[] = { king_c_bits, king_s_bits, king_h_bits, king_d_bits };
	char *queen_bits[] = { queen_c_bits, queen_s_bits, queen_h_bits, queen_d_bits };
	char *jack_bits[] = { jack_c_bits, jack_s_bits, jack_h_bits, jack_d_bits };

	bp = suit_tab + i;
	copy_upside_down(bp->bits, new_bits, bp->w, bp->h);
	suit_map[i] = make_color_map(bp->bits, bp->w, bp->h, (Suit)i);
	suit_r_map[i] = make_color_map(new_bits, bp->w, bp->h, (Suit)i);

	bp = suit_sm_tab + i;
	copy_upside_down(bp->bits, new_bits, bp->w, bp->h);
	suit_sm_map[i] = make_color_map(bp->bits, bp->w, bp->h, (Suit)i);
	suit_sm_r_map[i] = make_color_map(new_bits, bp->w, bp->h, (Suit)i);
	bp = suit_lg_tab + i;
	suit_lg_map[i] = bp->bits ? make_color_map(bp->bits, bp->w, bp->h, (Suit)i) :
	    suit_map[i];

	jack_map[i] = make_color_map(compose_picture(new_bits, jack_bits[i]),
				     47, 91, (Suit)i);
	queen_map[i] = make_color_map(compose_picture(new_bits, queen_bits[i]),
				     47, 91, (Suit)i);
	king_map[i] = make_color_map(compose_picture(new_bits, king_bits[i]),
				     47, 91, (Suit)i);
    }
    joker_map = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
        Joker_bits, Joker_width, Joker_height);     /* turn data into bitmap */
}

void init_cards(const char *cardset, int rx, int ry,
    unsigned long red, unsigned long cbcolor, int mem, const char *xpm) {
#ifdef NO_ROUND_CARDS
    rx = ry = 0;
#endif
    mem_option = mem;
    if (xpm && *xpm != '.' && *xpm != '/') {
	char *s;
	s = malloc(strlen(LIBDIR) + strlen(xpm) + 2);
	sprintf(s, "%s/%s", LIBDIR, xpm);
	xpmdir = s;
    } else
	xpmdir = xpm;	/* directory where to load from the Xpm-files */

    card.cardset = cardset;
    make_GCs(red, cbcolor);
    if (cardset) {	/* read external cardset */
#ifdef LOADCARDS
	read_bitmap(cardset)
#endif
	    ;
    } else {
        /* build internal cardset */
	CARD_WIDTH = -1;
	if (xpm) {
	    char s[200];
	    FILE *fp;
	    sprintf(s, "%s/Cards.conf", xpmdir);
	    if ((fp = fopen(s, "r"))) {
		if (fscanf(fp, "%d %d", &CARD_WIDTH, &CARD_HEIGHT) != 2)
		    CARD_WIDTH = -1;
		fclose(fp);
	    }
	}	
	if (CARD_WIDTH < 49 || CARD_WIDTH > 120
           || CARD_HEIGHT < 95 || CARD_HEIGHT > 200) {
	    CARD_WIDTH = FCARD_WIDTH;
	    CARD_HEIGHT = FCARD_HEIGHT;
	}
	build_internal_cardset();
    }
    
    cliprects[0].width = CARD_WIDTH + 1;
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
    
    /* adapt arrows to match cardset sizes */
    graphic.aw = graphic.ah = STD_DELTA / 6;
    graphic.ya_h = graphic.xa_w = graphic.aw * 2;
    graphic.xa_h = graphic.ya_w = graphic.aw * 4;

    card.back_delta_x = (CARD_WIDTH - logo_width)/2;
    card.back_delta_y = (CARD_HEIGHT - logo_height)/2;

    /* finally, compute a clipping mask for the cards */
    /* apply this clip mask to cardbackgc */
#ifndef NO_ROUND_CARDS
#ifndef STATIC_CLIPMAPS
    if (ROUND_W) {
	cardclipmap = XCreatePixmap(dpy, RootWindow(dpy, screen),
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
	     clipgc = XCreateGC(dpy, cardclipmap, gcflags, &gcv);
	     XFillRectangle(dpy, cardclipmap, clipgc, 0, 0, 
			    CARD_WIDTH, CARD_HEIGHT);
	     XFreeGC(dpy, clipgc);
	     
	     /* make gc for clipgc */
	     gcv.foreground = f;
	     gcv.background = b;
	     gcv.graphics_exposures = False;
	     gcflags = GCForeground | GCBackground | GCGraphicsExposures;
	     clipgc = XCreateGC(dpy, cardclipmap, gcflags, &gcv);
	     XmuFillRoundedRectangle(dpy, cardclipmap, clipgc, 0, 0, 
		     CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	     XmuDrawRoundedRectangle(dpy, cardclipmap, clipgc, 0, 0, 
		     CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	     XFreeGC(dpy, clipgc);
	 }
#ifdef DEBUG
	XWriteBitmapFile(dpy, "clip.mask.bm", cardclipmap, CARD_WIDTH, CARD_HEIGHT, -1, -1);
#endif
	/* fill the background */
	XSetClipMask(dpy, cardbackgc, cardclipmap);
    }
#else
    switch (ROUND_H) {
    case 3:
    case 4:
    case 5:
	cardclipmap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
	    clip4_bits, clip4_width, clip4_height);
	XSetClipMask(dpy, cardbackgc, cardclipmap);
	break;
    case 6:
    case 7:
    case 8:
    case 9:
	cardclipmap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
            clip7_bits, clip7_width, clip7_height);
	XSetClipMask(dpy, cardbackgc, cardclipmap);
	break;
    }
#endif
#endif
}



static void paint_cardback(int x, int y, int delta, Drawable d)
{
        if (!delta)
            delta = CARD_HEIGHT;

#define	INSETa	1
#define INSETb  1
	/* change the origin so cards will have the same back anywhere
	 * on the table
	 */
	/*
	 * there should be a tile centered in the card, with the
	 * surrounding tiles being partial
	 */
	XSetClipMask(dpy, backgc, None);
	XSetClipMask(dpy, blackgc, None);

#ifndef NO_ROUND_CARDS
	if (ROUND_W) {

	    XmuFillRoundedRectangle(dpy, d, blackgc, x, y, CARD_WIDTH-1, 
		(delta == CARD_HEIGHT) ? delta-1 : delta + ROUND_H * 2,
		ROUND_W, ROUND_H);
	} else
#endif
	    XFillRectangle(dpy, d, blackgc, x, y, CARD_WIDTH,
		(delta == CARD_HEIGHT) ? delta : delta);

	XSetTSOrigin(dpy, backgc, x + card.back_delta_x, y + card.back_delta_y);

#ifndef NO_ROUND_CARDS
	if (ROUND_W)
	    XmuFillRoundedRectangle(dpy, d, backgc, x + INSETa, y + INSETa, 
		CARD_WIDTH - INSETa - INSETb - 1,
		(delta == CARD_HEIGHT) ? 
			(CARD_HEIGHT - INSETa - INSETb - 1) : delta + ROUND_H * 2,
		ROUND_W, ROUND_H);
	else
#endif
	    XFillRectangle(dpy, d, backgc, x + INSETa, y + INSETa, 
		CARD_WIDTH - INSETa - INSETb - 1,
		(delta == CARD_HEIGHT) ? 
			(CARD_HEIGHT - INSETa - INSETb - 1) : delta-1);
	/* correction */
#ifndef NO_ROUND_CARDS
	if (ROUND_W) {
	    XmuDrawRoundedRectangle(dpy, d, blackgc, x, y, CARD_WIDTH-1, 
		(delta == CARD_HEIGHT) ? delta-1 : delta + ROUND_H * 2,
		ROUND_W, ROUND_H);
	    XmuDrawRoundedRectangle(dpy, d, blackgc, x+1, y+1, CARD_WIDTH-3, 
		(delta == CARD_HEIGHT) ? delta-3 : delta + ROUND_H * 2,
		ROUND_W-1, ROUND_H-1);
	} else
#endif
	    XDrawRectangle(dpy, d, blackgc, x+1, y+1, CARD_WIDTH-2,
		(delta == CARD_HEIGHT) ? delta-2 : delta);

}


/*
 * NOTE -- for all the pip drawers except the one that actually plots the
 * bits, the location is the card's location.  the drawer's take the
 * pip's center as location.
 */

/*
 * draws right-side-up pip
 *
 * location is for center of pip
 */

static void draw_pip(Suit suit, int x, int y, Drawable d)
{
    XCopyPlane(dpy, suit_map[suit], d, cardgc, 0, 0, 
        suit_tab[suit].w, suit_tab[suit].h, x - 15/2, y - 19/2, 1);
}

/*
 * draws upside-down pip
 *
 * location is for center of pip
 */
static void draw_did(Suit suit, int x, int y, Drawable d)
{
    if (card_is_clipped)	/* a clipped card never shows any did's */
	return;
    XCopyPlane(dpy, suit_map[suit], d, cardgc, 0, 0, 
        suit_tab[suit].w, suit_tab[suit].h, x - 15/2, y - 19/2, 1);
}

/*
 * draws big center pip
 */
static void draw_center_pip(Suit suit, int x, int y, Drawable d)
{   int w, h;
    if (card_is_clipped)	/* a clipped card never shows any big's */
	return;
    w = suit_lg_tab[suit].w;
    h = suit_lg_tab[suit].h;
    XCopyPlane(dpy, suit_lg_map[suit], d, cardgc, 0, 0, w, h,
	x - w/2, y - h/2, 1);
}

/* 
 * draw_two_pips
 */
static void draw_two_pips(Suit suit, int x, int y, Drawable d)
{
	draw_pip(suit, x + MID_CARD_X, y + CARD_ROW1_Y, d);
	draw_did(suit, x + MID_CARD_X, y + CARD_ROW5_Y, d);
}

/*
 * draw_four_pips
 */
static void draw_four_pips(Suit suit, int x, int y, Drawable d)
{
	draw_pip(suit, x + CARD_COL1_X, y + CARD_ROW1_Y, d);
	draw_did(suit, x + CARD_COL1_X, y + CARD_ROW5_Y, d);

	draw_pip(suit, x + CARD_COL3_X, y + CARD_ROW1_Y, d);
	draw_did(suit, x + CARD_COL3_X, y + CARD_ROW5_Y, d);
}

static void draw_six_pips(Suit suit, int x, int y, Drawable d)
{
	draw_pip(suit, x + CARD_COL1_X, y + CARD_ROW1_Y, d);
	draw_pip(suit, x + CARD_COL3_X, y + CARD_ROW1_Y, d);

	if (card_is_clipped)
		return;

	/* these are only visible when its not clipped */
	draw_pip(suit, x + CARD_COL1_X, y + CARD_ROW3_Y, d);
	draw_did(suit, x + CARD_COL1_X, y + CARD_ROW5_Y, d);

	draw_pip(suit, x + CARD_COL3_X, y + CARD_ROW3_Y, d);
	draw_did(suit, x + CARD_COL3_X, y + CARD_ROW5_Y, d);
}

static void draw_eight_pips(Suit suit, int x, int y, Drawable d)
{
	draw_pip(suit, x + CARD_COL1_X, y + CARD_ROW1_Y, d);
	draw_pip(suit, x + CARD_COL3_X, y + CARD_ROW1_Y, d);

	if (card_is_clipped)
		return;

	/* these are only visible when its not clipped */
	draw_pip(suit, x + CARD_COL1_X, y + CARD_ROW2_Y, d);
	draw_did(suit, x + CARD_COL1_X, y + CARD_ROW4_Y, d);
	draw_did(suit, x + CARD_COL1_X, y + CARD_ROW5_Y, d);

	draw_pip(suit, x + CARD_COL3_X, y + CARD_ROW2_Y, d);
	draw_did(suit, x + CARD_COL3_X, y + CARD_ROW4_Y, d);
	draw_did(suit, x + CARD_COL3_X, y + CARD_ROW5_Y, d);
}

static void draw_jack(Suit suit, int x, int y, Drawable d)
{
	XCopyPlane(dpy, jack_map[suit], d, cardgc, 
		0, 0, FACECARD_WIDTH-1, FACECARD_HEIGHT-1,
		x + 1 + (CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + 1 + (CARD_HEIGHT - FACECARD_HEIGHT)/2, 1);

	XDrawRectangle(dpy, d, cardgc,
		x + (CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + (CARD_HEIGHT - FACECARD_HEIGHT)/2,
		FACECARD_WIDTH, FACECARD_HEIGHT);
}

static void draw_queen(Suit suit, int x, int y, Drawable d)
{
	XCopyPlane(dpy, queen_map[suit], d, cardgc,
		0, 0, FACECARD_WIDTH-1, FACECARD_HEIGHT-1,
		x + 1+(CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + 1+(CARD_HEIGHT - FACECARD_HEIGHT)/2, 1);

	XDrawRectangle(dpy, d, cardgc,
		x + (CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + (CARD_HEIGHT - FACECARD_HEIGHT)/2,
		FACECARD_WIDTH, FACECARD_HEIGHT);
}

static void draw_king(Suit suit, int x, int y, Drawable d)
{
	XCopyPlane(dpy, king_map[suit], d, cardgc,
		0, 0, FACECARD_WIDTH-1, FACECARD_HEIGHT-1,
		x + 1+(CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + 1+(CARD_HEIGHT - FACECARD_HEIGHT)/2, 1);

	XDrawRectangle(dpy, d, cardgc,
		x + (CARD_WIDTH - FACECARD_WIDTH)/2, 
		y + (CARD_HEIGHT - FACECARD_HEIGHT)/2,
		FACECARD_WIDTH, FACECARD_HEIGHT);
}

static void draw_rank(int x, int y, Rank rank, Suit suit, Drawable d)
{

	if (suit == Heart || suit == Diamond)	{
		XCopyPlane(dpy, rank_map_red[rank], d, cardgc,
			0, 0, RANK_WIDTH, RANK_HEIGHT,
			x + RANK_LOC_X, y + RANK_LOC_Y, 1);

		if (!card_is_clipped)
		    XCopyPlane(dpy, rank_r_map_red[rank], d, cardgc,
			0, 0, RANK_WIDTH, RANK_HEIGHT,
			x + (CARD_WIDTH - RANK_WIDTH - RANK_LOC_X), 
			y + (CARD_HEIGHT - RANK_HEIGHT - RANK_LOC_Y), 1);
	} else	{
		XCopyPlane(dpy, rank_map[rank], d, cardgc,
			0, 0, RANK_WIDTH, RANK_HEIGHT,
			x + RANK_LOC_X, y + RANK_LOC_Y, 1);

		if (!card_is_clipped)
		    XCopyPlane(dpy, rank_r_map[rank], d, cardgc,
			0, 0, RANK_WIDTH, RANK_HEIGHT,
			x + (CARD_WIDTH - RANK_WIDTH - RANK_LOC_X), 
			y + (CARD_HEIGHT - RANK_HEIGHT - RANK_LOC_Y), 1);
	}

    {	int w, h, o;
	w = suit_sm_tab[suit].w;
	h = suit_sm_tab[suit].h;
	o = (suit == Diamond);
	XCopyPlane(dpy, suit_sm_map[suit], d, cardgc, 0, 0, w, h,
	    x + o + SMALL_LOC_X, y + SMALL_LOC_Y, 1);

	if (!card_is_clipped)
	    XCopyPlane(dpy, suit_sm_r_map[suit], d, cardgc, 0, 0, w, h,
		x - o + (CARD_WIDTH - w - SMALL_LOC_X),
		y + (CARD_HEIGHT - h - SMALL_LOC_Y), 1);
    }
}



static void paint_joker(int x, int y, Drawable d)
{
#ifndef NO_ROUND_CARDS
    if (ROUND_W) {
	XSetClipMask(dpy, blackgc, cardclipmap);
	XSetClipOrigin(dpy, blackgc, x, y);
    }
#endif

    XCopyPlane(dpy, joker_map, d, blackgc, 0, 0, CARD_WIDTH, CARD_HEIGHT,
	       x, y, 1);

#ifndef NO_ROUND_CARDS
    if (ROUND_W)
	XSetClipMask(dpy, blackgc, None);

    if (ROUND_W)
	XmuDrawRoundedRectangle(dpy, d, blackgc, x, y, 
		    CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
    else
#endif
	/* draw border on card */
	XDrawRectangle(dpy, d, blackgc, x, y, CARD_WIDTH-1, CARD_HEIGHT-1);
}
/*
 * delta is 0 if the card is fully showing
 */
static void paint_large_card(int x, int y, int rank, Suit suit, int delta, Drawable d)
{
	if (suit == Spade || suit == Club)	{
		cardgc = blackgc;
	} else	{
		cardgc = redgc;
	}

	if (delta) {
		cliprects[0].height = delta + ROUND_H;
		XSetClipRectangles(dpy, cardgc, x, y, cliprects, 1, Unsorted);
#ifndef NO_ROUND_CARDS
		if (ROUND_W)	{
		    XSetClipRectangles(dpy, whitegc, x, y, cliprects, 1, 
						Unsorted);
		    if (cardgc != blackgc)
			XSetClipRectangles(dpy, blackgc, x, y, cliprects, 1,
						Unsorted);

		    /* add in ROUND_H to height so only the top is rounded */
		    /* fill the background */
		    XmuFillRoundedRectangle(dpy, d, whitegc, x, y, 
			CARD_WIDTH-1, delta + ROUND_H, ROUND_W, ROUND_H);
		    /* draw border on card */
		    XmuDrawRoundedRectangle(dpy, d, blackgc, x, y, 
			CARD_WIDTH-1, delta + ROUND_H, ROUND_W, ROUND_H);
		} else	
#endif
		{
		    /* fill the background */
		    XFillRectangle(dpy, d, whitegc, x, y, 
					CARD_WIDTH-1, delta);
		    /* draw border on card */
		    XDrawRectangle(dpy, d, blackgc, x, y, 
					CARD_WIDTH-1, delta);
		}
		card_is_clipped = True;
	} else	{	/* fill all the card */
#ifndef NO_ROUND_CARDS
		if (ROUND_W)	{
		    /* fill the background */
		    XmuFillRoundedRectangle(dpy, d, whitegc, x, y, 
			CARD_WIDTH-1, CARD_HEIGHT-1,
			ROUND_W, ROUND_H);
		    /* draw border on card */
		    XmuDrawRoundedRectangle(dpy, d, blackgc, x, y, 
			CARD_WIDTH-1, CARD_HEIGHT-1,
			ROUND_W, ROUND_H);
		} else
#endif
		{
		    /* fill the background */
		    XFillRectangle(dpy, d, whitegc, x, y, 
			CARD_WIDTH-1, CARD_HEIGHT-1);
		    /* draw border on card */
		    XDrawRectangle(dpy, d, blackgc, x, y, 
			CARD_WIDTH-1, CARD_HEIGHT-1);
		}
		card_is_clipped = False;
	}

	switch (rank)	{
	case	King:
		draw_king(suit, x, y, d);
		break;
	case	Queen:
		draw_queen(suit, x, y, d);
		break;
	case	Jack:
		draw_jack(suit, x, y, d);
		break;

	case	Ten:
		draw_pip(suit, MID_CARD_X + x, CARD_TEN_Y1 + y, d);
		draw_did(suit, MID_CARD_X + x, CARD_TEN_Y2 + y, d);
		draw_eight_pips(suit, x, y, d);
		break;

	case	Nine:
		draw_pip(suit, x + MID_CARD_X, y + MID_CARD_Y, d);
		draw_eight_pips(suit, x, y, d);
		break;

	case	Eight:
		draw_did(suit, x + MID_CARD_X, y + CARD_EIGHT_Y, d);
		/* fall thru */
	case	Seven:
		draw_pip(suit, MID_CARD_X + x, CARD_SEVEN_Y + y, d);
		/* fall thru */
	case	Six:
		draw_six_pips(suit, x, y, d);
		break;

	case	Five:
		draw_pip(suit, x + MID_CARD_X, y + MID_CARD_Y, d);
		/* fall thru */
	case	Four:
		draw_four_pips(suit, x, y, d);
		break;

	case	Three:
		draw_pip(suit, x + MID_CARD_X, y + MID_CARD_Y, d);
		/* fall thru */
	case	Deuce:
		draw_two_pips(suit, x, y, d);
		break;
        case    13:
	case	Ace:
		draw_center_pip(suit, x + MID_CARD_X, y + MID_CARD_Y, d);
		break;
	default:
		assert(0);
	}

        if (rank != 13)
 	   draw_rank(x, y, rank, suit, d);

	/* clear the clip mask */
	XSetClipMask(dpy, cardgc, None);

#ifndef NO_ROUND_CARDS
	if (ROUND_W) {
	    if (!delta && !x &&!y) {
		/* special: if round card and initial call, repaint rounding */
		/* this is for extremely rounded card which get over-painted */
		XmuDrawRoundedRectangle(dpy, d, blackgc, x, y, 
		    CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	    }
	    XSetClipMask(dpy, whitegc, None);
	    if (cardgc != blackgc)
		XSetClipMask(dpy, blackgc, None);
	}
#endif
}

#ifdef LOADCARDS
static void paint_external_card(int x, int y, int cardnumber, int delta) {
    if (cardnumber == CARDBACK)
	cardgc = backgc;
    else if (SUIT(cardnumber) == Spade || SUIT(cardnumber) == Club) {
	cardgc = graphic.is_color ? blackgc: whitegc;
    } else {
	cardgc = redgc;
    }
    if (delta && !ROUND_W) {
	cliprects[0].height = delta;
	XSetClipRectangles(dpy, cardgc, x, y, cliprects, 1, Unsorted);
	if (inset)
	    XSetClipRectangles(dpy, blackgc, x, y, cliprects, 1, Unsorted);
    }

#ifndef NO_ROUND_CARDS
    if (ROUND_W) {
	XSetClipMask(dpy, cardgc, cardclipmap);
	XSetClipOrigin(dpy, cardgc, x, y);
	if (inset) {
	    XSetClipMask(dpy, blackgc, cardclipmap);
	    XSetClipOrigin(dpy, blackgc, x, y);
	}
    }
#endif

    XCopyPlane(dpy, card_bitmaps[cardnumber], table, cardgc, 0, 0, 
	CARD_WIDTH-2*inset, CARD_HEIGHT-2*inset, x+inset, y+inset, 1);

    if (inset) {	/* must draw border */
#ifndef NO_ROUND_CARDS
        if (ROUND_W)
  	    XmuDrawRoundedRectangle(dpy, table, blackgc, x, y, 
		CARD_WIDTH-1, CARD_HEIGHT-1, ROUND_W, ROUND_H);
	else
#endif
	    XDrawRectangle(dpy, table, blackgc, x, y, CARD_WIDTH-1, CARD_HEIGHT-1);
    }

    if (delta || ROUND_W) {
	XSetClipMask(dpy, cardgc, None);
	if (inset)
	    XSetClipMask(dpy, blackgc, None);
    }
}
#endif


void PaintCard(int x, int y, int c, int delta) {
    static Suit Suittab[4] = { Club, Spade, Heart, Diamond };
    static int initial_call = 1;   /* initial-call-flag */


    /* JOKERs not yet fully implemented */
    if (c == NOTHING)
	return;
    
    if (c != OUTLINE) {
#ifdef LOADCARDS
	if (card.cardset) {	/* externally loaded cards: */
	    if (IS_JOKER(c))
		c = SUIT(c) + SUITSYMBOL;	/* do this instead */
	    if (c < numcards) {
		paint_external_card(x, y, c, delta);
		return;
	    } else if (c == CARDBACK) {
		paint_cardback(x, y, delta, table);
		return;
	    } else {	/* must be suit symbol */
		c = OUTLINE;	/* do outline as replacement */
		/* and fall thru */
	    }
	} else
#endif
	{			/* do default cards */
	    if (!mem_option) {
		if (IS_JOKER(c))
		    paint_joker(x, y, table);	/* do this instead */
		else if (c != CARDBACK)
		    paint_large_card(x, y, RANK(c), Suittab[SUIT(c)], delta, table);
		else {
		    if (!delta)
			delta = CARD_HEIGHT;
		    paint_cardback(x, y, delta, table);
		}
		return;
	    } else {
		if (initial_call) {
		    int i;
		    initial_call = 0;
		    cardpicts[57] = 0;	/* Joker is loaded on request */
		    for (i = 0; i <= 56; ++i) {
			if (xpmdir && (i < 52 || i >= 56)) {
			    char s[200];
			    if (i == CARDBACK) {
				strcpy(s, xpmdir);
			        strcat(s, "/Cardback.xpm");
			    } else 
				sprintf(s, "%s/%s.%s.xpm", xpmdir,
				   US_rank_name[RANK(i)], US_suit_name[SUIT(i)]);
			    if (read_pixmap(s, &cardpicts[i]))
				continue;	/* load succeeded */
			}
			/* no file found, must create internal card */
			cardpicts[i] = XCreatePixmap(dpy, table,
		 	     CARD_WIDTH, CARD_HEIGHT, DefaultDepth(dpy, screen));
			if (i == CARDBACK)
			    paint_cardback(0, 0, CARD_HEIGHT, cardpicts[i]);
			else
			    paint_large_card(0, 0, RANK(i), Suittab[SUIT(i)], 0, cardpicts[i]);
		    }
#ifdef SAVE_IMAGES
		    for (i = 0; i < 52; ++i) {
			char s[20];
			sprintf(s, "%s.%s.xpm",
				US_rank_name[RANK(i)], US_suit_name[SUIT(i)]);
			write_pixmap(s, cardpicts[i], cardclipmap);
		    }
#endif
		}
		if (IS_JOKER(c)) {
		    c = 57;
		    if (!cardpicts[c]) {
			/* must load or draw joker */
			if (xpmdir) {
			    char s[200];
			    sprintf(s, "%s/Joker.xpm", xpmdir);
			    (void)read_pixmap(s, &cardpicts[c]);
			}
			if (!cardpicts[c]) {
			    cardpicts[c] = XCreatePixmap(dpy, table,
		 	       CARD_WIDTH, CARD_HEIGHT, DefaultDepth(dpy, screen));
			    paint_joker(0, 0, cardpicts[c]);
			}
		    }
		}
		delta += ROUND_H;
		if (delta == ROUND_H || delta > CARD_HEIGHT)
		    delta = CARD_HEIGHT;
		XSetClipOrigin(dpy, cardbackgc, x, y);
		XCopyArea(dpy, cardpicts[c], table, cardbackgc, 0, 0, CARD_WIDTH, delta, x, y);
		return;
	    }
	}
    }

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
