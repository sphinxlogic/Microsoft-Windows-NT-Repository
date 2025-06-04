/*
 * File:	test.cc
 * Purpose:	Demo for buttonbar class library
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
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

#include "wx_bbar.h"
#include "test.h"

// Declare two frames
MyFrame   *frame = NULL;
wxMenuBar *menu_bar = NULL;
wxIcon    *test_icon = NULL;

PaletteFrame *toolBarFrame = NULL;
TestToolBar *toolBar = NULL;
TestRibbon *toolRibbon = NULL;
wxBitmap *toolBarBitmaps[25];

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

#ifdef wx_x
#include "bitmaps/dirplain.xbm"
#include "bitmaps/draft.xbm"
#include "bitmaps/drawing.xbm"
#include "bitmaps/flowchar.xbm"
#include "bitmaps/write.xbm"
#endif

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Hello wxButtonBar", 100, 100, 450, 300);

  // Give it a status line
  frame->CreateStatusLine();

  // Give it an icon
#ifdef wx_msw
  test_icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  test_icon = new wxIcon("aiai.xbm");
#endif
  frame->SetIcon(test_icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_CREATE_PALETTE, "Create &palette");
  file_menu->Append(TEST_QUIT, "E&xit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, "&About");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->textWindow = new wxTextWindow(frame, 0, 0, -1, -1, wxNATIVE_IMPL);

  // Set up toolbar
#ifdef wx_msw
  toolBarBitmaps[0] = new wxBitmap("icon1");
  toolBarBitmaps[1] = new wxBitmap("icon2");
  toolBarBitmaps[2] = new wxBitmap("icon3");
  toolBarBitmaps[3] = new wxBitmap("icon4");
  toolBarBitmaps[4] = new wxBitmap("icon5");
  toolBarBitmaps[5] = new wxBitmap("icon6");
  toolBarBitmaps[6] = new wxBitmap("icon7");
  toolBarBitmaps[7] = new wxBitmap("icon8");
  toolBarBitmaps[8] = NULL;
  toolBarBitmaps[9] = NULL;

  toolBarBitmaps[10] = new wxBitmap("tool1");
  toolBarBitmaps[11] = new wxBitmap("tool2");
  toolBarBitmaps[12] = new wxBitmap("tool3");
  toolBarBitmaps[13] = new wxBitmap("tool4");
  toolBarBitmaps[14] = new wxBitmap("tool5");
  toolBarBitmaps[15] = new wxBitmap("tool6");
  toolBarBitmaps[16] = new wxBitmap("tool7");
  toolBarBitmaps[17] = new wxBitmap("tool8");
  toolBarBitmaps[18] = new wxBitmap("tool9");
  toolBarBitmaps[19] = new wxBitmap("tool10");
  toolBarBitmaps[20] = new wxBitmap("tool11");
  toolBarBitmaps[21] = new wxBitmap("tool12");
  toolBarBitmaps[22] = new wxBitmap("tool13");
  toolBarBitmaps[23] = new wxBitmap("tool14");
#endif
#ifdef wx_x
  toolBarBitmaps[0] = new wxBitmap(dirplain_bits, dirplain_width, dirplain_height);
  toolBarBitmaps[1] = new wxBitmap(draft_bits, draft_width, draft_height);
  toolBarBitmaps[2] = new wxBitmap(drawing_bits, drawing_width, drawing_height);
  toolBarBitmaps[3] = new wxBitmap(flowchart_bits, flowchart_width, flowchart_height);
  toolBarBitmaps[4] = new wxBitmap(write_bits, write_width, write_height);
  toolBarBitmaps[5] = new wxBitmap(write_bits, write_width, write_height);
  toolBarBitmaps[6] = new wxBitmap(write_bits, write_width, write_height);
  toolBarBitmaps[7] = new wxBitmap(write_bits, write_width, write_height);
  toolBarBitmaps[8] = NULL;
  toolBarBitmaps[9] = NULL;

  for (int j = 10; j < 24; j++)
//    if (toolBarBitmaps[j])
      toolBarBitmaps[j] = new wxBitmap(write_bits, write_width, write_height);
#endif
  toolBarBitmaps[24] = new wxBitmap(32, 32);

  // Draw into the last bitmap.
  // This memory DC must be created dynamically, because only by deleting the DC
  // can we deselect the bitmap from the DC. Windows doesn't allow more than
  // one DC having the same bitmap selected.
  wxMemoryDC *memDC = new wxMemoryDC;
  memDC->SetUserScale(0.15, 0.15);
  memDC->SelectObject(toolBarBitmaps[24]);
  memDC->SetBackground(wxWHITE_BRUSH);

  memDC->Clear();
  memDC->SetPen(wxRED_PEN);
  memDC->SetBrush(wxBLUE_BRUSH);
  memDC->DrawLine(0, 0, 200, 200);
  memDC->DrawLine(200, 0, 0, 200);
  memDC->DrawEllipse(250, 250, 100, 50);

  delete memDC;

  toolBarFrame = NULL;

  toolBarFrame = new PaletteFrame(frame, "Tools", 0, 0, 300, 200,
    wxSDI | wxSYSTEM_MENU | wxCAPTION | wxTHICK_FRAME);

  toolRibbon = new TestRibbon(frame, 0, 0, 100, 30, 0, wxVERTICAL, 1);
  toolRibbon->SetMargins(5, 5);
#if !WIN95
  toolRibbon->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
#endif
  frame->SetToolBar(toolRibbon);

#ifdef wx_msw
  int width = 24;
#else
  int width = 16;
#endif
  int offX = 5;
  int currentX = 5;

  toolRibbon->AddTool(0, toolBarBitmaps[0], NULL, FALSE, (float)currentX, -1, NULL, "New file");
  currentX += width + 5;
  toolRibbon->AddTool(1, toolBarBitmaps[1], NULL, FALSE, (float)currentX, -1, NULL, "Open file");
  currentX += width + 5;
  toolRibbon->AddTool(2, toolBarBitmaps[2], NULL, FALSE, (float)currentX, -1, NULL, "Save file");
  currentX += width + 5;
  toolRibbon->AddSeparator();
  toolRibbon->AddTool(3, toolBarBitmaps[3], NULL, FALSE, (float)currentX, -1, NULL, "Copy");
  currentX += width + 5;
  toolRibbon->AddTool(4, toolBarBitmaps[4], NULL, FALSE, (float)currentX, -1, NULL, "Cut");
  currentX += width + 5;
  toolRibbon->AddTool(5, toolBarBitmaps[5], NULL, FALSE, (float)currentX, -1, NULL, "Paste");
  currentX += width + 5;
  toolRibbon->AddSeparator();
  toolRibbon->AddTool(6, toolBarBitmaps[6], NULL, FALSE, (float)currentX, -1, NULL, "Print");
  currentX += width + 5;
  toolRibbon->AddSeparator();
  toolRibbon->AddTool(7, toolBarBitmaps[7], NULL, TRUE, currentX, -1, NULL, "Help");

  toolRibbon->CreateTools();

  frame->OnSize(-1, -1);
  frame->Show(TRUE);

  frame->SetStatusText("Hello, wxWindows");
  
  // Return the main frame window
  return frame;
}

// Toolbar-handling frame constructor
wxFrameWithToolBar::wxFrameWithToolBar(wxFrame *frame, char *title, int x, int y, int w, int h,
  long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  frameToolBar = NULL;
}

// Reposition the toolbar and child subwindow
void wxFrameWithToolBar::OnSize(int w, int h)
{
  float maxToolBarWidth = 0.0, maxToolBarHeight = 0.0;
  if (frameToolBar)
  {
    frameToolBar->GetMaxSize(&maxToolBarWidth, &maxToolBarHeight);
//    maxToolBarHeight = 30;
  }
  else
  {
    wxFrame::OnSize(w, h);
    return;
  }

  // Find the frame's child (assuming there is only one)
  wxWindow *child = NULL;
  wxNode *node = GetChildren()->First();
  while (node && !child)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if ((win != frameToolBar) &&
        (win->IsKindOf(CLASSINFO(wxPanel)) ||
         win->IsKindOf(CLASSINFO(wxTextWindow)) ||
         win->IsKindOf(CLASSINFO(wxCanvas))))
      child = win;

    node = node->Next();
  }
  int frameWidth, frameHeight;
  GetClientSize(&frameWidth, &frameHeight);

  frameToolBar->SetSize(0, 0, (int)frameWidth, (int)maxToolBarHeight);
  if (child)
    child->SetSize(0, (int)maxToolBarHeight, (int)frameWidth, (int)(frameHeight - maxToolBarHeight));
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrameWithToolBar(frame, title, x, y, w, h, wxSDI|wxDEFAULT_FRAME)
{
  textWindow = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case TEST_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    case TEST_CREATE_PALETTE:
    {
      if (!toolBarFrame)
      {
        toolBarFrame = new PaletteFrame(frame, "Tools", 0, 0, 300, 200,
          wxSDI | wxSYSTEM_MENU | wxCAPTION | wxTHICK_FRAME);
      }
      break;
    }
    case TEST_ABOUT:
    {
      (void)wxMessageBox("wxWindows buttonbar demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1996", "About Buttonbar test");
      break;
    }
  }
}

// Intercept menu item selection - only has an effect in Windows
void MyFrame::OnMenuSelect(int id)
{
  char *msg = NULL;
  switch (id)
  {
    case TEST_QUIT:
      msg = "Quit program";
      break;
    case -1:
      msg = "";
      break;
  }
  if (msg)
    frame->SetStatusText(msg);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  if (toolBarFrame)
    delete toolBarFrame;
  return TRUE;
}

TestToolBar::TestToolBar(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  wxButtonBar(frame, x, y, w, h, style, direction, RowsOrColumns)
{
}

Bool TestToolBar::OnLeftClick(int toolIndex, Bool toggled)
{
  char buf[200];
  sprintf(buf, "Clicked on tool %d", toolIndex);
  frame->SetStatusText(buf);
  return TRUE;
}

void TestToolBar::OnMouseEnter(int toolIndex)
{
  char buf[200];
  if (toolIndex > -1)
  {
    sprintf(buf, "This is tool number %d", toolIndex);
    frame->SetStatusText(buf);
  }
  else frame->SetStatusText("");
}

TestRibbon::TestRibbon(wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  wxButtonBar(frame, x, y, w, h, style, direction, RowsOrColumns)
{
}

Bool TestRibbon::OnLeftClick(int toolIndex, Bool toggled)
{
  char buf[200];
  sprintf(buf, "Clicked on tool %d", toolIndex);
  frame->SetStatusText(buf);
  return TRUE;
}

void TestRibbon::OnMouseEnter(int toolIndex)
{
  char buf[200];
  if (toolIndex > -1)
  {
    sprintf(buf, "This is tool number %d", toolIndex);
    frame->SetStatusText(buf);
  }
  else frame->SetStatusText("");
}

void TestRibbon::OnPaint(void)
{
  wxButtonBar::OnPaint();

  BeginDrawing();
  
  int w, h;
  GetSize(&w, &h);
  wxDC *dc = GetDC();
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxTRANSPARENT_BRUSH);
  DrawLine(0, h-1, w, h-1);

  EndDrawing();
}

// Define my frame constructor
PaletteFrame::PaletteFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  toolBar = NULL;

  toolBar = new TestToolBar(this, 10, 10, -1, -1, 0, wxVERTICAL, 5);
  toolBar->SetMargins(4, 4);
  toolBar->SetDefaultSize(32,32);
#if !WIN95
  toolBar->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
#endif
  int i;

  for (i = 10; i < 25; i++)
    toolBar->AddTool(i, toolBarBitmaps[i], NULL, TRUE, -1, -1, NULL);

  toolBar->CreateTools();

  toolBar->Layout();
  float maxWidth, maxHeight;
  toolBar->GetMaxSize(&maxWidth, &maxHeight);
  SetClientSize((int)maxWidth, (int)maxHeight);

  Show(TRUE);
}

PaletteFrame::~PaletteFrame(void)
{
  toolBarFrame = NULL;
}

