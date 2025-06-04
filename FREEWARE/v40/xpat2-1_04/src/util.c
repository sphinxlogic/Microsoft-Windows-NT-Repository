/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module util.c				     */
/*									     */
/*	actually, this file contains a portable random-number-generator	     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*  \begin{cite}							*/
/*  This following lines contain a set of c-language functions for	*/
/*  generating uniform integers.   This is a COMPLETELY PORTABLE	*/
/*  generator.								*/
/*  It will give IDENTICAL sequences of random numbers for any		*/
/*  architecture with at least 30-bit integers, regardless of the	*/
/*  integer representation, MAXINT value, or roundoff/truncation	*/
/*  method, etc.                                             		*/
/*									*/
/*  This Truly Remarkable RNG is described more fully in         	*/
/*  J. Bentley's column, ``The Software Exploratorium ''            	*/
/*  to appear in Unix Review in 1991.                               	*/
/*  It is based on one in Knuth, Vol 2, Section 3.2.2 (Algorithm A) 	*/
/*  \end{cite}								*/
/*									*/
/*  I got it from the DIMACS (netflow) archive at Rutgers University	*/
/*  and modified it a bit to make it work for 16-bit ints, but 32-bit   */
/*  longs also.								*/
/*			Michael						*/
/*									     */
/*****************************************************************************/
#include "xpatgame.h"


/*----RNG Global Variables-------*/ 

static int rand_a = 24; 
static int rand_b = 0;
static long arr[55];  

/*----RNG Initializer------------*/
/* Call once before using lprand */ 

void sprand(long seed) {
    int i, ii;
    long last, next;
    arr[0] = last = seed; 
    next = 1;
    for (i=1; i < 55; i++) {
    	ii = ( 21 * i ) % 55;
	arr[ii] = next;
    	next = last - next; 
    	if (next < 0)
      	    next += PRANDMAX;
    	last = arr[ii];
    }

    rand_a = 0;
    rand_b = 24; 
    for (i = 0; i < 165; i++) 
    	(void)prand();
}

/*---------RNG---------------------*/
/* Returns long integers from the  */
/* range 0...PRANDMAX-1            */ 

long prand(void) {
    long t;
    if (rand_a-- == 0)
	rand_a = 54;
    if (rand_b-- == 0)
	rand_b = 54;

    t = arr[rand_a] - arr[rand_b];

    t %= PRANDMAX;
    if (t < 0)
	t += PRANDMAX;
    arr[rand_a] = t;
    return t;
}
