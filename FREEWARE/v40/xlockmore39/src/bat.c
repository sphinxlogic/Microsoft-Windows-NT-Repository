
#ifndef lint
static char sccsid[] = "@(#)bat.c	3.9 96/05/25 xlockmore";

#endif

/*-
 * bat.c - A bouncing bat for xlock, the X Window System lockscreen.
 *
 * Copyright (c) 1988 by Sun Microsystems
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 18-Sep-95: 5 bats now in color (patol@info.isbiel.ch)
 * 20-Sep-94: 5 bats instead of bouncing balls, based on bounce.c
 *            (patol@info.isbiel.ch)
 * 2-Sep-93: bounce version (David Bagley bagleyd@hertz.njit.edu)
 * 1986: Sun Microsystems
 */

/* 
 * original copyright
 * **************************************************************************
 * Copyright 1988 by Sun Microsystems, Inc. Mountain View, CA.
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Sun or MIT not be used in advertising
 * or publicity pertaining to distribution of the software without specific
 * prior written permission. Sun and M.I.T. make no representations about the
 * suitability of this software for any purpose. It is provided "as is"
 * without any express or implied warranty.
 *
 * SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL SUN BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ***************************************************************************
 */

#include "xlock.h"
#include <math.h>

#ifdef HAS_XPM
#if 1
#include <X11/xpm.h>
#else
#include <xpm.h>
#endif
#include "pixmaps/bat-0.xpm"
#include "pixmaps/bat-1.xpm"
#include "pixmaps/bat-2.xpm"
#include "pixmaps/bat-3.xpm"
#include "pixmaps/bat-4.xpm"
#endif

#include "bitmaps/bat-0.xbm"
#include "bitmaps/bat-1.xbm"
#include "bitmaps/bat-2.xbm"
#include "bitmaps/bat-3.xbm"
#include "bitmaps/bat-4.xbm"

#define MAX_STRENGTH 24
#define FRICTION 15
#define PENETRATION 0.4
#define SLIPAGE 4
#define TIME 32

#define ORIENTS 8
#define ORIENTCYCLE 32
#define CCW 1
#define CW (ORIENTS-1)
#define DIR(x)	(((x)>=0)?CCW:CW)
#define SIGN(x)	(((x)>=0)?1:-1)
#define ABS(x)	(((x)>=0)?x:-(x))

ModeSpecOpt bat_opts =
{0, NULL, NULL, NULL};

static XImage bimages[] =
{
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1}
};
static XImage *images[ORIENTS / 2 + 1];

typedef struct {
	int         x, y, xlast, ylast;
	int         spincount, spindelay, spindir, orient;
	int         vx, vy, vang;
	int         mass, size, sizex, sizey;
	unsigned long color;
} batstruct;

typedef struct {
	int         width, height;
	int         nbats;
	int         restartnum;
	batstruct  *bats;
} bouncestruct;

static bouncestruct bounces[MAXSCREENS];

static void checkCollision(int a_bat);
static void drawabat(Window win, batstruct * bat);
static void movebat(batstruct * bat);
static void flapbat(batstruct * bat, int dir, int *vel);
static int  collide(int a_bat);
static void XEraseImage(Display * display, Window win, GC gc, int x, int y, int xlast, int ylast, int xsize, int ysize);

static      first = 1;

static unsigned char *bits[] =
{
	bat0_bits, bat1_bits, bat2_bits, bat3_bits, bat4_bits
};

#ifdef HAS_XPM
static char **pixs[] =
{
	bat0, bat1, bat2, bat3, bat4
};

#endif

static void
init_images(ModeInfo * mi)
{
	int         i;

#if HAS_XPM
	int         xpm_ret = 0;

	if (!MI_WIN_IS_MONO(mi) && MI_NPIXELS(mi) > 2)
		for (i = 0; i <= ORIENTS / 2; i++)
			xpm_ret += XpmCreateImageFromData(dsp, pixs[i], &(images[i]),
				   (XImage **) NULL, (XpmAttributes *) NULL);
	if (MI_WIN_IS_MONO(mi) || MI_NPIXELS(mi) <= 2 || xpm_ret != 0)
#endif
		for (i = 0; i <= ORIENTS / 2; i++) {
			bimages[i].data = (char *) bits[i];
			bimages[i].width = bat0_width;
			bimages[i].height = bat0_height;
			bimages[i].bytes_per_line = (bat0_width + 7) / 8;
			images[i] = &(bimages[i]);
		}
}

void
init_bat(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	bouncestruct *bp = &bounces[screen];
	int         i;

	if (first) {
		init_images(mi);
		first = 0;
	}
	bp->width = MI_WIN_WIDTH(mi);;
	bp->height = MI_WIN_HEIGHT(mi);
	bp->restartnum = TIME;

	bp->nbats = MI_BATCHCOUNT(mi);
	if (bp->nbats < 1)
		bp->nbats = 1;
	if (!bp->bats)
		bp->bats = (batstruct *) malloc(bp->nbats * sizeof (batstruct));
	i = 0;
	while (i < bp->nbats) {
		if (bat0_width > bp->width / 2 || bat0_height > bp->height / 2) {
			bp->bats[i].sizex = 7;
			bp->bats[i].sizey = 3;
			bp->bats[i].size = (bp->bats[i].sizex + bp->bats[i].sizey) / 2;
		} else {
			bp->bats[i].sizex = bat0_width;
			bp->bats[i].sizey = bat0_height;
			bp->bats[i].size = (bp->bats[i].sizex + bp->bats[i].sizey) / 2;
		}
		bp->bats[i].vx = ((LRAND() & 1) ? -1 : 1) * (LRAND() % MAX_STRENGTH + 1);
		bp->bats[i].x = (bp->bats[i].vx >= 0) ? 0 : bp->width - bp->bats[i].sizex;
		bp->bats[i].y = LRAND() % (bp->height / 2);
		if (i == collide(i)) {
			if (!MI_WIN_IS_MONO(mi) && MI_NPIXELS(mi) > 2)
				bp->bats[i].color = Scr[screen].pixels[LRAND() % Scr[screen].npixels];
			else
				bp->bats[i].color = WhitePixel(dsp, screen);
			bp->bats[i].xlast = -1;
			bp->bats[i].ylast = 0;
			bp->bats[i].spincount = 1;
			bp->bats[i].spindelay = 1;
			bp->bats[i].vy = ((LRAND() & 1) ? -1 : 1) * (LRAND() % MAX_STRENGTH);
			bp->bats[i].spindir = 0;
			bp->bats[i].vang = 0;
			bp->bats[i].orient = LRAND() % ORIENTS;
			i++;
		} else
			bp->nbats--;
	}
	XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
	XFillRectangle(dsp, win, Scr[screen].gc, 0, 0, bp->width, bp->height);
}

static void
checkCollision(int a_bat)
{
	bouncestruct *bp = &bounces[screen];
	int         i, amount, spin, d, size;
	double      x, y;

	for (i = 0; i < bp->nbats; i++) {
		if (i != a_bat) {
			x = (double) (bp->bats[i].x - bp->bats[a_bat].x);
			y = (double) (bp->bats[i].y - bp->bats[a_bat].y);
			d = (int) sqrt(x * x + y * y);
			size = (bp->bats[i].size + bp->bats[a_bat].size) / 2;
			if (d > 0 && d < size) {
				amount = size - d;
				if (amount > PENETRATION * size)
					amount = PENETRATION * size;
				bp->bats[i].vx += amount * x / d;
				bp->bats[i].vy += amount * y / d;
				bp->bats[i].vx -= bp->bats[i].vx / FRICTION;
				bp->bats[i].vy -= bp->bats[i].vy / FRICTION;
				bp->bats[a_bat].vx -= amount * x / d;
				bp->bats[a_bat].vy -= amount * y / d;
				bp->bats[a_bat].vx -= bp->bats[a_bat].vx / FRICTION;
				bp->bats[a_bat].vy -= bp->bats[a_bat].vy / FRICTION;
				spin = (bp->bats[i].vang - bp->bats[a_bat].vang) /
					(2 * size * SLIPAGE);
				bp->bats[i].vang -= spin;
				bp->bats[a_bat].vang += spin;
				bp->bats[i].spindir = DIR(bp->bats[i].vang);
				bp->bats[a_bat].spindir = DIR(bp->bats[a_bat].vang);
				if (!bp->bats[i].vang) {
					bp->bats[i].spindelay = 1;
					bp->bats[i].spindir = 0;
				} else
					bp->bats[i].spindelay = M_PI * bp->bats[i].size /
						(ABS(bp->bats[i].vang)) + 1;
				if (!bp->bats[a_bat].vang) {
					bp->bats[a_bat].spindelay = 1;
					bp->bats[a_bat].spindir = 0;
				} else
					bp->bats[a_bat].spindelay = M_PI * bp->bats[a_bat].size /
						(ABS(bp->bats[a_bat].vang)) + 1;
				return;
			}
		}
	}
}

void
draw_bat(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	bouncestruct *bp = &bounces[screen];
	int         i;

	for (i = 0; i < bp->nbats; i++) {
		drawabat(win, &bp->bats[i]);
		movebat(&bp->bats[i]);
	}
	for (i = 0; i < bp->nbats; i++)
		checkCollision(i);
	if (!(LRAND() % TIME))	/* Put some randomness into the time */
		bp->restartnum--;
	if (!bp->restartnum)
		init_bat(mi);
}

static void
drawabat(Window win, batstruct * bat)
{
	if (bat->sizex < bat0_width) {
		if (bat->xlast != -1) {
			XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
			XFillRectangle(dsp, win, Scr[screen].gc,
			     bat->xlast, bat->ylast, bat->sizex, bat->sizey);
		}
		XSetForeground(dsp, Scr[screen].gc, bat->color);
		XFillRectangle(dsp, win, Scr[screen].gc,
			       bat->x, bat->y, bat->sizex, bat->sizey);
	} else {
		XSetForeground(dsp, Scr[screen].gc, bat->color);
		XPutImage(dsp, win, Scr[screen].gc,
			  images[(bat->orient > ORIENTS / 2) ? ORIENTS - bat->orient : bat->orient],
			  0, 0, bat->x, bat->y, bat->sizex, bat->sizey);
		if (bat->xlast != -1) {
			XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
			XEraseImage(dsp, win, Scr[screen].gc,
				    bat->x, bat->y, bat->xlast, bat->ylast, bat->sizex, bat->sizey);
		}
	}
}

static void
movebat(batstruct * bat)
{
	bouncestruct *bp = &bounces[screen];

	bat->xlast = bat->x;
	bat->ylast = bat->y;
	bat->x += bat->vx;
	if (bat->x > (bp->width - bat->sizex)) {
		/* Bounce off the right edge */
		bat->x = 2 * (bp->width - bat->sizex) - bat->x;
		bat->vx = -bat->vx + bat->vx / FRICTION;
		flapbat(bat, 1, &bat->vy);
	} else if (bat->x < 0) {
		/* Bounce off the left edge */
		bat->x = -bat->x;
		bat->vx = -bat->vx + bat->vx / FRICTION;
		flapbat(bat, -1, &bat->vy);
	}
	bat->vy++;
	bat->y += bat->vy;
	if (bat->y >= (bp->height + bat->sizey)) {	/* Don't see bat bounce */
		/* Bounce off the bottom edge */
		bat->y = (bp->height - bat->sizey);
		bat->vy = -bat->vy + bat->vy / FRICTION;
		flapbat(bat, -1, &bat->vx);
	}			/* else if (bat->y < 0) { */
	/* Bounce off the top edge */
	/*bat->y = -bat->y;
	   bat->vy = -bat->vy + bat->vy / FRICTION;
	   flapbat(bat, 1, &bat->vx);
	   } */
	if (bat->spindir) {
		bat->spincount--;
		if (!bat->spincount) {
			bat->orient = (bat->spindir + bat->orient) % ORIENTS;
			bat->spincount = bat->spindelay;
		}
	}
}

static void
flapbat(batstruct * bat, int dir, int *vel)
{
	*vel -= (*vel + SIGN(*vel * dir) * bat->spindelay * ORIENTCYCLE /
		 (M_PI * bat->size)) / SLIPAGE;
	if (*vel) {
		bat->spindir = DIR(*vel * dir);
		bat->vang = *vel * ORIENTCYCLE;
		bat->spindelay = M_PI * bat->size / (ABS(bat->vang)) + 1;
	} else
		bat->spindir = 0;
}

static int
collide(int a_bat)
{
	bouncestruct *bp = &bounces[screen];
	int         i, d, x, y;

	for (i = 0; i < a_bat; i++) {
		x = (bp->bats[i].x - bp->bats[a_bat].x);
		y = (bp->bats[i].y - bp->bats[a_bat].y);
		d = (int) sqrt((double) (x * x + y * y));
		if (d < (bp->bats[i].size + bp->bats[a_bat].size) / 2)
			return i;
	}
	return i;
}

/* This stops some flashing, could be more efficient */
static void
XEraseImage(Display * display, Window win, GC gc, int x, int y, int xlast, int ylast, int xsize, int ysize)
{
	if (ylast < y) {
		if (y < ylast + ysize)
			XFillRectangle(display, win, gc, xlast, ylast, xsize, y - ylast);
		else
			XFillRectangle(display, win, gc, xlast, ylast, xsize, ysize);
	} else if (ylast > y) {
		if (y > ylast - ysize)
			XFillRectangle(display, win, gc, xlast, y + ysize, xsize, ylast - y);
		else
			XFillRectangle(display, win, gc, xlast, ylast, xsize, ysize);
	}
	if (xlast < x) {
		if (x < xlast + xsize)
			XFillRectangle(display, win, gc, xlast, ylast, x - xlast, ysize);
		else
			XFillRectangle(display, win, gc, xlast, ylast, xsize, ysize);
	} else if (xlast > x) {
		if (x > xlast - xsize)
			XFillRectangle(display, win, gc, x + xsize, ylast, xlast - x, ysize);
		else
			XFillRectangle(display, win, gc, xlast, ylast, xsize, ysize);
	}
}
