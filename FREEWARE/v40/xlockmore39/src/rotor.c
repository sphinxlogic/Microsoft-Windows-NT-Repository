#ifndef lint
static char sccsid[] = "@(#)rotor.c	3.9 96/05/25 xlockmore";

#endif

/*-
 * rotor.c - A swirly rotor for xlock, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * 08-Mar-95: CAT stuff for ## was tripping up some C compilers.  Removed.
 * 01-Dec-93: added patch for AIXV3 from
 *            (Tom McConnell, tmcconne@sedona.intel.com)
 * 11-Nov-90: put into xlock (by Steve Zellers, zellers@sun.com)
 * 16-Oct-90: Received from Tom Lawrence (tcl@cs.brown.edu: 'flight' simulator)
 */

/* 
 * A 'batchcount' of 3 or 4 works best!
 */

#include <math.h>
#include "xlock.h"

#define SAVE 		100	/* this is a good constant to tweak */
#define REPS  		50

#define MAXANGLE	10000.0	/* irrectangular */
#define DEFAULTCOUNT	3

ModeSpecOpt rotor_opts =
{0, NULL, NULL, NULL};

typedef unsigned char Boolean;

struct elem {
	float       angle;
	float       radius;
	float       start_radius;
	float       end_radius;
	float       radius_drift_max;
	float       radius_drift_now;

	float       ratio;
	float       start_ratio;
	float       end_ratio;
	float       ratio_drift_max;
	float       ratio_drift_now;
};

typedef struct flightstruct {
	struct elem *elements;
	int         pix;
	int         lastx, lasty;
	int         num, rotor, prev;
	int         savex[SAVE], savey[SAVE];
	float       angle;
	int         centerx, centery;
	Boolean     firsttime;
	Boolean     smallscreen;	/* for iconified view */
	Boolean     forward;
	Boolean     unused;
} flightstruct;


static flightstruct flights[MAXSCREENS];

void
init_rotor(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	flightstruct *fs = &flights[screen];
	int         x;
	struct elem *pelem;
	Boolean     wassmall;

	fs->centerx = MI_WIN_WIDTH(mi) / 2;
	fs->centery = MI_WIN_HEIGHT(mi) / 2;

	/*
	 * sometimes, you go into small view, only to see a really whizzy pattern
	 * that you would like to look more closely at. Normally, clicking in the
	 * icon reinitializes everything - but I don't, cuz I'm that kind of guy.
	 * HENCE, the wassmall stuff you see here.
	 */

	wassmall = fs->smallscreen;
	fs->smallscreen = (fs->centerx + fs->centery < 100);

	if (wassmall && !fs->smallscreen)
		fs->firsttime = True;
	else {
		fs->num = MI_BATCHCOUNT(mi);
		if (fs->num > 12)
			fs->num = DEFAULTCOUNT;

		if (fs->elements == NULL) {
			if ((fs->elements = (struct elem *)
			     malloc(sizeof (struct elem) * fs->num)) == 0) {
				perror("malloc");
				exit(1);
			}
		}
		(void) memset((char *) fs->savex, 0, sizeof (fs->savex));

		pelem = fs->elements;

		for (x = fs->num; --x >= 0; pelem++) {
			pelem->radius_drift_max = 1.0;
			pelem->radius_drift_now = 1.0;

			pelem->end_radius = 100.0;

			pelem->ratio_drift_max = 1.0;
			pelem->ratio_drift_now = 1.0;
			pelem->end_ratio = 10.0;
		}

		fs->rotor = 0;
		fs->prev = 1;
		fs->lastx = fs->centerx;
		fs->lasty = fs->centery;
		fs->angle = (LRAND() % (long) MAXANGLE) / 3;
		fs->forward = fs->firsttime = True;
	}
	XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));
	XFillRectangle(dsp, win, Scr[screen].gc, 0, 0,
		       MI_WIN_WIDTH(mi), MI_WIN_HEIGHT(mi));
}

void
draw_rotor(ModeInfo * mi)
{
	Window      win = MI_WINDOW(mi);
	register flightstruct *fs = &flights[screen];
	register struct elem *pelem;
	int         thisx, thisy;
	int         i, rp;
	int         x_1, y_1, x_2, y_2;

	for (rp = 0; rp < REPS; rp++) {
		thisx = fs->centerx;
		thisy = fs->centery;

		for (i = fs->num, pelem = fs->elements; --i >= 0; pelem++) {
			if (pelem->radius_drift_max <= pelem->radius_drift_now) {
				pelem->start_radius = pelem->end_radius;
				pelem->end_radius =
					(float) (LRAND() % 40000) / 100.0 - 200.0;
				pelem->radius_drift_max =
					(float) (LRAND() % 100000) + 10000.0;
				pelem->radius_drift_now = 0.0;
			}
			if (pelem->ratio_drift_max <= pelem->ratio_drift_now) {
				pelem->start_ratio = pelem->end_ratio;
				pelem->end_ratio =
					(float) (LRAND() % 2000) / 100.0 - 10.0;
				pelem->ratio_drift_max =
					(float) (LRAND() % 100000) + 10000.0;
				pelem->ratio_drift_now = 0.0;
			}
			pelem->ratio = pelem->start_ratio +
				(pelem->end_ratio - pelem->start_ratio) /
				pelem->ratio_drift_max * pelem->ratio_drift_now;
			pelem->angle = fs->angle * pelem->ratio;
			pelem->radius = pelem->start_radius +
				(pelem->end_radius - pelem->start_radius) /
				pelem->radius_drift_max * pelem->radius_drift_now;

			thisx += (int) (COSF(pelem->angle) * pelem->radius);
			thisy += (int) (SINF(pelem->angle) * pelem->radius);

			pelem->ratio_drift_now += 1.0;
			pelem->radius_drift_now += 1.0;
		}
		if (fs->firsttime)
			fs->firsttime = False;
		else {
			XSetForeground(dsp, Scr[screen].gc, BlackPixel(dsp, screen));

			x_1 = (int) fs->savex[fs->rotor];
			y_1 = (int) fs->savey[fs->rotor];
			x_2 = (int) fs->savex[fs->prev];
			y_2 = (int) fs->savey[fs->prev];

			if (fs->smallscreen) {
				x_1 /= 12;
				x_1 += fs->centerx - 2;
				x_2 /= 12;
				x_2 += fs->centerx - 2;
				y_1 /= 12;
				y_1 += fs->centery - 2;
				y_2 /= 12;
				y_2 += fs->centery - 2;
			}
			XDrawLine(dsp, win, Scr[screen].gc, x_1, y_1, x_2, y_2);

			if (!MI_WIN_IS_MONO(mi) && MI_NPIXELS(mi) > 2) {
				XSetForeground(dsp, Scr[screen].gc,
					       Scr[screen].pixels[fs->pix]);
				if (++fs->pix >= Scr[screen].npixels)
					fs->pix = 0;
			} else
				XSetForeground(dsp, Scr[screen].gc, WhitePixel(dsp, screen));

			x_1 = fs->lastx;
			y_1 = fs->lasty;
			x_2 = thisx;
			y_2 = thisy;

			if (fs->smallscreen) {
				x_1 /= 12;
				x_1 += fs->centerx - 2;
				x_2 /= 12;
				x_2 += fs->centerx - 2;
				y_1 /= 12;
				y_1 += fs->centery - 2;
				y_2 /= 12;
				y_2 += fs->centery - 2;
			}
			XDrawLine(dsp, win, Scr[screen].gc, x_1, y_1, x_2, y_2);
		}
		fs->savex[fs->rotor] = fs->lastx = thisx;
		fs->savey[fs->rotor] = fs->lasty = thisy;

		++fs->rotor;
		fs->rotor %= SAVE;
		++fs->prev;
		fs->prev %= SAVE;
		if (fs->forward) {
			fs->angle += 0.01;
			if (fs->angle >= MAXANGLE) {
				fs->angle = MAXANGLE;
				fs->forward = False;
			}
		} else {
			fs->angle -= 0.1;
			if (fs->angle <= 0) {
				fs->angle = 0.0;
				fs->forward = True;
			}
		}
	}
}
