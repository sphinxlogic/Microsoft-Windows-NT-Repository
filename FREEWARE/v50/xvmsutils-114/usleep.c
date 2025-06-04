/*
 *	usleep() compatibility function for VAX/VMS
 *
 *	**************************************************************
 *	*** ASSUMPTION (for now) that (usec * 10) can be contained ***
 *	*** in 31-bits						   ***
 *	**************************************************************
 */

#include <starlet.h>
#include <stdio.h>

int usleep(usec)
	unsigned usec;
{
	int Delta[2];

	/*
	 *	Calculate the correct VMS Delta time
	 */
	Delta[0] = -(10*usec);
	Delta[1] = -1;
	/*
	 *	Schedule a wakeup
	 */
	sys$schdwk(0, 0, Delta, 0);
	/*
	 *	Hibernate
	 */
	sys$hiber();
	/*
	 *	Done
	 */
	return TRUE;
}

