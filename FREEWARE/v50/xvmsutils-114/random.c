/*	Replacements for random(), srandom()	*/
#include <stdlib.h>
#include <math.h>

long random()
{
    return (rand());
}

srandom(new_seed)
unsigned int new_seed;
{
    srand(new_seed);
}


static int seed;

char *initstate(new_seed, state, n)
unsigned int new_seed;
char state[];
int n;
{
    int i;
    unsigned int sum;

    sum = 0;
    for (i=0; i<n; i++)
	sum += state[i];
    sum = sum + new_seed;
    seed = sum;
    return state;
}

