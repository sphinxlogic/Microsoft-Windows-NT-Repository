/* given a Now and an Obj with the object definition portion filled in,
 * fill in the sky position (s_*) portions.
 * calculation of positional coordinates reworked by
 * Michael Sternberg <sternberg@physik.tu-chemnitz.de>
 *  3/11/98: deflect was using op->s_hlong before being set in cir_pos().
 *  4/19/98: just edit a comment
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


static int obj_planet P_((Now *np, Obj *op));
static int obj_fixed P_((Now *np, Obj *op));
static int obj_elliptical P_((Now *np, Obj *op));
static int obj_hyperbolic P_((Now *np, Obj *op));
static int obj_parabolic P_((Now *np, Obj *op));
static int sun_cir P_((Now *np, Obj *op));
static int moon_cir P_((Now *np, Obj *op));
static void cir_sky P_((Now *np, double lpd, double psi, double rp, double *rho,
    double lam, double bet, double lsn, double rsn, Obj *op));
static void cir_pos P_((Now *np, double bet, double lam, double *rho, Obj *op));
static void elongation P_((double lam, double bet, double lsn, double *el));
static void deflect P_((double mjd1, double lpd, double psi, double rsn,
    double lsn, double rho, double *ra, double *dec));

/* given a Now and an Obj, fill in the approprirate s_* fields within Obj.
 * return 0 if all ok, else -1.
 */
int
obj_cir (np, op)
Now *np;
Obj *op;
{
	switch (op->o_type) {
	case FIXED:	 return (obj_fixed (np, op));
	case ELLIPTICAL: return (obj_elliptical (np, op));
	case HYPERBOLIC: return (obj_hyperbolic (np, op));
	case PARABOLIC:  return (obj_parabolic (np, op));
	case EARTHSAT:   return (obj_earthsat (np, op));
	case PLANET:     return (obj_planet (np, op));
	default:
	    printf ("obj_cir() called with type %d\n", op->o_type);
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
	p = op->pl.pl_code;
	if (p < 0 || p > MOON) {
	    printf ("unknown planet code: %d\n", p);
	    exit(1);
	}
	else if (p == SUN)
	    return (sun_cir (np, op));
	else if (p == MOON)
	    return (moon_cir (np, op));

	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjed, &lsn, &rsn, 0);

	/* find helio long/lat; sun/planet and earth/plant dist; ecliptic
	 * long/lat; diameter and mag.
	 */
	plans(mjed, p, &lpd, &psi, &rp, &rho, &lam, &bet, &dia, &mag);

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and angular size */
	f = op->s_phase ? 5*log10(rp*rho) - 5*log10(op->s_phase/100) : 100;
	set_smag (op, mag+f);
	op->s_size = dia/rho;

	return (0);
}

static int
obj_fixed (np, op)
Now *np;
Obj *op;
{
	double lsn, rsn;	/* true geoc lng of sun, dist from sn to earth*/
	double lam, bet;	/* geocentric ecliptic long and lat */
	double ha;		/* local hour angle */
	double el;		/* elongation */
	double alt, az;		/* current alt, az */
	double ra, dec;		/* ra and dec at epoch of date */
	double lst;

	if (epoch != EOD && (float)epoch != op->f_epoch) {
	    /* want a certain epoch -- if it's not what the database is at
	     * we change the original to save time next time assuming the
	     * user is likely to stick with this for a while.
	     */
	    double tra = op->f_RA, tdec = op->f_dec;
	    float tepoch = epoch;	/* compare to float precision */
	    precess (op->f_epoch, tepoch, &tra, &tdec);
	    op->f_epoch = tepoch;
	    op->f_RA = tra;
	    op->f_dec = tdec;
	}

	/* set ra/dec to astrometric @ epoch of date */
	ra = op->f_RA;
	dec = op->f_dec;
	precess (op->f_epoch, mjd, &ra, &dec);

	/* convert equatoreal ra/dec to mean geocentric ecliptic lat/long */
	eq_ecl (mjd, ra, dec, &bet, &lam);

	/* find solar ecliptical long.(mean equinox) and distance from earth */
	sunpos (mjed, &lsn, &rsn, NULL);

	/* allow for relativistic light bending near the sun */
	deflect (mjd, lam, bet, lsn, rsn, 1e10, &ra, &dec);

	/* TODO: correction for annual parallax would go here */

	/* correct EOD equatoreal for nutation/aberation to form apparent 
	 * geocentric
	 */
	nut_eq(mjd, &ra, &dec);
	ab_eq(mjd, lsn, &ra, &dec);
	op->s_gaera = ra;
	op->s_gaedec = dec;

	/* set s_ra/dec -- apparent if EOD else astrometric */
	if (epoch == EOD) {
	    op->s_ra = ra;
	    op->s_dec = dec;
	} else {
	    /* annual parallax at time mjd is to be added here, too, but
	     * technically in the frame of epoch (usually different from mjd)
	     */
	    op->s_ra = op->f_RA;	/* already precessed */
	    op->s_dec = op->f_dec;
	}

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
	double e_n;		/* mean daily motion */
	double rpd=0;
	double y;
	int pass;

	/* find location of earth from sun now */
	sunpos (mjed, &lsn, &rsn, 0);
	lg = lsn + PI;

	/* faster access to eccentricty */
	e = op->e_e;

	/* mean daily motion is derived fro mean distance */
	e_n = 0.9856076686/pow((double)op->e_a, 1.5);

	/* correct for light time by computing position at time mjd, then
	 *   again at mjd-dt, where
	 *   dt = time it takes light to travel earth-object distance.
	 */
	dt = 0;
	for (pass = 0; pass < 2; pass++) {

	    reduce_elements (op->e_epoch, mjd-dt, degrad(op->e_inc),
					degrad (op->e_om), degrad (op->e_Om),
					&inc, &om, &Om);

	    ma = degrad (op->e_M + (mjed - op->e_cepoch - dt) * e_n);
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
	cir_sky (np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	if (op->e_mag.whichm == MAG_HG) {
	    /* the H and G parameters from the Astro. Almanac.
	     */
	    hg_mag (op->e_mag.m1, op->e_mag.m2, rp, rho, rsn, &mag);
	} else {
	    /* the g/k model of comets */
	    gk_mag (op->e_mag.m1, op->e_mag.m2, rp, rho, &mag);
	}
	set_smag (op, mag);
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
	sunpos (mjed, &lsn, &rsn, 0);

	lg = lsn + PI;
	e = op->h_e;
	a = op->h_qp/(e - 1.0);
	n = .98563/sqrt(a*a*a);

	/* correct for light time by computing position at time mjd, then
	 *   again at mjd-dt, where
	 *   dt = time it takes light to travel earth-object distance.
	 */
	dt = 0;
	for (pass = 0; pass < 2; pass++) {

	    reduce_elements (op->h_epoch, mjd-dt, degrad(op->h_inc),
			    degrad (op->h_om), degrad (op->h_Om),
			    &inc, &om, &Om);

	    ma = degrad ((mjed - op->h_ep - dt) * n);
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
	cir_sky (np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	gk_mag (op->h_g, op->h_k, rp, rho, &mag);
	set_smag (op, mag);
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
	sunpos (mjed, &lsn, &rsn, 0);

	/* two passes to correct lam and bet for light travel time. */
	dt = 0.0;
	for (pass = 0; pass < 2; pass++) {
	    reduce_elements (op->p_epoch, mjd-dt, degrad(op->p_inc),
		degrad(op->p_om), degrad(op->p_Om), &inc, &om, &Om);
	    comet (mjed-dt, op->p_ep, inc, om, op->p_qp, Om,
				    &lpd, &psi, &rp, &rho, &lam, &bet);
	    dt = rho*LTAU/3600.0/24.0;	/* light travel time, in days / AU */
	}

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	gk_mag (op->p_g, op->p_k, rp, rho, &mag);
	set_smag (op, mag);
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
	double bsn;		/* true latitude beta of sun */
	double dhlong;

	sunpos (mjed, &lsn, &rsn, &bsn);/* sun's true coordinates; mean ecl. */

	op->s_sdist = 0.0;
	op->s_elong = 0.0;
	op->s_phase = 100.0;
	set_smag (op, -26.8);	/* TODO */
	dhlong = lsn-PI;	/* geo- to helio- centric */
	range (&dhlong, 2*PI);
	op->s_hlong = dhlong;
	op->s_hlat = -bsn;

	/* fill sun's ra/dec, alt/az in op */
	cir_pos (np, bsn, lsn, &rsn, op);
	op->s_edist = rsn;
	op->s_size = raddeg(4.65242e-3/rsn)*3600*2;

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
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double edistau;		/* earth-moon dist, in au */
	double el;		/* elongation, rads east */
	double ms;		/* sun's mean anomaly */
	double md;		/* moon's mean anomaly */
	double i;

	moon (mjed, &lam, &bet, &edistau, &ms, &md);	/* mean ecliptic & EOD*/
	sunpos (mjed, &lsn, &rsn, NULL);		/* mean ecliptic & EOD*/

	op->s_hlong = lam;			/* save geo in helio fields */
	op->s_hlat = bet;

	/* find angular separation from sun */
	elongation (lam, bet, lsn, &el);
	op->s_elong = raddeg(el);		/* want degrees */

	/* solve triangle of earth, sun, and elongation for moon-sun dist */
	op->s_sdist= sqrt (edistau*edistau + rsn*rsn - 2.0*edistau*rsn*cos(el));

	/* TODO: improve mag; this is based on a flat moon model. */
	set_smag (op, -12.7 + 2.5*(log10(PI) - log10(PI/2*(1+1.e-6-cos(el)))));

	/* find phase -- allow for projection effects */
	i = 0.1468*sin(el)*(1 - 0.0549*sin(md))/(1 - 0.0167*sin(ms));
	op->s_phase = (1+cos(PI-el-degrad(i)))/2*100;

	/* fill moon's ra/dec, alt/az in op and update for topo dist */
	cir_pos (np, bet, lam, &edistau, op);

	op->s_edist = edistau;
	op->s_size = 3600*2.0*raddeg(asin(MRAD/MAU/edistau));
						/* moon angular dia, seconds */

	return (0);
}

/* fill in all of op->s_* stuff except s_size and s_mag.
 * this is used for sol system objects (except sun and moon); never FIXED.
 */
static void
cir_sky (np, lpd, psi, rp, rho, lam, bet, lsn, rsn, op)
Now *np;
double lpd, psi;	/* heliocentric ecliptic long and lat */
double rp;		/* dist from sun */
double *rho;		/* dist from earth: in as geo, back as geo or topo */
double lam, bet;	/* true geocentric ecliptic long and lat */
double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
Obj *op;
{
	double el;		/* elongation */
	double f;		/* fractional phase from earth */

	/* compute elongation and phase */
	elongation (lam, bet, lsn, &el);
	el = raddeg(el);
	op->s_elong = el;
	f = 0.25 * ((rp+ *rho)*(rp+ *rho) - rsn*rsn)/(rp* *rho);
	op->s_phase = f*100.0; /* percent */

	/* set heliocentric long/lat; mean ecliptic and EOD */
	op->s_hlong = lpd;
	op->s_hlat = psi;

	/* fill solar sys body's ra/dec, alt/az in op */
	cir_pos (np, bet, lam, rho, op);        /* updates rho */

	/* set earth/planet and sun/planet distance */
	op->s_edist = *rho;
	op->s_sdist = rp;
}

/* fill equatoreal and horizontal op-> fields; stern
 *
 *    input:          lam/bet/rho geocentric mean ecliptic and equinox of day
 * 
 * algorithm at EOD:
 *   ecl_eq	--> ra/dec	geocentric mean equatoreal EOD (via mean obliq)
 *   deflect	--> ra/dec	  relativistic deflection
 *   nut_eq	--> ra/dec	geocentric true equatoreal EOD
 *   ab_eq	--> ra/dec	geocentric apparent equatoreal EOD
 *					if (PREF_GEO)  --> output
 *   ta_par	--> ra/dec	topocentric apparent equatoreal EOD
 *					if (!PREF_GEO)  --> output
 *   hadec_aa	--> alt/az	topocentric horizontal
 *   refract	--> alt/az	observed --> output
 *
 * algorithm at fixed epoch:
 *   ecl_eq	--> ra/dec	geocentric mean equatoreal EOD (via mean obliq)
 *   deflect	--> ra/dec	  relativistic deflection [for alt/az only]
 *   nut_eq	--> ra/dec	geocentric true equatoreal EOD [for aa only]
 *   ab_eq	--> ra/dec	geocentric apparent equatoreal EOD [for aa only]
 *   ta_par	--> ra/dec	topocentric apparent equatoreal EOD
 *     precess	--> ra/dec	topocentric equatoreal fixed equinox [eq only]
 *					--> output
 *   hadec_aa	--> alt/az	topocentric horizontal
 *   refract	--> alt/az	observed --> output
 */
static void
cir_pos (np, bet, lam, rho, op)
Now *np;
double bet, lam;/* geo lat/long (mean ecliptic of date) */
double *rho;	/* in: geocentric dist in AU; out: geo- or topocentic dist */
Obj *op;	/* object to set s_ra/dec as per epoch */
{
	double ra, dec;		/* apparent ra/dec, corrected for nut/ab */
	double tra, tdec;	/* astrometric ra/dec, no nut/ab */
	double lsn, rsn;	/* solar geocentric (mean ecliptic of date) */
	double ha_in, ha_out;	/* local hour angle before/after parallax */
	double dec_out;		/* declination after parallax */
	double dra, ddec;	/* parallax correction */
	double alt, az;		/* current alt, az */
	double lst;             /* local sidereal time */
	double rho_topo;        /* topocentric distance in earth radii */

	/* convert to equatoreal [mean equator, with mean obliquity] */
	ecl_eq (mjd, bet, lam, &ra, &dec);
	tra = ra;	/* keep mean coordinates */
	tdec = dec;

	/* get sun position */
	sunpos(mjed, &lsn, &rsn, NULL);

	/* allow for relativistic light bending near the sun.
	 * (avoid calling deflect() for the sun itself).
	 */
	if (!is_planet(op,SUN) && !is_planet(op,MOON))
	    deflect (mjd, op->s_hlong, op->s_hlat, lsn, rsn, *rho, &ra, &dec);

	/* correct ra/dec to form geocentric apparent */
	nut_eq (mjd, &ra, &dec);
	if (!is_planet(op,MOON))
		ab_eq (mjd, lsn, &ra, &dec);
	op->s_gaera = ra;
	op->s_gaedec = dec;

	/* find parallax correction for equatoreal coords */
	now_lst (np, &lst);
	ha_in = hrrad(lst) - ra;
	rho_topo = *rho * MAU/ERAD;             /* convert to earth radii */
	ta_par (ha_in, dec, lat, elev, &rho_topo, &ha_out, &dec_out);

	/* transform into alt/az and apply refraction */
	hadec_aa (lat, ha_out, dec_out, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;

	/* Get parallax differences and apply to apparent or astrometric place
	 * as needed.  For the astrometric place, rotating the CORRECTIONS
	 * back from the nutated equator to the mean equator will be
	 * neglected.  This is an effect of about 0.1" at moon distance.
	 * We currently don't have an inverse nutation rotation.
	 */
	if (pref_get(PREF_EQUATORIAL) == PREF_GEO) {
	    /* no topo corrections to eq. coords */
	    dra = ddec = 0.0;
	} else {
	    dra = ha_in - ha_out;	/* ra sign is opposite of ha */
	    ddec = dec_out - dec;
	    *rho = rho_topo * ERAD/MAU; /* return topocentric distance in AU */
	}

	/* fill in ra/dec fields */
	if (epoch == EOD) {		/* apparent geo/topocentric */
	    ra = ra + dra;
	    dec = dec + ddec;
	} else {			/* astrometric geo/topocent */
	    ra = tra + dra;
	    dec = tdec + ddec;
	    precess (mjd, epoch, &ra, &dec);
	}
	range(&ra, 2*PI);
	op->s_ra = ra;
	op->s_dec = dec;
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

/* apply relativistic light bending correction to ra/dec; stern
 *
 * The algorithm is from:
 * Mean and apparent place computations in the new IAU 
 * system. III - Apparent, topocentric, and astrometric 
 * places of planets and stars
 * KAPLAN, G. H.;  HUGHES, J. A.;  SEIDELMANN, P. K.;
 * SMITH, C. A.;  YALLOP, B. D.
 * Astronomical Journal (ISSN 0004-6256), vol. 97, April 1989, p. 1197-1210.
 *
 * This article is a very good collection of formulea for geocentric and
 * topocentric place calculation in general.  The apparent and
 * astrometric place calculation in this file currently does not follow
 * the strict algorithm from this paper and hence is not fully correct.
 * The entire calculation is currently based on the rotating EOD frame and
 * not the "inertial" J2000 frame.
 */
static void
deflect (mjd1, lpd, psi, lsn, rsn, rho, ra, dec)
double mjd1;		/* epoch */
double lpd, psi;	/* heliocentric ecliptical long / lat */
double rsn, lsn;	/* distance and longitude of sun */
double rho;		/* geocentric distance */
double *ra, *dec;	/* geocentric equatoreal */
{
	double hra, hdec;	/* object heliocentric equatoreal */
	double el;		/* HELIOCENTRIC elongation object--earth */
	double g1, g2;		/* relativistic weights */
	double u[3];		/* object geocentric cartesian */
	double q[3];		/* object heliocentric cartesian unit vect */
	double e[3];		/* earth heliocentric cartesian unit vect */
	double qe, uq, eu;	/* scalar products */
	int i;			/* counter */

#define G	1.32712438e20	/* heliocentric grav const; in m^3*s^-2 */
#define c	299792458.0	/* speed of light in m/s */

	elongation(lpd, psi, lsn-PI, &el);
	el = fabs(el);
	/* only continue if object is within about 10 deg around the sun
	 * and not obscured by the sun's disc (radius 0.25 deg)
	 *
	 * precise geocentric deflection is:  g1 * tan(el/2)
	 *	radially outwards from sun;  the vector munching below
	 *	just applys this component-wise
	 *	Note:	el = HELIOCENTRIC elongation.
	 *		g1 is always about 0.004 arc seconds
	 *		g2 varies from 0 (highest contribution) to 2
	 */
	if (el<degrad(170) || el>degrad(179.75)) return;

	/* get cartesian vectors */
	sphcart(*ra, *dec, rho, u, u+1, u+2);

	ecl_eq(mjd1, psi, lpd, &hra, &hdec);
	sphcart(hra, hdec, 1.0, q, q+1, q+2);

	ecl_eq(mjd1, 0.0, lsn-PI, &hra, &hdec);
	sphcart(hra, hdec, 1.0, e, e+1, e+2);

	/* evaluate scalar products */
	qe = uq = eu = 0.0;
	for(i=0; i<=2; ++i) {
	    qe += q[i]*e[i];
	    uq += u[i]*q[i];
	    eu += e[i]*u[i];
	}

	g1 = 2*G/(c*c*MAU)/rsn;
	g2 = 1 + qe;

	/* now deflect geocentric vector */
	g1 /= g2;
	for(i=0; i<=2; ++i)
	    u[i] += g1*(uq*e[i] - eu*q[i]);
	
	/* back to spherical */
	cartsph(u[0], u[1], u[2], ra, dec, &rho);	/* rho thrown away */
}

/* For RCS Only -- Do Not Edit */
static char *rcsid[2] = {(char *)rcsid, "@(#) $RCSfile: circum.c,v $ $Date: 1999/02/15 21:42:18 $ $Revision: 1.2 $ $Name:  $"};
