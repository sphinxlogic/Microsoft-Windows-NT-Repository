/*
 * File:	wx_main.h
 * Purpose:	wxApp declaration and a few other functions.
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_main.h	1.2 5/9/94" */

#ifndef wx_mainh
#define wx_mainh

#include "wx_defs.h"
#include "wx_obj.h"
#include "wx_stdev.h"
#include "wx_mgstr.h"
#include "wb_main.h"

#ifdef IN_CPROTO
typedef       void    *wxApp ;
#else

class wxFrame;
class wxKeyEvent;

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp: public wxbApp
{
  DECLARE_DYNAMIC_CLASS(wxApp)

 public:
  HINSTANCE hInstance;
  MSG current_msg;
  BOOL keep_going ;
  Bool resourceCollection;
  Bool pendingCleanup; // TRUE if we need to check the GDI object lists for cleanup

  // this variable holds the display mode, which is
  // passed to the WinMain function.
  int nCmdShow;

  wxApp(wxlanguage_t language = wxLANGUAGE_ENGLISH);
  ~wxApp(void);

  virtual int MainLoop(void);
  void ExitMainLoop(void);
  Bool Initialized(void);
  virtual Bool Pending(void) ;
  virtual void Dispatch(void) ;

  virtual BOOL DoMessage(void);
  virtual BOOL ProcessMessage(MSG* pMsg);
  virtual BOOL OnIdle(void);

  // Windows specific. Intercept keyboard input.
  virtual Bool OnCharHook(wxKeyEvent& event);

  // Windows specific. Set resource collection scheme on or off.
  inline void SetResourceCollection(Bool flag) { resourceCollection = flag; }
  inline Bool GetResourceCollection(void) { return resourceCollection; }
  inline void SetPendingCleanup(Bool flag) { pendingCleanup = flag; }
  inline Bool GetPendingCleanup(void) { return pendingCleanup; }
  Bool DoResourceCleanup(void);
};

extern HINSTANCE wxhInstance;

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))
WXDLLEXPORT int wxEntry(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpszCmdLine,
                    int nCmdShow, Bool enterLoop = TRUE);
#else
WXDLLEXPORT int wxEntry(HINSTANCE hInstance);
#endif

#endif // IN_CPROTO
#endif
