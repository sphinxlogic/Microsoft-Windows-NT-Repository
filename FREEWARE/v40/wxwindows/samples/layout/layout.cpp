/*
 * File:	layout.cc
 * Purpose:	Constraint-based layout demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
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
#include "wx.h"
#endif

#if !USE_CONSTRAINTS
#error You must set USE_CONSTRAINTS to 1 in wx_setup.h!
#endif

#include <ctype.h>
#include "layout.h"

// Declare two frames
MyFrame   *frame = NULL;
wxMenuBar *menu_bar = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

#ifdef wx_x
#include "aiai.xbm"
#endif

void TestSizer(void);

MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "wxWindows Layout Demo", 0, 0, 550, 500);

  frame->SetAutoLayout(TRUE);

  // Give it a status line
  frame->CreateStatusLine(2);

  // Load icon and bitmap
#ifdef wx_msw
  wxIcon *test_icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  wxIcon *test_icon = new wxIcon(aiai_bits, aiai_width, aiai_height);
#endif

  frame->SetIcon(test_icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(LAYOUT_LOAD_FILE, "&Load file",      "Load a text file");
  file_menu->Append(LAYOUT_TEST, "&Test sizers",      "Test sizer code");

  file_menu->AppendSeparator();
  file_menu->Append(LAYOUT_QUIT, "E&xit",                "Quit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(LAYOUT_ABOUT, "&About",              "About layout demo");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 1000, 500, 0);
//  frame->panel->SetAutoLayout(TRUE);
  frame->panel->SetLabelPosition(wxVERTICAL);

  // Create some panel items
  wxButton *btn1 = new wxButton(frame->panel, (wxFunction)&button_proc, "A button (1)") ;

  wxLayoutConstraints *b1 = new wxLayoutConstraints;
  b1->centreX.SameAs    (frame->panel, wxCentreX);
  b1->top.SameAs        (frame->panel, wxTop, 5);
  b1->width.PercentOf   (frame->panel, wxWidth, 80);
  b1->height.PercentOf  (frame->panel, wxHeight, 10);
  btn1->SetConstraints(b1);

  wxListBox *list = new wxListBox(frame->panel, (wxFunction)NULL, "A list",
                                  wxSINGLE, -1, -1, 200, 100);
  list->Append("Apple");
  list->Append("Pear");
  list->Append("Orange");
  list->Append("Banana");
  list->Append("Fruit");

  wxLayoutConstraints *b2 = new wxLayoutConstraints;
  b2->top.Below         (btn1, 5);
  b2->left.SameAs       (frame->panel, wxLeft, 5);
  b2->width.PercentOf   (frame->panel, wxWidth, 40);
  b2->bottom.SameAs     (frame->panel, wxBottom, 5);
  list->SetConstraints(b2);

  wxMultiText *mtext = new wxMultiText(frame->panel, (wxFunction)NULL, "Multiline text", "Some text",
                        -1, -1, 150, 100);

  wxLayoutConstraints *b3 = new wxLayoutConstraints;
  b3->top.Below         (btn1, 5);
  b3->left.RightOf      (list, 5);
  b3->right.SameAs      (frame->panel, wxRight, 5);
  b3->bottom.SameAs     (frame->panel, wxBottom, 5);
  mtext->SetConstraints(b3);

  frame->canvas = new MyCanvas(frame, 0, 0, 400, 400, wxRETAINED);
  frame->canvas->SetBackground(wxWHITE_BRUSH);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
//  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

  // Make a text window
  frame->text_window = new MyTextWindow(frame, 0, 250, 400, 250, wxNATIVE_IMPL);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (frame, wxLeft);
  c1->top.SameAs        (frame, wxTop);
  c1->right.PercentOf   (frame, wxWidth, 50);
  c1->height.PercentOf  (frame, wxHeight, 50);

  frame->panel->SetConstraints(c1);

  // Set constraints for canvas subwindow
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (frame->panel, wxRight);
  c2->top.SameAs        (frame, wxTop);
  c2->right.SameAs      (frame, wxRight);
  c2->height.PercentOf  (frame, wxHeight, 50);

  frame->canvas->SetConstraints(c2);

  // Set constraints for text subwindow
  wxLayoutConstraints *c3 = new wxLayoutConstraints;
  c3->left.SameAs       (frame, wxLeft);
  c3->top.Below         (frame->panel);
  c3->right.SameAs      (frame, wxRight);
  c3->bottom.SameAs     (frame, wxBottom);

  frame->text_window->SetConstraints(c3);

  frame->Show(TRUE);

  frame->SetStatusText("wxWindows layout demo");
  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel = NULL;
  text_window = NULL;
  canvas = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case LAYOUT_LOAD_FILE:
    {
      char *s = wxFileSelector("Load text file", NULL, NULL, NULL, "*.txt");
      if (s)
      {
        frame->text_window->LoadFile(s);
      }
      break;
    }
    case LAYOUT_QUIT:
    {
      OnClose();
      delete this;
      break;
    }
    case LAYOUT_TEST:
    {
      TestSizer();
      break;
    }
    case LAYOUT_ABOUT:
    {
      (void)wxMessageBox("wxWindows GUI library layout demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1995",
            "About Layout Demo", wxOK|wxCENTRE);
      break;
    }
    default:
    {
      break;
    }
  }
}

// Size the subwindows when the frame is resized
void MyFrame::OnSize(int w, int h)
{
  Layout();
}

void MyFrame::Draw(wxDC& dc, Bool draw_bitmaps)
{
  dc.SetPen(wxGREEN_PEN);
  dc.DrawLine(0.0, 0.0, 200.0, 200.0);
  dc.DrawLine(200.0, 0.0, 0.0, 200.0);

  dc.SetBrush(wxCYAN_BRUSH);
  dc.SetPen(wxRED_PEN);

  dc.DrawRectangle(100.0, 100.0, 100.0, 50.0);
  dc.DrawRoundedRectangle(150.0, 150.0, 100.0, 50.0,20.0);

  dc.DrawEllipse(250.0, 250.0, 100.0, 50.0);
  dc.DrawSpline(50.0, 200.0, 50.0, 100.0, 200.0, 10.0);
  dc.DrawLine(50.0, 230.0, 200.0, 230.0);

  dc.SetPen(wxBLACK_PEN);
  dc.DrawArc(50.0, 300.0, 100.0, 250.0, 100.0, 300.0);
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style):
 wxCanvas(frame, x, y, w, h, style)
{
}

MyCanvas::~MyCanvas(void)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(void)
{
  // need the last param for cl386.
  frame->Draw(*(GetDC()),TRUE);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button. Right button activates a menu.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
}

// Intercept character input
void MyCanvas::OnChar(wxKeyEvent& event)
{
  char buf[2];
  buf[0] = (char)event.KeyCode();
  buf[1] = 0;
  frame->SetStatusText(buf,1);

  // Process the default behaviour
  wxCanvas::OnChar(event);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

// Gets some user input, and sets the status line
void button_proc(wxButton& but, wxCommandEvent& event)
{
  int choice ;
  
  choice = wxMessageBox("Press OK to continue", "Try me",
                        wxOK | wxCANCEL,frame) ;
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

// Put some text into the text window
void list_proc(wxListBox& list, wxCommandEvent& event)
{
  // Test of list callback
#ifdef wx_x
  wxDebugMsg ("List select. Item %d string '%s'\n",
              event.commandInt,event.commandString) ;
#endif
  *(frame->text_window) << event.commandString;
#ifdef wx_x
  int *liste,count ;
  count = list.GetSelections(&liste) ;
  for (int i =0;i<count;i++)
	wxDebugMsg("Selection %d\n",liste[i]) ;
  wxDebugMsg("\n") ;
#endif
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}

void MyTextWindow::OnChar(wxKeyEvent& event)
{
  if (event.keyCode == WXK_RETURN)
    frame->SetStatusText("Pressed Enter.");

  if (event.keyCode == WXK_DELETE)
    frame->SetStatusText("Pressed Delete.");

  if (isdigit(event.keyCode))
  {
    wxTextWindow::OnChar(event);
  }
  else if (isascii(event.keyCode))
  {
    event.keyCode = toupper(event.keyCode);
    wxTextWindow::OnChar(event);
  }
}

SizerFrame::SizerFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel = new wxPanel(this);

  // A sizer to fit the whole panel, plus two sizers, one
  // above the other. A button is centred on the lower
  // sizer; a rowcol containing 3 buttons is centred on the upper
  // sizer.
  wxSizer *expandSizer = new wxSizer(panel, wxSizerExpand);
  expandSizer->SetName("expandSizer");

  wxLayoutConstraints *c;

  /////// TOP OF PANEL
  ///////
  wxSizer *topSizer = new wxSizer(expandSizer);
  topSizer->SetName("topSizer");

  // Specify constraints for the top sizer
  c = new wxLayoutConstraints;
  c->left.SameAs       (expandSizer, wxLeft);
  c->top.SameAs        (expandSizer, wxTop);
  c->right.SameAs      (expandSizer, wxRight);
  c->height.PercentOf  (expandSizer, wxHeight, 50);

  topSizer->SetConstraints(c);

 /*
  * Add a row-col sizer and some buttons
  */

  // Default is layout by rows, 20 columns per row, shrink to fit.
  wxRowColSizer *rowCol = new wxRowColSizer(topSizer);
  rowCol->SetName("rowCol");
  
  wxButton *button = new wxButton(panel, (wxFunction)NULL, "Button 1");
  rowCol->AddSizerChild(button);

  button = new wxButton(panel, (wxFunction)NULL, "Button 2");
  rowCol->AddSizerChild(button);

  button = new wxButton(panel, (wxFunction)NULL, "Button 3");
  rowCol->AddSizerChild(button);

  // Centre the rowcol in the middle of the upper sizer
  c = new wxLayoutConstraints;
  c->centreX.SameAs    (topSizer, wxCentreX);
  c->centreY.SameAs    (topSizer, wxCentreY);
  c->width.AsIs();
  c->height.AsIs();
  rowCol->SetConstraints(c);

  /////// BOTTOM OF PANEL
  ///////
  wxSizer *bottomSizer = new wxSizer(expandSizer);

  // Specify constraints for the bottom sizer
  c = new wxLayoutConstraints;
  c->left.SameAs       (expandSizer, wxLeft);
  c->top.PercentOf     (expandSizer, wxHeight, 50);
  c->right.SameAs      (expandSizer, wxRight);
  c->height.PercentOf  (expandSizer, wxHeight, 50);

  bottomSizer->SetConstraints(c);

  wxButton *button2 = new wxButton(panel, (wxFunction)NULL, "Test button");

  // The button should be a child of the bottom sizer
  bottomSizer->AddSizerChild(button2);

  // Centre the button on the sizer
  c = new wxLayoutConstraints;
  c->centreX.SameAs    (bottomSizer, wxCentreX);
  c->centreY.SameAs    (bottomSizer, wxCentreY);
  c->width.PercentOf   (bottomSizer, wxWidth, 20);
  c->height.PercentOf  (bottomSizer, wxHeight, 20);
  button2->SetConstraints(c);
}


// Size the subwindows when the frame is resized
void SizerFrame::OnSize(int w, int h)
{
  wxFrame::OnSize(w, h);
  panel->Layout();
}

Bool SizerFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

void TestSizer(void)
{
  SizerFrame *newFrame = new SizerFrame(NULL, "Sizer Test Frame", 50, 50, 500, 500);
  newFrame->Show(TRUE);
}
