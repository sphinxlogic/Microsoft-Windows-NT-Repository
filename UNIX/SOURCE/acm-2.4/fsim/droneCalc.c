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

/*
 *  Drone flight management
 */

int droneCalculations (c)
craft *c; {

	c->prevSg = c->Sg;

	c->Sg.x += c->Cg.x * deltaT;
	c->Sg.y += c->Cg.y * deltaT;
	c->Sg.z += c->Cg.z * deltaT;

/*
 *  Don't let drones get above 60 thousand feet
 */

	if (c->Sg.z < -60000.0)
		return 1;

/*
 *  Drone crash detection
 */

	if (c->Sg.z > 0.0)
		return 1;

	return 0;
}
