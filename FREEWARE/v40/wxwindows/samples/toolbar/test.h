/*
 * File:	test.h
 * Purpose:	Demo for toolbar class library
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

// Define a frame which can handle resizing with a toolbar plus one subwindow
class wxFrameWithToolBar: public wxFrame
{
  public:
    wxToolBar *frameToolBar;
    wxFrameWithToolBar(wxFrame *frame, char *title, int x, int y, int w, int h,
            long style = wxSDI | wxDEFAULT_FRAME);
    void OnSize(int w, int h);  // Default OnSize handler
    inline void SetToolBar(wxToolBar *tb) { frameToolBar = tb; }
    inline wxToolBar *GetFrameToolBar(void) { return frameToolBar; }
};

// Define a new frame
class MyFrame: public wxFrameWithToolBar
{
  public:
    wxTextWindow *textWindow;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    void OnMenuSelect(int id);
};

class TestToolBar: public wxToolBar
{
  public:
  TestToolBar(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  Bool OnLeftClick(int toolIndex, Bool toggled);
  void OnMouseEnter(int toolIndex);
};

class TestRibbon: public wxToolBar
{
  public:
  TestRibbon(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  Bool OnLeftClick(int toolIndex, Bool toggled);
  void OnMouseEnter(int toolIndex);
};

#define TEST_QUIT       100
#define TEST_ABOUT      101
