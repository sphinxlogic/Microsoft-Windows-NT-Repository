/*
 * File:	printing.h
 * Purpose:	Printing demo for wxWindows class library
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void) ;
    wxFrame *OnInit(void);
};

// Define a new canvas and frame
class MyCanvas;
class MyFrame: public wxFrame
{
  public:
    MyCanvas *canvas;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnSize(int w, int h);
    void OnMenuCommand(int id);
    void Draw(wxDC& dc);
    void OnActivate(Bool) {}
};

// Define a new canvas which can receive some events
class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style = wxRETAINED);
    ~MyCanvas(void) ;
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
    void OnScroll(wxCommandEvent& event);
};

class MyPrintout: public wxPrintout
{
 public:
  MyPrintout(char *title = "My printout"):wxPrintout(title) {}
  Bool OnPrintPage(int page);
  Bool HasPage(int page);
  Bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

  void DrawPageOne(wxDC *dc);
  void DrawPageTwo(wxDC *dc);
};

void GenericOk(wxButton& but, wxCommandEvent& event);

#define WXPRINT_QUIT            100
#define WXPRINT_PRINT           101
#define WXPRINT_PRINT_SETUP     102
#define WXPRINT_PREVIEW         103

#define WXPRINT_PRINT_PS        104
#define WXPRINT_PRINT_SETUP_PS  105
#define WXPRINT_PREVIEW_PS      106

#define WXPRINT_ABOUT           107

