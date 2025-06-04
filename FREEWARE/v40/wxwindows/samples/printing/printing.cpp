/*
 * File:	printing.cc
 * Purpose:	Printing demo for wxWindows class library
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

#if !USE_PRINTING_ARCHITECTURE
#error You must set USE_PRINTING_ARCHITECTURE to 1 in wx_setup.h to compile this demo.
#endif

#include <ctype.h>
#include "wx_mf.h"
#include "wx_print.h"

#include "printing.h"

// Declare a frame
MyFrame   *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

// Must initialise these in OnInit, not statically
wxPen     *red_pen;
wxFont    *labelFont;
wxFont    *itemFont;

float     zoom_factor = 1.0;

#ifdef wx_x
#include "aiai.xbm"
#endif

// Writes a header on a page. Margin units are in millimetres.
Bool WritePageHeader(wxPrintout *printout, wxDC *dc, char *text, float mmToLogical);

MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a small font
  itemFont = new wxFont(11, wxROMAN, wxNORMAL, wxNORMAL);
  labelFont = new wxFont(12, wxROMAN, wxITALIC, wxBOLD);

  // Create the main frame window
  frame = new MyFrame(NULL, "wxWindows Printing Demo", 0, 0, 400, 400);

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

  file_menu->Append(WXPRINT_PRINT, "&Print...",              "Print");
  file_menu->Append(WXPRINT_PRINT_SETUP, "Print &Setup...",              "Setup printer properties");
  file_menu->Append(WXPRINT_PREVIEW, "Print Pre&view",              "Preview");

#ifdef wx_msw
  file_menu->AppendSeparator();
  file_menu->Append(WXPRINT_PRINT_PS, "Print PostScript...",              "Print (PostScript)");
  file_menu->Append(WXPRINT_PRINT_SETUP_PS, "Print Setup PostScript...",              "Setup printer properties (PostScript)");
  file_menu->Append(WXPRINT_PREVIEW_PS, "Print Preview PostScript",              "Preview (PostScript)");
#endif
  file_menu->AppendSeparator();
  file_menu->Append(WXPRINT_QUIT, "E&xit",                "Exit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(WXPRINT_ABOUT, "&About",              "About this demo");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  MyCanvas *canvas = new MyCanvas(frame, 0, 0, 100, 100, wxRETAINED);

  canvas->SetBackground(wxWHITE_BRUSH);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
  canvas->SetPen(red_pen);

  // This ensures that the fonts get created as _screen_
  // fonts, not printer fonts.
  canvas->SetFont(itemFont);
  canvas->SetFont(labelFont);

  frame->canvas = canvas;

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  frame->SetStatusText("Printing demo");

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  canvas = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  wxDC *dc = canvas->GetDC();
  switch (id)
  {
    case WXPRINT_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    case WXPRINT_PRINT:
    {
#ifdef wx_msw
      myApp.SetPrintMode(wxPRINT_WINDOWS);
#else
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
      wxPrinter printer;
      MyPrintout printout("My printout");
      if (!printer.Print(this, &printout, TRUE))
        wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
      break;
    }
    case WXPRINT_PREVIEW:
    {
/*
#if DEBUG
      wxDebugContext::SetFile("debug.log");
      wxDebugContext::Dump();
      wxDebugContext::PrintStatistics();
#endif
*/
#ifdef wx_msw
      myApp.SetPrintMode(wxPRINT_WINDOWS);
#else
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
      // Pass two printout objects: for preview, and possible printing.
      wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout);
      if (!preview->Ok())
      {
        delete preview;
        wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
        return;
      }
      
      wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", 100, 100, 600, 650);
      frame->Centre(wxBOTH);
      frame->Initialize();
      frame->Show(TRUE);
      break;
    }
    case WXPRINT_PRINT_SETUP:
    {
#ifdef wx_msw
      myApp.SetPrintMode(wxPRINT_WINDOWS);
#else
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);
#endif
      wxPrintDialog printerDialog(this);
      printerDialog.GetPrintData().SetSetupDialog(TRUE);
      printerDialog.Show(TRUE);
      break;
    }
#ifdef wx_msw
    case WXPRINT_PRINT_PS:
    {
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);

      wxPrinter printer;
      MyPrintout printout("My printout");
      printer.Print(this, &printout, TRUE);
      break;
    }
    case WXPRINT_PREVIEW_PS:
    {
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);

      // Pass two printout objects: for preview, and possible printing.
      wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout);
      wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", 100, 100, 600, 650);
      frame->Centre(wxBOTH);
      frame->Initialize();
      frame->Show(TRUE);
      break;
    }
    case WXPRINT_PRINT_SETUP_PS:
    {
      myApp.SetPrintMode(wxPRINT_POSTSCRIPT);

      wxPrintDialog printerDialog(this);
      printerDialog.GetPrintData().SetSetupDialog(TRUE);
      printerDialog.Show(TRUE);
      break;
    }
#endif
    case WXPRINT_ABOUT:
    {
      (void)wxMessageBox("wxWindows printing demo\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1995",
            "About wxWindows printing demo", wxOK|wxCENTRE);
      break;
    }
  }
}

void MyFrame::Draw(wxDC& dc)
{
  dc.SetFont(itemFont);
  dc.SetBackgroundMode(wxTRANSPARENT);

  dc.SetBrush(wxCYAN_BRUSH);
  dc.SetPen(wxRED_PEN);

  dc.DrawRectangle(0.0, 30.0, 200.0, 100.0);
  dc.DrawText("Rectangle 200 by 100", 40.0, 40.0);

  dc.DrawEllipse(50.0, 140.0, 100.0, 50.0);

  dc.DrawText("Test message: this is in 11 point text", 10.0, 180.0);

  dc.SetPen(wxBLACK_PEN);
  dc.DrawLine(0.0, 0.0, 200.0, 200.0);
  dc.DrawLine(200.0, 0.0, 0.0, 200.0);
}

void MyFrame::OnSize(int w, int h)
{
  if (canvas)
  {
    int ww, hh;
    GetClientSize(&ww, &hh);
    canvas->SetSize(0, 0, ww, hh);
  }
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
  wxDC *dc = GetDC();
  
  // need the last param for cl386.
  frame->Draw(*dc);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button. Right button activates a menu.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
}

void MyCanvas::OnScroll(wxCommandEvent& event)
{
  wxCanvas::OnScroll(event);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  Show(FALSE);

  return TRUE;
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}

Bool MyPrintout::OnPrintPage(int page)
{
  wxDC *dc = GetDC();
  if (dc)
  {
    if (page == 1)
      DrawPageOne(dc);
    else if (page == 2)
      DrawPageTwo(dc);

    dc->SetDeviceOrigin(0, 0);

    char buf[200];
    sprintf(buf, "PAGE %d", page);
    dc->DrawText(buf, 10.0, 10.0);
    
    return TRUE;
  }
  else
    return FALSE;
}

Bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
  if (!wxPrintout::OnBeginDocument(startPage, endPage))
    return FALSE;

  return TRUE;
}

void MyPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = 2;
  *selPageFrom = 1;
  *selPageTo = 2;
}

Bool MyPrintout::HasPage(int pageNum)
{
  return (pageNum == 1 || pageNum == 2);
}

void MyPrintout::DrawPageOne(wxDC *dc)
{
/* You might use THIS code if you were scaling
 * graphics of known size to fit on the page.
 */
  float w, h;

  // We know the graphic is 200x200. If we didn't know this,
  // we'd need to calculate it.
  float maxX = 200;
  float maxY = 200;
  
  // Let's have at least 50 device units margin
  float marginX = 50;
  float marginY = 50;

  // Add the margin to the graphic size
  maxX += (2*marginX);
  maxY += (2*marginY);
  
  // Get the size of the DC in pixels
  dc->GetSize(&w, &h);

  // Calculate a suitable scaling factor
  float scaleX=(float)(w/maxX);
  float scaleY=(float)(h/maxY);

  // Use x or y scaling factor, whichever fits on the DC
  float actualScale = wxMin(scaleX,scaleY);

  // Calculate the position on the DC for centring the graphic
  float posX = (float)((w - (200*actualScale))/2.0);
  float posY = (float)((h - (200*actualScale))/2.0);

  // Set the scale and origin
  dc->SetUserScale(actualScale, actualScale);
  dc->SetDeviceOrigin(posX, posY);

  frame->Draw(*dc);
}

void MyPrintout::DrawPageTwo(wxDC *dc)
{
/* You might use THIS code to set the printer DC to ROUGHLY reflect
 * the screen text size. This page also draws lines of actual length 5cm
 * on the page.
 */
  // Get the logical pixels per inch of screen and printer
  int ppiScreenX, ppiScreenY;
  GetPPIScreen(&ppiScreenX, &ppiScreenY);
  int ppiPrinterX, ppiPrinterY;
  GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

  // This scales the DC so that the printout roughly represents the
  // the screen scaling. The text point size _should_ be the right size
  // but in fact is too small for some reason. This is a detail that will
  // need to be addressed at some point but can be fudged for the
  // moment.
  float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

  // Now we have to check in case our real page size is reduced
  // (e.g. because we're drawing to a print preview memory DC)
  int pageWidth, pageHeight;
  float w, h;
  dc->GetSize(&w, &h);
  GetPageSizePixels(&pageWidth, &pageHeight);

  // If printer pageWidth == current DC width, then this doesn't
  // change. But w might be the preview bitmap width, so scale down.
  float overallScale = scale * (float)(w/(float)pageWidth);
  dc->SetUserScale(overallScale, overallScale);
  
  // Calculate conversion factor for converting millimetres into
  // logical units.
  // There are approx. 25.1 mm to the inch. There are ppi
  // device units to the inch. Therefore 1 mm corresponds to
  // ppi/25.1 device units. We also divide by the
  // screen-to-printer scaling factor, because we need to
  // unscale to pass logical units to DrawLine.

  // Draw 50 mm by 50 mm L shape
  float logUnitsFactor = (float)(ppiPrinterX/(scale*25.1));
  float logUnits = (float)(50*logUnitsFactor);
  dc->SetPen(wxBLACK_PEN);
  dc->DrawLine(50.0, 50.0, (float)(50.0 + logUnits), 50.0);
  dc->DrawLine(50.0, 50.0, 50.0, (float)(50.0 + logUnits));

  dc->SetFont(itemFont);
  dc->SetBackgroundMode(wxTRANSPARENT);

  dc->DrawText("Some test text", 200.0, 200.0);

  // TESTING
  
  int leftMargin = 20;
  int rightMargin = 20;
  int topMargin = 20;
  int bottomMargin = 20;

  int pageWidthMM, pageHeightMM;
  GetPageSizeMM(&pageWidthMM, &pageHeightMM);

  float leftMarginLogical = (float)(logUnitsFactor*leftMargin);
  float topMarginLogical = (float)(logUnitsFactor*topMargin);
  float bottomMarginLogical = (float)(logUnitsFactor*(pageHeightMM - bottomMargin));
  float rightMarginLogical = (float)(logUnitsFactor*(pageWidthMM - rightMargin));

  dc->SetPen(wxBLACK_PEN);
  dc->DrawLine(leftMarginLogical, topMarginLogical, rightMarginLogical, topMarginLogical);
  dc->DrawLine(leftMarginLogical, bottomMarginLogical, rightMarginLogical, bottomMarginLogical);

  WritePageHeader(this, dc, "A header", logUnitsFactor);
}

// Writes a header on a page. Margin units are in millimetres.
Bool WritePageHeader(wxPrintout *printout, wxDC *dc, char *text, float mmToLogical)
{
  static wxFont *headerFont = NULL;
  if (!headerFont)
  {
    headerFont = wxTheFontList->FindOrCreateFont(16, wxSWISS, wxNORMAL, wxBOLD);
  }
  dc->SetFont(headerFont);

  int pageWidthMM, pageHeightMM;
  
  printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);

  int leftMargin = 10;
  int topMargin = 10;
  int rightMargin = 10;

  float leftMarginLogical = (float)(mmToLogical*leftMargin);
  float topMarginLogical = (float)(mmToLogical*topMargin);
  float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));

  float xExtent, yExtent;
  dc->GetTextExtent(text, &xExtent, &yExtent);
  float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
  dc->DrawText(text, xPos, topMarginLogical);

  dc->SetPen(wxBLACK_PEN);
  dc->DrawLine(leftMarginLogical, topMarginLogical+yExtent, rightMarginLogical, topMarginLogical+yExtent);

  return TRUE;
}
