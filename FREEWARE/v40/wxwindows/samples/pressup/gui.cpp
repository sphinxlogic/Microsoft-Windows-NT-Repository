/*
 * File:	gui.cc
 * Purpose:	pressup wxWindows app: GUI header file
 * Author:	Robert Cowell
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995
 * Adapted from the mininal app of wxWindows.
 */

static const char sccsid[] = "%W% %G%";

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

#include <string.h>
#include <stdio.h>

#include "board.h"
#include "gui.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

GridCanvas :: GridCanvas(wxFrame *frame, int n) :
wxCanvas(frame, 0, 0, n*30, n*30, wxRETAINED)
{
  marginX = 0.0;
  marginY = 0.0;

  tileBitmap = NULL;
  
#if defined(wx_msw) || defined(wx_x)
  tileBitmap = new wxBitmap("tile.bmp", wxBITMAP_TYPE_BMP);

  if (!tileBitmap->Ok())
  {
   delete tileBitmap;
   tileBitmap = NULL;
  }
  else
  {
    if (tileBitmap->GetColourMap())
      GetDC()->SetColourMap(tileBitmap->GetColourMap());
  }
#endif

  parent = frame;
  SetCursor( new wxCursor(wxCURSOR_HAND));
  
  red_pen = new wxPen("RED",1,wxSOLID);
  blue_pen = new wxPen("BLUE",1,wxSOLID);
  white_pen = new wxPen("WHITE",1,wxSOLID);
  black_pen = new wxPen("BLACK",1,wxSOLID);

  white_brush = new wxBrush("WHITE",wxSOLID);
  black_brush = new wxBrush("BLACK",wxSOLID);
  red_brush = new wxBrush("RED",wxSOLID);
  blue_brush = new wxBrush("BLUE",wxSOLID);

  game.resize(n);
}

void GridCanvas :: CalculateMargins()
{
  int width, height;
  GetClientSize(&width, &height);
  
  marginX = (width - (SQUARE_SIZE*game.size))/2.0;
  marginY = (height - (SQUARE_SIZE*game.size))/2.0;
  if (marginX < 0.0) marginX = 0.0;
  if (marginY < 0.0) marginY = 0.0;
}

void GridCanvas :: OnPaint()
// this is for redrawing the game if window is resized.
{
  TileBackground();
  DrawGrid();
  ShowBoard();
}

// Put a fancy background on the canvas
void GridCanvas :: TileBackground()
{
  if (tileBitmap)
  {
    int w, h, bitmapW, bitmapH;
    GetClientSize(&w, &h);
    bitmapW = tileBitmap->GetWidth();
    bitmapH = tileBitmap->GetHeight();
    
    wxMemoryDC memDC;
    wxCanvasDC *canvasDC = GetDC();
    memDC.SelectObject(tileBitmap);
    if (tileBitmap->GetColourMap())
     memDC.SetColourMap(tileBitmap->GetColourMap());
     
    for (int i = 0; i <= w; i += bitmapW)
      for (int j = 0; j <= h; j += bitmapH)
      {
        canvasDC->Blit(i, j, bitmapW, bitmapH, &memDC, 0, 0);
      }
  }
}

void GridCanvas :: NewGame(int i)
{
 Clear();
 // clear the status fields
 parent->SetStatusText("",0);
 parent->SetStatusText("",1);
 parent->SetStatusText("",2);
 parent->SetStatusText("",3);
 // reset the game board
 game.initialise(i);
 CalculateMargins();
 // now paint the new game board
 OnPaint();
}


void GridCanvas :: DrawGrid()
{
  SetPen(red_pen);
  for (int i = 0; i<= game.size; i++)
  {
   DrawLine(marginX + 0.0,  marginY + SQUARE_SIZE*i, marginX + SQUARE_SIZE*game.size, marginY + SQUARE_SIZE*i);
   DrawLine(marginX + SQUARE_SIZE*i, marginY + 0.0, marginX + SQUARE_SIZE*i, marginY + SQUARE_SIZE*game.size);
  }
  DrawPoint(marginX + SQUARE_SIZE*game.size,marginY + SQUARE_SIZE*game.size);
}

void GridCanvas :: DrawShape(int s, int pos)
{
 float x, y;
 x = marginX + (pos / game.size) * SQUARE_SIZE ;
 y = marginY + (pos % game.size) * SQUARE_SIZE ;
 DrawShape(s,x,y);
}

 

void GridCanvas :: DrawShape(int shape, float x, float y)
{
   x = marginX + (  (int)( ((x-marginX)+1.0)/SQUARE_SIZE))*SQUARE_SIZE+15;
   y = marginY + (  (int)( ((y-marginY)+1.0)/SQUARE_SIZE))*SQUARE_SIZE+15;
   // clear the box first

      SetPen(white_pen); SetBrush(white_brush);
      DrawRectangle(x-14,y-14,29.0, 29.0);

   switch (shape)
   {
    case 0:
    	{
	  SetPen(black_pen); SetBrush(white_brush);
	  DrawEllipse(x-5,y-5,11.0,11.0); break;
        }
 

    case 1: 
    	{
	  SetPen(red_pen); SetBrush(red_brush);
	  DrawEllipse(x-5,y-5,10.0,10.0); break;
        }

    case 2: 
    	{
	  SetPen(blue_pen); SetBrush(blue_brush);
	  DrawEllipse(x-5,y-5,10.0,10.0); break;
        }

    case 3: 
    	{
	  SetPen(black_pen); SetBrush(black_brush);
	  DrawRectangle(x-14,y-14,29.0, 29.0); break;
        }

    case 4: 
    	{
	  SetPen(red_pen); SetBrush(red_brush);
	  DrawRectangle(x-14,y-14,29.0, 29.0); break;
        }

    case 5: 
    	{
	  SetPen(blue_pen); SetBrush(blue_brush);
	  DrawRectangle(x-14,y-14, 29.0, 29.0); break;
        }

    case 6:
    	{
	  SetPen(black_pen); SetBrush(black_brush);
	  DrawEllipse(x -3 ,y -3 ,6.0, 6.0);
	  DrawLine(x-6,y,x+6,y);
	  DrawLine(x,y-6,x,y+6);

	  break;
	}

    default : break; // unknown shape!
   }
}

void GridCanvas :: OnEvent(wxMouseEvent & event)
{
 float x,y;
 int n;
 if (game.gameover)
   return;         // no more to play
 event.Position(&x,&y);
 if (event.LeftUp() && (n = IsInBoard(x,y)) )
  {
   n--;
   if (game.isValidMove(n))
    {
    	game.move(n);
	UpdateBoard();
	char * s = game.CheckWin();
	if (s)
	  {
	    game.gameover = 1;
            parent->SetStatusText(s,2);
	  }
	else // want the computers response
	  {
	    game.replyMove();
	    UpdateBoard();
          }
	s = game.CheckWin();
	if (s)
	  {
	    game.gameover = 1;
            parent->SetStatusText(s,2);
	  }
    }
    else
	game.beep(); //wxMessageBox("Invalid Move !!");
  }
}

int  GridCanvas :: IsInBoard(float x, float y)
{
  int n = ( (x-marginX)/SQUARE_SIZE < game.size) && ((y-marginY)/SQUARE_SIZE < game.size);
  return (n==0) ? 0 : 
  (n =  1 + (((int)( ((x-marginX)+1.0)/SQUARE_SIZE))*game.size) + (((int)( ((y-marginY)+1.0)/SQUARE_SIZE))) );
}

 
void GridCanvas :: ShowBoard()
{
  int n;
  for (int i=0; i< game.size; i++)
    for (int j=0; j < game.size; j++)
      {
	if ((n = i*game.size+ j) ==  game.star)
	  DrawShape(6,n);
	else
	  DrawShape(game.board[n], n);
      }
  ShowScores();
}

void GridCanvas :: ShowScores()
{
  // this does the scores.
  char buff[16], *s;
  strcpy(buff,"Me (blue):"); 
  s = buff + strlen(buff);
  sprintf(s,"%3d",game.blue); parent->SetStatusText(buff,0);
  strcpy(buff,"You (red):"); 
  s = buff + strlen(buff);
  sprintf(s,"%3d",game.red); parent->SetStatusText(buff,1);

  strcpy(buff,"Level: ");
  s = buff + strlen(buff);
  sprintf(s,"%3d",game.depth); parent->SetStatusText(buff,3);
}

void GridCanvas :: UpdateBoard()
{
  int n = game.laststar;
  if (n == game.star) return;
  if (n >= 0)
    DrawShape(game.board[n], n);
  if (game.star >= 0)
    DrawShape(6,game.star);
  ShowScores();
}


// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Pressup Game", 50, 50, 400, 300);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("pressup_icn"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("pressup.xbm", wxBITMAP_TYPE_XBM));
#endif

  // Make a menubar
  wxMenuBar *menu_bar = new wxMenuBar;

  wxMenu * menu = new wxMenu;
  menu_bar->Append(menu, "&File");
  menu->Append(NEWGAMEYOU,"New Game? &You go first?");
  menu->Append(NEWGAMEME,"New Game? &I go first");
  menu->AppendSeparator();
  menu->Append(MINIMAL_QUIT, "E&xit");

  menu = new wxMenu;
  menu_bar->Append(menu,"&Size");
  menu->Append(SIZE4,"&4 X 4");
  menu->Append(SIZE5,"&5 X 5");
  menu->Append(SIZE6,"&6 X 6");
  menu->Append(SIZE7,"&7 X 7");
  menu->Append(SIZE8,"&8 X 8");
  menu->Append(SIZE9,"&9 X 9");
  menu->Append(SIZE10,"&10 X 10");

  menu = new wxMenu;
  menu_bar->Append(menu,"&Depth");
  menu->Append(DEPTH1,"&Easy");
  menu->Append(DEPTH2,"Level &2");
  menu->Append(DEPTH3,"Level &3");
  menu->Append(DEPTH4,"Level &4");
  menu->Append(DEPTH5,"Level &5");
  menu->Append(DEPTH6,"Level &6");
  menu->Append(DEPTH7,"Level &7");
  menu->Append(DEPTH8,"Level &8");
  menu->Append(DEPTH9,"&Hard");

  menu = new wxMenu;
  menu_bar->Append(menu,"&Help");
  menu->Append(RULES,"&Rules");
  menu->Append(ABOUT,"&About");


  frame->SetMenuBar(menu_bar);

  frame -> canvas = new GridCanvas(frame, 7);

  // make a two part status line
  frame->CreateStatusLine(4);
  for (int j = 0; j < 4; j++)
    frame->SetStatusText("",j);
  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  shape = BLANK;
  canvas = NULL;
}

// Intercept menu commands
void MyFrame::OnSize(int w, int h)
{
  wxFrame::OnSize(w, h);
  if (canvas)
    canvas->CalculateMargins();
}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id) {
  case MINIMAL_QUIT:
    delete this;
    break;
  case DEPTH1:case DEPTH2:  case DEPTH3:
  case DEPTH4:case DEPTH5:  case DEPTH6:
  case DEPTH7:case DEPTH8:  case DEPTH9:
	{
	  canvas->game.setdepth(id);
	  // the next line is needed to hide a bug 
	  // with the first status text slot dissapearing
	  canvas->ShowScores();
	  break;
         }
  case SIZE4:   case SIZE5:   case SIZE6:
  case SIZE7:   case SIZE8:   case SIZE9:
  case SIZE10:
    	{ canvas->game.resize(id - 10); canvas->NewGame(0); break;}
  case NEWGAMEYOU:
	{ canvas->NewGame(0); break;  }
  case NEWGAMEME:
  	{ canvas->NewGame(1); break; }

  case RULES:
    {
    wxMessageBox("You must try to beat the computer by collecting\n"
		 "all your (red) pegs first, or failing that more pegs \n"
		 "than the computer (blue). The only legal moves are to a\n"
		 "square neighbouring the star which has not been\n"
		 "occupied already. The first move must be on a neutral square"
		 );
    break;
    }
  case ABOUT:
   {
     wxMessageBox("wxWindows port of Press-Ups program\n"
		  "originally by Prof. Steve Ward, based on code\n"
		  "in the C-Users' Group Library, volume 101.\n\n"
		  );
    break;
   }
  default:
    break;
  }
}

