/*
 * File:	wx_main.cc
 * Purpose:	wxApp implementation (X version)
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wx_main.cc,v 1.2 1994/08/14 21:28:43 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "@(#)wx_main.cc	1.2 5/9/94";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#ifndef VMS
#include <pwd.h>
#endif
#include <stdlib.h>

#include "wx_defs.h"
#include "wx_mgstr.h"
#include "wx_frame.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_gdi.h"
#include "wx_dialg.h"
#include "wx_privt.h"

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
/* Experimental removal: gives string.h conflict on Sun/gcc 2.6.3 */
// #include <X11/Xos.h>
#include <X11/Xatom.h>

Colormap wxMainColormap = 0;
long wxMaxRequestSize = 0;

#if USE_RESOURCES
extern void wxFlushResources(void); // Defined in wx_utils.cc
#endif

#ifdef wx_xview
#include <xview/cms.h>
extern Xv_Server xv_default_server;
int x_error_proc(Display *dpy, XErrorEvent *event);
Xv_Server xview_server;
#endif

#ifdef wx_motif
#include <Xm/Xm.h>
#include <X11/Shell.h>
#if WXGARBAGE_COLLECTION_ON
wxNonlockingHashTable *wxWidgetHashTable = NULL;
#else
wxHashTable *wxWidgetHashTable = NULL;
#endif
#endif

void wxCleanUp(void);

//   wxWindows Entry Point 
//   --- changed from main() to _wxEntry to allow
//   for better flexibility. (950215 edz)
//   [motivated by the needs of wxPython]
// for some VERY UNKNOWN reason, the VMS linker produces
// bad code (we get a '%NONAME-W-NOMSG, Message number 00000000'),
// if the 'main.c' file is compiled separately and linked into
// wx.olb. Therefore we do a source-include here, and all works fine.
// Also the 'extern "C"' statement results in the above error
#ifdef VMS
#include "main.c"
#endif

// Compiling with the Sun compiler under Solaris is a bit weird.
// If you get a wxEntry link error, change wxEntry to extern "C"
// or comment it out. I'd like to know what's going on here...
#if USE_C_MAIN
extern "C" int wxEntry(int argc, char *argv[]);
#endif


// If you get a link error for wxEntry, change this 'wxEntry' to 'main'.
// Some compilers don't like to link from main.c to wx_main.cc :-(
// See also above method which is probably better.
int wxEntry(int argc, char *argv[])
{
  if (!wxTheApp) {
    cerr << "wxWindows error: You have to define an instance of wxApp!\n";
    exit(0);
  }

  /* Set class/program without path: */
  char *progname = wxFileNameFromPath(argv[0]);
  wxTheApp->SetClassName(progname);
  wxTheApp->SetAppName(progname);

#ifdef wx_xview
  xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv,
          XV_X_ERROR_PROC, x_error_proc, // @@@ Hack Altert!!!!  
          NULL);
  xview_server = xv_default_server;
#elif defined(wx_motif)
  XtToolkitInitialize() ;
  wxTheApp->appContext = XtCreateApplicationContext() ;
  Display *dpy = XtOpenDisplay(wxTheApp->appContext,(String)NULL,NULL,
                            wxTheApp->GetClassName(),NULL,
# if XtSpecificationRelease < 5
                            0,(Cardinal*) &argc,argv) ;
# else
                            0,&argc,argv) ;
# endif
  if (!dpy) {
    cerr << "wxWindows could not open display for " << wxTheApp->GetClassName() << ": exiting.\n";
    exit(-1);
  }
  wxTheApp->topLevel = XtAppCreateShell((String)NULL,wxTheApp->GetClassName(),
                                     applicationShellWidgetClass,dpy,
                                     NULL,0) ;

  // Add general resize proc
  XtActionsRec rec;
  rec.string = "resize";
  rec.proc = (XtActionProc)wxWidgetResizeProc;
  XtAppAddActions(wxTheApp->appContext, &rec, 1);

#if WXGARBAGE_COLLECTION_ON
  wxWidgetHashTable = new wxNonlockingHashTable();
#else
  wxWidgetHashTable = new wxHashTable(wxKEY_INTEGER);
#endif
#endif
  Display *display = wxGetDisplay();

  /* No more wxMainColormap; remember default this way: */
  wxGetMainColormap(display);
  wxCommonInit();
  wxSetLanguage(wxTheApp->GetLanguage());

  wxMaxRequestSize = XMaxRequestSize(display);

  wxTheApp->argc = argc;
  wxTheApp->argv = argv;

  wxTheApp->wx_frame = wxTheApp->OnInit();

  // In XView, must ALWAYS have a main window.
  if (wxTheApp->wx_frame)
    wxTheApp->MainLoop();

  int retValue = wxTheApp->OnExit();
  wxCleanUp();
  return retValue;
}

// Cleans up any wxWindows internal structures left lying around
void wxCleanUp(void)
{
  wxCommonCleanUp();

#ifdef wx_xview
  delete wxFontPool;
#endif
#if USE_RESOURCES
  wxFlushResources();
#endif
}

wxApp::wxApp(wxlanguage_t language):wxbApp(language)
{
  wx_frame = NULL;
  wxTheApp = this;
  death_processed = FALSE;
  work_proc = NULL ;
}

wxApp::~wxApp(void)
{
}

Bool wxApp::Initialized(void)
{
  return (wx_frame != NULL);
}

// added by steve, 04.01.95
//
// a safer method for killing objects from methods of this
// objects:
// instead of calling 'delete this' in a method, you can
// use the 'wxPostDelete(this)' function. it inserts the
// object into a list, which is processed after the current event
// has been processed.
// 
// !!!! interestingly this doesn't work well for the MAIN FRAME !!!!
// therefore: DON'T DELETE THE MAIN FRAME WITH 'wxPostDelete' 
//
// this adds only a very small overhead to the event handling loop
//
// if you call 'delete this' from a menubar function, the
// widget is destroyed before the callback function is completed.
// under VMS this leads to a crash.
//
// static Bool somethingToDelete = FALSE;
// static wxList postDeleteList;

extern wxList wxPendingDelete;

// this function can be used by the application
void wxPostDelete(wxObject *object)
{
//  postDeleteList.Append(object);
//  somethingToDelete = TRUE;
  wxPendingDelete.Append(object);
}

// process the list and delete objects (used in main loop)
static void wxPostDeletion(void)
{
/*
  if(somethingToDelete){
    wxNode *node = postDeleteList.First();
    while(node!=NULL){
      wxObject *object = (wxObject *)node->Data();
      delete object;
      delete node;
      node = postDeleteList.First();
    }
    somethingToDelete = FALSE;
  }
*/
  if (wxTheApp)
    wxTheApp->DeletePendingObjects();
}

/*
 * Keep trying to process messages until WM_QUIT
 * received
 */

int wxApp::MainLoop(void)
{
#ifdef wx_motif
//  XtRealizeWidget(wxTheApp->topLevel);
  if (work_proc)
    XtAppAddWorkProc(wxTheApp->appContext,(XtWorkProc)work_proc,(XtPointer)this) ;
//  XtAppMainLoop(wxTheApp->appContext);

  // changed by steve, 29.10.94
  /* 
  * Sit around forever waiting to process X-events. Property Change
  * event are handled special, because they have to refer to
  * the root window rather than to a widget. therefore we can't
  * use an Xt-eventhandler.
  */
  XSelectInput(XtDisplay(wxTheApp->topLevel),
               XDefaultRootWindow(XtDisplay(wxTheApp->topLevel)),
               PropertyChangeMask);

  XEvent event;
  keep_going = TRUE;
  // Use this flag to allow breaking the loop via wxApp::ExitMainLoop()
  while (keep_going) {
    XtAppNextEvent(wxTheApp->appContext, &event);
    if(event.type == PropertyNotify){
      HandlePropertyChange(&event);
    }else
    {
      XtDispatchEvent(&event);
      // added by steve, 04.01.95, see above
      wxPostDeletion();
    }
  }
  return 0;
#elif defined(wx_xview)
  Frame frame = (Frame)(wx_frame->GetHandle());
  xv_main_loop(frame);
  return 0;
#endif
}

void wxApp::ExitMainLoop(void)
{
#ifdef wx_motif
  keep_going = FALSE;
#endif
#ifdef wx_xview
  notify_stop();
#endif
}

#ifdef wx_motif
// added by steve, 29.10.94
// this is a virtual function. it should be redefined
// in a derived class for handling property change events
// for XAtom IPC.
void wxApp::HandlePropertyChange(XEvent *event)                
{
  // by default do nothing special
  XtDispatchEvent(event); /* let motif do the work */
}
#endif

Bool wxApp::Pending(void)
{
#ifdef wx_motif
  XFlush(XtDisplay(wxTheApp->topLevel));
  return (XtAppPending(wxTheApp->appContext)) ;
#elif defined(wx_xview)
  return(FALSE) ; // Until I find what to do...
#endif
}

void wxApp::Dispatch(void)
{
#ifdef wx_motif
// The problem with XtAppNextEvent is that it automatically dispatches
// timers and alternate inputs and then stalls for an X event. If the
// only thing pending was alternate input, a wxYield() could stall.
// XtAppProcessEvent does what we want:
// (Matthew Flatt)

  XtAppProcessEvent(wxTheApp->appContext, XtIMAll);

// Old
//  XEvent event;
//  XtAppNextEvent(wxTheApp->appContext, &event);
//  XtDispatchEvent(&event);
#elif defined(wx_xview)
  return ; // Until I found what to do...
#endif
}

void wxExit(void)
{
  int retValue = 0;
  if (wxTheApp)
    retValue = wxTheApp->OnExit();
  wxCleanUp();

  exit(retValue);
}

// Yield to incoming messages
Bool wxYield(void)
{
  while (wxTheApp && wxTheApp->Pending())
    wxTheApp->Dispatch();
  return TRUE;
}

#ifdef wx_xview
int x_error_proc(Display *dpy, XErrorEvent *event)
{
  return XV_OK;
}
#endif

