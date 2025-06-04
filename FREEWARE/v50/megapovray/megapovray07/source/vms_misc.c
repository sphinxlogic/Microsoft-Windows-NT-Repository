/*
 * VMS_MISC.C
 * By: Jerome Lauret
 *     JLAURET@mail.chem.sunysb.edu
 *
 * The following code is necessary for POVRay v3.1g to compile on older 
 * versions of OpenVMS.
 */

#include "frame.h"
#include "povproto.h"  /* For Status_Info */
#include "povray.h"

#include <stdio.h>
#include <timeb.h>

#if __CRTL_VER < 70000000
   int gettimeofday(tv, tz)
   struct timeval  *tv;
   struct timezone *tz;
   {
     timeb_t tmp_time;
     ftime(&tmp_time);
     if (tv != NULL)
     {
       tv->tv_sec  = tmp_time.time;
       tv->tv_usec = tmp_time.millitm * 1000;
     }
     if (tz != NULL)
     {
       tz->tz_minuteswest = tmp_time.timezone;
       tz->tz_dsttime = tmp_time.dstflag;
     }
     return (0);
   } /*** End gettimeofday() ***/
#endif
