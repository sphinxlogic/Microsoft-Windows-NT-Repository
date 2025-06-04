/*
 * File:	wb_timer.cc
 * Purpose:	wxTimer implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_timer.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:05 am
 */

/* static const char sccsid[] = "@(#)wb_timer.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_list.h"
#include "wx_frame.h"
#include "wx_main.h"
#endif

#include "wx_timer.h"

#ifdef SVR4
#define SYSV
#endif

#include <time.h>
#include <sys/types.h>

#if defined(SVR4) || defined(SYSV) || defined(__sgi) || defined(__alpha) || defined(GNUWIN32)
#include <sys/time.h>
#endif

#if defined(sun) || defined(__osf__)
// At least on Sun, ftime is undeclared.
// Need to be verified on other platforms.
extern "C" int ftime(struct timeb *tp);
// extern "C" time_t time(time_t);
// #include <sys/timeb.h>
#if defined(SVR4) && !defined(__alpha)
// ditto for gettimeofday on Solaris 2.x.
extern "C" int gettimeofday(struct timeval *tp, void *);
#endif
#endif

wxbTimer::wxbTimer(void)
{
  WXSET_TYPE(wxTimer, wxTYPE_TIMER)
  
  milli = 0 ;
  lastMilli = -1 ;
}

wxbTimer::~wxbTimer(void)
{
}

// Override me!
void wxbTimer::Notify(void)
{
}

int wxbTimer::Interval(void)
{
  return milli ;
}

/*
 * Timer functions
 *
 */

long wxStartTime = 0;
void wxStartTimer(void)
{
#if defined(__xlC__) || defined(_AIX) || defined(SVR4) || defined(SYSV) || defined(GNUWIN32) // || defined(AIXV3)
  struct timeval tp;
#ifdef SYSV
  gettimeofday(&tp, (struct timezone *)NULL);
#else
  gettimeofday(&tp);
#endif
  wxStartTime = 1000*tp.tv_sec + tp.tv_usec/1000;
#elif (defined(__SC__) || defined(__sgi) || defined(__bsdi__) || defined(__alpha) || defined(__MWERKS__))
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
  wxStartTime = 1000*(60*(60*tp->tm_hour+tp->tm_min)+tp->tm_sec);
#else
  struct timeb tp;
  ftime(&tp);
  wxStartTime = 1000*tp.time + tp.millitm;
#endif
}

// Returns elapsed time in milliseconds
long wxGetElapsedTime(Bool resetTimer)
{
#if defined(__xlC__) || defined(_AIX) || defined(SVR4) || defined(SYSV) || defined(GNUWIN32) // || defined(AIXV3)
  struct timeval tp;
#ifdef SYSV
  gettimeofday(&tp, (struct timezone *)NULL);
#else
  gettimeofday(&tp);
#endif
  long oldTime = wxStartTime;
  long newTime = 1000*tp.tv_sec + tp.tv_usec / 1000;
  if (resetTimer)
    wxStartTime = newTime;
#elif (defined(__SC__) || defined(__sgi) || defined(__bsdi__) || defined(__alpha) || defined(__MWERKS__))
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
  long oldTime = wxStartTime;
  long newTime = 1000*(60*(60*tp->tm_hour+tp->tm_min)+tp->tm_sec);
  if (resetTimer)
    wxStartTime = newTime;
#else
  struct timeb tp;
  ftime(&tp);
  long oldTime = wxStartTime;
  long newTime = 1000*tp.time + tp.millitm;
  if (resetTimer)
    wxStartTime = newTime;
#endif
  return newTime - oldTime;
}

// EXPERIMENTAL: comment this out if it doesn't compile.
#if !defined(VMS) && !defined(__MWERKS__)
Bool wxGetLocalTime(long *timeZone, int *dstObserved)
{
#if (((defined(SYSV) && !defined(hpux)) || defined(__MSDOS__) || defined(wx_msw)) && !defined(GNUWIN32))
#ifdef __BORLANDC__
  /* Borland uses underscores */
  *timeZone = _timezone;
  *dstObserved = _daylight;
#else
  *timeZone = timezone;
  *dstObserved = daylight;
#endif
#elif defined(__xlC__) || defined(_AIX) || defined(SVR4) || defined(SYSV) || defined(GNUWIN32) // || defined(AIXV3)
  struct timeval tp;
#if defined(SYSV) || defined(GNUWIN32)
  struct timezone tz;
  gettimeofday(&tp, &tz);
  *timeZone = 60*(tz.tz_minuteswest);
  *dstObserved = tz.tz_dsttime;
#else
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
  *timeZone = tp->tm_gmtoff; // ???
  *dstOberved = tp->tm_isdst;
#endif
#else
// #error wxGetLocalTime not implemented.
  struct timeval tp;
  struct timezone tz;
  gettimeofday(&tp, &tz);
  *timeZone = 60*(tz.tz_minuteswest);
  *dstObserved = tz.tz_dsttime;
#endif
  return TRUE;
}
#endif

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long wxGetCurrentTime(void)
{
#if defined(__xlC__) || defined(_AIX) || defined(SVR4) || defined(SYSV) // || defined(AIXV3)
  struct timeval tp;
#ifdef SYSV
  gettimeofday(&tp, (struct timezone *)NULL);
#else
  gettimeofday(&tp);
#endif
  return tp.tv_sec;
#else // (defined(__SC__) || defined(__sgi) || defined(__bsdi__) || defined(__alpha))
  return time(0);
#endif
/*
#else
  struct timeb tp;
  ftime(&tp);
  return tp.time;
#endif
*/
}

