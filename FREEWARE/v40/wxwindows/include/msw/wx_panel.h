/*
 * File:	wx_panel.h
 * Purpose:	wxPanel subwindow, for panel items (widgets/controls)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_panel.h	1.2 5/9/94" */

#ifndef wx_panelh
#define wx_panelh

#include "wx_defs.h"
#include "wx_win.h"
#include "wb_panel.h"

#define PANEL_LEFT_MARGIN 4
#define PANEL_TOP_MARGIN  4
#define PANEL_HSPACING  8
#define PANEL_VSPACING  8

#ifdef IN_CPROTO
typedef       void    *wxPanel ;
#else

WXDLLEXPORT extern Constdata char *wxPanelNameStr;

class wxItem;
class wxFrame;
class WXDLLEXPORT wxPanel: public wxbPanel
{
  DECLARE_DYNAMIC_CLASS(wxPanel)

 public:
  // For panel item positioning.
  int cursor_x;
  int cursor_y;
  int max_width;
  int max_height;
  int max_line_height;
  wxWindow *last_created ;

  wxBrush *backBrush ;
  PAINTSTRUCT *tempPS;

  wxPanel(void);
  wxPanel(wxWindow *parent,
          int x=-1, int y=-1, int width=-1, int height=-1, long style=0,
          Constdata char *name = wxPanelNameStr);

  ~wxPanel(void);

  Bool Create(wxWindow *window,
          int x=-1, int y=-1, int width=-1, int height=-1, long style=0,
          Constdata char *name = wxPanelNameStr);

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void Centre(int direction) ;
  virtual void Enable(Bool Flag) ;
  void AddChild(wxObject *child) ;

  // Set current label position, i.e. will label be on top or to the left
  // Doesn't do anything right now.
  void SetLabelPosition(int pos);  // wxHORIZONTAL or wxVERTICAL

  // Start a new line
  void NewLine(void);
  void NewLine(int pixels);
  void RealNewLine(void) ;

  // Tab specified number of pixels
  void Tab(void);

  void Tab(int pixels);

  void GetCursor(int *x, int *y);

  // Set/get horizontal spacing
  void SetHorizontalSpacing(int sp);
  int GetHorizontalSpacing(void);

  // Set/get vertical spacing
  void SetVerticalSpacing(int sp);
  int GetVerticalSpacing(void);

  // Fits the panel around the items
  void Fit(void);

  // For panel item positioning.
  // Update next cursor position
  void AdvanceCursor(wxWindow *item);
  void RealAdvanceCursor(void);

  // If x or y are not specified (i.e. < 0), supply
  // values based on left to right, top to bottom layout.
  // Internal use only.
  void GetValidPosition(int *x, int *y);

  void SetBackgroundColour(wxColour& col) ;
  void SetLabelColour(wxColour& col) ;
  void SetButtonColour(wxColour& col) ;

#if USE_EXTENDED_STATICS
  virtual void DrawAllStaticItems(PAINTSTRUCT *ps);
#endif
  virtual void OnPaint(void);
};

#endif // IN_CPROTO
#endif // wx_panelh
