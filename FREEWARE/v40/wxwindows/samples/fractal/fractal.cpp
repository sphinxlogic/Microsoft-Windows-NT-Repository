/*
From andrewd%sfe.com.au@nsf Tue Apr  5 03:33:04 1994
Date: Tue, 5 Apr 1994 12:01:18 +1000
From: Andrew Davison <andrewd@au.com.sfe>
To: wxwin-users@ed.aiai
Subject: Fractal mountains
Content-Length: 4725

Hi,

This is a quick port of a fractal mountain generator originally
done for MS-Windows. On a Sun the colours look a little washed
out and there is not as much snow or high mountains (maybe the
random number generators fault). The viewing plane is not
quite right as the original code used SetViewportOrg() which there
doesn't seem to be an equivalent of under wxWindows, and my quick
hack doesn't fix. 
*/

/*
 * Changes by JACS -- added wxYield() in Fractal, so under
 * Windows, is responsive to double clicking on Close box.
 * First double-click will stop painting, next will quit
 * program.
 */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <stdlib.h>
#include <math.h>
#include <time.h>


#define Random(x) (rand() % x)
#define Randomize() (srand((unsigned int)time(NULL)))

static int detail = 9;		// CHANGE THIS... 7,8,9 etc
static Bool abortPaint = FALSE; // Added by JACS to make it responsive to
                                // 'Close' messages.
static Bool running = FALSE;                                
static wxMenuBar *menuBar = NULL;

// Define a new application type
class MyApp: public wxApp
{ public:
	wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
	MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
	void OnMenuCommand(int id);
	Bool OnClose(void);
};

// Define a new canvas which can receive some events
class MyCanvas: public wxCanvas
{
  public:
	MyCanvas(wxFrame *frame, int x, int y, int w, int h, int style = wxRETAINED);
	void Draw(void);

  private:
	void OnPaint(void);
	void Fractal(int X1, int Y1, int X2, int Y2, int Z1, int Z2, int Z3, int Z4, int Iteration, double Std, double Ratio);
	wxPen *SnowPen, *MtnPen, *GreenPen;
	wxBrush *WaterBrush;
	int Sealevel;
};

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Fractal Mountains for wxWindows", 0, 0, 640, 480);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(0, "E&xit");
  menuBar = new wxMenuBar;
  menuBar->Append(file_menu, "&File");
  frame->SetMenuBar(menuBar);

  int width, height;
  frame->GetClientSize(&width, &height);

  (void) new MyCanvas(frame, 0, 0, width, height, wxRETAINED);

  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id) {
	case 0:
	  delete this;
  }
}

Bool MyFrame::OnClose(void)
{
  if (running)
  {
    abortPaint = TRUE;
    return FALSE;
  }
  else
    return TRUE;
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h, int style):
 wxCanvas(frame, x, y, w, h, style)
{
}

void MyCanvas::OnPaint()
{
	Draw();
}

void MyCanvas::Draw()
{
        if (running) return;
        
        abortPaint = FALSE;
        running = TRUE;
        menuBar->EnableTop(0, FALSE);
        wxYield();
        
	Randomize();

/* Some compilers (e.g. AIX CSet) don't like this syntax, so need to
 * use temporary wxColour variables.
	SnowPen = new wxPen(wxColour(255,255,255), 2, wxSOLID);
	MtnPen = new wxPen(wxColour(128,0,0), 1, wxSOLID);
	GreenPen = new wxPen(wxColour(0,128,0), 1, wxSOLID);
	WaterBrush = new wxBrush(wxColour(0,0,128), wxSOLID);
*/
        wxColour wxCol1(255,255,255);
        SnowPen = new wxPen(wxCol1, 2, wxSOLID);

        wxColour wxCol2(128,0,0);
        MtnPen = new wxPen(wxCol2, 1, wxSOLID);

        wxColour wxCol3(0,128,0);
        GreenPen = new wxPen(wxCol3, 1, wxSOLID);

        wxColour wxCol4(0,0,128);
        WaterBrush = new wxBrush(wxCol4, wxSOLID);

	int Left, Top, Right, Bottom;
	GetClientSize(&Right, &Bottom);

	Right *= 3; Right /= 4;
	Bottom *= 3; Bottom /= 4;
	Left = 0;
	Top = Bottom/8;

	wxPoint Water[4];
	Water[0].x = Left;	 		    Water[0].y = Top;
	Water[1].x = Right;				Water[1].y = Top;
	Water[2].x = Right+Bottom/2;	Water[2].y = Bottom;
	Water[3].x = Bottom/2;			Water[3].y = Bottom;

	SetBrush(WaterBrush);
	DrawPolygon(4, Water);

	double H = 0.75;
	double Scale = Bottom;
	double Ratio = 1.0 / pow(2.0, H);
	double Std = Scale * Ratio;
	Sealevel = Random(18) - 8;

	Fractal(Left, Top, Right, Bottom, 0, 0, 0, 0, detail, Std, Ratio);

        menuBar->EnableTop(0, TRUE);
        running = FALSE;
}

void MyCanvas::Fractal(int X1, int Y1, int X2, int Y2, int Z1, int Z2, int Z3, int Z4, int Iteration, double Std, double Ratio)
{
        if (abortPaint) return;
        wxYield();
        
	int Xmid = (X1 + X2) / 2;
	int Ymid = (Y1 + Y2) / 2;
	int Z23 = (Z2 + Z3) / 2;
	int Z41 = (Z4 + Z1) / 2;
	int Newz = (int)((Z1 + Z2 + Z3 + Z4) / 4 + (double)(Random(17) - 8) / 8.0 * Std);

	if (--Iteration)
	{
		int Z12 = (Z1 + Z2) / 2;
		int Z34 = (Z3 + Z4) / 2;
		double Stdmid = Std * Ratio;

		Fractal(Xmid, Y1, X2, Ymid, Z12, Z2, Z23, Newz, Iteration, Stdmid, Ratio);
		Fractal(X1, Y1, Xmid, Ymid, Z1, Z12, Newz, Z41, Iteration, Stdmid, Ratio);
		Fractal(Xmid, Ymid, X2, Y2, Newz, Z23, Z3, Z34, Iteration, Stdmid, Ratio);
		Fractal(X1, Ymid, Xmid, Y2, Z41, Newz, Z34, Z4, Iteration, Stdmid, Ratio);
	}
	else
	{
		if (Newz <= Sealevel)
		{
			wxPoint P[4];
			P[0].x = Y1 / 2 + X1;	P[0].y = Y1 + Z1;
			P[1].x = Y1 / 2 + X2;	P[1].y = Y1 + Z2;
			P[2].x = Y2 / 2 + X2;	P[2].y = Y2 + Z3;
			P[3].x = Y2 / 2 + X1;	P[3].y = Y2 + Z4;

			SetPen(wxBLACK_PEN);
			SetBrush(wxBLACK_BRUSH);

			DrawPolygon(4, P);

			if (Z1 >= -(60+Random(25)))
				SetPen(GreenPen);
			else if (Z1 >= -(100+Random(25)))
				SetPen(MtnPen);
			else
				SetPen(SnowPen);

			DrawLine(Ymid/2+X2, Ymid+Z23, Ymid/2+X1, Ymid+Z41);
		}
	}
}

