/*
 * File:	mdi.h
 * Purpose:	MDI demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

#ifndef docviewh
#define docviewh

#include "wx_doc.h"

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void);
    wxFrame *OnInit(void);

    wxFrame *CreateChildFrame(wxDocument *doc, wxView *view, Bool isCanvas);
};

extern MyApp myApp;

// Define a new frame
class MyCanvas;
class MyFrame: public wxDocParentFrame
{
  DECLARE_CLASS(MyFrame)
 public:
  wxMenu *editMenu;
  
  // This pointer only needed if in single window mode
  MyCanvas *canvas;
  
  MyFrame(wxDocManager *manager, wxFrame *frame, char *title, int x, int y, int w, int h, long type);
  void OnMenuCommand(int id);
  MyCanvas *CreateCanvas(wxView *view, wxFrame *parent);
};

extern MyFrame *GetMainFrame(void);
extern Bool singleWindowMode;

#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   2

#endif
