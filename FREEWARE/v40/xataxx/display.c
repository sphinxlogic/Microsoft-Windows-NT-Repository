#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include "struct.h"

#ifdef VMS
#define srandom srand
#endif /* VMS */

Display *dpy,*odpy,*currdpy;
Window win,owin,currwin;
GC gc,ogc,currgc;
int xsize,ysize,obstacle_color;
extern int botmode;
extern int oldw, oldb;
int scr, oscr, currscr;
extern Cursor upc, downc, oupc, odownc;
Pixmap bamp, obamp, currbamp;
int xwin,ywin, curr_player;
extern Bool flumping;
extern int botlevel;


print_board()
{
  int x,y,*board;
  int i,j;

  x = return_x_size();
  y = return_y_size();

  board = (int *) return_board();

  for (i=0;i!=y;i++) {
    for (j=0;j!=x;j++)
      printf("%d ",board[j*y+i]);
    printf("\n");
  }
}

initialize_display(name)
     char *name;
{
  int scn;
  XSetWindowAttributes attr;
  XGCValues gcvalues;
  XSizeHints hints;
  XClassHint classhint;
  XWMHints wmhint;

  xsize = return_x_size();
  ysize = return_y_size();
  xwin = xsize*X_PIECE_SIZE;
  ywin = ysize*Y_PIECE_SIZE+TEXT_SIZE;
  hints.min_width = xwin;
  hints.max_width = xwin;
  hints.min_height = ywin;
  hints.max_height = ywin;
  hints.flags = PMinSize | PMaxSize;
  classhint.res_name = "xataxx";
  classhint.res_class = "xataxx";
  wmhint.input = True;
  wmhint.flags = InputHint;

  if (xwin < 120) xwin = 120;
  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr,"Could not open local display.  Bummer, drag.\n");
    exit(1);
  }
  if (botmode) {
    if (!(odpy = XOpenDisplay(name))) {
      fprintf(stderr,"Could not open display %s.  Bummer, drag.\n",name);
      exit(1);
    }
  }
#ifdef DEBUG
	printf("Display(s) open.\n");
#endif
  scn = DefaultScreen(dpy);
  scr = scn;
  win = XCreateWindow(dpy,DefaultRootWindow(dpy),100,100,xwin,ywin,
                            1,DefaultDepth(dpy,scr),InputOutput,CopyFromParent,0,0);
  attr.backing_store = Always;
  attr.event_mask = ButtonPressMask | KeyPressMask | ButtonReleaseMask 
    | ExposureMask | MotionNotify;
  attr.background_pixel = BlackPixel(dpy,scr);
  XChangeWindowAttributes(dpy,win,CWEventMask|CWBackingStore|CWBackPixel,&attr);
  XSetWMHints(dpy, win, &wmhint);
  XSetClassHint(dpy, win, &classhint);
  XSetIconName(dpy,win,"XAtaxx: White");
  XStoreName(dpy,win,"XAtaxx: White");
  XSetNormalHints(dpy, win, &hints);
  XMapWindow(dpy,win);
  gcvalues.foreground = WhitePixel(dpy,scn);
  gcvalues.background = BlackPixel(dpy,scn);
  gc = XCreateGC(dpy,win,GCForeground|GCBackground,&gcvalues);
  XSetFunction(dpy,gc,GXcopy);
  bamp = XCreatePixmap(dpy, win, xwin, ywin, DefaultDepth(dpy,scr));
  if (botmode) {
    scn = DefaultScreen(odpy);
    oscr = scn;
    attr.background_pixel = BlackPixel(odpy,oscr);
    owin = XCreateWindow(odpy,DefaultRootWindow(odpy),
			     100,100,xwin,ywin,
			     1,1,InputOutput,CopyFromParent,0,0);
    XChangeWindowAttributes(odpy,owin,CWEventMask|CWBackingStore|CWBackPixel,&attr);
    XSetWMHints(odpy, owin, &wmhint);
    XSetClassHint(odpy, owin, &classhint);
    XSetIconName(odpy,owin,"XAtaxx: Black");
    XStoreName(odpy,owin,"XAtaxx: Black");
    XSetNormalHints(odpy, owin, &hints);
    XMapWindow(odpy,owin);
    gcvalues.foreground = WhitePixel(odpy,scn);
    gcvalues.background = BlackPixel(odpy,scn);
    ogc = XCreateGC(odpy,owin,GCForeground,&gcvalues);
    XSetFunction(odpy,ogc,GXcopy);
    obamp = XCreatePixmap(odpy, owin, xwin, ywin, DefaultDepth(dpy,scr));
  }

#ifdef DEBUG
	printf("Opt window dealt with.\n");
#endif

  XFlush(dpy);
  if (botmode) {
    XFlush(odpy);
  }
}

wipemap(cdpy, cbamp, cgc, cscr, cwin)
Display *cdpy;
Pixmap cbamp;
int cscr;
GC cgc;
Window cwin;
{
  GC ccgc;

  ccgc = XCreateGC(cdpy, cwin, 0, 0);
  XSetForeground(cdpy, ccgc, BlackPixel(cdpy,cscr));
  XFillRectangle(cdpy, cbamp, ccgc, 0, 0, xwin, ywin);
  XFlush(cdpy);
}

draw_board(board)
     int *board;
{
  int occ,x,y;

  if (currdpy != dpy && !botmode)
    return;
  wipemap(currdpy, currbamp, currgc, currscr, currwin);
  for (y=0;y!=ysize;y++)
    for (x=0;x!=xsize;x++)
      if ((occ=board[x*ysize+y])!=EMPTY) {
	if (occ==WHITE)
	  draw_circle(x,y,WHITE);
	else  {
	  if (occ==BLACK)
	    draw_circle(x,y,BLACK);
	  else
	    draw_obstacle(x,y);
	}
      }
  show_scores(currdpy, currbamp, currgc, oldw, oldb);
  XFlush(currdpy);
}

show_scores(sdpy, swin, sgc, ws, bs)
Display *sdpy;
Window swin;
GC sgc;
int ws, bs;
{
  char mess[100];

  sprintf(mess,"White: %d",ws);
  XDrawString(sdpy,swin,sgc,5,ysize*Y_PIECE_SIZE+20,mess,strlen(mess));
  sprintf(mess,"Black: %d",bs);
  XDrawString(sdpy,swin,sgc,xsize*X_PIECE_SIZE-55,ysize*Y_PIECE_SIZE+20,mess,strlen(mess));
  XFlush(sdpy);
}

popupbutton(button, x, y)
unsigned int button;
int *x, *y;
{
  XEvent evt;
  Bool qqx = True;

  if (currdpy == dpy)
	XDefineCursor(dpy,win,downc);
  else
	XDefineCursor(odpy,owin,odownc);
  while(qqx) {
	while(!XCheckTypedWindowEvent(currdpy, currwin, (ButtonPress|ButtonRelease), &evt));	
	  if (evt.type == ButtonRelease)
	    if(evt.xbutton.button == button)
	      qqx = False;
  }
  *x = evt.xbutton.x;
  *y = evt.xbutton.y;
  if (currdpy == dpy)
	XDefineCursor(dpy,win,upc);
  else
	XDefineCursor(odpy,owin,oupc);
}

#define SELWID 5

piece_sel(x, y, display, win, cgc)
int x,y;
Display *display;
Window *win;
GC cgc;
{
  int qx1, qx2, qy1, qy2;
  int qq;
  GC pgc;
  XGCValues gcvalues;

  gcvalues.function = GXcopy; /*GXinvert*/
  pgc = XCreateGC(display,win,GCFunction,&gcvalues);
  XCopyGC(display,cgc,GCForeground|GCBackground,pgc);
  qx1 = x*X_PIECE_SIZE;
  qy1 = y*Y_PIECE_SIZE;
  qx2 = qx1 + X_PIECE_SIZE;
  qy2 = qy1 + Y_PIECE_SIZE;

  XDrawLine(display, win, pgc, qx1, qy1, qx1+SELWID, qy1);
  XDrawLine(display, win, pgc, qx1, qy1, qx1, qy1+SELWID);
  XDrawLine(display, win, pgc, qx2, qy1, qx2, qy1+SELWID);
  XDrawLine(display, win, pgc, qx1, qy2, qx1+SELWID, qy2);
  XDrawLine(display, win, pgc, qx1, qy2, qx1, qy2-SELWID);
  XDrawLine(display, win, pgc, qx2, qy1, qx2-SELWID, qy1);
  XDrawLine(display, win, pgc, qx2, qy2, qx2, qy2-SELWID);
  XDrawLine(display, win, pgc, qx2, qy2, qx2-SELWID, qy2);
  XFlush(display);
}

inboard(bx, by) /* true if x,y is in the board */
int bx, by;
{
  if (bx >= return_x_size() || by >= return_y_size())
	return(0);
  else
    return(1);
}

play_loop()
{
  XEvent event;
  int currx,curry,cx1,cy1,cx2,cy2,cx3,cy3,down=0,readfds,white,black,x,y;
  char mess[100],c;

  curr_player = WHITE;
  srandom(time(NULL));
  x = return_x_size();
  y = return_y_size();
  place_piece(0,0,WHITE);
  place_piece(x-1,y-1,WHITE);
  place_piece(0,y-1,BLACK);
  place_piece(x-1,0,BLACK);
  obstacle_color = curr_player;
  XNextEvent(dpy,&event);
  redraw_all(curr_player);
  if (botmode) {
    XNextEvent(odpy,&event);
    redraw_all(curr_player);
  }

  while (1) {
    while(!end_game()) {
      select_player(curr_player);
      if (!down) {
        cx1= -1; cy1= -1; cx2= -1; cy2= -1;
      }
      XNextEvent(currdpy,&event);
      switch(event.type) {
      case ButtonPress:
	currx = event.xbutton.x/X_PIECE_SIZE;
	curry = event.xbutton.y/Y_PIECE_SIZE;
	if (event.xbutton.button == 1)
		popupbutton(event.xbutton.button, &cx3, &cy3);
	else
		break;
	cx3 = cx3/X_PIECE_SIZE;
	cy3 = cy3/Y_PIECE_SIZE;	   
	if (cx3 == currx && cy3 == curry && inboard(cx3,cy3)) {
	  if (cx1 >= 0) {
            if(!b_peek(cx3, cy3)) {
	      cx2 = cx3;
              cy2 = cy3;
	      if(!move_piece(cx1,cy1,cx2,cy2,curr_player)) {
  	        if (botmode) {
	          curr_player = (curr_player==WHITE) ? BLACK : WHITE;
	          if (!valid_move(curr_player))
		    curr_player = (curr_player==WHITE) ? BLACK : WHITE;
	        }
	        else {
		  if (valid_move(BLACK))
	            obstacle_color = BLACK;
		  redraw_all(WHITE);
	          redraw_win(currdpy, currwin, currbamp, currgc);
                  if (valid_move(BLACK) && botlevel < 2)
		    msec_wait(557567);
		  robot_move(BLACK);
                  flumping = False;
	            while(!valid_move(WHITE)&&valid_move(BLACK)) {
		      redraw_all(WHITE);
		      orobot_move(BLACK);
	            }
		  flumping = True;
	        }
              } /* matches if(!move_piece(cx1,cy1,cx2,cy2,curr_player))  */
	      obstacle_color = curr_player;
	      down = 0;
	      redraw_all(curr_player);
            }
	    else {
	      if (b_peek(cx3,cy3) == curr_player && inboard(cx3,cy3)) {
		if(cx3 == cx1 && cy3 == cy1) {
		  redraw_all(curr_player);
		  cx1 = -1; cy1 = -1;
		  down=0;
		}
		else {
		  if (inboard(cx3,cy3)) {
		    piece_sel(cx1,cy1, currdpy, currwin, currgc);
		    cx1 = cx3; cy1 = cy3;
			redraw_all(curr_player);
		    piece_sel(cx1,cy1, currdpy, currwin, currgc);
                  }
		}
	      }
            }
          } /* matches if (cx1 >= 0)  */
	  else {
	    if(b_peek(currx, curry) == curr_player) {
		cx1 = cx3;
		cy1 = cy3;
		piece_sel(cx1,cy1, currdpy, currwin, currgc);
		down = 1;
	    }
	  }
	}
        flumping = False;
	while((!count_board(BLACK)) && valid_move(WHITE)) {
	  orobot_move(WHITE);
	  redraw_all(WHITE);
	}
	while((!count_board(WHITE)) && valid_move(BLACK)) {
	  orobot_move(BLACK);
	  redraw_all(BLACK);
	}
        flumping = True;
	break;
      case Expose:
	redraw_win(currdpy, currwin, currbamp, currgc);
	break;
      case KeyPress:
	XLookupString(&event, &c, 1, NULL, NULL);
	if (c == 'q')
		quit();
	break;
      default:
	break;
      } /* matches switch(event.type) { */
    } /* matches while(!end_game()) { */
    white = count_board(WHITE);
    black = count_board(BLACK);
    oldw = white;
    oldb = black;
    if (white > black)
      sprintf(mess,"White is the winner.");
    else {
      if (black > white)
	sprintf(mess,"Black is the winner.");
      else
	sprintf(mess,"The game is a tie.");
    }
  
    select_player(WHITE);
    XDrawString(currdpy,currwin,currgc,5,ysize*Y_PIECE_SIZE+35,mess,strlen(mess));
    XFlush(currdpy);
    if (botmode) {
      select_player(BLACK);
      XDrawString(currdpy,currwin,currgc,5,ysize*Y_PIECE_SIZE+35,mess,strlen(mess));
      XFlush(currdpy);
    }
    XNextEvent(dpy,&event);
    return;
  }
}

init_pieces()
{
  int curr_player=WHITE,x,y;

  /* Place initial pieces */
  x = return_x_size();
  y = return_y_size();
  clean_board();
  place_piece(0,0,WHITE);
  place_piece(x-1,y-1,WHITE);
  place_piece(0,y-1,BLACK);
  place_piece(x-1,0,BLACK);
  curr_player = WHITE;
  obstacle_color = curr_player;
  redraw_all(curr_player);
}

redraw_all(curr_player)
{
  select_player(WHITE);
  oldw = count_board(WHITE);
  oldb = count_board(BLACK);
  draw_board(return_board());
  redraw_win(currdpy, currwin, currbamp, currgc);
  if (botmode) {
    select_player(BLACK);
    draw_board(return_board());
    redraw_win(currdpy, currwin, currbamp, currgc);
  }
  select_player(curr_player);
}  

select_player(curr_player)
     int curr_player;
{
  if (curr_player==WHITE) {
    currdpy = dpy;
    currwin = win;
    currgc = gc;
    currbamp = bamp;
    currscr = scr;
  }
  else {
    currdpy = odpy;
    currwin = owin;
    currgc = ogc;
    currbamp = obamp;
    currscr = oscr;
  }
}




