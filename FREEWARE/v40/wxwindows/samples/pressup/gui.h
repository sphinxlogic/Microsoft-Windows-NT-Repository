/*
 * File:	gui.h
 * Purpose:	GUI header file
 * Author:	Robert Cowell
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995
 * Adapted from the mininal app of wxWindows.
 */


// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
// Define the gridcanvas class
class GridCanvas : public wxCanvas
{
public:
  // Margin
  float marginX;
  float marginY;
  wxBitmap *tileBitmap;

  GridCanvas(wxFrame *frame, int n=7);
  ~GridCanvas(){}

  void OnEvent(wxMouseEvent & event);
  void DrawGrid();
  void DrawShape(int s, int pos) ;
  void DrawShape(int s, float x, float y);
  void OnPaint();
  void TileBackground();
  void NewGame(int);
  void CalculateMargins(void);

  wxPen * blue_pen;
  wxPen * red_pen;
  wxPen * white_pen;
  wxPen * black_pen;

  wxBrush * white_brush;
  wxBrush * red_brush;
  wxBrush * blue_brush;
  wxBrush * black_brush;


  // board characteristics and functions
  // this is called by the mouse, returns n+1 if mouse event
  // is in inside the canvas. n is the relevant board position
  int IsInBoard(float x, float y);
  
  void ShowBoard();
  void UpdateBoard();
  void ShowScores();

  BBOARD game; // the board the game is played on

  wxFrame * parent;
};

class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
    Bool OnClose(void) { return TRUE; }
    void OnSize(int, int);

    GridCanvas * canvas;
    int shape;
};

#define SQUARE_SIZE 30.0

// ID for the menu quit command
#define MINIMAL_QUIT 100
#define DEPTH1 	1
#define DEPTH2 	2
#define DEPTH3 	3
#define DEPTH4 	4
#define DEPTH5 	5
#define DEPTH6 	6
#define DEPTH7 	7
#define DEPTH8 	8
#define DEPTH9 	9

#define SIZE4   14
#define SIZE5   15
#define SIZE6   16
#define SIZE7   17
#define SIZE8   18
#define SIZE9   19
#define SIZE10  20

#define NEWGAMEYOU 30
#define NEWGAMEME  31

#define BLANK 40
#define REDC  41
#define BLUEC 42
#define FULL  43
#define REDS  44
#define BLUES  45
#define STAR  46

#define ABOUT 50
#define RULES 51

