/*
 * File:	test.h
 * Purpose:	wxPropertySheet class test
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifdef __GNUG__
#pragma interface
#endif

#ifndef testh
#define testh

#include "wx_plist.h"
#include "wx_pform.h"

class MyChild;

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void);
    wxFrame *OnInit(void);
};

extern MyApp myApp;

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long type);
    Bool OnClose(void);
    void OnMenuCommand(int id);
};

extern wxFrame *GetMainFrame(void);

#define PROPERTY_QUIT                   1
#define PROPERTY_ABOUT                  2
#define PROPERTY_TEST_DIALOG_LIST       3
#define PROPERTY_TEST_FRAME_LIST        4
#define PROPERTY_TEST_DIALOG_FORM       5
#define PROPERTY_TEST_FRAME_FORM        6

void RegisterValidators(void);
void PropertyListTest(Bool useDialog);
void PropertyFormTest(Bool useDialog);

#endif
