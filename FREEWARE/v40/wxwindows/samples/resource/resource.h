/*
 * File:	resource.h
 * Purpose:	Demo for wxWindows resources
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

class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;
    MyFrame(wxFrame *frame, Const char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    void OnActivate(Bool) {}
};

class MyDialogBox : public wxDialogBox
{
  public:
   void OnCommand(wxWindow& win, wxCommandEvent& event);
};

#define RESOURCE_QUIT       4
#define RESOURCE_TEST1      2
