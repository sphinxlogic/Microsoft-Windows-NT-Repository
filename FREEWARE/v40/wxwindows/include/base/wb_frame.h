/*
 * File:	wb_frame.h
 * Purpose:	wxFrame declaration
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_frame.h	1.2 5/9/94" */

#ifndef wxb_frameh
#define wxb_frameh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_win.h"

#define wxMAX_STATUS   5

#ifdef IN_CPROTO
typedef       void    *wxbFrame ;
#else

WXDLLEXPORT extern Constdata char *wxFrameNameStr;
WXDLLEXPORT extern Constdata char *wxStatusLineNameStr;

class wxMenuBar;
class wxPanel ;
class wxFrame;
class WXDLLEXPORT wxbFrame: public wxWindow
{
 public:
  Bool modal_showing;
  wxMenuBar *wx_menu_bar;
  wxIcon *icon;
  Bool status_line_exists;
  Bool status_widths_exists;
  int *status_widths;

  int nb_status;

  wxWindow *frameToolBar ;

  wxbFrame(void);
  wxbFrame(wxFrame *parent, Const char *title,
          int x=-1, int y=-1, int width=-1, int height=-1,
          long type = wxSDI | wxDEFAULT_FRAME, Constdata char *name = wxFrameNameStr);
  ~wxbFrame(void);

  Bool Create(wxFrame *parent, Const char *title,
          int x=-1, int y=-1, int width=-1, int height=-1,
          long type = wxSDI | wxDEFAULT_FRAME, Constdata char *name = wxFrameNameStr);

  // Override, e.g. to resize subwindows
  void OnSize(int x, int y);

  // The default thing is to set the focus for the first child window.
  // Override for your own behaviour.
  void OnActivate(Bool flag);

  // Default behaviour is to display a help string for the menu item.
  virtual void OnMenuSelect(int id);
  inline virtual void OnMenuCommand(int WXUNUSED(id)) {};         // Called on frame menu command

  // Set menu bar
  virtual void SetMenuBar(wxMenuBar *menu_bar) = 0;
  virtual wxMenuBar *GetMenuBar(void);

  // Set icon
  virtual void SetIcon(wxIcon *icon) = 0;

  // Create status line
  virtual void CreateStatusLine(int number = 1, Constdata char *name = wxStatusLineNameStr) = 0;

  // Set status line text
  virtual void SetStatusText(char *text, int number = 0) = 0;
  virtual Bool StatusLineExists(void);
  virtual void SetStatusWidths(int n, int *widths_field) = 0;
  virtual Bool StatusWidthsExists(void);

  void Centre(int direction = wxBOTH);

  // Iconize
  virtual void Iconize(Bool iconize) = 0;
  virtual Bool Iconized(void) = 0;
  // Windos 3.x maximize/restore
  virtual void Maximize(Bool maximize) = 0;

  virtual void LoadAccelerators(char *table) = 0;

  // Call this to simulate a menu command
  virtual void Command(int id);
  virtual void ProcessCommand(int id);

  // Toolbar (currently, for use by Windows MDI parent frames ONLY)
  virtual inline void SetToolBar(wxWindow *toolbar) { frameToolBar = toolbar; }
  virtual inline wxWindow *GetToolBar(void) { return frameToolBar; }
};

#endif // IN_CPROTO
#endif // wxb_frameh
