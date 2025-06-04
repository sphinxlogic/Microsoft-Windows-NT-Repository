/*
 * File:	wb_main.cc
 * Purpose:	wxApp implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_main.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
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
#include "wx_defs.h"
#include "wx_panel.h"
#include "wx_frame.h"
#include "wx_main.h"
#include "wx_utils.h"
#include "wx_gdi.h"
#include "wx_dc.h"
#include "wx_dialg.h"
#include "wx_types.h"
#endif

#include "wx_sysev.h"
#include "wx_dcps.h"
#include "wx_res.h"

#include <string.h>

extern char *wxBuffer;
extern char *wxOsVersion;

wxApp *wxTheApp = NULL;

wxbApp::wxbApp(wxlanguage_t lang)
{
  WXSET_TYPE(wxApp, wxTYPE_APP)
  
  wx_class = NULL;
  wantDebugOutput = TRUE ;
  appName = NULL;
  argc = 0;
  argv = NULL;
  death_processed = FALSE;
#ifdef wx_msw
  printMode = wxPRINT_WINDOWS;
#else
  printMode = wxPRINT_POSTSCRIPT;
#endif
  work_proc = NULL;
  wx_frame = NULL;
  exitOnFrameDelete = TRUE;
  showOnInit = TRUE;
  wxLang = lang;
}

wxbApp::~wxbApp(void)
{
  if (appName)
    delete[] appName;
  if (wx_class)
    delete[] wx_class;
}

Bool wxbApp::Initialized(void)
{
  return FALSE;
}

wxFrame *wxbApp::OnInit(void)
{
  return NULL;
}

int wxbApp::OnExit(void)
{
  return 0;
}

char *wxbApp::GetAppName(void)
{
  if (appName)
    return appName;
  else if (wx_class)
    return wx_class;
  else return NULL;
}

void wxbApp::SetAppName(char *name)
{
  if (name == appName)
    return;
  if (appName)
    delete[] appName;
  if (name)
    appName = copystring(name);
}

char *wxbApp::GetClassName(void)
{
  return wx_class;
}

void wxbApp::SetClassName(char *name)
{
  if (name == wx_class)
    return;
  if (wx_class)
    delete[] wx_class;
  if (name)
    wx_class = copystring(name);
}

wxWindow *wxbApp::GetTopWindow(void)
{
  return wx_frame;
}

extern wxList wxPendingDelete;

void wxbApp::DeletePendingObjects(void)
{
  wxNode *node = wxPendingDelete.First();
  while (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    
    delete obj;

    if (wxPendingDelete.Member(obj))
      delete node;

    // Deleting one object may have deleted other pending
    // objects, so start from beginning of list again.
    node = wxPendingDelete.First();
  }
}

void wxCommonInit(void)
{
#ifdef wx_msw
  wxBuffer = new char[1500];
#else
  wxBuffer = new char[BUFSIZ + 512];
#endif
#if USE_DYNAMIC_CLASSES
  wxClassInfo::InitializeClasses();
#endif

#ifdef wx_x
  wxTheFontNameDirectory.Initialize();
#endif

#if defined(wx_x) && USE_RESOURCES
  // Read standard font names from .Xdefaults

  extern char *wxDecorativeFontName;
  extern char *wxRomanFontName;
  extern char *wxModernFontName;
  extern char *wxSwissFontName;
  extern char *wxScriptFontName;
  extern char *wxTeletypeFontName;
  extern char *wxDefaultFontName;

  (void) wxGetResource("wxWindows", "defaultFamily", &wxDefaultFontName);
  (void) wxGetResource("wxWindows", "decorativeFamily", &wxDecorativeFontName);
  (void) wxGetResource("wxWindows", "romanFamily", &wxRomanFontName);
  (void) wxGetResource("wxWindows", "modernFamily", &wxModernFontName);
  (void) wxGetResource("wxWindows", "swissFamily", &wxSwissFontName);
  (void) wxGetResource("wxWindows", "scriptFamily", &wxScriptFontName);
  (void) wxGetResource("wxWindows", "teletypeFamily", &wxTeletypeFontName);
#endif

#if USE_RESOURCES
  (void) wxGetResource("wxWindows", "OsVersion", &wxOsVersion);
#endif

  wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
  wxTheColourDatabase->Initialize();
  wxInitializeStockObjects();

#if USE_OLD_TYPE_SYSTEM
  wxInitStandardTypes();
#endif

  wxInitStandardEvents();

  // For PostScript printing
#if USE_POSTSCRIPT
  wxInitializePrintSetupData();
  wxThePrintPaperDatabase = new wxPrintPaperDatabase;
  wxThePrintPaperDatabase->CreateDatabase();
#endif

}

void wxCommonCleanUp(void)
{
#if USE_WX_RESOURCES
  extern void wxDeleteResourceBuffer(void);

  wxDefaultResourceTable.ClearTable();
  wxDeleteResourceBuffer();
#endif

  wxDeleteStockObjects() ;
  // Destroy all GDI lists, etc.
  delete wxTheBrushList;
  delete wxThePenList;
  delete wxTheFontList;
  delete wxTheBitmapList;
  delete wxTheColourDatabase;
#if USE_POSTSCRIPT
  wxInitializePrintSetupData(FALSE);
  delete wxThePrintPaperDatabase;
  wxThePrintPaperDatabase = NULL;
#endif

#ifdef wx_x
  wxTheFontNameDirectory.CleanUp();
#endif

  wxDeleteEventLists() ;

  delete[] wxBuffer;
}

