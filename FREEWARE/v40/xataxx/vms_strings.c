/*
 *	STRINGS.C	(C adaptation of STRINGS.MAR)
 */

#include <string.h>
#include <ctype.h>
/*
 * bcopy()
 *
 * copies length of source to destination
 */
void bcopy (src, dest, length)
char *src, *dest;
unsigned long int length;
{
  memcpy(dest, src, length);
}

