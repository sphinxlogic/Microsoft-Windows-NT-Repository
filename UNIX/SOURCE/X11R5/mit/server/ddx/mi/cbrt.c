/* $XFree86: mit/server/ddx/mi/cbrt.c,v 2.0 1994/03/06 06:42:53 dawes Exp $ */
/* $XConsortium: cbrt.c,v 1.0 90/09/29 10:24:02 rws Exp $ */

/* simple cbrt, in case your math library doesn't have a good one */

double pow();

double
cbrt(x)
    double x;
{
    if (x > 0.0)
	return pow(x, 1.0/3.0);
    else
	return -pow(-x, 1.0/3.0);
}
