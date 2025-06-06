/*
 * File:	wb_text.h
 * Purpose:	wxTextWindow - simple text subwindow class
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:03 am
 */

/* sccsid[] = "@(#)wb_text.h	1.2 5/9/94" */

#ifndef wxb_texth
#define wxb_texth

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_frame.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef IN_CPROTO
typedef       void    *wxbTextWindow ;
#else

WXDLLEXPORT extern Constdata char *wxTextWindowNameStr;

class WXDLLEXPORT wxbTextWindow: public wxWindow

// 16-bit Borland 4.0 doesn't seem to allow multiple inheritance with wxWindow and streambuf:
// it complains about deriving a huge class from the huge class streambuf. !!

#if (defined(__BORLANDC__) && !defined(WIN32)) || ( defined(__MWERKS__) || defined(WXUSINGDLL))
#define NO_TEXT_WINDOW_STREAM
#endif

#ifndef NO_TEXT_WINDOW_STREAM
, public streambuf
#endif

{
 public:
  char *file_name;

  wxbTextWindow(void);
  wxbTextWindow(wxWindow *parent, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0, Constdata char *name = wxTextWindowNameStr);
  ~wxbTextWindow(void);

#ifndef NO_TEXT_WINDOW_STREAM
  int overflow(int i);
  int sync(void);
  int underflow(void);
#endif

  virtual Bool LoadFile(char *file) = 0;
  virtual Bool SaveFile(char *file) = 0;
  virtual void WriteText(char *text) = 0;
  virtual void Clear(void) = 0;
  virtual void DiscardEdits(void) = 0;
  virtual Bool Modified(void) = 0;
  virtual char *GetContents(void) = 0;
  virtual void SetInsertionPoint(long pos) = 0;
  virtual void SetInsertionPointEnd(void) = 0;
  virtual long GetInsertionPoint(void) = 0;
  virtual long GetLastPosition(void) = 0;
  virtual long XYToPosition(long x, long y) = 0;
  virtual void PositionToXY(long pos, long *x, long *y) = 0;
  virtual int GetNumberOfLines(void) = 0;
  virtual void ShowPosition(long pos) = 0;
  virtual int GetLineLength(long lineNo) = 0;
  virtual int GetLineText(long lineNo, char *buf) = 0;
  virtual void Replace(long from, long to, char *value) = 0;
  virtual void Remove(long from, long to) = 0;
  virtual void SetSelection(long from, long to) = 0;
  virtual void Copy(void) = 0; // Copy selection to clipboard
  virtual void Paste(void) = 0; // Paste clipboard into text window
  virtual void Cut(void) = 0; // Copy selection to clipboard, then remove selection.
  virtual void SetEditable(Bool editable) = 0;
  inline virtual void SetFont(wxFont *theFont) { font = theFont; }
  inline virtual wxFont *GetFont(void) { return font; }

  virtual void OnDropFiles(int n, char *files[], int x, int y);

  wxbTextWindow& operator<<(char *s);
  wxbTextWindow& operator<<(int i);
  wxbTextWindow& operator<<(long i);
  wxbTextWindow& operator<<(float f);
  wxbTextWindow& operator<<(double d);
  wxbTextWindow& operator<<(char c);
};

#define wxTEXT_MAX_LINES 1000

#endif // IN_CPROTO
#endif // wxb_texth
