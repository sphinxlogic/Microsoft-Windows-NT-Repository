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

#include <math.h>
#include "pm.h"

/*
 *  resupplyCheck :  If a player is on the airport grounds and motionless,
 *		     then invoke the plane's resupply procedure.
 */

void resupplyCheck () {

	craft	*c;
	int	i;
	double	d, x, y, z;

	for ((i=0, c = &ptbl[0]); i<MAXPLAYERS; (++i, ++c)) {

		if (c->type != CT_PLANE)
			continue;

		if (mag(c->Cg) == 0.0) {
			x = c->Sg.x - teamLoc[c->team].x;
			y = c->Sg.y - teamLoc[c->team].y;
			z = c->Sg.z - teamLoc[c->team].z;
			d = sqrt (x * x + y * y + z * z);
			if (d <= MAX_GROUND_DISTANCE)
				(*c->cinfo->resupply)(c);
			(*wtbl[c->curWeapon].select)(c);
		}
	}
}
