/*
 * File:	wb_main.h
 * Purpose:	wxApp declaration and a few other functions.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wb_main.h	1.2 5/9/94" */

#ifndef wxb_mainh
#define wxb_mainh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_obj.h"
#include "wx_mgstr.h"

// Here's a macro you can use if your compiler
// really, really wants main() to be in your main program
// (e.g. hello.cc)

#if defined(AIX) || defined(AIX4) /* || defined(__hpux) */
#define IMPLEMENT_WXWIN_MAIN int main(int argc, char *argv[]) { return wxEntry(argc, argv); }
#else
#define IMPLEMENT_WXWIN_MAIN
#endif


#ifdef IN_CPROTO
typedef       void    *wxbApp ;
#else

class wxFrame;
class wxWindow;
class wxApp ;

#define wxPRINT_WINDOWS         1
#define wxPRINT_POSTSCRIPT      2

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxbApp: public wxObject
{
 public:
  int wantDebugOutput ;
  wxlanguage_t wxLang;
  char *wx_class;
  char *appName;
  wxFrame *wx_frame;
  int argc;
  char **argv;
  Bool death_processed;
  Bool exitOnFrameDelete;
  Bool showOnInit;
  int printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
  void (*work_proc)(wxApp*app); // work procedure;
  
  wxbApp(wxlanguage_t language = wxLANGUAGE_DEFAULT);
  ~wxbApp(void);
  virtual wxFrame *OnInit(void);
  virtual int OnExit(void);
  virtual int MainLoop(void) = 0;
  virtual Bool Initialized(void);
  virtual Bool Pending(void) = 0 ;
  virtual void Dispatch(void) = 0 ;
  inline void SetPrintMode(int mode) { printMode = mode; }
  inline int GetPrintMode(void) { return printMode; }
  
  inline void SetExitOnFrameDelete(Bool flag) { exitOnFrameDelete = flag; }
  inline Bool GetExitOnFrameDelete(void) { return exitOnFrameDelete; }

  inline void SetShowFrameOnInit(Bool flag) { showOnInit = flag; }
  inline Bool GetShowFrameOnInit(void) { return showOnInit; }

  virtual char *GetAppName(void);
  virtual void SetAppName(char *name);

  virtual char *GetClassName(void);
  virtual void SetClassName(char *name);
  virtual wxWindow *GetTopWindow(void);
  
  virtual void DeletePendingObjects(void);
  inline wxlanguage_t GetLanguage(void) { return wxLang; }
};

WXDLLEXPORT extern wxApp *wxTheApp;

WXDLLEXPORT void wxCleanUp(void);
WXDLLEXPORT void wxCommonCleanUp(void); // Call this from the platform's wxCleanUp()
WXDLLEXPORT void wxCommonInit(void);    // Call this from the platform's initialization

// Force an exit from main loop
WXDLLEXPORT void wxExit(void);

// Yield to other apps/messages
WXDLLEXPORT Bool wxYield(void);

#endif // IN_CPROTO
#endif
