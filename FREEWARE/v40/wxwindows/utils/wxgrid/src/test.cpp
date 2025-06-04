/*
 * File:	test.cc
 * Purpose:	wxGrid test
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

#include "wxgrid.h"

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    wxGrid *grid;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
    void OnActivate(Bool active);
    Bool OnClose(void) { return TRUE; }
};

wxBitmap *cellBitmap1 = NULL;
wxBitmap *cellBitmap2 = NULL;

// ID for the menu quit command
#define GRID_QUIT 1
#define GRID_TOGGLE_EDITABLE 2
#define GRID_LEFT_CELL       3
#define GRID_CENTRE_CELL     4
#define GRID_RIGHT_CELL      5
#define GRID_TOGGLE_ROW_LABEL 6
#define GRID_TOGGLE_COL_LABEL 7
#define GRID_COLOUR_LABEL_BACKGROUND 8
#define GRID_COLOUR_LABEL_TEXT       9
#define GRID_NORMAL_LABEL_COLOURING  10
#define GRID_COLOUR_CELL_BACKGROUND 11
#define GRID_COLOUR_CELL_TEXT       12
#define GRID_NORMAL_CELL_COLOURING  13
#define GRID_TOGGLE_DIVIDERS        14

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
#ifdef wx_msw
  cellBitmap1 = new wxBitmap("bitmap1");
  cellBitmap2 = new wxBitmap("bitmap2");
#endif

  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "wxGrid Sample", 50, 50, 450, 300);
  
  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("mondrian"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(GRID_QUIT, "E&xit");

  wxMenu *settings_menu = new wxMenu;
  settings_menu->Append(GRID_TOGGLE_EDITABLE, "&Toggle editable");
  settings_menu->Append(GRID_TOGGLE_ROW_LABEL, "Toggle ro&w label");
  settings_menu->Append(GRID_TOGGLE_COL_LABEL, "Toggle co&l label");
  settings_menu->Append(GRID_TOGGLE_DIVIDERS, "Toggle &dividers");
  settings_menu->AppendSeparator();
  settings_menu->Append(GRID_LEFT_CELL, "&Left cell alignment ");
  settings_menu->Append(GRID_CENTRE_CELL, "&Centre cell alignment ");
  settings_menu->Append(GRID_RIGHT_CELL, "&Right cell alignment ");
  settings_menu->AppendSeparator();
  settings_menu->Append(GRID_COLOUR_LABEL_BACKGROUND, "Choose a label &background colour");
  settings_menu->Append(GRID_COLOUR_LABEL_TEXT, "Choose a label fore&ground colour");
  settings_menu->Append(GRID_NORMAL_LABEL_COLOURING, "&Normal label colouring");
  settings_menu->AppendSeparator();
  settings_menu->Append(GRID_COLOUR_CELL_BACKGROUND, "Choo&se a cell &background colour");
  settings_menu->Append(GRID_COLOUR_CELL_TEXT, "Choose &a cell foreground colour");
  settings_menu->Append(GRID_NORMAL_CELL_COLOURING, "N&ormal cell colouring");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(settings_menu, "&Settings");
  frame->SetMenuBar(menu_bar);

  // Make a grid
  frame->grid = new wxGrid(frame, 0, 0, 400, 400);

  frame->grid->CreateGrid(10, 8);
  frame->grid->SetColumnWidth(3, 200);
  frame->grid->SetRowHeight(4, 45);
  frame->grid->SetCellValue("First cell", 0, 0);
  frame->grid->SetCellValue("Another cell", 1, 1);
  frame->grid->SetCellValue("Yet another cell", 2, 2);
  frame->grid->SetCellTextFont(wxTheFontList->FindOrCreateFont(12, wxROMAN, wxITALIC, wxNORMAL), 0, 0);
  frame->grid->SetCellTextColour(*wxRED, 1, 1);
  frame->grid->SetCellBackgroundColour(*wxCYAN, 2, 2);
  if (cellBitmap1 && cellBitmap2)
  {
    frame->grid->SetCellAlignment(wxCENTRE, 5, 0);
    frame->grid->SetCellAlignment(wxCENTRE, 6, 0);
    frame->grid->SetCellBitmap(cellBitmap1, 5, 0);
    frame->grid->SetCellBitmap(cellBitmap2, 6, 0);
  }
  
  frame->grid->UpdateDimensions();
  
  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  grid = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id) {
    case GRID_TOGGLE_EDITABLE:
    {
      grid->SetEditable(!grid->GetEditable());
      grid->Refresh();
      break;
    }
    case GRID_TOGGLE_ROW_LABEL:
    {
      if (grid->GetLabelSize(wxVERTICAL) > 0)
        grid->SetLabelSize(wxVERTICAL, 0);
      else
        grid->SetLabelSize(wxVERTICAL, 40);
      grid->Refresh();
      break;
    }
    case GRID_TOGGLE_COL_LABEL:
    {
      if (grid->GetLabelSize(wxHORIZONTAL) > 0)
        grid->SetLabelSize(wxHORIZONTAL, 0);
      else
        grid->SetLabelSize(wxHORIZONTAL, 20);
      grid->Refresh();
      break;
    }
    case GRID_TOGGLE_DIVIDERS:
    {
      if (!grid->GetDividerPen())
        grid->SetDividerPen(wxThePenList->FindOrCreatePen("LIGHT GREY", 1, wxSOLID));
      else
        grid->SetDividerPen(NULL);
      grid->Refresh();
      break;
    }
    case GRID_LEFT_CELL:
    {
      grid->SetCellAlignment(wxLEFT);
      grid->Refresh();
      break;
    }
    case GRID_CENTRE_CELL:
    {
      grid->SetCellAlignment(wxCENTRE);
      grid->Refresh();
      break;
    }
    case GRID_RIGHT_CELL:
    {
      grid->SetCellAlignment(wxRIGHT);
      grid->Refresh();
      break;
    }
    case GRID_COLOUR_LABEL_BACKGROUND:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      wxColourDialog dialog(this, &data);
      if (dialog.Show(TRUE))
      {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetLabelBackgroundColour(col);
        grid->Refresh();
      }
      
      break;
    }
    case GRID_COLOUR_LABEL_TEXT:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      wxColourDialog dialog(this, &data);
      if (dialog.Show(TRUE))
      {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetLabelTextColour(col);
        grid->Refresh();
      }
      
      break;
    }
    case GRID_NORMAL_LABEL_COLOURING:
    {
      grid->SetLabelBackgroundColour(*wxLIGHT_GREY);
      grid->SetLabelTextColour(*wxBLACK);
      grid->Refresh();
      break;
    }
    case GRID_COLOUR_CELL_BACKGROUND:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      wxColourDialog dialog(this, &data);
      if (dialog.Show(TRUE))
      {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetCellBackgroundColour(col);
        grid->Refresh();
      }
      
      break;
    }
    case GRID_COLOUR_CELL_TEXT:
    {
      wxColourData data;
      data.SetChooseFull(TRUE);
      wxColourDialog dialog(this, &data);
      if (dialog.Show(TRUE))
      {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        grid->SetCellTextColour(col);
        grid->Refresh();
      }
      
      break;
    }
    case GRID_NORMAL_CELL_COLOURING:
    {
      grid->SetCellBackgroundColour(*wxWHITE);
      grid->SetCellTextColour(*wxBLACK);
      grid->Refresh();
      break;
    }
    case GRID_QUIT:
    {
      delete this;
      break;
    }
  }
}

// Ensure that the grid's edit control always has the focus.
void MyFrame::OnActivate(Bool active)
{
  if (grid) grid->OnActivate(active);
}

