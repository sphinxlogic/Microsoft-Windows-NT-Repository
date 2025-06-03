#ident	"@(#)xface.c 1.1 91/09/05"

/*
 *  UnCompface - 48x48x1 image decompression
 *
 *  Copyright (c) James Ashton - Sydney University - June 1990.
 *
 *  Written 11th November 1989.
 *
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose and without fee is hereby granted, provided
 *  that the above copyright notice appear in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation.  This software is provided "as is" without express or
 *  implied warranty.
 */

#include <stdio.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xface.h"

static char empty_bits[(WIDTH/8) * HEIGHT];
static int screen;

static GC xface_gc;
static Drawable xface_xid;
static Display *dpy;


/*
 * The following two routines are the hooks into the mail tool program.
 * They will need to be adjusted appropriately for each individual mail tool.
 *
 * mt_create_xface() will return an XView panel message item with a glyph
 * label. This is where the X-Face: image will be displayed. A handle to
 * that image is saved.
 *
 * mt_display_xface() passes in a buffer containing a copy of the processed
 * X-Face: header lines, or a NULL character in the first character position
 * if there is no X-Face: header for this mail message. If there is an
 * X-Face, then it is passed to uncompface, and the input buffer is over-
 * written with the data representation of the X-Face: image. An XImage is
 * created using this data, and the image ropped into the drawable associated
 * with the panel message item.
 */

Panel_item
mt_create_xface(panel, x, y)
	Panel			panel;
	int			x, y;
{
	unsigned long gc_mask;
	struct panel_item_data *p;
	Panel_item item;
	Server_image sv;
	XGCValues gc_val;

	(void) memset(empty_bits, 0, (WIDTH/8) * HEIGHT);
	p = (struct panel_item_data *)
		calloc(1, sizeof(struct panel_item_data));
	sv = xv_create(XV_NULL, SERVER_IMAGE,
		SERVER_IMAGE_DEPTH, 1,
		SERVER_IMAGE_X_BITS, empty_bits,
		XV_WIDTH, WIDTH,
		XV_HEIGHT, HEIGHT,
		0);
        item = xv_create(panel, PANEL_MESSAGE,
		XV_X, x,
		XV_Y, y,
		PANEL_CLIENT_DATA, p,
		PANEL_LABEL_IMAGE, sv,
		PANEL_LABEL_BOLD, TRUE,
		0);

	sv = (Server_image) xv_get(item, PANEL_LABEL_IMAGE);
	xface_xid = (Drawable) xv_get(sv, XV_XID);
	dpy = (Display *) xv_get(sv, XV_DISPLAY);
	screen = DefaultScreen(dpy);

	gc_mask = GCForeground | GCBackground | GCGraphicsExposures;
	gc_val.foreground = 1;
	gc_val.background = 0;
	gc_val.graphics_exposures = False;
	xface_gc = XCreateGC(dpy, xface_xid, gc_mask, &gc_val);
	return(item);
}


void
mt_display_xface(mt_info)
	char		*msg;
{
	int i;
	int no_xface = 1;
	XImage *image;

	if (*mt_info) {
		mt_info[strlen(mt_info)-1] = '\0';
		if (uncompface(mt_info) >= 0) no_xface = 0;
	}
	image = XCreateImage(dpy, DefaultVisual(dpy, screen),
			1, XYPixmap, 0,
			(no_xface) ? empty_bits : mt_info,
			WIDTH, HEIGHT, 8, 0);
	XPutImage(dpy, xface_xid, xface_gc, image,
		  0, 0, 0, 0, WIDTH, HEIGHT);
	XDestroyImage(image);
	(void) xv_set(mt_xface_item, XV_SHOW, TRUE, 0);
}


static int
uncompface(fbuf)
char *fbuf;
{
	if (!(status = setjmp(comp_env))) {
		UnCompAll(fbuf);
		UnGenFace();
		WriteFace(fbuf);
	}
	return(status);
}


static void
PopGreys(f, wid, hei)
char *f;
int wid, hei;
{
	if (wid > 3) {
		wid /= 2;
		hei /= 2;
		PopGreys(f, wid, hei);
		PopGreys(f + wid, wid, hei);
		PopGreys(f + WIDTH * hei, wid, hei);
		PopGreys(f + WIDTH * hei + wid, wid, hei);
	}
	else {
		wid = BigPop(freqs);
		if (wid & 1)
			*f = 1;
		if (wid & 2)
			*(f + 1) = 1;
		if (wid & 4)
			*(f + WIDTH) = 1;
		if (wid & 8)
			*(f + WIDTH + 1) = 1;
	}
}


static void
UnCompress(f, wid, hei, lev)
register char *f;
register int wid, hei, lev;
{
	switch (BigPop(&levels[lev][0])) {
		case WHITE :
			return;
		case BLACK :
			PopGreys(f, wid, hei);
			return;
		default :
			wid /= 2;
			hei /= 2;
			lev++;
			UnCompress(f, wid, hei, lev);
			UnCompress(f + wid, wid, hei, lev);
			UnCompress(f + hei * WIDTH, wid, hei, lev);
			UnCompress(f + wid + hei * WIDTH, wid, hei, lev);
			return;
	}
}


static void
UnCompAll(fbuf)
char *fbuf;
{
	register char *p;

	BigClear();
	BigRead(fbuf);
	p = F;
	while (p < F + PIXELS)
		*(p++) = 0;
	UnCompress(F, 16, 16, 0);
	UnCompress(F + 16, 16, 16, 0);
	UnCompress(F + 32, 16, 16, 0);
	UnCompress(F + WIDTH * 16, 16, 16, 0);
	UnCompress(F + WIDTH * 16 + 16, 16, 16, 0);
	UnCompress(F + WIDTH * 16 + 32, 16, 16, 0);
	UnCompress(F + WIDTH * 32, 16, 16, 0);
	UnCompress(F + WIDTH * 32 + 16, 16, 16, 0);
	UnCompress(F + WIDTH * 32 + 32, 16, 16, 0);
}


static void
Gen(f)
register char *f;
{
	register int m, l, k, j, i, h;

	for (j = 0; j < HEIGHT;  j++) {
		for (i = 0; i < WIDTH;  i++) {
			h = i + j * WIDTH;
			k = 0;
			for (l = i - 2; l <= i + 2; l++)
				for (m = j - 2; m <= j; m++) {
					if ((l >= i) && (m == j))
					    continue;
					if ((l > 0) && (l <= WIDTH) && (m > 0))
					    k = *(f+l+m*WIDTH) ? k*2+1 : k*2;
				}
			switch (i) {
				case 1 :
					switch (j) {
						case 1 : GEN(g_22);
						case 2 : GEN(g_21);
						default : GEN(g_20);
					}
					break;
				case 2 :
					switch (j) {
						case 1 : GEN(g_12);
						case 2 : GEN(g_11);
						default : GEN(g_10);
					}
					break;
				case WIDTH - 1 :
					switch (j) {
						case 1 : GEN(g_42);
						case 2 : GEN(g_41);
						default : GEN(g_40);
					}
					break;
				case WIDTH :
					switch (j) {
						case 1 : GEN(g_32);
						case 2 : GEN(g_31);
						default : GEN(g_30);
					}
					break;
				default :
					switch (j) {
						case 1 : GEN(g_02);
						case 2 : GEN(g_01);
						default : GEN(g_00);
					}
					break;
			}
		}
	}
}


static void
UnGenFace()
{
	Gen(F);
}


static void
BigRead(fbuf)
register char *fbuf;
{
	register int c;

	while (*fbuf != '\0') {
		c = *(fbuf++);
		if ((c < FIRSTPRINT) || (c > LASTPRINT))
			continue;
		BigMul(NUMPRINTS);
		BigAdd((WORD)(c - FIRSTPRINT));
	}
}


static void
WriteFace(fbuf)
char *fbuf;
{
	register char *s, *t;
	register int i, bits;

	s = F;
	t = fbuf;
	bits = i = 0;
	while (s < F + PIXELS) {
		if (*(s++))
			i = i * 2 + 1;
		else
			i *= 2;
		if (++bits == BITSPERBYTE) {
			*(t++) = i;
			bits = i = 0;
		}
	}
}


static int
BigPop(p)
register Prob *p; 
{
	static WORD tmp;
	register int i;

	BigDiv(0, &tmp);
	i = 0;
	while ((tmp < p->p_offset) || (tmp >= p->p_range + p->p_offset)) {
		p++;
		i++;
	}
	BigMul(p->p_range);
	BigAdd(tmp - p->p_offset);
	return i;
}


/*
 * Divide B by a storing the result in B and the remainder in the word
 * pointer to by r
 */

static void
BigDiv(a, r)
register WORD a, *r;
{
	register int i;
	register WORD *w;
	register COMP c, d;

	a &= WORDMASK;
	if ((a == 1) || (B.b_words == 0)) {
		*r = 0;
		return;
	}
	if (a == 0) {		/* treat this as a == WORDCARRY */
				/* and just shift everything right a WORD */
		i = --B.b_words;
		w = B.b_word;
		*r = *w;
		while (i--) {
			*w = *(w + 1);
			w++;
		}
		*w = 0;
		return;
	}
	w = B.b_word + (i = B.b_words);
	c = 0;
	while (i--) {
		c <<= BITSPERWORD;
		c += (COMP)*--w;
		d = c / (COMP)a;
		c = c % (COMP)a;
		*w = (WORD)(d & WORDMASK);
	}
	*r = c;
	if (B.b_word[B.b_words - 1] == 0)
		B.b_words--;
}


/*
 * Multiply a by B storing the result in B
 */

static void
BigMul(a)
register WORD a;
{
	register int i;
	register WORD *w;
	register COMP c;

	a &= WORDMASK;
	if ((a == 1) || (B.b_words == 0))
		return;
	if (a == 0) {		/* treat this as a == WORDCARRY */
				/* and just shift everything left a WORD */
		if ((i = B.b_words++) >= MAXWORDS - 1)
			longjmp(comp_env, ERR_INTERNAL);
		w = B.b_word + i;
		while (i--) {
			*w = *(w - 1);
			w--;
		}
		*w = 0;
		return;
	}
	i = B.b_words;
	w = B.b_word;
	c = 0;
	while (i--) {
		c += (COMP)*w * (COMP)a;
		*(w++) = (WORD)(c & WORDMASK);
		c >>= BITSPERWORD;
	}
	if (c) {
		if (B.b_words++ >= MAXWORDS)
			longjmp(comp_env, ERR_INTERNAL);
		*w = (COMP)(c & WORDMASK);
	}
}


/*
 * Add to a to B storing the result in B
 */

static void
BigAdd(a)
WORD a;
{
	register int i;
	register WORD *w;
	register COMP c;

	a &= WORDMASK;
	if (a == 0)
		return;
	i = 0;
	w = B.b_word;
	c = a;
	while ((i < B.b_words) && c) {
		c += (COMP)*w;
		*w++ = (WORD)(c & WORDMASK);
		c >>= BITSPERWORD;
		i++;
	}
	if ((i == B.b_words) && c) {
		if (B.b_words++ >= MAXWORDS)
			longjmp(comp_env, ERR_INTERNAL);
		*w = (COMP)(c & WORDMASK);
	}
}

static void
BigClear()
{
	B.b_words = 0;
}
