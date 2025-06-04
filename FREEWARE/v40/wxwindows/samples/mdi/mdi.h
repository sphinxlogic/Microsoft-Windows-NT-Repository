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

// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};

class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x, int y, int w, int h);
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
};

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
    Bool OnClose(void);
    void OnMenuCommand(int id);
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

void GenericOk(wxButton& but, wxCommandEvent& event);

#define MDI_QUIT        1
#define MDI_NEW_WINDOW  2
#define MDI_REFRESH     3
#define MDI_CHILD_QUIT  4
#define MDI_ABOUT       5
