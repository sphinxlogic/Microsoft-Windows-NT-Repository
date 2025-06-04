/**************************************************************************
 * wxPoem: wxpoem.h                                                       *
 *                                                                        *
 * A small C++ program which displays a random poem on execution.         *
 * It also allows search for poems containing a string.                   *
 * It requires winpoem.dat and creates winpoem.idx.                       *
 *                                                                        *
 * Converted from WinPoem source.                                         *
 *                                                                        *
 * Copyright 1994 Julian Smart, email J.Smart@ed.ac.uk                    *
 *                                                                        *
 * Filename: wxpoem.cc                                                    *
 * Purpose:  Main source file for wxPoem.                                 *
 * Version:  1.0                                                          *
 *************************************************************************/


// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};


// Define a new canvas which can receive some events
class MyCanvas: public wxCanvas
{
  public:
    MyCanvas(wxFrame *frame, int x = -1, int y = -1, int w = -1, int h = -1);
    void OnPaint(void);
    void OnEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);
};

// Define a new frame
class MainWindow: public wxFrame
{
  public:
    MyCanvas *canvas;
    MainWindow(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
    Bool OnClose(void);
    void OnMenuCommand(int id);

    // Display next page or poem
    void NextPage(void);

    // Display previous page
    void PreviousPage(void);

    // User search
    void Search(Bool);

    // Look in file for string
    long DoSearch(void);

    // Do the actual drawing of text (or just calculate size needed)
    void ScanBuffer(wxDC *dc, Bool DrawIt, int *max_x, int *max_y);

    // Load the poem
    void GetIndexLoadPoem(void);
    void Resize(void);
};

// Menu items
#define         POEM_NEXT       100
#define         POEM_PREVIOUS   101
#define         POEM_COPY       102
#define         POEM_SEARCH     103
#define         POEM_NEXT_MATCH 104
#define         POEM_ABOUT      105
#define         POEM_EXIT       106
#define         POEM_COMPILE    107
#define         POEM_HELP_CONTENTS 108
#define         POEM_BIGGER_TEXT 109
#define         POEM_SMALLER_TEXT 110
#define         POEM_MINIMIZE   111


