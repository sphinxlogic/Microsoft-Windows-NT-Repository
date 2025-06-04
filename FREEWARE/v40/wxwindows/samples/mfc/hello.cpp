// hello.cpp : Defines the class behaviors for the application.
//           Hello is a simple program which consists of a main window
//           and an "About" dialog which can be invoked by a menu choice.
//           It is intended to serve as a starting-point for new
//           applications.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// *** MODIFIED BY JULIAN SMART TO DEMONSTRATE CO-EXISTANCE WITH wxWINDOWS ***
//
// This sample pops up an initial wxWindows frame, with a menu item
// that allows a new MFC window to be created. Note that CDummyWindow
// is a class that allows a wxWindows window to be seen as a CWnd
// for the purposes of specifying a valid main window to the
// MFC initialisation.
//
// You can easily modify this code so that an MFC window pops up
// initially as the main frame, and allows wxWindows frames to be
// created subsequently:
//
// (1) Make MyApp::OnInit return NULL, not create a window.
// (2) Restore the MFC code to create a window in InitInstance, and remove
//     creation of CDummyWindow.
//
// IMPORTANT NOTE: to compile this sample, you must first edit
// wx/src/msw/wx_main.cc, set NOWINMAIN to 1, and remake wxWindows
// (it only needs to recompile wx_main.cc).
// This eliminates the duplicate WinMain function which MFC implements.

#include "stdafx.h"
#include "resource.h"

#include "hello.h"

#include "wx.h"
#include "wx_privt.h"


/////////////////////////////////////////////////////////////////////////////

// theApp:
// Just creating this application object runs the whole application.
//
CTheApp theApp;

// wxWindows elements

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
    wxFrame *CreateFrame(void);
 };

class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x, int y, int w, int h);
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
};

class MyChild: public wxFrame
{
  public:
    MyCanvas *canvas;
    MyChild(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
    ~MyChild(void);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    void OnActivate(Bool active);
};

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;

// Initialise this in OnInit, not statically
wxPen *red_pen;
wxFont *small_font;

// ID for the menu quit command
#define HELLO_QUIT 1
#define HELLO_NEW  2

MyApp myApp;

/////////////////////////////////////////////////////////////////////////////

// CMainWindow constructor:
// Create the window with the appropriate style, size, menu, etc.
//
CMainWindow::CMainWindow()
{
	LoadAccelTable( "MainAccelTable" );
	Create( NULL, "Hello Foundation Application",
		WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu" );
}

// OnPaint:
// This routine draws the string "Hello, Windows!" in the center of the
// client area.  It is called whenever Windows sends a WM_PAINT message.
// Note that creating a CPaintDC automatically does a BeginPaint and
// an EndPaint call is done when it is destroyed at the end of this
// function.  CPaintDC's constructor needs the window (this).
//
void CMainWindow::OnPaint()
{
	CString s = "Hello, Windows!";
	CPaintDC dc( this );
	CRect rect;

	GetClientRect( rect );
	dc.SetTextAlign( TA_BASELINE | TA_CENTER );
	dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut( ( rect.right / 2 ), ( rect.bottom / 2 ),
				s, s.GetLength() );
}

// OnAbout:
// This member function is called when a WM_COMMAND message with an
// IDM_ABOUT code is received by the CMainWindow class object.  The
// message map below is responsible for this routing.
//
// We create a ClDialog object using the "AboutBox" resource (see
// hello.rc), and invoke it.
//
void CMainWindow::OnAbout()
{
	CDialog about( "AboutBox", this );
	about.DoModal();
}

void CMainWindow::OnTest()
{
  wxMessageBox("This is a wxWindows message box.\nWe're about to create a new wxWindows frame.", "wxWindows", wxOK);
  myApp.CreateFrame();
}

// CMainWindow message map:
// Associate messages with member functions.
//
// It is implied that the ON_WM_PAINT macro expects a member function
// "void OnPaint()".
//
// It is implied that members connected with the ON_COMMAND macro
// receive no arguments and are void of return type, e.g., "void OnAbout()".
//
BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )
	//{{AFX_MSG_MAP( CMainWindow )
	ON_WM_PAINT()
	ON_COMMAND( IDM_ABOUT, OnAbout )
	ON_COMMAND( IDM_TEST, OnTest )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// When any CTheApp object is created, this member function is automatically
// called.  Any data may be set up at this point.
//
// Also, the main window of the application should be created and shown here.
// Return TRUE if the initialization is successful.
//
BOOL CTheApp::InitInstance()
{
	TRACE( "HELLO WORLD\n" );

	SetDialogBkColor();     // hook gray dialogs (was default in MFC V1)

        wxEntry(m_hInstance, m_hPrevInstance, m_lpCmdLine, m_nCmdShow, FALSE);

/*
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow( m_nCmdShow );
	m_pMainWnd->UpdateWindow();
*/

        if (wxTheApp && wxTheApp->GetTopWindow())
        {
          m_pMainWnd = new CDummyWindow(wxTheApp->GetTopWindow()->GetHWND());
        }

	return TRUE;
}

int CTheApp::ExitInstance()
{
  wxCleanUp();

  return CWinApp::ExitInstance();
}

// Override this to provide wxWindows message loop
// compatibility

BOOL CTheApp::PreTranslateMessage(MSG *msg)
{
  if (wxTheApp && wxTheApp->ProcessMessage(msg))
    return TRUE;
  else
    return CWinApp::PreTranslateMessage(msg);
}

/*********************************************************************
 * wxWindows elements
 ********************************************************************/
 
wxFrame *MyApp::OnInit(void)
{
  // Don't exit app when the top level frame is deleted
//  SetExitOnFrameDelete(FALSE);
  
  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a small font
  small_font = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);

  // You can return NULL instead if you like, instead of creating
  // a top-level wxWindows window (for example if you use MFC to create
  // the top-level window).
//  return NULL;

  return CreateFrame();
}

wxFrame *MyApp::CreateFrame(void)
{
      MyChild *subframe = new MyChild(NULL, "Canvas Frame", 10, 10, 300, 300,
                             wxDEFAULT_FRAME | wxSDI);

      subframe->SetTitle("wxWindows canvas frame");

      // Give it a status line
      subframe->CreateStatusLine();

      // Make a menubar
      wxMenu *file_menu = new wxMenu;

      file_menu->Append(HELLO_NEW, "&New MFC Window");
      file_menu->Append(HELLO_QUIT, "&Close");

      wxMenuBar *menu_bar = new wxMenuBar;

      menu_bar->Append(file_menu, "&File");

      // Associate the menu bar with the frame
      subframe->SetMenuBar(menu_bar);

      int width, height;
      subframe->GetClientSize(&width, &height);

      MyCanvas *canvas = new MyCanvas(subframe, 0, 0, width, height);
      wxCursor *cursor = new wxCursor(wxCURSOR_PENCIL);
      canvas->SetCursor(cursor);
      subframe->canvas = canvas;

      // Give it scrollbars
      canvas->SetScrollbars(20, 20, 50, 50, 4, 4);
      canvas->SetPen(red_pen);

      subframe->Show(TRUE);
      // Return the main frame window
      return subframe;
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

MyChild::MyChild(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
  canvas = NULL;
}

MyChild::~MyChild(void)
{
}

// Intercept menu commands
void MyChild::OnMenuCommand(int id)
{
  switch (id)
  {
    case HELLO_NEW:
    {
      CMainWindow *mainWin = new CMainWindow();
      mainWin->ShowWindow( TRUE );
      mainWin->UpdateWindow();
      break;
    }
    case HELLO_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    default:
    {
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


// Dummy MFC window for specifying a valid main window to MFC, using
// a wxWindows HWND.
CDummyWindow::CDummyWindow(HWND hWnd):CWnd()
{
  Attach(hWnd);
}

// Don't let the CWnd destructor delete the HWND
CDummyWindow::~CDummyWindow(void)
{
  Detach();
}


// The MSVC++ 1.5 compiler mysteriously complains that this is
// undefined unless we duplicate it here. WHY???
void wxWindow::PreDelete(HDC WXUNUSED(dc))
{
  mouseInWindow = FALSE ;
}

HWND wxWindow::GetHWND(void)
{
  HWND hWnd = 0;

  switch (wxWinType)
  {
    case wxTYPE_XWND:
    case wxTYPE_MDICHILD:
    {
      wxWnd *wnd = (wxWnd *)handle;
      hWnd = (HWND)wnd->handle;
      break;
    }
    default:
    {
      hWnd = (HWND)ms_handle;
      break;
    }
  }
  return hWnd;
}

// Find an item given the MS Windows handle
wxWindow *wxWindow::FindItemByHWND(HWND hWnd)
{
  if (!children)
    return NULL;
  wxNode *current = children->First();
  while (current)
  {
    wxObject *obj = (wxObject *)current->Data() ;
    if (wxSubType(obj->__type,wxTYPE_PANEL))
    {
      // Do a recursive search.
      wxPanel *panel = (wxPanel*)obj ;
      wxWindow *wnd = panel->FindItemByHWND(hWnd) ;
      if (wnd)
        return wnd ;
    }
    else
    {
      wxItem *item = (wxItem *)current->Data();
      if ((HWND)(item->ms_handle) == hWnd)
        return item;
      else
      {
        // In case it's a 'virtual' control (e.g. radiobox)
        if (item->__type == wxTYPE_RADIO_BOX)
        {
          wxRadioBox *rbox = (wxRadioBox *)item;
          int i;
          for (i = 0; i < rbox->no_items; i++)
            if (rbox->radioButtons[i] == hWnd)
              return item;
        }
      }
    }
    current = current->Next();
  }
  return NULL;
}
