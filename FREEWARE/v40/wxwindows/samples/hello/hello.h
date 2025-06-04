/*
 * File:	hello.h
 * Purpose:	Demo for wxWindows class library
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
    MyFrame(wxFrame *frame, Const char *title, int x, int y, int w, int h);
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
    void OnScroll(wxCommandEvent& event);
};

// Callbacks
void button_proc(wxButton& but, wxCommandEvent& event);
void check_proc(wxCheckBox& but, wxCommandEvent& event);
void list_proc(wxListBox& list, wxCommandEvent& event);
void text_proc(wxText& but, wxCommandEvent& event);
void GenericOk(wxButton& but, wxCommandEvent& event);
void PopupFunction(wxMenu& menu, wxCommandEvent& event);

// Timer
class MyTimer: public wxTimer
{
 public:
  void Notify(void);
};

#define HELLO_QUIT       100
#define HELLO_PRINT      101
#define HELLO_TWIPS      103
#define HELLO_METRIC     104
#define HELLO_LOMETRIC   105
#define HELLO_NORMAL     106
#define HELLO_ZOOM       107
#define HELLO_ABOUT      108
#define HELLO_TIMER_ON   109
#define HELLO_TIMER_OFF  110
#define HELLO_LOAD_FILE  111
#define HELLO_SCALE      112
#define HELLO_PRINT_EPS  113
                              // Windows-only option
#define HELLO_COPY_MF    114
                              // Windows-only option
#define HELLO_SAVE_MF    115
                              // Windows-only option
#define HELLO_COPY_BITMAP 122
                              // Windows-only option

#define	HELLO_FILE1	116
#define	HELLO_FILE2	117
#define	HELLO_FILE3	118
#define	HELLO_FILE4	119
#define	HELLO_FILE5	120

#define HELLO_CHECKABLE 121
