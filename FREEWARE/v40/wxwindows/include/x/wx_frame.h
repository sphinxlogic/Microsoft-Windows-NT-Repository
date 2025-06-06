/*
 * File:	wx_frame.h
 * Purpose:	wxFrame declaration (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_frame.h	1.2 5/9/94" */

#ifndef wx_frameh
#define wx_frameh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wb_frame.h"

#ifdef wx_motif
#include <Xm/Xm.h>
#endif

#ifdef IN_CPROTO
typedef       void    *wxFrame ;
#else

class wxPanel;
class wxMenuBar;

extern Constdata char *wxFrameNameStr;
extern Constdata char *wxStatusLineNameStr;

class wxFrame: public wxbFrame
{
  DECLARE_DYNAMIC_CLASS(wxFrame)

 private:
  char *frameTitle;
 public:
  // Store current size so we can ignore resize if only
  // a reposition!
  int lastWidth;
  int lastHeight;
  int visibleStatus; /* used by show-&-hide sequence fix */

#ifdef wx_motif
  Widget frameShell;
  Widget frameWidget;
  Widget workArea;
  Widget clientArea;
  Widget menuBarWidget;
  Widget statusLineWidget;
  Widget statusLineForm;
  Widget statusTextWidget[wxMAX_STATUS];
  Widget GetMenuBarWidget(void);
  Bool PreResize(void);
#endif
#ifdef wx_xview
  Bool isPinned;
  wxPanel *menu_bar_panel;
  int y_offset;
  char *statusText[wxMAX_STATUS];
#endif

  wxFrame(void);
  wxFrame(wxFrame *parent, char *title,
          int x=-1, int y=-1, int width=-1, int height=-1,
          long style = wxSDI | wxDEFAULT_FRAME, char *name = "frame");

  ~wxFrame(void);

  Bool Create(wxFrame *parent, char *title,
          int x=-1, int y=-1, int width=-1, int height=-1,
          long style = wxSDI | wxDEFAULT_FRAME, char *name = "frame");

  void SetClientSize(int width, int height);
  void GetClientSize(int *width, int *height);

  void GetSize(int *width, int *height);
  void GetPosition(int *x, int *y);
  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  // Avoid compiler warning
  void SetSize(int w, int h) { wxbFrame::SetSize(w, h); }
  Bool Show(Bool show);

  // Set menu bar
  void SetMenuBar(wxMenuBar *menu_bar);

  // Set title
  char *GetTitle(void);
  void SetTitle(char *title);

  // Set icon
  void SetIcon(wxIcon *icon);

  // Create status line
  void CreateStatusLine(int number=1, Constdata char *name = wxStatusLineNameStr);

  // Set status line text
  void SetStatusText(char *text, int number = 0);

  // Set status line widths
  void SetStatusWidths(int n, int *widths_field);

  // Fit frame around subwindows
  void Fit(void);

  // Iconize
  void Iconize(Bool iconize);
  Bool Iconized(void);
  // Windos 3.x maximize/restore
  void Maximize(Bool maximize);

  void LoadAccelerators(char *table);

  void CaptureMouse(void);
  void ReleaseMouse(void);
};

#endif // IN_CPROTO
#endif // wx_frameh
