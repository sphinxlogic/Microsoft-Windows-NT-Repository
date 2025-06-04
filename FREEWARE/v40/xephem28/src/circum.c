/* given a Now and an Obj with the object definition portion filled in,
 * fill in the sky position (s_*) portions.
 */

#include <stdio.h>
#include <math.h>
#if defined(__STDC__)
#include <stdlib.h>
#endif

#include "P_.h"
#include "astro.h"
#include "circum.h"
#include "preferences.h"

extern int obj_earthsat P_((Now *np, Obj *op));
extern void now_lst P_((Now *np, double *lst));
extern void hg_mag P_((double h, double g, double rp, double rho, double rsn,
    double *mp));
extern void gk_mag P_((double g, double k, double rp, double rho, double *mp));

static int obj_planet P_((Now *np, Obj *op));
static int obj_fixed P_((Now *np, Obj *op));
static int obj_elliptical P_((Now *np, Obj *op));
static int obj_hyperbolic P_((Now *np, Obj *op));
static int obj_parabolic P_((Now *np, Obj *op));
static int sun_cir P_((Now *np, Obj *op));
static int moon_cir P_((Now *np, Obj *op));
static void cir_sky P_((Now *np, double lpd, double psi, double rp, double rho,
    double lam, double bet, double lsn, double rsn, Obj *op));
static void elongation P_((double lam, double bet, double lsn, double *el));

/* given a Now and an Obj, fill in the approprirate s_* fields within Obj.
 * return 0 if all ok, else -1.
 */
int
obj_cir (np, op)
Now *np;
Obj *op;
{
	switch (op->type) {
	case FIXED:	 return (obj_fixed (np, op));
	case ELLIPTICAL: return (obj_elliptical (np, op));
	case HYPERBOLIC: return (obj_hyperbolic (np, op));
	case PARABOLIC:  return (obj_parabolic (np, op));
	case EARTHSAT:   return (obj_earthsat (np, op));
	case PLANET:     return (obj_planet (np, op));
	default:
	    printf ("obj_cir() called with type %d\n", op->type);
	    exit(1);
	    return (-1);	/* just for lint */
	}
}

static int
obj_planet (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double lpd, psi;	/* heliocentric ecliptic long and lat */
	double rp;		/* dist from sun */
	double rho;		/* dist from earth */
	double lam, bet;	/* geocentric ecliptic long and lat */
	double dia, mag;	/* angular diameter at 1 AU and magnitude */
	double f;		/* fractional phase from earth */
	int p;

	/* validate code and check for a few special cases */
	p = op->pl.code;
	if (p < 0 || p > MOON) {
	    printf ("unknown planet code: %d \n", p);
	    exit(1);
	}
	else if (p == SUN)
	    return (sun_cir (np, op));
	else if (p == MOON)
	    return (moon_cir (np, op));


	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjd, &lsn, &rsn);

	/* find helio long/lat; sun/planet and earth/plant dist; ecliptic
	 * long/lat; diameter and mag.
	 */
	plans(mjd, p, &lpd, &psi, &rp, &rho, &lam, &bet, &dia, &mag);

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and angular size */
	/* original Duffet-Smith code:
	f = 0.25 * (((rp+rho)*(rp+rho) - rsn*rsn)/(rp*rho));
	op->s_mag = MAGSCALE * (mag + 5.0*log(rp*rho/sqrt(f))/log(10.0));
	*/
	/* This one makes good sense and checks better; it uses a model which
	 * sqaures the phase influence.
	 */
	f = op->s_phase ? 5*log10(rp*rho) - 5*log10(op->s_phase/100) : -100;
	op->s_mag = (int)floor(MAGSCALE*(mag + f) + 0.5);

	op->s_size = dia/rho + 0.5;

	return (0);
}

static int
obj_fixed (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun, dist from sn to earth*/
	double lam, bet;	/* geocentric ecliptic long and lat */
	double lst;		/* local sidereal time */
	double ha;		/* local hour angle */
	double el;		/* elongation */
	double alt, az;		/* current alt, az */
	double ra, dec;		/* ra and dec at epoch of date */

	/* set ra/dec to their values at epoch of date */
	ra = op->f_RA;
	dec = op->f_dec;
	precess (op->f_epoch, mjd, &ra, &dec);

	/* set s_ra/dec at desired epoch.  */
	if (epoch == EOD) {
	    op->s_ra = ra;
	    op->s_dec = dec;
	} else {
	    /* want a certain epoch -- if it's not what the database is at
	     * we change the original to save time next time assuming the
	     * user is likely to stick with this for a while.
	     */
	    if ((float)epoch != op->f_epoch) {
		double tra = op->f_RA, tdec = op->f_dec;
		float tepoch = epoch;	/* compare to float precision */
		precess (op->f_epoch, tepoch, &tra, &tdec);
		op->f_epoch = tepoch;
		op->f_RA = tra;
		op->f_dec = tdec;
	    }
	    op->s_ra = op->f_RA;
	    op->s_dec = op->f_dec;
	}

	/* convert equitorial ra/dec to geocentric ecliptic lat/long */
	eq_ecl (mjd, ra, dec, &bet, &lam);

	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjd, &lsn, &rsn);

	/* compute elongation from ecliptic long/lat and sun geocentric long */
	elongation (lam, bet, lsn, &el);
	el = raddeg(el);
	op->s_elong = el;

	/* these are really the same fields ...
	op->s_mag = op->f_mag;
	op->s_size = op->f_size;
	*/

	/* alt, az: correct for refraction; use eod ra/dec. */
	now_lst (np, &lst);
	ha = hrrad(lst) - ra;
	hadec_aa (lat, ha, dec, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;

	return (0);
}

/* compute sky circumstances of an object in heliocentric elliptic orbit at *np.
 */
static int
obj_elliptical (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double dt;		/* light travel time to object */
	double lg;		/* helio long of earth */
	double nu, ea;		/* true anomaly and eccentric anomaly */
	double ma;		/* mean anomaly */
	double rp=0;		/* distance from the sun */
	double lo, slo, clo;	/* angle from ascending node */
	double inc;		/* inclination */
	double psi=0;		/* heliocentric latitude */
	double spsi=0, cpsi=0;	/* trig of heliocentric latitude */
	double lpd; 		/* heliocentric longitude */
	double rho=0;		/* distance from the Earth */
	double om;		/* arg of perihelion */
	double Om;		/* long of ascending node. */
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double e;		/* fast eccentricity */
	double ll=0, sll, cll;	/* helio angle between object and earth */
	double mag;		/* magnitude */
	double rpd=0;
	double y;
	int pass;

	/* find location of earth from sun now */
	sunpos (mjd, &lsn, &rsn);
	lg = lsn + PI;

	/* faster access to eccentricty */
	e = op->e_e;

	/* mean daily motion is optional -- fill in from period if 0 */
	if (op->e_n == 0.0) {
	    double a = op->e_a;
	    op->e_n = 0.9856076686/sqrt(a*a*a);
	}

	/* correct for light time by computing position at time mjd, then
	 *   again at mjd-dt, where
	 *   dt = time it takes light to travel earth-object distance.
	 * this is basically the same code as pelement() and plans()
	 *   combined and simplified for the special case of osculating
	 *   (unperturbed) elements. we have added reduction of elements using
	 *   reduce_elements().
	 */
	dt = 0;
	for (pass = pref_get(PREF_ALGO)==PREF_ACCURATE ? 0 : 1; pass<2; pass++){

	    reduce_elements (op->e_epoch, mjd-dt, degrad(op->e_inc),
			    degrad (op->e_om), degrad (op->e_Om),
			    &inc, &om, &Om);

	    ma = degrad (op->e_M + (mjd - op->e_cepoch - dt) * op->e_n);
	    anomaly (ma, e, &nu, &ea);
	    rp = op->e_a * (1-e*e) / (1+e*cos(nu));
	    lo = nu + om;
	    slo = sin(lo);
	    clo = cos(lo);
	    spsi = slo*sin(inc);
	    y = slo*cos(inc);
	    psi = asin(spsi);
	    lpd = atan(y/clo)+Om;
	    if (clo<0) lpd += PI;
	    range (&lpd, 2*PI);
	    cpsi = cos(psi);
	    rpd = rp*cpsi;
	    ll = lpd-lg;
	    rho = sqrt(rsn*rsn+rp*rp-2*rsn*rp*cpsi*cos(ll));

	    dt = rho*LTAU/3600.0/24.0;	/* light travel time, in days / AU */
	}

	/* compute sin and cos of ll */
	sll = sin(ll);
	cll = cos(ll);

	/* find geocentric ecliptic longitude and latitude */
	if (rpd < rsn)
	    lam = atan(-1*rpd*sll/(rsn-rpd*cll))+lg+PI;
	else
	    lam = atan(rsn*sll/(rpd-rsn*cll))+lpd;
	range (&lam, 2*PI);
	bet = atan(rpd*spsi*sin(lam-lpd)/(cpsi*rsn*sll));

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	if (op->e_mag.whichm == MAG_HG) {
	    /* the H and G parameters from the Astro. Almanac.
	     */
	    hg_mag (op->e_mag.m1, op->e_mag.m2, rp, rho, rsn, &mag);
	} else {
	    /* the g/k model of comets */
	    gk_mag (op->e_mag.m1, op->e_mag.m2, rp, rho, &mag);
	}
	op->s_mag = mag * MAGSCALE;
	op->s_size = op->e_size / rho;

	return (0);
}

/* compute sky circumstances of an object in heliocentric hyperbolic orbit.
 */
static int
obj_hyperbolic (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double dt;		/* light travel time to object */
	double lg;		/* helio long of earth */
	double nu, ea;		/* true anomaly and eccentric anomaly */
	double ma;		/* mean anomaly */
	double rp=0;		/* distance from the sun */
	double lo, slo, clo;	/* angle from ascending node */
	double inc;		/* inclination */
	double psi=0;		/* heliocentric latitude */
	double spsi=0, cpsi=0;	/* trig of heliocentric latitude */
	double lpd; 		/* heliocentric longitude */
	double rho=0;		/* distance from the Earth */
	double om;		/* arg of perihelion */
	double Om;		/* long of ascending node. */
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double e;		/* fast eccentricity */
	double ll=0, sll, cll;	/* helio angle between object and earth */
	double n;		/* mean daily motion */
	double mag;		/* magnitude */
	double a;		/* mean distance */
	double rpd=0;
	double y;
	int pass;

	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjd, &lsn, &rsn);

	lg = lsn + PI;
	e = op->h_e;
	a = op->h_qp/(e - 1.0);
	n = .98563/sqrt(a*a*a);

	/* correct for light time by computing position at time mjd, then
	 *   again at mjd-dt, where
	 *   dt = time it takes light to travel earth-object distance.
	 */
	dt = 0;
	for (pass = pref_get(PREF_ALGO)==PREF_ACCURATE ? 0 : 1; pass<2; pass++){

	    reduce_elements (op->h_epoch, mjd-dt, degrad(op->h_inc),
			    degrad (op->h_om), degrad (op->h_Om),
			    &inc, &om, &Om);

	    ma = degrad ((mjd - op->h_ep - dt) * n);
	    anomaly (ma, e, &nu, &ea);
	    rp = a * (e*e-1.0) / (1.0+e*cos(nu));
	    lo = nu + om;
	    slo = sin(lo);
	    clo = cos(lo);
	    spsi = slo*sin(inc);
	    y = slo*cos(inc);
	    psi = asin(spsi);
	    lpd = atan(y/clo)+Om;
	    if (clo<0) lpd += PI;
	    range (&lpd, 2*PI);
	    cpsi = cos(psi);
	    rpd = rp*cpsi;
	    ll = lpd-lg;
	    rho = sqrt(rsn*rsn+rp*rp-2*rsn*rp*cpsi*cos(ll));

	    dt = rho*5.775518e-3;	/* light travel time, in days */
	}

	/* compute sin and cos of ll */
	sll = sin(ll);
	cll = cos(ll);

	/* find geocentric ecliptic longitude and latitude */
	if (rpd < rsn)
	    lam = atan(-1*rpd*sll/(rsn-rpd*cll))+lg+PI;
	else
	    lam = atan(rsn*sll/(rpd-rsn*cll))+lpd;
	range (&lam, 2*PI);
	bet = atan(rpd*spsi*sin(lam-lpd)/(cpsi*rsn*sll));

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	gk_mag (op->h_g, op->h_k, rp, rho, &mag);
	op->s_mag = mag * MAGSCALE;
	op->s_size = op->h_size / rho;

	return (0);
}

/* compute sky circumstances of an object in heliocentric hyperbolic orbit.
 */
static int
obj_parabolic (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double mag;		/* magnitude */
	double inc, om, Om;
	double lpd, psi, rp, rho;
	double dt;
	int pass;

	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjd, &lsn, &rsn);

	/* two passes to correct lam and bet for light travel time.e_ */
	dt = 0.0;
	for (pass = pref_get(PREF_ALGO)==PREF_ACCURATE ? 0 : 1; pass<2; pass++){
	    reduce_elements (op->p_epoch, mjd-dt, degrad(op->p_inc),
		degrad(op->p_om), degrad(op->p_Om), &inc, &om, &Om);
	    comet (mjd-dt, op->p_ep, inc, om, op->p_qp, Om,
				    &lpd, &psi, &rp, &rho, &lam, &bet);
	    dt = rho*LTAU/3600.0/24.0;	/* light travel time, in days / AU */
	}

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	gk_mag (op->p_g, op->p_k, rp, rho, &mag);
	op->s_mag = mag * MAGSCALE;
	op->s_size = op->p_size / rho;

	return (0);
}

/* find sun's circumstances now.
 */
static int
sun_cir (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double deps, dpsi;	/* nutation on obliquity and longitude */
	double lst;		/* local sidereal time */
	double ehp;		/* angular diamter of earth from object */
	double ha;		/* hour angle */
	double ra, dec;		/* ra and dec now */
	double alt, az;		/* alt and az */
	double dhlong;

	sunpos (mjd, &lsn, &rsn);	/* sun's true ecliptic long and dist */
	nutation (mjd, &deps, &dpsi);	/* correct for nutation ... */
	lsn += dpsi;
	lsn -= degrad(20.4/3600);	/* and aberation */

	op->s_edist = rsn;
	op->s_sdist = 0.0;
	op->s_elong = 0.0;
	op->s_phase = 100.0;
	op->s_size = raddeg(4.65242e-3/rsn)*3600*2;
	op->s_mag = MAGSCALE * -26.8;	/* TODO */
	dhlong = lsn-PI;	/* geo- to helio- centric */
	range (&dhlong, 2*PI);
	op->s_hlong = dhlong;
	op->s_hlat = 0.0;


	/* convert geocentric ecliptic lat/long to equitorial ra/dec of date */
	ecl_eq (mjd, 0.0, lsn, &ra, &dec);

	/* find s_ra/dec at desired epoch */
	if (epoch == EOD) {
	    op->s_ra = ra;
	    op->s_dec = dec;
	} else {
	    double tra = ra, tdec = dec;
	    precess (mjd, epoch, &tra, &tdec);
	    op->s_ra = tra;
	    op->s_dec = tdec;
	}

	/* find alt/az based on unprecessed ra/dec */
	now_lst (np, &lst);
	ha = hrrad(lst) - ra;
	ehp = (2.0 * ERAD / MAU) / op->s_edist;
	ta_par (ha, dec, lat, elev, ehp, &ha, &dec);
	hadec_aa (lat, ha, dec, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;
	return (0);
}

/* find moon's circumstances now.
 */
static int
moon_cir (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double deps, dpsi;	/* nutation on obliquity and longitude */
	double lst;		/* local sidereal time */
	double ehp;		/* angular diamter of earth from object */
	double ha;		/* hour angle */
	double ra, dec;		/* ra and dec now */
	double alt, az;		/* alt and az */
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double edistau;		/* earth-moon dist, in au */
	double el;		/* elongation, rads east */
	double ms;		/* sun's mean anomaly */
	double md;		/* moon's mean anomaly */
	double i;

	moon (mjd, &lam, &bet, &ehp, &ms, &md);	/* moon's true ecliptic loc */
	nutation (mjd, &deps, &dpsi);	/* correct for nutation */
	lam += dpsi;
	range (&lam, 2*PI);

	op->s_edist = (ERAD/MAU)/sin(ehp);	/* earth-moon dist, want au */
	op->s_size = 3600*31.22512*sin(ehp);/* moon angular dia, seconds */
	op->s_hlong = lam;		/* save geo in helio fields */
	op->s_hlat = bet;

	/* convert geocentric ecliptic lat/long to equitorial ra/dec of date */
	ecl_eq (mjd, bet, lam, &ra, &dec);

	/* find s_ra/dec at desired epoch */
	if (epoch == EOD) {
	    op->s_ra = ra;
	    op->s_dec = dec;
	} else {
	    double tra = ra, tdec = dec;
	    precess (mjd, epoch, &tra, &tdec);
	    op->s_ra = tra;
	    op->s_dec = tdec;
	}

	sunpos (mjd, &lsn, &rsn);
	range (&lsn, 2*PI);

	/* find angular separation from sun */
	elongation (lam, bet, lsn, &el);
	op->s_elong = raddeg(el);		/* want degrees */

	/* solve triangle of earth, sun, and elongation for moon-sun dist */
	edistau = op->s_edist;
	op->s_sdist= sqrt (edistau*edistau + rsn*rsn - 2.0*edistau*rsn*cos(el));

	/* TODO: improve mag; this is based on a flat moon model. */
	op->s_mag = MAGSCALE * 
	    (-12.7 + 2.5*(log10(PI) - log10(PI/2*(1+1.e-6-cos(el)))));

	/* find phase -- allow for projection effects */
	i = 0.1468*sin(el)*(1 - 0.0549*sin(md))/(1 - 0.0167*sin(ms));
	op->s_phase = (1+cos(PI-el-degrad(i)))/2*100;

	/* show topocentric alt/az by correcting ra/dec for parallax 
	 * as well as refraction.
	 * use unprecessed ra/dec.
	 */
	now_lst (np, &lst);
	ha = hrrad(lst) - ra;
	ta_par (ha, dec, lat, elev, ehp, &ha, &dec);
	hadec_aa (lat, ha, dec, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;

	return (0);
}

/* fill in all of op->s_* stuff except s_size and s_mag */
static void
cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op)
Now *np;
double lpd, psi;	/* heliocentric ecliptic long and lat */
double rp;		/* dist from sun */
double rho;		/* dist from earth */
double lam, bet;	/* true geocentric ecliptic long and lat */
double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
Obj *op;
{
	double a;		/* geoc angle between object and sun */
	double ra, dec;		/* ra and dec at epoch of date */
	double el;		/* elongation */
	double lst;		/* local sidereal time */
	double ehp;		/* ehp: angular dia of earth from body */
	double ha;		/* local hour angle */
	double f;		/* fractional phase from earth */
	double alt, az;		/* current alt, az */
	double deps, dpsi;	/* nutation on obliquity and longitude */

	/* correct for nutation ... */
	nutation (mjd, &deps, &dpsi);
	lam += dpsi;

	/* and correct for 20.4" aberation */
	a = lsn - lam;
	lam -= degrad(20.4/3600)*cos(a)/cos(bet);
	bet -= degrad(20.4/3600)*sin(a)*sin(bet);

	/* convert geocentric ecliptic lat/long to equitorial ra/dec of date */
	ecl_eq (mjd, bet, lam, &ra, &dec);

	/* set s_ra/s_dec to that of desired display epoch. */
	if (epoch == EOD) {
	    op->s_ra = ra;
	    op->s_dec = dec;
	} else {
	    double tra = ra, tdec = dec;
	    precess (mjd, epoch, &tra, &tdec);
	    op->s_ra = tra;
	    op->s_dec = tdec;
	}

	/* set earth/planet and sun/planet distance */
	op->s_edist = rho;
	op->s_sdist = rp;

	/* compute elongation and phase */
	elongation (lam, bet, lsn, &el);
	el = raddeg(el);
	op->s_elong = el;
	f = 0.25 * ((rp+rho)*(rp+rho) - rsn*rsn)/(rp*rho);
	op->s_phase = f*100.0; /* percent */

	/* set heliocentric long/lat */
	op->s_hlong = lpd;
	op->s_hlat = psi;

	/* alt, az: correct for parallax and refraction; use eod ra/dec. */
	now_lst (np, &lst);
	ha = hrrad(lst) - ra;
	ehp = (2.0*6378.0/146.0e6) / rho;
	ta_par (ha, dec, lat, elev, ehp, &ha, &dec);
	hadec_aa (lat, ha, dec, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;
}

/* given geocentric ecliptic longitude and latitude, lam and bet, of some object
 * and the longitude of the sun, lsn, find the elongation, el. this is the
 * actual angular separation of the object from the sun, not just the difference
 * in the longitude. the sign, however, IS set simply as a test on longitude
 * such that el will be >0 for an evening object <0 for a morning object.
 * to understand the test for el sign, draw a graph with lam going from 0-2*PI
 *   down the vertical axis, lsn going from 0-2*PI across the hor axis. then
 *   define the diagonal regions bounded by the lines lam=lsn+PI, lam=lsn and
 *   lam=lsn-PI. the "morning" regions are any values to the lower left of the
 *   first line and bounded within the second pair of lines.
 * all angles in radians.
 */
static void
elongation (lam, bet, lsn, el)
double lam, bet, lsn;
double *el;
{
	*el = acos(cos(bet)*cos(lam-lsn));
	if (lam>lsn+PI || (lam>lsn-PI && lam<lsn)) *el = - *el;
}
