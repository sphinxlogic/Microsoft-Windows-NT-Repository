// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_split.h"

class MyApp;
class MyFrame;
class MyCanvas;

class MyApp: public wxApp
{
public:
	wxFrame* OnInit();
};

class MyFrame: public wxFrame
{
public:
	MyFrame(wxFrame* frame, char* title, int x, int y, int w, int h);
	virtual ~MyFrame();

	void OnMenuCommand(int id);
	Bool OnClose();

private:
	wxMenu*		fileMenu;
	wxMenuBar*	menuBar;
	MyCanvas*	leftCanvas;
	MyCanvas*	rightCanvas;
    wxSplitterWindow* splitter;
};

class MyCanvas: public wxCanvas // ScrolledArea
{
public:
	MyCanvas(wxWindow* parent, int x, int y, int w, int h);
	virtual ~MyCanvas();

	void OnPaint();
};

// ID for the menu quit command
#define SPLIT_QUIT          1
#define SPLIT_HORIZONTAL    2
#define SPLIT_VERTICAL      3
#define SPLIT_UNSPLIT       4

// This statement initializes the whole application and calls OnInit
MyApp myApp;


wxFrame *MyApp::OnInit(void)
{
	MyFrame* frame = new MyFrame(
			0,
			"wxSplitterWindow Example",
			-1, -1, 400, 300
			);

	// Show the frame
	frame->Show(TRUE);
  
	// Return the main frame window
	return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame* frame, char* title, int x, int y, int w, int h):
	wxFrame(frame, title, x, y, w, h)
{
	// set the icon
#ifdef wx_msw
	SetIcon(new wxIcon("mondrian"));
#endif
#ifdef wx_x
	SetIcon(new wxIcon("aiai.xbm"));
#endif

	// Make a menubar
	fileMenu = new wxMenu;
	fileMenu->Append(SPLIT_VERTICAL, "Split &Vertically");
	fileMenu->Append(SPLIT_HORIZONTAL, "Split &Horizontally");
	fileMenu->Append(SPLIT_UNSPLIT, "&Unsplit");
	fileMenu->Append(SPLIT_QUIT, "E&xit");

	menuBar = new wxMenuBar;
	menuBar->Append(fileMenu, "&File");

	SetMenuBar(menuBar);

    splitter = new wxSplitterWindow(this, 0, 0, 400, 400,
//            wxSP_BORDER);
            wxSP_3D);
//            wxSP_NOBORDER);

	leftCanvas = new MyCanvas(splitter, 0, 0, 400, 400);
    leftCanvas->SetBackground(wxRED_BRUSH);
    leftCanvas->SetScrollbars(18, 18, 8, 24, 4, 4);

	rightCanvas = new MyCanvas(splitter, 0, 0, 400, 400);
    rightCanvas->SetBackground(wxCYAN_BRUSH);
    rightCanvas->SetScrollbars(18, 18, 19, 505, 4, 4);
    rightCanvas->Show(FALSE);

    splitter->Initialize(leftCanvas);

    // Set this to prevent unsplitting
//    splitter->SetMinimumPaneSize(20);
    CreateStatusLine();
}

MyFrame::~MyFrame()
{
}

Bool MyFrame::OnClose()
{
	return TRUE;
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
	switch (id)
	{
	case SPLIT_QUIT:
		this->Close(TRUE);
		break;
    case SPLIT_VERTICAL :
        if ( splitter->IsSplit() )
            splitter->Unsplit();
        leftCanvas->Show(TRUE);
        rightCanvas->Show(TRUE);
        splitter->SplitVertically( leftCanvas, rightCanvas );
        break;
    case SPLIT_HORIZONTAL :
        if ( splitter->IsSplit() )
            splitter->Unsplit();
        leftCanvas->Show(TRUE);
        rightCanvas->Show(TRUE);
        splitter->SplitHorizontally( leftCanvas, rightCanvas );
        break;
    case SPLIT_UNSPLIT :
        if ( splitter->IsSplit() )
            splitter->Unsplit();
        break;
	}
}

MyCanvas::MyCanvas(wxWindow* parent, int x, int y, int w, int h) :
	wxCanvas(parent, x, y, w, h)
{
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnPaint()
{
	wxCanvasDC* dc = GetDC();
	dc->BeginDrawing();
        dc->SetFont(wxNORMAL_FONT);

	int w, h;
	GetVirtualSize(&w, &h);
	for (int x = 0; x <= w; x += 25)
	{
		dc->DrawLine(x, 0, x, h);
		for (int y = 0; y <= h; y += 25)
		{
			dc->DrawLine(0, y, w, y);
		}
	}
	dc->DrawLine(0, h, w, h);
	dc->DrawLine(w, 0, w, h);
	dc->DrawLine(0, 0, w, h);
	dc->DrawLine(w, 0, 0, h);

    dc->SetBackgroundMode(wxTRANSPARENT);
	char str[256];
	sprintf(str, "Virtual size: %d x %d", w, h);
	dc->DrawText(str, 0, 20);

	int cw, ch;
	GetClientSize(&cw, &ch);
	sprintf(str, "Client size: %d x %d", cw, ch);
	dc->DrawText(str, 0, 45);

	dc->EndDrawing();
}
