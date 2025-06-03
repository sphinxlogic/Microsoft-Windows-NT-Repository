/*
 *  acm : an aerial combat simulator for X
 *  Copyright (C) 1991  Riley Rainey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program;  if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave., Cambridge, MA 02139, USA.
 */

#include "pm.h"

static struct {
	long	mask;
	char	*name;
	}	*pptr, panelVec[] = {
	SYS_ENGINE1,	"OIL PRES",
	SYS_HYD1,	"HYD1 PRES",
	SYS_HYD2,	"HYD2 PRES",
	SYS_GEN1,	"GEN1 FAIL",
	SYS_GEN2,	"GEN2 FAIL",
	SYS_FLAPS,	"FLAP FAIL",
	SYS_SPEEDBRAKE, "SPBRK FAIL",
	SYS_RADAR,	"RADAR FAIL",
	SYS_TEWS,	"TEWS FAIL",
	SYS_HUD,	" HUD FAIL",
	FLAG_LOWFUEL,	" LOW FUEL",
	FLAG_SPEEDBRAKE, "SPD BRAKE",
	FLAG_WHEELBRAKE, "  BRAKES",
	0,		(char *) 0 };

#define panelRows	7
#define panelColumns	2
#define panelChars	10
#define lightMargin	((u->rftw + 1) / 3)	
#define panelWMargin	(u->rftw * 2)
#define panelHMargin	(u->rfth / 2)

long	lastBits[MAXPLAYERS];
int	rdraw[MAXPLAYERS];
int	lastPos[MAXPLAYERS];
int	lastGPos[MAXPLAYERS][3];

void	initPanel (c)
craft	*c; {

	rdraw[c->pIndex] = 0;
	lastPos[c->pIndex] = -1;
	lastGPos[c->pIndex][0] = lastGPos[c->pIndex][1] =
		lastGPos[c->pIndex][2] = -1;
	lastBits [c->pIndex] = SYS_NODAMAGE;

}

void	redrawPanel (c)
craft	*c; {

	rdraw [c->pIndex] = 1;
	lastPos[c->pIndex] = -1;
	lastGPos[c->pIndex][0] = lastGPos[c->pIndex][1] =
		lastGPos[c->pIndex][2] = -1;
	lastBits [c->pIndex] = ~lastBits[c->pIndex];
}

void	resizePanel (c, u)
craft	*c;
viewer	*u; {

	u->panelx = (int)((TEWS_X - 20) * u->scaleFactor + 0.5) -
		(u->TEWSSize + 1) / 2 -
		panelColumns * (panelChars * u->rftw + 2 * lightMargin +
		panelWMargin);
	u->panely = PANEL_Y * u->scaleFactor;

}

void doPanel (c, u)
craft	*c;
viewer	*u; {

	int	cellH, cellW;
	int	xi, yi, x, y, i;
	long	changeBits;

	doGearLights (c, u);

	cellH = u->rfth + 2 * lightMargin + panelHMargin;
	cellW = u->rftw * panelChars + 2 * lightMargin + panelWMargin;

	changeBits = lastBits[c->pIndex] ^ c->damageBits;

	for (pptr = &panelVec[0], i=0; pptr->mask != 0; ++pptr, ++i) {
		if (changeBits & pptr->mask) {
			xi = i / panelRows;
			yi = i % panelRows;
			x = u->panelx;
			x += xi * cellW + lightMargin;
			y = u->panely;
			y += yi * cellH + lightMargin;
			if ((c->damageBits & pptr->mask) == 0) 
				XDrawImageString (u->dpy, u->win, u->gc,
					x, y, pptr->name, strlen(pptr->name));
			else {
				y -= u->rfth;
				XClearArea (u->dpy, u->win, x, y,
					u->rftw * panelChars,
					u->rfth + lightMargin, False);
			}
		}
	}

	lastBits[c->pIndex] = c->damageBits;
	rdraw[c->pIndex] = 0;
}

int doGearLights (c, u)
craft	*c;
viewer	*u; {

	int	x, y, x1, y1, pos;
	double	geardown;

	pos = (c->flags & FL_GHANDLE_DN) ? 1 : 0;

	x = u->xCenter + (u->radarWidth + 1) / 2 + (int) (20.0 *
		u->scaleFactor + 0.5);

	y = u->height + (int) (80.0 * u->scaleFactor + 0.5);

	XSetForeground (u->dpy, u->gc, (u->v->flags & VPMono) ?
		WhitePixel(u->v->dpy, u->v->screen) :
		VConstantColor(u->v, whitePixel));

	XSetPlaneMask (u->dpy, u->gc, AllPlanes);

	if (pos != lastPos[c->pIndex]) {
		XCopyPlane (u->dpy, u->handle[pos], u->win, u->gc, 0, 0,
			handleDn_width, handleDn_height, x, y, 1);
		lastPos[c->pIndex] = pos;
	}

	geardown = pi / 2.0;

	if (c->flags & FL_GEAR0_UP)
		pos = 0;
	else if (c->curGear[0] != geardown)
		pos = 1;
	else
		pos = 2;
	
	if (pos != lastGPos[c->pIndex][0]) {
		x1 = x + handleDn_width + (int)(20.0 * u->scaleFactor + 0.5);
		y1 = y + (int)(10.0 * u->scaleFactor + 0.5);
		XCopyPlane (u->dpy, u->gearLight[pos], u->win, u->gc, 0, 0,
			gearDown_width, gearDown_height, x1, y1, 1);
		lastGPos[c->pIndex][0] = pos;
	}

	if (c->flags & FL_GEAR1_UP)
		pos = 0;
	else if (c->curGear[1] != geardown)
		pos = 1;
	else
		pos = 2;
	
	if (pos != lastGPos[c->pIndex][1]) {
		x1 = x + handleDn_width;
		y1 = y + (int)(50.0 * u->scaleFactor + 0.5);
		XCopyPlane (u->dpy, u->gearLight[pos], u->win, u->gc, 0, 0,
			gearDown_width, gearDown_height, x1, y1, 1);
		lastGPos[c->pIndex][1] = pos;
	}

	if (c->flags & FL_GEAR2_UP)
		pos = 0;
	else if (c->curGear[2] != geardown)
		pos = 1;
	else
		pos = 2;
	
	if (pos != lastGPos[c->pIndex][2]) {
		x1 = x + handleDn_width + (int)(40.0 * u->scaleFactor + 0.5);
		y1 = y + (int)(50.0 * u->scaleFactor + 0.5);
		XCopyPlane (u->dpy, u->gearLight[pos], u->win, u->gc, 0, 0,
			gearDown_width, gearDown_height, x1, y1, 1);
		lastGPos[c->pIndex][2] = pos;
	}
	return 0;

}
