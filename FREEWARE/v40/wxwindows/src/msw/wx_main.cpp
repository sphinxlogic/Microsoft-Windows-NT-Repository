/*
 * File:	wx_main.cc
 * Purpose:	wxApp implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_main.cc,v 1.1 1994/08/14 21:59:17 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

// For compilers that support precompilation, includes "wx.h".
#define IN_WX_MAIN_CPP
#include "wx_prec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_frame.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_gdi.h"
#include "wx_dc.h"
#include "wx_dialg.h"
#include "wx_cmdlg.h"
#endif

#include "wx_mgstr.h"
#include "wx_privt.h"
#include "wx_itemp.h" // Private header for items

#include <string.h>

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
#define APIENTRY FAR PASCAL
#endif
 
#ifdef WIN32
#define _EXPORT
#else
#define _EXPORT _export
#endif
 
#if !defined(WIN32)
#define DLGPROC FARPROC
#endif

#if FAFA_LIB
#include "fafa.h"
#endif

#if USE_GAUGE
#include "..\..\contrib\gauge\zyzgauge.h"
#endif

#if CTL3D
#include <ctl3d.h>
#endif

#if WIN95
#ifndef GNUWIN32
#include <commctrl.h>
#endif
#endif

#if !defined(WIN32) && !defined(__WATCOMC__)

#ifdef __BORLANDC__
#define RPA_DEFAULT 1
#else
#include <penwin.h>
#endif

#endif

HINSTANCE wxhInstance = 0;

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
extern wxList *wxWinHandleList;
#else
extern wxNonlockingHashTable *wxWinHandleList;
#endif
extern FARPROC wxGenericControlSubClassProc;

extern void wxSetKeyboardHook(Bool doIt);

long last_msg_time; /* MATTHEW: timeStamp implementation */

/* Hack to support Watcom 32-bit Windows, DLLs etc */
//#if defined(__WINDOWS_386__) || defined(_WINDLL)
//# define _MULTIPLE_INSTANCES
//#endif
//*** I'll recommend to define it in the compiler command line.
//*** Generally you use WATCOM for the big applications, 
//*** so multiple instances are not so necessary.
//*** If you still want to use multiple instances for WATCOM be careful:
//*** you should make unique all windows classes names in all modules!
//*** I did it for wxWindows itself and for fafa.lib. 
//*** I didn't improve anything else!!!
//*** D.Chubraev.

#ifdef _MULTIPLE_INSTANCES
# define ZZ	32
#else
# define ZZ	/**/
#endif
char wxFrameClassName[ZZ]         = "wxFrameClass";
char wxMDIFrameClassName[ZZ]      = "wxMDIFrameClass";
char wxMDIChildFrameClassName[ZZ] = "wxMDIChildFrameClass";
char wxPanelClassName[ZZ]         = "wxPanelClass";
char wxCanvasClassName[ZZ]        = "wxCanvasClass";
#undef ZZ

HICON wxSTD_FRAME_ICON = NULL;
HICON wxSTD_MDICHILDFRAME_ICON = NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = NULL;

HICON wxDEFAULT_FRAME_ICON = NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON = NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON = NULL;

HFONT wxSTATUS_LINE_FONT = NULL;
LRESULT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);


// PenWindows Support

#if !defined(__WATCOMC__) && !defined(GNUWIN32)
 
HANDLE hPenWin = (HANDLE)NULL;
#ifndef __WINDOWS_386__
typedef VOID (CALLBACK * PENREGPROC)(WORD,BOOL);
#endif

// The routine below allows Windows applications (binaries) to
// support Pen input when running under Microsoft Windows for
// Pen Computing 1.0 without need of the PenPalete.
//
// Should masked edit functions be added to wxWindows we would
// be a new class of functions to support BEDIT controls.
//
// (The function is a NOOP for native Windows-NT)

// Moved outside the function by JACS to make this file compile
#ifndef WIN32
#ifdef __WINDOWS_386__
  static HINDIR RegPenAppHandle = 0;
# define RegPenApp(u, b) (void)InvokeIndirectFunction(RegPenAppHandle, u, b)
#else
  static  VOID (CALLBACK * RegPenApp) (WORD, BOOL) = NULL;
#endif /* Watcom 32-bit Windows supervisor */
#endif

static void EnablePenAppHooks (Bool hook)
{
#ifndef WIN32
//  if (wxGetOsVersion() == wxWINDOWS_NT) return;

  if (hook)
    {
      if (hPenWin)
	return;

      ///////////////////////////////////////////////////////////////////////
      // If running on a Pen Windows system, register this app so all
      // EDIT controls in dialogs are replaced by HEDIT controls.
      if ((hPenWin = (HANDLE)GetSystemMetrics (SM_PENWINDOWS)) != (HANDLE) NULL)
	{
	  // We do this fancy GetProcAddress simply because we don't
	  // know if we're running Pen Windows.
#ifdef __WINDOWS_386__
	  FARPROC addr;
	  if ((addr = GetProcAddress (hPenWin, "RegisterPenApp")) != NULL)
	    {
	      RegPenAppHandle = GetIndirectFunctionHandle (addr, INDIR_WORD, INDIR_WORD, INDIR_ENDLIST);
	      RegPenApp (RPA_DEFAULT, TRUE);
	    }
#else /* Normal DLL calling convention */
	  if ((RegPenApp = (PENREGPROC)GetProcAddress (hPenWin, "RegisterPenApp")) != NULL)
	    (*RegPenApp) (RPA_DEFAULT, TRUE);
#endif /* Watcom 32-bit Windows supervisor */
	}
    }
  else
    {
      ///////////////////////////////////////////////////////////////////////
      // If running on a Pen Windows system, unregister
      if (hPenWin)
	{
	  // Unregister this app 
#ifdef __WINDOWS_386__
	  if (RegPenAppHandle)
	    RegPenApp (RPA_DEFAULT, FALSE);
# undef RegPenApp
#else /* Normal DLL calling convention */
	  if (RegPenApp != NULL)
	    (*RegPenApp) (RPA_DEFAULT, FALSE);
#endif /* Watcom 32-bit Windows supervisor */
	  hPenWin = (HANDLE) NULL;
	}
    }
#endif	/* ! Windows-NT */
}

#endif
  // End Watcom

void wxInitialize(HANDLE hInstance)
{
  wxCommonInit();

#if WIN95
  InitCommonControls();
  if (!LoadLibrary("RICHED32.DLL"))
  {
    wxMessageBox("Could not initialise Rich Edit DLL");
  }
#endif

#if CTL3D
  if (!Ctl3dRegister(hInstance))                                       
    wxFatalError("Cannot register CTL3D");

  Ctl3dAutoSubclass(hInstance);
#endif
#if FAFA_LIB
  InitFafa(hInstance);
#endif
#if USE_GAUGE
  if (!gaugeInit(hInstance))
    wxFatalError("Cannot initalize Gauge library");
#endif

  wxSTD_FRAME_ICON = LoadIcon(hInstance, "wxSTD_FRAME");
  wxSTD_MDIPARENTFRAME_ICON = LoadIcon(hInstance, "wxSTD_MDIPARENTFRAME");
  wxSTD_MDICHILDFRAME_ICON = LoadIcon(hInstance, "wxSTD_MDICHILDFRAME");

  wxDEFAULT_FRAME_ICON = LoadIcon(hInstance, "wxDEFAULT_FRAME");
  wxDEFAULT_MDIPARENTFRAME_ICON = LoadIcon(hInstance, "wxDEFAULT_MDIPARENTFRAME");
  wxDEFAULT_MDICHILDFRAME_ICON = LoadIcon(hInstance, "wxDEFAULT_MDICHILDFRAME");

  wxSTATUS_LINE_FONT = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0,
                    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS,
                    "Arial");

///////////////////////////////////////////////////////////////////////
// Register the frame window class.
  WNDCLASS wndclass;   // Structure used to register Windows class.

  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass.hInstance     = hInstance;
  wndclass.hIcon         = NULL;        // wxSTD_FRAME_ICON;
  wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
#if FAFA_LIB
  wndclass.hbrBackground =  (HBRUSH)(COLOR_APPWORKSPACE+1) ;
#else
  wndclass.hbrBackground = GetStockObject( WHITE_BRUSH );
#endif
  wndclass.lpszMenuName  = NULL;
#ifdef _MULTIPLE_INSTANCES
  sprintf( wxFrameClassName,"wxFrameClass%d", hInstance );
#endif
  wndclass.lpszClassName = wxFrameClassName;

  if (!RegisterClass( &wndclass ))
    wxFatalError("Can't register Frame Window class");

///////////////////////////////////////////////////////////////////////
// Register the MDI frame window class.
  WNDCLASS wndclass1;   // Structure used to register Windows class.

  wndclass1.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass1.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass1.cbClsExtra    = 0;
  wndclass1.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass1.hInstance     = hInstance;
  wndclass1.hIcon         = NULL; // wxSTD_MDIPARENTFRAME_ICON;
  wndclass1.hCursor       = LoadCursor( NULL, IDC_ARROW );
#if FAFA_LIB
  wndclass1.hbrBackground =  (HBRUSH)(COLOR_APPWORKSPACE+1) ;
#else
  wndclass1.hbrBackground = NULL;
#endif
  wndclass1.lpszMenuName  = NULL;

#ifdef _MULTIPLE_INSTANCES
  sprintf( wxMDIFrameClassName,"wxMDIFrameClass%d", hInstance );
#endif
  wndclass1.lpszClassName = wxMDIFrameClassName;
  if (!RegisterClass( &wndclass1 ))
    wxFatalError("Can't register MDI Frame window class");

///////////////////////////////////////////////////////////////////////
// Register the MDI child frame window class.
  WNDCLASS wndclass4;   // Structure used to register Windows class.

  wndclass4.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass4.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass4.cbClsExtra    = 0;
  wndclass4.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass4.hInstance     = hInstance;
  wndclass4.hIcon         = NULL;       // wxSTD_MDICHILDFRAME_ICON;
  wndclass4.hCursor       = LoadCursor( NULL, IDC_ARROW );
#if FAFA_LIB
  wndclass4.hbrBackground =  (HBRUSH)(COLOR_WINDOW+1) ;
#else
  wndclass4.hbrBackground = NULL;
#endif
  wndclass4.lpszMenuName  = NULL;
#ifdef _MULTIPLE_INSTANCES
  sprintf( wxMDIChildFrameClassName,"wxMDIChildFrameClass%d", hInstance );
#endif
  wndclass4.lpszClassName = wxMDIChildFrameClassName;

  if (!RegisterClass( &wndclass4 ))
   wxFatalError("Can't register MDI child frame window class");

///////////////////////////////////////////////////////////////////////
// Register the panel window class.
  WNDCLASS wndclass2;   // Structure used to register Windows class.
  memset(&wndclass2, 0, sizeof(WNDCLASS));   // start with NULL defaults
  // Use CS_OWNDC to avoid messing about restoring the context
  // for every graphic operation.
  wndclass2.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass2.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass2.cbClsExtra    = 0;
  wndclass2.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass2.hInstance     = hInstance;
  wndclass2.hIcon         = NULL;
  wndclass2.hCursor       = NULL;
#if (FAFA_LIB && !USE_GREY_BACKGROUND)
  // wndclass2.hbrBackground = GetStockObject( LTGRAY_BRUSH );
  // No no no... After investigations, I found that Ctl3d use BTNFACE color
  // (which is ALWAYS grey :-))
  // So, respect the behavior!
  wndclass2.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1) ;
#else
  wndclass2.hbrBackground = GetStockObject( LTGRAY_BRUSH );
#endif
  wndclass2.lpszMenuName  = NULL;
#ifdef _MULTIPLE_INSTANCES
  sprintf( wxPanelClassName,"wxPanelClass%d", hInstance );
#endif
  wndclass2.lpszClassName = wxPanelClassName;
  if (!RegisterClass( &wndclass2 ))
   wxFatalError("Can't register Panel Window class");

///////////////////////////////////////////////////////////////////////
// Register the canvas and textsubwindow class name
  WNDCLASS wndclass3;   // Structure used to register Windows class.
  memset(&wndclass3, 0, sizeof(WNDCLASS));   // start with NULL defaults
  // Use CS_OWNDC to avoid messing about restoring the context
  // for every graphic operation.
  wndclass3.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS ; 
  wndclass3.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass3.cbClsExtra    = 0;
  wndclass3.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass3.hInstance     = hInstance;
  wndclass3.hIcon         = NULL;
  wndclass3.hCursor       = NULL;
#if FAFA_LIB
  wndclass3.hbrBackground = (HBRUSH)(COLOR_WINDOW+1) ;
#else
  wndclass3.hbrBackground = NULL;
#endif
  wndclass3.lpszMenuName  = NULL;
#ifdef _MULTIPLE_INSTANCES
  sprintf( wxCanvasClassName,"wxCanvasClass%d", hInstance );
#endif
  wndclass3.lpszClassName = wxCanvasClassName;
  if (!RegisterClass( &wndclass3))
   wxFatalError("Can't register Canvas class");

#ifndef __WATCOMC__
#if !defined(__win32s__) && !defined(WIN32)
///////////////////////////////////////////////////////////////////////
// If running on a Pen Windows system, register this app so all
// EDIT controls in dialogs are replaced by HEDIT controls.
// (Notice the CONTROL statement in the RC file is "EDIT",
// RegisterPenApp will automatically change that control to
// an HEDIT.
  if ((hPenWin = (HANDLE)GetSystemMetrics(SM_PENWINDOWS)) != (HANDLE)NULL) {
    // We do this fancy GetProcAddress simply because we don't
    // know if we're running Pen Windows.
//   if ( ((FARPROC)RegPenApp = GetProcAddress(hPenWin, "RegisterPenApp"))!= NULL)
// ADDED THIS CAST TO MAKE IT COMPILE UNDER VC++ -- JACS
   if ( (RegPenApp = (VOID (CALLBACK *)(WORD, BOOL))GetProcAddress(hPenWin, "RegisterPenApp"))!= NULL)
     (*RegPenApp)(RPA_DEFAULT, TRUE);
  }
///////////////////////////////////////////////////////////////////////
#endif
#endif

#if !WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  wxWinHandleList = new wxList(wxKEY_INTEGER);
#else
  wxWinHandleList = new wxNonlockingHashTable();
#endif

  // This is to foil optimizations in Visual C++ that
  // throw out dummy.obj.
#if (_MSC_VER >= 800) && !defined(WXMAKINGDLL)
  extern char wxDummyChar;
  if (wxDummyChar) wxDummyChar++;
#endif
  wxSetKeyboardHook(TRUE);
}


// Cleans up any wxWindows internal structures left lying around
void wxCleanUp(void)
{
#if WXGARBAGE_COLLECTION_ON /* MATTHEW: GC */
  extern wxList wxModelessWindows;
  wxNode *node;
  while (node = wxModelessWindows.First())
    ((wxWindow *)node->Data())->Show(FALSE);
  GC_gcollect();
#endif

  wxSetKeyboardHook(FALSE);
  wxCommonCleanUp();

#ifndef __WATCOMC__
#if !defined(__win32s__) && !defined(WIN32)
  if (hPenWin) {
    // Unregister this app 
    if (RegPenApp != NULL)
	(*RegPenApp)(RPA_DEFAULT, FALSE);
  }
#endif
#endif
  
  if (wxSTD_FRAME_ICON)
    DestroyIcon(wxSTD_FRAME_ICON);
  if (wxSTD_MDICHILDFRAME_ICON)
    DestroyIcon(wxSTD_MDICHILDFRAME_ICON);
  if (wxSTD_MDIPARENTFRAME_ICON)
    DestroyIcon(wxSTD_MDIPARENTFRAME_ICON);

  if (wxDEFAULT_FRAME_ICON)
    DestroyIcon(wxDEFAULT_FRAME_ICON);
  if (wxDEFAULT_MDICHILDFRAME_ICON)
    DestroyIcon(wxDEFAULT_MDICHILDFRAME_ICON);
  if (wxDEFAULT_MDIPARENTFRAME_ICON)
    DestroyIcon(wxDEFAULT_MDIPARENTFRAME_ICON);

  DeleteObject(wxSTATUS_LINE_FONT);
#if CTL3D
  Ctl3dUnregister(wxhInstance);
#endif
#if FAFA_LIB
  EndFafa() ;
#endif
  if (wxGenericControlSubClassProc)
    FreeProcInstance(wxGenericControlSubClassProc);
    
  delete wxControlHandleList;
  
  if (wxWinHandleList)
    delete wxWinHandleList ;
    
/* Is this necessary/possible? What if we unregister whilst another wxWin
 * app is running?
  // Unregister window classes
  UnregisterClass(wxFrameClassName, wxhInstance);
  UnregisterClass(wxMDIFrameClassName, wxhInstance);
  UnregisterClass(wxMDIChildFrameClassName, wxhInstance);
  UnregisterClass(wxPanelClassName, wxhInstance);
  UnregisterClass(wxCanvasClassName, wxhInstance);
*/
}

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))

// Main wxWindows entry point

int wxEntry(HINSTANCE hInstance, HINSTANCE WXUNUSED(hPrevInstance), char* m_lpCmdLine,
                    int nCmdShow, Bool enterLoop)
{
  wxhInstance = hInstance;

  wxInitialize(hInstance);

  if (!wxTheApp) {
    MessageBox(NULL, "You have to define an instance of wxApp!", "wxWindows Error", MB_APPLMODAL | MB_ICONSTOP | MB_OK);
    return 0;
  }
  wxSetLanguage(wxTheApp->GetLanguage());

  // Split command line into tokens, as in usual main(argc, argv)
  char **command = new char*[50];
  
  int count = 0;
  char *buf = new char[strlen(m_lpCmdLine) + 1];
  
  // Hangs around until end of app. in case
  // user carries pointers to the tokens

  /* Model independent strcpy */
  {
    int i;
    for (i = 0; (buf[i] = m_lpCmdLine[i]) != 0; i++)
    {
      /* loop */;
    }
  }

  // Get application name
  {
    char name[200];
    ::GetModuleFileName(hInstance, name, 199);

  // Is it only 16-bit Borland that already copies the program name
  // to the first argv index?
#if !defined(GNUWIN32)
// #if ! (defined(__BORLANDC__) && !defined(WIN32))
    command[count++] = copystring(name);
// #endif
#endif

    strcpy(name, wxFileNameFromPath(name));
    wxStripExtension(name);
    wxTheApp->SetAppName(name);
  }

  /* Break-up string */
  // Treat strings enclosed in double-quotes as single arguments
    char* str = buf;
	while (*str)
	{
		while (*str && *str <= ' ') str++;	// skip whitespace
		if (*str == '"')
		{
			str++;
			command[count++] = str;
			while (*str && *str != '"') str++;
		}
		else if (*str)
		{
			command[count++] = str;
			while (*str && *str > ' ') str++;
		}
		if (*str) *str++ = '\0';
	}

  command[count] = NULL; /* argv[] is NULL terminated list! */

  wxTheApp->argc = count;
  wxTheApp->argv = command;
  wxTheApp->hInstance = hInstance;
// store the show-mode parameter of MSW for (maybe) later use.
// this can be used to inform the program about special show modes
// under MSW
  wxTheApp->nCmdShow = nCmdShow; // added by steve, 27.11.94

  wxTheApp->wx_frame = wxTheApp->OnInit();
  
  if (!enterLoop)
    return 0;

  int retValue = 1;
  if (wxTheApp->wx_frame && wxTheApp->wx_frame->handle) {
    // show the toplevel frame, only if we are not iconized (from MS-Windows)
    if(wxTheApp->GetShowFrameOnInit() && (nCmdShow!=SW_HIDE)) wxTheApp->wx_frame->Show(TRUE);
    retValue = wxTheApp->MainLoop();
  }

  if (wxTheApp->wx_frame)
  {
    wxTheApp->wx_frame->GetEventHandler()->OnClose();
    delete wxTheApp->wx_frame;
  }

  wxTheApp->OnExit();

  wxCleanUp();
  delete [] buf ;
  delete [] command[0] ;
  delete [] command ;
  return retValue;
}

#else /*  _WINDLL  */

int wxEntry(HINSTANCE hInstance)
{
  wxhInstance = hInstance;
  wxInitialize(hInstance);

  wxTheApp->argc = 0;
  wxTheApp->argv = NULL;
  wxTheApp->hInstance = hInstance;

  wxTheApp->wx_frame = wxTheApp->OnInit();
  if (wxTheApp->wx_frame && wxTheApp->wx_frame->handle) {
    wxTheApp->wx_frame->Show(TRUE);
  }

  return 1;
}
#endif // _WINDLL

wxApp::wxApp(wxlanguage_t language):wxbApp(language)
{
  wx_frame = NULL;
  wxTheApp = this;
  death_processed = FALSE;
  work_proc = NULL ;
  wx_class = NULL;
  resourceCollection = TRUE;
  pendingCleanup = FALSE;
}

wxApp::~wxApp(void)
{
}

Bool wxApp::Initialized(void)
{
#ifndef _WINDLL
  if (wx_frame)
    return TRUE;
  else
    return FALSE;
#endif
#ifdef _WINDLL // Assume initialized if DLL (no way of telling)
  return TRUE;
#endif
}

/*
 * Get and process a message, returning FALSE if WM_QUIT
 * received.
 *
 */
BOOL wxApp::DoMessage(void)
{
  if (!::GetMessage(&current_msg, NULL, NULL, NULL))
  {
    return FALSE;
  }
    
  // Process the message
  if (!ProcessMessage(&current_msg))
  {
    ::TranslateMessage(&current_msg);
    last_msg_time = current_msg.time; /* MATTHEW: timeStamp impl. */
    ::DispatchMessage(&current_msg);
  }
  return TRUE;
/*
 * This method is correct to make the work_proc function,
 * but eats up CPU time doing nothing when executing OnIdle()!
 * So probably it's best to use timers to implement background
 * processing.
  if (::PeekMessage(&current_msg, NULL, NULL, NULL, PM_REMOVE))
  {
    if (current_msg.message == WM_QUIT)
      return FALSE;
    else
    {
      // Process the message
      if (!ProcessMessage(&current_msg))
      {
        ::TranslateMessage(&current_msg);
        last_msg_time = current_msg.time; // MATTHEW: timeStamp impl.
        ::DispatchMessage(&current_msg);
      }
      return TRUE;
    }
  }
  return TRUE;
*/
}

/*
 * Keep trying to process messages until WM_QUIT
 * received
 */

int wxApp::MainLoop(void)
{
  keep_going = TRUE;
  while (keep_going)
  {
    while (!::PeekMessage(&current_msg, NULL, NULL, NULL, PM_NOREMOVE) &&
           OnIdle()) {}
    if (!DoMessage())
      keep_going = FALSE;

//    while (Pending())
//      Dispatch();
  }

  return current_msg.wParam;
}

void wxApp::ExitMainLoop(void)
{
  keep_going = FALSE;
}

Bool wxApp::Pending(void)
{
  OnIdle() ;
  return (::PeekMessage(&current_msg, NULL, NULL, NULL, PM_NOREMOVE)) ;
}

void wxApp::Dispatch(void)
{
    if (!DoMessage())
      keep_going = FALSE;
}

/*
 * Give all windows a chance to preprocess
 * the message. Some may have accelerator tables, or have
 * MDI complications.
 */
BOOL wxApp::ProcessMessage(MSG *msg)
{
  HWND hWnd;

  // Anyone for a message? Try youngest descendants first.
  for (hWnd = msg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
  {
    wxWnd *wnd = wxFindWinFromHandle(hWnd);
    if (wnd)
    {
       if (wnd->ProcessMessage(msg))
         return TRUE;

       // STOP if we've reached the top of the hierarchy!
       if (wx_frame && (wnd == (wxWnd *)wx_frame->handle))
          return FALSE;
    }
  }

  if (wx_frame && ((wxWnd *)wx_frame->handle)->ProcessMessage(msg))
     return TRUE;
  else return FALSE;
}

BOOL wxApp::OnIdle(void)
{
  if (resourceCollection && pendingCleanup)
    DoResourceCleanup();
  
  // 'Garbage' collection of windows deleted with Close().
  DeletePendingObjects();
    
  if (work_proc)
    (*work_proc)(this) ;
  return FALSE;
}

// Windows specific. Intercept keyboard input: by default,
// route it to the active frame or dialog box.
Bool wxApp::OnCharHook(wxKeyEvent& event)
{
  wxWindow *win = wxGetActiveWindow();
  if (win)
    return win->GetEventHandler()->OnCharHook(event);
  else
    return FALSE;
}

// Free up font objects that are not being used at present.
Bool wxApp::DoResourceCleanup(void)
{
//  wxDebugMsg("ResourceCleanup\n");

  if (wxTheFontList)
  {
    wxNode *node = wxTheFontList->First();
    while (node)
    {
      wxGDIObject *obj = (wxGDIObject *)node->Data();
      if ((obj->GetResourceHandle() != 0) && (obj->GetResourceUsage() == 0))
      {
//        wxDebugMsg("Freeing font %ld (GDI object %d)\n", (long)obj, (int)obj->GetResourceHandle());
        obj->FreeResource();
      }
      node = node->Next();
    }
  }
  if (wxThePenList)
  {
    wxNode *node = wxThePenList->First();
    while (node)
    {
      wxGDIObject *obj = (wxGDIObject *)node->Data();
      if ((obj->GetResourceHandle() != 0) && (obj->GetResourceUsage() == 0))
      {
//        wxDebugMsg("Freeing pen %ld (GDI object %d)\n", (long)obj, (int)obj->GetResourceHandle());
        obj->FreeResource();
      }
      node = node->Next();
    }
  }
  if (wxTheBrushList)
  {
    wxNode *node = wxTheBrushList->First();
    while (node)
    {
      wxGDIObject *obj = (wxGDIObject *)node->Data();
      if ((obj->GetResourceHandle() != 0) && (obj->GetResourceUsage() == 0))
      {
//        wxDebugMsg("Freeing brush %ld (GDI object %d)\n", (long)obj, (int)obj->GetResourceHandle());
        obj->FreeResource();
      }
      node = node->Next();
    }
  }

  SetPendingCleanup(FALSE);
  return FALSE;
}

void wxExit(void)
{
  if (wxTheApp)
    (void)wxTheApp->OnExit();
  wxCleanUp();
  FatalAppExit(0, "Fatal error: exiting");
}

// Yield to incoming messages
Bool wxYield(void)
{
  MSG msg;
  // We want to go back to the main message loop
  // if we see a WM_QUIT. (?)
  while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && msg.message != WM_QUIT)
  {
    if (!wxTheApp->DoMessage())
      break;
  }

  return TRUE;
}

// Reset background brushes
#if FAFA_LIB
HBRUSH SetupBackground(HWND wnd)
{
char tmp[128] ;

  CreatePensBrushes() ;

#if defined(GNUWIN32) && !defined(GetClassName)
#if defined(UNICODE)
#define GetClassName  GetClassNameW
#else
#define GetClassName  GetClassNameA
#endif
#endif

  GetClassName(wnd,tmp,127) ;
  if (strncmp(tmp,wxCanvasClassName,127)==0         ||
#if !USE_GREY_BACKGROUND
      strncmp(tmp,wxPanelClassName,127)==0          ||
#endif
      strncmp(tmp,wxMDIChildFrameClassName,127)==0
     )
  {
#ifdef WIN32
    SetClassLong(wnd,GCL_HBRBACKGROUND,(LONG)NULL) ;
#else
    SetClassWord(wnd,GCW_HBRBACKGROUND,(WORD)NULL) ;
#endif
    return brushBack ;
  }
  else if (strncmp(tmp,wxFrameClassName,127)==0     ||
           strncmp(tmp,wxMDIFrameClassName,127)==0
          )
  {
#ifdef WIN32
    SetClassLong(wnd,GCL_HBRBACKGROUND,(LONG)NULL) ;
#else
    SetClassWord(wnd,GCW_HBRBACKGROUND,(WORD)NULL) ;
#endif
    return brushFrame ;
  }
  return NULL ;
}
#endif

// May wish not to have a DllMain or WinMain, e.g. if we're programming
// a Netscape plugin.
#ifndef NOMAIN

/////////////////////////////////////////////////////////////////////////////////
// WinMain
// Note that WinMain is also defined in dummy.obj, which is linked to
// an application that is using the DLL version of wxWindows.

#if !defined(_DLL)

#ifdef __WATCOMC__
int PASCAL
#else
int APIENTRY
#endif

 WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR m_lpCmdLine, int nCmdShow )
{
  return wxEntry(hInstance, hPrevInstance, m_lpCmdLine, nCmdShow);
}
#endif

/////////////////////////////////////////////////////////////////////////////////
// DllMain

#if defined(_DLL)

// DLL entry point

extern "C"
BOOL WINAPI DllMain (HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
        // Only call wxEntry if the application itself is part of the DLL.
        // If only the wxWindows library is in the DLL, then the initialisation
        // will be called when the application implicitly calls WinMain.

#if !defined(WXMAKINGDLL)
        return wxEntry(hModule);
#endif
	    break;

	case DLL_PROCESS_DETACH:
	default:
	    break;
	}
  return TRUE;
}

#endif // _DLL

#endif // NOMAIN

#undef IN_WX_MAIN_CPP
