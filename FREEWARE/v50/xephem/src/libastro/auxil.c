/* aux functions just to be able to leave the rest like those in xephem.
 */

#include <stdio.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

int
pref_get (p)
Preferences p;
{
	int pval;

	switch (p) {

	case PREF_EQUATORIAL:
	    pval = PREF_TOPO;
	    break;

	case PREF_DATE_FORMAT:
	    pval = PREF_MDY;
	    break;

	default: printf ("Unknown pref: %d\n", p); exit(1);

	}

	return (pval);
}

/* given an mjd, return it modified for terrestial dynamical time */
double
mm_mjed (np)
Now *np;
{
	return (mjd + deltat(mjd)/86400.0);
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: auxil.c,v $ $Date: 1998/02/26 04:51:18 $ $Revision: 1.1 $ $Name:  $"};
