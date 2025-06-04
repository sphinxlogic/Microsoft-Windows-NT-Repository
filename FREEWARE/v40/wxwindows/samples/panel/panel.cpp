/*
 * File:	panel.cc
 * Purpose:	Shows how to paint a background for a panel.
 8
 *              Hint: if you wish to draw items with labels
 *              on a background, use the wxUSER_COLOURS panel style
 *              and set the background brush to be transparent,
 *              so that the labels will be drawn on a transparent
 *              background.
 *
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "panel.h"

MyFrame   *frame = NULL;
wxMenuBar *menu_bar = NULL;
wxItem *item1 = NULL;
wxItem *item2 = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

#if USE_GAUGE
int gaugePos = 0;
wxGauge *gauge1 = NULL;
#endif

wxButton *bitmapButton = NULL;
wxMessage *bitmapMessage = NULL;
wxBitmap *currentBitmap = NULL;
wxBitmap *backgroundBitmap = NULL;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  backgroundBitmap = new wxBitmap("marble.bmp", wxBITMAP_TYPE_BMP);
  
  // Create the main frame window
  frame = new MyFrame(NULL, "Panel test", 0, 0, 430, 340, wxSDI|wxDEFAULT_FRAME);

  // Give it an icon
#ifdef wx_msw
  wxIcon *test_icon = new wxIcon("aiai_icn");
  frame->SetIcon(test_icon);
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(PANELS_LOAD_BITMAP, "&Load background bitmap (.BMP)");
  file_menu->Append(PANELS_QUIT, "&Quit");


  wxMenu *help_menu = new wxMenu;
  help_menu->Append(PANELS_ABOUT, "&About");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new MyPanel(frame, 0, 0, 400, 400, wxUSER_COLOURS);

  frame->panel->SetLabelPosition(wxVERTICAL);
  frame->panel->SetBackground(wxWHITE_BRUSH);

  wxColour col1("RED");  
  wxColour col2("CYAN");
#if USE_GAUGE
  frame->panel->NewLine();
  frame->panel->SetLabelPosition(wxVERTICAL);
  // Range is 100
  gauge1 = new wxGauge(frame->panel, NULL, 100, 80, 120, 250, 40, wxHORIZONTAL);
  gauge1->SetShadowWidth(3);
  gauge1->SetButtonColour(col1);
  gauge1->SetBackgroundColour(col2);
  gauge1->SetValue(0);
  gaugePos = 0;
  frame->panel->NewLine();
#endif
  wxButton *but1 = new wxButton(frame->panel, (wxFunction)&button_proc, "Continue", 50, 205, 100, 50);
  wxButton *but2 = new wxButton(frame->panel, (wxFunction)&quit_proc, "Exit", 250, 205, 100, 50);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  panel = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case PANELS_LOAD_BITMAP:
    {
      char *s = wxFileSelector("Load bitmap", NULL, NULL, NULL, "*.*");
      if (s)
      {
        char *f = copystring(s);
        wxBitmap *bitmap = new wxBitmap(f, wxBITMAP_TYPE_BMP);
        delete[] f;
        if (bitmap)
	{
          if (backgroundBitmap)
            delete backgroundBitmap;
          backgroundBitmap = bitmap;
          panel->Refresh();
        }
      }
     
      break;
    }
    case PANELS_QUIT:
    {
      OnClose();
      delete this;
      break;
    }
    case PANELS_ABOUT:
    {
      (void)wxMessageBox("Panel Test\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1995", "About Panel Test");
      break;
    }
  }
}
// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}

void MyFrame::OnSize(int w, int h)
{
  wxFrame::OnSize(w, h);
}

void quit_proc(wxButton& but, wxEvent& event)
{
  delete frame;
}

// Gets some user input, and sets the status line
void button_proc(wxButton& but, wxEvent& event)
{
#if USE_GAUGE
  gaugePos += 5;
  if (gaugePos > 100)
    gaugePos = 0;
  gauge1->SetValue(gaugePos);
#endif
}

MyPanel::MyPanel(wxFrame *frame, int x, int y, int w, int h, long style):
  wxPanel(frame, x, y, w, h, style)
{
}

void MyPanel::OnPaint(void)
{
  wxDC *dc = GetDC();
  if (!backgroundBitmap)
    return;
    
  dc->BeginDrawing();

  wxMemoryDC memDC;

  memDC.SelectObject(backgroundBitmap);
  if (backgroundBitmap->GetColourMap())
  {
    memDC.SetColourMap(backgroundBitmap->GetColourMap());
    dc->SetColourMap(backgroundBitmap->GetColourMap());
  }
  
  int w, h;
  GetClientSize(&w, &h);

  int i, j;
  for (i = 0; i <= w; i += backgroundBitmap->GetWidth())
    for (j = 0; j <= h; j += backgroundBitmap->GetHeight())
    {
      dc->Blit(i, j, backgroundBitmap->GetWidth(), backgroundBitmap->GetHeight(), &memDC, 0, 0);
    }

  memDC.SelectObject(NULL);

  // Paint any static items
  wxPanel::OnPaint();

  dc->EndDrawing();
}
