/* Copyright (C) 1992 Nathan Sidwell */
/*{{{  the problem with real time under unix*/
/*
 * Getting accurate, stable timing is difficult. An auto repeating
 * interrupt cannot be used, because that has rude behaviour, if your
 * slightly too slow. So we must fire off the next IRQ ourselves,
 * preferably inside the interrupt routine. But, there will be some
 * slippage, due to the time for the kernel to deliver the interrupt, and
 * us to set off the next one.
 * A clever way of getting accurate average behaviour is to keep a
 * record of how long the previous IRQ actually took, and use that to
 * generate a correction factor, to be used for the next IRQ request.
 * But, that doesn't converge, because it's fighting the granularity of the
 * kernel's preemptive scheduler. For instance, if we want a rate of
 * 37mS, but the scheduling is in 10mS chunks, we'll be (at least)
 * 3mS too slow, we adjust the delta to be -3 (34mS), and next time we're
 * also 3mS too slow, so we adjust it again to -6 (31mS), and the same
 * happens, so its now a delta of -9 (28mS). This time we're
 * 7mS too fast so the delta changes again to -2 (35mS). As you can
 * see, the delta keeps changing. On average the timing should be ok, but
 * some frames will be too fast and some too slow, which gives
 * jerky animation.
 * Another thing which exacerbates this is that the IRQ is set in uS, but
 * the time can only be go to mS accuracy.
 * So you have a choice, smooth animation, but not quite accurate, or
 * jerky animation, but accurate on average. I've elected for smooth
 * animation.
 *
 * Now to complicate things further. The elapsed game time is now
 * kept, on a per screen basis. Because the game can be paused and there
 * are the history screens, the clock cannot be simply used. I could count
 * animation frames and scale by the interrupt time, but this will always
 * underestimate (due to the slipage above), and may be completely wrong
 * because the interrupts are being missed (but see below).
 * Most systems will have a gettimeofday which gives usec precision of the
 * clock (but not usec accuracy wrt real time, but should be good enough
 * relative to itself). I use this, stopping and starting at appropriate
 * points, to get the elapsed time.
 *
 * In order to get meaningful score comparisons between different
 * hardware, I measure the percentage of missed frames. If this gets too
 * large, I use the error factor to increase the frame time, and
 * scale the scores downwards appropriately. The actual frame time
 * should converge asymtotically to the minimum that the hardware
 * can support, if the hardware is not fast enough for the game.
 */
/*}}}*/
#include "xmris.h"
#include <time.h>
#include <sys/time.h>
#include <signal.h>
/*{{{  signal_hold & signal_release*/
#ifdef POSIX
#  define MASK sigset_t
#  define signal_hold(signal, maskp) \
    {\
      MASK temp; \
      sigemptyset(&temp); \
      sigaddset(&temp, signal); \
      sigprocmask(SIG_BLOCK, &temp, (maskp)); \
    }
#  define signal_release(maskp) sigprocmask(SIG_SETMASK, maskp, (MASK *)NULL)
#else
#  ifdef __hpux /* hpux is a weird mixture of BSD & SYSV */
#    define MASK    int
#    define signal_hold(signal, maskp)    (*(maskp) = sigblock(sigmask(signal)))
#    define signal_release(maskp)   sigsetmask(*(maskp))
#  else
#    ifdef SYSV
#      define MASK    int
#      define signal_hold(signal, maskp)    (*(maskp) = sighold(signal), signal)
#      define signal_release(maskp)   sigrelse(*(maskp))
#    else
#      define MASK    int
#      define signal_hold(signal, maskp)    (*(maskp) = sigblock(sigmask(signal)))
#      define signal_release(maskp)   sigsetmask(*(maskp))
#    endif /* SYSV */
#  endif /* __hpux */
#endif /* POSIX */
/*}}}*/
/*{{{  get current time*/
#ifdef USETIME
#define gettick(ptr) (*(ptr) = time((time_t *)NULL))
#define TICKTIME (unsigned long)1000000
typedef time_t tick_t;
#else
#define gettick(ptr) \
    {\
      struct timeval timeofday; \
      gettimeofday(&timeofday, (struct timezone *)NULL); \
      *(ptr) = (unsigned long)timeofday.tv_sec * 1000 + \
	  (unsigned long)timeofday.tv_usec / 1000; \
    }
#define TICKTIME (unsigned long)1000
typedef unsigned long tick_t;
#endif /* USETIME */
/*}}}*/
/*{{{  timer*/
static struct
{
  void      (*handler)();   /* original handler */
  unsigned  volatile elapsed;       /* timer elapsed */
  unsigned  volatile waiting;       /* waiting for the interrupt */
  struct itimerval interval;        /* interval time */
  unsigned  state;          /* timing state */
  tick_t    game;           /* start of game tick */
  tick_t    start;          /* timing start */
  tick_t    stop;           /* timing stop */
  unsigned  count;          /* frame count */
  unsigned  missed;         /* missed count */
} timer;
/*}}}*/
/*{{{  prototypes*/
static VOIDFUNC timer_alarm PROTOARG((int));
/*}}}*/
/*{{{  void timer_alarm(sig)*/
static VOIDFUNC timer_alarm
/* ARGSUSED */
FUNCARG((sig),
	int       sig
)
{
  /*
   * Most calls are undefined in a signal handler
   * (only signal, exit, longjump & abort are guaranteed to work)
   * This should work, except on _really_ weird library implementations,
   * because timer.waiting is only true when the main thread is stuck
   * in a wait() call.
   */
  timer.elapsed = 1;
  signal(SIGALRM, timer_alarm);
  if(timer.waiting)
    {
      setitimer(ITIMER_REAL, &timer.interval, (struct itimerval *)NULL);
      timer.waiting = 0;
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void timer_close()*/
extern VOIDFUNC timer_close FUNCARGVOID
/*
 * closes the timer stuff
 */
{
  MASK      mask;
  
  signal_hold(SIGALRM, &mask);
  timer.interval.it_value.tv_usec = 0;
  timer.interval.it_interval.tv_usec = 0;
  if(!timer.elapsed)
    sigpause(0);
  signal_release(&mask);
  setitimer(ITIMER_REAL, &timer.interval, (struct itimerval *)NULL);
  signal(SIGALRM, timer.handler);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void timer_open()*/
extern VOIDFUNC timer_open FUNCARGVOID
/*
 * initialize the timer stuff
 * this means installing the alarm signal handler
 * and starting the first tick
 */
{
  timer.interval.it_interval.tv_sec = 0;
  timer.interval.it_interval.tv_usec = 0;
  timer.interval.it_value.tv_sec = 0;
  timer.interval.it_value.tv_usec = 0;
  timer.handler = signal(SIGALRM, timer_alarm);
  timer.waiting = 0;
  timer.elapsed = 1;
  global.missed = 0;
  global.dilation = FRAME_SCALE;
  global.scale = SCORE_SCALE;
  timer.interval.it_value.tv_usec = 10000;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  int timer_set(tick, state)*/
extern unsigned timer_set
FUNCARG((tick, state),
	unsigned  long tick
ARGSEP  unsigned  state
)
/*
 * sets the timer tick and fiddles the timer state
 * if the tick is zero, then the timer state only is altered
 */
{
  unsigned  value;
  
  if(tick)
    {
      {
	/* stupid compilers with broken stringizizing
	 * and stupid compilers which don't understand \
	 * outside of #define */
	unsigned  t1, t2;
	
	t1 = timer.state == TIMING_OFF || timer.state == TIMING_PAUSE;
	t2 = state == TIMING_OFF || state == TIMING_PAUSE;
	assert(t1 || t2);
      }
      timer.interval.it_value.tv_usec = tick;
      timer_wait();
    }
  value = timer.state;
  switch(state)
  {
    /*{{{  case TIMING_ON:*/
    case TIMING_ON:
    {
      if(timer.state == TIMING_OFF)
	global.msec = 0;
      if(timer.state != TIMING_ON)
	{
	  gettick(&timer.start);
	  if(timer.state == TIMING_OFF)
	    timer.game = timer.start;
	  else
	    {
	      tick_t    now;
	      
	      gettick(&now);
	      timer.game += now - timer.stop;
	    }
	  timer.state = TIMING_ON;
	  timer.count = 0;
	  timer.missed = 0;
	}
      break;
    }
    /*}}}*/
    /*{{{  case TIMING_OFF:*/
    case TIMING_OFF:
    {
      if(timer.state != TIMING_OFF)
	{
	  if(timer.state == TIMING_ON)
	    gettick(&timer.stop);
	  global.msec = (timer.stop - timer.game) *
	      (TICKTIME / (unsigned long)1000);
	  timer.state = TIMING_OFF;
	}
      break;
    }
    /*}}}*/
    /*{{{  case TIMING_PAUSE:*/
    case TIMING_PAUSE:
    {
      if(timer.state == TIMING_ON)
	{
	  timer.state = TIMING_PAUSE;
	  gettick(&timer.stop);
	}
      break;
    }
    /*}}}*/
    default:
      assert(0);
  }
  return value;
}
/*}}}*/
/*{{{  void timer_wait()*/
extern VOIDFUNC timer_wait FUNCARGVOID
/*
 * waits for the next timer interrupt
 * if this has already gone by, then we immediately return
 * If we arrive here before the interrupt, the interrupt is retriggered
 * in the signal handler (timer_alarm), for minimum slipage. If
 * we're too late, then the interrupt has to be restarted here. (If
 * the signal handler did it, things rapidly get out of hand.)
 */
{
  MASK      mask;
  int       point;
  
  point = -1;
  signal_hold(SIGALRM, &mask);
  if(!timer.elapsed)
    {
      if(global.missed)
	{
	  point = WINDOW_WIDTH - global.missed;
	  global.missed--;
	}
      timer.waiting = 1;
      sigpause(0);
    }
  else
    {
      timer.missed++;
      setitimer(ITIMER_REAL, &timer.interval, (struct itimerval *)NULL);
      if(global.missed < WINDOW_WIDTH)
	{
	  global.missed++;
	  point = WINDOW_WIDTH - global.missed;
	}
    }
  timer.elapsed = 0;
  signal_release(&mask);
  /*{{{  do frame ratio?*/
  if(timer.state == TIMING_ON && timer.count++ == FRAME_RATIO_UPDATE - 1)
    {
      unsigned  dilation;
      int       xold, xnew;
      
      if(timer.missed > (global.dilation == FRAME_SCALE ?
	  FRAME_MISS_START : FRAME_MISS_STOP))
	{
	  unsigned long usec;
	  
	  gettick(&timer.stop);
	  usec = (unsigned long)(timer.stop - timer.start) * TICKTIME;
	  if(usec > TICKTIME / 2)
	    usec -= TICKTIME / 2;
	  usec /= FRAME_RATIO_UPDATE;
	  dilation = usec * FRAME_SCALE / timer.interval.it_value.tv_usec;
	}
      else
	dilation = FRAME_SCALE;
      xold = WINDOW_WIDTH - (int)((unsigned long)WINDOW_WIDTH *
	    FRAME_SCALE / global.dilation);
      xnew = WINDOW_WIDTH - (int)((unsigned long)WINDOW_WIDTH *
	    FRAME_SCALE / dilation);
      /*{{{  swap?*/
      if(xold > xnew)
	{
	  int     t;
	  
	  t = xold;
	  xold = xnew;
	  xnew = t;
	}
      /*}}}*/
      XDrawLine(display.display, display.window, GCN(GC_LOAD),
	  xold, PIXELY(CELLS_DOWN, CELL_HEIGHT),
	  xnew, PIXELY(CELLS_DOWN, CELL_HEIGHT));
      global.dilation = dilation;
      timer.start = timer.stop;
      timer.missed = timer.count = 0;
      /*{{{  set score scale*/
      {
	unsigned long scale;
	unsigned  last;
	
	scale = (unsigned long)SCORE_SCALE * SCORE_SCALE *
	    FRAME_SCALE / global.dilation;
	dilation = SCORE_SCALE;
	do
	  {
	    last = dilation;
	    dilation = ((unsigned long)dilation * dilation + scale) /
		dilation / 2;
	  }
	while(dilation != last);
	global.scale = dilation;
      }
      /*}}}*/
    }
  /*}}}*/
  if(point >= 0)
    XDrawPoint(display.display, display.window, GCN(GC_LOAD),
	point, PIXELY(CELLS_DOWN, CELL_HEIGHT));
  return VOIDRETURN;
}
/*}}}*/
