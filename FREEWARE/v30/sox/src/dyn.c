
/*
 * July 5, 1991
 * Copyright 1991 Lance Norskog And Sundry Contributors
 * This source code is freely redistributable and may be used for
 * any purpose.  This copyright notice must be maintained. 
 * Lance Norskog And Sundry Contributors are not responsible for 
 * the consequences of using this software.
 */

/*
 * Sound Tools dynamic compander effect file.
 *
 * Compresses or expands dynamic range, i.e. range between
 * soft and loud sounds.  U-law compression basically does this.
 *
 * Doesn't work.  Giving up for now.
 */

#include "st.h"
#include <math.h>

/* Private data for DYN.C file */
typedef struct dyn{
	int	rest;			/* bytes remaining in current block */
} *dyn_t;

/*
 * Process options
 */
dyn_getopts(effp, n, argv) 
eff_t effp;
int n;
char **argv;
{
	if (n)
		fail("Copy effect takes no options.");
}

/*
 * Prepare processing.
 */
dyn_start(effp)
eff_t effp;
{
	/* nothing to do */
	/* stuff data into delaying effects here */
}

/*
 * Processed signed long samples from ibuf to obuf.
 * Return number of samples processed.
 */

dyn_flow(effp, ibuf, obuf, isamp, osamp)
eff_t effp;
long *ibuf, *obuf;
int *isamp, *osamp;
{
	dyn_t dyn = (dyn_t) effp->priv;
	int len, done;
	
	char c;
	unsigned char uc;
	short s;
	unsigned short us;
	long l;
	unsigned long ul;
	float f;
	double d, tmp;
	int sign;

#define NORMIT (65536.0)

	len = ((*isamp > *osamp) ? *osamp : *isamp);
	for(done = 0; done < len; done++) {

		d = *ibuf++;
		if (d == 0.0)
			l = 0;
		else {
			if (d < 0.0) {
				d *= -1.0;
				sign = -1;
			} else
				sign = 1;
			d /= NORMIT;
			tmp = log10(d);
			tmp = pow(8.0, tmp);
			tmp = tmp * NORMIT;
			l = tmp * sign;
		}
		*obuf++ = l;
	}
}

/*
 * Drain out remaining samples if the effect generates any.
 */

dyn_drain(effp, obuf, osamp)
long *obuf;
int *osamp;
{
	*osamp = 0;
}

/*
 * Do anything required when you stop reading samples.  
 *	(free allocated memory, etc.)
 */
dyn_stop(effp)
eff_t effp;
{
	/* nothing to do */
}


