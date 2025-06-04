/*
 * File:	test.h
 * Purpose:	Demo for tree library
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

class MyFrame: public wxFrame
{
  public:
    ShapeCanvas *canvas;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
   // void OnSize(int w, int h);
    void OnMenuCommand(int id);
    void Draw(wxDC& dc, Bool draw_bitmaps = TRUE);
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

class ChartShape: public RectangleShape, public wxStoredBarLineChart
{ 
	public: 
	ChartShape(float wi, float hi);          
  void OnDraw();                         
	void OnRightClick(float x, float y, int keys);
}; 

class MyForm: public wxForm
{          
	ChartShape *myObject;
	char *orient;
  char *inoutnone;
  Bool td;
  wxList string_list1;
 public:
  //void EditForm(MyObject *object, wxPanel *panel);
	void EditForm(ChartShape *object, wxPanel *panel);  
  void OnOk(void);
  void OnCancel(void);          
  void OnUpdate(void);
};

#define OPTION1 1                      


void GraphTest(wxStoredBarLineChart *chart);

#define TEST_QUIT      1
#define TEST_ABOUT     2
#define TEST_HORIZONTAL 3
#define TEST_VERTICAL 4
#define TEST_ZOOM1 5
#define TEST_ZOOM2 6
#define TEST_ZOOM3 7    
#define TEST_ABOUTwx     8
#define DATA1 9
#define DATA2 10
#define DATA3 11
#define DATA4 12
#define DATA5 13
#define DATA6 14
#define DATA7 15
#define DATA8 16
#define DATA9 17
#define DATA10 18





