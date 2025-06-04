/*
 * File:	wx_timer.h
 * Purpose:	wxTimer - provides simple timer functionality
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_timer.h	1.2 5/9/94" */

#ifndef wx_timerh
#define wx_timerh

#include "wx_defs.h"
#include "wx_obj.h"
#include "wb_timer.h"

#ifdef IN_CPROTO
typedef       void    *wxTimer ;
#else

class WXDLLEXPORT wxTimer: public wxbTimer
{
  DECLARE_DYNAMIC_CLASS(wxTimer)

 public:
  long id;
 public:
  wxTimer(void);
  ~wxTimer(void);
  virtual Bool Start(int milliseconds = -1,Bool one_shot = FALSE); // Start timer
  virtual void Stop(void);                   // Stop timer
};

#endif // IN_CPROTO
#endif // wx_timerh
