/*
 * File:	form.h
 * Purpose:	Demo for wxWindows `forms' class
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

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
};

class MyObject: public wxObject
{
 public:
   char *string1;
   char *string2;
   char *string3;
   int int1;
   Bool bool1;
   float float1;
   wxList string_list1;
   MyObject(void);
};

class MyForm: public wxForm
{
 public:
  void EditForm(MyObject *object, wxPanel *panel);
  void OnOk(void);
  void OnCancel(void);
};

#define FORM_QUIT  1
#define FORM_EDIT  2
