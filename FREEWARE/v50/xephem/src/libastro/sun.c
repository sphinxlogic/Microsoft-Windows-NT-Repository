#include <stdio.h>
#include <math.h>

#include "P_.h"
#include "astro.h"
#include "vsop87.h"

/* given the modified JD, mjd, return the true geocentric ecliptic longitude
 *   of the sun for the mean equinox of the date, *lsn, in radians, the
 *   sun-earth distance, *rsn, in AU, and the latitude *bsn, in radians
 *   (since this is always <= 1.2 arcseconds, in can be neglected by
 *   calling with bsn = NULL).
 *
 * if the APPARENT ecliptic longitude is required, correct the longitude for
 *   nutation to the true equinox of date and for aberration (light travel time,
 *   approximately  -9.27e7/186000/(3600*24*365)*2*pi = -9.93e-5 radians).
 */
void
sunpos (mjd, lsn, rsn, bsn)
double mjd;
double *lsn, *rsn, *bsn;
{
	static double last_mjd = -3691, last_lsn, last_rsn, last_bsn;
	double ret[6];

	if (mjd == last_mjd) {
	    *lsn = last_lsn;
	    *rsn = last_rsn;
	    if (bsn) *bsn = last_bsn;
	    return;
	}

	vsop87(mjd, SUN, 0.0, ret);	/* full precision earth pos */

	*lsn = ret[0] - PI;		/* revert to sun pos */
	range (lsn, 2*PI);		/* normalise */

	last_lsn = *lsn;		/* memorise */
	last_rsn = *rsn = ret[2];
	last_bsn = -ret[1];
	last_mjd = mjd;

	if (bsn) *bsn = last_bsn;	/* assign only if non-NULL pointer */
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: sun.c,v $ $Date: 1997/05/19 18:21:42 $ $Revision: 1.1 $ $Name:  $"};
