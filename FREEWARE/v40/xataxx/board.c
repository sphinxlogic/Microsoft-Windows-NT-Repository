#include <X11/Xlib.h>
#include "struct.h"

#ifdef VMS
#define random rand
#endif  /* VMS */

extern Display *dpy, *odpy, *currdpy;
extern Window win, owin, currwin;
extern GC gc, ogc, currgc;
extern int xsize, ysize, obstacle_color, scr, oscr, currscr, curr_player;
extern int botmode;
extern Pixmap mapw, mapb, omapw, omapb, wblock, bblock, owblock, obblock, bamp, obamp, currbamp;

#define abs(num) ((num) < 0) ? -(num) : (num);
#define b_look(x,y) board[x*y_board_size+y]

int *board, *bl2;
int x_board_size,y_board_size;
Bool flumping, wflump, fflump, jflump;

/* Allocate game board */
allocate_board(xsize,ysize)
     int xsize,ysize;
{
  int x,y;

  if ((xsize<MIN_BOARD_SIZE)||(xsize>MAX_BOARD_SIZE))
    return(1);
  if ((ysize<MIN_BOARD_SIZE)||(ysize>MAX_BOARD_SIZE))
    return(1);

  x_board_size = xsize;
  y_board_size = ysize;

  board = (int *) malloc(sizeof(int)*x_board_size*y_board_size);
  bl2 = (int *) malloc(sizeof(int)*x_board_size*y_board_size);
  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++)
      board[x*y_board_size+y] = EMPTY;

  return(0);
}

#ifdef NEWBOT

/* Copy one board array onto another for look-ahead */
copy_board(bs, bd)
int *bs, *bd;
{
  int x,y;

  for(x=0;x!=xsize;x++)
    for(y=0;y!=ysize;y++)
      bd[x*ysize+y] = bs[x*ysize+y];

  return(0);
}

#endif

/* Add obstruction to current board */
add_obstruction(xpos,ypos)
     int xpos,ypos;
{
  if (!board)
    return(1);

  if ((xpos>x_board_size)||(xpos < 0))
    return(1);
  if ((ypos>y_board_size)||(ypos < 0))
    return(1);

  b_look(xpos,ypos) = OBSTACLE;

  return(0);
}

/* Initialize the player pieces */
place_piece(xpos,ypos,player)
     int xpos,ypos,player;
{
  if ((xpos>x_board_size)||(xpos < 0))
    return(1);
  if ((ypos>y_board_size)||(ypos < 0))
    return(1);

  b_look(xpos,ypos) = player;
  if ((player!=WHITE)&&(player!=BLACK))
    return(1);

  return(1);
}

update_square(x,y,xnew,ynew,player)
int x,y,xnew,ynew,player;
{
  Pixmap smap;

  if ((x>=0)&&(x<x_board_size)&&(y>=0)&&(y<y_board_size))
    if ((b_look(x,y)!=EMPTY)&&(b_look(x,y)!=OBSTACLE)&&b_look(x,y)!=player) {
      b_look(x,y) = player; 
      if (flumping && jflump) {
        if (player == WHITE)
          smap = mapw;
        else
          smap = mapb;
        blat(xnew, ynew, x, y, smap, dpy, scr, gc, win, bamp);
        if (botmode) {
          if (player == WHITE)
	    smap = omapw;
          else
	    smap = omapb;
          blat(xnew, ynew, x, y, smap, odpy, oscr, ogc, owin, obamp);
	}
      }
    }
}

/* Move a piece.
   Return 1 if the piece cannot be moved.
   */
move_piece(xcurr,ycurr,xnew,ynew,player)
     int xcurr,ycurr,xnew,ynew,player;
{
  int xdiff,ydiff,other_player,p;
  Pixmap smap;

  /* Player must own current position */
  if (b_look(xcurr,ycurr) != player)
    return(1);

  /* The new position must be empty */
  if (b_look(xnew,ynew) != EMPTY)
    return(1);

  /* Check the validity of moving to the new position */
  xdiff = abs(xcurr-xnew);
  ydiff = abs(ycurr-ynew);

  /* Simple one square slurp */
  if ((xdiff <= 1)&&(ydiff <=1))
    b_look(xnew,ynew) = player;
  else {
    if ((xdiff <= 2)&&(ydiff <=2)) {
      b_look(xcurr,ycurr) = EMPTY;
      b_look(xnew,ynew) = player;
      redraw_win(dpy, win, bamp, gc);
      if (botmode)
        redraw_win(odpy, owin, obamp, ogc);
    }
    else
      return(1);
  }
  if(flumping) {
    if (player == WHITE)
	smap = mapw;
    else
	smap = mapb;
    blat(xcurr, ycurr, xnew, ynew, smap, dpy, scr, gc, win, bamp);
    if (botmode) {
      if (player == WHITE)
	smap = omapw;
      else
	smap = omapb;
      blat(xcurr, ycurr, xnew, ynew, smap, odpy, oscr, ogc, owin, obamp);
    }
  }
  /* Update the squares around the new one */
  update_square((xnew-1),(ynew-1),xnew,ynew,player);
  update_square((xnew),(ynew-1),xnew,ynew,player);
  update_square((xnew+1),(ynew-1),xnew,ynew,player);
  update_square((xnew+1),(ynew),xnew,ynew,player);
  update_square((xnew+1),(ynew+1),xnew,ynew,player);
  update_square((xnew),(ynew+1),xnew,ynew,player);
  update_square((xnew-1),(ynew+1),xnew,ynew,player);
  update_square((xnew-1),(ynew),xnew,ynew,player);

  return(0);
}

#ifdef NEWBOT

sample_move(xcurr,ycurr,xnew,ynew,player)
     int xcurr,ycurr,xnew,ynew,player;
{
  int xdiff,ydiff,other_player,p;

  /* Player must own current position */
  if (b_look(xcurr,ycurr) != player)
    return(1);

  /* The new position must be empty */
  if (b_look(xnew,ynew) != EMPTY)
    return(1);

  /* Check the validity of moving to the new position */
  xdiff = abs(xcurr-xnew);
  ydiff = abs(ycurr-ynew);

  /* Simple one square slurp */
  if ((xdiff <= 1)&&(ydiff <=1))
    b_look(xnew,ynew) = player;
  else {
    if ((xdiff <= 2)&&(ydiff <=2)) {
      b_look(xcurr,ycurr) = EMPTY;
      b_look(xnew,ynew) = player;
    }
    else
      return(1);
  }
  /* Update the squares around the new one */
  update_square((xnew-1),(ynew-1),xnew,ynew,player);
  update_square((xnew),(ynew-1),xnew,ynew,player);
  update_square((xnew+1),(ynew-1),xnew,ynew,player);
  update_square((xnew+1),(ynew),xnew,ynew,player);
  update_square((xnew+1),(ynew+1),xnew,ynew,player);
  update_square((xnew),(ynew+1),xnew,ynew,player);
  update_square((xnew-1),(ynew+1),xnew,ynew,player);
  update_square((xnew-1),(ynew),xnew,ynew,player);

  return(0);
}

#endif

int b_peek(x,y)
int x,y;
{
  return(board[x*y_board_size+y]);
}

/* Return an array of integers specifying the board */
int *return_board()
{
  return(board);
}

return_x_size()
{
  return(x_board_size);
}

return_y_size()
{
  return(y_board_size);
}

/* Return 1 if the game is at an end. */
end_game()
{
  int x,y;

  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++)
      if (b_look(x,y)==EMPTY)
	return(0);
  
  return(1);
}

/* Clean the board */
clean_board()
{
  int x,y;

  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++)
      if (b_look(x,y)!=OBSTACLE)
	b_look(x,y) = EMPTY;
}

/* Return the number of pieces belonging to player. */
count_board(player)
     int player;
{
  int x,y,count=0;

  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++)
      if (b_look(x,y)==player)
	count++;

  return(count);
}
  
#define do_check(x,y,player) \
  if (((x)>=0)&&((x)<x_board_size)&&((y)>=0)&&((y)<y_board_size)) \
    if (b_look((x),(y))==EMPTY) \
      return(1);

/* Return 1 if player has a valid move at x,y. */
check_move(x,y,player)
     int x,y,player;
{
  do_check(x-1,y-1,player);
  do_check(x,y-1,player);
  do_check(x+1,y-1,player);
  do_check(x+1,y,player);
  do_check(x+1,y+1,player);
  do_check(x,y+1,player);
  do_check(x-1,y+1,player);
  do_check(x-1,y,player);

  do_check(x-2,y-2,player);
  do_check(x-1,y-2,player);
  do_check(x,y-2,player);
  do_check(x+1,y-2,player);
  do_check(x+2,y-2,player);
  do_check(x+2,y-1,player);
  do_check(x+2,y,player);
  do_check(x+2,y+1,player);
  do_check(x+2,y+2,player);
  do_check(x+1,y+2,player);
  do_check(x,y+2,player);
  do_check(x-1,y+2,player);
  do_check(x-2,y+2,player);
  do_check(x-2,y+1,player);
  do_check(x-2,y,player);
  do_check(x-2,y-1,player);
  return(0);
}

/* Return 1 if player has a valid move. */
valid_move(player)
{
  int x,y;

  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++)
      if (b_look(x,y)==player)
	if (check_move(x,y,player))
	  return(1);

  return(0);
}

typedef struct {
  int ox,oy;
  int nx,ny;
} thing;

b_look2(x,y)
     int x,y;
{
  if ((x>=0)&&(x<x_board_size)&&(y>=0)&&(y<y_board_size))
    return(b_look(x,y));
  else
    return(OBSTACLE);
}

thing best;
int best_count,oplayer,curr_count;

evaluate(oldx,oldy,newx,newy,oplayer)
{
  if ((newx>=0)&&(newx<x_board_size)&&(newy>=0)&&(newy<y_board_size)) { 
    if (b_look2(newx,newy)==EMPTY) { 
    if (b_look2((newx-1),(newy-1))==oplayer) 
      curr_count++; 
    if (b_look2(newx,(newy-1))==oplayer) 
      curr_count++; 
    if (b_look2((newx+1),(newy-1))==oplayer) 
      curr_count++; 
    if (b_look2((newx+1),newy)==oplayer) 
      curr_count++; 
    if (b_look2((newx+1),(newy+1))==oplayer) 
      curr_count++; 
    if (b_look2(newx,(newy+1))==oplayer) 
      curr_count++; 
    if (b_look2((newx-1),(newy+1))==oplayer) 
      curr_count++; 
    if (b_look2((newx-1),newy)==oplayer) 
      curr_count++; 
    if (curr_count > best_count) { 
      if ((best_count==-1)||(random()&1)) { 
	best.ox = oldx; 
        best.oy = oldy; 
	best.nx = newx; 
	best.ny = newy; 
        best_count = curr_count; 
      } 
    } 
  }
}
}

#ifdef NEWBOT

smart_robot(player)
int player;
{
  int botp, enemp, oppon, bgross, egross, bnet, enet, nx, ny;
  thing b1, b2;

  oppon = (player==WHITE) ? BLACK : WHITE;
  botp = count_board(player);
  enemp = count_board(oppon);
  copy_board(board, bl2);
  best_count = -500;

  for (x=0;x!=x_board_size;x++) {
    for (y=0;y!=y_board_size;y++) {
      if (b_look(x,y)==player) {
        bgross = count_board(player);
	egross = count_board(oppon); /* must get values for nx, ny & loop */
        if ((nx>=0) && (nx<x_board_size) && (ny>=0) && (ny<y_board_size)) {
	  sample_move(x, y, nx, ny, player);
	  robot_move(oppon);
	  bnet = bgross - botp;
	  enet = egross - enemp; 
	  if ((bnet-enet) > best_count) {
	    b1.ox = x;
	    b1.oy = y;
            b1.nx = nx;
	    b1.ny = ny;
            best_count = (bnet-enet);
          }
	}
      }
    }
  }
}

#endif

/* Make a move for player. */
orobot_move(player)
     int player;
{
  int x,y;

  best_count = -1;
  best.ox = -1;
  oplayer = (player==WHITE) ? BLACK : WHITE;

  for (x=0;x!=x_board_size;x++)
    for (y=0;y!=y_board_size;y++) {
      if (b_look(x,y)==player) {
	curr_count=1; evaluate(x,y,(x-1),(y-1),oplayer);
	curr_count=1; evaluate(x,y,(x),(y-1),oplayer);
	curr_count=1; evaluate(x,y,(x+1),(y-1),oplayer);
	curr_count=1; evaluate(x,y,(x+1),(y),oplayer);
	curr_count=1; evaluate(x,y,(x+1),(y+1),oplayer);
	curr_count=1; evaluate(x,y,(x),(y+1),oplayer);
	curr_count=1; evaluate(x,y,(x-1),(y+1),oplayer);
	curr_count=1; evaluate(x,y,(x-1),(y),oplayer);
	
	curr_count= 0; evaluate(x,y,(x-2),(y-2),oplayer);
	curr_count= 0; evaluate(x,y,(x-1),(y-2),oplayer);
	curr_count= 0; evaluate(x,y,(x),(y-2),oplayer);
	curr_count= 0; evaluate(x,y,(x+1),(y-2),oplayer);
	curr_count= 0; evaluate(x,y,(x+2),(y-2),oplayer);
	curr_count= 0; evaluate(x,y,(x+2),(y-1),oplayer);
	curr_count= 0; evaluate(x,y,(x+2),(y),oplayer);
	curr_count= 0; evaluate(x,y,(x+2),(y+1),oplayer);
	curr_count= 0; evaluate(x,y,(x+2),(y+2),oplayer);
	curr_count= 0; evaluate(x,y,(x+1),(y+2),oplayer);
	curr_count= 0; evaluate(x,y,(x),(y+2),oplayer);
	curr_count= 0; evaluate(x,y,(x-1),(y+2),oplayer);
	curr_count= 0; evaluate(x,y,(x-2),(y+2),oplayer);
	curr_count= 0; evaluate(x,y,(x-2),(y+1),oplayer);
	curr_count= 0; evaluate(x,y,(x-2),(y),oplayer);
	curr_count= 0; evaluate(x,y,(x-2),(y-1),oplayer);
      }
    }
  if (best_count==-1)
    return;
  move_piece(best.ox,best.oy,best.nx,best.ny,player);
}








