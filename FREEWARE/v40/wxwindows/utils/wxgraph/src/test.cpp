/*
 * File:     test.cc
 * Purpose:  wxWindows Graph demo
 *
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wxgraph.h"
#include "test.h"

// Declare two frames
MyFrame   *frame = NULL;
wxMenuBar *menu_bar = NULL;
Bool      timer_on = FALSE;
wxBitmap  *test_bitmap = NULL;
wxIcon    *test_icon = NULL;

wxGraphLayout *myGraph = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

// For drawing lines in a canvas
float     xpos = -1;
float     ypos = -1;

// Must initialise these in OnInit, not statically
wxPen     *red_pen;
wxFont    *small_font;

float     zoom_factor = 1.0;

#ifdef wx_x
#include "aiai.xbm"
#endif

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a small font
  small_font = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);

  // Create the main frame window
  frame = new MyFrame(NULL, "Graph Test", 0, 0, 350, 350);

  // Give it a status line
  frame->CreateStatusLine(2);

  // Give it an icon
#ifdef wx_msw
  test_icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  test_icon = new wxIcon(aiai_bits, aiai_width, aiai_height);
#endif

  frame->SetIcon(test_icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_QUIT, "&Quit",                "Quit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, "&About",              "About Graph Test");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  MyCanvas *canvas = new MyCanvas(frame);
  canvas->GetDC()->SetPen(wxBLACK_PEN);
  canvas->GetDC()->SetFont(wxNORMAL_FONT);

  wxCursor *cursor = new wxCursor(wxCURSOR_PENCIL);
  canvas->SetCursor(cursor);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

  frame->canvas = canvas;

  myGraph = new wxGraphLayout(canvas->GetDC());
  myGraph->SetBoundingBox(30, 40, 250, 310);
  GraphTest(*myGraph);
  myGraph->Draw();

  frame->Show(TRUE);

  frame->SetStatusText("Hello, graph!");

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  wxDC *dc = NULL;
  if (canvas)
    dc = canvas->GetDC();
  switch (id)
  {
    case TEST_QUIT:
    {
      OnClose();
      if (myGraph)
        delete myGraph ;
      delete this;
      break;
    }
    case TEST_ABOUT:
    {
      (void)wxMessageBox("wxWindows graph library demo Vsn 1.50\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1993", "About graph test");
      break;
    }
  }
}

void MyFrame::Draw(wxDC& dc, Bool draw_bitmaps)
{
  if (myGraph)
    myGraph->Draw();
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style):
 wxCanvas(frame, x, y, w, h, style)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(void)
{
  frame->Draw(*(GetDC()), TRUE);
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

// Intercept character input
void MyCanvas::OnChar(wxKeyEvent& event)
{
  char buf[2];
  buf[0] = (char)event.KeyCode();
  buf[1] = 0;
  frame->SetStatusText(buf, 1);

  // Process the default behaviour
  wxCanvas::OnChar(event);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}

void quit_proc(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *box = (wxDialogBox *)but.GetParent();
  box->Show(FALSE);
  delete box;
}

// Gets some user input, and sets the status line
void button_proc(wxButton& but, wxCommandEvent& event)
{
  int choice = wxMessageBox("Press OK to continue", "Try me", wxOK | wxCANCEL);

  if (choice == wxOK)
  {
    char *text = wxGetTextFromUser("Enter some text", "Text input", "");
    if (text)
    {
      (void)wxMessageBox(text, "Result", wxOK);

      frame->SetStatusText(text);
    }
  }
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}

void GraphTest(wxGraphLayout& graph)
{
  graph.AddNode(1, "a");
  graph.AddNode(2, "b");
  graph.AddNode(3, "c");
  graph.AddNode(4, "d");
  graph.AddNode(5, "e");
  graph.AddNode(6, "f");
  graph.AddNode(7, "g");
  graph.AddNode(8, "h");
  graph.AddNode(9, "i");

  graph.AddArc(100, 1, 2);
  graph.AddArc(101, 1, 3);
  graph.AddArc(102, 2, 4);
  graph.AddArc(103, 1, 4);
  graph.AddArc(104, 3, 5);
  graph.AddArc(104, 3, 6);
  graph.AddArc(104, 3, 7);
  graph.AddArc(104, 4, 8);
  graph.AddArc(104, 8, 5);
  graph.AddArc(104, 8, 9);

  graph.DoLayout();
}


