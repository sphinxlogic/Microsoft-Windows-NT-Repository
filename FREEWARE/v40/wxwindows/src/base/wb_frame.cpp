/*
 * File:	wb_frame.cc
 * Purpose:	wxFrame implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_frame.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

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
#include "wx_frame.h"
#include "wx_stdev.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_menu.h"
#include "wx_mnuit.h"
#include "wx_text.h"
#include "wx_dialg.h"
#endif

class wxFrame;

#include "wx_stdev.h"

wxbFrame::wxbFrame(void)
{
  WXSET_TYPE(wxFrame, wxTYPE_FRAME)
  
  nb_status = 0;
  frameToolBar = NULL ;
  modal_showing = FALSE;
  status_line_exists = FALSE;
  icon = NULL;
  wx_menu_bar = NULL;
  status_widths = NULL;
}

wxbFrame::wxbFrame(wxFrame *WXUNUSED(Parent), Const char *WXUNUSED(title), int WXUNUSED(x), int WXUNUSED(y),
                 int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  WXSET_TYPE(wxFrame, wxTYPE_FRAME)
  
  windowStyle = style;
  frameToolBar = NULL ;
  modal_showing = FALSE;
  status_line_exists = FALSE;
  icon = NULL;
  wx_menu_bar = NULL;
  status_widths = NULL;
}

Bool wxbFrame::Create(wxFrame *Parent, Const char *WXUNUSED(title), int WXUNUSED(x), int WXUNUSED(y),
                 int WXUNUSED(width), int WXUNUSED(height), long style, Constdata char *WXUNUSED(name))
{
  windowStyle = style;
#if !WXGARBAGE_COLLECTION_ON
  if (!Parent)
    wxTopLevelWindows.Append(this);
#endif
  return TRUE;
}

wxbFrame::~wxbFrame(void)
{
  wxTopLevelWindows.DeleteObject(this);
}

// Default resizing behaviour - if only ONE subwindow,
// resize to client rectangle size
void wxbFrame::OnSize(int WXUNUSED(x), int WXUNUSED(y))
{
#if DEBUG > 1
  wxDebugMsg("wxbFrame::OnSize\n");
#endif
  if ((windowStyle & wxMDI_PARENT) == wxMDI_PARENT)
    return;

  // Search for a child which is a subwindow, not another frame.
  wxWindow *child = NULL;
  // Count the number of _subwindow_ children
  int noChildren = 0;
  for(wxNode *node = GetChildren()->First(); node; node = node->Next())
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (win->IsKindOf(CLASSINFO(wxPanel)) ||
        win->IsKindOf(CLASSINFO(wxTextWindow)) ||
        win->IsKindOf(CLASSINFO(wxCanvas)))
    {
      child = win;
      noChildren ++;
    }
  }

  // If not one child, call the Layout function if compiled in
  if (!child || (noChildren > 1)
#if USE_CONSTRAINTS
   || GetAutoLayout()
#endif
   )
  {
#if USE_CONSTRAINTS
    if (GetAutoLayout())
      Layout();
#endif
    return;
  }
  
  if (child)
  {
    int client_x, client_y;

#if DEBUG > 1
    wxDebugMsg("wxbFrame::OnSize: about to set the child's size.\n");
#endif

    GetClientSize(&client_x, &client_y);
    child->SetSize(0, 0, client_x, client_y);
  }
}

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxbFrame::OnActivate(Bool flag)
{
  if (!flag || (GetWindowStyleFlag() & wxMDI_PARENT))
    return;

  for(wxNode *node = GetChildren()->First(); node; node = node->Next())
  {
    // Find a child that's a subwindow, but not a dialog box.
    wxWindow *child = (wxWindow *)node->Data();
    if ((child->IsKindOf(CLASSINFO(wxPanel)) &&
         !child->IsKindOf(CLASSINFO(wxDialogBox))) ||
        child->IsKindOf(CLASSINFO(wxTextWindow)) ||
        child->IsKindOf(CLASSINFO(wxCanvas)))
    {
#if DEBUG > 1
      wxDebugMsg("wxbFrame::OnActivate: about to set the child's focus.\n");
#endif
      child->SetFocus();
      return;
    }
  }
}

// Default menu selection behaviour - display a help string
void wxbFrame::OnMenuSelect(int id)
{
  if (StatusLineExists())
  {
    if (id == -1)
      SetStatusText("");
    else
    {
      wxMenuBar *menuBar = GetMenuBar();
      if (menuBar)
      {
        char *helpString = GetMenuBar()->GetHelpString(id);
        if (helpString)
          SetStatusText(helpString);
      }
    }
  }
}

wxMenuBar *wxbFrame::GetMenuBar(void)
{
  return wx_menu_bar;
}

Bool wxbFrame::StatusLineExists(void)
{
  return status_line_exists;
}

Bool wxbFrame::StatusWidthsExists(void)
{
  return status_widths_exists;
}

void wxbFrame::Centre(int direction)
{
  int display_width, display_height, width, height, x, y;
  wxDisplaySize(&display_width, &display_height);

  GetSize(&width, &height);
  GetPosition(&x, &y);

  if (direction & wxHORIZONTAL)
    x = (int)((display_width - width)/2);
  if (direction & wxVERTICAL)
    y = (int)((display_height - height)/2);

  SetSize(x, y, width, height);
}

// Call this to simulate a menu command
void wxbFrame::Command(int id)
{
  ProcessCommand(id);
}

void wxbFrame::ProcessCommand(int id)
{
  wxCommandEvent commandEvent(wxEVENT_TYPE_MENU_COMMAND);
  commandEvent.commandInt = id;

  if (wxNotifyEvent(commandEvent, TRUE))
    return;

  wxMenuBar *bar = GetMenuBar() ;
  if (!bar)
    return;

  // Motif does the job by itself!!
#ifndef wx_motif
  wxMenuItem *item = bar->FindItemForId(id) ;
  if (item && item->checkable)
  {
//wxDebugMsg("Toggling id %d\n",id) ;
    bar->Check(id,!bar->Checked(id)) ;
  }
#endif
  OnMenuCommand(id);

  wxNotifyEvent(commandEvent, FALSE);
}
