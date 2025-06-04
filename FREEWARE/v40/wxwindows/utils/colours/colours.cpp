/*
  File:       colours.cc

  Author:     Ian Brown, Avionics R&D, British Aerospace Defence.

  Disclaimer: All the usual stuff about myself or BAe not giving any
              warranty or statement to the suitability of this code.
              i.e. If this blows up your machine and kills everyone
              in the vicinity then tough :)

  This program demonstrates the default colours available from wxwin.
*/

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

static wxMenuBar *menuBar = NULL;
#define NUM_COLS 70
static char *ColourNames[NUM_COLS]={"AQUAMARINE",
				    "BLACK",
				    "BLUE",
				    "BLUE VIOLET",
				    "BROWN",
				    "CADET BLUE",
				    "CORAL",
				    "CORNFLOWER BLUE",
				    "CYAN",
				    "DARK GREY",
				    "DARK GREEN",
				    "DARK OLIVE GREEN",
				    "DARK ORCHID",
				    "DARK SLATE BLUE",
				    "DARK SLATE GREY",
				    "DARK TURQUOISE",
				    "DIM GREY",
				    "FIREBRICK",
				    "FOREST GREEN",
				    "GOLD",
				    "GOLDENROD",
				    "GREY",
				    "GREEN",
				    "GREEN YELLOW",
				    "INDIAN RED",
				    "KHAKI",
				    "LIGHT BLUE",
				    "LIGHT GREY",
				    "LIGHT STEEL BLUE",
				    "LIME GREEN",
				    "LIGHT MAGENTA",
				    "MAGENTA",
				    "MAROON",
				    "MEDIUM AQUAMARINE",
				    "MEDIUM GREY",
				    "MEDIUM BLUE",
				    "MEDIUM FOREST GREEN",
				    "MEDIUM GOLDENROD",
				    "MEDIUM ORCHID",
				    "MEDIUM SEA GREEN",
				    "MEDIUM SLATE BLUE",
				    "MEDIUM SPRING GREEN",
				    "MEDIUM TURQUOISE",
				    "MEDIUM VIOLET RED",
				    "MIDNIGHT BLUE",
				    "NAVY",
				    "ORANGE",
				    "ORANGE RED",
				    "ORCHID",
				    "PALE GREEN",
				    "PINK",
				    "PLUM",
				    "PURPLE",
				    "RED",
				    "SALMON",
				    "SEA GREEN",
				    "SIENNA",
				    "SKY BLUE",
				    "SLATE BLUE",
				    "SPRING GREEN",
				    "STEEL BLUE",
				    "TAN",
				    "THISTLE",
				    "TURQUOISE",
				    "VIOLET",
				    "VIOLET RED",
				    "WHEAT",
				    "WHITE",
				    "YELLOW",
				    "YELLOW GREEN"
				    };
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
	MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style = wxRETAINED);
	void Draw(void);

  private:
	void OnPaint(void);
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
  MyFrame *frame = new MyFrame(NULL, "Colour Sampler for wxWindows", 0, 0, 700, 480);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(0, "E&xit");
  menuBar = new wxMenuBar;
  menuBar->Append(file_menu, "&File");
  frame->SetMenuBar(menuBar);

  int width, height;
  frame->GetClientSize(&width, &height);

  MyCanvas *canvas = new MyCanvas(frame, 0, 0, width, height, wxRETAINED);

  // Give the Canvas some Scroll Bars
  canvas->SetScrollbars(20,20,720/20,1000/20,10,10);
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
    return TRUE;
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h, long style):
 wxCanvas(frame, x, y, w, h, style)
{
}

void MyCanvas::OnPaint()
{
	Draw();
}

void MyCanvas::Draw()
{
 int i;
 wxFont *font = new wxFont(12,wxDEFAULT,wxNORMAL,wxBOLD);
 SetFont(font);

 for (i=0; i<NUM_COLS; i+=2){
   wxPen *pen = new wxPen(ColourNames[i],1,wxSOLID);
   wxBrush *brush = new wxBrush(ColourNames[i],wxSOLID);

   pen->SetColour(ColourNames[i]);
   brush->SetColour(ColourNames[i]);
   SetPen(pen);
   SetBrush(brush);

   wxColour *colour = new wxColour(ColourNames[i]);
   SetTextForeground(colour);
   DrawText(ColourNames[i],10,10*i+5);
   DrawRoundedRectangle(180,10*i+1,150,18,8);
   delete colour;

   // MUST deselect old pen and brush out of
   // canvas device context before deleting
   // them, or Windows will quickly crash!
   // Passing NULL restores the original
   // pen/brush, thereby deselecting
   // the new pen/brush
   SetPen(NULL);
   SetBrush(NULL);
   
   delete pen;
   delete brush;
   
   pen = new wxPen(ColourNames[i+1],1,wxSOLID);
   brush = new wxBrush(ColourNames[i+1],wxSOLID);
   pen->SetColour(ColourNames[i+1]);
   brush->SetColour(ColourNames[i+1]);
   SetPen(pen);
   SetBrush(brush);

   colour = new wxColour(ColourNames[i+1]);
   SetTextForeground(colour);
   DrawText(ColourNames[i+1],360,10*i+5);
   DrawRoundedRectangle(540,10*i+1,150,18,8);
   delete colour;

   SetPen(NULL);
   SetBrush(NULL);
   
   delete pen;
   delete brush;
   
   SetPen(wxBLACK_PEN);
   DrawLine(350,0,350,NUM_COLS/2*20-1);
 }
 // Select font out of device context before deleting
 // the font
 SetFont(NULL);
 delete font;
}

