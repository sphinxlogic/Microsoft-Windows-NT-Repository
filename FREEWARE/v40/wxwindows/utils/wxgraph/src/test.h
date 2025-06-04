/*
 * File:     test.h
 * Purpose:  Demo for graph library
 *
 */

// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};

class MyCanvas;

class MyFrame: public wxFrame
{
  public:
    MyCanvas *canvas;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    void Draw(wxDC& dc, Bool draw_bitmaps = TRUE);
};

// Define a new canvas which can receive some events
class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1, long style = wxRETAINED);
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
};

// Callbacks
void button_proc(wxButton& but, wxCommandEvent& event);
void list_proc(wxListBox& list, wxCommandEvent& event);
void GenericOk(wxButton& but, wxCommandEvent& event);

void GraphTest(wxGraphLayout& graph);

#define TEST_QUIT      1
#define TEST_ABOUT     2

