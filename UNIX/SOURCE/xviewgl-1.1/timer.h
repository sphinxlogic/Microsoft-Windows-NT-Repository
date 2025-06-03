#ifndef HAS_ASYNC_INPUT
BOOLEAN *timed_out_flag();
#endif
void timer_intr(int ignored);
void set_timer(int sec, int usec);
void sit_for(int msec);
extern int mindel;
extern BOOLEAN paused;
#ifdef HAS_VOLATILE
extern volatile BOOLEAN timed_out;
#else
BOOLEAN *timed_out_flag();
#define timed_out (*timed_out_flag())
#endif
