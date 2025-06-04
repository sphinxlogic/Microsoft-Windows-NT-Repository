/* -*-c++-*-  */

/*
 * File:	pop.cc
 * Purpose:	popup wxWindows app
 * Author:	Robert Cowell
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995
 */


// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"


static int BestMove;		/* Returned by search	*/
static int Adj[16] = { -1,-1,-1,0,-1,1,0,-1,0,1,1,-1,1,0,1,1};

//static int	Depth;		/* Search depth (default = 3)	*/
static int	Helpflag;
static char	FFlag;	    /* -f option: machine goes first */
static char     BFlag;	    /* Debugging flag		    */
static char	Startflag;	/* True on first move only */

static const char *image = ".rbXRB";
 
static int HISFIRST ; 	/* His best first move	*/
static int  MYFIRST ; /* My best first move	*/
static const int  BELL = 0x07;
static const int  BACKSP = 0x08;




// BBOARD member functions.

BBOARD :: BBOARD(int k)
{
  board = 0 ; 
  gameover = 1;
  if (k >= 0)
    area = k*(size = k);
  else
    area = size = 0;
  depth = 3;
  resize(k);
}

BBOARD :: ~BBOARD()
{
  delete [] board;
}

void BBOARD :: resize(int s)
{
  if (size != s || board == 0)
    {
      delete [] board;
      board = new char[(area = s*s)];
      size = s;
    }
  initialise();
}

void BBOARD :: initialise(int istart)
{
  for (int i=0; i<area; i++) board[i] = 0;
  for (int j=1; j<(size-1); j++)
    { 
      board[j] = 1;
      board[area -1-j] = 1;
      board[size*j] = 2;
      board[size*j + size-1] = 2;
    };
  laststar = star = -1; red = 0; blue = 0;
  gameover = 0;
 
  HISFIRST = (size*2+1);	/* His best first move	*/
  MYFIRST = (size+size/2-1);    /* My best first move	*/
  if (istart)       		/* computer starts first */
   {
     search(depth,1,-32000,-32000);
     move(BestMove);
   }
}

void BBOARD :: setdepth(int i)
{
  depth = (i <=0) ? 3 : i;
}


char * BBOARD :: CheckWin(void)
{	
  static char * s[] = { "I win!", "You win!", "Tie game!", 0};
  int i = search(1,1,-32000,-32000);
  if (BestMove >= 0) return 0;
  
  if (i>0) return s[0];
  if (i<0) return s[1];
  if (i==0) return s[2];
  return (char *)0; // avoids no return value error warning
}

void BBOARD :: beep()
{
  wxBell();
}


void BBOARD :: move(int n)
{
  int type;
  type = (board[n] += 3);
  if (type == 4) red++;
  else if (type == 5) blue++;
  laststar = star;
  star = n;
}

int BBOARD :: search (int ddepth, int who, int alpha, int beta)
{
  int i,j,k;
  int myalpha,hisalpha,result ;
  int best;
  int num;
  int bestmove, ii, jj, n;
  int SavStar;
  int SavBlue;
  int SavRed;
  int Save;
  char moves[9];
   
  best = -32000;
  num = 0;
  SavStar = star;
  SavBlue = blue;
  SavRed  = red;
  BestMove = -1;		/* No best move yet...	*/
  
  if (SavStar == -1)	/* special case opening moves	*/
    {
      BestMove = HISFIRST;
      if (who > 0) BestMove = MYFIRST;
      return 0;
    };
  
  if (!ddepth--)
    return(who * (blue - red));
  if (blue == (size*2-4) || red == (size*2-4))
    return(who*(blue - red)*1000);

  /* alpha-beta pruning   */
  if (who>0)   { myalpha = blue; hisalpha = red; }
  else 	{ myalpha = red; hisalpha = blue; }
  myalpha += ddepth;  /* Most optimistic estimate. */
  if (myalpha > (size*2-4)) myalpha = (size*2-4);
  if (myalpha == (size*2-4)) myalpha = 1000*(myalpha-hisalpha);
  else myalpha -= hisalpha;
  if (myalpha <= alpha) return best;
  
  k = star;
  i = k%size;
  j = k/size;
  for (n=0; n<8; n++)
    {
      if ((ii = i+Adj[n+n]) < 0 || ii >= size) continue;
      if ((jj = j+Adj[n+n+1]) < 0 || jj >= size) continue;
      if (board[moves[num] = jj*size + ii] < 3) num++; }
  if (num == 0) return(who*(blue - red)*1000);
  bestmove = moves[0];
  for (i=0; i<num; i++)
    { 
      Save = board[moves[i]];	move(moves[i]);
      k = -search(ddepth,-who,beta,alpha);
      board[moves[i]] = Save;
      blue = SavBlue; red = SavRed; star = SavStar;
      if (k > alpha) alpha = k;
      if (k > best) { best = k; bestmove = moves[i]; }
      if (k>100) break; 
    }
  BestMove = bestmove;
  return best; 
}

int BBOARD :: isValidMove(int n)
{
  if (size <= 0 || n < 0 || n >= area) return 0;

   
  int dr =  (n/size)  - (star/size);
  if (dr < 0) dr = -dr;
  int dc =  (n % size) - (star%size);
  if (dc < 0) dc = -dc;

  if ((star < 0 &&  board[n] == 0) ||
      (star >=0 && dr < 2 && dc < 2 &&  board[n] < 3))
    return 1;
  else
    return 0;
}

void BBOARD :: replyMove()
{
 if (!gameover && !CheckWin())
   {
     search(depth,1,-32000,-32000);
     move(BestMove);
   }
}
