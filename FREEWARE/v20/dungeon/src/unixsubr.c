/* Operating system dependent subroutines.
 *
 * COPYRIGHT 1980, 1990, INFOCOM COMPUTERS AND COMMUNICATIONS, CAMBRIDGE MA.
 * ALL RIGHTS RESERVED, COMMERCIAL USAGE STRICTLY PROHIBITED
 * WRITTEN BY M. ADLER
 *
 * ITIME - Return system time in component form
 */

#include <time.h>

void itime_(h, m, s)
int *h, *m, *s;

{	static time_t t;
	struct tm *tm;

	time(&t);
	tm = localtime(&t);
	*h = tm->tm_hour;
	*m = tm->tm_min;
	*s = tm->tm_sec;
}
