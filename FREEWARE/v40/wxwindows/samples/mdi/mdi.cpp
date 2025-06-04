/*
 * File:	mdi.cc
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1994
 * Updated:	
 * Copyright:	(c) 1994, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#endif

/*
 * Purpose:  MDI demo for wxWindows class library
 *           Run with no arguments for SDI style, with -mdi argument
 *           for MDI style windows.
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "mdi.h"

MyFrame *frame = NULL;
wxList my_children;
Bool isMDI = FALSE;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
wxPen *red_pen;

long top_frame_type = wxSDI | wxDEFAULT_FRAME;
long child_frame_type = wxSDI | wxDEFAULT_FRAME;

wxFont *small_font;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Find out if we're SDI or MDI
  if (argc > 1 && (strcmp(argv[1], "-mdi") == 0))
  {
    isMDI = TRUE;
    top_frame_type = wxMDI_PARENT | wxDEFAULT_FRAME;
    child_frame_type = wxMDI_CHILD | wxDEFAULT_FRAME;
  }

  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a small font
  small_font = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);

  // Create the main frame window
  frame = new MyFrame(NULL, "SDI/MDI Demo", 0, 0, 300, 400, top_frame_type);

  // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef wx_msw
  wxIcon *icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  wxIcon *icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MDI_NEW_WINDOW, "&New window");
  file_menu->Append(MDI_QUIT, "&Exit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(MDI_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Show(TRUE);

  // Essential - return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case MDI_QUIT:
    {
      OnClose();
      delete this;
      break;
    }
    case MDI_ABOUT:
    {
      (void)wxMessageBox("wxWindows 1.50 MDI Demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1994\nUsage: mdi.exe [-mdi]", "About MDI Demo");
      break;
    }
    case MDI_NEW_WINDOW:
    {
      // Make another frame, containing a canvas
      MyChild *subframe = new MyChild(frame, "Canvas Frame", 10, 10, 300, 300,
                             child_frame_type);

      char titleBuf[100];
      sprintf(titleBuf, "Canvas Frame %d", winNumber);
      subframe->SetTitle(titleBuf);
      winNumber ++;

      // Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef wx_msw
      wxIcon *icon = new wxIcon("chrt_icn");
#endif
#ifdef wx_x
      wxIcon *icon = new wxIcon("aiai.xbm");
#endif
      subframe->SetIcon(icon);

      // Give it a status line
      subframe->CreateStatusLine();

      // Make a menubar
      wxMenu *file_menu = new wxMenu;

      if (isMDI)
        file_menu->Append(MDI_NEW_WINDOW, "&New window");

      file_menu->Append(MDI_CHILD_QUIT, "&Close child");

      if (isMDI)
        file_menu->Append(MDI_QUIT, "&Exit");

      wxMenu *option_menu = new wxMenu;

      // Dummy option
      option_menu->Append(MDI_REFRESH, "&Refresh picture");

      wxMenu *help_menu = new wxMenu;
      help_menu->Append(MDI_ABOUT, "&About");

      wxMenuBar *menu_bar = new wxMenuBar;

      menu_bar->Append(file_menu, "&File");
      menu_bar->Append(option_menu, "&Options");
      menu_bar->Append(help_menu, "&Help");

      // Associate the menu bar with the frame
      subframe->SetMenuBar(menu_bar);

      int width, height;
      subframe->GetClientSize(&width, &height);
//      wxTextWindow *textsw = new wxTextWindow(subframe, 0, 0, width, height);

      MyCanvas *canvas = new MyCanvas(subframe, 0, 0, width, height);
      wxCursor *cursor = new wxCursor(wxCURSOR_PENCIL);
      canvas->SetCursor(cursor);
      subframe->canvas = canvas;

      // Give it scrollbars
      canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
      canvas->SetPen(red_pen);

      subframe->Show(TRUE);
      break;
    }
  }
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h):
 wxCanvas(frame, x, y, w, h)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(void)
{
  SetFont(small_font);
  SetPen(wxGREEN_PEN);
  DrawLine(0, 0, 200, 200);
  DrawLine(200, 0, 0, 200);

  SetBrush(wxCYAN_BRUSH);
  SetPen(wxRED_PEN);
  DrawRectangle(100, 100, 100, 50);
  DrawRoundedRectangle(150, 150, 100, 50, 20);

  DrawEllipse(250, 250, 100, 50);
  DrawSpline(50, 200, 50, 100, 200, 10);
  DrawLine(50, 230, 200, 230);
  DrawText("This is a test string", 50, 230);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
  SetPen(wxBLACK_PEN);
  float x, y;
  event.Position(&x, &y);
  if (xpos > -1 && ypos > -1 && event.Dragging())
  {
    DrawLine(xpos, ypos, x, y);
  }
  xpos = x;
  ypos = y;
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  // Must delete children
  wxNode *node = my_children.First();
  while (node)
  {
    MyChild *child = (MyChild *)node->Data();
    wxNode *next = node->Next();
    child->OnClose();
    delete child;
    node = next;
  }
  return TRUE;
}


MyChild::MyChild(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  canvas = NULL;
  my_children.Append(this);
}

MyChild::~MyChild(void)
{
  my_children.DeleteObject(this);
}

// Intercept menu commands
void MyChild::OnMenuCommand(int id)
{
  switch (id)
  {
    case MDI_CHILD_QUIT:
    {
      OnClose();
      delete this;
      break;
    }
    default:
    {
      frame->OnMenuCommand(id);
      break;
    }
  }
}

void MyChild::OnActivate(Bool active)
{
  if (active && canvas)
    canvas->SetFocus();
}

Bool MyChild::OnClose(void)
{
  return TRUE;
}


void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}
