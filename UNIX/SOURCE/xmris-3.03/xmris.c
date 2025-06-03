/* Copyright (C) 1992 Nathan Sidwell */
/*{{{  the problem with recursive function definitions*/
/*
 * In order to keep a single control loop, I use animate functions,
 * which are called through a pointer. To chain these, the animate
 * functions return a pointer to the animate function to call next time
 * round. Uh oh, a function which returns a pointer to a function which
 * returns a pointer to ..., you get the idea. To make it worse, in order
 * to implement callback, the animate functions take a pointer to the
 * animate function to call back, when they're done. This gives the two
 * main loops (demo and game play), the ability to both call the same
 * animate functions, and have them return correctly.
 * We could wrap the function definition up in a structure, because we
 * can have self referential structures, but that introduces another
 * level of indirection 'cos passing structures is not K&R compatible.
 * So what I do is define the animate function as taking a pointer to
 * a void function returning void and returning a pointer to a void
 * function returning a void. Then all I have to do is _carefully_ cast
 * these things before using them.
 * And remember kiddies, ANSI says that sizeof(void *) maybe _smaller_
 * than sizeof(void (*)()), which is why I don't cast function pointers
 * into data pointers, 'cos they might not fit.
 * Some (supposidly ANSI) compilers are confused about the declaration
 * and definition of such functions. They only accept K&R style
 * declarations and definitions for them. (It is possible that I've got
 * them slightly wrong, with a superflous set of () some where, but gcc
 * takes it, and my depreciated cdecl, hacked out of K&R 2 agrees with
 * what I've got.) For these platforms, you can give the define
 * BROKEN_FPTRS, which will force K&R mode on just those cases where
 * it all goes sadly wrong. A notable case is codecenter (alias saberc),
 * which just doesn't want to know. (It also performs an addition level
 * of string stripping on the make file, so that the high score file name
 * has its quotes stripped off, but that's another story.)
 * May be I should just have a big switch statement, which will
 * be more robust, and easier on optimizers, but break the
 * object orientedness of the whole thing.
 */
/*}}}*/
#include "xmris.h"
#include <time.h>
#include <unistd.h>
/*{{{  prototypes*/
static VOIDFUNC age_scores PROTOARG((void));
static PROTOANIMATE(animate_death);
static PROTOANIMATE(animate_pause);
static VOIDFUNC calc_extra_home PROTOARG((void));
static VOIDFUNC process_xevents PROTOARG((void));
static int xio_error PROTOARG((Display *));
/*}}}*/
/*{{{  void add_score(increment, x, y)*/
extern VOIDFUNC add_score
FUNCARG((points, x, y),
	unsigned  points
ARGSEP  int       x
ARGSEP  int       y
)
/*
 * adds the given score (which may be zero)
 * and displays it at the top of the screen
 * if the coordinate > 0 then add the score into the onboard list
 */
{
  points = ((unsigned long)points * global.scale / SCORE_SCALE) * SCORE_ROUND;
  global.score += points;
  /*{{{  text score*/
  {
    size_t    length;
    char      text[10];
    int       x, y;
	
    length = itoa(text, global.score, 0);
    x = PIXELX(4, -GAP_WIDTH) - (length + 2) * font.width;
    y = PIXELY(-1, CELL_HEIGHT / 2) + font.center;
    XDrawImageString(display.display, display.back, GCN(GC_TEXT),
	x, y, text, length);
    add_background(x, y - font.ascent,
	length * font.width, (unsigned)(font.ascent + font.descent));
  }
  /*}}}*/
  /*{{{  board score?*/
  if(y)
    {
      size_t    length;
      char      text[10];
      int       i;
      SCORE     *sptr;
      SPRITE    *dptr;
	
      dptr = &sprites[SPRITE_DIGITS];
      length = itoa(text, (unsigned long)points, 0);
      /*{{{  remove oldest score?*/
      if(update.score.scores == BOARD_SCORES)
	{
	  add_background(update.score.list[0].place.x,
	      update.score.list[0].place.y,
	      DIGIT_WIDTH * 4, DIGIT_HEIGHT);
	  update.score.scores--;
	  for(sptr = update.score.list, i = update.score.scores; i--; sptr++)
	    memcpy(sptr, sptr + 1, sizeof(SCORE));
	}
      /*}}}*/
      sptr = &update.score.list[update.score.scores++];
      sptr->count = SCORE_SHOW;
      sptr->place.x = x - DIGIT_WIDTH * 2;
      sptr->place.y = y - DIGIT_HEIGHT / 2;
      /*{{{  centering*/
      if(length != 4)
	{
	  x = (4 - length) * (DIGIT_WIDTH / 2);
	  XCopyArea(display.display, dptr->image, sptr->image, GCN(GC_COPY),
	      10 * DIGIT_WIDTH, 0, (unsigned)x, DIGIT_HEIGHT, 0, 0);
	  XCopyArea(display.display, dptr->image, sptr->image, GCN(GC_COPY),
	      10 * DIGIT_WIDTH, 0, (unsigned)x,
	      DIGIT_HEIGHT, 4 * DIGIT_WIDTH - x, 0);
	  XCopyArea(display.display, dptr->mask, sptr->mask, GCN(GC_COPY),
	      10 * DIGIT_WIDTH, 0, (unsigned)x, DIGIT_HEIGHT, 0, 0);
	  XCopyArea(display.display, dptr->mask, sptr->mask, GCN(GC_COPY),
	      10 * DIGIT_WIDTH, 0, (unsigned)x,
	      DIGIT_HEIGHT, 4 * DIGIT_WIDTH - x, 0);
	}
      else
	x = 0;
      /*}}}*/
      for(i = 0; i < length; i++, x += DIGIT_WIDTH)
	{
	  XCopyArea(display.display, dptr->image, sptr->image, GCN(GC_COPY),
	      (text[i] - '0') * DIGIT_WIDTH, 0, DIGIT_WIDTH, DIGIT_HEIGHT,
	      x, 0);
	  XCopyArea(display.display, dptr->mask, sptr->mask, GCN(GC_COPY),
	      (text[i] - '0') * DIGIT_WIDTH, 0, DIGIT_WIDTH, DIGIT_HEIGHT,
	      x, 0);
	}
    }
  /*}}}*/
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void age_scores()*/
static VOIDFUNC age_scores FUNCARGVOID
/*
 * ages the onboard scores, and removes the old ones
 */
{
  SCORE     *sptr;
  int       i;

  for(sptr = update.score.list, i = update.score.scores; i--; sptr++)
    if(!sptr->count--)
      {
	Pixmap    image, mask;
	unsigned  ix;
	SCORE     *optr;
	
	add_background(sptr->place.x, sptr->place.y,
	    DIGIT_WIDTH * 4, DIGIT_HEIGHT);
	mask = sptr->mask;
	image = sptr->image;
	for(ix = i, optr = sptr; ix--; optr++)
	  memcpy(optr, optr + 1, sizeof(SCORE));
	optr->mask = mask;
	optr->image = image;
	update.score.scores--;
	sptr--;
     }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  ANIMATE animate_death(next)*/
static FUNCANIMATE(animate_death, next)
/*
 * spin the player around a bit, and then remove all the monsters and
 * return back to whoever called me
 */
{
  static PROTOANIMATE((*rts));
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_death;
  if(next)
    /*{{{  start*/
    {
      unsigned  i;
      unsigned  base;
    
      rts = (PROTOANIMATE((*)))next;
      player.ball.count = 8;
      i = monster.list[0].face;
      if(i >= 6)
	i = 2 + (i & 1);
      for(base = 8; base--;)
	if(player_dies[base] == i)
	  break;
      global.count = 0;
      monster.list[0].count = 0;
      monster.list[0].cycle = (base & 3) << 1;
      monster.list[0].image = (base & 4) << 1;
      monster.list[0].type = SPRITE_PLAYER_DEAD + (base << 1);
      timer_set((unsigned long)0, TIMING_PAUSE);
    }
    /*}}}*/
  if(global.count && global.count < 16)
    /*{{{  skip?*/
    if(global.throw == 1)
      {
	global.throw = 2;
	global.count = 16;
	monster.list[0].count = DIE_DELAY;
      }
    /*}}}*/
  if(monster.list[0].count++ < DIE_DELAY)
    age_scores();
  else if(global.count == 16)
    /*{{{  finish*/
    {
      MONSTER   *mptr;
      unsigned  ix;
      
      for(mptr = monster.list, ix = monster.monsters; ix--; mptr++)
	add_background(mptr->pixel.x, mptr->pixel.y,
	  CELL_WIDTH, CELL_HEIGHT);
      monster.monsters = 0;
      global.lives--;
      if(global.lives)
	{
	  XFillRectangle(display.display, display.back, GCN(GC_CLEAR),
	      PIXELX((int)global.lives - 1, 0), PIXELY(CELLS_DOWN, 0),
	      CELL_WIDTH, CELL_HEIGHT);
	  add_background(PIXELX((int)global.lives - 1, 0),
	      PIXELY(CELLS_DOWN, 0), CELL_WIDTH, CELL_HEIGHT);
	}
      if(extra.escape)
	{
	  extra.escape = 0;
	  draw_extra();
	}
      global.count = 17;
    }
    /*}}}*/
  else if(global.count == 17)
    /*{{{  end*/
    {
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      age_scores();
      monster.list[0].count = 0;
      global.count++;
      monster.list[0].type =
	  SPRITE_PLAYER_DEAD + monster.list[0].image +
	  ((monster.list[0].cycle + global.count) & 7);
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_pause(next)*/
static FUNCANIMATE(animate_pause, next)
/*
 * let everybody go to sleep until they're woken up.
 * Don't forget to put a flying carpet under the player, so
 * it doesn't have to float in mid air. (No one has yet observed
 * yogic flying, and they're not going to start now.
 */
{
  static PROTOANIMATE((*rts));
  static int face;
  static MONSTER *seat;
  PROTOVOID(*then);
  
  then = (PROTOVOID(*))animate_pause;
  global.pause = 0;
  if(next)
    /*{{{  start*/
    {
      char      buffer[65];
      size_t    length;
      CELL      *cptr;
      int       depth;
      
      rts = (PROTOANIMATE((*)))next;
      sprintf(buffer, "%s to continue, %s to quit",
	  XKeysymToString(data.keysymbols[KEY_THROW]),
	  XKeysymToString(data.keysymbols[KEY_QUIT]));
      length = strlen(buffer);
      assert(length < sizeof(buffer));
      XFillRectangle(display.display, display.copy, GCN(GC_CLEAR),
	  BORDER_LEFT + 1, PIXELY(CELLS_DOWN, 0),
	  BOARD_WIDTH - 2, CELL_HEIGHT);
      XDrawImageString(display.display, display.copy, GCN(GC_TEXT),
	  CENTER_X - (int)(length * font.width / 2),
	  PIXELY(CELLS_DOWN, CELL_HEIGHT / 2) + font.center,
	  buffer, length);
      XCopyArea(display.display, display.copy, display.window, GCN(GC_COPY),
	  BORDER_LEFT + 1, PIXELY(CELLS_DOWN, 0),
	  BOARD_WIDTH - 2, CELL_HEIGHT,
	  BORDER_LEFT + 1, PIXELY(CELLS_DOWN, 0));
      face = monster.list[0].face;
      monster.list[0].cycle = MONSTER_CYCLES * 3;
      cptr = BOARDCELL(monster.list[0].cell.x, monster.list[0].cell.y);
      depth = cptr->depths[1];
      if(monster.list[0].offset.x < 0)
	{
	  if(depth < cptr[-1].depths[1])
	    depth = cptr[-1].depths[1];
	}
      else if(monster.list[0].offset.x > 0)
	{
	  if(depth < cptr[1].depths[1])
	    depth = cptr[1].depths[1];
	}
      if(!monster.list[0].shot && monster.list[0].offset.y < depth)
	seat = spawn_monster(SPRITE_SEAT, 0, 0,
	    monster.list[0].cell.x, monster.list[0].cell.y,
	    monster.list[0].offset.x, monster.list[0].offset.y + CELL_HEIGHT);
      else
	seat = NULL;
      timer_set((unsigned long)0, TIMING_PAUSE);
    }
    /*}}}*/
  if(global.pressed & (1 << KEY_QUIT) || global.throw == 1)
    /*{{{  end*/
    {
      MONSTER   *mptr;
      unsigned  count;
      
      global.throw = 2;
      if(global.pressed & (1 << KEY_QUIT))
	{
	  global.lives = 1;
	  global.count = 1;
	  monster.list[0].shot = 1;
	}
      global.pressed &= ~(1 << KEY_QUIT);
      for(mptr = monster.list, count = monster.monsters; count--; mptr++)
	mptr->cycle %= MONSTER_CYCLES;
      if(seat)
	seat->type = 5;
      monster.list[0].face = face;
      add_background(BORDER_LEFT + 1, PIXELY(CELLS_DOWN, 0),
	  BOARD_WIDTH - 2, CELL_HEIGHT);
      timer_set((unsigned long)0, TIMING_ON);
      assert(rts != (PROTOANIMATE((*)))NULL);
      then = (*rts)((PROTOVOID(*))NULL);
    }
    /*}}}*/
  else
    /*{{{  animate*/
    {
      MONSTER   *mptr;
      unsigned  count;
      
      age_scores();
      if(player.ball.state == 2 || player.ball.state == 4)
	{
	  bounce_ball();
	  if(!player.ball.state)
	    player.ball.count = 8;
	}
      for(mptr = monster.list, count = monster.monsters; count--; mptr++)
	if(mptr->face < 16)
	  {
	    if(!mptr->cycle)
	      {
		if(mptr == monster.list && mptr->face < 8)
		  {
		    mptr->face = 8 | ((mptr->face & 1) ^
		      (mptr->face == 1 || mptr->face == 4));
		    if(!player.ball.state)
		      player.ball.count = 8;
		  }
		mptr->cycle = MONSTER_CYCLES * 4;
		mptr->image++;
		if(mptr->image == (mptr->type != 6 ?
		    MONSTER_IMAGES : DIAMOND_IMAGES))
		  mptr->image = 0;
	      }
	    mptr->cycle--;
	  }
      if(global.whizz)
	set_colors(-1);
    }
    /*}}}*/
  return then;
}
/*}}}*/
/*{{{  ANIMATE animate_game(next)*/
extern FUNCANIMATE(animate_game, next)
/*
 * this is one of the main control loops. It
 * plays a game until the player dies fatally (as opposed to
 * dying non-fatally and being resurected.
 */
{
  static int state = 0;
  PROTOVOID(*then);
  
  assert(!next);
  if(state == 1 && global.pause)
    {
      global.pause = 0;
      then = animate_pause((PROTOVOID(*))animate_game);
    }
  else
    {
      for(then = (PROTOVOID(*))NULL; then == (PROTOVOID(*))NULL;)
	switch(state)
	{
	  /*{{{  case 0:*/
	  case 0:
	  {
	    extra.got = 0;
	    extra.select = 0;
	    extra.escape = 0;
	    extra.score = 0;
	    extra.count = FRAMES_PER_SECOND;
	    global.diamond = 0;
	    create_xtra_monster(0);
	    global.lives = START_LIVES;
	    global.score = 0;
	    global.screen = 0;
	    history.msec = 0;
	    state = 4;
	    break;
	  }
	  /*}}}*/
	  /*{{{  case 1:*/
	  case 1:
	  {
	    if(global.count)
	      /*{{{  game frame*/
	      {
		then = (PROTOVOID(*))animate_game;
		age_scores();
		if(global.state != 4)
		  move_player();
		else if(!monster.list[0].shot)
		  {
		    if(!monster.list[0].cycle)
		      {
			monster.list[0].cycle = MONSTER_CYCLES;
			monster.list[0].image++;
			if(monster.list[0].image == MONSTER_IMAGES)
			  monster.list[0].image = 0;
		      }
		    monster.list[0].cycle--;
		  }
		if(global.broken || global.stepped)
		  calc_distances();
		if(global.broken && extra.escape == 2)
		  calc_extra_home();
		global.broken = global.stepped = 0;
		move_monsters();
		bounce_ball();
		move_apples();
		if(!global.state)
		  /*{{{  monster escape?*/
		  {
		    if(!monster.delay && chaotic() < DEN_ESCAPE_PROB)
		      monster.delay = DEN_ESCAPE_DELAY * DEN_ESCAPE_FLASH + 1;
		    if(monster.delay)
		      {
			monster.delay--;
			if(!(monster.delay % DEN_ESCAPE_FLASH))
			  draw_center((unsigned)(monster.delay /
			      DEN_ESCAPE_FLASH & 1 ?
			      SPRITE_NORMAL + 6 : SPRITE_DEN));
			if(!monster.delay)
			  {
			    spawn_monster(0, 3, 3, DEN_X, DEN_Y, 0, 0);
			    monster.den--;
			    if(!monster.den)
			      {
				global.state = 1;
				draw_center(SPRITE_PRIZE_BASE +
				  (global.screen - 1) % SPRITE_PRIZES);
			      }
			  }
		      }
		  }
		  /*}}}*/
		else if(global.state == 2 &&
		    !monster.den && !monster.drones)
		  {
		    if(!global.whizz)
		      set_colors(1);
		    global.state = 3;
		  }
		if(global.state != 4)
		  {
		    killed_player();
		    if(!global.cherries || !monster.normals ||
			monster.list[0].shot || extra.got == 0x1F ||
			global.diamond == 2)
		      {
			timer_set((unsigned long)0, TIMING_PAUSE);
			/*{{{  happy?*/
			if(!monster.list[0].shot)
			  {
			    int       depth;
			    CELL      *cptr;
			    
			    monster.list[0].face = 10;
			    cptr = BOARDCELL(monster.list[0].cell.x,
				monster.list[0].cell.y);
			    depth = cptr->depths[1];
			    if(monster.list[0].offset.x < 0)
			      {
				if(depth < cptr[-1].depths[1])
				  depth = cptr[-1].depths[1];
			      }
			    else if(monster.list[0].offset.x > 0)
			      {
				if(depth < cptr[1].depths[1])
				  depth = cptr[1].depths[1];
			      }
			    if(monster.list[0].offset.y < depth)
			      spawn_monster(SPRITE_SEAT, 0, 0,
				  monster.list[0].cell.x,
				  monster.list[0].cell.y,
				  monster.list[0].offset.x,
				  monster.list[0].offset.y + CELL_HEIGHT);
			  }
			/*}}}*/
			global.state = 4;
			if(global.whizz)
			  global.whizz = 2;
		      }
		  }
		else
		  /*{{{  ending*/
		  {
		    if(global.throw == 1)
		      global.count = 1;
		    else if(update.score.scores || player.ball.state > (global.count > 1))
		      global.count++;
		    else
		      {
			APPLE   *aptr;
			int     i;
		  
			for(aptr = apple.list, i = apple.apples; i--; aptr++)
			  if(aptr->state)
			    {
			      global.count++;
			      break;
			    }
		      }
		    global.count--;
		    if(player.ball.state == 1 &&
			global.count < SCORE_SHOW / 2)
		      {
			player.ball.state = 2;
			player.ball.count = 0;
		      }
		    else if(player.ball.state == 3)
		      player.ball.state = 4;
		  }
		  /*}}}*/
		/*{{{  extra stuff*/
		if(!extra.escape)
		  {
		    unsigned  temp;
		
		    if(!extra.count--)
		      new_xtra();
		    temp = global.score / 5000;
		    if(global.state != 4 && temp != extra.score)
		      {
			extra.score = temp;
			extra_escape();
			extra.count = XTRA_HOME_DELAY;
		      }
		  }
		else
		  {
		    extra.score = global.score / 5000;
		    if(global.state != 2 && !extra.count--)
		      {
			extra.escape = 2;
			if(!global.broken)
			  calc_extra_home();
		      }
		  }
		/*}}}*/
		if(global.whizz)
		  set_colors(-1);
	      }
	      /*}}}*/
	    else if(monster.list[0].shot)
	      {
		state = 2;
		then = animate_death((PROTOVOID(*))animate_game);
	      }
	    else
	      {
		timer_set((unsigned long)0, TIMING_OFF);
		history.msec += global.msec;
		history.times[0] = global.msec;
		global.msec = 0;
		if(global.diamond == 2)
		  {
		    global.diamond = 3;
		    history.ending |= 3;
		    state = 3;
		    then = animate_diamond((PROTOVOID(*))animate_game);
		  }
		else if(extra.got == 0x1F)
		  /*{{{  extra life*/
		  {
		    history.ending |= 2;
		    state = 3;
		    then = animate_extra_life((PROTOVOID(*))animate_game);
		  }
		  /*}}}*/
		else if(!monster.normals)
		  {
		    history.ending |= 1;
		    state = 3;
		  }
		else
		  state = 3;
	      }
	    break;
	  }
	  /*}}}*/
	  /*{{{  case 2:*/
	  case 2:
	  {
	    global.state = 0;
	    global.whizz = 0;
	    if(!global.lives)
	      {
		state = 0;
		timer_set((unsigned long)0, TIMING_OFF);
		history.msec += global.msec;
		high_score(global.score, global.screen, history.msec);
		then = animate_demo((PROTOVOID(*))NULL);
	      }
	    else
	      {
		state = 1;
		timer_set(FRAME_RATE, TIMING_ON);
		global.count = SCORE_SHOW;
		if(global.diamond < 3)
		  global.diamond = 0;
		/*{{{  initialize stuff*/
		{
		  monster.monsters = 0;
		  monster.delay = 0;
		  monster.den = monster.normals;
		  monster.drones = 0;
		  spawn_monster(4, 3, 3,
		    PLAYER_START_X, PLAYER_START_Y, 0, 0);
		  monster.list[0].stop = 1;
		  monster.player = BOARDCELL(PLAYER_START_X, PLAYER_START_Y);
		  player.old_ball.state = 0;
		  player.old_ball.count = 8;
		  player.ball.state = 0;
		  player.thrown = 0;
		  player.bashed = 0;
		  player.pressed = 0;
		  player.next = 0;
		  bounce_ball();
		}
		/*}}}*/
		set_colors(1);
		calc_distances();
		if(monster.den)
		  draw_center(SPRITE_DEN);
	      }
	    break;
	  }
	  /*}}}*/
	  /*{{{  case 3:*/
	  case 3:
	  {
	    state = 4;
	    if(!(global.screen % HISTORY_SHOW))
	      then = animate_history((PROTOVOID(*))animate_game);
	    break;
	  }
	  /*}}}*/
	  /*{{{  case 4:*/
	  case 4:
	  {
	    unsigned  ix;
	    
	    global.state = 0;
	    state = 2;
	    global.screen++;
	    new_board();
	    global.difficulty = global.screen + DIFFICULTY_PEDESTAL;
	    monster.normals = global.screen >= 3 ?
		8 + global.screen / 10 * 2 : 6;
	    if(monster.normals > MONSTERS - 6)
	      monster.normals = MONSTERS - 6;
	    history.prize <<= 1;
	    history.ending <<= 2;
	    for(ix = CELLS_DOWN - 2; ix--;)
	      history.times[ix + 1] = history.times[ix];
	    then = animate_zoom((PROTOVOID(*))animate_game);
	    break;
	  }
	  /*}}}*/
	}
    }
  return then;
}
/*}}}*/
/*{{{  void calc_distances()*/
extern VOIDFUNC calc_distances FUNCARGVOID
/*
 * sets the distances from each cell to the player
 * this is so the monsters have non-local knowlegde
 * increment the non-zero cells
 * this proceeds as a sort of flood fill operation, starting
 * from the player's cell and moving outwards
 */
{
  CELL    **aptr, **sptr;
  CELL    *list[2][FLOOD_FILL];
  CELL    *cptr;
  int     toggle;
  int     x, y;
  int     count;
  int     visited;

  for(y = CELLS_DOWN; y--;)
    {
      cptr = BOARDCELL(0, y);
      for(x = CELLS_ACROSS; x--; cptr++)
	cptr->distance = cptr->visit ? 0 : 255;
    }
  toggle = 0;
  cptr = BOARDCELL(monster.list[0].cell.x, monster.list[0].cell.y);
  cptr->distance = count = 1;
  visited = 0;
  list[0][0] = cptr;
  list[0][1] = NULL;
  while(list[toggle][0])
  {
    sptr = list[toggle];
    toggle = !toggle;
    aptr = list[toggle];
    count++;
    visited++;
    while((cptr = *sptr++) != NULL)
      {
	CELL      *tptr;
	
	/*{{{  go up?*/
	if(cptr->depths[0])
	  {
	    tptr = cptr - CELL_STRIDE;
	    if(tptr->visit && !tptr->distance)
	      {
		tptr->distance = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go down?*/
	if(cptr->depths[1])
	  {
	    tptr = cptr + CELL_STRIDE;
	    if(tptr->visit && !tptr->distance)
	      {
		tptr->distance = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go left?*/
	if(cptr->depths[2])
	  {
	    tptr = cptr - 1;
	    if(tptr->visit && !tptr->distance)
	      {
		tptr->distance = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go right?*/
	if(cptr->depths[3])
	  {
	    tptr = cptr + 1;
	    if(tptr->visit && !tptr->distance)
	      {
		tptr->distance = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	assert(aptr - list[toggle] < FLOOD_FILL);
      }
    *aptr = NULL;
  }
  global.visited = visited;
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void calc_extra_home()*/
static VOIDFUNC calc_extra_home FUNCARGVOID
/*
 * sets the distances from each cell to the extra home row
 * this is so the monsters have non-local knowlegde
 * increment the non-zero cells
 * this proceeds as a sort of flood fill operation, starting
 * from the home row and moving outwards
 */
{
  CELL    **aptr, **sptr;
  CELL    *list[2][FLOOD_FILL];
  CELL    *cptr;
  int     toggle;
  int     x, y;
  int     count;

  for(y = CELLS_DOWN; y--;)
    {
      cptr = BOARDCELL(0, y);
      for(x = CELLS_ACROSS; x--; cptr++)
	cptr->xtra = cptr->visit ? 0 : 255;
    }
  toggle = 0;
  cptr = BOARDCELL(4, 0);
  cptr[0].xtra = cptr[1].xtra = cptr[2].xtra = cptr[3].xtra = count = 1;
  list[0][0] = cptr;
  list[0][1] = cptr + 1;
  list[0][2] = cptr + 2;
  list[0][3] = cptr + 3;
  list[0][4] = NULL;
  while(list[toggle][0])
  {
    sptr = list[toggle];
    toggle = !toggle;
    aptr = list[toggle];
    count++;
    while((cptr = *sptr++) != NULL)
      {
	CELL      *tptr;
	
	/*{{{  go up?*/
	if(cptr->depths[0])
	  {
	    tptr = cptr - CELL_STRIDE;
	    if(tptr->visit && !tptr->xtra)
	      {
		tptr->xtra = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go down?*/
	if(cptr->depths[1])
	  {
	    tptr = cptr + CELL_STRIDE;
	    if(tptr->visit && !tptr->xtra)
	      {
		tptr->xtra = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go left?*/
	if(cptr->depths[2])
	  {
	    tptr = cptr - 1;
	    if(tptr->visit && !tptr->xtra)
	      {
		tptr->xtra = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	/*{{{  go right?*/
	if(cptr->depths[3])
	  {
	    tptr = cptr + 1;
	    if(tptr->visit && !tptr->xtra)
	      {
		tptr->xtra = count;
		*aptr++ = tptr;
	      }
	  }
	/*}}}*/
	assert(aptr - list[toggle] < FLOOD_FILL);
      }
    *aptr = NULL;
  }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void fatal_error(text, ...)*/
extern VOIDFUNC fatal_error
FUNCVARARG((text, VARARG),
	char const *text
)
{
  va_list   args;
  
  VARARGSET(args, text);
  vfprintf(stderr, text, args);
  fputc('\n', stderr);
  destroy_widget();
  close_toolkit();
  exit(1);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  size_t itoa(text, n, width)*/
extern size_t itoa
FUNCARG((text, number, digits),
	char      *text    /* output text (include 0) */
ARGSEP  unsigned long number   /* number to convert */
ARGSEP  unsigned  digits   /* field width to convert into */
)
/*
 * formats an integer to a string
 * in the specified number of digits
 * pads leading zeros to ' '
 * returns the number of characters used
 */
{
  char      reverse[10];
  size_t    l, length;

  l = 0;
  do
    {
      reverse[l++] = number % 10 + '0';
      number /= 10;
    }
  while(number);
  if(!digits)
    length = 0;
  else if(l < digits)
    {
      length = digits - l;
      memset(text, ' ', length);
    }
  else
    {
      length = 0;
      l = digits;
    }
  while(l)
    text[length++] = reverse[--l];
  text[length] = 0;
  return length;
}
/*}}}*/
/*{{{  int main(argc, argv)*/
extern int main
FUNCARG((argc, argv),
	int     argc
ARGSEP  char    **argv
)
{
  PROTOANIMATE((*animate));
  
  real_uid = getuid();
  effective_uid = geteuid();
  if(real_uid != effective_uid)
    set_euid(real_uid);
  open_toolkit(argc, argv);
  init_scores();
  /*{{{  help?*/
  if(data.help)
    {
      char const *ptr;
	
      if(!*argv)
	ptr = "xmris";
      else
	{
	  ptr = *argv;
	  for(ptr += strlen(ptr) - 1; ptr != *argv; ptr--)
	    if(ptr[-1] == '/')
	      break;
	}
      list_help(ptr);
      close_toolkit();
      return 0;
    }
  /*}}}*/
  /*{{{  high scores?*/
  if(data.scores)
    {
      list_scores();
      close_toolkit();
      return 0;
    }
  /*}}}*/
  XSetIOErrorHandler(xio_error);
  create_widget();
  XMapWindow(display.display, display.window);
  timer_open();
  animate = animate_demo;
  while(!global.quit)
    {
      process_xevents();
      if(global.pressed & (1 << KEY_THROW))
	{
	  if(global.throw == 0)
	    global.throw = 1;
	}
      else
	global.throw = 0;
      if(global.pressed & 1 << KEY_ICONIZE)
	{
	  XIconifyWindow(display.display, XtWindow(display.toplevel),
	    display.screen);
	  global.pressed ^= 1 << KEY_ICONIZE;
	  XSync(display.display, False);
	}
      else
	{
	  assert(animate != (PROTOANIMATE((*)))NULL);
	  animate = (PROTOANIMATE((*)))(*animate)((PROTOVOID(*))NULL);
	  show_updates();
	  timer_wait();
	}
    }
  timer_close();
  destroy_widget();
  close_toolkit();
  return 0;
}
/*}}}*/
/*{{{  void process_xevents()*/
static VOIDFUNC process_xevents FUNCARGVOID
/*
 * this reads X events from the window's queue until there are no
 * more left. Called once per frame.
 * Key presses have to be handled carefully in game mode, because
 * we might get both a keypress and key release in the same call,
 * and thus not show that the key is pressed. Also auto repeat tends
 * to bugger things up, I've put in an XSync whenever we get a key release
 * so that if it's auto repeat, the key press should get here before we
 * quit out of the loop. You could always turn of auto repeat if
 * it's still a problem, but you can't do that on a per application
 * basis, so it'd be rude to do it auto matically.
 */
{
  static    int outside = 1;
  static    int unpress = 0;
  static    int iconic = 1;
  int       pressed;
  int       unpause;

  XSync(display.display, False);
  pressed = global.pressed &= ~unpress;
  unpress = 0;
  unpause = 0;
  /*{{{  read the events*/
  while(QLength(display.display) || iconic ||
      (global.pressed & ~unpress) & 1 << KEY_PAUSE ||
      (!global.key && global.state == MODE_KEY_DEF))
    {
      XEvent    event;
    
      XNextEvent(display.display, &event);
      if(event.xany.window == display.window)
	switch(event.type)
	{
	  /*{{{  case KeyPress:*/
	  case KeyPress:
	  /*
	   * When a key is pressed, we check to see if it is
	   * a control key. If so, then we set the relevant pressed bit.
	   */
	  {
	    KeySym    symbol;
	    char      chr;
	    XComposeStatus  status;
	    unsigned  ix;
	  
	    XLookupString(&event.xkey, &chr, 1, &symbol, &status);
	    global.key = symbol;
	    if(global.state != MODE_KEY_DEF)
	      for(ix = KEYS; ix--;)
		if(symbol == data.keysymbols[ix])
		  {
		    unsigned  bit;
	  
		    bit = 1 << ix;
		    global.pressed |= bit;
		    unpress &= ~bit;
		    if(ix == KEY_PAUSE)
		      timer_set((unsigned long)0, TIMING_PAUSE);
		    break;
		  }
	    break;
	  }
	  /*}}}*/
	  /*{{{  case KeyRelease:*/
	  case KeyRelease:
	  /*
	   * when a key is released, we check to see if it is
	   * the controlling direction key. If so, then we stop
	   */
	  {
	    char      chr;
	    KeySym    symbol;
	    XComposeStatus status;
	    unsigned  ix;
	  
	    XLookupString(&event.xkey, &chr, 1, &symbol, &status);
	    for(ix = KEYS; ix--;)
	      if(symbol == data.keysymbols[ix])
		{
		  unsigned  bit;
	  
		  bit = 1 << ix;
		  if(pressed & bit)
		    global.pressed &= ~bit;
		  else
		    unpress |= bit;
		  XSync(display.display, False);
		}
	    break;
	  }
	  /*}}}*/
	  /*{{{  case Expose:*/
	  case Expose:
	    if(!iconic)
	      refresh_window();
	    break;
	  /*}}}*/
	  /*{{{  case EnterNotify:*/
	  case EnterNotify:
	    if(!iconic)
	      XSetInputFocus(display.display, display.window,
		  RevertToParent, event.xcrossing.time);
	    global.pressed = unpress = 0;
	    outside = 0;
	    break;
	  /*}}}*/
	  /*{{{  case LeaveNotify:*/
	  case LeaveNotify:
	    outside = 1;
	    break;
	  /*}}}*/
	}
      else
	switch(event.type)
	{
	  /*{{{  case UnmapNotify:*/
	  case UnmapNotify:
	    unpause = timer_set((unsigned long)0, TIMING_PAUSE) == TIMING_ON;
	    iconic = 1;
	    break;
	  /*}}}*/
	  /*{{{  case MapNotify:*/
	  case MapNotify:
	    iconic = 0;
	    if(!outside)
	      XSetInputFocus(display.display, display.window,
		  RevertToParent, event.xcrossing.time);
	    break;
	  /*}}}*/
	  /*{{{  case PropertyNotify:*/
	  case PropertyNotify:
	    if(event.xproperty.atom == display.DEC_icon_atom)
	      {
		unpause = timer_set((unsigned long)0, TIMING_PAUSE) == TIMING_ON;
		iconic = 1;
	      }
	    break;
	  /*}}}*/
	}
    }
  /*}}}*/
  if((outside && global.state < 5) || global.pressed & 1 << KEY_PAUSE)
    global.pause = 1;
  else if(unpause)
    timer_set((unsigned long)0, TIMING_ON);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  unsigned chaotic()*/
extern unsigned chaotic FUNCARGVOID
/*
 * a simple pseudo random number generator
 * it generates 8 new bits of number at each call
 * using a 31 bit maximal length linear feedback shift register
 * the taps are bits 0 and 3
 */
{
  unsigned  bits;

  if(!seed)
    seed = time((time_t *)NULL);
  bits = ((seed >> 3) ^ seed) & 0xFF;
  seed = (seed >> 8) | (bits << 23);
  return bits;
}
/*}}}*/
/*{{{  int xio_error(dptr)*/
static int xio_error
/* ARGSUSED */
FUNCARG((dptr),
	Display *dptr
)
{
  static before = 0;
  
  if(!before)
    {
      before = 1;
      destroy_widget();
      close_toolkit();
    }
  exit(0);
  return 0;
}
/*}}}*/
