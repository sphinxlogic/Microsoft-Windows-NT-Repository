/*
 * File:	wb_timer.h
 * Purpose:	wxTimer - provides simple timer functionality
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:05 am
 */

/* sccsid[] = "@(#)wb_timer.h	1.2 5/9/94" */

#ifndef wxb_timerh
#define wxb_timerh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_obj.h"

#if (!defined(__SC__) && !defined(__sgi) && !defined(GNUWIN32) && !defined(__MWERKS__))
#include <sys/timeb.h>
#endif

#ifdef IN_CPROTO
typedef       void    *wxbTimer ;
#else

class WXDLLEXPORT wxbTimer: public wxObject
{
 public:
  Bool oneShot ;
  int  milli ;
  int  lastMilli ;
  wxbTimer(void);
  ~wxbTimer(void);
  virtual Bool Start(int milliseconds = -1,Bool one_shot=FALSE) = 0; // Start timer
  virtual void Stop(void) = 0;                   // Stop timer
  virtual void Notify(void);                 // Override this member
  virtual int Interval(void) ; // Returns the current interval time (0 if stop)
};

// Timer functions (milliseconds)
WXDLLEXPORT void wxStartTimer(void);
// Gets time since last wxStartTimer or wxGetElapsedTime
WXDLLEXPORT long wxGetElapsedTime(Bool resetTimer = TRUE);

// EXPERIMENTAL: comment this out if it doesn't compile.
WXDLLEXPORT Bool wxGetLocalTime(long *timeZone, int *dstObserved);

// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
WXDLLEXPORT long wxGetCurrentTime(void);

#endif // IN_CPROTO
#endif // wxb_timerh
