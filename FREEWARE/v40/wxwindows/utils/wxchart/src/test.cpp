/*
 * File:	test.cc
 * Purpose:	Chart demo
 * Author:	Neil Dudman
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include "wx_form.h"  
#include "shapes.h"
#include "lbchart.h"
// #include "colour.cpp"
#include "shapes.h" 
#include "string.h"
   
#include "test.h"  

MyForm *my_form = NULL;
wxFrame *my_frame = NULL;
wxPanel *my_panel = NULL;
ShapeCanvas *canvas=NULL;
//Shape *recshape1 = NULL;
ChartShape *recshape1 = NULL;

// Declare two frames
MyFrame   *frame = NULL;
wxMenuBar *menu_bar = NULL;
Bool      timer_on = FALSE;
wxBitmap  *test_bitmap = NULL;
wxIcon    *test_icon = NULL;


ChartShape::ChartShape(float wi, float hi): 
					RectangleShape(wi,hi), wxStoredBarLineChart()  												 
{ 
	GraphTest(this);
}                                        

void ChartShape::OnDraw()
{ 
	wxStoredBarLineChart::SetDC(dc);
	RectangleShape::OnDraw();		
	SetMaxWidthHeight(width,height);
	SetChartCenter(xpos,ypos);   	
 	wxStoredBarLineChart::Draw();
}               
 
void ChartShape::OnRightClick(float x, float y, int keys) 
{	
	wxMenu *menu = new wxMenu;
  menu->Append(OPTION1, "&option1","Redraw the Chart Horizontal");
  menu->Append(OPTION1, "&option1","Redraw the Chart Vertical");
  menu->AppendSeparator();
  menu->Append(OPTION1, "&Quit",                "Quit program");
	/*	#ifdef wx_motif wanted to used a popup menu but didn't work ?? :-()
			FakePopupMenu(menu, x, y);
		#else    
		//	wxPopupMenu(menu, x, y);   
	    wxPopupMenu * menu2 = new wxPopupMenu(NULL, NULL,
           x, y, -1, -1, "MyPopupTest","ITEM", NULL,
           OnExit, NULL, 20);

		#endif*/
	if (!my_form)
  {
    my_frame = new wxFrame(frame, "Form",50,50,300,400);
    my_form = new MyForm;             
    my_panel = new wxPanel(my_frame);
    my_form->EditForm(this, my_panel);
    my_frame->Show(TRUE);
  }
  else my_frame->Show(TRUE);     
}

Bool MyConstraint(int type, char *value, char *label, char *msg_buffer)
{
  if (value && (strlen(value) > 7))
  {
    sprintf(msg_buffer, "Value for %s should be 7 characters or less",
            label);
    return FALSE;
  }
  else return TRUE;
}

void MyForm::EditForm(ChartShape *object, wxPanel *panel)
{ 
	myObject = object;
	int a = object->GetChartOrientation();
	if(a==wxVERTICAL)
		orient="Vertical";
	else               
		orient="Horizontal";
		
  td = object->Get3D();
  object->trailingText = new char[50];	  
  
	object->leadingText = new char[50]; 
	
	int in = object->GetTickInOutNone();
	if(in == wxTickIn)
			inoutnone = "In";
	else if(in == wxTickOut)
		inoutnone = "Out";		
	else
		inoutnone = "None";
							
	Add(wxMakeFormString("Title", &(object->chartTitle), wxFORM_DEFAULT,NULL, NULL,
											wxVERTICAL, 220, -1));	                                                                                               
	Add(wxMakeFormBool("3 Dimensional", &(td)));
	Add(wxMakeFormNewLine());
	
	Add(wxMakeFormShort("edgeLeft", &(object->edgeLeft), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL,
                       wxVERTICAL, 30, -1));
  Add(wxMakeFormShort("edgeRight", &(object->edgeRight), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL,
                       wxVERTICAL, 30, -1));
	Add(wxMakeFormShort("edgeTop", &(object->edgeTop), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL,
                       wxVERTICAL, 30, -1));
  Add(wxMakeFormShort("edgeBottom", &(object->edgeBottom), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL, 
                       wxVERTICAL, 30, -1));   
                       
  Add(wxMakeFormNewLine());
	Add(wxMakeFormBool("Display Values", &(object->showValues)));
  Add(wxMakeFormString("Orientataion", &(orient), wxFORM_CHOICE,
                       new wxList(wxMakeConstraintStrings("Vertical", "Horizontal",
                       0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormNewLine());          
  Add(wxMakeFormString("TickMarks", &(inoutnone), wxFORM_CHOICE,
                       new wxList(wxMakeConstraintStrings("In", "Out", "None",
                       0), 0), NULL, wxVERTICAL));
  Add(wxMakeFormShort("Column Spacing", &(object->colSpace), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL, 
                       wxVERTICAL, 30, -1));   
  Add(wxMakeFormShort("Column Spacing", &(object->colSpace), wxFORM_DEFAULT,
                       new wxList(wxMakeConstraintFunction(MyConstraint), 0), NULL, 
                       wxVERTICAL, 30, -1));   
	Add(wxMakeFormNewLine());                                 
	Add(wxMakeFormString("Leading Text", &(object->leadingText), wxFORM_DEFAULT,
                        NULL, NULL, wxVERTICAL, 30, -1));   
  Add(wxMakeFormString("Trailing Text", &(object->trailingText), wxFORM_DEFAULT,
                       NULL, NULL, wxVERTICAL, 30, -1));   
    
  Add(wxMakeFormNewLine());

  
  Add(wxMakeFormNewLine());

  AssociatePanel(panel);
  
  panel->Fit();
}

void MyForm::OnOk(void)
{
  my_frame->Show(FALSE);

  //delete my_form;
  //delete my_frame;
	OnUpdate();
  my_form = NULL;
  my_frame = NULL;
  //frame->canvas->OnPaint();
}
void MyForm::OnUpdate(void)
{                                                
	if(strcmp(orient,"Vertical")==0)
		 myObject->SetChartOrientation(wxVERTICAL);
	else                                         
		myObject->SetChartOrientation(wxHORIZONTAL);
		
	if(td)
		myObject->Set3D(TRUE);
	else
		myObject->Set3D(FALSE);	

               
  if(strcmp(inoutnone,"In")==0)
			myObject->SetTickInOutNone(wxTickIn);
	else if(strcmp(inoutnone,"Out")==0)
		myObject->SetTickInOutNone(wxTickOut);
	else
		myObject->SetTickInOutNone(wxTickNone);
		
	myObject->OnDraw();
}

void MyForm::OnCancel(void)
{
  my_frame->Show(FALSE);

  //delete my_form;
  //delete my_frame;

  my_form = NULL;
  my_frame = NULL;
}

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
MyApp     myApp;

// For drawing lines in a canvas
float     xpos = -1;
float     ypos = -1;

// Must initialise these in OnInit, not statically
wxPen     *red_pen;
wxFont    *graphFont;

float     zoom_factor = 1.0;

#ifdef wx_x
#include "aiai.xbm"
#endif
// The `main program' equivalent, creating the windows and returning the
// main frame

wxFrame *MyApp::OnInit(void)
{
  // Create a red pen
  red_pen = new wxPen("RED", 3, wxSOLID);

  // Create a font
  graphFont = new wxFont(12, wxSWISS, wxNORMAL, wxNORMAL);

  // Create the main frame window
  frame = new MyFrame(NULL, "chart Test", 0, 0, 400, 550);

  // Give it a status line
  frame->CreateStatusLine(2);

  // Give it an icon
#ifdef wx_msw
  test_icon = new wxIcon("aiai_icn");
#endif
#ifdef wx_x
  test_icon = new wxIcon(aiai_bits, aiai_width, aiai_height);
#endif

  frame->SetIcon(test_icon);

 	// Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_HORIZONTAL, "&Horizontal","Redraw the Chart Horizontal");
  file_menu->Append(TEST_VERTICAL, "&Vertical","Redraw the Chart Vertical");
  file_menu->AppendSeparator();
  file_menu->Append(TEST_QUIT, "&Quit",                "Quit program");

	wxMenu *zoom_menu = new wxMenu;
	zoom_menu->Append(TEST_ZOOM1, "&Zoom out", 				"Zooms out");
	zoom_menu->Append(TEST_ZOOM2, "&Zoom in", 				"Zooms in");
	zoom_menu->Append(TEST_ZOOM3, "&Zoom Normal", 				"Zooms back to normal");

	wxMenu *datasets = new wxMenu;
	datasets->Append(DATA1,"&Bar1",			"Display/hide first bar",TRUE);
	datasets->Append(DATA2,"&Bar2",			"Display/hide second bar",TRUE);
	datasets->Append(DATA3,"&Bar3",			"Display/hide third bar",TRUE);
	datasets->Append(DATA4,"&XYPlot",			"Display/hide XYPlot",TRUE);
	datasets->Append(DATA5,"&Percent Bar",			"Display/hide Percent Bar",TRUE);
	datasets->Append(DATA6,"&Pie",			"Display/hide Pie Chart",TRUE);
	datasets->Append(DATA7,"&Line",			"Display/hide Line Chart",TRUE);
	datasets->Append(DATA8,"&Curve",			"Display/hide Curve Chart",TRUE);
	datasets->Append(DATA9,"&Area",			"Display/hide Area Chart",TRUE);
	datasets->Append(DATA10,"&Floating Bar",			"Display/hide Floating Bar Chart",TRUE);
	datasets->Check(DATA1,TRUE);
	datasets->Check(DATA2,TRUE);
	datasets->Check(DATA3,TRUE);
	datasets->Check(DATA4,TRUE);
	datasets->Check(DATA5,TRUE);
	datasets->Check(DATA6,TRUE);
	datasets->Check(DATA7,TRUE);
	datasets->Check(DATA8,TRUE);
	datasets->Check(DATA9,TRUE);
	datasets->Check(DATA10,TRUE);
	
  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, "&About",              "About Chart Test");
  help_menu->Append(TEST_ABOUTwx, "&About wxwin",              "About wxWin");

  menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(zoom_menu, "&Zoom"); 
  menu_bar->Append(datasets, "&Display Data");
  menu_bar->Append(help_menu, "&Help");  

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  //MyCanvas *canvas = new MyCanvas(frame,-1.-1.10,5);
  canvas = new ShapeCanvas(frame);
	recshape1 = new ChartShape(400.0,400.0); 
	recshape1->xpos = 200;
	recshape1->ypos = 200;
	canvas->AddObject(recshape1);
	
	canvas->ShowAll(TRUE);
	canvas->ShowGrid(TRUE);
	canvas->SetGridSpacing(10);
	
  int wn, hn;
  canvas->GetClientSize(&wn,&hn);
  
  canvas->GetDC()->SetPen(wxBLACK_PEN);
  canvas->GetDC()->SetFont(graphFont);

  wxCursor *cursor = new wxCursor(wxCURSOR_ARROW);
  canvas->SetCursor(cursor);
	
  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
  canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

  frame->canvas = canvas;
/*
  myChart = new wxStoredBarLineChart(canvas,20); //canvas->GetDC(),20);
  GraphTest(*myChart);
  myChart->Draw();*/
  
  frame->Show(TRUE);

  frame->SetStatusText("Hello, chart!");
  
  /*wxColourFrame *colorFrame; 
  colorFrame=new wxColourFrame (frame, 10, 400);
	colorFrame->Show(TRUE);*/
  
  // Return the main frame window
  return frame;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
}
             
// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  wxDC *dc = NULL;
  float xx,yy;
  if (canvas)
    dc = canvas->GetDC();
  switch (id)
  {
    case TEST_QUIT:
    {
      OnClose();
      if (canvas)
        delete canvas ;
      delete this;
      break;
    }
    case TEST_VERTICAL:
    {
      if (canvas)
      {                 
				recshape1->SetChartOrientation(wxVERTICAL);      
        recshape1->OnDraw();
      }
      break;
    }  
    case TEST_HORIZONTAL:
    {
      if (recshape1)
      {
        recshape1->SetChartOrientation(wxHORIZONTAL);
        recshape1->OnDraw();
      }
      break;
    }
    case TEST_ZOOM1:       
    {
    	if(recshape1)
    	{	xx = 0.5; 
    		yy = 0.5;
    		dc->SetUserScale(xx,yy);
    		canvas->Redraw();
    	}                            
    	break;
    }         
    case TEST_ZOOM2:       
    { if(recshape1)
    	{
    		xx = 1.5; 
    		yy = 1.5;
    		dc->SetUserScale(xx,yy);
    		canvas->Redraw();
    	}
    	break;
    } 
    case TEST_ZOOM3:       
    { if(recshape1)
    	{
    		xx = 1; 
    		yy = 1;
    		dc->SetUserScale(xx,yy);
    		canvas->Redraw();
    	}
    	break;
    } 
    case TEST_ABOUT:
    {
      (void)wxMessageBox("wxWindows Chart library demo Vsn 1.1\nAuthor: Neil Dudman N.Dudman@herts.ac.uk\n", "About Chart Test");
      break;
    }                           
    case TEST_ABOUTwx:
    {
    	(void)wxMessageBox(
				"wxWindows Portable C/C++ GUI Class Library\n"
				"by Julian Smart\nJ.Smart@ed.ac.uk\n"
				"http://www.aiai.ed.ac.uk/~jacs/html/wxwin.html\nAIAI (c) 1995",
				"About wxWindows");
			break;              
		}
		case DATA1:
		{
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,0); // first bar
			else	                   
				recshape1->ShowDataSet(FALSE,0); // first bar						
			recshape1->OnDraw();
			break;  
		}
		case DATA2:                                               
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,1); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,1); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA3:                     
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,2); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,2); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA4:                  
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,3); // first bar			
			else	                   
					recshape1->ShowDataSet(FALSE,3); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA5:                                               
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,4); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,4); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA6:   
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,5); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,5); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA7:                    
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,6); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,6); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA8:                    
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,7); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,7); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA9:                    
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,8); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,8); // first bar						
			recshape1->OnDraw();
			break;  
		case DATA10:                    
			if(menu_bar->Checked(id))
				recshape1->ShowDataSet(TRUE,9); // first bar			
			else	                   
				recshape1->ShowDataSet(FALSE,9); // first bar						
			recshape1->OnDraw();
			break;  
    }
} 

void MyFrame::Draw(wxDC& dc, Bool draw_bitmaps)
{
  if (canvas)   ;
    //canvas->OnDraw();
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
Bool MyFrame::OnClose(void)
{
  return TRUE;
}

void quit_proc(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *box = (wxDialogBox *)but.GetParent();
  box->Show(FALSE);
  delete box;
}

// Gets some user input, and sets the status line
void button_proc(wxButton& but, wxCommandEvent& event)
{
  int choice = wxMessageBox("Press OK to continue", "Try me", wxOK | wxCANCEL);

  if (choice == wxOK)
  {
    char *text = wxGetTextFromUser("Enter some text", "Text input", "");
    if (text)
    {
      (void)wxMessageBox(text, "Result", wxOK);

      frame->SetStatusText(text);
    }
  }
}

void GenericOk(wxButton& but, wxCommandEvent& event)
{
  wxDialogBox *dialog = (wxDialogBox *)but.GetParent();

  dialog->Show(FALSE);
}

void GraphTest(wxStoredBarLineChart *chart)
{ 
	// Set the number of colums to display            
  chart->SetStartEndCol(0,11);       
  chart->SetColumnGap(10);
    
  // Set chart Orientation , defaults to wxVERTICAL
	chart->SetChartOrientation(wxVERTICAL);
  chart->Set3D(FALSE);
  chart->ShowValues(FALSE);
	// Set title font and text	
	wxFont *titleFont = wxTheFontList->FindOrCreateFont(15,wxDECORATIVE,wxNORMAL,wxBOLD);
	chart->SetTitleFont(titleFont);
	chart->SetTitle("A Title Demo CHART");            
	
	// Setup the Bar/line/column Lables
  chart->SetLabel(0,"Tue");
  chart->SetLabel(1,"Wed");
  chart->SetLabel(2,"Thur");
  chart->SetLabel(3,"Fri");
  chart->SetLabel(4,"Sat");
  chart->SetLabel(5,"Sun");
  chart->SetLabel(6,"Now");
  chart->SetLabel(7,"And");
  chart->SetLabel(8,"Then");
  chart->SetLabel(9,"It");
  chart->SetLabel(10,"Rains");
  chart->SetLabel(11,"Well");

  // Add a new dataset to chart, display type curve	
  chart->AddDataSet(wxBar,0);// was 0
  wxColour *colour = wxTheColourDatabase->FindColour("GREEN");
  wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colour,wxSOLID);
  chart->SetDataStyle(0,brush);
  // define data  
  chart->SetData(0,0,4);
  chart->SetData(1,0,6);
  chart->SetData(2,0,8);   
  chart->SetData(3,0,9);   
  chart->SetData(4,0,2);   
  chart->SetData(5,0,5);   
  chart->SetData(6,0,9);   
  chart->SetData(7,0,3);   
  chart->SetData(8,0,2);   
  chart->SetData(9,0,12);   
  chart->SetData(10,0,17); 
  chart->SetData(11,0,13); 
  
	// Add a new dataset to chart, display type curve
  chart->AddDataSet(wxBar,7);
	// define data     
  chart->SetData(0,7,4);
  chart->SetData(1,7,1);
  chart->SetData(2,7,20);   
  chart->SetData(3,7,3);   
  chart->SetData(4,7,6);   
  chart->SetData(5,7,4);   
  chart->SetData(6,7,5);   
  chart->SetData(7,7,3);   
  chart->SetData(8,7,9);   
  chart->SetData(9,7,7);   
  chart->SetData(10,7,10); 
  chart->SetData(11,7,13);  
                 
  chart->AddDataSet(wxBar,2);
  colour = wxTheColourDatabase->FindColour("BLUE");
  wxPen *pen = wxThePenList->FindOrCreatePen(colour,2,wxSOLID);
  chart->SetLineStyle(2,pen);
	// define data    
  chart->SetData(0,2,2);
  chart->SetData(1,2,4);
  chart->SetData(2,2,12);   
  chart->SetData(3,2,6);   
  chart->SetData(4,2,5);   
  chart->SetData(5,2,7);   
  chart->SetData(6,2,8);   
  chart->SetData(7,2,5);   
  chart->SetData(8,2,2);   
  chart->SetData(9,2,1);   
  chart->SetData(10,2,4); 
  chart->SetData(11,2,6); 
          
  
  // Add a new dataset to chart, display type curve
  chart->AddDataSet(wxXYPlot,1);
	// define data     
  chart->SetData(0,1,6);
  chart->SetData(1,1,1);
  chart->SetData(2,1,20);   
  chart->SetData(3,1,3);   
  chart->SetData(4,1,6);   
  chart->SetData(5,1,4);   
  chart->SetData(6,1,5);   
  chart->SetData(7,1,3);   
  chart->SetData(8,1,9);   
  chart->SetData(9,1,7);   
  chart->SetData(10,1,10); 
  chart->SetData(11,1,13);  

	// Add a new dataset to chart, display type percent bar
	wxList *list	= new wxList;
	int c=3,cc=4, ca = 5, cb = 6;	
	list->Append((wxObject *)c);
	list->Append((wxObject *)cc);
	list->Append((wxObject *)ca);
	list->Append((wxObject *)cb);
  chart->AddDataSet(wxPercentBar,list);
  colour = wxTheColourDatabase->FindColour("LIME");
  pen = wxThePenList->FindOrCreatePen(colour,2,wxSOLID);
  chart->SetLineStyle(2,pen);
	// define data start value column    
  chart->SetData(0,3,2);
  chart->SetData(1,3,4);
  chart->SetData(2,3,6);   
  chart->SetData(3,3,7);   
  chart->SetData(4,3,8);   
  chart->SetData(5,3,9);   
  chart->SetData(6,3,10);   
  chart->SetData(7,3,4);   
  chart->SetData(8,3,5);   
  chart->SetData(9,3,6);   
  chart->SetData(10,3,7); 
  chart->SetData(11,3,8); 
  // define data next value column
  chart->SetData(0,4,4);
  chart->SetData(1,4,5);
  chart->SetData(2,4,8);   
  chart->SetData(3,4,9);   
  chart->SetData(4,4,10);   
  chart->SetData(5,4,11);   
  chart->SetData(6,4,15);   
  chart->SetData(7,4,9);   
  chart->SetData(8,4,10);   
  chart->SetData(9,4,15);   
  chart->SetData(10,4,10); 
  chart->SetData(11,4,9); 
// define data next value column
  chart->SetData(0,5,7);
  chart->SetData(1,5,4);
  chart->SetData(2,5,9);   
  chart->SetData(3,5,2);   
  chart->SetData(4,5,12);   
  chart->SetData(5,5,1);   
  chart->SetData(6,5,3);   
  chart->SetData(7,5,21);   
  chart->SetData(8,5,4);   
  chart->SetData(9,5,19);   
  chart->SetData(10,5,12); 
  chart->SetData(11,5,21); 
// define data next value column
  chart->SetData(0,6,3);
  chart->SetData(1,6,3);
  chart->SetData(2,6,9);   
  chart->SetData(3,6,2);   
  chart->SetData(4,6,4);   
  chart->SetData(5,6,3);   
  chart->SetData(6,6,8);   
  chart->SetData(7,6,8);   
  chart->SetData(8,6,2);   
  chart->SetData(9,6,5);   
  chart->SetData(10,6,7); 
  chart->SetData(11,6,9);     

	
	// Add a new dataset to chart, display type curve
  chart->AddDataSet(wxPie,8);
	// define data     
  chart->SetData(0,8,2);
  chart->SetData(1,8,1);
  chart->SetData(2,8,20);   
  chart->SetData(3,8,3);   
  chart->SetData(4,8,6);   
  chart->SetData(5,8,4);   
  chart->SetData(6,8,5);   
  chart->SetData(7,8,3);   
  chart->SetData(8,8,9);   
  chart->SetData(9,8,13);   
  chart->SetData(10,8,10); 
  chart->SetData(11,8,13);  
  
  chart->AddDataSet(wxLine,9);
	// define data     
  chart->SetData(0,9,2);
  chart->SetData(1,9,1);
  chart->SetData(2,9,20);   
  chart->SetData(3,9,3);   
  chart->SetData(4,9,6);   
  chart->SetData(5,9,4);   
  chart->SetData(6,9,5);   
  chart->SetData(7,9,3);   
  chart->SetData(8,9,9);   
  chart->SetData(9,9,13);   
  chart->SetData(10,9,10); 
  chart->SetData(11,9,13);  
  
   chart->AddDataSet(wxCurve,10);
	// define data     
  chart->SetData(0,10,2);
  chart->SetData(1,10,1);
  chart->SetData(2,10,20);   
  chart->SetData(3,10,3);   
  chart->SetData(4,10,6);   
  chart->SetData(5,10,4);   
  chart->SetData(6,10,5);   
  chart->SetData(7,10,3);   
  chart->SetData(8,10,9);   
  chart->SetData(9,10,13);   
  chart->SetData(10,10,10); 
  chart->SetData(11,10,13);  
  
  chart->AddDataSet(wxArea,11);
	// define data     
  chart->SetData(0,11,9);
  chart->SetData(1,11,16);
  chart->SetData(2,11,19);   
  chart->SetData(3,11,4);   
  chart->SetData(4,11,7);   
  chart->SetData(5,11,10);   
  chart->SetData(6,11,8);   
  chart->SetData(7,11,7);   
  chart->SetData(8,11,6);   
  chart->SetData(9,11,3);   
  chart->SetData(10,11,8); 
  chart->SetData(11,11,13);  
  
  // Add a new dataset to chart, display type floating Bar
	wxList *list2	= new wxList;
	c=12,cc=13;	
	list2->Append((wxObject *)c);
	list2->Append((wxObject *)cc);
	chart->AddDataSet(wxBarFloating,list2);
  //colour = wxTheColourDatabase->FindColour("RED");
  //wxBrush *brush1 = wxTheBrushList->FindOrCreateBrush(colour,2,wxSOLID);
  //chart->SetDataStyle(Brush);
	// define data start value column    
  chart->SetData(0,12,2);
  chart->SetData(1,12,4);
  chart->SetData(2,12,6);   
  chart->SetData(3,12,7);   
  chart->SetData(4,12,8);   
  chart->SetData(5,12,9);   
  chart->SetData(6,12,10);   
  chart->SetData(7,12,4);   
  chart->SetData(8,12,5);   
  chart->SetData(9,12,6);   
  chart->SetData(10,12,7); 
  chart->SetData(11,12,8); 
  // define data next value column
  chart->SetData(0,13,4);
  chart->SetData(1,13,5);
  chart->SetData(2,13,8);   
  chart->SetData(3,13,9);   
  chart->SetData(4,13,10);   
  chart->SetData(5,13,11);   
  chart->SetData(6,13,15);   
  chart->SetData(7,13,9);   
  chart->SetData(8,13,10);   
  chart->SetData(9,13,15);   
  chart->SetData(10,13,10); 
  chart->SetData(11,13,9); 

  // By default all datasets are display but so we can turn them on|off
  chart->ShowDataSet(TRUE,1); // first bar
  chart->ShowDataSet(TRUE,2); // second bar
  chart->ShowDataSet(TRUE,3); // third bar
  chart->ShowDataSet(TRUE,4); // xyplot
  chart->ShowDataSet(TRUE,5); // percent bar
  chart->ShowDataSet(TRUE,6); // pie chart
  chart->ShowDataSet(TRUE,7); // line chart
  chart->ShowDataSet(TRUE,8); // curve chart
  chart->ShowDataSet(TRUE,9); // area chart
  //int order[MAXDATASETS] = { 5,4,3,2,1,6,7,8,9,10 };
	//chart->SetDataSetOrder(order);       
} 


