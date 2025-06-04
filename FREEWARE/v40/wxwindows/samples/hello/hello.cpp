/*
 * File:	hello.cc
 * Purpose:	Demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   26 May 97    3:22 pm
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

#ifdef _MSC_VER
#pragma warning (disable : 4244)
#endif

#include <ctype.h>
#include "wx_timer.h"
#include "wx_mf.h"
#include "wx_clipb.h"
#include "hello.h"

#if defined(wx_msw) && !defined(__WATCOMC__) && !defined(GNUWIN32)
#include "mmsystem.h"
#endif

// Declare two frames
MyFrame   *frame = NULL;
wxFrame   *subframe = NULL;
wxMenuBar *menu_bar = NULL;
MyTimer   the_timer;
Bool      timer_on = FALSE;
wxBitmap  *test_bitmap = NULL;
wxIcon    *test_icon = NULL;
wxMenu	  *scale_menu,*the_file_menu ;

wxButton *btn1,*btn2 ;
wxMenu    *popupMenu = NULL;

// This statement initialises the whole application
MyApp     myApp;

// For drawing lines in a canvas
float     xpos = -1;
float     ypos = -1;

// Must initialise these in OnInit, not statically
wxPen     *red_pen;
wxFont    *labelFont;
wxFont    *itemFont;
wxFont    *textWindowFont;

float     zoom_factor = 1.0;

#ifdef wx_x
#include "aiai.xbm"
#include "fload.xbm"
#endif

#define	BUTTON_TAG	1
#define	PIXMAP_TAG	2

#define	MAX_FILES	5
int	nbLoadedFiles=0 ;
char	*loadedFiles[MAX_FILES] ;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// Testing of ressources
MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
#if defined(wx_msw) && !defined(__WATCOMC__) && !defined(GNUWIN32)
  mciSendString("play welcome.wav", wxBuffer, 1000, 0);
#endif

  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a small font
  itemFont = new wxFont(11, wxROMAN, wxNORMAL, wxNORMAL);
  labelFont = new wxFont(12, wxROMAN, wxITALIC, wxBOLD);
  textWindowFont = new wxFont(12, wxSWISS, wxNORMAL, wxNORMAL);

  // Create the main frame window
  frame = new MyFrame(NULL, "Hello wxWindows", 0, 0, 550, 500);

  // Give it a status line
  frame->CreateStatusLine(2);

  // Load icon and bitmap
#ifdef wx_msw
  test_icon = new wxIcon("aiai_icn");
  test_bitmap = new wxBitmap("fload") ;
#endif
#ifdef wx_x
  test_icon = new wxIcon(aiai_bits, aiai_width, aiai_height);
  test_bitmap = new wxBitmap(fload_bits,fload_width,fload_height) ;
#endif

  frame->SetIcon(test_icon);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  the_file_menu = file_menu ;

  file_menu->Append(HELLO_LOAD_FILE, "&Load file\tCtrl+L",      "Load a text file");

  wxMenu *pullright_menu = new wxMenu("Title Scale");
  scale_menu = pullright_menu ;
  pullright_menu->Append(HELLO_TWIPS, "&Twips",         "Twips scale");
  pullright_menu->Append(HELLO_LOMETRIC, "&10th mm",    "Metric scale (0.1 mm)");
  pullright_menu->Append(HELLO_METRIC, "&Metric",       "Metric scale (mm)");
  pullright_menu->Append(HELLO_NORMAL, "&Normal size",  "Pixel scale");
  pullright_menu->Append(HELLO_ZOOM, "&Zoom...",        "Zoom by specified amount");

  file_menu->Append(HELLO_SCALE, "&Scale picture", pullright_menu, "Scale picture");

  file_menu->AppendSeparator();
  file_menu->Append(HELLO_PRINT_EPS, "Print to &EPS file", 
                                                        "Print to EPS file");
#ifdef wx_msw
  file_menu->Append(HELLO_PRINT, "&Print",              "Print picture");

#if USE_METAFILE
  file_menu->Append(HELLO_COPY_MF, "&Copy metafile to clipboard",
                                                        "Copy metafile to clipboard");
  file_menu->Append(HELLO_SAVE_MF, "Save &metafile",
                                                        "Save metafile in a file");
#endif
  file_menu->Append(HELLO_COPY_BITMAP, "Copy &bitmap to clipboard",
                                                        "Copy bitmap to clipboard");
#endif
  file_menu->AppendSeparator();
  file_menu->Append(HELLO_QUIT, "&Quit",                "Quit program");

  wxMenu *timer_menu = new wxMenu;
  timer_menu->Append(HELLO_TIMER_ON, "Timer &on",       "Turn timer on");
  timer_menu->Append(HELLO_TIMER_OFF, "Timer o&ff",     "Turn timer off");
  timer_menu->Append(HELLO_CHECKABLE, "&Toggle item",    "Checkable item",TRUE);

  wxMenu *cursor_menu = new wxMenu;
  cursor_menu->Append(wxCURSOR_ARROW, "Arrow");
  cursor_menu->Append(wxCURSOR_WAIT, "Wait");
  cursor_menu->Append(wxCURSOR_IBEAM, "Ibeam");
  cursor_menu->Append(wxCURSOR_CROSS, "Cross");
  cursor_menu->Append(wxCURSOR_SIZENWSE, "Size NWSE");
  cursor_menu->Append(wxCURSOR_SIZENESW, "Size NESW");
  cursor_menu->Append(wxCURSOR_SIZEWE, "Size WE");
  cursor_menu->Append(wxCURSOR_SIZENS, "Size NS");
  cursor_menu->Append(wxCURSOR_PENCIL, "Pencil");
  cursor_menu->Append(wxCURSOR_BULLSEYE, "Bullseye");
  cursor_menu->Append(wxCURSOR_MAGNIFIER, "Magnifier");
  cursor_menu->Append(wxCURSOR_HAND, "Hand");
  cursor_menu->Append(wxCURSOR_NO_ENTRY, "No Entry");
  cursor_menu->Append(wxCURSOR_CHAR, "Char");
  cursor_menu->Append(wxCURSOR_LEFT_BUTTON, "Left Button");
  cursor_menu->Append(wxCURSOR_RIGHT_BUTTON, "Right Button");
  cursor_menu->Append(wxCURSOR_MIDDLE_BUTTON, "Middle Button");
  cursor_menu->Append(wxCURSOR_QUESTION_ARROW, "Question arrow");
  cursor_menu->Append(wxCURSOR_SIZING, "Sizing");
  cursor_menu->Append(wxCURSOR_SPRAYCAN, "Spraycan");
  cursor_menu->Append(wxCURSOR_WATCH, "Watch");
  cursor_menu->Append(wxCURSOR_POINT_LEFT, "Point left");
  cursor_menu->Append(wxCURSOR_POINT_RIGHT, "Point right");
  cursor_menu->Append(wxCURSOR_PAINT_BRUSH, "Paint brush");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(HELLO_ABOUT, "&About",              "About Hello wxWindows");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(timer_menu, "&Timer");
  menu_bar->Append(cursor_menu, "&Cursor");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  menu_bar->Enable(HELLO_TIMER_ON, TRUE);
  menu_bar->Enable(HELLO_TIMER_OFF, FALSE);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 1000, 500, 0, "MyMainFrame");
  
  frame->panel->SetLabelPosition(wxVERTICAL);
  frame->panel->SetLabelFont(labelFont);
  frame->panel->SetButtonFont(itemFont);
  frame->panel->SetFont(itemFont);

  // Create some panel items
  btn1 = new wxButton(frame->panel, (wxFunction)&button_proc, "A button") ;
  btn1->SetClientData((char*)BUTTON_TAG) ;

  btn2 = new wxButton(frame->panel, (wxFunction)&button_proc, test_bitmap) ;
  btn2->SetClientData((char*)PIXMAP_TAG) ;

  // A wxText that processes the ENTER key (this _may_ foul up tab traversal)
  wxText *txt = new wxText(frame->panel, (wxFunction)&text_proc, "A text item", "Initial value",
    -1, -1, 150, -1); //, wxPROCESS_ENTER) ;

  wxCheckBox *check = new wxCheckBox(frame->panel, (wxFunction)&check_proc , "A check box");

  btn1->SetDefault();
  frame->panel->NewLine();

  char *choice_strings[4];
  choice_strings[0] = "Julian";
  choice_strings[1] = "Hattie";
  choice_strings[2] = "Ken";
  choice_strings[3] = "Dick";

  wxChoice *choice = new wxChoice(frame->panel, NULL, "A choice item",
                     -1, -1, -1, -1, 4, choice_strings);
  choice->SetSelection(0);

  char *radio_strings[6];
  radio_strings[0] = "1";
  radio_strings[1] = "2";
  radio_strings[2] = "3";
  radio_strings[3] = "4";
  radio_strings[4] = "5";
  radio_strings[5] = "6";
  wxRadioBox *radioBox = new wxRadioBox(frame->panel,NULL,"Radio box",-1,-1,-1,-1,
                       6,radio_strings,2,wxVERTICAL|wxFLAT);

  wxListBox *list = new wxListBox(frame->panel, (wxFunction)&list_proc, "A list",
                                  wxSINGLE|wxALWAYS_SB, -1, -1, 150, 100);

  list->Append("Apple");
  list->Append("Pear");
  list->Append("Orange");
  list->Append("Banana");
  list->Append("Fruit");

  int multiTextY = -1;
#ifdef wx_xview
  // XView sometimes needs to be cajoled into displaying
  // panel items at the correct position.
  int dummyX;
  list->GetPosition(&dummyX, &multiTextY);
#endif

  wxMultiText *multiText = new wxMultiText(frame->panel, (wxFunction)NULL, "Multiline text", "Some text",
                        -1, multiTextY, 150, 100);

  frame->panel->NewLine();

  wxMessage *message = new wxMessage(frame->panel, "Hello! A simple message");

  wxSlider *slider = new wxSlider(frame->panel, NULL, "A slider",
                     40, 22, 101, 150, -1, -1, wxHORIZONTAL);

  // Make a text window
  frame->text_window = new MyTextWindow(frame, 0, 250, 400, 250, wxNATIVE_IMPL);
  frame->text_window->SetFont(textWindowFont);
  
  frame->text_window->DragAcceptFiles(TRUE);

  // Make another frame, containing a canvas
  subframe = new wxFrame(NULL, "Canvas", 300, 300, 400, 300);

  int width, height;
  subframe->GetClientSize(&width, &height);

  MyCanvas *canvas = new MyCanvas(subframe, 0, 0, width, height, wxRETAINED);

  wxCursor *cursor = new wxCursor(wxCURSOR_PENCIL);
  canvas->SetBackground(wxWHITE_BRUSH);
  canvas->SetCursor(cursor);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
  //  canvas->SetScrollbars(20, 14, 50, 50, 4, 4);
  canvas->SetPen(red_pen);
  frame->canvas = canvas;

  frame->OnSize(550, 500);

  frame->Show(TRUE);
  frame->LoadAccelerators("menus_accel") ;

  subframe->Show(TRUE);

  // Load a file into the text window
  frame->text_window->LoadFile("welcome.txt");
  frame->SetStatusText("Hello, wxWindows");

  popupMenu = new wxMenu(NULL, (wxFunction)PopupFunction);
  popupMenu->Append(100, "Item 1");
  popupMenu->Append(101, "Item 2");
  popupMenu->Append(102, "Item 3");
  popupMenu->Break() ;
  popupMenu->Append(103, "Item 4");
  popupMenu->Append(104, "Item 5");

  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, Const char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel = NULL;
  text_window = NULL;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  wxDC *dc = canvas->GetDC();
  switch (id)
  {
    case HELLO_FILE1:
    case HELLO_FILE2:
    case HELLO_FILE3:
    case HELLO_FILE4:
    case HELLO_FILE5:
      frame->text_window->LoadFile(loadedFiles[id-HELLO_FILE1]);
    break ;
    case HELLO_LOAD_FILE:
    {
      char *s = wxFileSelector("Load text file", NULL, NULL, NULL, "*.txt");
      if (s)
      {
#ifdef wx_msw
        wxUnix2DosFilename(s);
#endif
        frame->text_window->LoadFile(s);
        if (nbLoadedFiles<MAX_FILES)
        {
          if (nbLoadedFiles==0)
            the_file_menu->AppendSeparator() ;
          loadedFiles[nbLoadedFiles] = copystring(s) ;
          if (loadedFiles[nbLoadedFiles])
          {
            the_file_menu->Append(HELLO_FILE1+nbLoadedFiles,s) ;
            nbLoadedFiles++ ;
          }
        }
        else
        {
          if (loadedFiles[0])
            delete [] loadedFiles[0] ;
          for (int i=0;i<MAX_FILES-1;i++)
            loadedFiles[i] = loadedFiles[i+1] ;
          loadedFiles[MAX_FILES-1] = copystring(s) ;
        }
        for (int i=0;i<nbLoadedFiles;i++)
          if (loadedFiles[i])
            the_file_menu->SetLabel(HELLO_FILE1+i, loadedFiles[i]) ;
      }
      break;
    }
    case HELLO_TWIPS:
    {
      dc->SetMapMode(MM_TWIPS);
      dc->Clear();
      Draw(*dc,TRUE);
      break;
    }
    case HELLO_METRIC:
    {
      dc->SetMapMode(MM_METRIC);
      dc->Clear();
      Draw(*dc,TRUE);
      break;
    }
    case HELLO_LOMETRIC:
    {
      dc->SetMapMode(MM_LOMETRIC);
      dc->Clear();
      Draw(*dc,TRUE);
      break;
    }
    case HELLO_NORMAL:
    {
      dc->SetUserScale(1.0, 1.0);
      dc->SetMapMode(MM_TEXT);
      dc->Clear();
      Draw(*dc,TRUE);
      break;
    }
    case HELLO_ZOOM:
    {
      char *zoom_factor_s = FloatToString(zoom_factor);
      char *new_zoom_factor_s = wxGetTextFromUser("Enter new zoom factor", "Input", zoom_factor_s);
      zoom_factor_s = new_zoom_factor_s ;
      if (zoom_factor_s)
      {
        StringToFloat(zoom_factor_s, &zoom_factor);
        dc->SetUserScale(zoom_factor, zoom_factor);
        dc->Clear();
        Draw(*dc,TRUE);
      }
      break;
    }

    case HELLO_CHECKABLE:
      if (menu_bar->Checked(id))
        (void)wxMessageBox(
              "state: Check",
	      "Checkable item",
	       wxOK|wxCENTRE);
      else
        (void)wxMessageBox(
              "state: Uncheck",
	      "Checkable item",
	       wxOK|wxCENTRE);
    break ;

    case HELLO_QUIT:
    {
      for (int i=0;i<nbLoadedFiles;i++)
        if (loadedFiles[i])
          delete [] loadedFiles[i] ;
      this->Close(TRUE);
      break;
    }
    case HELLO_PRINT_EPS:
    {
#if USE_POSTSCRIPT
      wxPostScriptDC dc(NULL, TRUE);
      if (dc.Ok())
      {
        if (dc.StartDoc("Hello printout"))
        {
          dc.StartPage();
          Draw(dc,FALSE);
          dc.EndPage();
          dc.EndDoc();
        }
      }
#endif
      break;
    }
#ifdef wx_msw
    case HELLO_PRINT:
    {
      wxPrinterDC dc(NULL, NULL, NULL);
      if (dc.Ok())
      {
        dc.StartDoc("Hello printout");
        dc.StartPage();
        Draw(dc, TRUE);
        dc.EndPage();
        dc.EndDoc();
      }
      break;
    }
#if USE_METAFILE
    case HELLO_COPY_MF:
    {
      wxMetaFileDC dc;
      if (dc.Ok())
      {
        Draw(dc, FALSE);
        wxMetaFile *mf = dc.Close();
        if (mf)
        {
          Bool success = mf->SetClipboard((int)(dc.MaxX() + 10), (int)(dc.MaxY() + 10));
          delete mf;
        }
      }
      break;
    }
    case HELLO_SAVE_MF:
    {
      char *f = wxFileSelector("Save metafile", NULL, NULL, NULL, "*.wmf", wxSAVE | wxOVERWRITE_PROMPT);
      if (f)
      {
        wxMetaFileDC dc(f);
        if (dc.Ok())
        {
          Draw(dc, FALSE);
          wxMetaFile *mf = dc.Close();
          if (mf)
            delete mf;

          wxMakeMetaFilePlaceable(f, (int)dc.MinX()-10, (int)dc.MinY()-10, (int)dc.MaxX()+10, (int)dc.MaxY()+10);
        }
      }
      break;
    }
#endif
    case HELLO_COPY_BITMAP:
    {
      wxMemoryDC dc;
      wxBitmap *bitmap = new wxBitmap(400, 400);
      if (!bitmap->Ok())
      {
        wxMessageBox("Could not create bitmap.", "Hello demo", wxOK);
        return;
      }
      dc.SelectObject(bitmap);
      dc.Clear();
      Draw(dc, FALSE);
      dc.SelectObject(NULL);
      if (wxOpenClipboard())
      {
        if (!wxSetClipboardData(wxCF_BITMAP, bitmap, 400, 400))
          wxMessageBox("Could not copy bitmap to clipboard.", "Hello demo", wxOK);
        wxCloseClipboard();
      }
      else
        wxMessageBox("Could not open clipboard.", "Hello demo", wxOK);
      
      delete bitmap;
      break;
    }
#endif
    case HELLO_ABOUT:
    {
      (void)wxMessageBox("wxWindows GUI library demo Vsn 1.67\nAuthor: Julian Smart J.Smart@ed.ac.uk\nAIAI (c) 1994",
            "About wxHello", wxOK|wxCENTRE);
      break;
    }
    case HELLO_TIMER_ON:
    {
      the_timer.Start(5000);
      timer_on = TRUE;
      menu_bar->Enable(HELLO_TIMER_ON, FALSE);
      menu_bar->Enable(HELLO_TIMER_OFF, TRUE);
      break;
    }
    case HELLO_TIMER_OFF:
    {
      the_timer.Stop();
      menu_bar->Enable(HELLO_TIMER_ON, TRUE);
      menu_bar->Enable(HELLO_TIMER_OFF, FALSE);
      break;
    }
    default:
    {
      wxCursor *cursor = new wxCursor(id);
      frame->canvas->SetCursor(cursor);
      break;
    }
  }
}

// Size the subwindows when the frame is resized
void MyFrame::OnSize(int w, int h)
{
  if (panel && text_window)
  {
    int width, height;
    GetClientSize(&width, &height);
    panel->SetSize(0, 0, width, (int)(height/2));
    text_window->SetSize(0, (int)(height/2), width, (int)(height/2));
  }
}

void MyFrame::Draw(wxDC& dc, Bool draw_bitmaps)
{
  dc.SetFont(itemFont);

  dc.SetPen(wxGREEN_PEN);
  dc.DrawLine(0.0, 0.0, 200.0, 200.0);
  dc.DrawLine(200.0, 0.0, 0.0, 200.0);

  dc.SetBrush(wxCYAN_BRUSH);
  dc.SetPen(wxRED_PEN);

  dc.DrawRectangle(100.0, 100.0, 100.0, 50.0);
  dc.DrawRoundedRectangle(150.0, 150.0, 100.0, 50.0,20.0);

  dc.SetClippingRegion(150.0, 150.0, 100.0, 50.0);
  dc.DrawText("This text should be clipped within the rectangle", 150.0, 170.0);
  dc.DestroyClippingRegion();

  dc.DrawEllipse(250.0, 250.0, 100.0, 50.0);
#if USE_SPLINES
  dc.DrawSpline(50.0, 200.0, 50.0, 100.0, 200.0, 10.0);
#endif
  dc.DrawLine(50.0, 230.0, 200.0, 230.0);
  dc.DrawText("This is a test string", 50.0, 230.0);

  dc.SetPen(wxBLACK_PEN);
  dc.DrawArc(50.0, 300.0, 100.0, 250.0, 100.0, 300.0);

  if (draw_bitmaps)
  {
    // The easy way to draw bitmaps, using icons
    dc.DrawIcon(test_icon, 250.0, 50.0);

/*  An alternative way to draw bitmaps.
    wxMemoryDC temp_dc(dc);
    temp_dc.SelectObject(test_bitmap);
    dc.Blit(250, 50, BITMAP_WIDTH, BITMAP_HEIGHT, &temp_dc, 0, 0);
*/
    char buf[10];
    for (int i = 0; i < 1000; i += 50)
    {
      sprintf(buf, "%d", i);
      dc.DrawText(buf, (float)i, (float)400);
    }
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
  frame->Draw(*(GetDC()),TRUE);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button. Right button activates a menu.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
  float x, y;
  event.Position(&x, &y);
  wxDC *dc = GetDC();

  if (event.RightDown())
  {
    PopupMenu(popupMenu, dc->LogicalToDeviceX(x), dc->LogicalToDeviceY(y));
  }
  else
  {
    if (xpos > -1 && ypos > -1 && event.Dragging() && event.LeftIsDown())
    {
      SetPen(wxBLACK_PEN);
      SetBrush(wxTRANSPARENT_BRUSH);
      DrawLine(xpos, ypos, x, y);
    }
  }
  xpos = x;
  ypos = y;
}

void MyCanvas::OnScroll(wxCommandEvent& event)
{
  wxCanvas::OnScroll(event);
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
  if (subframe)
    subframe->Close(TRUE);

  delete popupMenu;

  Show(FALSE);

  return TRUE;
}

void quit_proc(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *box = (wxDialogBox *)but.GetParent();
  box->Show(FALSE);
  delete box;
}

// Gets some user input, and sets the status line
void text_proc(wxText& but, wxCommandEvent& event)
{
  if (event.eventType==wxEVENT_TYPE_TEXT_COMMAND)
	*(frame->text_window) << "Value changed\n" ;
  else if (event.eventType==wxEVENT_TYPE_TEXT_ENTER_COMMAND)
	*(frame->text_window) << "Enter key\n" ;
}

// Gets some user input, and sets the status line
void check_proc(wxCheckBox& but, wxCommandEvent& event)
{
  btn2->Enable(!but.GetValue()) ; // test of insensitive Pixmap
}

// Gets some user input, and sets the status line
void button_proc(wxButton& but, wxCommandEvent& event)
{
  int choice ;
  
  long tag = (long)but.GetClientData() ;
  if (tag==PIXMAP_TAG)
  {
    frame->OnMenuCommand(HELLO_LOAD_FILE) ;
    return ;
  }
  
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
  *(frame->text_window) << event.commandString << "\n";

  ostream str(frame->text_window);
  str << "Testing\n";
  str.flush();
#ifdef wx_x
  int *liste,count ;
  count = list.GetSelections(&liste) ;
  for (int i =0;i<count;i++)
	wxDebugMsg("Selection %d\n",liste[i]) ;
  wxDebugMsg("\n") ;
#endif

  if (event.GetEventType() == wxEVENT_TYPE_LISTBOX_DCLICK_COMMAND)
    wxMessageBox("Double click!", "Hello", wxOK);
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}

// Timer notify proc
void MyTimer::Notify(void)
{
  *(frame->text_window) << "Timer went!\n";
}

void PopupFunction(wxMenu& menu, wxCommandEvent& event)
{
  char buf[200];
  sprintf(buf, "Popup menu item selected %d", event.commandInt);
  frame->SetStatusText(buf);
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
