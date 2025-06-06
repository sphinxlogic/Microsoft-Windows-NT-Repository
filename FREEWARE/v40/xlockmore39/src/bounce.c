#ifndef lint
static char sccsid[] = "@(#)bounce.c	3.9 96/05/25 xlockmore";

#endif

/*-
 * bounce.c - A bouncing ball for xlock, the X Window System lockscreen.
 *
 * Copyright (c) 1988 by Sun Microsystems
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 18-Sep-95: tinkered with it to look like bat.c .
 * 15-Jul-94: cleaned up in time for the final match.
 * 4-Apr-94: spinning multiple ball version
 *             (I got a lot of help from with the physics of ball to ball
 *             collision looking at the source of xpool from Ismail ARIT
 *             iarit@tara.mines.colorado.edu).
 * 22-Mar-94: got rid of flashing problem by only erasing parts of the image
 *             that will not be in the next image.
 * 2-Sep-93: xlock version (David Bagley bagleyd@hertz.njit.edu)
 * 1986: Sun Microsystems
 */

/*-
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

/* 
   Open for improvement: o include different balls (size and mass)  (... how
   about be real crazy and put in an American/Australian football?). o mask
   out the corners of ball. o should only have 1 bitmap for ball, the others
   should be generated as 90 degree rotations. */

#include "xlock.h"
#include <math.h>
#include "bitmaps/bounce-0.xbm"
#include "bitmaps/bounce-1.xbm"
#include "bitmaps/bounce-2.xbm"
#include "bitmaps/bounce-3.xbm"

#define MAX_STRENGTH 24
#define FRICTION 24
#define PENETRATION 0.3
#define SLIPAGE 4
#define TIME 32

#define ORIENTS 4
#define ORIENTCYCLE 16
#define CCW 1
#define CW (ORIENTS-1)
#define DIR(x)	(((x)>=0)?CCW:CW)
#define SIGN(x)	(((x)>=0)?1:-1)
#define ABS(x)	(((x)>=0)?x:-(x))

ModeSpecOpt bounce_opts =
{0, NULL, NULL, NULL};

static XImage bimages[] =
{
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
	{0, 0, 0, XYBitmap, 0, LSBFirst, 8, LSBFirst, 8, 1},
};

typedef struct {
	int         x, y, xlast, ylast;
	int         spincount, spindelay, spindir, orient;
	int         vx, vy, vang;
	int         mass, size;
	unsigned long color;
} ballstruct;

typedef struct {
	int         width, height;
	int         nballs;
	int         restartnum;
	ballstruct *balls;
} bouncestruct;

static bouncestruct bounces[MAXSCREENS];

static void checkCollision(int aball);
static void drawball(Window win, ballstruct * ball);
static void moveball(ballstruct * ball);
static void spinball(ballstruct * ball, int dir, int *vel);
static int  collide(int aball);
static void XEraseImage(Display * display, Window win, GC gc, int x, int y, int xlast, int ylast, int xsize, int ysize);

static      first = 1;

static unsigned char *bits[] =
{
	bounce0_bits, bounce1_bits, bounce2_bits, bounce3_bits
};

static void
init_images(void)
{
	int         i;

	for (i = 0; i < ORIENTS; i++) {
		bimages[i].data = (char *) bits[i];
		bimages[i].width = bounce0_width;
		bimages[i].height = bounce0_height;
		bimages[i].bytes_per_line = (bounce0_width + 7) / 8;
	}
}

void
init_bounce(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	bouncestruct *bp = &bounces[screen];
	int         i;

	if (first) {
		init_images();
		first = 0;
	}
	bp->width = MI_WIN_WIDTH(mi);
	bp->height = MI_WIN_HEIGHT(mi);
	bp->restartnum = TIME;

	bp->nballs = MI_BATCHCOUNT(mi);
	if (bp->nballs < 1)
		bp->nballs = 1;
	if (!bp->balls)
		bp->balls = (ballstruct *) malloc(bp->nballs * sizeof (ballstruct));
	i = 0;
	while (i < bp->nballs) {
		if (bounce0_width > bp->width / 2 || bounce0_height > bp->height / 2)
			bp->balls[i].size = 5;
		else
			bp->balls[i].size = (bounce0_width + bounce0_height) / 2;
		bp->balls[i].vx = ((LRAND() & 1) ? -1 : 1) *
			(LRAND() % MAX_STRENGTH + 1);
		bp->balls[i].x = (bp->balls[i].vx >= 0) ?
			0 : bp->width - bp->balls[i].size;
		bp->balls[i].y = LRAND() % (bp->height / 2);
		if (i == collide(i)) {
			if (!MI_WIN_IS_MONO(mi) && MI_NPIXELS(mi) > 2)
				bp->balls[i].color =
					Scr[screen].pixels[LRAND() % Scr[screen].npixels];
			else
				bp->balls[i].color = WhitePixel(dsp, screen);
			bp->balls[i].xlast = -1;
			bp->balls[i].ylast = 0;
			bp->balls[i].spincount = 1;
			bp->balls[i].spindelay = 1;
			bp->balls[i].vy = ((LRAND() & 1) ? -1 : 1) * (LRAND() % MAX_STRENGTH);
			bp->balls[i].spindir = 0;
			bp->balls[i].vang = 0;
			bp->balls[i].orient = LRAND() % ORIENTS;
			i++;
		} else
			bp->nballs--;
	}
	XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
	XFillRectangle(dsp, win, Scr[screen].gc, 0, 0, bp->width, bp->height);
}

static void
checkCollision(int aball)
{
	bouncestruct *bp = &bounces[screen];
	int         i, amount, spin, d, size;
	double      x, y;

	for (i = 0; i < bp->nballs; i++) {
		if (i != aball) {
			x = (double) (bp->balls[i].x - bp->balls[aball].x);
			y = (double) (bp->balls[i].y - bp->balls[aball].y);
			d = (int) sqrt(x * x + y * y);
			size = (bp->balls[i].size + bp->balls[aball].size) / 2;
			if (d > 0 && d < size) {
				amount = size - d;
				if (amount > PENETRATION * size)
					amount = PENETRATION * size;
				bp->balls[i].vx += amount * x / d;
				bp->balls[i].vy += amount * y / d;
				bp->balls[i].vx -= bp->balls[i].vx / FRICTION;
				bp->balls[i].vy -= bp->balls[i].vy / FRICTION;
				bp->balls[aball].vx -= amount * x / d;
				bp->balls[aball].vy -= amount * y / d;
				bp->balls[aball].vx -= bp->balls[aball].vx / FRICTION;
				bp->balls[aball].vy -= bp->balls[aball].vy / FRICTION;
				spin = (bp->balls[i].vang - bp->balls[aball].vang) /
					(2 * size * SLIPAGE);
				bp->balls[i].vang -= spin;
				bp->balls[aball].vang += spin;
				bp->balls[i].spindir = DIR(bp->balls[i].vang);
				bp->balls[aball].spindir = DIR(bp->balls[aball].vang);
				if (!bp->balls[i].vang) {
					bp->balls[i].spindelay = 1;
					bp->balls[i].spindir = 0;
				} else
					bp->balls[i].spindelay = M_PI * bp->balls[i].size /
						(ABS(bp->balls[i].vang)) + 1;
				if (!bp->balls[aball].vang) {
					bp->balls[aball].spindelay = 1;
					bp->balls[aball].spindir = 0;
				} else
					bp->balls[aball].spindelay = M_PI * bp->balls[aball].size /
						(ABS(bp->balls[aball].vang)) + 1;
				return;
			}
		}
	}
}

void
draw_bounce(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	bouncestruct *bp = &bounces[screen];
	int         i;

	for (i = 0; i < bp->nballs; i++) {
		drawball(win, &bp->balls[i]);
		moveball(&bp->balls[i]);
	}
	for (i = 0; i < bp->nballs; i++)
		checkCollision(i);
	if (!(LRAND() % TIME))	/* Put some randomness into the time */
		bp->restartnum--;
	if (!bp->restartnum)
		init_bounce(mi);
}

static void
drawball(Window win, ballstruct * ball)
{
	if (ball->size <= 5) {
		if (ball->xlast != -1) {
			XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
			XFillRectangle(dsp, win, Scr[screen].gc,
			   ball->xlast, ball->ylast, ball->size, ball->size);
		}
		XSetForeground(dsp, Scr[screen].gc, ball->color);
		XFillRectangle(dsp, win, Scr[screen].gc,
			       ball->x, ball->y, ball->size, ball->size);
	} else {
		XSetForeground(dsp, Scr[screen].gc, ball->color);
		XPutImage(dsp, win, Scr[screen].gc,
			  &(bimages[ball->orient]),
			  0, 0, ball->x, ball->y, ball->size, ball->size);
		if (ball->xlast != -1) {
			XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
			XEraseImage(dsp, win, Scr[screen].gc,
				    ball->x, ball->y, ball->xlast, ball->ylast, ball->size, ball->size);
		}
	}
}

static void
moveball(ballstruct * ball)
{
	bouncestruct *bp = &bounces[screen];

	ball->xlast = ball->x;
	ball->ylast = ball->y;
	ball->x += ball->vx;
	if (ball->x > (bp->width - ball->size)) {
		/* Bounce off the right edge */
		ball->x = 2 * (bp->width - ball->size) - ball->x;
		ball->vx = -ball->vx + ball->vx / FRICTION;
		spinball(ball, 1, &ball->vy);
	} else if (ball->x < 0) {
		/* Bounce off the left edge */
		ball->x = -ball->x;
		ball->vx = -ball->vx + ball->vx / FRICTION;
		spinball(ball, -1, &ball->vy);
	}
	ball->vy++;
	ball->y += ball->vy;
	if (ball->y >= (bp->height - ball->size)) {
		/* Bounce off the bottom edge */
		ball->y = (bp->height - ball->size);
		ball->vy = -ball->vy + ball->vy / FRICTION;
		spinball(ball, -1, &ball->vx);
	} else if (ball->y < 0) {
		/* Bounce off the top edge */
		ball->y = -ball->y;
		/* HACK to make it look better for iconified mode */
		ball->vy = 0;
/*-ball->vy + ball->vy / FRICTION;*/
		spinball(ball, 1, &ball->vx);
	}
	if (ball->spindir) {
		ball->spincount--;
		if (!ball->spincount) {
			ball->orient = (ball->spindir + ball->orient) % ORIENTS;
			ball->spincount = ball->spindelay;
		}
	}
}

static void
spinball(ballstruct * ball, int dir, int *vel)
{
	*vel -= (*vel + SIGN(*vel * dir) * ball->spindelay * ORIENTCYCLE /
		 (M_PI * ball->size)) / SLIPAGE;
	if (*vel) {
		ball->spindir = DIR(*vel * dir);
		ball->vang = *vel * ORIENTCYCLE;
		ball->spindelay = M_PI * ball->size / (ABS(ball->vang)) + 1;
	} else
		ball->spindir = 0;
}

static int
collide(int aball)
{
	bouncestruct *bp = &bounces[screen];
	int         i, d, x, y;

	for (i = 0; i < aball; i++) {
		x = (bp->balls[i].x - bp->balls[aball].x);
		y = (bp->balls[i].y - bp->balls[aball].y);
		d = (int) sqrt((double) (x * x + y * y));
		if (d < (bp->balls[i].size + bp->balls[aball].size) / 2)
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
