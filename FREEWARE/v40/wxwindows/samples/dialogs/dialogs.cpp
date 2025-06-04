/*
 * File:	dialogs.cc
 * Purpose:	Dialogs example
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_cmdlg.h"

wxFont *canvasFont = NULL;
wxColour *canvasTextColour = NULL;

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
    Bool OnClose(void) { return TRUE; }
};

class MyCanvas: public wxCanvas
{
 public:
  MyCanvas(wxWindow *parent):
      wxCanvas(parent)
    {
    }
  void OnPaint(void)
  {
    GetDC()->SetFont(canvasFont);
    GetDC()->SetTextForeground(canvasTextColour);
    GetDC()->SetBackgroundMode(wxTRANSPARENT);
    GetDC()->DrawText("wxWindows common dialogs test application", 10, 10);
  }
};

// Menu IDs
#define DIALOGS_CHOOSE_COLOUR               1
#define DIALOGS_CHOOSE_COLOUR_GENERIC       2
#define DIALOGS_CHOOSE_FONT                 3
#define DIALOGS_CHOOSE_FONT_GENERIC         4

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

MyCanvas *myCanvas = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "wxWindows dialogs example", 50, 50, 400, 300);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("aiai_icn"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif
  canvasTextColour = new wxColour("BLACK");
  canvasFont = wxNORMAL_FONT;

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(DIALOGS_CHOOSE_COLOUR, "&Choose colour");
  file_menu->Append(DIALOGS_CHOOSE_COLOUR_GENERIC, "Choose colour (&generic)");
  file_menu->AppendSeparator();
  file_menu->Append(DIALOGS_CHOOSE_FONT, "Choose &font");
  file_menu->Append(DIALOGS_CHOOSE_FONT_GENERIC, "Choose f&ont (generic)");
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  myCanvas = new MyCanvas(frame);
  myCanvas->SetBackground(wxWHITE_BRUSH);

  frame->Centre(wxBOTH);
  
  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case wxID_EXIT:
//      delete this;
      Close(TRUE);
      break;
    case DIALOGS_CHOOSE_COLOUR:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      for (int i = 0; i < 16; i++)
      {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
      }
      
      wxColourDialog *dialog = new wxColourDialog(this, &data);
      if (dialog->Show(TRUE))
      {
        wxColourData retData = dialog->GetColourData();
        wxColour col = retData.GetColour();
        wxBrush *brush = wxTheBrushList->FindOrCreateBrush(&col, wxSOLID);
        myCanvas->SetBackground(brush);
        myCanvas->Clear();
        myCanvas->Refresh();
      }
      dialog->Close();
      break;
    }
    case DIALOGS_CHOOSE_COLOUR_GENERIC:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      for (int i = 0; i < 16; i++)
      {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
      }
      
      wxGenericColourDialog *dialog = new wxGenericColourDialog(this, &data);
      if (dialog->Show(TRUE))
      {
        wxColourData retData = dialog->GetColourData();
        wxColour col = retData.GetColour();
        wxBrush *brush = wxTheBrushList->FindOrCreateBrush(&col, wxSOLID);
        myCanvas->SetBackground(brush);
        myCanvas->Clear();
        myCanvas->Refresh();
      }
      dialog->Close();
      break;
    }
    case DIALOGS_CHOOSE_FONT:
    {
      wxFontData data;
      data.SetInitialFont(canvasFont);
      data.SetColour(*canvasTextColour);
      
      wxFontDialog *dialog = new wxFontDialog(this, &data);
      if (dialog->Show(TRUE))
      {
        wxFontData retData = dialog->GetFontData();
        canvasFont = retData.GetChosenFont();
        (*canvasTextColour) = retData.GetColour();
        myCanvas->Refresh();
      }
      dialog->Close();
      break;
    }
    case DIALOGS_CHOOSE_FONT_GENERIC:
    {
      wxFontData data;
      data.SetInitialFont(canvasFont);
      data.SetColour(*canvasTextColour);
      
      wxGenericFontDialog *dialog = new wxGenericFontDialog(this, &data);
      if (dialog->Show(TRUE))
      {
        wxFontData retData = dialog->GetFontData();
        canvasFont = retData.GetChosenFont();
        (*canvasTextColour) = retData.GetColour();
        myCanvas->Refresh();
      }
      dialog->Close();
      break;
    }
  }
}

