/* -*-c++-*-  */
 /*
 * File:	pop.h
 * Purpose:	popup wxWindows app
 * Author:	Robert Cowell
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995
 */

#ifndef INCLUDE_POP_H
#define INCLUDE_POP_H

class  BBOARD
{	
  public:
  BBOARD(int = 7);
  ~BBOARD();

  void resize(int);
  void setdepth(int);
  void initialise(int = 0);

  char *board;

  int  laststar, star;
  char red;
  char blue;

  int size, area ;
  int depth;
  int gameover;

// game functions
  int isValidMove(int n);
  void move(int n);
  void replyMove();
  int search (int ddepth, int who, int alpha, int beta);
  char *  CheckWin(void);
  void beep();
};

#endif

