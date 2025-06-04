/*
 *	UNIX-style Time Functions
 *
 */
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "vms_unix_time.h"

/*
 *	gettimeofday(2) - Returns the current time
 *
 *	NOTE: The timezone portion is useless on VMS.
 *	Even on UNIX, it is only provided for backwards
 *	compatibilty and is not guaranteed to be correct.
 */

int gettimeofday(tv)
struct timeval  *tv;
{
    timeb_t tmp_time;

    ftime(&tmp_time);

    if (tv != NULL)
    {
	tv->tv_sec  = tmp_time.time;
	tv->tv_usec = tmp_time.millitm * 1000;
    }


    return (0);

} /*** End gettimeofday() ***/

