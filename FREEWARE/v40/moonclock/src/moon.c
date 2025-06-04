/* MoonTool oclock interface (dtime.o phase.o moon.o) */

#include <stdio.h>
#include <math.h>
#ifdef VMS
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif
#endif
#include "tws.h"

#define todeg(x)  ((x)*180.0/M_PI)

extern double jtime(), phase();

/* -1 (new-moon) .. 0 (full-moon) .. +1 (new-moon) */
void GetMoonPhase(mphase)
double *mphase;
{
	double jd, angphase, cphase, aom, cdist, cangdia, csund, csuang;

	jd = jtime(dtwstime());

	angphase = phase( jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);

	*mphase = angphase / M_PI - 1.0;
}
