
/*
 * July 5, 1991
 * Copyright 1991 Lance Norskog And Sundry Contributors
 * This source code is freely redistributable and may be used for
 * any purpose.  This copyright notice must be maintained. 
 * Lance Norskog And Sundry Contributors are not responsible for 
 * the consequences of using this software.
 */

/*
 * Sound Tools High-Pass effect file.
 *
 * Algorithm:  1nd order filter.
 * From Fugue source code:
 *
 * 	output[N] = B * (output[N-1] - input[N-1] + input[N])
 *
 * 	A = 2.0 * pi * center
 * 	B = exp(-A / frequency)
 */

#include <math.h>
#include "st.h"

/* Private data for Highpass effect */
typedef struct highpstuff {
	float	center;
	double	A, B;
	double	in1, out1;
} *highp_t;

/*
 * Process options
 */
highp_getopts(effp, n, argv) 
eff_t effp;
int n;
char **argv;
{
	highp_t highp = (highp_t) effp->priv;

	if ((n < 1) || !sscanf(argv[0], "%f", &highp->center))
		fail("Usage: highp center");
}

/*
 * Prepare processing.
 */
highp_start(effp)
eff_t effp;
{
	highp_t highp = (highp_t) effp->priv;
	if (highp->center > effp->ininfo.rate*2)
		fail("Highpass: center must be < minimum data rate*2\n");
	
	highp->A = (M_PI * 2.0 * highp->center) / effp->ininfo.rate;
	highp->B = exp(-highp->A / effp->ininfo.rate);
	highp->in1 = 0.0;
	highp->out1 = 0.0;
}

/*
 * Processed signed long samples from ibuf to obuf.
 * Return number of samples processed.
 */

highp_flow(effp, ibuf, obuf, isamp, osamp)
eff_t effp;
long *ibuf, *obuf;
int *isamp, *osamp;
{
	highp_t highp = (highp_t) effp->priv;
	int len, done;
	double d;
	long l;

	len = ((*isamp > *osamp) ? *osamp : *isamp);
	d = highp->out1;

	/* yeah yeah yeah registers & integer arithmetic yeah yeah yeah */
	for(done = 0; done < len; done++) {
		l = *ibuf++;
		d = (highp->B * ((d - highp->in1) + (double) l)) / 65536.0;
		d *= 0.8;
		if (d > 32767)
			d = 32767;
		if (d < - 32767)
			d = - 32767;
		highp->in1 = l;
		*obuf++ = d * 65536;
	}
	highp->out1 = d;
}

/*
 * Do anything required when you stop reading samples.  
 * Don't close input file! 
 */
highp_stop(effp)
eff_t effp;
{
	/* nothing to do */
}

