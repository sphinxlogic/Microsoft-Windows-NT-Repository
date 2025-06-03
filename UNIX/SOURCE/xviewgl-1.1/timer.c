#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <signal.h>
#ifdef VMS
#include <time.h>
#else
#include <sys/time.h>
#endif
#include "my_types.h"
#include "timer.h"
#include "utils.h"
#include "windows.h"

BOOLEAN paused=FALSE;
int mindel = 0;

#ifdef HAS_VOLATILE
volatile BOOLEAN timed_out;
#else

BOOLEAN *timed_out_flag()
{
static BOOLEAN timed_out_val;
    return &timed_out_val;
}
#endif

void timer_intr(int ignored)
{
    timed_out=TRUE;
}

#ifdef VMS
#include libdtdef

void set_timer(int sec, int usec)
{
byte delta[8];
float secs = (1.0*sec) + (1.0e-6*usec);
    if ((sec==0) && (usec == 0)) {
	timed_out=TRUE;
	return;
    }
    timed_out=FALSE;
    lib$cvtf_to_internal_time(&LIB$K_DELTA_SECONDS_F,&secs,delta);
    sys$setimr(0,delta,timer_intr,0,0);
}
#else
void set_timer(int sec, int usec)
{
struct itimerval del, dummy;
    if ((sec==0) && (usec == 0)) {
	timed_out=TRUE;
	return;
    }
    timed_out=FALSE;
    signal(SIGALRM,timer_intr);
    del.it_interval.tv_sec = 0;
    del.it_interval.tv_usec = 0;
    del.it_value.tv_sec = sec;
    del.it_value.tv_usec = usec;
    setitimer(0,&del,&dummy);
}
#endif

void sit_for(int msec)
{
    if (msec < mindel)
	msec = mindel;
    set_timer(msec/100,(msec%100)*10000);
    flush_windows();
#if defined(HAS_ASYNC_INPUT) && !defined(VMS)
    if (!timed_out) {
	sigblock(1<<SIGALRM);
	if (!timed_out)
	    sigpause(0);
	sigsetmask(0);
    }
#else
    while (!timed_out)
	evt_handler(0);
#endif
    while (paused) {
	evt_handler(0);
    }
}
