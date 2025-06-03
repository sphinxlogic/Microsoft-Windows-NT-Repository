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
#include <X11/Xutil.h>

extern int flightCalculations();
extern int missileCalculations();
extern void doEvents(), doViews(), flapControl(), resupplyCheck();

int cur = 0;

int redraw () {

	int	i;
	craft	*p;

	for ((i=0, p=ptbl); i<MAXPLAYERS; (++i, ++p)) {
		if (p->type == CT_PLANE) {
			doEvents (p);
			if (flightCalculations (p) == 1)
				killPlayer (p);
			doWeaponUpdate (p);
			flapControl (p);
		}
		else if (p->type == CT_DRONE)
			if (droneCalculations (p) == 1)
				killPlayer (p);
	}

	for ((i=0, p=mtbl); i<MAXPROJECTILES; (++i, ++p)) {
		if (p->type == CT_MISSILE) {
			if (missileCalculations (p) == 1)
				killMissile (p);
		}
		else if (p->type == CT_CANNON) {
			if (cannonCalculations (p) == 1)
				killMissile (p);
		}
		else if (p->type == CT_EXPLOSION) {
			-- p->flameDuration;
			if ((-- p->duration) <= 0)
				p->type = CT_FREE;
		}
	}

	lookForImpacts ();

	if (cur % REDRAW_EVERY == 0)
		doViews ();

	if (cur++ % RESUPPLY_EVERY == 0)
		resupplyCheck();

	curTime += deltaT;

	return 0;

}
