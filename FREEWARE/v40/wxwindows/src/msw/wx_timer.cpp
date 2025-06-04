/*
 * File:	wx_timer.cc
 * Purpose:	wxTimer implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_timer.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:17 am
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_setup.h"
#include "wx_list.h"
#include "wx_main.h"
#endif

#include "wx_timer.h"
#include <time.h>
#include <sys/types.h>
#if !defined(__SC__) && !defined(GNUWIN32) && !defined(__MWERKS__)
#include <sys/timeb.h>
#endif
#ifdef WIN32
#define _EXPORT /**/
#else
#define _EXPORT _export
#endif

wxList wxTimerList(wxKEY_INTEGER);
UINT WINAPI _EXPORT wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

wxTimer::wxTimer(void)
{
  id = 0;
}

wxTimer::~wxTimer(void)
{
  Stop();

  wxTimerList.DeleteObject(this);
}

Bool wxTimer::Start(int milliseconds,Bool mode)
{
  oneShot = mode ;
  if (milliseconds < 0)
    milliseconds = lastMilli;

  if (milliseconds <= 0)
    return FALSE;

  lastMilli = milli = milliseconds;

  wxTimerList.DeleteObject(this);
  TIMERPROC wxTimerProcInst = (TIMERPROC) MakeProcInstance((FARPROC)wxTimerProc,
                                          wxhInstance);

  id = SetTimer(NULL, (UINT)(id ? id : 1), (UINT)milliseconds, wxTimerProcInst);
  if (id > 0)
  {
    wxTimerList.Append(id, this);
    return TRUE;
  }
  else return FALSE;
}

void wxTimer::Stop(void)
{
  if (id) {
    KillTimer(NULL, (UINT)id);
    wxTimerList.DeleteObject(this); /* @@@@ */
  }
  id = 0 ;
  milli = 0 ;
}

UINT WINAPI _EXPORT wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
  wxNode *node = wxTimerList.Find((long)idTimer);
  if (node)
  {
    wxTimer *timer = (wxTimer *)node->Data();
    if (timer->id==0)
      return(0) ; // Avoid to process spurious timer events
    if (timer->oneShot)
      timer->Stop() ;
    timer->Notify();
  }
  return 0;
}

