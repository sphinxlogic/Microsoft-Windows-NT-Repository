/*
 * File:	view.h
 * Purpose:	View and template classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

#ifndef viewh
#define viewh

#include "wx_doc.h"

class MyCanvas: public wxCanvas
{
  public:
    wxView *view;
    
    MyCanvas(wxView *v, wxFrame *frame, int x, int y, int w, int h, long style);
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
};

class MyTextWindow: public wxTextWindow
{
  public:
    wxView *view;
    
    MyTextWindow(wxView *v, wxFrame *frame, int x, int y, int w, int h, long style);
};

class DrawingView: public wxView
{
  DECLARE_DYNAMIC_CLASS(DrawingView)
 private:
 public:
  wxFrame *frame;
  MyCanvas *canvas;
  
  DrawingView(void) { canvas = NULL; frame = NULL; };
  ~DrawingView(void) {};

  Bool OnCreate(wxDocument *doc, long flags);
  void OnDraw(wxDC *dc);
  void OnUpdate(wxView *sender, wxObject *hint = NULL);
  Bool OnClose(Bool deleteWindow = TRUE);
  void OnMenuCommand(int cmd);
};

class TextEditView: public wxView
{
  DECLARE_DYNAMIC_CLASS(TextEditView)
 private:
 public:
  wxFrame *frame;
  MyTextWindow *textsw;
  
  TextEditView(wxDocument *doc = NULL): wxView(doc) { frame = NULL; textsw = NULL; }
  ~TextEditView(void) {}

  Bool OnCreate(wxDocument *doc, long flags);
  void OnDraw(wxDC *dc);
  void OnUpdate(wxView *sender, wxObject *hint = NULL);
  Bool OnClose(Bool deleteWindow = TRUE);
};

#endif
