#include <stdio.h>
#include <X11/Xlib.h>
#include "struct.h"

extern int obstacle_color, oldw, oldb;
extern Display *dpy, *odpy;
extern Window win, owin;
extern int botmode;

#define DEMODELAY 1

#ifdef VMS
#define random rand
#define srandom srand
#endif /* VMS */

demo()
{
  XEvent event;
  int curr_player=WHITE,currx,curry,x,y;
  char mess[100];

  srandom(time(NULL));
  x = return_x_size();
  y = return_y_size();
  place_piece(0,0,WHITE);
  place_piece(x-1,y-1,WHITE);
  place_piece(0,y-1,BLACK);
  place_piece(x-1,0,BLACK);
  obstacle_color = curr_player;
/*
  XNextEvent(dpy,&event);
*/
  redraw_all(curr_player);
  if (botmode) {
    XNextEvent(odpy,&event);
    redraw_all(curr_player);
  }

    redraw_all(curr_player);
    while (!end_game()) {
      robot_move(WHITE);
      if (valid_move(BLACK))
        obstacle_color = BLACK;
      oldw = count_board(WHITE);
      oldb = count_board(BLACK);
      redraw_all(curr_player);
      if(!waitferkey(DEMODELAY))
	return(1);
      robot_move(BLACK);
      if (valid_move(WHITE))
        obstacle_color = WHITE;
      oldw = count_board(WHITE);
      oldb = count_board(BLACK);
      redraw_all(curr_player);
      if(!waitferkey(DEMODELAY))
	return(1);
    }
    oldw = count_board(WHITE);
    oldb = count_board(BLACK);
    redraw_all(curr_player);
    return(0);

}
