/*
 * File:	wb_text.cc
 * Purpose:	wxTextWindow implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_text.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_text.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx_defs.h"
#include "wx_text.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_gdi.h"
#endif

wxbTextWindow::wxbTextWindow(void)
#ifndef NO_TEXT_WINDOW_STREAM
 :streambuf()
#endif
{
  WXSET_TYPE(wxTextWindow, wxTYPE_TEXT_WINDOW)
  
  file_name = NULL;
  window_parent = NULL;
  font = wxSWISS_FONT;
#ifdef wx_msw
  font->UseResource();
#endif
#ifndef NO_TEXT_WINDOW_STREAM
  if( allocate() != EOF) setp(base(),ebuf());
#endif
}

wxbTextWindow::wxbTextWindow(wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(width), int WXUNUSED(height),
                           long style, Constdata char *WXUNUSED(name))
#ifndef NO_TEXT_WINDOW_STREAM
 :streambuf()
#endif
{
  WXSET_TYPE(wxTextWindow, wxTYPE_TEXT_WINDOW)
  
  windowStyle = style;
  file_name = NULL;
  window_parent = parent;
  font = wxSWISS_FONT;
#ifdef wx_msw
  font->UseResource();
#endif

#ifndef NO_TEXT_WINDOW_STREAM
  if( allocate()!= EOF) setp(base(),ebuf());
#endif
}

wxbTextWindow::~wxbTextWindow(void)
{
}

void wxbTextWindow::OnDropFiles(int n, char *files[], int WXUNUSED(x), int WXUNUSED(y))
{
  // By default, load the first file into the text window.
  if (n > 0)
  {
    LoadFile(files[0]);
  }
}

// The streambuf code was partly taken from chapter 3 by Jerry Schwarz of
// AT&T's "C++ Lanuage System Release 3.0 Library Manual" - Stein Somers

//=========================================================================
// Called then the buffer is full (gcc 2.6.3) 
// or when "endl" is output (Borland 4.5)
//=========================================================================
// Class declaration using multiple inheritance doesn't work properly for
// Borland. See note in wb_text.h.
#ifndef NO_TEXT_WINDOW_STREAM
int wxbTextWindow::overflow(int c)
{
  // Make sure there is a holding area
  if ( allocate()==EOF )
  {
    wxError("Streambuf allocation failed","Internal error");
    return EOF;
  }
  
  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  // Reset get area
  setg(0,0,0);

  // Make sure there is a put area
  if ( ! pptr() )
  {
    wxError("Put area not opened","Internal error");
    setp( base(), base() );
  }


  // Determine how many characters have been inserted but no consumed
  int plen = pptr() - pbase();

  // Now Jerry relies on the fact that the buffer is at least 2 chars
  // long, but the holding area "may be as small as 1" ???
  // And we need an additional \0, so let's keep this inefficient but
  // safe copy.

  // If c!=EOF, it is a character that must also be comsumed
  int xtra = c==EOF? 0 : 1;

  // Write temporary C-string to wxTextWindow
  {
  char *txt = new char[plen+xtra+1];
  memcpy(txt, pbase(), plen);
  txt[plen] = (char)c;     // append c
  txt[plen+xtra] = '\0';   // append '\0' or overwrite c
    // If the put area already contained \0, output will be truncated there
  WriteText(txt);
    delete[] txt;
  }

  // Reset put area
  setp(pbase(), epptr());

#if defined(__WATCOMC__)
  return __NOT_EOF;
#elif defined(zapeof)     // HP-UX (all cfront based?)
  return zapeof(c);
#else
  return c!=EOF ? c : 0;  // this should make everybody happy
#endif

/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return EOF;
*/
}

//=========================================================================
// called then "endl" is output (gcc) or then explicit sync is done (Borland)
//=========================================================================
int wxbTextWindow::sync(void)
{
  // Verify that there are no characters in get area
  if ( gptr() && gptr() < egptr() )
  {
     wxError("Who's trespassing my get area?","Internal error");
     return EOF;
  }

  if ( pptr() && pptr() > pbase() ) return overflow(EOF);

  return 0;
/* OLD CODE
  int len = pptr() - pbase();
  char *txt = new char[len+1];
  strncpy(txt, pbase(), len);
  txt[len] = '\0';
  (*this) << txt;
  setp(pbase(), epptr());
  delete[] txt;
  return 0;
*/
}

//=========================================================================
// Should not be called by a "ostream". Used by a "istream"
//=========================================================================
int wxbTextWindow::underflow(void)
{
  return EOF;
}
#endif

wxbTextWindow& wxbTextWindow::operator<<(char *s)
{
  WriteText(s);
  return *this;
}

wxbTextWindow& wxbTextWindow::operator<<(float f)
{
  static char buf[100];
  sprintf(buf, "%.2f", f);
  WriteText(buf);
  return *this;
}

wxbTextWindow& wxbTextWindow::operator<<(double d)
{
  static char buf[100];
  sprintf(buf, "%.2f", d);
  WriteText(buf);
  return *this;
}

wxbTextWindow& wxbTextWindow::operator<<(int i)
{
  static char buf[100];
  sprintf(buf, "%i", i);
  WriteText(buf);
  return *this;
}

wxbTextWindow& wxbTextWindow::operator<<(long i)
{
  static char buf[100];
  sprintf(buf, "%ld", i);
  WriteText(buf);
  return *this;
}

wxbTextWindow& wxbTextWindow::operator<<(char c)
{
  char buf[2];

  buf[0] = c;
  buf[1] = 0;
  WriteText(buf);
  return *this;
}

