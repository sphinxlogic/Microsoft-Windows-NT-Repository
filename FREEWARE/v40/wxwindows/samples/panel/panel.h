/*
 * File:	panel.h
 * Purpose:	Panel test
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};

class MyPanel: public wxPanel
{
  public:
    MyPanel(wxFrame *frame, int x, int y, int w, int h, long style);
    void OnPaint(void);
};

class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxWindow *parent, int x, int y, int w, int h, long style = wxBORDER):
      wxCanvas(parent, x, y, w, h, style)
    {
    }
    void OnPaint(void)
    {
      wxDC *dc = GetDC();
      dc->SetPen(wxBLACK_PEN);
      dc->SetBrush(wxGREEN_BRUSH);
      dc->DrawRoundedRectangle(20, 20, 100, 100);
    }
};

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
    Bool OnClose(void);
    void OnSize(int w, int h);
    void OnMenuCommand(int id);
};

// Callbacks
void button_proc(wxButton& but, wxEvent& event);
void quit_proc(wxButton& but, wxEvent& event);

#define PANELS_QUIT       100
#define PANELS_ABOUT      101
#define PANELS_LOAD_BITMAP 102

