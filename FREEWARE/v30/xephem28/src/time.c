/* get the time from the os.
 */

#include <stdio.h>
#include <time.h>

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern int tz_fromsys P_((Now *np));

static long c0;
static double mjd0;

/* save current mjd and corresponding system clock for use by inc_mjd().
 * this establishes the base correspondence between the mjd and system clock.
 */
void
set_t0 (np)
Now *np;
{
	mjd0 = mjd;
	c0 = time (NULL);
}

/* fill in n_mjd from the system clock.
 * try to set timezone name and offset too but only if __STDC__ -- there's too
 *   many conflicting ways to do it otherwise.
 */
void
time_fromsys (np)
Now *np;
{
#if defined(__STDC__)
	time_t t;
#else
	long t;
#endif

	t = time(NULL);

	/* t is seconds since 00:00:00 1/1/1970 UTC on UNIX systems;
	 * mjd was 25567.5 then.
	 */
#if defined(VMS)
	/* VMS returns t in seconds since 00:00:00 1/1/1970 Local Time
	 * so we need to add the timezone offset to get UTC.
	 * Don't need to worry about 'set_t0' and 'inc_mjd' because
	 * they only deal in relative times.
	 * this change courtesy Stephen Hirsch <oahirsch@southpower.co.nz>
	 */
	mjd = (25567.5 + t/3600.0/24.0) + (tz/24.0);
#else
	mjd = 25567.5 + t/3600.0/24.0;
#endif

	(void) tz_fromsys(np);
}

/* given the mjd within np, try to figure the timezone from the os.
 * return 0 if it looks like it worked, else -1.
 */
int
tz_fromsys (np)
Now *np;
{
#if defined(__STDC__)
	struct tm *gtmp;
	time_t t;

	t = (mjd - 25567.5) * (3600.0*24.0) + 0.5;

	/* try to find out timezone by comparing local with UTC time.
	 * GNU doesn't have difftime() so we do time math with doubles.
	 */
	gtmp = gmtime (&t);
	if (gtmp) {
	    double gmkt, lmkt;
	    struct tm *ltmp;

	    gtmp->tm_isdst = 0;	/* _should_ always be 0 already */
	    gmkt = (double) mktime (gtmp);

	    ltmp = localtime (&t);
	    ltmp->tm_isdst = 0;	/* let mktime() figure out zone */
	    lmkt = (double) mktime (ltmp);

	    tz = (gmkt - lmkt) / 3600.0;
	    (void) strftime (tznm, sizeof(tznm)-1, "%Z", ltmp);
	    return (0);
	} else
	    return (-1);
#else
	return (-1);
#endif
}

void
inc_mjd (np, inc, rtcflag)
Now *np;
double inc;	/* hours to increment mjd */
int rtcflag;	/* just increment mjd to be the current time */
{
	if (rtcflag) {
	    long c = time (NULL);
	    mjd = mjd0 + (c - c0)/SPD;
	} else
	    mjd += inc/24.0;

	/* round to nearest whole second.
	 * without this, you can get fractional days so close to .5 but
	 * not quite there that mjd_hr() can return 24.0
	 */
	rnd_second (&mjd);
}
