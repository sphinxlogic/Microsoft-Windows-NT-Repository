/* worked derived largely directly from Duffet-Smith */

#ifndef PI
#define	PI		3.141592653589793
#endif

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

/* ratio of from synodic (solar) to sidereal (stellar) rate */
#define	SIDRATE		.9972695677

#define	SPD	(24.0*3600.0)	/* seconds per day */

/* manifest names for planets.
 * N.B. must coincide with usage in pelement.c and plans.c.
 * N.B. only the first 8 are valid for use with plans().
 */
#define	MERCURY	0
#define	VENUS	1
#define	MARS	2
#define	JUPITER	3
#define	SATURN	4
#define	URANUS	5
#define	NEPTUNE	6
#define	PLUTO	7

/* a few more handy ones */
#define	SUN	8
#define	MOON	9
#define	OBJX	10
#define	OBJY	11
#define	NOBJ	12	/* total number of basic objects */


/* global function declarations */

/* aa_hadec.c */
extern void aa_hadec P_((double lat, double alt, double az, double *ha,
    double *dec));
extern void hadec_aa P_((double lat, double ha, double dec, double *alt,
    double *az));

/* anomaly.c */
extern void anomaly P_((double ma, double s, double *nu, double *ea));

/* comet.c */
extern void comet P_(( double mjd, double ep, double inc, double ap, double qp,
    double om, double *lpd, double *psi, double *rp, double *rho, double *lam,
    double *bet));

/* eq_ecl.c */
extern void eq_ecl P_((double mjd, double ra, double dec, double *lat,
    double *lng));
extern void ecl_eq P_((double mjd, double lat, double lng, double *ra,
    double *dec));

/* mjd.c */
extern void cal_mjd P_((int mn, double dy, int yr, double *mjd));
extern void mjd_cal P_((double mjd, int *mn, double *dy, int *yr));
extern int mjd_dow P_((double mjd, int *dow));
extern void mjd_dpm P_((double mjd, int *ndays));
extern void mjd_year P_((double mjd, double *yr));
extern void year_mjd P_((double y, double *mjd));
extern void rnd_second P_((double *t));
extern double mjd_day P_((double jd));
extern double mjd_hr P_((double jd));
extern void range P_((double *v, double r));

/* moon.c */
extern void moon P_((double mjd, double *lam, double *bet, double *hp,
    double *msp, double *mdp));

/* mooncolong.c */
extern void moon_colong P_((double mjd, double lt, double lg, double *cp,
    double *kp, double *ap));

/* nutation.c */
extern void nutation P_((double mjd, double *deps, double *dpsi));

/* obliq.c */
extern void obliquity P_((double mjd, double *eps));

/* parallax.c */
extern void ta_par P_((double tha, double tdec, double phi, double ht,
    double ehp, double *aha, double *adec));

/* pelement.c */
extern void pelement P_((double mjd, double plan[8][9]));

/* plans.c */
extern void plans P_((double mjd, int p, double *lpd0, double *psi0,
    double *rp0, double *rho0, double *lam, double *bet, double *dia,
    double *mag));

/* precess.c */
extern void precess P_((double mjd1, double mjd2, double *ra, double *dec));

/* reduce.c */
extern void reduce_elements P_((double mjd0, double mjd, double inc0,
    double ap0, double om0, double *inc, double *ap, double *om));

/* refract.c */
extern void refract P_((double pr, double tr, double ta, double *aa));
void unrefract P_((double pr, double tr, double aa, double *ta));

/* riset.c */
extern void riset P_((double ra, double dec, double lat, double dis,
    double *lstr, double *lsts, double *azr, double *azs, int *status));

/* sun.c */
extern void sunpos P_((double mjd, double *lsn, double *rsn));

/* utc_gst.c */
extern void utc_gst P_((double mjd, double utc, double *gst));
extern void gst_utc P_((double mjd, double gst, double *utc));
