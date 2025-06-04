/* stubs to support xephem references */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern double deltat (double Mjd);
extern double atod P_((char *buf));

void
pm_set (int p)
{
}

void
xe_msg (char *msg, int app_modal)
{
	fprintf (stderr, "%s\n", msg);
}

FILE *
fopenh (name, how)
char *name;
char *how;
{
	return (fopen (name, how));
}

int
existsh (name)
char *name;
{
	FILE *fp = fopen (name, "r");

	if (fp) {
	    fclose (fp);
	    return (0);
	}
	return (-1);
}


/* given an mjd, return it modified for terrestial dynamical time */
/* N.B. this is here just to avoid pulling in pref_get() in libastro/aux.c */
double
mm_mjed (np)
Now *np;
{
        return (mjd + deltat(mjd)/86400.0);
}

char *
syserrstr ()
{
	return (strerror(errno));
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: stubs.c,v $ $Date: 1999/03/23 18:51:17 $ $Revision: 1.2 $ $Name:  $"};
