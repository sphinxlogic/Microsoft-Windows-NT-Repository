/* find rise and set circumstances, ie, riset_cir() and related functions. */

#include <stdio.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif

#include "P_.h"
#include "astro.h"
#include "circum.h"

extern int obj_cir P_((Now *np, Obj *op));
extern void now_lst P_((Now *np, double *lst));


#define	TMACC	(10./3600.)	/* convergence accuracy, hours */


static void stationary_riset P_((Obj *op, Now *np, double dis, double *lstr,
    double *lsts, double *lstt, double *azr, double *azs, double *altt,
    int *status));
static void transit P_((double r, double d, Now *np, double *lstt,
    double *altt));
static void e_riset_cir P_((Now *np, Obj *op, double dis, RiseSet *rp));
static int find_zero P_((Now *np, Obj *op, double dis, double t0, double t1,
    double a0, double a1, double *tp, double *azp));
static int find_max P_((Now *np, Obj *op, double tr, double ts, double *tp,
    double *alp));

/* find where and when an object, op, will rise and set and
 *   it's transit circumstances. all times are local, angles rads e of n.
 * dis is the angle down from an ideal horizon, in rads (see riset()).
 */
void
riset_cir (np, op, dis, rp)
Now *np;
Obj *op;
double dis;
RiseSet *rp;
{
#define	MAXPASSES	6
	double lstr, lsts, lstt; /* local sidereal times of rising/setting */
	double lnoon;		/* mjd of local noon */
	double x;		/* discarded tmp value */
	Now n;			/* copy to move time around */
	Obj o;			/* copy to get circumstances at n */
	double lst;		/* lst at local noon */
	double diff, lastdiff;	/* iterative improvement to mjd0 */
	int pass;
	int rss;

	/* work with local copies so we can move the time around */
	n = *np;
	o = *op;

	/* fast Earth satellites need a different approach.
	 * "fast" here is pretty arbitrary -- just too fast to work with the
	 * iterative approach based on refining the times for a "fixed" object.
	 */
	if (op->type == EARTHSAT && op->es_n > FAST_SAT_RPD) {
	    e_riset_cir (&n, &o, dis, rp);
	    return;
	}

	/* start the iteration at local noon */
	lnoon = mjd_day(mjd - tz/24.0) + (12.0 + tz)/24.0;

	/* assume no problems initially */
	rp->rs_flags = 0;

	/* first approximation is to find rise/set times of a fixed object
	 * at the current epoch in its position at local noon.
	 */
	n.n_mjd = lnoon;
	n.n_epoch = EOD;
	now_lst (&n, &lst);	/* lst at local noon */
	stationary_riset (&o, &n, dis, &lstr, &lsts, &lstt, &x, &x, &x, &rss);

    chkrss:	/* come here to check rss status and control next step */
	switch (rss) {
	case  0:  break;
	case  1: rp->rs_flags = RS_NEVERUP; return;
	case -1: rp->rs_flags = RS_CIRCUMPOLAR; goto dotransit;
	default: rp->rs_flags = RS_ERROR; return;
	}

	/* find a better approximation to the rising circumstances based on
	 * more passes, each using a "fixed" object at the location at
	 * previous approximation of the rise time.
	 */
	lastdiff = 1000.0;
	for (pass = 1; pass < MAXPASSES; pass++) {
	    diff = (lstr - lst)*SIDRATE; /* next guess at rise time wrt noon */
	    if (diff > 12.0)
		diff -= 24.0*SIDRATE;	/* not tomorrow, today */
	    else if (diff < -12.0)
		diff += 24.0*SIDRATE;	/* not yesterday, today */
	    n.n_mjd = lnoon + diff/24.0;/* next guess at mjd of rise */
	    stationary_riset (&o, &n, dis, &lstr, &x, &x, &rp->rs_riseaz,
								&x, &x, &rss);
	    if (rss != 0) goto chkrss;
	    if (fabs (diff - lastdiff) < TMACC)
		break;
	    lastdiff = diff;
	}
	if (pass == MAXPASSES)
	    rp->rs_flags |= RS_NORISE;	/* didn't converge - no rise today */
	else {
	    rp->rs_risetm = 12.0 + diff;
	    if (!is_planet(op, MOON) &&
		    (rp->rs_risetm <= 24.0*(1.0-SIDRATE)
			|| rp->rs_risetm >= 24.0*SIDRATE))
		rp->rs_flags |= RS_2RISES;
	}

	/* find a better approximation to the setting circumstances based on
	 * more passes, each using a "fixed" object at the location at
	 * previous approximation of the set time.
	 */
	lastdiff = 1000.0;
	for (pass = 1; pass < MAXPASSES; pass++) {
	    diff = (lsts - lst)*SIDRATE;/* next guess at set time wrt noon */
	    if (diff > 12.0)
		diff -= 24.0*SIDRATE;	/* not tomorrow, today */
	    else if (diff < -12.0)
		diff += 24.0*SIDRATE;	/* not yesterday, today */
	    n.n_mjd = lnoon + diff/24.0;/* next guess at mjd of set */
	    stationary_riset (&o, &n, dis, &x, &lsts, &x, &x,
						    &rp->rs_setaz, &x, &rss);
	    if (rss != 0) goto chkrss;
	    if (fabs (diff - lastdiff) < TMACC)
		break;
	    lastdiff = diff;
	}
	if (pass == MAXPASSES)
	    rp->rs_flags |= RS_NOSET;	/* didn't converge - no set today */
	else {
	    rp->rs_settm = 12.0 + diff;
	    if (!is_planet(op, MOON) &&
		    (rp->rs_settm <= 24.0*(1.0-SIDRATE)
			|| rp->rs_settm >= 24.0*SIDRATE))
		rp->rs_flags |= RS_2SETS;
	}

    dotransit:
	/* find a better approximation to the transit circumstances based on
	 * more passes, each using a "fixed" object at the location at
	 * previous approximation of the transit time.
	 */
	lastdiff = 1000.0;
	for (pass = 1; pass < MAXPASSES; pass++) {
	    diff = (lstt - lst)*SIDRATE;/*next guess at transit time wrt noon*/
	    if (diff > 12.0)
		diff -= 24.0*SIDRATE;	/* not tomorrow, today */
	    else if (diff < -12.0)
		diff += 24.0*SIDRATE;	/* not yesterday, today */
	    n.n_mjd = lnoon + diff/24.0;/* next guess at mjd of transit */
	    stationary_riset (&o, &n, dis, &x, &x, &lstt, &x, &x,
							&rp->rs_tranalt, &rss);
	    if (fabs (diff - lastdiff) < TMACC)
		break;
	    lastdiff = diff;
	}
	if (pass == MAXPASSES)
	    rp->rs_flags |= RS_NOTRANS;	/* didn't converge - no transit today */
	else {
	    rp->rs_trantm = 12.0 + diff;
	    if (!is_planet(op, MOON) &&
		    (rp->rs_trantm <= 24.0*(1.0-SIDRATE)
			|| rp->rs_trantm >= 24.0*SIDRATE))
		rp->rs_flags |= RS_2TRANS;
	}
}

/* find local times when sun is dis rads below horizon.
 */
void
twilight_cir (np, dis, dawn, dusk, status)
Now *np;
double dis;
double *dawn, *dusk;
int *status;
{
	RiseSet rs;
	Obj o;

	o.type = PLANET;
	o.pl.code = SUN;
	riset_cir (np, &o, dis, &rs);
	*dawn = rs.rs_risetm;
	*dusk = rs.rs_settm;
	*status = rs.rs_flags;
}

/* find the local rise/set/transit circumstances of a fixed object, *op.
 * use lp to decide the day and the location circumstances.
 * fill *status is have any problems.
 */
static void
stationary_riset (op, np, dis, lstr, lsts, lstt, azr, azs, altt, status)
Obj *op;
Now *np;
double dis;
double *lstr, *lsts, *lstt;
double *azr, *azs, *altt;
int *status;
{
	/* find object op's topocentric ra/dec at np..
	 * (this must include parallax if it's in the solar system).
	 */
	if (obj_cir (np, op) < 0) {
	    printf ("stationary_riset: can't get object loc\n");
	    exit (1);
	}
	
	if (is_ssobj(op)) {
	    /* obj_cir() gives geocentric ra/dec; we need to account for
	     * parallax in solar system objects to get topocentric ra/dec.
	     */
	    double tra, tdec;
	    double ehp, lst, ha;
	    ehp = asin ((ERAD/MAU)/op->s_edist);
	    now_lst (np, &lst);
	    ha = hrrad(lst) - op->s_ra;
	    ta_par (ha, op->s_dec, lat, elev, ehp, &ha, &tdec);
	    tra = hrrad(lst) - ha;
	    range (&tra, 2*PI);
	    op->s_ra = tra;
	    op->s_dec = tdec;
	}

	riset (op->s_ra, op->s_dec, lat, dis, lstr, lsts, azr, azs, status);
	transit (op->s_ra, op->s_dec, np, lstt, altt);
}


/* find when and how hi a fixed object at (r,d) is when it transits. */
static void
transit (r, d, np, lstt, altt)
double r, d;	/* ra and dec, rads */
Now *np;	/* for refraction info */
double *lstt;	/* local sidereal time of transit */
double *altt;	/* local, refracted, altitude at time of transit */
{
	*lstt = radhr(r);
	*altt = PI/2 - lat + d;
	if (*altt > PI/2)
	    *altt = PI - *altt;
	refract (pressure, temp, *altt, altt);
}

/* find where and when a fast-moving Earth satellite, op, will rise and set and
 *   it's transit circumstances. all times are local, angles rads e of n.
 * dis is the angle down from an ideal horizon, in rads (see riset()).
 * idea is to walk forward in time looking for alt-dis==0 crossings.
 * initial time step is a few degrees (based on average daily motion).
 * we stop as soon as we see both a rise and set but never longer than one day.
 * N.B. we assume *np and *op are working copies we can mess up.
 * N.B. obj_cir() for earth satellites includes refraction; we must unrefract
 *   before comparing with dis because dis includes any desired refraction too.
 */
static void
e_riset_cir (np, op, dis, rp)
Now *np;
Obj *op;
double dis;
RiseSet *rp;
{
#define	DEGSTEP	5		/* time step is about this many degrees */
	int steps;		/* max number of time steps */
	double dt;		/* time change per step, days */
	double t0, t1;		/* current and next mjd values */
	double tr, ts;		/* mjd times of rise and set */
	double a0, a1;		/* altitude at t0 and t1 */
	int rise, set;		/* flags to check when we find these events */
	int i;

	dt = DEGSTEP * (1.0/360.0/op->es_n);
	steps = (int)(1.0/dt);
	rise = set = 0;
	rp->rs_flags = 0;

	if (obj_cir (np, op) < 0) {
	    rp->rs_flags |= RS_ERROR;
	    return;
	}

	t0 = mjd;
	a0 = op->s_alt;
	unrefract (pressure, temp, a0, &a0);
	a0 += dis;

	for (i = 0; i < steps && (!rise || !set); i++) {
	    mjd = t1 = t0 + dt;
	    if (obj_cir (np, op) < 0) {
		rp->rs_flags |= RS_ERROR;
		return;
	    }
	    a1 = op->s_alt;
	    unrefract (pressure, temp, a1, &a1);
	    a1 += dis;

	    if (a0 < 0 && a1 > 0 && !rise) {
		/* found a rise event -- interpolate to refine */
		double az;
		if (find_zero (np, op, dis, t0, t1, a0, a1, &tr, &az) < 0) {
		    rp->rs_flags |= RS_ERROR;
		    return;
		}
		rp->rs_risetm = mjd_hr(tr - tz/24.0);
		rp->rs_riseaz = az;
		rise = 1;
	    } else if (a0 > 0 && a1 < 0 && !set) {
		/* found a setting event -- interpolate to refine */
		double az;
		if (find_zero (np, op, dis, t0, t1, a0, a1, &ts, &az) < 0) {
		    rp->rs_flags |= RS_ERROR;
		    return;
		}
		rp->rs_settm = mjd_hr(ts - tz/24.0);
		rp->rs_setaz = az;
		set = 1;
	    }

	    t0 = t1;
	    a0 = a1;
	}

	/* find time of maximum altitude, if we know both the rise and
	 * set times and the former occurs before the latter.
	 */
	if (rise && set && ts > tr) {
	    double tt, al;
	    if (find_max (np, op, tr, ts, &tt, &al) < 0) {
		rp->rs_flags |= RS_ERROR;
		return;
	    }
	    rp->rs_trantm = mjd_hr(tt - tz/24.0);
	    rp->rs_tranalt = al;
	} else
	    rp->rs_flags |= RS_NOTRANS;

	/* check for some bad conditions */
	if (!rise) {
	    if (a0 > 0)
		rp->rs_flags |= RS_CIRCUMPOLAR;
	    else
		rp->rs_flags |= RS_NORISE;
	}
	if (!set) {
	    if (a0 < 0)
		rp->rs_flags |= RS_NEVERUP;
	    else
		rp->rs_flags |= RS_NOSET;
	}
}

/* use secant method to search for time when unrefracted altitude is dis.
 * set *tp to time and *azp to the azimuth then.
 * return 0 if all ok, else -1.
 * N.B. we assume *np and *op are working copies we can mess up.
 */
static int
find_zero (np, op, dis, t0, t1, a0, a1, tp, azp)
Now *np;
Obj *op;
double dis;
double t0, t1;
double a0, a1;
double *tp, *azp;
{
	double t, da;

	do {
	    da = a1 - a0;
	    t = t1 - a1*(t1-t0)/da;
	    mjd = t;
	    if (obj_cir (np, op) < 0)
		return (-1);
	    t0 = t1;
	    t1 = t;
	    a0 = a1;
	    a1 = op->s_alt;
	    unrefract (pressure, temp, a1, &a1);
	    a1 += dis;
	} while (fabs(t1 - t0) >= TMACC/24.0);

	*tp = t;
	*azp = op->s_az;

	return (0);
}


/* use a simple binary search to find the time of max altitude between the
 * given rise and set times.
 * N.B. we assume *np and *op are working copies we can mess up.
 * N.B. we don't care about displacement or need to use unrefracted altitude
 *   because we just want a relative max. too, our caller can use *alp directly.
 * return 0 if ok, else -1.
 */
static int
find_max (np, op, tr, ts, tp, alp)
Now *np;
Obj *op;
double tr, ts;		/* times of rise and set */
double *tp, *alp;	/* time of max altitude, and that altitude */
{
	double dt, t, a;

	/* initial guess is in the center */
	mjd = t = (ts + tr)/2;
	if (obj_cir (np, op) < 0)
	    return (-1);
	a = op->s_alt;

	/* creep up to it */
	dt = TMACC/24.0 * 2.0;

	do {
	    mjd = t += dt;
	    if (obj_cir (np, op) < 0)
		return (-1);
	    if (op->s_alt < a)
		dt *= -.7;   /* turn around and go slower (.5 repeats prior) */
	    a = op->s_alt;
	} while (fabs(dt) >= TMACC/24.0);

	*tp = t;
	*alp = a;

	return (0);
}
