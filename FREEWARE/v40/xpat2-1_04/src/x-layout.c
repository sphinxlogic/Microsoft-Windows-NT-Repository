/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module X-layout.c			     */
/*									     */
/*	Layout function for the X interface				     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#include "X-pat.h"

/* layout types for Spider, Gypsy are 0..7 */
#define FREECELL_MAGIC		0x220
#define DELIGHT_MAGIC		0x260
#define MONTECARLO_MAGIC	0x320
#define MIDNIGHTOIL_MAGIC	0x370
#define MICHAELS_MAGIC		0x3c0
#define ROYAL_MAGIC		0x3e0
#define BAKERS_MAGIC		0x400
#define CANFIELD_MAGIC		0x420

#define MCDX 14
#define MCDY 4

/* how far may slots at the bottom of the window be extended? */
/* I think it sufffices if 1/3 of the card is visible */
#define BOTTOM_INC	(graphic.ygap + 2*CARD_HEIGHT/3)

void new_rules_notify(void) {
    XSize_t w, h;
#ifdef useXview
    xv_set(mainwindow,
	   FRAME_LABEL, (rules.longname? rules.longname: rules.shortname),
	   0);
#else
    XStoreName(dpy, mainwindow, rules.longname ? rules.longname : rules.shortname);
#endif
    refresh_screen();		/* sometimes needed (at least if area is smaller) */
    
    /* compute minimum size */
    generic_minwindow(&w, &h);
    graphic.min_width = w;
    graphic.min_height = h;
#ifdef LABER
    printf("min size set to %d, %d\n", w, h);
#endif
#ifdef useXlib
    {   XSizeHints xsh;
	xsh.flags = PMinSize;
	xsh.min_width = w;
	xsh.min_height = h;
	XSetWMNormalHints(dpy, table, &xsh);
    }
    w = max(graphic.width, w);
    h = max(graphic.height, h);
#else
    w = max(w, graphic.w_width);
    h = max(h, graphic.w_height);
#endif
    /* w and h are the maximum of the current visible size and the minimum size */
    if (w != graphic.width || h != graphic.height)
	soft_Resize(w, h);		/* request a resize if greater */
    init_layout();			/* new setup */
}

void cmd_Layout(void) {
    int i;
    init_layout();
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i)
	if (pile_resize(i))
	    draw_pileupdate(i, 0);
}


#ifndef useXlib

#define calc_yoff(w)	graphic.ygap
#define YOFF		graphic.ygap

#else	/* Xlib must do some work */

#define YOFF		graphic.yoff

int arrange_buttons(struct singlebutton *b, int *startx, int *starty,
    XSize_t maxx, long *magic) {
    int x, y, num;
    long mask;
    /* printf("arrange width %d\n", maxx); */
    x = *startx;
    y = *starty;
    num = 0;
    *magic = 0;
    mask = 1L;
    while (b->text) {		/* weiterer button vorhanden */
	++num;
	b->x = x;
	b->y = y;
	b->w = 2 * button.bx + XTextWidth(button.font, b->text, strlen(b->text));
	b->h = 2 * button.by + button.fontheight;

	while (x > graphic.xgap && x + b->w + graphic.xgap >= maxx) {
	    /* line is full, next line */
	    *magic |= mask;
	    x = *startx;
	    y += 2 * button.by + button.fontheight + graphic.ygap;
	    b->x = x;
	    b->y = y;
	}
	mask <<= 1;
	x += b->w + graphic.xgap;
	++b;
    }
    *startx = x;
    *starty = y + 2 * button.by + button.fontheight + graphic.ygap;
    return num;
}

static int calc_yoff(int w) {
    long dummy;
    int x, y;
    x = graphic.xgap;
    y = graphic.ygap;
    button.num = arrange_buttons(button.b, &x, &y, w, &dummy);
    graphic.buttons_height = y;
    graphic.message_x = 10;
    graphic.message_y = y + TABLE_BW + graphic.message_font->ascent;
    graphic.message_height = 2 * TABLE_BW +
	graphic.message_font->ascent + graphic.message_font->descent;
    return graphic.buttons_height + graphic.message_height + graphic.ygap;
}
#endif



/* there are 4 types of piles: VDECK, tmps, slots, stacks */
/* the IDECK is of no interest for this function */
#define SLOTFACTOR 2	/* minimum std height of a slot, measured in units
			   (CARD_HEIGHT+graphic.ygap) */

void generic_minwindow(XSize_t *x, XSize_t *y) {	
    int min1;

    switch (rules.layout_hints) {
    case 0:
	if (!rules.numtmps) {
	   XSize_t x1, y1, x2, y2;
	
	   if (rules.numstacks > rules.numslots)
	       min1 = rules.numstacks;
	   else if (rules.numstacks == rules.numslots)
	       min1 = rules.numstacks + (rules.variant & DECK_SOURCE ? 1 : 0);
	   else  /* stacks < slots */ if (rules.variant & DECK_SOURCE)
	       min1 = max(rules.numstacks+2, rules.numslots);
	   else
	       min1 = rules.numslots;
	   
	   x1 = graphic.xgap * (min1+1) + CARD_WIDTH * min1;
	   y1 = calc_yoff(x1) + (CARD_HEIGHT + graphic.ygap) * (SLOTFACTOR+1);
	   
	   min1 = rules.numslots + rules.numdecks;
	   if (rules.variant & DECK_SOURCE)
	       ++min1;
	   x2 = graphic.xgap * (min1+1) + CARD_WIDTH * min1;
	   y2 = calc_yoff(x2) + (CARD_HEIGHT + graphic.ygap) * 4;
	   
	   /* as a selection rule, minimize the size of the window needed */
	   if (x1 * y1 < x2 * y2) {	/* stacks on the top */
	       *x = x1;
	       *y = y1;
	   } else {	/* stacks on the right side */
	       *x = x2;
	       *y = y2;
	   }
	   return;
	}
	/* else fall thru to SeaHaven type */
    case 1:
	min1 = max(rules.numslots, rules.numstacks + rules.numtmps);
	*x = graphic.xgap * (min1+1) + CARD_WIDTH * min1;
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * (SLOTFACTOR+1);
	return;
    case 2:
	/* The deck, the stack, the slots */
	min1 = rules.numslots + 1;
	*x = graphic.xgap * (min1+1) + CARD_WIDTH * min1;
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * (SLOTFACTOR+1);
	return;
    case 3:	/* Monte Carlo */
	/* The deck, no stack, the slots */
	min1 = 1+rules.param[1];
	*x = graphic.xgap - MCDX + min1 * (CARD_WIDTH + graphic.xgap + MCDX);
	*y = calc_yoff(*x) + (CARD_HEIGHT + MCDY + graphic.ygap) * 
	    ((rules.numslots+rules.param[1]-1)/rules.param[1]);
	return;
    case 4:	/* Midnight Oil: 18 Slots */
	/* The deck, no stack, the slots */
	min1 = (rules.numslots/2) + rules.numdecks;
	*x = graphic.xgap * (min1+1) + CARD_WIDTH * min1;
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * 4;
	return;
    case 5:	/* Michael's Fantasy */
	/* No deck, 4 stacks, 4 Tmps, the slots */
	*x = 3 * graphic.xgap + (3 + rules.numslots) 
	    * (CARD_WIDTH + graphic.xgap);
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * 3;
	return;
    case 6:	/* Royal Cotillion */
	/* No deck, 4 stacks, 4 Tmps, the slots */
	min1 = max(2, rules.numslots) + rules.numdecks + (rules.numtmps+3)/4;
	*x = 3 * graphic.xgap + min1 * (CARD_WIDTH + graphic.xgap);
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * 4;
	return;
    case 7:	/* Baker's Dozen */
	/* No deck, 4 stacks, a dozen slots */
	min1 = rules.numdecks + (rules.numslots+1)/2;
	*x = graphic.xgap + min1 * (CARD_WIDTH + graphic.xgap);
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * 4;
	return;
    case 8:	/* Canfield */
	/* No deck, 4 stacks, a dozen slots */
	*x = graphic.xgap + 5 * (CARD_WIDTH + graphic.xgap);
	*y = calc_yoff(*x) + (CARD_HEIGHT + graphic.ygap) * 4;
	return;
    }
}

static int FreeCell_layout(int xnum, int ynum, int h) {
    int i, cols, off;
    struct pile *p = graphic.pile;

    cols = max(rules.numslots, rules.numstacks + rules.numtmps);

    off = cols - rules.numstacks;
    for (i = 0; i < rules.numstacks; ++i) {
	p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (i+off);
	p->y = YOFF;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = CARD_HEIGHT;
	++p;
    }
    off = cols - rules.numslots;
    for (i = 0; i < rules.numslots; ++i) {
	p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (i+off);
	p->y = YOFF + graphic.ygap + CARD_HEIGHT;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = h - YOFF - CARD_HEIGHT - graphic.ygap + BOTTOM_INC;
	++p;
    }
    off = 0;
    for (i = 0; i < rules.numtmps; ++i) {
	p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (i+off);
	p->y = YOFF;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = CARD_HEIGHT;
	++p;
    }
    p->x = p->y = -1;
    return FREECELL_MAGIC;
}


static int gen_layout(int xnum, int ynum, int h) {
    int i, cols, off, rows;
    struct pile *p = graphic.pile;
    int new_layout_type;

    /* try to do big layout */
    graphic.preferred_height = YOFF;	/* will be increased */
    cols = rules.numslots + rules.numdecks;
    if (rules.variant & DECK_SOURCE)
	++cols;

    if (xnum >= cols && ynum >= 4) {
	/* big layout is OK */
	new_layout_type = 0;
	cols -= rules.numdecks;
	rows = 4;
	if (xnum >= rules.numslots + rules.numdecks + 1) {
	    /* big layout with deck OK */
	    cols = rules.numslots + 1;
	    ++new_layout_type;
	}
	for (i = 0; i < rules.numstacks; ++i) {
	    p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (cols + i/4);
	    p->y = YOFF + SUIT(i) * (CARD_HEIGHT+graphic.ygap);
	    p->xtotal = CARD_WIDTH;
	    p->ymaxheight = CARD_HEIGHT;
	    ++p;
	}
	off = cols - rules.numslots;
	for (i = 0; i < rules.numslots; ++i) {
	    p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (off+i);
	    p->y = YOFF;
	    p->xtotal = CARD_WIDTH;
	    p->ymaxheight = h - YOFF + BOTTOM_INC;
	    ++p;
	}
	cols += rules.numdecks;
    } else {
	/* small layout */
	rows = 1 + SLOTFACTOR;
	if (rules.numstacks > rules.numslots) {
	    new_layout_type = 2;
	    cols = rules.numstacks;
	} else if (rules.numstacks == rules.numslots) {
	    if (rules.variant & DECK_SOURCE) {
		new_layout_type = 5;
		cols = rules.numstacks + 1;
	    } else {
		new_layout_type = 4;	/* only case without a deck */
		cols = rules.numstacks;
		if (xnum >= cols + 1) {	/* have a deck anyway */
		    ++cols;
		    ++new_layout_type;	
		}
	    }
	} else {
	    new_layout_type = 6;
	    if (rules.variant & DECK_SOURCE)
		cols = max(rules.numstacks + 2, rules.numslots);
	    else
		cols = rules.numslots;
	}

	off = cols - rules.numstacks;
	for (i = 0; i < rules.numstacks; ++i) {
	    p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (i+off);
	    p->y = YOFF;
	    p->xtotal = CARD_WIDTH;
	    p->ymaxheight = CARD_HEIGHT;
	    ++p;
	}
	off = cols - rules.numslots;
	for (i = 0; i < rules.numslots; ++i) {
	    p->x = graphic.xgap + (graphic.xgap + CARD_WIDTH) * (i+off);
	    p->y = YOFF + graphic.ygap + CARD_HEIGHT;
	    p->xtotal = CARD_WIDTH;
	    p->ymaxheight = h - YOFF - CARD_HEIGHT - graphic.ygap + BOTTOM_INC;
	    ++p;
	}
    }
    /* fine. now do the deck(s) (if any) */
    /* p is pointer to first deck pile */
    /* cols is the number of columns on the tableau */
    graphic.preferred_width = (cols+1) * graphic.xgap + cols * CARD_WIDTH;
    graphic.preferred_height += rows * (graphic.ygap + CARD_HEIGHT);
#ifdef LABER
    printf("gen_layout setting up: %d rows, %d cols, preferred size = (%d,%d)\n",
	   rows, cols, graphic.preferred_width, graphic.preferred_height);
#endif
    p->xtotal = CARD_WIDTH;
    p->ymaxheight = CARD_HEIGHT;
    if (rules.variant & NODEAL) {
	p->x = p->y = -1;
    } else {
	switch (new_layout_type) {
	case 2: /* case 3: */
	    p->x = graphic.xgap;
	    p->y = YOFF + graphic.ygap + CARD_HEIGHT;
	    break;
	case 0:	case 4:
	    p->x = p->y = -1;	/* no deck */
	    break;
	case 1: case 5:
	    p->x = graphic.xgap;
	    p->y = YOFF + (graphic.ygap + CARD_HEIGHT) / 2;
	    break;
	case 6: /* case 7: */
	    p->x = graphic.xgap;
	    p->y = YOFF;
	    break;
	default:
	    abort();
	}
    }
    /* do a second deck? */
    if (rules.variant & DECK_SOURCE) {
	p[1] = p[0];	/* IDECK is one more back */
	if (new_layout_type == 6) {
	    p->y = YOFF;
	    p->x = 2 * graphic.xgap + CARD_WIDTH;
	} else {
	    p->x = graphic.xgap;
	    p->y = YOFF + ((rules.variant & NODEAL) ? 1 : 3)
		* (graphic.ygap + CARD_HEIGHT) / 2;
	}
    }
    return new_layout_type;
}

static int Delight_Layout(int xnum, int ynum, int h) {
    int i, x = 0;
    struct pile *p = graphic.pile;

    for (i = 0; i < game.numpiles; ++i) {
	p = graphic.pile+i;
	p->x = -1;
	p->y = -1;
    }
    p = graphic.pile+IDECK;
    p->x = (x += graphic.xgap);
    p->y = YOFF;
    p->xtotal = CARD_WIDTH;
    p->ymaxheight = CARD_HEIGHT;

    /* want dest. for hint arrows */
    p = graphic.pile+FIRST_STACK;
    p->x = x;
    p->y = YOFF + CARD_HEIGHT + graphic.ygap;
    p->xtotal = CARD_WIDTH;
    p->ymaxheight = CARD_HEIGHT;

    p = graphic.pile+FIRST_SLOT;
    for (i = 0; i < rules.numslots; ++i) {
	x += CARD_WIDTH + graphic.xgap;
	p->x = x;
	p->y = YOFF;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = h - YOFF + BOTTOM_INC;
	++p;
    }
    return DELIGHT_MAGIC;
}

static int MonteCarlo_Layout(int xnum, int ynum, int h) {
    int i, y, x;
    struct pile *p = graphic.pile;

    for (i = 0; i < game.numpiles; ++i) {
	p = graphic.pile+i;
	p->x = -1;
	p->y = -1;
    }
    y = YOFF;
    p = graphic.pile+IDECK;
    p->x = graphic.xgap;
    p->y = y;
    p->xtotal = CARD_WIDTH;
    p->ymaxheight = CARD_HEIGHT;

    p = graphic.pile+FIRST_SLOT;
    x = CARD_WIDTH + 2 * graphic.xgap;
    for (i = 0; i < rules.numslots; ++i) {
	p->x = x + (i%5) * (CARD_WIDTH + graphic.xgap + MCDX);
	p->y = y + (i/5) * (CARD_HEIGHT + graphic.ygap + MCDY);
	p->xtotal = CARD_WIDTH;
	p->xdelta = MCDX;
	p->delta = MCDY;
	p->ymaxheight = CARD_HEIGHT + MCDY;
	p->xmaxwidth = CARD_WIDTH + MCDX;
	++p;
    }
    return MONTECARLO_MAGIC;
}

static int MidnightOil_Layout(int xnum, int ynum, int h) {
    int i, y, x;
    struct pile *p = graphic.pile;

    for (i = 0; i < game.numpiles; ++i) {
	p = graphic.pile+i;
	p->x = -1;
	p->y = -1;
    }


    y = YOFF;
    x = graphic.xgap;


    p = graphic.pile+FIRST_SLOT;
    for (i = 0; i < rules.numslots; ++i) {
	p->x = x;
	p->y = y;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = 2 * CARD_HEIGHT + graphic.ygap;
	if (i & 1) {
	    p->y += p->ymaxheight + graphic.ygap;
	    x += CARD_WIDTH + graphic.xgap;
	}
	++p;
    }
    if (rules.numslots & 1)
	/* correctly advance x position */
	x += graphic.xgap + CARD_WIDTH;

    p = graphic.pile+FIRST_STACK;
    for (i = 0; i < rules.numstacks; ++i) {
	p->x = x;
	p->y = y + (i & 3) * (graphic.ygap + CARD_HEIGHT);
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = CARD_HEIGHT;
	if ((i&3) == 3)
	    x += CARD_WIDTH + graphic.xgap;
	++p;
    }
    return MIDNIGHTOIL_MAGIC;
}

static int Michaels_Layout(int xnum, int ynum, int h) {
    int i, j, y, x = graphic.xgap;
    struct pile *p = graphic.pile;

    for (i = 0; i < game.numpiles; ++i) {
	p = graphic.pile+i;
	p->x = -1;
	p->y = -1;
    }
    y = YOFF;
    /* first the slots */
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	if (i == FIRST_SLOT+rules.numslots/2)
	    x += 5 * graphic.xgap + 3 * CARD_WIDTH;
	p = graphic.pile+i;
	p->x = x;
	p->y = y;
	x += graphic.xgap + CARD_WIDTH;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = h - YOFF + BOTTOM_INC;
    }
    /* now the tmps */
    x = 2 * graphic.xgap + rules.numslots/2 * (CARD_WIDTH + graphic.xgap);
    for (j = 0; j < rules.numtmps; ++j) {
	p = graphic.pile+i+j;
	p->x = x + ((j & 2) ? 2 * (graphic.xgap + CARD_WIDTH) : 0);
	p->y = y + ((j & 1) ? 2 * (graphic.ygap + CARD_HEIGHT) : 0);
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = CARD_HEIGHT;
    }
    /* at last the stacks */
    for (i = FIRST_STACK; i <= LAST_STACK; ++i) {
	static int xoff[] = { 0, 1, 1, 2 };
	static int yoff[] = { 2, 1, 3, 2 };
	p = graphic.pile+i;
	p->x = x + xoff[i] * (CARD_WIDTH + graphic.xgap);
	p->y = y + (yoff[i] * (CARD_HEIGHT + graphic.ygap)) / 2;
	p->xtotal = CARD_WIDTH;
	p->ymaxheight = CARD_HEIGHT;
    }
    return MICHAELS_MAGIC;
}

static int Royal_Layout(int xnum, int ynum, int h) {
    int i, x = graphic.xgap;
    struct pile *p = graphic.pile;

    if (rules.numslots > 2)
	x += (rules.numslots - 2) * (CARD_WIDTH+graphic.xgap) / 2;
    p = graphic.pile + IDECK;
    p->x = x;
    p->y = YOFF;
    --p;	/* VDECK */
    p->x = x + graphic.xgap + CARD_WIDTH;
    p->y = YOFF;
    x = graphic.xgap;
    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	p = graphic.pile+i;
	p->x = x;
	p->y = YOFF + CARD_HEIGHT + graphic.ygap;
	x += graphic.xgap + CARD_WIDTH;
	p->ymaxheight = 3 * CARD_HEIGHT + 2 * graphic.ygap;
    }
    x = 2 * graphic.xgap + max(rules.numslots, 2) * (CARD_WIDTH + graphic.xgap);
    for (i = FIRST_STACK; i <= LAST_STACK; ++i) {
	p = graphic.pile+i;
	p->x = x + (i / 4) * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF + (i & 3) * (CARD_HEIGHT + graphic.ygap);
    }
    x += rules.numdecks * (graphic.xgap + CARD_WIDTH) + graphic.xgap;
    for (i = 0; i < rules.numtmps; ++i) {
	p = graphic.pile+LAST_SLOT+1+i;
	p->x = x + (i / 4) * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF + (i & 3) * (CARD_HEIGHT + graphic.ygap);
    }
    return ROYAL_MAGIC;
}

static int Bakers_Layout(int xnum, int ynum, int h) {
    int i, x;
    struct pile *p;

    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	p = graphic.pile+i;
	p->x = graphic.xgap + (i - FIRST_SLOT) / 2 * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF + ((i - FIRST_SLOT) % 2) * 2 * (CARD_HEIGHT + graphic.ygap);
	p->ymaxheight = 2 * CARD_HEIGHT + graphic.ygap;
    }

    x = graphic.xgap + (rules.numslots+1)/2 * (CARD_WIDTH + graphic.xgap);
    for (i = FIRST_STACK; i <= LAST_STACK; ++i) {
	p = graphic.pile+i;
	p->x = x + (i / 4) * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF + (i & 3) * (CARD_HEIGHT + graphic.ygap);
    }
    return BAKERS_MAGIC;
}

static int Canfield_Layout(int xnum, int ynum, int h) {
    int i;
    struct pile *p;

    for (i = FIRST_SLOT; i <= LAST_SLOT; ++i) {
	p = graphic.pile+i;
	p->x = graphic.xgap + (i - FIRST_SLOT+1) * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF + (CARD_HEIGHT + graphic.ygap);
	p->ymaxheight = 3 * CARD_HEIGHT + graphic.ygap;
    }
    for (i = FIRST_STACK; i <= LAST_STACK; ++i) {
	p = graphic.pile+i;
	p->x = graphic.xgap + (i+1) * (CARD_WIDTH + graphic.xgap);
	p->y = YOFF;
    }
    p = graphic.pile + LAST_SLOT + 1;
    /* the stock */
    p->x = graphic.xgap;
    p->y = YOFF + (CARD_HEIGHT + graphic.ygap)/2;
    
    ++p;
    /* the talon */
    p->x = graphic.xgap;
    p->y = p[-1].y + (CARD_HEIGHT + graphic.ygap);

    ++p;
    /* the hand */
    p->x = graphic.xgap;
    p->y = p[-1].y + (CARD_HEIGHT + graphic.ygap);
    return CANFIELD_MAGIC;
}

void init_layout(void) {
    int w, h;
    static int layout_type = -1, yoff = -1;
    static long magic = -1;
    int new_layout_type;
    long new_magic = 0;
    int xnum, ynum;	/* number of cards in either direction */

    /* get the size of the actual window */
#ifdef useXaw
    w = graphic.w_width;
    h = graphic.w_height;
#else
    w = graphic.width;
    h = graphic.height;
#endif
#ifdef LABER
    printf("init_layout requested for size (%d,%d)\n", w, h);
#endif

#ifdef useXlib
    {   int x, y;
	x = graphic.xgap;
	y = graphic.ygap;
	/* y is used space in vertical direction */
	/* get new_magic! */
	button.num = arrange_buttons(button.b, &x, &y, w, &new_magic);
	/* sorry, work is done twice */
	graphic.yoff = calc_yoff(w);
    }
#endif

    xnum = (w - graphic.xgap) / (CARD_WIDTH+graphic.xgap);
    ynum = (h - YOFF) / (CARD_HEIGHT+graphic.ygap);

    /* the default is that there is only one layout per rule set,
       which is minsize and preferred size at the same time.
       So we set it here. It can be overridden by the special
       layout functions */
    graphic.preferred_width = graphic.min_width;
    graphic.preferred_height = graphic.min_height;

    /* assign defaults */
    {   Pileindex i;
	struct pile *p;
	for (i = 0; i < game.numpiles; ++i) {
	    p = graphic.pile+i;
	    p->x = -1;
	    p->y = -1;
	    p->xtotal = CARD_WIDTH;
	    p->ytotal = CARD_HEIGHT;
	    p->xmaxwidth  = CARD_WIDTH;
	    p->ymaxheight = CARD_HEIGHT;
	    p->xdelta  = 0;
	}
    }
    switch (rules.layout_hints) {
    case 0:
    case 1:
	new_layout_type = rules.numtmps ?
	    FreeCell_layout(xnum, ynum, h) :
		gen_layout(xnum, ynum, h);
	break;
    case 2:
	new_layout_type = Delight_Layout(xnum, ynum, h);
	break;
    case 3:
	new_layout_type = MonteCarlo_Layout(xnum, ynum, h);
	break;
    case 4:
	new_layout_type = MidnightOil_Layout(xnum, ynum, h);
	break;
    case 5:
	new_layout_type = Michaels_Layout(xnum, ynum, h);
	break;
    case 6:
	new_layout_type = Royal_Layout(xnum, ynum, h);
	break;
    case 7:
	new_layout_type = Bakers_Layout(xnum, ynum, h);
	break;
    case 8:
	new_layout_type = Canfield_Layout(xnum, ynum, h);
	break;
    default:
	new_layout_type = 0xffff;	/* keep compiler happy */
	assert(0);
    }
    /* printf("deck %d position: (%d,%d), first slot at (%d,%d)\n",
	p - graphic.pile, p->x, p->y,
	graphic.pile[rules.numstacks].x, graphic.pile[rules.numstacks].y);
    printf("cols = %d, off = %d\n", cols, off);
	printf("xxdeck at pos %d,%d\n", graphic.pile[DECK].x, graphic.pile[DECK].y); */
    if (new_layout_type != layout_type || YOFF != yoff) {	/* everything changed */
	layout_type = new_layout_type;
	yoff = YOFF;
	magic = new_magic;
	refresh_screen();
    } else if (magic != new_magic) {		/* at least one button did move */
	magic = new_magic;
	XClearArea(dpy, table, 0, 0, 0, YOFF-2, True);	/* just redraw the buttons */
    }
}

int pile_resize(int i) {
    int j, delta, xdelta;
    struct pile *p;

    p = graphic.pile + i;
    p->xtotal = CARD_WIDTH;
    p->ytotal = CARD_HEIGHT;
    if (game.piletype[i] == Slot)
	delta = STD_DELTA;
    else
	delta = 0;
    xdelta = 0;
    if (game.piletype[i] != Slot || EMPTY(i)) {
	/* done */
	/* p->xdelta = xdelta; */
	p->delta = delta;
	return 0;
    }

    if (CARDS_ON_PILE(i) > 1)
	delta = (p->ymaxheight-CARD_HEIGHT) / (CARDS_ON_PILE(i)-1);

    if (delta > STD_DELTA)          /* respect maximum value */
	delta = STD_DELTA;
    if (delta < 1)                  /* respect minimum value */
        delta = 1;

    /* printf("delta = %d, %d cards on pile %d\n", delta, CARDS_ON_PILE(i), i); */
    j = INDEX_OF_FIRST_CARD(i);
    graphic.cardy[j] = 0;
    while (j != INDEX_OF_LAST_CARD(i)) {	/* not topmost card */
	p->ytotal += delta;
	graphic.cardy[j+1] = graphic.cardy[j] + delta;
	++j;
    }
    j = graphic.pile[i].delta != delta;	/* change of shrink factor? */
    graphic.pile[i].delta = delta;
    /* graphic.pile[i].xdelta = xdelta; */	/* unchanged for now */
    return j;
}
