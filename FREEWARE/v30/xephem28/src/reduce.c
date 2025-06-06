#include <math.h>

#include "P_.h"
#include "astro.h"

/* convert those orbital elements that change from epoch mjd0 to epoch mjd.
 */
void
reduce_elements (mjd0, mjd, inc0, ap0, om0, inc, ap, om)
double mjd0;	/* initial epoch */
double mjd;	/* desired epoch */
double inc0;	/* initial inclination, rads */
double ap0;	/* initial argument of perihelion, as an mjd */
double om0;	/* initial long of ascending node, rads */
double *inc;	/* desired inclination, rads */
double *ap;	/* desired epoch of perihelion, as an mjd */
double *om;	/* desired long of ascending node, rads */
{
	double t0, t1;
	double tt, tt2, t02, tt3;
	double eta, th, th0;
	double a, b;
	double dap;
	double cinc, sinc;
	double ot, sot, cot, ot1;
	double seta, ceta;

	t0 = mjd0/365250.0;
	t1 = mjd/365250.0;

	tt = t1-t0;
	tt2 = tt*tt;
        t02 = t0*t0;
	tt3 = tt*tt2;
        eta = (471.07-6.75*t0+.57*t02)*tt+(.57*t0-3.37)*tt2+.05*tt3;
        th0 = 32869.0*t0+56*t02-(8694+55*t0)*tt+3*tt2;
        eta = degrad(eta/3600.0);
        th0 = degrad((th0/3600.0)+173.950833);
        th = (50256.41+222.29*t0+.26*t02)*tt+(111.15+.26*t0)*tt2+.1*tt3;
        th = th0+degrad(th/3600.0);
	cinc = cos(inc0);
        sinc = sin(inc0);
	ot = om0-th0;
	sot = sin(ot);
        cot = cos(ot);
	seta = sin(eta);
        ceta = cos(eta);
	a = sinc*sot;
        b = ceta*sinc*cot-seta*cinc;
	ot1 = atan(a/b);
        if (b<0) ot1 += PI;
        b = sinc*ceta-cinc*seta*cot;
        a = -1*seta*sot;
	dap = atan(a/b);
        if (b<0) dap += PI;

        *ap = ap0+dap;
	range (ap, 2*PI);
        *om = ot1+th;
	range (om, 2*PI);

        if (inc0<.175)
	    *inc = asin(a/sin(dap));
	else
	    *inc = 1.570796327-asin((cinc*ceta)+(sinc*seta*cot));
}
