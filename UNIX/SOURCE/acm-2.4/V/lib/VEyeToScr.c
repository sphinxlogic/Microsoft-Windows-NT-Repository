#include "Vlib.h"

/*
 *  Transform a 3-D point in the eye space system into viewable
 *  coordinates.  The function returns 1 if the x,y information is
 *  displayable (probably displayable, that is).
 *
 *  VWorldToEye can be used to convert world coordinates to x,y values.
 */

int VEyeToScreen (v, p, x, y)
Viewport	*v;
VPoint		*p;
int		*x, *y; {

	register int valid;

	if (p->z > 0.0) {
		*x = v->Middl.x + v->Scale.x * p->x / p->z;
		*y = v->Middl.y - v->Scale.y * p->y / p->z;
		valid = 1;
	}
	else
		valid = 0;

	return valid;
}
