/* $XFree86: mit/lib/Xt/NextEvent.c,v 2.1 1993/09/22 15:33:01 dawes Exp $ */
/* $XConsortium: NextEvent.c,v 1.110 93/02/10 15:47:39 converse Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "IntrinsicI.h"
#include <stdio.h>
#include <errno.h>
#ifdef _MINIX
#include <assert.h>
#include <signal.h>
#endif

extern int errno;

static TimerEventRec* freeTimerRecs;
static WorkProcRec* freeWorkRecs;

#ifdef AMOEBA
#define fd_set	Fd_set
#define event	__event
#define interval __interval
#include <amoeba.h>
#undef event
#undef interval
#endif

/* Some systems running NTP daemons are known to return strange usec
 * values from gettimeofday.  At present (3/90) this has only been
 * reported on SunOS...
 */

#ifndef NEEDS_NTPD_FIXUP
# if defined(sun) || defined(MOTOROLA)
#  define NEEDS_NTPD_FIXUP 1
# else
#  define NEEDS_NTPD_FIXUP 0
# endif
#endif

#if NEEDS_NTPD_FIXUP
#define FIXUP_TIMEVAL(t) { \
	while ((t).tv_usec >= 1000000) { \
	    (t).tv_usec -= 1000000; \
	    (t).tv_sec++; \
	} \
	while ((t).tv_usec < 0) { \
	    if ((t).tv_sec > 0) { \
		(t).tv_usec += 1000000; \
		(t).tv_sec--; \
	    } else { \
		(t).tv_usec = 0; \
		break; \
	    } \
	}}
#else
#define FIXUP_TIMEVAL(t)
#endif /*NEEDS_NTPD_FIXUP*/



/*
 * Private routines
 */
#define ADD_TIME(dest, src1, src2) { \
	if(((dest).tv_usec = (src1).tv_usec + (src2).tv_usec) >= 1000000) {\
	      (dest).tv_usec -= 1000000;\
	      (dest).tv_sec = (src1).tv_sec + (src2).tv_sec + 1 ; \
	} else { (dest).tv_sec = (src1).tv_sec + (src2).tv_sec ; \
	   if(((dest).tv_sec >= 1) && (((dest).tv_usec <0))) { \
	    (dest).tv_sec --;(dest).tv_usec += 1000000; } } }


#define TIMEDELTA(dest, src1, src2) { \
	if(((dest).tv_usec = (src1).tv_usec - (src2).tv_usec) < 0) {\
	      (dest).tv_usec += 1000000;\
	      (dest).tv_sec = (src1).tv_sec - (src2).tv_sec - 1;\
	} else 	(dest).tv_sec = (src1).tv_sec - (src2).tv_sec;  }

#define IS_AFTER(t1, t2) (((t2).tv_sec > (t1).tv_sec) \
	|| (((t2).tv_sec == (t1).tv_sec)&& ((t2).tv_usec > (t1).tv_usec)))

#define IS_AT_OR_AFTER(t1, t2) (((t2).tv_sec > (t1).tv_sec) \
	|| (((t2).tv_sec == (t1).tv_sec)&& ((t2).tv_usec >= (t1).tv_usec)))

#ifdef SVR4
/*  This is a fix for the difference between BSD's select() and
 *  some SVR4's select().  Some SVR4 select() implementations can neve
 *  return a value larger than the total number of file descriptors being
 *  checked.  So, if you select for read and write on one file descriptor,
 *  and both are true, those implementations of select() will only return 1.
 *  BSD select in the same situation will return 2.
 *
 *	Additionally, BSD select() on timing out, will zero the masks,
 *	while SVR4 does not.  This is fixed here as well.
 *
 *	Jerry Whelan, guru@bradley.edu, June 12th, 1993
 */

#ifdef __STDC__
static int BSDselect(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif

static int
BSDselect(nfds, readfds, writefds, exceptfds, timeout)
int nfds;
fd_set *readfds, *writefds, *exceptfds;
struct timeval *timeout;
{
    int    rval, i;

    rval = select(nfds, readfds, writefds, exceptfds, timeout);

    switch(rval) {
	case -1:    return(rval);
	 	    break;

	case 0:     if(readfds != NULL)
		    FD_ZERO(readfds);
		    if(writefds != NULL)
			FD_ZERO(writefds);
		    if(exceptfds != NULL)
			FD_ZERO(exceptfds);

		    return(rval);
		    break;

	default:    for(i=0, rval=0; i < nfds; i++) {
			if((readfds != NULL) && FD_ISSET(i, readfds)) rval++;
			if((writefds != NULL) && FD_ISSET(i, writefds)) rval++;
			if((writefds != NULL) && FD_ISSET(i, exceptfds)) rval++;
		    }
		    return(rval);
    }
/* Should never get here */
}

#define select(a,b,c,d,e)       BSDselect(a,b,c,d,e)

#endif /* SVR4 */

static void QueueTimerEvent(app, ptr)
    XtAppContext app;
    TimerEventRec *ptr;
{
        TimerEventRec *t,**tt;
        tt = &app->timerQueue;
        t  = *tt;
        while (t != NULL &&
                IS_AFTER(t->te_timer_value, ptr->te_timer_value)) {
          tt = &t->te_next;
          t  = *tt;
         }
         ptr->te_next = t;
         *tt = ptr;
}

#ifdef AMOEBA
static int
gettimeofday(tvp, tzp)
    struct timeval *tvp;
    struct timezone *tzp;
{
    static unsigned long lasttime, timedelta;
    unsigned long curtime;
    long time();

    curtime = sys_milli();
    if (lasttime == 0 || lasttime > curtime) {
	/* re-init timer */
	timedelta = time(0) - curtime/1000;
    }
    lasttime = curtime;
    tvp->tv_sec = timedelta + curtime/1000;
    tvp->tv_usec = 1000 * (curtime%1000);
}
#endif /* AMOEBA */

/* 
 * Routine to block in the toolkit.  This should be the only call to select.
 *
 * This routine returns when there is something to be done.
 *
 * Before calling this with ignoreInputs==False, app->outstandingQueue should
 * be checked; this routine will not verify that an alternate input source
 * has not already been enqueued.
 *
 *
 * _XtwaitForSomething( ignoreTimers, ignoreInputs, ignoreEvents,
 *			block, howlong, appContext)
 * Boolean ignoreTimers;     (Don't return if a timer would fire
 *				Also implies forget timers exist)
 *
 * Boolean ignoreInputs;     (Ditto for input callbacks )
 *
 * Boolean ignoreEvents;     (Ditto for X events)
 *
 * Boolean block;	     (Okay to block)
 * TimeVal howlong;	     (howlong to wait for if blocking and not
 *				doing Timers... Null mean forever.
 *				Maybe should mean shortest of both)
 * XtAppContext app;	     (Displays to check wait on)
 * Returns display for which input is available, if any
 * and if ignoreEvents==False, else returns -1
 *
 * if ignoring everything && block=True && howlong=NULL, you'll have
 * lots of time for coffee; better not try it!  In fact, it probably
 * makes little sense to do this regardless of the value of howlong
 * (bottom line is, we don't bother checking here).
 */
#ifndef _MINIX
#if NeedFunctionPrototypes
int _XtwaitForSomething(
	_XtBoolean ignoreTimers,
	_XtBoolean ignoreInputs,
	_XtBoolean ignoreEvents,
	_XtBoolean block,
	unsigned long *howlong,
	XtAppContext app
        )
#else
int _XtwaitForSomething(ignoreTimers, ignoreInputs, ignoreEvents,
			block, howlong, app)
	Boolean ignoreTimers;
	Boolean ignoreInputs;
	Boolean ignoreEvents;
	Boolean block;
	unsigned long *howlong;
	XtAppContext app;
#endif
{
	struct timeval  cur_time;
	struct timeval  start_time;
	struct timeval  wait_time;
	struct timeval  new_time;
	struct timeval  time_spent;
	struct timeval	max_wait_time;
	static struct timeval  zero_time = { 0 , 0};
	register struct timeval *wait_time_ptr;
#ifndef AMOEBA
	Fd_set rmaskfd, wmaskfd, emaskfd;
	static Fd_set zero_fd = { 0 };
#else
	int timout;
#endif
	int nfound, i, d;
	
 	if (block) {
		(void) gettimeofday (&cur_time, NULL);
		FIXUP_TIMEVAL(cur_time);
		start_time = cur_time;
		if(howlong == NULL) { /* special case for ever */
			wait_time_ptr = 0;
		} else { /* block until at most */
			max_wait_time.tv_sec = *howlong/1000;
			max_wait_time.tv_usec = (*howlong %1000)*1000;
			wait_time_ptr = &max_wait_time;
		}
	} else {  /* don't block */
		max_wait_time = zero_time;
		wait_time_ptr = &max_wait_time;
	}

      WaitLoop:
	while (1) {
		if (app->timerQueue != NULL && !ignoreTimers && block) {
		    if(IS_AFTER(cur_time, app->timerQueue->te_timer_value)) {
			TIMEDELTA (wait_time, app->timerQueue->te_timer_value, 
				   cur_time);
			if(howlong==NULL || IS_AFTER(wait_time,max_wait_time)){
				wait_time_ptr = &wait_time;
			} else {
				wait_time_ptr = &max_wait_time;
			}
		    } else wait_time_ptr = &zero_time;
		} 
#ifndef AMOEBA
		if( !ignoreInputs ) {
			rmaskfd = app->fds.rmask;
			wmaskfd = app->fds.wmask;
			emaskfd = app->fds.emask;
		} else {
			rmaskfd = zero_fd;
			wmaskfd = zero_fd;
			emaskfd = zero_fd;
		}
		if (!ignoreEvents) {
		    for (d = 0; d < app->count; d++) {
			FD_SET (ConnectionNumber(app->list[d]),
			        (fd_set *)&rmaskfd);
		    }
		}
		nfound = select (app->fds.nfds, (fd_set *) &rmaskfd,
			         (fd_set *) &wmaskfd, (fd_set *) &emaskfd,
				 wait_time_ptr);
#else
		/*
		 * Unfortunately we cannot use select when using the
		 * amoeba protocol. We call _XAmSelect to wait for the
		 * display to produce input (or until the timer runs
		 * out).
		 * This has two disadvantages:
		 * - We cannot wait for multiple displays.
		 *   (this is handled in Display.c, by not allowing more
		 *   than one to be opened)
		 * - We cannot wait for other file descriptors.
		 *   (I know no solution for this. Let's hope it doesn't
		 *   matter).
		 * However, this should at least enable us to handle most
		 * applications (i.e. with one display and no files).
		 *
		 * There is a workaround, however, in the form of
		 * XamSetSema and XamBlock, which allow you to wait for
		 * X events or other things.
		 */
		if (wait_time_ptr == 0 && ignoreEvents) {
		    fprintf(stderr, "XtWaitForSomething: infinite wait\n");
		    abort();
		}
		if (app->count != 1)
		    XtErrorMsg("xxx", "XtwaitForSomething", "XtToolkitError",
			"No display", (String *)NULL, (Cardinal *)NULL);
		if (wait_time_ptr)
		    timout = wait_time_ptr->tv_sec*1000 +
				(wait_time_ptr->tv_usec+999)/1000;
		else
		    timout = -1;
		if (ignoreEvents) {
		    mutex mu;

		    mu_init(&mu);
		    mu_lock(&mu);
		    mu_trylock(&mu, timout);
		    nfound = 0;
		} else {
		    nfound = _XAmSelect(ConnectionNumber(app->list[0]), timout);
		}
#endif /* AMOEBA */
		if (nfound == -1) {
			/*
			 *  interrupt occured recalculate time value and select
			 *  again.
			 */
			if (errno == EINTR) {
			    errno = 0;  /* errno is not self reseting */
			    if (block) {
				if (wait_time_ptr == NULL) /*howlong == NULL*/
				    continue;
				(void)gettimeofday (&new_time, NULL);
				FIXUP_TIMEVAL(new_time);
				TIMEDELTA(time_spent, new_time, cur_time);
				cur_time = new_time;
				if(IS_AFTER(time_spent, *wait_time_ptr)) {
					TIMEDELTA(wait_time, *wait_time_ptr,
						  time_spent);
					wait_time_ptr = &wait_time;
					continue;
				} else {
					/* time is up anyway */
					nfound = 0;
				}
			    }
			} else {
			    char Errno[12];
			    String param = Errno;
			    Cardinal param_count = 1;

			    if (!ignoreEvents) {
				/* get Xlib to detect a bad connection */
				for (d = 0; d < app->count; d++) {
				    if (XEventsQueued(app->list[d],
						      QueuedAfterReading))
					return d;
				}
			    }
			    sprintf( Errno, "%d", errno);
			    XtAppWarningMsg(app, "communicationError","select",
			       XtCXtToolkitError,"Select failed; error code %s",
			       &param, &param_count);
			    continue;
			}
		} /* timed out or input available */
		break;
	}
	
	if (nfound == 0) {
		if(howlong) *howlong = (unsigned long)0;  /* Timed out */
		return -1;
	}
	if(block && howlong != NULL) { /* adjust howlong */
	    (void) gettimeofday (&new_time, NULL);
	    FIXUP_TIMEVAL(new_time);
	    TIMEDELTA(time_spent, new_time, start_time);
	    if(*howlong <= (time_spent.tv_sec*1000+time_spent.tv_usec/1000))
		*howlong = (unsigned long)0;  /* Timed out */
	    else
		*howlong -= (time_spent.tv_sec*1000+time_spent.tv_usec/1000);
	}
#ifndef AMOEBA
	if(ignoreInputs) {
	    if (ignoreEvents) return -1; /* then only doing timers */
	    for (d = 0; d < app->count; d++) {
		if (FD_ISSET(ConnectionNumber(app->list[d]), (fd_set *)&rmaskfd)) {
		    if (XEventsQueued( app->list[d], QueuedAfterReading ))
			return d;
		    /*
		     * An error event could have arrived
		     * without any real events, or events
		     * could have been swallowed by Xlib,
		     * or the connection may be broken.
		     * We can't tell the difference, so
		     * ssume Xlib will eventually discover
		     * a broken connection.
		     */
		}
	    }
	    goto WaitLoop;	/* must have been only error events */
        }
	{
	int ret = -1;
	Boolean found_input = False;

	for (i = 0; i < app->fds.nfds && nfound > 0; i++) {
	    XtInputMask condition = 0;
	    if (FD_ISSET (i, (fd_set *)&rmaskfd)) {
		nfound--;
		if (!ignoreEvents) {
		    for (d = 0; d < app->count; d++) {
			if (i == ConnectionNumber(app->list[d])) {
			    if (ret == -1) {
				if (XEventsQueued( app->list[d],
						   QueuedAfterReading ))
				    ret = d;
				/*
				 * An error event could have arrived
				 * without any real events, or events
				 * could have been swallowed by Xlib,
				 * or the connection may be broken.
				 * We can't tell the difference, so
				 * assume Xlib will eventually discover
				 * a broken connection.
				 */
			    }
			    goto ENDILOOP;
			}
		    }
		}
		condition = XtInputReadMask;
	    }
	    if (FD_ISSET (i, (fd_set *)&wmaskfd)) {
		condition |= XtInputWriteMask;
		nfound--;
	    }
	    if (FD_ISSET (i, (fd_set *)&emaskfd)) {
		condition |= XtInputExceptMask;
		nfound--;
	    }
	    if (condition) {
		InputEvent *ep;
		for (ep = app->input_list[i]; ep; ep = ep->ie_next) {
		    if (condition & ep->ie_condition) {
			ep->ie_oq = app->outstandingQueue;
			app->outstandingQueue = ep;
		    }
		}
		found_input = True;
	    }
ENDILOOP:   ;
	} /* endfor */
	if (ret >= 0 || found_input)
	    return ret;
	goto WaitLoop;		/* must have been only error events */
	}
#else
	return nfound ? 0 : -1;
#endif
}
#else /* _MINIX */
#if NeedFunctionPrototypes
int _XtwaitForSomething(
	_XtBoolean ignoreTimers,
	_XtBoolean ignoreInputs,
	_XtBoolean ignoreEvents,
	_XtBoolean block,
	unsigned long *howlong,
	XtAppContext app
        )
#else
int _XtwaitForSomething(ignoreTimers, ignoreInputs, ignoreEvents,
			block, howlong, app)
	Boolean ignoreTimers;
	Boolean ignoreInputs;
	Boolean ignoreEvents;
	Boolean block;
	unsigned long *howlong;
	XtAppContext app;
#endif
{
/* MINIX version. Inputs are not supported. */

	struct timeval  cur_time;
	struct timeval  start_time;
	struct timeval  wait_time;
	struct timeval  new_time;
	struct timeval  time_spent;
	struct timeval	max_wait_time;
	static struct timeval  zero_time = { 0 , 0};
	register struct timeval *wait_time_ptr;
	int r, i, d, fd;
	asio_fd_set_t asio_bits;
	struct fwait fw;
	
 	if (block) {
		(void) gettimeofday (&cur_time, NULL);
		FIXUP_TIMEVAL(cur_time);
		start_time = cur_time;
		if(howlong == NULL) { /* special case for ever */
			wait_time_ptr = 0;
		} else { /* block until at most */
			max_wait_time.tv_sec = *howlong/1000;
			max_wait_time.tv_usec = (*howlong %1000)*1000;
			wait_time_ptr = &max_wait_time;
		}
	} else {  /* don't block */
		max_wait_time = zero_time;
		wait_time_ptr = &max_wait_time;
	}

      WaitLoop:
	while (1) {
		ASIO_FD_ZERO(&asio_bits);
		if (app->timerQueue != NULL && !ignoreTimers && block) {
		    if(IS_AFTER(cur_time, app->timerQueue->te_timer_value)) {
			TIMEDELTA (wait_time, app->timerQueue->te_timer_value, 
				   cur_time);
			if(howlong==NULL || IS_AFTER(wait_time,max_wait_time)){
				wait_time_ptr = &wait_time;
			} else {
				wait_time_ptr = &max_wait_time;
			}
		    } else wait_time_ptr = &zero_time;
		} 
		if (!ignoreEvents) {
		    for (d = 0; d < app->count; d++) {
		    	if (XEventsQueued(app->list[d], QueuedAfterReading))
			    return d;
			fd= MNX_XConnectionNumber(app->list[d]);
			assert(fd != -1);
			ASIO_FD_SET (fd, ASIO_READ, &asio_bits);
		    }
		}
		fw.fw_flags= 0;
		fw.fw_bits= asio_bits.afds_bits;
		fw.fw_maxfd= ASIO_FD_SETSIZE;
		if (wait_time_ptr)
			r= timed_fwait(&fw, wait_time_ptr);
		else
			r= fwait(&fw);
		if (r == -1) {
			/*
			 *  interrupt occured recalculate time value and select
			 *  again.
			 */
			if (errno == EINTR) {
			    errno = 0;  /* errno is not self reseting */
			    if (block) {
				if (wait_time_ptr == NULL) /*howlong == NULL*/
				    continue;
				(void)gettimeofday (&new_time, NULL);
				FIXUP_TIMEVAL(new_time);
				TIMEDELTA(time_spent, new_time, cur_time);
				cur_time = new_time;
				if(IS_AFTER(time_spent, *wait_time_ptr)) {
					TIMEDELTA(wait_time, *wait_time_ptr,
						  time_spent);
					wait_time_ptr = &wait_time;
					continue;
				}
				/* time is up */
			    }
			} else {
			    char Errno[12];
			    String param = Errno;
			    Cardinal param_count = 1;

			    sprintf( Errno, "%d", errno);
			    XtAppWarningMsg(app, "communicationError","fwait",
			       XtCXtToolkitError,"Select failed; error code %s",
			       &param, &param_count);
			    return -1;
			}
		} /* timed out or input available */
		break;
	}
	
	if (r == -1) {
		if(howlong) *howlong = (unsigned long)0;  /* Timed out */
		return -1;
	}
	assert(!ignoreEvents);
	if(block && howlong != NULL) { /* adjust howlong */
	    (void) gettimeofday (&new_time, NULL);
	    FIXUP_TIMEVAL(new_time);
	    TIMEDELTA(time_spent, new_time, start_time);
	    if(*howlong <= (time_spent.tv_sec*1000+time_spent.tv_usec/1000))
		*howlong = (unsigned long)0;  /* Timed out */
	    else
		*howlong -= (time_spent.tv_sec*1000+time_spent.tv_usec/1000);
	}

	for (d = 0; d < app->count; d++) {
	    fd= MNX_XConnectionNumber(app->list[d]);
	    if (fw.fw_fd != fd)
		continue;
	    assert(fw.fw_operation == ASIO_READ);
	    XReplyReadStatus(app->list[d], fw.fw_result, fw.fw_errno);
	    if (XEventsQueued( app->list[d], QueuedAfterReading ))
		return d;
	    /*
	     * An error event could have arrived
	     * without any real events, or events
	     * could have been swallowed by Xlib,
	     * or the connection may be broken.
	     * We can't tell the difference, so
	     * ssume Xlib will eventually discover
	     * a broken connection.
	     */
	}
	goto WaitLoop;	/* must have been only error events */
}
#endif /* _MINIX */

#define IeCallProc(ptr) \
    (*ptr->ie_proc) (ptr->ie_closure, &ptr->ie_source, (XtInputId*)&ptr);

#define TeCallProc(ptr) \
    (*ptr->te_proc) (ptr->te_closure, (XtIntervalId*)&ptr);

/*
 * Public Routines
 */

XtIntervalId XtAddTimeOut(interval, proc, closure)
	unsigned long interval;
	XtTimerCallbackProc proc;
	XtPointer closure;
{
	return XtAppAddTimeOut(_XtDefaultAppContext(), 
		interval, proc, closure); 
}


XtIntervalId XtAppAddTimeOut(app, interval, proc, closure)
	XtAppContext app;
	unsigned long interval;
	XtTimerCallbackProc proc;
	XtPointer closure;
{
	TimerEventRec *tptr;
        struct timeval current_time;

	if (freeTimerRecs) {
	    tptr = freeTimerRecs;
	    freeTimerRecs = tptr->te_next;
	}
	else tptr = XtNew(TimerEventRec);

	tptr->te_next = NULL;
	tptr->te_closure = closure;
	tptr->te_proc = proc;
	tptr->app = app;
	tptr->te_timer_value.tv_sec = interval/1000;
	tptr->te_timer_value.tv_usec = (interval%1000)*1000;
        (void) gettimeofday(&current_time, NULL);
	FIXUP_TIMEVAL(current_time);
        ADD_TIME(tptr->te_timer_value,tptr->te_timer_value,current_time);
	QueueTimerEvent(app, tptr);
	return( (XtIntervalId) tptr);
}

void  XtRemoveTimeOut(id)
    XtIntervalId id;
{
   TimerEventRec *t, *last, *tid = (TimerEventRec *) id;

   /* find it */

   for(t = tid->app->timerQueue, last = NULL;
	   t != NULL && t != tid;
	   t = t->te_next) last = t;

   if (t == NULL) return; /* couldn't find it */
   if(last == NULL) { /* first one on the list */
       t->app->timerQueue = t->te_next;
   } else last->te_next = t->te_next;

   t->te_next = freeTimerRecs;
   freeTimerRecs = t;
   return;
}

XtWorkProcId XtAddWorkProc(proc, closure)
	XtWorkProc proc;
	XtPointer closure;
{
	return XtAppAddWorkProc(_XtDefaultAppContext(), proc, closure);
}

XtWorkProcId XtAppAddWorkProc(app, proc, closure)
	XtAppContext app;
	XtWorkProc proc;
	XtPointer closure;
{
	WorkProcRec *wptr;

	if (freeWorkRecs) {
	    wptr = freeWorkRecs;
	    freeWorkRecs = wptr->next;
	} else wptr = XtNew(WorkProcRec);

	wptr->next = app->workQueue;
	wptr->closure = closure;
	wptr->proc = proc;
	wptr->app = app;
	app->workQueue = wptr;

	return (XtWorkProcId) wptr;
}

void  XtRemoveWorkProc(id)
	XtWorkProcId id;
{
	WorkProcRec *wid= (WorkProcRec *) id, *w, *last;

	/* find it */
	for(w = wid->app->workQueue, last = NULL; w != NULL && w != wid; w = w->next) last = w;

	if (w == NULL) return; /* couldn't find it */

	if(last == NULL) wid->app->workQueue = w->next;
	else last->next = w->next;

	w->next = freeWorkRecs;
	freeWorkRecs = w;
}

XtInputId XtAddInput( source, Condition, proc, closure)
	int source;
	XtPointer Condition;
	XtInputCallbackProc proc;
	XtPointer closure;
{
	return XtAppAddInput(_XtDefaultAppContext(),
		source, Condition, proc, closure);
}

XtInputId XtAppAddInput(app, source, Condition, proc, closure)
	XtAppContext app;
	int source;
	XtPointer Condition;
	XtInputCallbackProc proc;
	XtPointer closure;
{
#ifndef _MINIX
	InputEvent* sptr;
	XtInputMask condition = (XtInputMask) Condition;
	
	if (!condition ||
	    condition & ~(XtInputReadMask|XtInputWriteMask|XtInputExceptMask))
	    XtAppErrorMsg(app,"invalidParameter","xtAddInput",XtCXtToolkitError,
			  "invalid condition passed to XtAppAddInput",
			  (String *)NULL, (Cardinal *)NULL);

	if (app->input_max <= source) {
	    Cardinal n = source + 1;
	    app->input_list = (InputEvent**)XtRealloc((char*) app->input_list,
						      n * sizeof(InputEvent*));
	    bzero((char *) &app->input_list[app->input_max],
		  (unsigned) (n - app->input_max) * sizeof(InputEvent*));
	    app->input_max = n;
	}
	sptr = XtNew(InputEvent);
	sptr->ie_proc = proc;
	sptr->ie_closure = closure;
	sptr->app = app;
	sptr->ie_oq = NULL;
	sptr->ie_source = source;
	sptr->ie_condition = condition;
	sptr->ie_next = app->input_list[source];
	app->input_list[source] = sptr;

	if (condition & XtInputReadMask)   FD_SET(source, (fd_set *)&app->fds.rmask);
	if (condition & XtInputWriteMask)  FD_SET(source, (fd_set *)&app->fds.wmask);
	if (condition & XtInputExceptMask) FD_SET(source, (fd_set *)&app->fds.emask);

	if (app->fds.nfds < (source+1)) app->fds.nfds = source+1;
	app->fds.count++;
	return((XtInputId)sptr);

#else  /* _MINIX */
	printf("XtAppAddInput not implemented\n");
	abort();
#endif /* _MINIX */
}

void XtRemoveInput( id )
	register XtInputId  id;
{
#ifndef _MINIX
  	register InputEvent *sptr, *lptr;
	XtAppContext app = ((InputEvent *)id)->app;
	register int source = ((InputEvent *)id)->ie_source;
	Boolean found = False;

	sptr = app->outstandingQueue;
	lptr = NULL;
	for (; sptr != NULL; sptr = sptr->ie_oq) {
	    if (sptr == (InputEvent *)id) {
		if (lptr == NULL) app->outstandingQueue = sptr->ie_oq;
		else lptr->ie_oq = sptr->ie_oq;
	    }
	    lptr = sptr;
	}

	if(app->input_list && (sptr = app->input_list[source]) != NULL) {
		for( lptr = NULL ; sptr; sptr = sptr->ie_next ){
			if(sptr == (InputEvent *) id) {
				XtInputMask condition = 0;
				if(lptr == NULL) {
				    app->input_list[source] = sptr->ie_next;
				} else {
				    lptr->ie_next = sptr->ie_next;
				}
				for (lptr = app->input_list[source];
				     lptr; lptr = lptr->ie_next)
				    condition |= lptr->ie_condition;
				if ((sptr->ie_condition & XtInputReadMask) &&
				    !(condition & XtInputReadMask))
				   FD_CLR(source, (fd_set *)&app->fds.rmask);
				if ((sptr->ie_condition & XtInputWriteMask) &&
				    !(condition & XtInputWriteMask))
				   FD_CLR(source, (fd_set *)&app->fds.wmask);
				if ((sptr->ie_condition & XtInputExceptMask) &&
				    !(condition & XtInputExceptMask))
				   FD_CLR(source, (fd_set *)&app->fds.emask);
				XtFree((char *) sptr);
				found = True;
				break;
			}
			lptr = sptr;	      
		}
	}

    if (found)
	app->fds.count--;
    else
	XtAppWarningMsg(app, "invalidProcedure","inputHandler",XtCXtToolkitError,
                   "XtRemoveInput: Input handler not found",
		   (String *)NULL, (Cardinal *)NULL);
#else  /* _MINIX */
	printf("XtRemoveInput not implemented\n");
	abort();
#endif /* _MINIX */
}

void _XtRemoveAllInputs(app)
    XtAppContext app;
{
    int i;
    for (i = 0; i < app->input_max; i++) {
	InputEvent* ep = app->input_list[i];
	while (ep) {
	    InputEvent *next = ep->ie_next;
	    XtFree( (char*)ep );
	    ep = next;
	}
    }
    XtFree((char *) app->input_list);
}

/* Do alternate input and timer callbacks if there are any */

static void DoOtherSources(app)
	XtAppContext app;
{
	TimerEventRec *te_ptr;
	InputEvent *ie_ptr;
	struct timeval  cur_time;

#define DrainQueue() \
	for (ie_ptr = app->outstandingQueue; ie_ptr != NULL;) { \
	    app->outstandingQueue = ie_ptr->ie_oq;		\
	    ie_ptr ->ie_oq = NULL;				\
	    IeCallProc(ie_ptr);					\
	    ie_ptr = app->outstandingQueue;			\
	}
/*enddef*/
	DrainQueue();
	if (app->fds.count > 0) {
	    /* Call _XtwaitForSomething to get input queued up */
	    (void) _XtwaitForSomething(TRUE, FALSE, TRUE, FALSE,
		(unsigned long *)NULL, app);
	    DrainQueue();
	}
	if (app->timerQueue != NULL) {	/* check timeout queue */
	    (void) gettimeofday (&cur_time, NULL);
	    FIXUP_TIMEVAL(cur_time);
	    while(IS_AT_OR_AFTER (app->timerQueue->te_timer_value, cur_time)) {
		te_ptr = app->timerQueue;
		app->timerQueue = te_ptr->te_next;
		te_ptr->te_next = NULL;
		if (te_ptr->te_proc != NULL)
		    TeCallProc(te_ptr);
		te_ptr->te_next = freeTimerRecs;
		freeTimerRecs = te_ptr;
              if (app->timerQueue == NULL) break;
	    }
	}
#undef DrainQueue
}

/* If there are any work procs, call them.  Return whether we did so */

static Boolean CallWorkProc(app)
	XtAppContext app;
{
	register WorkProcRec *w = app->workQueue;
	Boolean delete;

	if (w == NULL) return FALSE;

	app->workQueue = w->next;

	delete = (*(w->proc)) (w->closure);

	if (delete) {
	    w->next = freeWorkRecs;
	    freeWorkRecs = w;
	}
	else {
	    w->next = app->workQueue;
	    app->workQueue = w;
	}
	return TRUE;
}

/*
 * XtNextEvent()
 * return next event;
 */

void XtNextEvent(event)
	XEvent *event;
{
	XtAppNextEvent(_XtDefaultAppContext(), event);
}

void _XtRefreshMapping(event, dispatch)
    XEvent *event;
    Boolean dispatch;
{
    XtPerDisplay pd = _XtGetPerDisplay(event->xmapping.display);

    if (event->xmapping.request != MappingPointer &&
	pd && pd->keysyms && (event->xmapping.serial >= pd->keysyms_serial))
	_XtBuildKeysymTables( event->xmapping.display, pd );
    XRefreshKeyboardMapping(&event->xmapping);
    if (dispatch && pd && pd->mapping_callbacks)
	XtCallCallbackList((Widget) NULL,
			   (XtCallbackList)pd->mapping_callbacks,
			   (XtPointer)event );
}

void XtAppNextEvent(app, event)
	XtAppContext app;
	XEvent *event;
{
    int i, d;

    for (;;) {
	if (app->count == 0)
	    DoOtherSources(app);
	else {
	    for (i = 1; i <= app->count; i++) {
		d = (i + app->last) % app->count;
		if (d == 0) DoOtherSources(app);
		if (XEventsQueued(app->list[d], QueuedAfterReading))
		    goto GotEvent;
	    }
	    for (i = 1; i <= app->count; i++) {
		d = (i + app->last) % app->count;
		if (XEventsQueued(app->list[d], QueuedAfterFlush))
		    goto GotEvent;
	    }
	}

	/* We're ready to wait...if there is a work proc, call it */
	if (CallWorkProc(app)) continue;

	d = _XtwaitForSomething(FALSE, FALSE, FALSE, TRUE,
				(unsigned long *) NULL, app);

	if (d != -1) {
	  GotEvent:
	    XNextEvent (app->list[d], event);
	    app->last = d;
	    if (event->xany.type == MappingNotify)
		_XtRefreshMapping(event, False);
	    return;
	} 

    } /* for */
}
    
void XtProcessEvent(mask)
	XtInputMask mask;
{
	XtAppProcessEvent(_XtDefaultAppContext(), mask);
}

void XtAppProcessEvent(app, mask)
	XtAppContext app;
	XtInputMask mask;
{
	int i, d;
	XEvent event;
	struct timeval cur_time;

	if (mask == 0) return;

	for (;;) {
	    if (mask & XtIMTimer && app->timerQueue != NULL) {
		(void) gettimeofday (&cur_time, NULL);
		FIXUP_TIMEVAL(cur_time);
		if (IS_AT_OR_AFTER(app->timerQueue->te_timer_value, cur_time)){
		    TimerEventRec *te_ptr = app->timerQueue;
		    app->timerQueue = app->timerQueue->te_next;
		    te_ptr->te_next = NULL;
                    if (te_ptr->te_proc != NULL)
		        TeCallProc(te_ptr);
		    te_ptr->te_next = freeTimerRecs;
		    freeTimerRecs = te_ptr;
		    return;
		}
	    }
    
	    if (mask & XtIMAlternateInput) {
		if (app->fds.count > 0 && app->outstandingQueue == NULL) {
		    /* Call _XtwaitForSomething to get input queued up */
		    (void) _XtwaitForSomething(TRUE, FALSE, TRUE, FALSE,
			    (unsigned long *)NULL, app);
		}
		if (app->outstandingQueue != NULL) {
		    InputEvent *ie_ptr = app->outstandingQueue;
		    app->outstandingQueue = ie_ptr->ie_oq;
		    ie_ptr->ie_oq = NULL;
		    IeCallProc(ie_ptr);
		    return;
		}
	    }
    
	    if (mask & XtIMXEvent) {
		for (i = 1; i <= app->count; i++) {
		    d = (i + app->last) % app->count;
		    if (XEventsQueued(app->list[d], QueuedAfterReading))
			goto GotEvent;
		}
		for (i = 1; i <= app->count; i++) {
		    d = (i + app->last) % app->count;
		    if (XEventsQueued(app->list[d], QueuedAfterFlush))
			goto GotEvent;
		}
	    }

	    /* Nothing to do...wait for something */

	    if (CallWorkProc(app)) continue;

	    d = _XtwaitForSomething(
				    (mask & XtIMTimer ? FALSE : TRUE),
				    (mask & XtIMAlternateInput ? FALSE : TRUE),
				    (mask & XtIMXEvent ? FALSE : TRUE),
				    TRUE,
				    (unsigned long *) NULL, app);

	    if (mask & XtIMXEvent && d != -1) {
	      GotEvent:
		XNextEvent(app->list[d], &event);
		app->last = d;
		if (event.xany.type == MappingNotify) {
		    _XtRefreshMapping(&event, False);
		}
		XtDispatchEvent(&event);
		return;
	    } 
	
	}    
}

XtInputMask XtPending()
{
	return XtAppPending(_XtDefaultAppContext());
}

XtInputMask XtAppPending(app)
	XtAppContext app;
{
	struct timeval cur_time;
	int d;
	XtInputMask ret = 0;

/*
 * Check for pending X events
 */
	for (d = 0; d < app->count; d++) {
	    if (XEventsQueued(app->list[d], QueuedAfterReading)) {
		ret = XtIMXEvent;
		break;
	    }
	}
	if (ret == 0) {
	    for (d = 0; d < app->count; d++) {
		if (XEventsQueued(app->list[d], QueuedAfterFlush)) {
		    ret = XtIMXEvent;
		    break;
		}
	    }
	}

/*
 * Check for pending alternate input
 */
	if (app->timerQueue != NULL) {	/* check timeout queue */ 
	    (void) gettimeofday (&cur_time, NULL);
	    FIXUP_TIMEVAL(cur_time);
	    if ((IS_AT_OR_AFTER(app->timerQueue->te_timer_value, cur_time))  &&
                (app->timerQueue->te_proc != 0)) {
		ret |= XtIMTimer;
	    }
	}

	if (app->outstandingQueue != NULL) ret |= XtIMAlternateInput;
	else {
	    /* This won't cause a wait, but will enqueue any input */

	    if(_XtwaitForSomething(TRUE, FALSE, FALSE, FALSE, (unsigned long *) NULL,
		    app) != -1) ret |= XtIMXEvent;
	    if (app->outstandingQueue != NULL) ret |= XtIMAlternateInput;
	}
	return ret;
}

/* Peek at alternate input and timer callbacks if there are any */

static Boolean PeekOtherSources(app)
	XtAppContext app;
{
	struct timeval  cur_time;

	if (app->outstandingQueue != NULL) return TRUE;

	if (app->fds.count > 0) {
	    /* Call _XtwaitForSomething to get input queued up */
	    (void) _XtwaitForSomething(TRUE, FALSE, TRUE, FALSE,
		    (unsigned long *)NULL, app);
	    if (app->outstandingQueue != NULL) return TRUE;
	}

	if (app->timerQueue != NULL) {	/* check timeout queue */
	    (void) gettimeofday (&cur_time, NULL);
	    FIXUP_TIMEVAL(cur_time);
	    if (IS_AT_OR_AFTER (app->timerQueue->te_timer_value, cur_time))
		return TRUE;
	}

	return FALSE;
}

Boolean XtPeekEvent(event)
	XEvent *event;
{
	return XtAppPeekEvent(_XtDefaultAppContext(), event);
}

Boolean XtAppPeekEvent(app, event)
	XtAppContext app;
	XEvent *event;
{
	int i, d;
	Boolean foundCall = FALSE;
	
	for (i = 1; i <= app->count; i++) {
	    d = (i + app->last) % app->count;
	    if (d == 0) foundCall = PeekOtherSources(app);
	    if (XEventsQueued(app->list[d], QueuedAfterReading))
		goto GotEvent;
	}
	for (i = 1; i <= app->count; i++) {
	    d = (i + app->last) % app->count;
	    if (XEventsQueued(app->list[d], QueuedAfterFlush))
		goto GotEvent;
	}
	
	if (foundCall) {
	    event->xany.type = 0;
	    event->xany.display = NULL;
	    event->xany.window = 0;
	    return FALSE;
	}
	
	d = _XtwaitForSomething(FALSE, FALSE, FALSE, TRUE,
				(unsigned long *) NULL, app);
	
	if (d != -1) {
	  GotEvent:
	    XPeekEvent(app->list[d], event);
	    app->last = (d == 0 ? app->count : d) - 1;
	    return TRUE;
	}
	event->xany.type = 0;	/* Something else must be ready */
	event->xany.display = NULL;
	event->xany.window = 0;
	return FALSE;
}	

#ifdef _MINIX
#include <time.h>

void sig_func(sig)
int sig;
{
	signal(SIGALRM, sig_func);
	alarm(1);
}

int timed_fwait(fwp, tvp)
struct fwait *fwp;
struct timeval *tvp;
{
	struct timeval end_tim, curr_tim, diff_tim;
	time_t start_time, alarm_time, curr_time;
	void (*sigf) _ARGS(( int sig ));
	int r, err;

	gettimeofday(&curr_tim, NULL);
	ADD_TIME(end_tim, *tvp, curr_tim);
	start_time= time(NULL);
	sigf= signal(SIGALRM, sig_func);
	alarm_time= alarm(0);
	for(;;)
	{
		if (IS_AT_OR_AFTER(end_tim, curr_tim))
		{	
			err = EINTR;
			r = -1;
			break;
		}
		TIMEDELTA(diff_tim, end_tim, curr_tim);
		if (diff_tim.tv_usec)
			diff_tim.tv_sec++;
		assert(diff_tim.tv_sec > 0);
		alarm(diff_tim.tv_sec);
		r= fwait(fwp);
		if (r == -1 && errno == EINTR) {
			gettimeofday(&curr_tim, NULL);
		} else {
			err= errno;
			break;
		}
	}

	/* Restore alarm pending when timed_fwait was called */
	alarm(0);
	signal(SIGALRM, sigf);
	if (alarm_time)
	{
		curr_time= time(NULL);
		if (curr_time < start_time + alarm_time)
			alarm((start_time + alarm_time) - curr_time);
		else
			alarm(1);	/* best we can do */
	}

	errno= err;
	return r;
}

#endif /* _MINIX */
