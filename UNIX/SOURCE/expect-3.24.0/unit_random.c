/* unit_random.c - compute random numbers from 0 to 1, for expect's send -h

Written by: Don Libes, NIST, 10/31/89

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.

*/

/* This implementation sacrifices beauty for portability */

void
init_unit_random()
{
	srand(getpid());
}

float
unit_random()
{
	/* current implementation is pathetic but works */
	/* 99991 is largest prime in my CRC - can't hurt, eh? */
	return((float)(1+(rand()%99991))/99991.0);
}
