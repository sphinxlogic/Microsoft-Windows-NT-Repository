/*
 * File:	dialoged.cc
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

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

#include "dialoged.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp myApp;

MyApp::MyApp(void)
{
}

wxResourceManager theResourceManager;

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *MyApp::OnInit(void)
{
  theResourceManager.Initialize();
  theResourceManager.ShowResourceEditor(TRUE);
  
  if (argc > 1)
    theResourceManager.Load(argv[1]);

  return theResourceManager.GetEditorFrame();
}

