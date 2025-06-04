/*
 * File:	layout.h
 * Purpose:	Layout demo for wxWindows class library
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

// Define a new frame
class MyTextWindow;
class MyCanvas;

class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;
    MyTextWindow *text_window;
    MyCanvas *canvas;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnSize(int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    void Draw(wxDC& dc, Bool draw_bitmaps = TRUE);
    void OnActivate(Bool) {}
};

// Define a new text subwindow that can respond to drag-and-drop
class MyTextWindow: public wxTextWindow
{
  public:
  MyTextWindow(wxFrame *frame, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=0):
    wxTextWindow(frame, x, y, width, height, style)
  { DragAcceptFiles(TRUE);
  }

  void OnDropFiles(int n, char *files[], int x, int y)
  { LoadFile(files[0]);
  }
  void OnChar(wxKeyEvent& event);
};

// Define a new canvas which can receive some events
class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style = wxRETAINED);
    ~MyCanvas(void) ;
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
};

class SizerFrame: public wxFrame
{
  public:
    wxPanel *panel;
    SizerFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnSize(int w, int h);
    Bool OnClose(void);
};

// Callbacks
void button_proc(wxButton& but, wxCommandEvent& event);
void list_proc(wxListBox& list, wxCommandEvent& event);
void GenericOk(wxButton& but, wxCommandEvent& event);

#define LAYOUT_QUIT       100
#define LAYOUT_TEST       101
#define LAYOUT_ABOUT      102
#define LAYOUT_LOAD_FILE  103
