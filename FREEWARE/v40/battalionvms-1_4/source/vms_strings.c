/*
 *	STRINGS.C	(C adaptation of STRINGS.MAR)
 */

#include <string.h>
#include <ctype.h>

/*
 * bzero()
 *
 * zero out source for length bytes
 */
void bzero(src, length)
char *src;
unsigned long int length;
{
   memset(src, 0, length);
}

/*
 * ffs()
 *
 * finds the first bit set in the argument and returns the index
 * of that bit (starting at 1).
 */
int ffs(mask)
unsigned long int mask;
{
   register int i;

   for (i = 0; i < sizeof(mask); i++)
	if (mask & (1 << i)) return (i+1);

   return(-1);
}

