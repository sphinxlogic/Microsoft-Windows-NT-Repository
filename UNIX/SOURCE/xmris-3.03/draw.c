/* Copyright (C) 1992 Nathan Sidwell */
#include "xmris.h"
#include "time.h"
/*{{{  prototypes*/
static VOIDFUNC draw_ball PROTOARG((unsigned));
static VOIDFUNC plot_ball PROTOARG((unsigned, int, int));
/*}}}*/
/*{{{  void add_background(x, y, w, h)*/
extern VOIDFUNC add_background
FUNCARG((x, y, width, height),
	int       x
ARGSEP  int       y
ARGSEP  unsigned  width
ARGSEP  unsigned  height
)
/*
 * adds an area to the background update list
 */
{
  BACKGROUND *bptr;

  assert(update.back.backs != BACK_UPDATES);
  bptr = &update.back.list[update.back.backs++];
  bptr->place.x = x;
  bptr->place.y = y;
  bptr->size.x = width;
  bptr->size.y = height;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  ANIMATE animate_zoom(next)*/
extern FUNCANIMATE(animate_zoom, next)
/*
 * zooms out on the initial board
 * and then return from whence I came
 */
{
  static PROTOANIMATE((*rts));
  static unsigned demo;
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_zoom;
  if(next)
    /*{{{  start*/
    {
      rts = (PROTOANIMATE((*)))next;
      XFillRectangle(display.display, display.copy, GCN(GC_CLEAR),
	  BORDER_LEFT + 1, BORDER_TOP + 1, BOARD_WIDTH - 2, BOARD_HEIGHT - 2);
      global.missed = 0;
      refresh_window();
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  BORDER_LEFT + 1, BORDER_TOP + 1, BOARD_WIDTH - 2, BOARD_HEIGHT - 2,
	  BORDER_LEFT + 1, BORDER_TOP + 1);
      /*{{{  do the apple sprites*/
      {
	int       i;
	APPLE     *aptr;
	    
	for(aptr = apple.list, i = apple.apples; i--; aptr++)
	  {
	    SPRITE    *sptr;
	    
	    sptr = &sprites[SPRITE_APPLE];
	    XCopyArea(display.display, sptr->mask, display.copy, GCN(GC_MASK),
		0, 0, CELL_WIDTH, CELL_HEIGHT, aptr->pixel.x, aptr->pixel.y);
	    XCopyArea(display.display, sptr->image, display.copy, GCN(GC_OR),
		0, 0, CELL_WIDTH, CELL_HEIGHT, aptr->pixel.x, aptr->pixel.y);
	  }
      }
      /*}}}*/
      set_colors(1);
      demo = global.state == MODE_DEMO;
      timer_set(ZOOM_RATE, TIMING_PAUSE);
      global.count = 0;
      global.state = MODE_ZOOM;
    }
    /*}}}*/
  if(global.count >= BOARD_HEIGHT / (2 * ZOOM_Y) || global.throw == 1 ||
      (demo && (global.pause ||
      global.pressed & (1 << KEY_QUIT | 1 << KEY_KEYBOARD))))
    /*{{{  end*/
    {
      if(!demo && global.count < BOARD_HEIGHT / (2 * ZOOM_Y))
	XCopyArea(display.display, display.copy, display.window,
	    GCN(GC_COPY), BORDER_LEFT + 1, BORDER_TOP + 1,
	    BOARD_WIDTH - 2, BOARD_HEIGHT - 2,
	    BORDER_LEFT + 1, BORDER_TOP + 1);
      if(!demo && global.throw == 1)
	global.throw = 2;
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      unsigned  x, y;
      
      x = global.count * ZOOM_X;
      y = global.count * ZOOM_Y;
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  CENTER_X - ZOOM_X - (int)x, CENTER_Y - ZOOM_Y - (int)y,
	  ZOOM_X, 2 * y + 2 * ZOOM_Y,
	  CENTER_X - ZOOM_X - (int)x, CENTER_Y - ZOOM_Y - (int)y);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  CENTER_X + (int)x, CENTER_Y - ZOOM_Y - (int)y,
	  ZOOM_X, 2 * y + 2 * ZOOM_Y,
	  CENTER_X + (int)x, CENTER_Y - ZOOM_Y - (int)y);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  CENTER_X - (int)x, CENTER_Y - ZOOM_Y - (int)y,
	  2 * x, ZOOM_Y,
	  CENTER_X - (int)x, CENTER_Y - ZOOM_Y - (int)y);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  CENTER_X - (int)x, CENTER_Y + (int)y,
	  2 * x, ZOOM_Y,
	  CENTER_X - (int)x, CENTER_Y + (int)y);
      global.count++;
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  void blank_board(map)*/
extern VOIDFUNC blank_board
FUNCARG((map),
	BOARD const *map
)
/*
 * sets up a blank board
 */
{
  /*{{{  clear board array*/
  {
    unsigned  i;
    CELL      *ptr;
	
    for(i = sizeof(board) / sizeof(board[0]), ptr = board; i--; ptr++)
      {
	ptr->depths[0] = 0;
	ptr->depths[1] = 0;
	ptr->depths[2] = 0;
	ptr->depths[3] = 0;
	ptr->holes[0] = 0;
	ptr->holes[1] = 0;
	ptr->holes[2] = 0;
	ptr->holes[3] = 0;
	ptr->visit = 0;
	ptr->distance = 0;
	ptr->ball = 0;
	ptr->xtra = 0;
	ptr->sprite = 0;
	ptr->apple = 0;
      }
  }
  /*}}}*/
  /*{{{  draw blank background*/
  {
    XGCValues gcv;
  
    XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    /*{{{  color?*/
    if(data.mono != True)
      {
	backgrounds[0] = map->colors[0];
	backgrounds[1] = map->colors[1];
	gcv.fill_style = FillSolid;
	gcv.foreground = colors[display.dynamic ? 0 : backgrounds[0]].pixel;
	XChangeGC(display.display, GCN(GC_BOARD),
	    GCFillStyle | GCForeground, &gcv);
	XFillRectangle(display.display, display.back, GCN(GC_BOARD),
	    BORDER_LEFT, BORDER_TOP, BOARD_WIDTH, BOARD_HEIGHT);
      }
    /*}}}*/
    /*{{{  draw stipple*/
    {
      gcv.fill_style = FillStippled;
      gcv.stipple = fills[map->fill].mask;
      gcv.foreground = data.mono != False ?
	  display.black : colors[display.dynamic ? 1 : backgrounds[1]].pixel;
      XChangeGC(display.display, GCN(GC_BOARD),
	  GCStipple | GCFillStyle | GCForeground, &gcv);
      XFillRectangle(display.display, display.back, GCN(GC_BOARD),
	  BORDER_LEFT, BORDER_TOP, BOARD_WIDTH, BOARD_HEIGHT);
    }
    /*}}}*/
    XDrawLine(display.display, display.back, GCN(GC_SET),
	BORDER_LEFT, BORDER_TOP, BORDER_LEFT + BOARD_WIDTH, BORDER_TOP);
    XDrawLine(display.display, display.back, GCN(GC_SET),
	BORDER_LEFT, BORDER_TOP + BOARD_HEIGHT - 1,
	BORDER_LEFT + BOARD_WIDTH, BORDER_TOP + BOARD_HEIGHT - 1);
    XDrawLine(display.display, display.back, GCN(GC_SET),
	PIXELX(4, -1), PIXELY(-1, 0), PIXELX(4, -1), PIXELY(-1, CELL_HEIGHT));
    XDrawLine(display.display, display.back, GCN(GC_SET),
	PIXELX(4, XTRA_SPACING * 4 + CELL_WIDTH), PIXELY(-1, 0),
	PIXELX(4, XTRA_SPACING * 4 + CELL_WIDTH), PIXELY(-1, CELL_HEIGHT));
  }
  /*}}}*/
  /*{{{  add the xtra*/
  {
    unsigned  i;
	
    extra.escape = 0;
    for(i = 5; i--;)
      draw_extra_letter(i);
  }
  /*}}}*/
  /*{{{  initialize stuff*/
  {
    global.cherries = 5 * 8;
    update.back.backs = 0;
    update.score.scores = 0;
    player.old_ball.state = 0;
    player.old_ball.count = 8;
    player.ball.state = 0;
    player.ball.count = 8;
    monster.monsters = 0;
    apple.apples = 0;
  }
  /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void bounding_box(x, y, width, height)*/
extern VOIDFUNC bounding_box
FUNCARG((x, y, width, height),
	int       x
ARGSEP  int       y
ARGSEP  unsigned  width
ARGSEP  unsigned  height
)
/*
 * updates the update box so that it includes the
 * supplied rectangle
 * remember to empty the update box to the background list
 */
{
  if(!update.set)
    {
      update.br.x = (update.tl.x = x) + width;
      update.br.y = (update.tl.y = y) + height;
      update.set = 1;
    }
  else
    {
      if(update.tl.x > x)
	update.tl.x = x;
      if(update.tl.y > y)
	update.tl.y = y;
      if(update.br.x < (int)(x + width))
	update.br.x = (int)(x + width);
      if(update.br.y < (int)(y + height))
	update.br.y = (int)(y + height);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void create_xtra_monster(ix)*/
extern VOIDFUNC create_xtra_monster
FUNCARG((ix),
	unsigned  ix
)
/*
 * create an xtra monster sprite, by taking the virgin one, and
 * slapping a letter on its chest
 */
{
  SPRITE    *dptr;
  SPRITE    *sptr;
  SPRITE    *lptr;
  unsigned  i;
  
  sptr = &sprites[SPRITE_XTRA];
  lptr = &sprites[SPRITE_EXTRA];
  for(dptr = &sprites[SPRITE_XTRA], sptr = &sprites[SPRITE_XTRA_SOURCE],
      i = MONSTER_IMAGES; i--; dptr++, sptr++)
    {
      XCopyArea(display.display, sptr->image, dptr->image, GCN(GC_COPY),
	  0, 0, CELL_WIDTH, CELL_HEIGHT, 0, 0);
      XCopyArea(display.display, lptr->mask, dptr->image, GCN(GC_MASK),
	  (int)ix * (CELL_WIDTH / 2), 0, CELL_WIDTH / 2, CELL_HEIGHT / 2,
	  XTRA_LETTER_X, XTRA_LETTER_Y);
      XCopyArea(display.display, lptr[!(extra.got & 1 << ix)].image,
	  dptr->image, GCN(GC_OR),
	  (int)ix * (CELL_WIDTH / 2), 0, CELL_WIDTH / 2, CELL_HEIGHT / 2,
	  XTRA_LETTER_X, XTRA_LETTER_Y);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void draw_ball(flag)*/
static VOIDFUNC draw_ball
FUNCARG((flag),
	unsigned  flag
)
/*
 * draws the old ball on or off the window
 */
{
  switch(player.old_ball.state)
  {
    /*{{{  case 0:*/
    case 0:
    {
      if(player.old_ball.count < 8)
	{
	  COORD const *hold;
	   
	  hold = &ball_hold[player.old_ball.count * MONSTER_IMAGES +
	      player.old_ball.image];
	  plot_ball(flag, player.old_ball.pixel.x + hold->x,
	      player.old_ball.pixel.y + hold->y);
	}
      break;
    }
    /*}}}*/
    /*{{{  case 1:*/
    case 1:
    {
      plot_ball(flag, player.old_ball.pixel.x, player.old_ball.pixel.y);
      break;
    }
    /*}}}*/
    /*{{{  case 2: case 4:*/
    case 2: case 4:
    {
      COORD     offset;
      COORD     pixel;
      unsigned  bits;
	
      pixel.x = player.old_ball.pixel.x;
      pixel.y = player.old_ball.pixel.y;
      if(player.old_ball.count)
	{
	  offset.x = player.old_ball.count * BALL_EX;
	  offset.y = player.old_ball.count * BALL_EY;
	  bits = 0xFF;
	  /*{{{  set clips*/
	  {
	    if(pixel.x < offset.x)
	      bits &= 0xF8;
	    if(pixel.y < offset.y)
	      bits &= 0x3E;
	    if(pixel.x + offset.x > BOARD_WIDTH + BALL_WIDTH)
	      bits &= 0x8F;
	    if(pixel.y + offset.y > BOARD_HEIGHT + BALL_HEIGHT)
	      bits &= 0xE3;
	  }
	  /*}}}*/
	  /*{{{  do inner bits*/
	  {
	    if(bits & 0x01)
	      plot_ball(flag, pixel.x - offset.x, pixel.y - offset.y);
	    if(bits & 0x02)
	      plot_ball(flag, pixel.x - offset.x, pixel.y);
	    if(bits & 0x04)
	      plot_ball(flag, pixel.x - offset.x, pixel.y + offset.y);
	    if(bits & 0x08)
	      plot_ball(flag, pixel.x, pixel.y + offset.y);
	    if(bits & 0x10)
	      plot_ball(flag, pixel.x + offset.x, pixel.y + offset.y);
	    if(bits & 0x20)
	      plot_ball(flag, pixel.x + offset.x, pixel.y);
	    if(bits & 0x40)
	      plot_ball(flag, pixel.x + offset.x, pixel.y - offset.y);
	    if(bits & 0x80)
	      plot_ball(flag, pixel.x, pixel.y - offset.y);
	  }
	  /*}}}*/
	  offset.x *= 2;
	  offset.y *= 2;
	  /*{{{  set clips*/
	  {
	    if(pixel.x < offset.x)
	      bits &= 0xF8;
	    if(pixel.y < offset.y)
	      bits &= 0x3E;
	    if(pixel.x + offset.x > BOARD_WIDTH + BALL_WIDTH)
	      bits &= 0x8F;
	    if(pixel.y + offset.y > BOARD_HEIGHT + BALL_HEIGHT)
	      bits &= 0xE3;
	  }
	  /*}}}*/
	  /*{{{  do inner bits*/
	  {
	    if(bits & 0x01)
	      plot_ball(flag, pixel.x - offset.x, pixel.y - offset.y);
	    if(bits & 0x02)
	      plot_ball(flag, pixel.x - offset.x, pixel.y);
	    if(bits & 0x04)
	      plot_ball(flag, pixel.x - offset.x, pixel.y + offset.y);
	    if(bits & 0x08)
	      plot_ball(flag, pixel.x, pixel.y + offset.y);
	    if(bits & 0x10)
	      plot_ball(flag, pixel.x + offset.x, pixel.y + offset.y);
	    if(bits & 0x20)
	      plot_ball(flag, pixel.x + offset.x, pixel.y);
	    if(bits & 0x40)
	      plot_ball(flag, pixel.x + offset.x, pixel.y - offset.y);
	    if(bits & 0x80)
	      plot_ball(flag, pixel.x, pixel.y - offset.y);
	  }
	  /*}}}*/
	}
      else
	plot_ball(flag, pixel.x, pixel.y);
      break;
    }
    /*}}}*/
    /*{{{  case 3:*/
    case 3:
      break;
    /*}}}*/
  }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void draw_center(ix)*/
extern VOIDFUNC draw_center
FUNCARG((ix),
	unsigned  ix
)
/*
 * sets the center sprite and draws it on the background
 */
{
  BOARDCELL(DEN_X, DEN_Y)->sprite = ix;
  add_background(PIXELX(DEN_X, 0), PIXELY(DEN_Y, 0), CELL_WIDTH, CELL_HEIGHT);
  XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
      PIXELX(DEN_X, 0), PIXELY(DEN_Y, 0), CELL_WIDTH, CELL_HEIGHT);
  back_sprite(ix, 0, PIXELX(DEN_X, 0), PIXELY(DEN_Y, 0));
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void draw_extra()*/
extern VOIDFUNC draw_extra FUNCARGVOID
/*
 * whack the current extra monster onto the background
 */
{
  int       x;
  
  x = PIXELX(4, extra.select * XTRA_SPACING);
  back_sprite(SPRITE_XTRA + (chaotic() & 1), 1, x, PIXELY(-1, 0));
  add_background(x, PIXELY(-1, 0), CELL_WIDTH, CELL_HEIGHT);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void draw_extra_letter(ix)*/
extern VOIDFUNC draw_extra_letter
FUNCARG((ix),
	unsigned  ix
)
/*
 * draw one of the extra letters on the background
 * and add it to the update list
 */
{
  SPRITE    *lptr;
  int       x;
  
  lptr = &sprites[SPRITE_EXTRA];
  x = PIXELX(4, ix * XTRA_SPACING);
  XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
      x, PIXELY(-1, 0), CELL_WIDTH, CELL_HEIGHT);
  if(display.background != COLOUR_ZERO)
    XCopyArea(display.display, lptr->mask,
	display.back, GCN(GC_MASK), (int)ix * (CELL_WIDTH / 2), 0,
	CELL_WIDTH / 2, CELL_HEIGHT / 2,
	x + XTRA_LETTER_X, PIXELY(-1, XTRA_LETTER_Y));
  XCopyArea(display.display, lptr[!(extra.got & 1 << ix)].image,
      display.back, GCN(GC_OR), (int)ix * (CELL_WIDTH / 2), 0,
      CELL_WIDTH / 2, CELL_HEIGHT / 2,
      x + XTRA_LETTER_X, PIXELY(-1, XTRA_LETTER_Y));
  add_background(x, PIXELY(-1, 0), CELL_WIDTH, CELL_HEIGHT);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void new_board()*/
extern VOIDFUNC new_board FUNCARGVOID
/*
 * sets up a new board
 */
{
  BOARD const *map;

  map = &boards[global.screen % BOARDS];
  blank_board(map);
  XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
      PIXELX(DEN_X, -GAP_WIDTH / 2), PIXELY(DEN_Y, -GAP_HEIGHT / 2),
      CELL_WIDTH + GAP_WIDTH, CELL_HEIGHT + GAP_HEIGHT);
  XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
      BORDER_LEFT + (CELL_WIDTH + GAP_WIDTH) * 4 + GAP_WIDTH, BORDER_TOP + 1,
      XTRA_SPACING * 4 + CELL_WIDTH, GAP_HEIGHT * 2 - 1);
  draw_extra();
  /*{{{  add the screen number*/
  {
    char      text[10];
    int       length;
	
    strcpy(text, "Screen ");
    length = 7 + itoa(text + 7, (unsigned long)global.screen, 0);
    XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	PIXELX(8, 0) + (int)font.width * 2,
	PIXELY(-1, CELL_HEIGHT / 2) + font.center, text, length);
  }
  /*}}}*/
  add_score(0, 0, 0);
  /*{{{  add lives*/
  if(global.lives)
    {
      unsigned  lives;
	
      for(lives = global.lives - 1; lives--;)
	back_sprite(SPRITE_PLAYER + 6, 0,
	    PIXELX((int)lives, 0), PIXELY(CELLS_DOWN, 0));
    }
  /*}}}*/
  /*{{{  copy the map*/
  {
    char const *mptr;
    CELL      *cptr;
    unsigned  y, x;
    unsigned  mask;
	
    apple.apples = 0;
    mask = 1 << (chaotic() % 4);
    mptr = (char const *)map->map;
    cptr = BOARDCELL(0, 0);
    for(y = CELLS_DOWN; y--; cptr += CELL_STRIDE - CELLS_ACROSS, mptr++)
      for(x = CELLS_ACROSS; x--; mptr++, cptr++)
	{
	  switch(*mptr)
	  {
	    /*{{{  case '@': (cherry)*/
	    case '@':
	      cptr->sprite = SPRITE_CHERRY;
	      break;
	    /*}}}*/
	    /*{{{  case 'X': (path)*/
	    case 'X':
	      cptr[0].visit = 1;
	      if(cptr[-1].visit)
		{
		  cptr[-1].depths[3] = CELL_WIDTH + GAP_WIDTH;
		  cptr[0].depths[2] = -(CELL_WIDTH + GAP_WIDTH);
		}
	      if(cptr[-CELL_STRIDE].visit)
		{
		  cptr[-CELL_STRIDE].depths[1] = CELL_HEIGHT + GAP_HEIGHT;
		  cptr[0].depths[0] = -(CELL_HEIGHT + GAP_HEIGHT);
		}
	      break;
	    /*}}}*/
	    /*{{{  case '.': (nothing)*/
	    case '.':
	      break;
	    /*}}}*/
	    /*{{{  default: (apple)*/
	    default:
	    {
	      char const *ptr;
	    
	      ptr = strchr(hex_text, *mptr);
	      assert(ptr);
	      if(mask & (ptr - hex_text))
		spawn_apple((int)(CELLS_ACROSS - 1 - x),
		    (int)(CELLS_DOWN - 1 - y), 0, 0);
	      break;
	    }
	    /*}}}*/
	  }
	}
  }
  /*}}}*/
#ifdef RANDOM_APPLES
  /*{{{  add the apples*/
  {
    unsigned  i;
	
    apple.apples = 0;
    for(i = INITIAL_APPLES; i--;)
      {
	int       y, x;
	unsigned  j;
	CELL      *cptr;
	
	do
	  {
	    do
	      j = chaotic();
	    while(j >= CELLS_ACROSS * (CELLS_DOWN - 1));
	    x = j % CELLS_ACROSS;
	    y = j / CELLS_ACROSS;
	    cptr = BOARDCELL(x, y);
	  }
	while(cptr->visit || cptr->sprite || cptr[CELL_STRIDE].visit ||
	    cptr->apple || cptr[1].apple || cptr[-1].apple ||
	    cptr[CELL_STRIDE].apple || cptr[-CELL_STRIDE].apple);
	cptr->apple = 1;
	spawn_apple(x, y, 0, 0);
      }
  }
  /*}}}*/
#endif /* RANDOM_APPLES */
  /*{{{  cut the background*/
  {
    unsigned  y, x;
    COORD     cell;
    CELL      *cptr;
	
    cptr = BOARDCELL(0, 0);
    cell.x = PIXELX(0, 0);
    cell.y = PIXELY(0, 0);
    for(y = CELLS_DOWN; y--; cptr += CELL_STRIDE - CELLS_ACROSS,
	cell.x -= (CELL_WIDTH + GAP_WIDTH) * CELLS_ACROSS,
	cell.y += CELL_HEIGHT + GAP_HEIGHT)
      for(x = CELLS_ACROSS; x--; cptr++, cell.x += CELL_WIDTH + GAP_WIDTH)
	if(cptr->visit)
	  munch_hole(cptr, cell.x, cell.y);
	else if(cptr->sprite)
	  back_sprite(cptr->sprite, 1, cell.x, cell.y);
  }
  /*}}}*/
  XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
      0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void plot_ball(flag, x, y)*/
static VOIDFUNC plot_ball
FUNCARG((flag, x, y),
	unsigned  flag
ARGSEP  int       x
ARGSEP  int       y
)
/*
 * plots the ball centered on the given coordinate
 * flag is 0 for unplot, 1 for plot
 */
{
  if(flag)
    {
      XCopyArea(display.display, sprites[SPRITE_BALL].mask, display.copy,
	  GCN(GC_MASK), 0, 0, BALL_WIDTH, BALL_HEIGHT,
	  x - BALL_WIDTH / 2, y - BALL_HEIGHT / 2);
      XCopyArea(display.display, sprites[SPRITE_BALL].image, display.copy,
	  GCN(GC_OR), 0, 0, BALL_WIDTH, BALL_HEIGHT,
	  x - BALL_WIDTH / 2, y - BALL_HEIGHT / 2);
    }
  add_background(x - BALL_WIDTH / 2, y - BALL_HEIGHT / 2,
      BALL_WIDTH, BALL_HEIGHT);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void refresh_window()*/
extern VOIDFUNC refresh_window FUNCARGVOID
/*
 * refreshes the display window
 */
{
  if(global.state == MODE_ZOOM)
    {
      unsigned  x, y;
      
      x = global.count * ZOOM_X;
      y = global.count * ZOOM_Y;
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  0, 0, WINDOW_WIDTH, BORDER_TOP + 1, 0, 0);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  0, BORDER_TOP + BOARD_HEIGHT - 1, WINDOW_WIDTH, BORDER_BOTTOM + 1,
	  0, BORDER_TOP + BOARD_HEIGHT - 1);
      if(BORDER_LEFT > 0)
	XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	    0, BORDER_TOP, BORDER_LEFT + 1, BOARD_HEIGHT, 0, BORDER_TOP);
      if(BORDER_RIGHT > 0)
	XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	    BORDER_LEFT + BOARD_WIDTH - 1, BORDER_TOP, BORDER_RIGHT + 1,
	    BOARD_HEIGHT, BORDER_LEFT + BOARD_WIDTH - 1, BORDER_TOP);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  CENTER_X - (int)x, CENTER_Y - (int)y, 2 * x, 2 * y,
	  CENTER_X - (int)x, CENTER_Y - (int)y);
    }
  else
    XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
  if(global.missed)
    XDrawLine(display.display, display.window, GCN(GC_LOAD),
	WINDOW_WIDTH - (int)global.missed,
	PIXELY(CELLS_DOWN, CELL_HEIGHT),
	WINDOW_WIDTH, PIXELY(CELLS_DOWN, CELL_HEIGHT));
  if(global.dilation != FRAME_SCALE)
    XDrawLine(display.display, display.window, GCN(GC_LOAD),
	0, PIXELY(CELLS_DOWN, CELL_HEIGHT),
	WINDOW_WIDTH - (int)((unsigned long)WINDOW_WIDTH * FRAME_SCALE /
	    global.dilation), PIXELY(CELLS_DOWN, CELL_HEIGHT));
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void show_updates()*/
extern VOIDFUNC show_updates FUNCARGVOID
/*
 * shows all the updates on the update list
 * and delete the dead apples and monsters.
 * remember to keep the monster lists intact.
 * the monsters a drawn in reverse order, so that the player
 * ends up on top of everybody.
 * We must be careful about the order of the update, so that there
 * isn't excessive flicker. That's why we have a copy window to do the
 * updates first, and then copy the relevant areas onto the display window
 */
{
  draw_ball(0);
  /*{{{  backgrounds to copy*/
  {
    unsigned  i;
    BACKGROUND *bptr;
	
    for(bptr = update.back.list, i = update.back.backs; i--; bptr++)
      XCopyArea(display.display, display.back, display.copy, GCN(GC_COPY),
	  bptr->place.x, bptr->place.y, bptr->size.x, bptr->size.y,
	  bptr->place.x, bptr->place.y);
  }
  /*}}}*/
  if(global.state != MODE_ZOOM)
    {
      /*{{{  do the monster backgrounds*/
      {
	int       i;
	MONSTER   *mptr;
	MONSTER   *nptr;
	    
	for(mptr = nptr = monster.list, i = monster.monsters; i--; mptr++)
	  {
	    int   new;
	    
	    update.set = 0;
	    if(mptr->type == 5)
	      new = 0;
	    else if(mptr->type == 6)
	      new = SPRITE_DIAMOND + mptr->image;
	    else if(mptr->type > 5)
	      new = mptr->type;
	    else if(mptr->chew)
	      new = SPRITE_CHOMP + mptr->image;
	    else if(mptr->face >= 16)
	      new = SPRITE_SQUISHED - 16 + mptr->type * 2 + mptr->face;
	    else
	      new = SPRITE_MONSTERS + mptr->type * (6 * MONSTER_IMAGES) +
		  mptr->face * MONSTER_IMAGES + mptr->image;
	    if(!mptr->back && mptr->old_sprite && (new != mptr->old_sprite ||
		mptr->pixel.x != mptr->old_pixel.x ||
		mptr->pixel.y != mptr->old_pixel.y ||
		(mptr == &monster.list[0] &&
		  !player.old_ball.state && player.ball.state)) &&
		mptr->on)
	      {
		XCopyArea(display.display, display.back, display.copy,
		    GCN(GC_COPY),
		    mptr->old_pixel.x, mptr->old_pixel.y,
		    CELL_WIDTH, CELL_HEIGHT,
		    mptr->old_pixel.x, mptr->old_pixel.y);
		if(mptr->pixel.x != mptr->old_pixel.x ||
		    mptr->pixel.y != mptr->old_pixel.y || !new)
		  bounding_box(mptr->old_pixel.x, mptr->old_pixel.y,
		      CELL_WIDTH, CELL_HEIGHT);
	      }
	    mptr->old_sprite = new;
	    if(new)
	      {
		mptr->old_pixel.x = mptr->pixel.x;
		mptr->old_pixel.y = mptr->pixel.y;
		mptr->on = INRANGE(mptr->pixel.x,
		    1 - CELL_WIDTH, WINDOW_WIDTH) &&
		    INRANGE(mptr->pixel.y, 1 - CELL_HEIGHT, WINDOW_HEIGHT);
		if(!mptr->back && mptr->on)
		  bounding_box(mptr->old_pixel.x, mptr->old_pixel.y,
		      CELL_WIDTH, CELL_HEIGHT);
		mptr->tptr = nptr;
		nptr++;
	      }
	    else
	      mptr->tptr = NULL;
	    mptr->back = 0;
	    if(update.set)
	      add_background(update.tl.x, update.tl.y,
		  (unsigned)(update.br.x - update.tl.x),
		  (unsigned)(update.br.y - update.tl.y));
	  }
      }
      /*}}}*/
      /*{{{  do the apple backgrounds*/
      {
	int       i;
	APPLE     *aptr;
	APPLE     *dptr;
	    
	for(aptr = dptr = apple.list, i = apple.apples; i--; aptr++)
	  {
	    int   new;
	    
	    update.set = 0;
	    new = aptr->state;
	    if(!aptr->back && (new != aptr->old_state ||
		aptr->pixel.x != aptr->old_pixel.x ||
		aptr->pixel.y != aptr->old_pixel.y))
	      {
		APPLE_SIZE const *asp;
		int       x, y;
		unsigned  width, height;
	    
		asp = &apple_sizes[aptr->old_state];
		x = aptr->old_pixel.x + asp->offset.x;
		y = aptr->old_pixel.y + asp->offset.y;
		width = asp->size.x;
		height = asp->size.y;
		XCopyArea(display.display, display.back, display.copy,
		    GCN(GC_COPY), x, y, width, height, x, y);
		bounding_box(x, y, width, height);
	      }
	    if(new != 6)
	      {
		APPLE_SIZE const *asp;
		  
		aptr->old_pixel.x = aptr->pixel.x;
		aptr->old_pixel.y = aptr->pixel.y;
		aptr->old_state = new;
		asp = &apple_sizes[new];
		if(!aptr->back)
		  bounding_box(aptr->old_pixel.x + asp->offset.x,
		      aptr->old_pixel.y + asp->offset.y,
		      asp->size.x, asp->size.y);
		if(aptr->list)
		  aptr->list = aptr->list->tptr;
		aptr->back = 0;
		if(aptr != dptr)
		  memcpy(dptr, aptr, sizeof(APPLE));
		dptr++;
	      }
	    else
	      apple.apples--;
	    if(update.set)
	      add_background(update.tl.x, update.tl.y,
		  (unsigned)(update.br.x - update.tl.x),
		  (unsigned)(update.br.y - update.tl.y));
	  }
      }
      /*}}}*/
      /*{{{  do the apple sprites*/
      {
	int       i;
	APPLE     *aptr;
	    
	for(aptr = apple.list, i = apple.apples; i--; aptr++)
	  {
	    SPRITE    *sptr;
	    APPLE_SIZE const *asp;
	    int       x, y;
	    unsigned  width, height;
	    
	    asp = &apple_sizes[aptr->old_state];
	    sptr = &sprites[(aptr->ghost && aptr->old_state < 3 ?
		SPRITE_GHOST : SPRITE_APPLE) + aptr->old_state];
	    x = aptr->old_pixel.x + asp->offset.x;
	    y = aptr->old_pixel.y + asp->offset.y;
	    width = asp->size.x;
	    height = asp->size.y;
	    XCopyArea(display.display, sptr->mask, display.copy, GCN(GC_MASK),
		0, 0, width, height, x, y);
	    XCopyArea(display.display, sptr->image, display.copy, GCN(GC_OR),
		0, 0, width, height, x, y);
	  }
      }
      /*}}}*/
      /*{{{  do the monster sprites*/
      {
	int       i;
	MONSTER   *mptr;
	    
	for(mptr = &monster.list[monster.monsters - 1], i = monster.monsters;
	    i--; mptr--)
	  if(mptr->old_sprite)
	    {
	      if(mptr->on)
		{
		  SPRITE    *sptr;
	    
		  sptr = &sprites[mptr->old_sprite];
		  XCopyArea(display.display, sptr->mask, display.copy,
		      GCN(GC_MASK), 0, 0, CELL_WIDTH, CELL_HEIGHT,
		      mptr->old_pixel.x, mptr->old_pixel.y);
		  XCopyArea(display.display, sptr->image, display.copy,
		      GCN(GC_OR), 0, 0, CELL_WIDTH, CELL_HEIGHT,
		      mptr->old_pixel.x, mptr->old_pixel.y);
		}
	      if(mptr->list)
		mptr->list = mptr->list->tptr;
	    }
      }
      /*}}}*/
    }
  memcpy(&player.old_ball, &player.ball, sizeof(BALL));
  draw_ball(1);
  /*{{{  scores to copy*/
  {
    unsigned  i;
    SCORE     *sptr;
	
    for(sptr = update.score.list, i = update.score.scores; i--; sptr++)
      {
	XCopyArea(display.display, sptr->mask, display.copy, GCN(GC_MASK),
	    0, 0, DIGIT_WIDTH * 4, DIGIT_HEIGHT,
	    sptr->place.x, sptr->place.y);
	XCopyArea(display.display, sptr->image, display.copy, GCN(GC_OR),
	    0, 0, DIGIT_WIDTH * 4, DIGIT_HEIGHT,
	    sptr->place.x, sptr->place.y);
      }
  }
  /*}}}*/
  /*{{{  areas to window*/
  {
    unsigned  i;
    BACKGROUND *bptr;
	
    for(bptr = update.back.list, i = update.back.backs; i--; bptr++)
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  bptr->place.x, bptr->place.y, bptr->size.x, bptr->size.y,
	  bptr->place.x, bptr->place.y);
  }
  /*}}}*/
  /*{{{  scores to window*/
  {
    unsigned  i;
    SCORE     *sptr;
	
    for(sptr = update.score.list, i = update.score.scores; i--; sptr++)
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  sptr->place.x, sptr->place.y, DIGIT_WIDTH * 4, DIGIT_HEIGHT,
	  sptr->place.x, sptr->place.y);
  }
  /*}}}*/
  /*{{{  delete monsters*/
  {
    MONSTER   *mptr;
    unsigned  count;
    
    for(mptr = monster.list, count = monster.monsters; count--; mptr++)
      if(!mptr->old_sprite)
	monster.monsters--;
      else if(mptr != mptr->tptr)
	memcpy(mptr->tptr, mptr, sizeof(MONSTER));
      
  }
  /*}}}*/
  update.back.backs = 0;
  XSync(display.display, False);
  return VOIDRETURN;
}
/*}}}*/
