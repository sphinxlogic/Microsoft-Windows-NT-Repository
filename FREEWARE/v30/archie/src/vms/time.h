#ifndef __PKTIME
#define __PKTIME

struct timeval
{
  long tv_sec;
  long tv_usec;
};

struct timezone
{
  int tz_minuteswest;
  int tz_dsttime;
};

struct itimerval
{
  struct timeval it_interval;
  struct timeval it_value;
};

#define ITIMER_REAL 0
#define timerclear(x) (x)->tv_sec = (x)->tv_usec = 0

#ifndef __GNUC__
# include <sys$library:time.h>
#else /* not Gnu C */
# include <gnu_cc_include:[000000]time.h>
#endif /* Gnu C */

#endif /* __PKTIME */
